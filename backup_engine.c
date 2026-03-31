#include "smart_copy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <zlib.h>  // Para compresión (instala libz-dev si no tienes)

// Función auxiliar para comprimir datos (usando zlib)
int compress_data(const char *input, size_t input_len, char **output, size_t *output_len) {
    uLongf compressed_len = compressBound(input_len);
    *output = malloc(compressed_len);
    if (!*output) return 0;
    if (compress((Bytef *)*output, &compressed_len, (const Bytef *)input, input_len) != Z_OK) {
        free(*output);
        return 0;
    }
    *output_len = compressed_len;
    return 1;
}

// Función auxiliar para descomprimir
int decompress_data(const char *input, size_t input_len, char **output, size_t *output_len) {
    uLongf decompressed_len = input_len * 4;  // Estimación
    *output = malloc(decompressed_len);
    if (!*output) return 0;
    if (uncompress((Bytef *)*output, &decompressed_len, (const Bytef *)input, input_len) != Z_OK) {
        free(*output);
        return 0;
    }
    *output_len = decompressed_len;
    return 1;
}

// Función auxiliar para versioning: genera nombre con _vN
void generate_versioned_name(const char *dest, char *versioned_dest, size_t max_len) {
    int version = 1;
    while (version < 100) {  // Límite arbitrario
        snprintf(versioned_dest, max_len, "%s_v%d", dest, version);
        if (access(versioned_dest, F_OK) == -1) break;  // Si no existe, úsalo
        version++;
    }
}

// Implementación de smart_copy (pseudokernel: system calls optimizados)
CopyResult smart_copy(const char *src, const char *dest, int flags) {
    CopyResult result = {0, 0, 0.0, ""};
    clock_t start = clock();

    // Validar permisos (simulando kernel)
    struct stat st;
    if (stat(src, &st) == -1) {
        snprintf(result.log_message, sizeof(result.log_message), "Error: No se puede acceder a %s", src);
        return result;
    }
    if (!(st.st_mode & S_IRUSR)) {
        snprintf(result.log_message, sizeof(result.log_message), "Error: Permisos insuficientes para leer %s", src);
        return result;
    }

    // Manejar versioning
    char final_dest[1024];
    if (flags & FLAG_VERSION) {
        generate_versioned_name(dest, final_dest, sizeof(final_dest));
    } else {
        strcpy(final_dest, dest);
    }

    // Abrir archivos
    int fd_src = open(src, O_RDONLY);
    if (fd_src < 0) {
        snprintf(result.log_message, sizeof(result.log_message), "Error al abrir origen: %s", strerror(errno));
        return result;
    }
    int fd_dest = open(final_dest, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode);
    if (fd_dest < 0) {
        close(fd_src);
        snprintf(result.log_message, sizeof(result.log_message), "Error al crear destino: %s", strerror(errno));
        return result;
    }

    // Copiar con buffer
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    size_t total_bytes = 0;
    while ((bytes_read = read(fd_src, buffer, BUFFER_SIZE)) > 0) {
        if (flags & FLAG_COMPRESS) {
            // Comprimir antes de escribir
            char *compressed;
            size_t comp_len;
            if (!compress_data(buffer, bytes_read, &compressed, &comp_len)) {
                snprintf(result.log_message, sizeof(result.log_message), "Error en compresión");
                close(fd_src);
                close(fd_dest);
                return result;
            }
            write(fd_dest, compressed, comp_len);
            free(compressed);
            total_bytes += comp_len;
        } else {
            write(fd_dest, buffer, bytes_read);
            total_bytes += bytes_read;
        }
    }

    close(fd_src);
    close(fd_dest);

    clock_t end = clock();
    result.success = 1;
    result.bytes_copied = total_bytes;
    result.time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    snprintf(result.log_message, sizeof(result.log_message), "Copia exitosa en modo kernel (flags: %d)", flags);

    return result;
}

// Implementación de stdio_copy (comparación con stdio.h)
CopyResult stdio_copy(const char *src, const char *dest) {
    CopyResult result = {0, 0, 0.0, ""};
    clock_t start = clock();

    FILE *fsrc = fopen(src, "rb");
    if (!fsrc) {
        snprintf(result.log_message, sizeof(result.log_message), "Error al abrir origen con stdio");
        return result;
    }
    FILE *fdest = fopen(dest, "wb");
    if (!fdest) {
        fclose(fsrc);
        snprintf(result.log_message, sizeof(result.log_message), "Error al crear destino con stdio");
        return result;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t total_bytes = 0;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fsrc)) > 0) {
        fwrite(buffer, 1, bytes_read, fdest);
        total_bytes += bytes_read;
    }

    fclose(fsrc);
    fclose(fdest);

    clock_t end = clock();
    result.success = 1;
    result.bytes_copied = total_bytes;
    result.time_taken = (double)(end - start) / CLOCKS_PER_SEC;
    strcpy(result.log_message, "Copia exitosa con stdio.h");

    return result;
}
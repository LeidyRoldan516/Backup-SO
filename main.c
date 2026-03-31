#include "smart_copy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <libgen.h>

void print_usage(const char *prog_name) {
    printf("Uso: %s [opciones] <origen> <destino>\n", prog_name);
    printf("Opciones:\n");
    printf("  --compress    Activar compresión\n");
    printf("  --version     Activar versioning\n");
    printf("  --help        Mostrar esta ayuda\n");
    printf("Ejemplo: %s --compress --version archivo.txt backup.txt\n", prog_name);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Parsear flags
    int flags = 0;
    int arg_index = 1;
    for (; arg_index < argc - 2; arg_index++) {
        if (strcmp(argv[arg_index], "--compress") == 0) {
            flags |= FLAG_COMPRESS;
        } else if (strcmp(argv[arg_index], "--version") == 0) {
            flags |= FLAG_VERSION;
        } else if (strcmp(argv[arg_index], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Opción desconocida: %s\n", argv[arg_index]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (arg_index + 1 >= argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *src = argv[arg_index];
    const char *dest = argv[arg_index + 1];

    const char *out_dir = "backup_out";
    if (mkdir(out_dir, 0755) == -1 && errno != EEXIST) {
        perror("Error creando directorio de salida");
        return EXIT_FAILURE;
    }

    char dest_basename[1024];
    strncpy(dest_basename, dest, sizeof(dest_basename) - 1);
    dest_basename[sizeof(dest_basename) - 1] = '\0';
    char *dest_baseptr = basename(dest_basename);

    char smart_dest[1024];
    char stdio_dest[1024];
    snprintf(smart_dest, sizeof(smart_dest), "%s/%s", out_dir, dest_baseptr);
    snprintf(stdio_dest, sizeof(stdio_dest), "%s/%s_stdio", out_dir, dest_baseptr);

    printf("=== SMART BACKUP KERNEL-SPACE UTILITY ===\n");
    printf("Origen: %s\n", src);
    printf("Destino: %s\n", smart_dest);
    printf("Flags: %s%s\n", (flags & FLAG_COMPRESS) ? "Compresión " : "", (flags & FLAG_VERSION) ? "Versioning" : "Ninguno");
    printf("\n");

    // Prueba 1: Smart Copy (pseudokernel)
    printf("--- Prueba 1: Smart Copy (Pseudokernel) ---\n");
    CopyResult smart_result = smart_copy(src, smart_dest, flags);
    if (smart_result.success) {
        printf("Éxito: %zu bytes copiados en %.6f segundos\n", smart_result.bytes_copied, smart_result.time_taken);
    } else {
        printf("Error: %s\n", smart_result.log_message);
    }
    printf("Log: %s\n\n", smart_result.log_message);

    // Prueba 2: Stdio Copy (comparación)
    printf("--- Prueba 2: Stdio Copy (Estándar) ---\n");
    CopyResult stdio_result = stdio_copy(src, stdio_dest);
    if (stdio_result.success) {
        printf("Éxito: %zu bytes copiados en %.6f segundos\n", stdio_result.bytes_copied, stdio_result.time_taken);
    } else {
        printf("Error: %s\n", stdio_result.log_message);
    }
    printf("Log: %s\n\n", stdio_result.log_message);

    // Análisis de performance
    printf("--- Análisis de Performance ---\n");
    if (smart_result.success && stdio_result.success) {
        double speedup = stdio_result.time_taken / smart_result.time_taken;
        printf("Smart Copy es %.2fx más rápido que Stdio Copy\n", speedup);
        printf("Diferencia de tiempo: %.6f segundos\n", stdio_result.time_taken - smart_result.time_taken);
    } else {
        printf("No se pudo comparar (uno falló)\n");
    }

    return EXIT_SUCCESS;
}
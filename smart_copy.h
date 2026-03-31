#ifndef SMART_COPY_H
#define SMART_COPY_H

#include <time.h>  // Para medición de tiempos

// Constantes de flags para opciones creativas
#define FLAG_COMPRESS 1    // Activar compresión
#define FLAG_VERSION  2    // Activar versioning
#define BUFFER_SIZE 4096   // Tamaño de buffer 

// Estructura para resultados de copia (para logging y análisis)
typedef struct {
    int success;           // 1 si exitoso, 0 si falló
    size_t bytes_copied;   // Bytes copiados
    double time_taken;     // Tiempo en segundos
    char log_message[256]; // Mensaje de log (simulando kernel logs)
} CopyResult;

// Firma de la función "de sistema" optimizada (pseudokernel)
CopyResult smart_copy(const char *src, const char *dest, int flags);

// Firma para comparación: copia usando stdio.h (estándar)
CopyResult stdio_copy(const char *src, const char *dest);

#endif // SMART_COPY_H

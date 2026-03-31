# Smart Backup Kernel-Space Utility

## Descripción del Proyecto

Sistema inteligente de respaldo de archivos que simula una función a nivel de kernel (pseudosistema) y la compara con una implementación estándar. El proyecto demuestra cómo las llamadas al sistema (system calls) optimizan el rendimiento frente a librerías de alto nivel.

### Capas del Proyecto
- **Capa de Kernel (Simulada)**: Función `smart_copy()` que usa system calls (`open`, `read`, `write`, `mkdir`) con buffer de 4096 bytes, validación de permisos y logging.
- **Capa de Usuario**: Función `stdio_copy()` con `fopen`, `fread`, `fwrite` para comparación de rendimiento.

---

## Funcionalidades Creativas Implementadas

### 1. **Compresión con zlib** (`--compress`)
- Comprime archivos durante la copia usando la librería zlib.
- Reduce el tamaño de almacenamiento (hasta 56%).
- Ejemplo: Archivo de 220 bytes → 96 bytes comprimidos.

### 2. **Versioning** (`--version`)
- Crea versiones automáticas si el archivo ya existe.
- Evita sobrescribir backups anteriores.
- Ejemplo: `archivo.txt` → `archivo_v1.txt`, `archivo_v2.txt`, etc.

---

## Requisitos

- GCC (compilador C)
- zlib-dev (`sudo apt install libz-dev`)
- Linux/WSL

---

## Archivos principales

```
smart_copy.h          - Definiciones, constantes y firmas de funciones
backup_engine.c       - Lógica central (smart_copy y stdio_copy)
main.c                - Interfaz de usuario y procesamiento de argumentos
README.md             - Este archivo
REPORTE_PROYECTO.txt  - Análisis detallado de performance (convertir a PDF)
backup_out/           - Carpeta con resultados de pruebas
```

---

## Compilación

```bash
gcc main.c backup_engine.c -lz -o smart_backup
```

**Explicación:**
- `gcc`: Compilador GNU C
- `main.c backup_engine.c`: Archivos fuente a compilar
- `-lz`: Linkear librería zlib
- `-o smart_backup`: Nombre del ejecutable generado

---

## Uso

### Sintaxis básica:
```bash
./smart_backup [opciones] <archivo_origen> <nombre_destino>
```

### Opciones:
```
--compress    Activar compresión (reduce tamaño)
--version     Activar versioning (evita sobrescribir)
--help        Mostrar esta ayuda
```

### Ejemplos:

**1. Copia básica (sin opciones):**
```bash
./smart_backup archivo.txt backup.txt
```
Resultado: `backup_out/backup.txt` y `backup_out/backup.txt_stdio`

**2. Copia con compresión:**
```bash
./smart_backup --compress archivo.txt backup.txt
```
Resultado: Archivo más pequeño en `backup_out/backup.txt_v1`

**3. Copia con ambas opciones:**
```bash
./smart_backup --compress --version archivo.txt backup.txt
```
Resultado: Archivo comprimido y versionado en `backup_out/backup.txt_v1`

**4. Ver ayuda:**
```bash
./smart_backup --help
```

---

## Salida del Programa

Cada ejecución imprime:

```
=== SMART BACKUP KERNEL-SPACE UTILITY ===
Origen: archivo.txt
Destino: backup_out/backup.txt
Flags: Compresión Versioning

--- Prueba 1: Smart Copy (Pseudokernel) ---
Éxito: 145 bytes copiados en 0.003270 segundos
Log: Copia exitosa en modo kernel (flags: 3)

--- Prueba 2: Stdio Copy (Estándar) ---
Éxito: 220 bytes copiados en 0.001722 segundos
Log: Copia exitosa con stdio.h

--- Análisis de Performance ---
Smart Copy es 0.53x más rápido que Stdio Copy
Diferencia de tiempo: -0.001548 segundos

Archivos generados en: backup_out
```

## Notas Técnicas

### ¿Por qué es "Función de Sistema"?

1. **System Calls**: Usa `open()`, `read()`, `write()` (llamadas directas al kernel).
2. **Buffer Optimizado**: 4096 bytes (tamaño de página del sistema).
3. **Validación de Permisos**: Verifica acceso con `stat()` antes de copiar.
4. **Logging**: Registra operaciones como un SO lo haría.
5. **Performance**: Supera a librerías de alto nivel en archivos grandes.

### Diferencia con Stdio.h

| Aspecto | Smart Copy | Stdio Copy |
|---------|-----------|-----------|
| **Función** | System calls (`open`, `read`, `write`) | Librerías `fopen`, `fread`, `fwrite` |
| **Buffer** | 4096 bytes (página) | Variable (internamente buffered) |
| **Permisos** | Valida con `stat()` | No valida antes |
| **Compresión** | Soporta zlib | No soporta |
| **Nivel** | Bajo (kernel) | Alto (C library) |

---

## Autor

Proyecto de "Smart Backup Kernel-Space Utility" para Sistemas Operativos - EAFIT 
Leidy Dayhana Roldan Osorio

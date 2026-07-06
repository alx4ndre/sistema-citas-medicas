#include <stdio.h>

/* ==================== 1. Estructura y constantes ==================== */
#define MAX_CITAS 100

#define LEN_CODIGO 16
#define LEN_NOMBRE 41
#define LEN_ESPECIALIDAD 31
#define LEN_FECHA 11
#define LEN_HORA 6
#define LEN_MEDICO 41
#define LEN_LINEA 256

#define ARCHIVO_CITAS "citas.csv"

/* Estructura que representa una cita medica. */
typedef struct {
    char codigo_cita[LEN_CODIGO];
    char nombre_paciente[LEN_NOMBRE];
    char especialidad[LEN_ESPECIALIDAD];
    char fecha[LEN_FECHA];
    char hora[LEN_HORA];
    char medico[LEN_MEDICO];
} Cita;
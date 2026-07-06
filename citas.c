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

/* ==================== 2. Utilidades y validaciones ==================== */
/* Responsable: Mateo Mero */

/* Calcula la longitud de una cadena recorriendola con un indice. */
int longitud(const char *cadena) {
    int n = 0;
    while (cadena[n] != '\0') {
        n++;
    }
    return n;
}

/* Copia origen en destino sin exceder tam-1 caracteres. */
void copiar(char *destino, const char *origen, int tam) {
    int i = 0;
    while (origen[i] != '\0' && i < tam - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

/* Devuelve 1 si dos cadenas son iguales, 0 en caso contrario. */
int iguales(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

/* Devuelve 1 si el caracter es un digito del 0 al 9. */
int esDigito(char c) {
    return (c >= '0' && c <= '9');
}

/* Devuelve 1 si patron aparece como subcadena dentro de texto. */
int contieneSubcadena(const char *texto, const char *patron) {
    int i, j;
    if (patron[0] == '\0') return 1;
    for (i = 0; texto[i] != '\0'; i++) {
        j = 0;
        while (patron[j] != '\0' && texto[i + j] == patron[j]) {
            j++;
        }
        if (patron[j] == '\0') return 1;
    }
    return 0;
}

/* Elimina espacios, tabuladores y saltos de linea al final de la cadena. */
void limpiarEspacios(char *cadena) {
    int len = longitud(cadena);
    while (len > 0) {
        char c = cadena[len - 1];
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            cadena[len - 1] = '\0';
            len--;
        } else {
            break;
        }
    }
}

/* Codigo: 1 a 15 caracteres, alfanumerico, sin espacios. */
int validarFormatoCodigo(const char *codigo) {
    int len = longitud(codigo);
    int i;
    if (len < 1 || len > 15) return 0;
    for (i = 0; i < len; i++) {
        char c = codigo[i];
        int esLetra = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        if (!esLetra && !esDigito(c)) return 0;
    }
    return 1;
}

/* Fecha: formato DD/MM/AAAA (10 caracteres). */
int validarFormatoFecha(const char *fecha) {
    int i, dia, mes;
    if (longitud(fecha) != 10) return 0;
    if (fecha[2] != '/' || fecha[5] != '/') return 0;
    for (i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!esDigito(fecha[i])) return 0;
    }
    dia = (fecha[0] - '0') * 10 + (fecha[1] - '0');
    mes = (fecha[3] - '0') * 10 + (fecha[4] - '0');
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;
    return 1;
}

/* Hora: formato HH:MM (5 caracteres, 24 horas). */
int validarFormatoHora(const char *hora) {
    int i, horas, minutos;
    if (longitud(hora) != 5) return 0;
    if (hora[2] != ':') return 0;
    for (i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!esDigito(hora[i])) return 0;
    }
    horas = (hora[0] - '0') * 10 + (hora[1] - '0');
    minutos = (hora[3] - '0') * 10 + (hora[4] - '0');
    if (horas < 0 || horas > 23) return 0;
    if (minutos < 0 || minutos > 59) return 0;
    return 1;
}

/* Devuelve 1 si el codigo ya existe en el arreglo de citas. */
int existeCodigo(const Cita citas[], int total, const char *codigo) {
    int i;
    for (i = 0; i < total; i++) {
        if (iguales(citas[i].codigo_cita, codigo)) return 1;
    }
    return 0;
}

/* Devuelve 1 si ya hay una cita en esa fecha y hora (excluyendo un codigo). */
int horarioOcupado(const Cita citas[], int total, const char *fecha,
                   const char *hora, const char *codigo_excluir) {
    int i;
    for (i = 0; i < total; i++) {
        if (iguales(citas[i].codigo_cita, codigo_excluir)) continue;
        if (iguales(citas[i].fecha, fecha) && iguales(citas[i].hora, hora)) {
            return 1;
        }
    }
    return 0;
}
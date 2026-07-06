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

/* ==================== 3. Persistencia en archivo CSV ==================== */
/* Responsable: Mateo Mero */

/* Lee el campo separado por comas que empieza en linea[*pos] y lo copia
 * en destino (maximo tam-1 caracteres). Avanza *pos tras la coma. */
void leerCampo(const char *linea, int *pos, char *destino, int tam) {
    int i = 0;
    while (linea[*pos] != '\0' && linea[*pos] != ',') {
        if (i < tam - 1) {
            destino[i] = linea[*pos];
            i++;
        }
        (*pos)++;
    }
    destino[i] = '\0';
    if (linea[*pos] == ',') {
        (*pos)++;
    }
}

/* Carga las citas desde el archivo CSV. Devuelve 1 si existe, 0 si no. */
int cargarCitas(Cita citas[], int *total) {
    FILE *archivo = fopen(ARCHIVO_CITAS, "r");
    char linea[LEN_LINEA];
    int esCabecera = 1;
    int pos;
    Cita c;

    *total = 0;
    if (archivo == NULL) {
        return 0; /* aun no existe el archivo: no es un error fatal */
    }

    while (fgets(linea, LEN_LINEA, archivo) != NULL && *total < MAX_CITAS) {
        limpiarEspacios(linea);
        if (linea[0] == '\0') continue;               /* ignorar vacias */
        if (esCabecera) { esCabecera = 0; continue; } /* saltar cabecera */

        pos = 0;
        leerCampo(linea, &pos, c.codigo_cita, LEN_CODIGO);
        leerCampo(linea, &pos, c.nombre_paciente, LEN_NOMBRE);
        leerCampo(linea, &pos, c.especialidad, LEN_ESPECIALIDAD);
        leerCampo(linea, &pos, c.fecha, LEN_FECHA);
        leerCampo(linea, &pos, c.hora, LEN_HORA);
        leerCampo(linea, &pos, c.medico, LEN_MEDICO);

        if (c.codigo_cita[0] != '\0') {
            citas[*total] = c;
            (*total)++;
        }
    }

    fclose(archivo);
    return 1;
}

/* Guarda todas las citas en el archivo CSV. Devuelve 1 si tuvo exito. */
int guardarCitas(const Cita citas[], int total) {
    FILE *archivo = fopen(ARCHIVO_CITAS, "w");
    int i;
    if (archivo == NULL) {
        return 0;
    }

    fprintf(archivo, "codigo_cita,nombre_paciente,especialidad,fecha,hora,medico\n");
    for (i = 0; i < total; i++) {
        fprintf(archivo, "%s,%s,%s,%s,%s,%s\n",
                citas[i].codigo_cita, citas[i].nombre_paciente,
                citas[i].especialidad, citas[i].fecha,
                citas[i].hora, citas[i].medico);
    }

    fclose(archivo);
    return 1;
}
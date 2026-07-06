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

/* ==================== 4. Operaciones CRUD ==================== */
/* Responsable: Juan Yepes */

void imprimirEncabezado(void) {
    printf("%-12s %-20s %-15s %-11s %-6s %-20s\n",
           "Codigo", "Paciente", "Especialidad", "Fecha", "Hora", "Medico");
    printf("--------------------------------------------------------------------------------------\n");
}

void imprimirFila(const Cita *c) {
    printf("%-12s %-20s %-15s %-11s %-6s %-20s\n",
           c->codigo_cita, c->nombre_paciente, c->especialidad,
           c->fecha, c->hora, c->medico);
}

/* Lee una linea desde teclado y le quita el salto de linea final. */
void leerTexto(char *destino, int tam) {
    if (fgets(destino, tam, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    limpiarEspacios(destino);
}

int buscarIndicePorCodigo(const Cita citas[], int total, const char *codigo) {
    int i;
    for (i = 0; i < total; i++) {
        if (iguales(citas[i].codigo_cita, codigo)) return i;
    }
    return -1;
}

void registrarCita(Cita citas[], int *total) {
    Cita nueva;
    char buffer[LEN_LINEA];

    if (*total >= MAX_CITAS) {
        printf("\nCapacidad maxima alcanzada (%d citas).\n", MAX_CITAS);
        return;
    }

    printf("\n--- Registrar nueva cita ---\n");

    while (1) {
        printf("Codigo (alfanumerico, 1-15, sin espacios): ");
        leerTexto(buffer, LEN_LINEA);
        if (!validarFormatoCodigo(buffer)) {
            printf("Error: codigo invalido. Intente nuevamente.\n");
            continue;
        }
        if (existeCodigo(citas, *total, buffer)) {
            printf("Error: ya existe una cita con ese codigo.\n");
            continue;
        }
        copiar(nueva.codigo_cita, buffer, LEN_CODIGO);
        break;
    }

    printf("Nombre del paciente: ");
    leerTexto(buffer, LEN_LINEA);
    copiar(nueva.nombre_paciente, buffer, LEN_NOMBRE);

    printf("Especialidad: ");
    leerTexto(buffer, LEN_LINEA);
    copiar(nueva.especialidad, buffer, LEN_ESPECIALIDAD);

    while (1) {
        printf("Fecha (DD/MM/AAAA): ");
        leerTexto(buffer, LEN_LINEA);
        if (!validarFormatoFecha(buffer)) {
            printf("Error: formato de fecha invalido.\n");
            continue;
        }
        copiar(nueva.fecha, buffer, LEN_FECHA);
        break;
    }

    while (1) {
        printf("Hora (HH:MM, 24h): ");
        leerTexto(buffer, LEN_LINEA);
        if (!validarFormatoHora(buffer)) {
            printf("Error: formato de hora invalido.\n");
            continue;
        }
        if (horarioOcupado(citas, *total, nueva.fecha, buffer, "")) {
            printf("Error: ya hay una cita en esa fecha y hora.\n");
            continue;
        }
        copiar(nueva.hora, buffer, LEN_HORA);
        break;
    }

    printf("Medico: ");
    leerTexto(buffer, LEN_LINEA);
    copiar(nueva.medico, buffer, LEN_MEDICO);

    citas[*total] = nueva;
    (*total)++;
    printf("Cita registrada correctamente (codigo %s).\n", nueva.codigo_cita);
}

void listarCitas(const Cita citas[], int total) {
    int i;
    if (total == 0) {
        printf("\nNo hay citas registradas.\n");
        return;
    }
    printf("\n--- Listado de citas (%d) ---\n", total);
    imprimirEncabezado();
    for (i = 0; i < total; i++) {
        imprimirFila(&citas[i]);
    }
}

void buscarCita(const Cita citas[], int total) {
    char buffer[LEN_LINEA];
    int i, encontrados = 0;

    if (total == 0) {
        printf("\nNo hay citas registradas.\n");
        return;
    }

    printf("\n--- Buscar cita ---\n");
    printf("Codigo exacto o parte del nombre del paciente: ");
    leerTexto(buffer, LEN_LINEA);

    for (i = 0; i < total; i++) {
        int porCodigo = iguales(citas[i].codigo_cita, buffer);
        int porNombre = contieneSubcadena(citas[i].nombre_paciente, buffer);
        if (porCodigo || porNombre) {
            if (encontrados == 0) imprimirEncabezado();
            imprimirFila(&citas[i]);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron citas con ese criterio.\n");
    } else {
        printf("Coincidencias encontradas: %d\n", encontrados);
    }
}

void actualizarCita(Cita citas[], int total) {
    char buffer[LEN_LINEA];
    Cita temp;
    int idx;

    if (total == 0) {
        printf("\nNo hay citas registradas.\n");
        return;
    }

    printf("\n--- Actualizar cita ---\n");
    printf("Codigo de la cita a actualizar: ");
    leerTexto(buffer, LEN_LINEA);

    idx = buscarIndicePorCodigo(citas, total, buffer);
    if (idx == -1) {
        printf("No existe una cita con ese codigo.\n");
        return;
    }

    temp = citas[idx]; /* se edita una copia; se aplica solo si es valida */
    printf("Cita actual: ");
    imprimirFila(&temp);
    printf("Enter sin escribir para mantener el valor actual.\n");

    printf("Nuevo nombre [%s]: ", temp.nombre_paciente);
    leerTexto(buffer, LEN_LINEA);
    if (buffer[0] != '\0') copiar(temp.nombre_paciente, buffer, LEN_NOMBRE);

    printf("Nueva especialidad [%s]: ", temp.especialidad);
    leerTexto(buffer, LEN_LINEA);
    if (buffer[0] != '\0') copiar(temp.especialidad, buffer, LEN_ESPECIALIDAD);

    while (1) {
        printf("Nueva fecha (DD/MM/AAAA) [%s]: ", temp.fecha);
        leerTexto(buffer, LEN_LINEA);
        if (buffer[0] == '\0') break;
        if (!validarFormatoFecha(buffer)) {
            printf("Error: formato invalido.\n");
            continue;
        }
        copiar(temp.fecha, buffer, LEN_FECHA);
        break;
    }

    while (1) {
        printf("Nueva hora (HH:MM) [%s]: ", temp.hora);
        leerTexto(buffer, LEN_LINEA);
        if (buffer[0] == '\0') break;
        if (!validarFormatoHora(buffer)) {
            printf("Error: formato invalido.\n");
            continue;
        }
        copiar(temp.hora, buffer, LEN_HORA);
        break;
    }

    printf("Nuevo medico [%s]: ", temp.medico);
    leerTexto(buffer, LEN_LINEA);
    if (buffer[0] != '\0') copiar(temp.medico, buffer, LEN_MEDICO);

    if (horarioOcupado(citas, total, temp.fecha, temp.hora, temp.codigo_cita)) {
        printf("Error: la fecha y hora ya estan ocupadas por otra cita.\n");
        printf("No se aplicaron los cambios.\n");
        return;
    }

    citas[idx] = temp;
    printf("Cita actualizada correctamente.\n");
}

void eliminarCita(Cita citas[], int *total) {
    char buffer[LEN_LINEA];
    int idx, i;

    if (*total == 0) {
        printf("\nNo hay citas registradas.\n");
        return;
    }

    printf("\n--- Eliminar cita ---\n");
    printf("Codigo de la cita a eliminar: ");
    leerTexto(buffer, LEN_LINEA);

    idx = buscarIndicePorCodigo(citas, *total, buffer);
    if (idx == -1) {
        printf("No existe una cita con ese codigo.\n");
        return;
    }

    printf("Cita encontrada: ");
    imprimirFila(&citas[idx]);
    printf("Confirma la eliminacion? (S/N): ");
    leerTexto(buffer, LEN_LINEA);

    if (buffer[0] == 'S' || buffer[0] == 's') {
        for (i = idx; i < *total - 1; i++) {
            citas[i] = citas[i + 1];
        }
        (*total)--;
        printf("Cita eliminada correctamente.\n");
    } else {
        printf("Operacion cancelada.\n");
    }
}
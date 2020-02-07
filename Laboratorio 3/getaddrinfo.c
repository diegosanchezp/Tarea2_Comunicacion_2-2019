/******************************************************************************
 *                                                                            *
 * Compilar con "gcc -std=c99 -o gai getaddrinfo.c -D_POSIX_C_SOURCE=200112L" *
 *                                                                            *
 * Es necesario colocar la definicion de _POSIX_C_SOURCE con el valor 200112L *
 * como minimo para para habilitar las funciones y estructuras de datos       *
 * relacionadas a getaddrinfo().                                              *
 *                                                                            *
 * Tambien es posible compilar con la opcion "-std=gnu99" para habilitar      *
 * estas funciones sin necesidad de la definicion.                            *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * Ejecutar como "./gai [NOMBRE DE DOMINIO]"                                  *
 *                                                                            *
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdint.h>     /* Necesario para el tipo de dato uint8_t */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * El tipo de dato struct addrinfo representa la informacion de
 * un host obtenida por medio del resolvedor DNS del sistema.
 */
typedef struct addrinfo addr_t;

/*
 * El tipo de dato struct sockaddr_in representa la informacion
 * referente a una direccion IPv4.
 */
typedef struct sockaddr_in ipv4_t;

int main(int argc, const char ** argv) {
  addr_t       hints;
  addr_t *     res = NULL;
  addr_t *     i;
  ipv4_t *     addr;
  const char * host = argc > 1 ? argv[1] : "wikipedia.org";

  /*
   * Activamos los campos de hints para controlar el tipo de
   * respuestas que nos retornara el sistema operativo.
   */
  memset(&hints, 0, sizeof(addr_t)); /* Es buena practica vaciar el struct */
  hints.ai_family = AF_INET;         /* Solo queremos direcciones IPv4 */
  hints.ai_socktype = SOCK_STREAM;   /* Solo queremos direcciones para TCP */

  /*
   * Realizamos la solicitud de resolucion de nombres.
   */
  int rc = getaddrinfo(host, "http", &hints, &res);

  /*
   * Si todo salio bien, entonces getaddrinfo() retorna 0.
   */
  if (rc == 0) {
    /*
     * La funcion getaddrinfo() retorna las direcciones resueltas como
     * una lista enlazada. En Internet, un nombre puede estar asociado a
     * mas de una direccion, asi como una direccion puede estar asociada
     * a mas de un nombre.
     */
    for (i = res; i != NULL; i = i->ai_next) {
      /*
       * Verificamos si la direccion obtenida es una direccion IPv4.
       */
      if (i->ai_addr->sa_family == AF_INET) {
        /*
         * Covertimos la estructura de direccion a la correspondiente a IPv4.
         */
        addr = (ipv4_t *)i->ai_addr;

        /*
         * Imprimimos los octetos de la direccion.
         */
        printf("Direccion: %u.%u.%u.%u\n",
               (uint8_t)addr->sin_addr.s_addr,
               (uint8_t)(addr->sin_addr.s_addr >> 8),
               (uint8_t)(addr->sin_addr.s_addr >> 16),
               (uint8_t)(addr->sin_addr.s_addr >> 24));
      }
    }

  } else {
    /*
     * Si hubo un error, la funcion gai_strerror() nos traduce el codigo
     * de error retornado por getaddrinfo en un string imprimible.
     */
    fprintf(stderr, "%s: fatal error: %s\n", argv[0], gai_strerror(rc));
    return 1;
  }

  /*
   * Al terminar de usar la lista retornada por getaddrinfo() hay que
   * borrarla para liberar la memoria que usa.
   */
  if (res != NULL)
    freeaddrinfo(res);

  return 0;
}

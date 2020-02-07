/******************************************************************************
 *                                                                            *
 * Compilar con "gcc -std=c99 -o ec echo_client.c -D_POSIX_C_SOURCE=200112L"  *
 *                                                                            *
 * Es necesario colocar la definicion de _POSIX_C_SOURCE con el valor 200112L *
 * como minimo para para habilitar la estructuras de datos sockaddr_in.       *
 *                                                                            *
 * Tambien es posible compilar con la opcion "-std=gnu99" para habilitar      *
 * estas funciones sin necesidad de la definicion.                            *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * Ejecutar como "./ec"                                                       *
 *                                                                            *
 ******************************************************************************/

#include <stdio.h>
#include <errno.h>      /* Para la variable global errno */
#include <signal.h>     /* Para la funcion signal() */
#include <strings.h>    /* Para la funcion bzero() */
#include <stdbool.h>    /* Para el tipo de dato bool */
#include <unistd.h>     /* Para la funcion close() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>  /* Para la funcion htons() */

/*
 * Este es el puerto en el que el servidor se pondra a escuchar.
 */
#define PORT 9989

/*
 * Tamaño del buffer de mensajes.
 */
#define BUFFER_SIZE 256

int main(int argc, char ** argv) {
  int                  fd, rc;
  socklen_t            sin_size = sizeof(struct sockaddr_in);
  struct sockaddr_in   server;
  struct addrinfo      s_addr;
  struct sockaddr_in * ipv4;
  struct addrinfo *    res;
  char                 c, buffer[BUFFER_SIZE];
  ssize_t              br, bs;
  const char *         host = argc > 1 ? argv[1] : "127.0.0.1";
  bool                 done = false;

  /*
   * Convertimos la dirección IP (o nombre de dominio) del servidor en
   * una estructura de datos que puede usarse en la interfaz de sockets.
   */
  rc = getaddrinfo(host, NULL, NULL, &res);
  if (rc != 0) {
    /*
     * Si hubo un error, la funcion gai_strerror() nos traduce el codigo
     * de error retornado por getaddrinfo en un string imprimible.
     */
    fprintf(stderr, "%s: fatal error: %s\n", argv[0], gai_strerror(rc));
    return 1;
  }

   /*
   * Creamos un descriptor de archivos para un socket TCP sobre IPv4.
   */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd == -1 ){
    perror("Error in socket()");
    return 1;
  }

  /*
   * Establecemos los parametros del socket.
   */
  server.sin_family = AF_INET;         /* Red IPv4. */
  server.sin_port = htons(PORT);       /* Puerto a utilizar. */
  ipv4 = (struct sockaddr_in *)res->ai_addr;
  server.sin_addr.s_addr = ipv4->sin_addr.s_addr; /* Direccion IP del servidor. */
  bzero(&(server.sin_zero), 8);        /* Bytes en cero. */

  /*
   * Conectamos con el servidor.
   */
  rc = connect(fd, (struct sockaddr *)&server, sin_size);
  if (rc == -1) {
    perror("Error en connect()");
    goto end; /* Dios mio, que horror! */
  }

  /*
   * Procedemos a enviar datos al servidor.
   */
  while (!done) {
    /*
     * Leemos de la terminal un byte.
     */
    while((br = read(0, &c, 1))) {
      /*
       * Eviamos un byte al servidor.
       */
      bs = send(fd, &c, 1, 0);

      /*
       * Terminamos si hubo un error en el envio.
       */
      if(bs <= 0)
        break;

      /*
       * Recibimos un byte del servidor.
       */
      br = recv(fd, &c, 1, 0);

      /*
       * Terminamos si hubo un error en la recepcion.
       */
      if (br < 0 && errno != EWOULDBLOCK)
        break;

      /*
       * Escribimos un caracter a la pantalla.
       */
      write(1, &c, 1);
    }

    /*
     * Si la funcion read() retorna 0, es porque el usuario presiono
     * ctrl+D (fin de archivo) en la terminal. En este punto podemos
     * entonces proceder a cerrar la conexion.
     */
    done = true;
  }

 end:
  /*
   * Cerramos el descriptor de archivo cuando ya no sea necesario.
   */
  close(fd);

  return 0;
}

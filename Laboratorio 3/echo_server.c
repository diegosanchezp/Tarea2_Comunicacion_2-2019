/******************************************************************************
 *                                                                            *
 * Compilar con "gcc -std=c99 -o es echo_server.c -D_POSIX_C_SOURCE=200112L"  *
 *                                                                            *
 * Es necesario colocar la definicion de _POSIX_C_SOURCE con el valor 200112L *
 * como minimo para para habilitar la estructuras de datos sockaddr_in.       *
 *                                                                            *
 * Tambien es posible compilar con la opcion "-std=gnu99" para habilitar      *
 * estas funciones sin necesidad de la definicion.                            *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *                                                                            *
 * Ejecutar como "./es"                                                       *
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
 * Esta es la cantidad de conexiones pendientes que soporta el
 * servidor. Si el servidor está conectado con un cliente y hay
 * BACKLOG clientes en espera y llega un cliente adicional, sera
 * rechazado por el sistema operativo.
 */
#define BACKLOG 10

/*
 * Tamaño del buffer de mensajes.
 */
#define BUFFER_SIZE 256

/*
 * Indica si el servidor debe detenerse (cuando es true).
 */
bool g_done = false;

/*
 * Manejador que captura la senal SIGINT (ctrl + C).
 */
void onsignal(int signal) {
  g_done = true;
}

int main(void) {
  int                  server_fd, client_fd, rc;
  socklen_t            sin_size = sizeof(struct sockaddr_in);
  struct sockaddr_in   server;
  struct sockaddr_in   client;
  char                 buffer[BUFFER_SIZE];
  ssize_t              br, bs;

  /*
   * Establecemos el manejador para capturar la senal SIGINT
   */
  signal(SIGINT, onsignal);

  /*
   * Creamos un descriptor de archivos para un socket TCP sobre IPv4.
   */
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd == -1 ){
    perror("Error in socket()");
    return 1;
  }

  /*
   * Establecemos los parametros del socket.
   */
  server.sin_family = AF_INET;         /* Red IPv4. */
  server.sin_port = htons(PORT);       /* Puerto a utilizar. */
  server.sin_addr.s_addr = INADDR_ANY; /* Acepta conexiones de cualquier origen. */
  bzero(&(server.sin_zero), 8);        /* Bytes en cero. */

  /*
   * Le pedimos a Linux que le asigne los parametros del socket al
   * descriptor de archivo.
   */
  rc = bind(server_fd, (struct sockaddr*)&server, sizeof(struct sockaddr));
  if(rc == -1){
    perror("Error in bind()");
    return 1;
  }

  /*
   * Habilitamos el modo de escucha pasiva en este socket. Este
   * paso efectivamente establece que el socket funcionara en modo
   * servidor.
   */
  rc = listen(server_fd, BACKLOG);
  if(rc == -1){
    perror("Error in listen()");
    return 1;
  }

  while (!g_done) {
    /*
     * Esperamos a que se conecte algun cliente. La funcion accept es
     * bloqueante y retorna un descriptor de archivo para comunicarse con
     * el cliente. El parametro de salida "client" contiene la direccion IP
     * del cliente.
     */
    client_fd = accept(server_fd, (struct sockaddr *)&client, &sin_size);
    if(client_fd == -1) {
      if (errno != EINTR)
        perror("Error in accept()");
      continue;
    }

    /*
     * Llenamos con ceros el buffer de mensajes.
     */
    bzero(buffer, BUFFER_SIZE * sizeof(char));

    /*
     * Leemos mensajes del cliente y se los reenviamos directamente. La
     * funcion recv() retorna 0 cuando se cierra la conexion del lado del
     * cliente, y -1 en caso de error.
     */
    while ((br = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
      bs = send(client_fd, buffer, br, 0);
      if (bs == -1)
        perror("Error in send()");
    }

    if (br == -1) {
      perror("Error in recv()");
    }

    /*
     * Cerramos la conexión con el cliente cuando ya no es necesaria.
     */
    close(client_fd);
  }

  /*
   * Cerramos los descriptores de archivo cuando ya no sean necesarios.
   */
  close(server_fd);

  return 0;
}

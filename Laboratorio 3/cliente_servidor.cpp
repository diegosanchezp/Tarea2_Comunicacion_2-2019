/*Programa que es cliente y servidor a la vez*/

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>   /* Para la variable global errno */
#include <signal.h>  /* Para la funcion signal() */
#include <strings.h> /* Para la funcion bzero() */
#include <stdbool.h> /* Para el tipo de dato bool */
#include <unistd.h>  /* Para la funcion close() */
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> /* Para la funcion htons() */

#define PORT "9989"
#define BACKLOG 2

/*
 * Tamaño del buffer de mensajes.
 */
#define BUFFER_SIZE 256
void cliente(const char *direccion)
{
  struct addrinfo hints, *res;
  int sockfd;

  // Cargar datos en estructura
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(direccion, PORT, &hints, &res);

  // Crear un socket
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  // Conectar
  connect(sockfd, res->ai_addr, res->ai_addrlen);
}

void servidor()
{
  struct sockaddr_storage client_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, client_fd;
  int len, bytes_enviados, rc;
  char buffer[BUFFER_SIZE];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;     // usar IPv4 o IPv6
  hints.ai_socktype = SOCK_STREAM; // paquetes llegan en orden
  hints.ai_flags = AI_PASSIVE;     // rellenar ip automaticamente

  // Cargar datos en estructura
  getaddrinfo(NULL, PORT, &hints, &res);

  // Crear socket y asociar puerto con el
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  rc = bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (rc == -1)
  {
    perror("Error in bind()");
    return;
  }

  // Escuchar por peticiones
  rc = listen(sockfd, BACKLOG);
  if (rc == -1)
  {
    perror("Error in listen()");
    return;
  }
  // Manejo de errores (FALTA COMPLETAR)
  // Aceptar conexion
  addr_size = sizeof client_addr;
  client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
  if (client_fd == -1)
  {
    if (errno != EINTR)
    {
      perror("Error in accept()");
      return;
    }
  }
  // A partir de aqui ya esta listo para comunicar con el cliente
  char *msg = "Respuestas del servidor";

  len = strlen(msg);
  bytes_enviados = send(sockfd, msg, len, 0);
}

int main(int argc, char **argv)
{
  return 0;
}
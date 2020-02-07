/*Programa que es cliente y servidor a la vez*/

/***/

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
#include <thread>         // std::thread
#include <iostream>       // std::cout
#include <mutex>          // std::mutex


#define PORT "9989"
#define BACKLOG 2
#define SERVER_IP "190.169.75.130"
/*
 * Tamaño del buffer de mensajes.
 */
#define BUFFER_SIZE 256

std::mutex mtx;           // mutex for critical section



void cliente(char *ip)
{
  struct addrinfo hints, *res;
  int sockfd;
  char respuesta[BUFFER_SIZE];
  //mtx.lock();
  // Cargar datos en estructura
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(ip, PORT, &hints, &res);
  
  // Crear un socket
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  
  // Conectar
  int conE = connect(sockfd, res->ai_addr, res->ai_addrlen);
  if(conE==-1){std::cout<< "Error en conexion" << std::endl;}
  char *msg = "Hola servidor";
  std::cout<<"Cliente ejecutandose"<<std::endl;
  int len = strlen(msg);
  
  // Enviar data al servidor
  send(sockfd, msg, len, 0);
  
  // Recibir data del servidor
  recv(sockfd, respuesta, BUFFER_SIZE -1, 0);

  std::cout << "Respuesta del servidor: " << respuesta <<std::endl;
  // Cerrar conexion una vez enviado el mensaje
  //mtx.unlock();
  //close(sockfd);
}

void servidor()
{

  struct sockaddr_storage client_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, client_fd;
  int len, bytes_enviados, rc;
  char buffer[BUFFER_SIZE];

  //mtx.lock();
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
  std::cout << "servidor Escuchando" << buffer<< std::endl;
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
  
  char hname[500];
  gethostname(hname, 500);
  std::cout << "Host name: " << hname << std::endl;
  // A partir de aqui ya esta listo para comunicar con el cliente

  // Recibir mensaje
  recv(client_fd, buffer, BUFFER_SIZE -1, 0);
  
  std::cout << "servidor recibio respuesta del cliente: " << buffer<< std::endl;
  
  char *msg = "Hola desde el servidor";

  len = strlen(msg);
  bytes_enviados = send(client_fd, msg, len, 0);
  std::cout << "Servidor: Bytes enviados" << bytes_enviados<< std::endl;
  

  // Desocupar socket
  int yes=1;
  //char yes='1'; // Solaris people use this
  
  // lose the pesky "Address already in use" error message
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
  } 
  // Cerrar conexiones cliente y servidor en orden
  close(client_fd);
  close(sockfd);
  //mtx.unlock();
}

int main(int argc, char **argv)
{
  if(argc == 1){
    servidor();
  }
  if(argc==2){
    cliente(argv[1]);
  }
  return 0;
}
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
#define BUFFER_SIZE 256
int main(int argc, char **argv)
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
      return 0;
    }
  
    // Escuchar por peticiones
    rc = listen(sockfd, BACKLOG);
    if (rc == -1)
    {
      perror("Error in listen()");
      return 0;
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
        return 0;
      }
    }
    // A partir de aqui ya esta listo para comunicar con el cliente
  
    // Recibir mensaje
    recv(sockfd, buffer, BUFFER_SIZE -1, 0);
    
    std::cout << "servidor recibio respuesta del cliente: " << buffer<< std::endl;
    
    char *msg = "Respuestas del servidor";
  
    len = strlen(msg);
    bytes_enviados = send(sockfd, msg, len, 0);
    // Cerrar conexiones cliente y servidor en orden
    //close(client_fd);
    //close(sockfd);
  
    // Desocupar socket
    int yes=1;
    //char yes='1'; // Solaris people use this
    
    // lose the pesky "Address already in use" error message
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
        perror("setsockopt");
        exit(1);
    } 
    //mtx.unlock();

    // Cerrar conexiones cliente y servidor en orden
    close(client_fd);
    close(sockfd);
    return 0;
}
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
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 256
int main(int argc, char **argv){
    struct addrinfo hints, *res;
    int sockfd;
    char respuesta[BUFFER_SIZE];
    //mtx.lock();
    // Cargar datos en estructura
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(SERVER_IP, PORT, &hints, &res);
    
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
    close(sockfd);
    return 0;
}
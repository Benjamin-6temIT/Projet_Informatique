#include <wiringPi.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>



int func( int a ) {
    return 2 * a;
}

void sendData( int sockfd, int x ) {
    int n;
    
    char buffer[32];
    sprintf( buffer, "%d\n", x );
    if ( (n = write( sockfd, buffer, strlen(buffer) ) ) < 0 )
    {
        printf("ERROR writing to socket");
    }
    buffer[n] = '\0';
}

int getData( int sockfd ) {
    char buffer[32];
    int n;
    
    if ( (n = read(sockfd,buffer,31) ) < 0 )
    {
        printf("ERROR reading from socket");

    }
    
    buffer[n] = '\0';
    return atoi( buffer );
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno = 51717, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int data;
    wiringPiSetup();
    pinMode(27,OUTPUT);
    
    printf( "using port #%d\n", portno );
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons( portno );
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR on binding");
    }
    
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    //--- infinite wait on a connection ---
    while ( 1 ) {
        printf( "waiting for new client...\n" );
        if ( ( newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen) ) < 0 )
        {
            printf("ERROR on accept");
        }
        
        printf( "opened new communication with client\n" );
        
        while ( 1 ) {
            //---- wait for a number from client ---
            data = getData( newsockfd );
            printf( "donnees recus :  %d\n", data );
            
            if ( data < 0 )
            {
                break;
            }
            
           /* if(data == 11)
            {
                printf("allumage lampe verte\n");
                digitalWrite(27,HIGH);
                sendData( newsockfd, 101 );
            }*/
            
            switch (data) {
                case 11:
                    printf("allumage lampe verte\n");
                    digitalWrite(27,HIGH);
                    sendData( newsockfd, 101 );
                    break;
                    
                case 10 :
                    printf("arret lampe verte\n");
                    digitalWrite(27,LOW);
                    sendData( newsockfd, 101 );
                    break;
                    
                    
                default:
                    printf("La lampe ne peut pas s'allumer");
                    break;
            }
            
            
            
            
            data = func( data );
            
            //--- send new data back ---
            sendData( newsockfd, data );
        }
        close( newsockfd );
        
        //--- if -2 sent by client, we can quit ---
        if ( data == -2 )
        {
            break;
        }
    }
    return 0; 
}
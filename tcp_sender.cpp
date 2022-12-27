#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

 int	port = 8888,
        localSocket,
        remoteSocket;

struct sockaddr_in localAddr, remoteAddr;

int addrLen = sizeof(sockaddr_in);

void *display(void *ptr)
{
    int socket = *(int *)ptr;
    
    cv::Mat img;
    img = cv::Mat::zeros(600, 600, CV_8UC3);
    
    if (!img.isContinuous())
        img = img.clone();

    int imgSize = img.total() * img.elemSize();
    int bytes = 0;

    std::cout << "Image Size:" << imgSize << std::endl;

    while(1)
    {   
        if ((send(socket, img.data, imgSize, 0)) < 0)
        {
             std::cerr << "connect loss" << std::endl;
             close(remoteSocket);
             break;
        } 
    }
}

int main(int argc, char **argv)
{ 
        pthread_t thread_id;

        if ( (argc > 1) && (strcmp(argv[1],"-h") == 0) )
		{
			std::cerr << "usage : ./sever_video [port] [capture device]\n" <<
			"port : socket port (8888 default)\n" <<
			"capture device : (1 default)\n\n";
			exit(1);
        }
		if (argc == 2) port = atoi(argv[1]);
        
        localSocket = socket(AF_INET , SOCK_STREAM , 0);
        if (localSocket == -1)
            perror("socket() call failed!!");
             
        int on = 1;
        if(setsockopt(localSocket, SOL_SOCKET, SO_REUSEADDR,
		 &on, sizeof(on)) < 0)
        {
			std::cout <<  "socket option set error" << std::endl;
			return -1;
        }

        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = INADDR_ANY;
        localAddr.sin_port = htons( port );
        
        if( bind(localSocket,(struct sockaddr *)&localAddr,
         sizeof(localAddr)) < 0)
		{
            perror("Can't bind() socket");
            exit(1);
        }

        for(;;)
        {
			listen(localSocket, 5);          
			std::cout <<  "Server Port:" << port << std::endl;
			std::cout <<  "Waiting for connections...\n";      
			
			remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr,
			 (socklen_t*)&addrLen);
			
			if (remoteSocket < 0)
			{
				perror("accept failed!");
				exit(1);
			}  
			std::cout << "Connection accepted\n";

			pthread_create(&thread_id,NULL,display,&remoteSocket);
         }
         
        pthread_join(thread_id, NULL);
        close(remoteSocket); 
        close(localSocket);      
           
        std::cout << std::endl;
        return 0;
}
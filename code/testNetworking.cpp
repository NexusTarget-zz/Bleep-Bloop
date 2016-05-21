#include <stdio.h>
#include <time.h>

extern "C" int init(int d_lev);
extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);

int main (){
   // This sets up the RPi hardware and ensures
   // everything is working correctly
   init(0);
   connect_to_server("130.195.6.196", 1024); //connects to server with the ip address 130.195.6.196 on port 1024
   send_to_server("Please"); //sends please to the connected server
   char message[24];
   receive_from_server(message); //receives message from the connected server
   printf("%s/n", message);
   send_to_server(message);
   printf("%s/n", "Message was sent to the server");
   return 0;
}

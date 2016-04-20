#include <stdio.h>
#include <time.h>

extern "C" int init(int d_lev);
extern "C" int connect_to_server( char server_addr[15], int port);
extern "C" int send_to_server(char password[24]);
extern "C" int receive_from_server(char password[24]);

int main (){
   // This sets up the RPi hardware and ensures
   // everything is working correctly
   init(0);
   connect_to_server("###.###.###.###", ####); //connects to server with the ip address 130.195.6.196 on port 1024
   send_to_server("Please"); //sends please to the connected server
   char password[24];
   receive_from_server(password); //Password from the connected server
   printf("%s/n", password);
   send_to_server(password);
   printf("%s/n", "Password was sent to the server");
   return 0;
}

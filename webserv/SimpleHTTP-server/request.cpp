#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

int main()
{
   /*
      server_fd: fd for socket the server listens on
      new_socket: fd for an accepted client connection
      address: a struct holds address information for the server( IP@ and PORT)
      opt: setting socket option (like SO_REUSEADDR)
      addrlen: size of struct addrss, used for binding and accepting connections
      PORT: port a erver will listen on
   */
   int server_fd, new_socket;
   struct sockaddr_in address;
   int opt = 1;
   int addrlen = sizeof(address);
   const int PORT = 8080;

   // creating socket file descriptor

   /*
      AF_INET: address family (IPV2)
      SOCK_STREAM: socket type (TCP, which is connection-oriented)
      0 : specifies the protocal (0 means it uses the default socket type which is TCP for SOCK_STREAM)
   */
   if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
   {
      perror("Socket failed");
      exit(EXIT_FAILURE);
   }

   //Forcefully attaching socket to the port 8080

   /*
      `setsockopt`: this function sets options on the socket
      `SO_REUSEADDR`: allows the socket to bound to @ that is already in use
   */
   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
   {
      perror("Setsockopt failed");
      exit(EXIT_FAILURE);
   }
   address.sin_family = AF_INET; // specifies address family as IPV4
   address.sin_addr.s_addr = INADDR_ANY; //Binds the socket to all available interfaces (i.e., it listens on all IP addresses the machine has)
   address.sin_port = htons(PORT);// Converts the port number from host byte order to network byte order using htons (short for "host to network short").

   //Bind the socket to the network addess and port

   /*
      bind: This function binds the socket to the IP address and port specified in address.
      If binding fails (e.g., the port is already in use), an error message is printed, and the program exits
   */
   if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
   {
      perror("Bind failed");
      exit(EXIT_FAILURE);
   }

   //listen for incoming connections

   /*
      listen: Puts the server socket into a listening state, ready to accept connections. 
      The second argument (3) specifies the backlog, which is the maximum number of pending connections in the queue. If listen fails, an error is printed.
   */
   if (listen(server_fd,3) < 0)
   {
      perror("Listen failed");
      exit(EXIT_FAILURE);
   }
   std::cout << "Server started on Port" << PORT << "\n";
   
   //Accept a connection

   /*
      accept: Accepts a new incoming connection on the listening socket (server_fd). 
      It returns a new file descriptor (new_socket) for the connection. This is the point where the server starts interacting with a client.
   */
   if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
   {
      perror("Accept failed");
      exit(EXIT_FAILURE);
   }
   std::cout << "Connection accepted \n";

   //Handling HTTP Request and Response

   /*
      1.char buffer[30000] = {0};: Creates a buffer to store the incoming data from the client.
      2.read(new_socket, buffer, 30000);: Reads data from the client into the buffer. 
         This data is the HTTP request sent by the client.
      3.std::string html: contains HTTP response
         - version and status code
         - Content-Type: text/html: The MIME type of the content being returned(HTML in this case)
         - body
      ** send(new_socket, html.c_str(), html.size(), 0);: Sends the HTTP response back to the client using the new_socket descriptor.
       html.c_str() converts the string to a C-style string, and html.size() gives the length of the response.

   */
   char buffer[30000] = {0};
   read(new_socket, buffer, 30000);
   std::cout << buffer << "\n";

    std::string html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                       "<html><head><title>My Web Server</title></head>"
                       "<body><h1>Hello, World! first test of HTTP Server</h1></body></html>";
      
   send(new_socket, html.c_str(), html.size(), 0);
   std::cout << "Response sent\n";

   return 0;
   
}
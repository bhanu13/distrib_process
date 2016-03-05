#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

#define MAX_PROCS 4
#define MAX_MSG_L 50
#define BASE_PORT 3000		// Starting from Port 3000
#define IP "localhost"

using namespace std;

struct process
{
	// public:
		process();
		~process();

		void killprocess();
		int create_client_connection(int pid);			// Create a new connection with a server process
		void * create_server(void * t);
		// For pthreads
		static void * create_server_callback(void * t);
		static void * get_command_callback(void * t);
		static void * get_data_callback(void * t);

		void close_client(int dest_id);				// Close connection with a server process
		void close_server();

		int connect_to_server(int pid);
		//---------------------------------------------
		//Multicast Functionality
		static void * multicast_send_callback(void * t);

		void multicast_send(string & msg);
		bool check_timestamp(int * msg_ts, int src_id);

		void add_timestamp(string & send_msg);
		void get_timestamp(string & recv_msg, int * msg_ts);
		void set_ts(string new_ts);

		//----------------------------------------------

		// Unicast Functionality 
		void unicast_send(int dest_id, string send_msg);
		void unicast_receive(int src_id, string & recv_msg);
		
		void delay_send(int dest_id, string send_msg);
		void delay_receive(int src_id, string & recv_msg);
		int delay_channel();

		static void * delay_send_callback(void * t);
		static void * delay_receive_callback(void * t);

		//----------------------------------------------

		void * get_data(int t);						// New thread for each connection
		void * get_command(void * t);						// A single thread for command input

		// Helper Functions
		int connect_to_server_(const char* domain_or_ip, const char* port_string);	// Open a new socket to the specified listening port
		static int server_response(int socket_to_client);			// The response sent to client process
		void close_connection(int pid);
		int create_listen_server(const char* port_to_bind,  int(*handleConnection)(int));		// Create a listen server at specified port
		void * get_in_addr(struct sockaddr* sa);
		void sigchld_handler(int s);
	// private:
		int my_id;			// My Process ID.
		int c_sockets[MAX_PROCS];	// My Open Sockets with other processes.
		int my_socket;		// My listening socket to accept new connections.
		int num_connect;
		int server_on;

		// TEMP DATA for multi threaded
		int new_id;
		// int send_id;
		string msg_send;
		string msg_recv;

		int * ts;
		int ts_lock;

		// int socket_lock;
		int can_send;
		// string Q[10];
};

#endif
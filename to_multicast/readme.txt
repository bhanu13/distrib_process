bagarwa2, 03/03/16

This directory contains the implementation of total ordering multicast on top the of delay channel unicast.

MAX Delay
5secs

MIN Delay
1sec

Usage:
make 					- to compile
./proc 					- to execute the program
id <num> 				- to give the process it's particular id (ranging from 0 to 3)
server 					- spawn server for the currect process id
connect <dest_id> 		- connect the current process to the process with the dest_id
seq 					- Specify the current process as the sequencer
lock 					- Prints the state of the locks
msend "Message"			- Send the message to all connected processes in total ordering

Shortcuts
type: s - corresponds to server command
type: c - corresponds to connect command


	IP 						ProcessID
BASE_PORT + ProcessID		ProcessID

BASE_PORT = 3000
So, IPs used would be:
3000, 3001, 3002, 3003 and 3004

Functionality to Test:
- A thread runs in the background to collect incoming data from each of the open connections and prints on the terminal.
- The multicast_send_callback() creates a new thread to call multicast_send() to send the message to all processes including the sequencer.
	- A unique token is added to the msg that is retrieved by each process and broadcasted by the sequencer to enforce total ordering.

  Function Prototypes:

  		static void * multicast_send_callback(void * t);
		void multicast_send(string & msg);

		int get_token(string msg, int src_id);
		void broadcast_token(string & msg);
		string generate_token(string & msg);		

- A process class exists to handle functionality for spawning a server, connecting to a server and taking in commands.

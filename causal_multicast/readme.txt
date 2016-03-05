bagarwa2, 03/03/16

This directory contains the implementation of causal ordering multicast on top the of delay channel unicast.

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
msend "Message"			- Send the message to all connected processes in causal ordering
ts 						- Prints the time stamp of the current process
lock 					- Prints out the states of the time stamp lock and send lock
set ts <new_ts>			- Used to set the time stamp in the format <t0,t1,t2,t3>


Shortcuts
type: s - corresponds to server command
type: c - corresponds to connect command

		
	IP 						ProcessID
BASE_PORT + ProcessID		ProcessID

BASE_PORT = 3000
So, IPs used would be:
3000, 3001, 3002, 3003 and 3004

Functionality to Test:
- A thread runs in the background to collect incoming data from each of the open connections and prints the data received on the terminal.
- mulicast_send_callback() implements a new thread to send causally ordered multicast to each process connected to.
	- This is implemented by appending the vector timestamp of the message.
	- The receiving process retrieves the time stamp and checks if the message is the next one to be delivered else it waits until the appropriate message
	  is received.
	- Each process maintains the vector time stamp of the last event.

  Function Prototypes:
		static void * multicast_send_callback(void * t);

		void multicast_send(string & msg);

		bool check_timestamp(int * msg_ts, int src_id);
		void add_timestamp(string & send_msg);
		void get_timestamp(string & recv_msg, int * msg_ts);
		void set_ts(string new_ts);

- A process class exists to handle functionality for spawning a server, connecting to a server and taking in commands.

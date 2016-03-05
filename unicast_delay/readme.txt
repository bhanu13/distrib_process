bagarwa2, 03/03/16

This directory contains the implementation of the delay channel on top of the unicast send/receive functionality
for the process class.

MAX Delay
10secs

MIN Delay
1sec

Usage:
make 					- to compile
./proc 					- to execute the program
id <num> 				- to give the process it's particular id (ranging from 0 to 3)
server 					- spawn server for the currect process id
connect <dest_id> 		- connect the current process to the process with the dest_id
send <dest_id> "Message" 	- Send the message to the destination id in this format using delay_send functionality


	IP 						ProcessID
BASE_PORT + ProcessID		ProcessID

BASE_PORT = 3000
So, IPs used would be:
3000, 3001, 3002, 3003 and 3004

Functionality to Test:
- A thread runs in the background to collect incoming data from all the open connections and prints on the terminal.
- delay_send() and delay_receive() functions simulate the delay by making the process sleep for a random amount of time before calling 
  unicast send from delay_send()

  Function Prototypes:
  		void delay_send(int dest_id, string send_msg);
		void delay_receive(int src_id, string & recv_msg);
		int delay_channel();
		
- A process class exists to handle functionality for spawning a server, connecting to a server and taking in commands.

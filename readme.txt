Process Simulation

This is a repository that implements the simulation of a Distributed System as a collection of processes.
It implements the ability of a process to act as a client or a server or both.
Processes can communicate through different methods of message passing.

It uses TCP sockets in C, Posix Threads and convenient libraries such as string, time, unordered map etc. from C++. Thus, the files can be compiled using either g++ or clang++. Makefiles exist in each directory along with the source code.

Includes:
- process library
- Source code for Delayed Unicast, Totally Ordered Multicast and Causally Ordered Multicast.


Features:
- A process class to handle functionality for spawning a server, connecting to a server as a client and taking in commands.
- Simulates network delays as in a practical system.
- Supports up to 4 processes to interact with each other using unique process ids can be increased by changing MAX_PROCS in .h file.
- Easy to use command line interface for different functions.
- Implements total ordering using token generation and causal ordering using vector time stamps.


Usage:
- Open a new terminal (Terminal 1) - acting as process P1
	- Execute ./proc
	- Input "id <pid>"
	- Input "server"

- Open another terminal (Terminal 2) - acting as process P2
	- Execute ./proc
	- Input "id <another pid>"
	- Input "connect <pid>" 

Example:

	On Terminal 1:
	$ ./proc 
	Command: id 0
		ID set to 0
	Command: server
		Command: server: bound to port 3000 and waiting for connections...
	server: got connection from ::1
	Connected to process 0
	Connection Setup with Process 1
	Received "hello" from process 1 system time is 1457060722

	On Terminal 2:
	$ ./proc 
	Command: id 1
		ID set to 1
	Command: connect 0
		Connected to process 0
	Command: Received "Connection Setup with Process" from process 0 system time is 1457060706
	send 0 hello
	Command: Sent "hello" to process 0 system time is 1457060715


Accepted Commands:

make 							- to compile
./proc 							- to execute the program
id [num] 						- to give the process it’s particular id (ranging from 0 to 3)
server 							- spawn server for the correct process id
connect [dest id] 				- connect the current process to the process with the dest id
send [dest id] ”Message” 		- Send the message to the destination id in this format using delay send functionality
recv [src id] 					- blocks and waits until a message arrives from the src id process using unicast receive msend ”Message” - Send the message to all connected processes in the appropriate ordering
ts 								- Prints the time stamp of the current process
lock 							- Prints out the states of the time stamp lock and send lock
set ts [new ts] 				- Used to set the time stamp in the format ”t0,t1,t2,t3”
seq 							- Specify the current process as the sequencer


Pending:

- Clean up code is not robust yet.
- Closing sockets and cleaning up dynamic memory.
- Switch to real synchronization primitives(mutexes) instead of my own hacky spin locks.

Copyright bhanu13, bagarwa2, 2016
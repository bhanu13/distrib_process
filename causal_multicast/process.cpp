#include "process.h"

struct process_t{
	process * p;
	int pid;
};
// Commands Supported:
// send <p_id> "Message"
// msend "Message"
// connect <p_id>
// ts 
// id <my_id>
// exit <p_id>
// server <my_id>
// close
void * process::get_command_callback(void * t)
{
	process * p = (process *) t;
	p->get_command(NULL);
	return NULL;
}

void * process::get_command(void * t)
{
	while(num_connect >= 0){

	cout<<"Command: ";
	string cmd;
	getline(cin, cmd);

	int idx_send = cmd.find("send");
	int idx_msend = cmd.find("msend");
	int idx_recv = cmd.find("recv");
	int idx_end = cmd.find("exit");
	int idx_id = cmd.find("id");
	int idx_connect = cmd.find("connect");
	int idx_server = cmd.find("server");
	int idx_close_server = cmd.find("close");	
	int idx_ts = cmd.find("ts");

	int idx_set_ts = cmd.find("set ts");
	int idx_ts_lock = cmd.find("lock");

	int dest_id = -1;
	if(idx_send >= 0 && idx_msend < 0)
	{
		int proc_idx = cmd.find_first_of("012345");
		if(proc_idx >= 0)
			dest_id = (int)cmd[proc_idx] - '0';
		// cout<<dest_id<<endl;
		if(dest_id >= 0 && dest_id < MAX_PROCS && c_sockets[dest_id] != -1)
		{
			string msg = cmd.substr(proc_idx + 2);
			this->msg_send = msg;
			new_id = dest_id;
			// delay_send(dest_id, msg);
			// Create a new thread to send the message
			// Add non-blocking functionality
			pthread_t send_t;
			pthread_create(&send_t, NULL, delay_send_callback, this);

		}
		else 
		{
			cout<<"Invalid Destination Process ID"<<endl;
		}
	}
	else if(idx_msend >= 0)
	{
		msg_send = cmd.substr(idx_msend + 6);
		// multicast_send(msg_send);
		pthread_t msend_t;
		pthread_create(&msend_t, NULL, multicast_send_callback, this);

	}
	else if(idx_recv >= 0)
	{
		int src_id = -1;
		int proc_idx = cmd.find_first_of("012345");
		if(proc_idx >= 0)
			src_id = (int)cmd[proc_idx] - '0';

		if(src_id >= 0 && src_id < MAX_PROCS && c_sockets[src_id] != -1)
		{
			string msg = cmd.substr(proc_idx + 1);
			// delay_receive(src_id, msg);
			// Create a new thread to receive message
			msg = this->msg_recv;
		}
		else 
		{
			cout<<"Invalid Source Process ID"<<endl;
		}
	}
	else if(idx_end >= 0)
	{
		dest_id = (int)cmd[cmd.find_first_of("012345")] - '0';
		if(dest_id >= 0 && c_sockets[dest_id] != -1)
			close_client(dest_id);
		else
			break;
	}

	else if(idx_id >= 0)
	{
		int temp_id = (int)cmd[cmd.find_first_of("012345")] - '0';
		if(temp_id>= 0)
			my_id = temp_id;
		cout<<"ID set to "<<my_id<<endl;
	}

	else if(idx_close_server >= 0)
	{
		cout<<"Closing server."<<endl;
		close(my_socket);
		server_on = 0;
		my_socket = -1;
	}
	else if(idx_set_ts >= 0)
	{
		int i = cmd.find_first_of("0123456789");
		if(i >= 0){
			set_ts(cmd.substr(i));
		}
	}
	else if(idx_ts >= 0)
	{
		for(int i = 0; i<MAX_PROCS; i++)
		{
			cout<<ts[i]<<",";
		}
		cout<<endl;
	}
	else if(idx_ts_lock >= 0)
	{
		cout<<"State of the Timestamp Lock "<<ts_lock<<endl;
		cout<<"State of the Send Lock "<<can_send<<endl;

	}
	//----- Shortcuts for running the server and connecting to another server.
	else if(idx_connect >= 0 || ((int)cmd.find("c") >= 0))
	{
		dest_id = (int)cmd[cmd.find_first_of("012345")] - '0';
		if(dest_id >= 0)
			connect_to_server(dest_id);
	}

	else if(idx_server >= 0 || ((int)cmd.find("s") >= 0))
	{
		if(server_on == 0)
		{
			int proc_idx = cmd.find_first_of("012345");

			if(proc_idx >= 0)
				my_id = (int)cmd[proc_idx] - '0';
			server_on = 1;
			pthread_t server_t;
			pthread_create(&server_t, NULL, create_server_callback, this);
		}
	}
	else
	{
		cout<<"Invalid Command."<<endl;
	}

	}

	return NULL;
}

void process::set_ts(string new_ts)
{
	int offset = 0;
	for(int i = 0; i<MAX_PROCS; i++)
	{
		ts[i] = stoi(new_ts.substr(2*i + offset, 2));
		if(ts[i] >= 10)
			offset += 1;
	}
}

void * process::multicast_send_callback(void * t)
{
	process * p = (process *) t;
	p->multicast_send(p->msg_send);
	pthread_exit(NULL);
}

void process::multicast_send(string & msg)
{
	usleep(300000);		// Added some delay for safe keeping
	
	if(msg.length() < 1)
		return;

	while(ts_lock == 1){
		usleep(100);
	};

	ts_lock = 1;

	ts[my_id]++;
	ts_lock = 0;

	for(int i = 0; i<MAX_PROCS; i++)
	{
		if(c_sockets[i] != -1)
		{
			// if(!fork()) // this is the child process
			// {
			// 	// signal(SIGINT, stop_server);
			// 	srand(time(NULL) - i*2);			// Re-seed to output random values for forked processes
			// 	delay_send(i, msg);
			// 	exit(0);
			// }

			while(can_send == 1){
				usleep(100);
			};

			can_send = 1;
			// this->send_id = i;
			process_t * p_t = new process_t;
			p_t->pid = i;
			p_t->p = this;

			pthread_t send_t;
			pthread_create(&send_t, NULL, delay_send_callback, p_t);
			can_send = 0;
		}
	}

	cout<<"Message "<<msg<<" sent successfully!"<<endl;
	can_send = 0;
}
// No delay in channel when sending.
void * process::delay_send_callback(void * t)
{
	process_t * p_t = (process_t *) t;
	process * p = p_t->p;
	int pid = p_t->pid;
	delete p_t;
	p_t = NULL;
	// process * p = (process *) t;
	p->delay_send(pid, p->msg_send);
	pthread_exit(NULL);
}

void process::delay_send(int dest_id, string send_msg)
{
	if(send_msg.length() >= 1 && c_sockets[dest_id] != -1) {
	// time_t t = chrono::system_clock::to_time_t(chrono::system_clock::now());
		add_timestamp(send_msg);
	// cout<<"Sent "<<'"'<<send_msg<<'"'<<" to process "<<dest_id<< " system time is "<<t<<endl;
		sleep(delay_channel());
		unicast_send(dest_id, send_msg);
	}
}
//Delay in the channel when receiving.
void * process::delay_receive_callback(void * t)
{
	process_t * p_t = (process_t *) t;
	process * p = p_t->p;
	int pid = p_t->pid;
	delete p_t;
	p_t = NULL;

	// process * p = (process *) t;
	string temp_msg;
	p->delay_receive(pid, temp_msg);
	if(p->msg_recv.find("exit") != -1)
	{
		p->close_client(pid);
	}
	pthread_exit(NULL);
}


void process::delay_receive(int src_id, string & recv_msg)
{
	unicast_receive(src_id, recv_msg);
	// sleep(delay_channel());
	if(recv_msg.length() >= 1) {
		int msg_ts[] = {0, 0, 0, 0};
		if((int)recv_msg.find('#') >= 0) 
		{
			get_timestamp(recv_msg, msg_ts);
			while(!check_timestamp(msg_ts, src_id)){
				usleep(100);
			};
		}
		// time_t t = chrono::system_clock::to_time_t(chrono::system_clock::now());
		cout<<"Received "<<'"'<<recv_msg<<'"'<<" from process "<<src_id<<endl;
	}
	return;
}

bool process::check_timestamp(int * msg_ts, int src_id)
{
	while(ts_lock == 1){
		usleep(100);
	};

	ts_lock = 1;
	bool ret = true;
	if(msg_ts[my_id] > ts[my_id])		// Invalid Message let it go through
	{
		ts_lock = 0;
		return true;
	}

	for(int i = 0; i<MAX_PROCS; i++)
	{
		if(i != src_id)
		{
			if(msg_ts[i] > ts[i])
			{
				ret = false;
				break;
			}
		}
		else
		{
			if( (msg_ts[i] != (ts[i] + 1)) )
			{
				ret = false;
				break;
			}
		}
	}
	if(!ret)
	{
		process_t * p_t = new process_t;
		p_t->pid = src_id;
		p_t->p = this;
		ts_lock = 0;
		pthread_t msg_t;
		pthread_create(&msg_t, NULL, delay_receive_callback, p_t);
		return ret;
	}

	ts[src_id]++;
	ts_lock = 0;
	// can_send = 1;
	return true;
}

int process::delay_channel()
{
	return ((rand() % 5) + 1);
}

// Negative 
void process::add_timestamp(string & send_msg)
{
	while(ts_lock == 1){
		usleep(100);
	};

	string t = "#";
	for(int i = 0; i<MAX_PROCS; i++)
	{
		t.append(to_string(ts[i]));
		if(i != MAX_PROCS - 1)
			t.append(",");
	}
	send_msg.append(t);
	// cout<<send_msg<<endl;
	ts_lock = 0;
	return;
}

void process::get_timestamp(string & recv_msg, int * msg_ts)
{
	int idx = recv_msg.find('#');

	int offset = 0;
	for(int i = 0; i<MAX_PROCS; i++)
	{
		// msg_ts[i] = (int)(recv_msg[idx + 2*i + 1] - '0');
		msg_ts[i] = stoi(recv_msg.substr(idx + 2*i + 1 + offset, 2));
		if(msg_ts[i] >= 10)
			offset += 1;
		// cout<<msg_ts[i]<<",";
	}
	// cout<<endl;
	recv_msg = recv_msg.substr(0, idx);
	return;
}

void process::unicast_send(int dest_id, string send_msg)
{
	int socket = c_sockets[dest_id];
	if(socket != -1)
	{
		// string send_msg;
		// cout<<"Server :";
		// getline(cin, send_msg);
		char *s_msg = &send_msg[0u];

		if(send(socket, s_msg, send_msg.length(), 0) == -1)
		{
			cout<<"Send error"<<endl;
			ts[my_id]--;
			perror("send");
		}
		// cout<<"Message sent to "<<dest_id<<endl;
	}
	return;
}

void process::unicast_receive(int src_id, string & recv_msg)
{
	int socket = c_sockets[src_id];
	char recv_msg_[30];
	memset(recv_msg_, 0, 30);

	if(socket != -1)
	{
		int bytes_received;
		if((bytes_received = recv(socket, recv_msg_, 29, 0)) == -1)
		{
			cout<<"Error in Recv"<<endl;
			perror("recv");
			return;
		}
		if(recv_msg_[0] != '\0')
		{
			recv_msg = string(recv_msg_);
		}
	}

	return;
}


void process::close_client(int dest_id)
{
	int socket = c_sockets[dest_id];
	cout<<"Closing client connection with Process "<<dest_id<<endl;
	close(socket);
	c_sockets[dest_id] = -1;
	num_connect--;
}

void * process::create_server_callback(void * t)
{
	process * p = (process *) t;
	p->create_server(NULL);
	pthread_exit(NULL);
}

void * process::create_server(void * t)
{
	string port = to_string(BASE_PORT + my_id);
	char *port_ = &port[0u];
	create_listen_server((char *)port_, server_response);
	pthread_exit(NULL);
}

void * process::get_data_callback(void * t)
{
	process_t * p_t = (process_t *) t;
	process * p = p_t->p;
	int pid = p_t->pid;
	delete p_t;
	p_t = NULL;
	// process * p = (process *) t;
	// int client_id = p->num_connect - 1;
	p->get_data(pid);
	pthread_exit(NULL);
}
// Change p_id to id informed by the process that is connected to it.
void * process::get_data(int p_id)
{
	// int client_id = *((int *)p_id);
	int client_id = p_id;
	// int client_id = new_id;
	// int socket = c_sockets[client_id];
	// int bytes_received;
	// char rec_msg[30];
	// memset(rec_msg, 0, 30);

	while(c_sockets[client_id] != -1)
	{
		// if((bytes_received = recv(socket, rec_msg, 29, 0)) == -1)
		// {
		// 	perror("recv");
		// 	pthread_exit(NULL);
		// }
		string msg;
		delay_receive(client_id, msg);
		// msg.copy(rec_msg, msg.length(),0);
		// if(rec_msg[0] != '\0')
		// {
		// 	cout<<endl<<"Process "<<client_id<<": "<<rec_msg<<endl;
		// 	memset(rec_msg, 0, 30);
		// }
	}
	close_connection(client_id);
	num_connect--;
	pthread_exit(NULL);

}


int process::server_response(int socket_to_client)
{
	char buffer[MAX_MSG_L];
	string connect = "Connection Setup with Process";
	char *connect_ = &connect[0u];
	strcpy(buffer, connect_);
	if(send(socket_to_client, buffer, MAX_MSG_L, 0) == -1)
	{
		perror("send");
		return -1;
	}
	memset(buffer, 0, 30);
	if(recv(socket_to_client, buffer, MAX_MSG_L, 0) == -1)
		return -1;

	string response = string(buffer);
	int new_id = (int)response[response.find_first_of("012345")] - '0';
	string connect_response = "Connection Setup with Process " + to_string(new_id);
	cout<<connect_response<<endl;
	return new_id;
}

process::process()
{
	// this->my_id = pid;
	ts = new int(MAX_PROCS);
	for(int i = 0; i<MAX_PROCS; i++)
	{
		c_sockets[i] = -1;		// No client connections yet.
		ts[i] = 0;
	}
	num_connect = 0;
	server_on = 0;
	my_socket = -1;
	can_send = 0;
	ts_lock = 0;
	// socket_lock = 0;
	srand (time(NULL));

	// pthread_t cmd_t;
	// pthread_create(&cmd_t, NULL, get_command_callback, this);
	// get_command(NULL);
}

process::~process()
{
	killprocess();
}

int process::connect_to_server(int pid)
{
	string port = to_string(BASE_PORT + pid);
	char *port_ = &port[0u];
	int server_socket = connect_to_server_(IP, port_);
	if(server_socket < 0)
		return -1;
	c_sockets[pid] = server_socket;
	cout<<"Connected to process "<<pid<<endl;
	num_connect++;
	pthread_t t_recv;
	// int dest_id = pid;
	// new_id = pid;
	unicast_send(pid, "my_id is " + to_string(my_id));
	process_t * p_t = new process_t;
	p_t->pid = pid;
	p_t->p = this;
	pthread_create(&t_recv, NULL, get_data_callback, p_t);
	return 0;
}


void process::close_connection(int pid)
{
	close(c_sockets[pid]);
	return;
}

void process::killprocess()
{
	for(int i = 0; i<MAX_PROCS; i++)
	{
		if(c_sockets[i] != -1)
			close_connection(i);
	}
	delete [] ts;
	close_server();
}

void process::close_server()
{
	if(server_on == 1)
	{
		close(my_socket);
		server_on = 0;
	}
}


//---------------------------------------------------------------------------------
// get sockaddr, IPv4 or IPv6:
void* process::get_in_addr(struct sockaddr* sa)
{
	if(sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	else
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void process::sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

//NOTE: only root can bind ports < 1024.

int process::create_listen_server(const char* port_to_bind,  int(*handleConnection)(int))
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	// struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if((rv = getaddrinfo(NULL, port_to_bind, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("server: socket");
			continue;
		}
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			return -1;
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if(p == NULL)
	{
		fprintf(stderr, "server: failed to bind\n");
		return -1;
	}
	freeaddrinfo(servinfo); // all done with this structure

	if(listen(sockfd, 10) == -1) //queue up to 10 un-accept()ed connect()s
	{
		perror("listen");
		return -1;
	}

	my_socket = sockfd;

	// sa.sa_handler = sigchld_handler; // reap all dead processes
	// sigemptyset(&sa.sa_mask);
	// sa.sa_flags = SA_RESTART;
	// if(sigaction(SIGCHLD, &sa, NULL) == -1)
	// {
	// 	perror("sigaction");
	// 	return -1;
	// }

	printf("server: bound to port %s and waiting for connections...\n", port_to_bind);

	while(server_on == 1) // main accept() loop
	{
		sin_size = sizeof their_addr;
		new_fd = accept(my_socket, (struct sockaddr*)&their_addr, &sin_size);
		if(new_fd == -1)
		{
			perror("accept");
			continue;
		}
		
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);
		// cout<<"Total no. of  connections: "<<num_connect<<endl;

		// TODO: Specify the process id that connects to this one.

		num_connect++;
		pthread_t t_recv;

		new_id = handleConnection(new_fd);
		c_sockets[new_id] = new_fd;

		process_t * p_t = new process_t;
		p_t->pid = new_id;
		p_t->p = this;

		pthread_create(&t_recv, NULL, get_data_callback, p_t);
				//(alternatively, we could create a thread instead of forking)
		// if(!fork()) // this is the child process
		// {
		// 	close(sockfd); // child doesn't need the listener
		// 	handleConnection(new_fd);
		// 	close(new_fd);
		// 	exit(0);
		// }
		// close(new_fd);  // parent doesn't need this
	}
	pthread_exit(NULL);
}


// Open a new connection to a listening server.
int process::connect_to_server_(const char* domain_or_ip, const char* port_string)
{
	struct addrinfo hints, *servinfo, *p;
	int rv, sockfd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((rv = getaddrinfo(domain_or_ip, port_string, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if(p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return -1;
	}
	// else
	// {
	// 	int server = stoi(string(domain_or_ip)) - BASE_PORT;
	// 	printf("Connected to process %d\n", server);
	// }
	freeaddrinfo(servinfo); // all done with this structure
	
	return sockfd;
}

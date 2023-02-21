#include <stdio.h>
#include<string>
#include <stdlib.h>
#include <pthread.h>
#if 1
#include <sys/socket.h>
#endif

//Causing issue with compilation in cpp,  added header netinet/in.h
#if 0 
#include <linux/in.h> 
#endif
#include <unistd.h>

#if 1
#include  <netinet/in.h>
#endif

// for broadcast
#include <netdb.h>

#include<bits/stdc++.h>
using namespace std;

typedef struct
{
	int sock;
	struct sockaddr address;
	socklen_t  addr_len;
} connection_t;

bool flag = false;

void* listner(void*);

class message{

        public:

        int uid_sender;
        int uid_max;
        int d;
        int d_max;
        int round;

       void init(int uid_sender, int uid_max, int d, int d_max, int round);
       void print();
};


void message::init(int s_uid_sender, int s_uid_max, int s_d, int s_d_max, int s_round){

        uid_sender = s_uid_sender;
        uid_max = s_uid_max;
        d = s_d;
        d_max = s_d_max;
        round = s_round;
}


void message::print(){
	cout << "uid: " << uid_sender << "uid_max: " << uid_max << "d: " << d << "d_max: " << d_max << "round: " << round << "\n";
}

class node{
	public: 
	
	int port;
	
	vector<int> neighbours;

	node(int port);
	node(int port, vector<int> neighbour_list);
	int broadcast(string destination, int port_1, string msg);
	void start_listner(void);
};


node::node(int port){
	port = port;
}

node::node(int port, vector<int> neighbour_list){
	port = port;
}

int node::broadcast(string destination, int port_1,string msg){

        int port = port_1;
        int sock = -1;
        struct sockaddr_in address;
        struct hostent * host;
        int len;

        /* create socket */
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock <= 0){
                //cout << " error: cannot create socket"<<  sock << "\n";
                return -3;
        }

        /* connect to server */
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        host = gethostbyname(destination.c_str());

        if (!host){
                cout << "error: unknown host" << port << host << "\n";
                return -4;
        }

        memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);

        if (connect(sock, (struct sockaddr *)&address, sizeof(address))){
                cout << " error: cannot connect to host" << host << port << "\n";
                return -5;
        }

        /* send text to server */
        len = msg.size();
        write(sock, &len, sizeof(int));
        write(sock, msg.c_str(), len);

        /* close socket */
        close(sock);

        /*Success*/
        return 0;
}

void node::start_listner(void){
	pthread_t thread;
//	sleep(3);
	pthread_create(&thread, 0, &listner, NULL);
	pthread_detach(thread);
	
}

void* listner(void*){
	
	int sock = -1;
	connection_t * connection;
	struct sockaddr_in address;
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(5678);

        if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
        {
                //fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		cout << "Cannot bind to port "  << "\n";
        //      return -4;
        }

        /* listen on port */
        if (listen(sock, 5) < 0)
        {
//              fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		cout << "cannot listen on port"  << "\n";
        //      return -5;
        }

	

	while(true){
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);

		/*TODO: call pthread_exit() when leader election is done*/
		if(connection->sock) {

			char * buffer;
			int len;
			connection_t * conn;
			long addr = 0;
	
			/* read length of message */
			read(connection->sock, &len, sizeof(int));
			if (len > 0)
			{
				addr = (long)((struct sockaddr_in *)&connection->address)->sin_addr.s_addr;
				buffer = (char *)malloc((len+1)*sizeof(char));
				buffer[len] = 0;

				/* read message */
				read(connection->sock, buffer, len);

				/* print message */
#if 1
				printf("%d.%d.%d.%d: %s\n",
						(int)((addr      ) & 0xff),
						(int)((addr >>  8) & 0xff),
						(int)((addr >> 16) & 0xff),
						(int)((addr >> 24) & 0xff),
						buffer);

#endif		

				cout << buffer << "\n";
				// fetch the values
				vector <int> rcv;
				string buff = buffer;
				stringstream strstrm(buff);
				string word;
				while(getline(strstrm,word,' ')){
					rcv.push_back(stoi(word));

				}
				// well formed message
				if(rcv.size() == 5){
		
		#if 0
					for (const auto& i: rcv){
						cout << i << ' ';
					}
		#endif
					message new_message;
					new_message.init(rcv[0],rcv[1],rcv[2],rcv[3],rcv[4]);
					new_message.print();
					// now push this message to one of the buffers
				}

				


				free(buffer);
			}

			/* close socket and clean up */
			close(connection->sock);
			free(connection);
			//pthread_exit(0);
		}
	}

}

int main(){
	
	node n1(5678);
	
	n1.start_listner();
	int i = 0 ;

	sleep(5);
	int uid = 342, d = 0, dmax = 10, rno=1,uid_max = INT_MAX;  
	while(1){
		string s_uid = to_string(uid);
		string s_d = to_string(d+i);
		string s_dmax = to_string(dmax);
		string s_rno = to_string(rno);
		string s_uid_max= to_string(uid_max);

		string s = s_uid + " " + s_uid_max + " "+ s_d + " " + s_dmax + " " + s_rno;
		i++;
	//	cout << s << endl;
		n1.broadcast("10.176.69.33",1234,s);
		sleep(1);
	}	
}


#if 0
int main(int argc, char ** argv)
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	pthread_t thread;

	/* check for command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		return -1;
	}

	/* obtain port number */
	if (sscanf(argv[1], "%d", &port) <= 0)
	{
		fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
		return -2;
	}

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		return -4;
	}

	/* listen on port */
	if (listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);
	
//	connection->sock = -1;
	while (1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
#if 1
		if (connection->sock <= 0)
		{
			free(connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			pthread_create(&thread, 0, listner, (void *)connection);
			pthread_detach(thread);
		}
#endif		
	}
//	pthread_create(&thread, 0, listner, (void *)connection);
//	pthread_detach(thread);

	cout << "Finished Listening\n";
	
	return 0;
}
#endif


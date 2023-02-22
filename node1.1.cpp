#include <stdio.h>
#include<string>
#include <stdlib.h>
#include <pthread.h>
#include<chrono>
#include<thread>
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
        int pulse;

       void init(int uid_sender, int uid_max, int d, int d_max, int round);
       void print();
};


void message::init(int s_uid_sender, int s_uid_max, int s_d, int s_d_max, int s_round){

        uid_sender = s_uid_sender;
        uid_max = s_uid_max;
        d = s_d;
        d_max = s_d_max;
        pulse = s_round;
}


void message::print(){
	cout << "uid: " << uid_sender << "uid_max: " << uid_max << "d: " << d << "d_max: " << d_max << "pulse: " << pulse << "\n";
}


/* Todo : define the constructor as node::node() is referenced in pelege_le*/
class node{
	public: 
	int uid;	
	int port;
	peleg_le elect;  // peleg's leader election 
	bool initialize;
	vector<int> neighbours;
	node();
	node(int port);
	node(int port, vector<int> neighbour_list);
	int broadcast(string destination, int port_1, string msg);
	void start_listner(void);
	void leaderelectionMsgProcess(message msg);
	void start_pelegs_le();
};

void node::LeaderElectionMsgProcess(message msg){
	cout << uid << " recieves " << msg;
	if (msg.){
		// msg in the same round
		if(msg.pulse == elect.pulse){
			elect.msg_list.push_back(msg);
		}
		// msg in round + 1 => add to buffer 
		else if(msg.pulse == elect.pulse + 1){
			elect.buffered_msg.push_back(msg);
		}
		else {
			cout << " Message exceeds the pulse/round number. TERMINATE!!!" << endl;
		}
	}
	else if(){
		if(elect.done == false){
			broadcast(); // broadcast a success message
			elect.done == true;
			cout << "Process is done" << endl;
		}
	}
}

void node::start_pelegs_le(){
	while(elect.done =false && initialize){ //initialize has to be set to false 
											// function Node
		broadcast(); //broadcast the round number
		bool rnd_done = false; 
		while(rnd_done == false && elect.done == false){
			if(elect.execute_peleg == true){
				broadcast(); // broadcast success msg
				elect.done = true;
			}
			rnd_done = true;
		}
		// TODO:  try catch mention exception e
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

node::node(){

}

node::node(int port){
	port = port;
}

node::node(int port, vector<int> neighbour_list){
	port = port;
}

/* TODO: integrate the initialize variable in the function*/
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


class peleg_le{
	
	public:
		node node_n;
		int pulse;
		int dist;
		int counter;
		int max;
		int candidate;
		vector<message> buffered_msg;
		vector<message> msg_list;
		bool done;

		peleg_le(node n);
		bool execute_peleg();
};


peleg_le::peleg_le(node n){
	
	node_n = n;
	buffered_msg.clear();
	msg_list.clear();
	pulse = 0;
	dist = 0;
	counter = 0;
	done = false;
	max = node_n.uid;
	candidate = 1;
}

bool peleg_le::execute_peleg(){
    pulse += 1;
    message max_msg = msg_list[0];
    vector <message> temp_list;
    temp_list.push_back(max_msg);
    for (int i =0; i < msg_list.size(); i++){
        message nxt_msg = msg_list[i];
        if(nxt_msg.uid_max > max_msg.uid_max){
            max_msg = nxt_msg;
            temp_list.clear();
        }
        else if(nxt_msg.uid_max > max_msg.uid_max){
            temp_list.push_back(nxt_msg);
        }
    }
    if ( max < max_msg.uid_max){
        candidate = 0;
        max = max_msg.uid_max;
        dist = pulse;
    }
    else if(max > max_msg.uid_max){
        counter =1 ; // uid remains the same for 2 rounds
    }
    else {
        int z = INT_MIN;
        for (int i =0; i<temp_list.size();i++){
            if(temp_list[i].d > z){
                z = temp_list[i].d;
            }
        }
        if (z > dist){
            dist = z;
            counter = 0; // distance was changed 
        }
        else if( z == dist){
            counter ++;
        }
    }
    msg_list.clear();
    for (auto k = buffered_msg.begin(); k!=buffered_msg.end();k++){
        if((k) != buffered_msg.end()){
            message nxt_msg = (*k);
            if(nxt_msg.pulse == pulse){
                msg_list.push_back(nxt_msg);
                buffered_msg.erase(k);
            }
        }
    }
    if(candidate == 1 && counter == 2){
        return true;
    }
    return false;
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
		cout << s << endl;
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


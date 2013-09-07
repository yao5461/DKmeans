#include "controller.h"

Controller::Controller() {
    this->init();
}

Controller::~Controller() {
    
}

Controller::Controller(const string host, short unsigned int port) {
    this->m_myHost = host;
    this->m_myPort = port;
}

void Controller::init() {
    this->m_dataDimension = 3;
    this->m_numOfCluster = 5;
    this->m_lenOfData = 10;
    
    this->m_myHost = "127.0.0.1";
    this->m_myPort = 9990;
    
    this->m_numOfMapper = 3;
    
    //should be modified
    this->m_currStatus = 0;
}

bool Controller::runTask() {
    
    //step 1: create server
    cout<<"try to create a server...."<<endl;
    if(!this->establishServer()) {
	cout<<"#Failed, end task!"<<endl;
	return false;
    }
    
    cout<<"server created successful!\nwaiting for clients..."<<endl;
    
    //step 2: wait for clients
    if(!this->waitAllClients()) {
	cout<<"#Wait clients time out! End task!"<<endl;
	return false;
    }
    
    //step 3:
}

bool Controller::establishServer() {
    bool flag = true;
    
    try{
	this->m_ptrServerScoket = new TCPServerSocket(this->m_myHost, this->m_myPort, this->m_numOfMapper);
    } catch (ClassException<TCPServerSocket> e) {
	flag = false;
	cerr<<"#Error:Failed to create server!"<<endl;
    }
    
    return flag;
}


bool Controller::waitAllClients() {
    pthread_t * listPthread = new pthread_t[this->m_numOfMapper];
    //pthread_gr
  
    for(int i = 0; i < this->m_numOfMapper; i++) {
	TCPSocket *client;
	
	try{
	    client = this->m_ptrServerScoket->accept();
	} catch(ClassException<TCPServerSocket> e) {
	    cerr<<"#Error: can't connect with clients"<<endl;
	    return false;
	}
	
	cout<<"A client connected! client address: "<<client->getForeignAddress()<<"\tclient port: "<<client->getForeignPort()<<endl;
	void * ret;
	int err = pthread_create(&listPthread[i], NULL, Controller::clientThread, client);
	//err = pthread_join(listPthread[i], &ret);
    }
   
    return true;
}

void* Controller::clientThread(void* arg) {
    TCPSocket *client = static_cast<TCPSocket*>(arg);  
    cout<<"Thread :"<<client->getForeignPort()<<endl;
    
    char *recv = new char[1024];
    
    //test code, test connect
    for(int i = 0; i < 100000; i++) {
	if(client->getForeignPort() == 9991) {
	  client->send("AAAA", 4);
	} else {
	  client->send("BBBB", 4);
	}
	client->recv(recv, 2);
	cout<<"received: "<<recv<<endl;
    }
    
    client->send("End!", 4);
    cout<<"Finished!"<<client->getForeignPort()<<endl;
}








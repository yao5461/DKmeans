#include "controller.h"

//define global variable
//store the status indormation of main thread and client threads
int m_currMainThreadStatus;
int* m_currClientThreadsStatus;

//mutex variable
pthread_mutex_t m_mainStatusMutex;
pthread_mutex_t m_clientStatusMutex;
pthread_mutex_t m_updateMutex;


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
    
    //init the init status of each thread
    m_currMainThreadStatus = NOP;
    m_currClientThreadsStatus = new int[this->m_numOfMapper];
    for(int i = 0; i < this->m_numOfMapper; i++) {
	m_currClientThreadsStatus[i] = NOP;
    }
    
    //init mutex
    pthread_mutex_init(&(m_mainStatusMutex), NULL);
    pthread_mutex_init(&(m_clientStatusMutex), NULL);
    pthread_mutex_init(&(m_updateMutex), NULL);
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
    
    bool working = true;
    
    //step 3:loop to classify.
    while(working) {
	int status;
	//get my status
	pthread_mutex_lock(&(m_mainStatusMutex));
	status = m_currMainThreadStatus;
	pthread_mutex_unlock(&(m_mainStatusMutex));

	pthread_mutex_lock(&(m_mainStatusMutex));	
	switch(status) {
	  case NOP:
	    if(this->isAllClientsStatus(INIT)) {
		m_currMainThreadStatus = INIT;
	    }
	    break;
	  case INIT:
	    if(this->isAllClientsStatus(WAIT)) {
		m_currMainThreadStatus = CAL;
	    }
	    break;
	  case CAL:
	    if(this->isAllClientsStatus(WAIT)) {
		if(this->canStopTask()) {
		    m_currMainThreadStatus = END;
		}
	    }
	    break;
	  case END:
	    working = false;
	    break;
	  default:
	    break;
	}
	pthread_mutex_unlock(&(m_mainStatusMutex));
    }
}

bool Controller::isAllClientsStatus(int tartgetStatus) {
    bool flag = true;
    
    pthread_mutex_lock(&(m_clientStatusMutex));
    for(int i = 0; i < this->m_numOfMapper; i++) {
	if(m_currClientThreadsStatus[i] != tartgetStatus) {
	    flag = false;
	    break;
	}
    }
    pthread_mutex_unlock(&(m_clientStatusMutex));
    
    return flag;
}

bool Controller::canStopTask() {
    //should be added
    
    return false;
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
  
    for(int i = 0; i < this->m_numOfMapper; i++) {
	TCPSocket *client;
	
	try{
	    client = this->m_ptrServerScoket->accept();
	} catch(ClassException<TCPServerSocket> e) {
	    cerr<<"#Error: can't connect with clients"<<endl;
	    return false;
	}
	
	cout<<"A client connected! client address: "<<client->getForeignAddress()<<"\tclient port: "<<client->getForeignPort()<<endl;
	//void * ret;
	int err = pthread_create(&listPthread[i], NULL, Controller::clientThread, client);
	//err = pthread_join(listPthread[i], &ret);
    }
   
    return true;
}

void* Controller::clientThread(void* arg) {
    //get client tcp socket object
    TCPSocket *client = static_cast<TCPSocket*>(arg); 
    unsigned short clientPort = client->getForeignPort();
    
    cout<<"Thread :"<<clientPort<<endl;
        
    //init this client thread status chage status into INIT
    int index = (clientPort % 10) - 1;
    pthread_mutex_lock(&(m_clientStatusMutex));
    m_currClientThreadsStatus[index] = INIT;
    pthread_mutex_unlock(&(m_clientStatusMutex));
    
    //init flag
    bool working = true;
    int status;
    
    //loop to do task
    while(working) {
	
	//get current status of process
	pthread_mutex_lock(&(m_mainStatusMutex));
	status = m_currMainThreadStatus;
	pthread_mutex_unlock(&(m_mainStatusMutex));
        
	//change client thread status or communicate with client according main thread status
	pthread_mutex_lock(&(m_clientStatusMutex));
	
	switch(status) {
	  case WAIT:
	    //do nothing, main thread has no status like this.
	    break;
	  case END:
	    working = false;
	    break;
	  case INIT:
	    //send init information to client.
	    sendBasicInfoToClient();
	    break;
	  case CAL:
	    //ask client to classify the data.
	    askClientClassifyData();
	    break;
	  case NOP:
	    //There is no status in main thread. Client thread do nothing.
	    break;
	  default:
	    break;
	}
	
	pthread_mutex_unlock(&(m_clientStatusMutex));
    }
}

bool Controller::sendBasicInfoToClient() {
    //should be added 
    return false;
}

void Controller::askClientClassifyData() {
    //should be added
}










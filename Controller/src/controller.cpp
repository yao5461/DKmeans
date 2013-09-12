#include "controller.h"

//define global variable
//store the status indormation of main thread and client threads
int m_currMainThreadStatus;
int* m_currClientThreadsStatus;

//mutex variable
pthread_mutex_t m_mainStatusMutex;
pthread_mutex_t m_clientStatusMutex;
pthread_mutex_t m_updateMutex;

//store new cnetroids after calculate once
map< int, vector<double> > m_newCentroids;
map< int, int > m_newCentroidsDataNum;
//store the information of centroids
map< int, vector<double> > m_currCentroids;

//a flag to record whether has been classified
bool m_hasNoClassified;

//store the command between server and clients
vector<string> m_commands;
//int m_lenOfCommand;

//basic information
int m_dataDimension;
int m_numOfCluster;
int m_lenOfData;

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
    m_dataDimension = 3;
    m_numOfCluster = 3;
    m_lenOfData = 12;
    
    this->m_myHost = "127.0.0.1";
    this->m_myPort = 5550;
    
    this->m_numOfMapper = 3;
    
    this->m_sourceDataFile = "Data.txt";
    this->m_configurationFile = "configuration.conf";
    
    //init the init status of each thread
    m_currMainThreadStatus = INIT;
    m_currClientThreadsStatus = new int[this->m_numOfMapper];
    for(int i = 0; i < this->m_numOfMapper; i++) {
	m_currClientThreadsStatus[i] = INIT;
    }
    
    //init mutex
    pthread_mutex_init(&(m_mainStatusMutex), NULL);
    pthread_mutex_init(&(m_clientStatusMutex), NULL);
    pthread_mutex_init(&(m_updateMutex), NULL);
    
    //init commands
    //m_lenOfCommand = 4;           //the length of each command
    m_commands.push_back("sen1");   //send basic information to client
    m_commands.push_back("sen2");   //send centroids information to client
    m_commands.push_back("sen3");   //send data to client
    m_commands.push_back("ask1");   //ask client to classify
    m_commands.push_back("ask2");   //ask client to return output information
    m_commands.push_back("ask3");   //ask client to client
    
    //init map store space
    vector<double> temp;
    for(int i = 0; i < m_dataDimension; i++) {
	temp.push_back(0);
    }
    
    for(int i = 0; i < m_numOfCluster; i++) {
	m_newCentroids.insert(map< int, vector<double> >::value_type(i, temp));
	m_currCentroids.insert(map< int, vector<double> >::value_type(i, temp));
	m_newCentroidsDataNum.insert(map< int, int >::value_type(i, 0));
    }
    
    m_hasNoClassified = false;
}

bool Controller::runTask() {
    //step 0: read data and calculate the init centroids
    this->readDataFromFile();
    
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

	//cout<<"main thread status: "<<status<<endl;
	//clean the store space
	vector<double> tempVector;
	for(int i = 0; i < m_dataDimension; i++) {
	    tempVector.push_back(0);
	}
	pthread_mutex_lock(&(m_updateMutex));
	for(int i = 0; i < m_numOfCluster; i++) {
	    m_newCentroids[i] = tempVector;
	    m_newCentroidsDataNum[i] = 0;
	}
	pthread_mutex_unlock(&(m_updateMutex));
	
	pthread_mutex_lock(&(m_mainStatusMutex));	
	switch(status) {
	  case NOP:
	    break;
	  case INIT:
	    if(this->isAllClientsStatus(WAIT)) {
		m_currMainThreadStatus = CAL;
	    }
	    break;
	  case CAL:
// 	    if(this->isAllClientsStatus(WAIT)) {
// 		if(this->canStopTask()) {
// 		    m_currMainThreadStatus = END;
// 		} 
// 	    }
	    if(this->isAllClientsStatus(CAL)) {
		m_currMainThreadStatus = WAIT;
	    }
	    break;
	  case WAIT:
	    if(this->isAllClientsStatus(WAIT)) {
		if(this->canStopTask()) {
		    m_currMainThreadStatus = END;
		} else {
		    m_currMainThreadStatus = CAL;
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
    bool result = true;
    
    //test code to print new centroids
    cout<<"test new centroids"<<endl;
    for(int i = 0; i < m_numOfCluster; i++) {
	for(int j = 0; j < m_dataDimension; j++) {
	    cout<<m_newCentroids[i][j]<<' ';
	}
	cout<<"\t"<<m_newCentroidsDataNum[i]<<endl;
    }
    cout<<endl;
    
    pthread_mutex_lock(&(m_updateMutex));
    for(int i = 0 ; i < m_numOfCluster; i++) {
	for(int j = 0; j < m_dataDimension; j++) {
	    if(m_currCentroids[i][j] != m_newCentroids[i][j]) {
		result = false;
		break;
	    }
	}
	
	if(!result) {
	    break;
	}
    }
    
    //update centroids
    if(m_hasNoClassified && !result) {
      cout<<"!!!!!!!!!!!!!!!"<<endl;
	for(int i = 0; i < m_numOfCluster; i++) {
	    m_currCentroids[i] = m_newCentroids[i];
	}
    }
    pthread_mutex_unlock(&(m_updateMutex));
    
    return result;
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
    
    //send basic information to clients
    if(!sendBasicInfoToClient(client)) {
	cout<<"#Error: Send basic information failed!"<<endl;
	return NULL;
    }
    
    //send data to client
    //to be continue..
    
    //init this client thread status chage status into WAIT. Client is ready.
    int index = (clientPort % 10) - 1;
    pthread_mutex_lock(&(m_clientStatusMutex));
    m_currClientThreadsStatus[index] = WAIT;
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
	
	switch(status) {
	  case WAIT:
	    //change client thread status to idel
	    pthread_mutex_lock(&(m_clientStatusMutex));
	    if(m_currClientThreadsStatus[index] == CAL) {
		m_currClientThreadsStatus[index] = WAIT;
	    }
	    pthread_mutex_unlock(&(m_clientStatusMutex));
	    break;
	  case END:
	    working = false;
	    break;
	  case INIT:
	    //nothing to do
	    //cout<<"Thread-"<<clientPort<<": init status(wait)"<<endl;
	    break;
	  case CAL:
	    //change client thread status to busy
	    pthread_mutex_lock(&(m_clientStatusMutex));
	    m_currClientThreadsStatus[index] = CAL;
	    pthread_mutex_unlock(&(m_clientStatusMutex));
	    
	    //ask client to classify the data.
	    askClientClassifyData(client);
	    
	    break;
	  case NOP:
	    //There is no status in main thread. Client thread do nothing.
	    break;
	  default:
	    break;
	}
    }
}

bool Controller::sendBasicInfoToClient(TCPSocket *client) {
    if(!sendCommand(client, 0)) {
	return false;
    }
    
    char *recvMsg = new char[1024];
    string recvFB;
    
    //send basic message
    stringstream sendStr;
    sendStr<<m_dataDimension<<'%'<<m_numOfCluster;
    
    //send it
    try{
	client->send(sendStr.str().c_str(), sendStr.str().length());
    } catch(ClassException<Socket> e) {
      	cout<<"#Error: send data failed! Message:\n\t\t"<<e.what()<<endl;
	return false;
    }
    
    //receive the message send back from client
    try{
	client->recv(recvMsg, 2);
	recvFB = recvMsg;
    } catch(ClassException<Socket> e) {
	cout<<"#Error4: Thread-"<<client->getForeignPort()<<" receive feedback failed! Message:\n\t\t"<<e.what()<<endl;
	return false;
    }
    
    //if client receive failed. return false
    if("OK" != recvFB) {
	return false;
    }
    
    cout<<"Thread-"<<client->getForeignPort()<<": basic data has been sent!"<<endl;
    
    delete recvMsg;
    
    return true;
}

bool Controller::askClientClassifyData(TCPSocket *client) {
    char *recvMsg = new char[1024];
    string strRecv;
	  
    //step 1: send centroids to clients
     if(!sendCommand(client, 1)) {
	return false;
    }
    
    //get current centroids information
    map< int, vector<double> > copyCurrCentroids;
    pthread_mutex_lock(&(m_updateMutex));
    copyCurrCentroids = m_currCentroids;
    pthread_mutex_unlock(&(m_updateMutex));
    
    //send centroids to clients
    for(int i = 0; i < m_numOfCluster; i++) {
	stringstream sendStr;
	
	for(int j = 0; j < m_dataDimension; j++) {
	    sendStr<<copyCurrCentroids[i][j]<<'%';
	}
	
	//cout<<"Thread-"<<client->getForeignPort()<<"centroids try to send centroids-"<<i<<endl;
	
	//send it
	try {
	    client->send(sendStr.str().c_str(), sendStr.str().length());
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: send error when send current centroids information!\n\t\t"<<e.what()<<endl;
	    return false;
	}
	
	//receive feed back. If it is successful, go on.
	try{
	    client->recv(recvMsg, 2);
	    strRecv = recvMsg;
	} catch(ClassException<Socket> e) {
	    cout<<"#Error1: Thread-"<<client->getForeignPort()<<"receive feedback failed! Message:\n\t\t"<<e.what()<<endl;
	    return false;
	}
	
	if("OK" != strRecv) {
	    cout<<"client-"<<client->getForeignPort()<<": send current controids centroids-"<<i<<" failed!"<<endl;
	    return false;
	}
	
	//cout<<"Thread-"<<client->getForeignPort()<<"centroids centroids-"<<i<<"has been sent!"<<endl;
    }
    cout<<"client-"<<client->getForeignPort()<<": finish to send current centroids task"<<endl;
    
    //step 2: send command to let client to do classify
    cout<<"Ask Client-"<<client->getForeignPort()<<": to do classify!"<<endl;
    if(!sendCommand(client, 3)) {     
	return false;
    }
    
    try{
	client->recv(recvMsg, 2);
	strRecv = recvMsg;
    } catch(ClassException<Socket> e) {
	cout<<"#Error2: Thread-"<<client->getForeignPort()<<"receive feedback failed! Message:\n\t\t"<<e.what()<<endl;
	return false;
    }
    
    if("OK" != strRecv) {
	cout<<"client-"<<client->getForeignPort()<<": do classify task failed!"<<endl;
	return false;
    }
    
    cout<<"client-"<<client->getForeignPort()<<": finish classify task"<<endl;
    
    //step 3: ask client send back output data
    cout<<"Ask Client-"<<client->getForeignPort()<<": to send back output data!"<<endl;
    if(!sendCommand(client, 4)) {
	return false;
    }
    
    //define the temp variables
    map<int, vector<double> > tempNewCentroids;
    map<int, int> tempCentrooidsDataNum;
    
    //init the temp maps
    vector<double> temp;
    for(int i = 0; i < m_dataDimension; i++) {
	temp.push_back(0);
    }
    for(int i = 0; i < m_numOfCluster; i++) {
	tempNewCentroids.insert(map< int, vector<double> >::value_type(i, temp));
	tempCentrooidsDataNum.insert(map< int, int >::value_type(i, 0));
    }
    
    cout<<"Thread-"<<client->getForeignPort()<<": try to receive the outout data from client!"<<endl;
    
    //receive new centroids information from clients
    for(int i = 0; i < m_numOfCluster; i++) {
	//clean buffer
	delete recvMsg;
	recvMsg = new char[1024];
      
	cout<<"Thread-"<<client->getForeignPort()<<": try to receive number in cluster from client for cluster-"<<i<<endl;
        //receive number of data in new centroids
	try {
	    client->recv(recvMsg, 1024);         //receive
	    strRecv = recvMsg;			  //copy to string type
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: receive output data from clients failed!"<<endl;
	    return false;
	}
	
	cout<<"Thread-"<<client->getForeignPort()<<": receive data message: "<<strRecv<<endl;
	tempCentrooidsDataNum[i] = atoi(strRecv.c_str());  //convert to int 
	
	string feedback = "OKOK";
	
	try {
	    //client->send("OK", 2);		    //feedback
	    client->send(feedback.c_str(), feedback.length());
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: send feedback error!"<<endl;
	    return false;
	}
	
	//clean buffer
	delete recvMsg;
	recvMsg = new char[1024];
	
	cout<<"Thread-"<<client->getForeignPort()<<": Finished receive number on cluster from client for cluster-"<<i<<endl;
	cout<<"Thread-"<<client->getForeignPort()<<": Try to receive new centroids message from client for cluster-"<<i<<endl;
	
	//receive average new controids
	try {
	    client->recv(recvMsg, 1024);
	    strRecv = recvMsg;
	    client->send("OK", 2);	    
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: receive output data from clients failed!"<<endl;
	    return false;
	}
	cout<<"Thread-"<<client->getForeignPort()<<": Received message from client is: "<<strRecv<<endl;
	cout<<"Thread-"<<client->getForeignPort()<<": Finished receive new centroids message from client for cluster-"<<i<<endl;
	
	//parse received partly new centroids data, and save into map
	vector<double> temp;
	for(int j = 0; j < m_dataDimension; j++) {
	    int index = strRecv.find_first_of('%');     //search
	    string subStr = strRecv.substr(0, index);   //copy
	    strRecv.erase(0, index);                    //cut
	    temp.push_back( atof(subStr.c_str()) );		 //convert and store
	}
	
	//store into temp map
	tempNewCentroids[i] = temp;
    }
    
    delete recvMsg;
    cout<<"Thread-"<<client->getForeignPort()<<": finished to receive output data from client!"<<endl;
    
    //step 4: update new centeroid information
    pthread_mutex_lock(&(m_updateMutex));
    for(int i = 0; i < m_numOfCluster; i++) {
	vector<double> tempVector;
	for(int j = 0; j < m_dataDimension; j++) {
	    double tempValue = m_newCentroids[i][j] * m_newCentroidsDataNum[i] + tempNewCentroids[i][j] * tempCentrooidsDataNum[i];
	    tempVector.push_back(tempValue);
	}
	m_newCentroids[i] = tempVector;
	m_newCentroidsDataNum[i] += tempCentrooidsDataNum[i];
    }
    
    if(!m_hasNoClassified) {
	m_hasNoClassified = true;
    }
    pthread_mutex_unlock(&(m_updateMutex));
    
    return true;
}

bool Controller::sendCommand(TCPSocket* client, int commandIndex) {
    if(commandIndex >= m_commands.size()) {
	cout<<"#Error: the command index is out-of size."<<endl;
	return false;
    }
    
    char *recvMsg = new char[3];
    string recvFB;
    
    //send command to client at first
    try{
	client->send(m_commands[commandIndex].c_str(), m_commands[commandIndex].length());
    } catch(ClassException<Socket> e) {
	cout<<"#Error: send command failed! Message:\n\t\t"<<e.what()<<endl;
	return false;
    }
    
    //receive the message send back from client
    try{
	client->recv(recvMsg, 2);
	recvFB = recvMsg;
    } catch(ClassException<Socket> e) {
	cout<<"#Error3: Thread-"<<client->getForeignPort()<<" receive feedback failed! Message:\n\t\t"<<e.what()<<endl;
	return false;
    }
    
    //if client receive failed. return false
    if("OK" != recvFB) {
	return false;
    }
    
    cout<<"Thread-"<<client->getForeignPort()<<": command %"<<m_commands[commandIndex]<<"% has been sent!"<<endl;
    
    delete recvMsg;
    
    return true;
}

bool Controller::readDataFromFile() {
    ifstream inFile(this->m_sourceDataFile.c_str(), ios::in);
    if(!inFile) {
	cout<<"#Error: read source data file failed!"<<endl;
	return false;
    } else {
	cout<<"Open source data file successfully"<<endl;
    }
    
    //vector<double> temp;
    vector<double> max;
    vector<double> min;
    
    //init the buffer space
    for(int i = 0; i < m_dataDimension; i++) {
	max.push_back(0);
	min.push_back(0);
    }
    
    //read source data
    for(int i = 0; i < m_lenOfData; i++) {
	for(int j = 0; j < m_dataDimension; j++) {
	   double temp;
	   inFile>>temp;
	   
	   if(i == 0 ) {
		max[j] = temp;
		min[j] = temp;
	   } else if(temp < min[j]) {
		min[j] = temp;
	   } else if(temp > max[j]) {
		max[j] = temp;
	   } else {
		//do other thing
	   }
	}
    }
    
    //close file
    inFile.close();
    
    cout<<"Close the source data file"<<endl;
    
    //generate the init centroids by random
    for(int i = 0; i < m_numOfCluster; i++) {
	vector<double> tempPoint;
	for(int j = 0; j < m_dataDimension; j++) {
	   int tempValue = ceil(max[j] - min[j]);
	   double randValue = rand() % tempValue + min[j];
	   tempPoint.push_back( randValue );
	}
	m_currCentroids[i] = tempPoint;
	//m_newCentroids[i] = tempPoint;
    }
    
    //test code: output the current centroids generate in last step
    cout<<"Test print: init centroids: "<<endl;
    for(int i = 0; i < m_numOfCluster; i++) {
	for(int j =0; j < m_dataDimension; j++) {
	    cout<<m_currCentroids[i][j]<<'\t';
	}
	cout<<endl;
    }
}
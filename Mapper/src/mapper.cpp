#include "mapper.h"

Mapper::Mapper() {

}

Mapper::~Mapper() {

}

Mapper::Mapper(const string& host, unsigned short port) {
  this->m_myHost = host;
  this->m_myPort = port;
}

bool Mapper::runTask() {
    const string & foreignAddress = "127.0.0.1";
    unsigned short foreignPort = 9990;
  
    cout<<"try to connect to the server!"<<endl;
    //step1: connect to server
    if(!this->establishConnect(foreignAddress, foreignPort)) {
	cout<<"#Error:Can't connect to server!"<<endl;
	return false;
    }
    
    cout<<"connected! \nwait commands"<<endl;
    
    //step2: wait for command
    bool working = true;
    char *recvMsg = new char[1024];
    
    while(working) {
	if(this->m_ptrSocket->recv(recvMsg, 4) == -1) {
	    cout<<"#Error when receive command: "<<endl;
	    this->m_ptrSocket->send("NO", 2);
	    continue;
	} else {
	    this->m_ptrSocket->send("OK", 2);
	}
	
	bool isCommandCorrect = true;
	string recvCommand = recvMsg;
	cout<<"receive command: "<<recvCommand<<endl;
	
	//notify the command
	if("ask1" == recvCommand) {		//do classify
	    this->classifyData();
	} else if("ask2" == recvCommand) {	//send output data after classify to server
	    //do nothing now
	} else if("sen1" == recvCommand) {     //receive basic information
	    this->receiveBasicInfo();
	} else if("sen2" == recvCommand){	//receive the centroid information
	    this->receiveCentroidInfo();
	} else if("sen3" == recvCommand) {	//receive data from server
	    this->receiveData();
	} else if("ask3" == recvCommand) {     //wait server
	    //just wait server, do nothing
	} else if("End!" == recvCommand) {
	    working = false;   //end
	} else {
	    cout<<"#Warning: the command % "<<recvCommand<<" % is not defined!"<<endl;
	    
	}
	
    }
    
    this->m_ptrSocket->cleanUp();
    
}

bool Mapper::establishConnect(const string& foreignAddress, unsigned short foreignPort) {
    bool flag = true;
  
    try{ 	     
	//this->m_ptrSocket = new SocketStream<TCPSocket>(foreignAddress, foreignPort);
	this->m_ptrSocket = new TCPSocket();
	this->m_ptrSocket->setLocalPort(this->m_myPort);
	this->m_ptrSocket->connect(foreignAddress, foreignPort);
    } catch (ClassException<Socket> e) {
	flag = false;
	cerr<<"#Execption(when establish connection): "<<e.what()<<endl;
    }
    
    return flag;
}

void Mapper::classifyData() {
    cout<<"do classify"<<endl;
    
    try{
	this->m_ptrSocket->send("OK", 2);
    } catch(ClassException<Socket> e) {
	cout<<"#Error: when send back OK to sever\n\t\tMessage: "<<e.what()<<endl;
    }
}

bool Mapper::receiveBasicInfo() {
    char *recvMsg = new char[1024];
    
    //receive the data
    try{
	this->m_ptrSocket->recv(recvMsg, 1024);
    } catch(ClassException<Socket> e) {
	cerr<<"#Error: can't receive the data!"<<endl;
	return false;
    }
    
    //parse the data
    string recvData = recvMsg;
    int indexDivision = recvData.find_first_of('%');
    string strDataDiemsion = recvData.substr(0, indexDivision);
    string strNumCluster = recvData.substr(indexDivision+1, recvData.length()-indexDivision-1);
    
    cout<<"receive: "<<strDataDiemsion<<"\t\t"<<strNumCluster<<endl;
    
    this->m_dataDimension = atoi(strDataDiemsion.c_str());
    this->m_numOfCluster = atoi(strNumCluster.c_str());
    
    //send back
    try {
	this->m_ptrSocket->send("OK", 2);
    } catch(ClassException<Socket> e) {
	cerr<<"#Execption: send message error!"<<endl;
	return false;
    }
    
    return true;
}

bool Mapper::receiveCentroidInfo() {
    cout<<"receive Centroids"<<endl;
    return true;
}

bool Mapper::receiveData() {
    return true;
}





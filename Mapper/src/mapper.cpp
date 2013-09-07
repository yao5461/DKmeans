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
    //string recvMsg;
    char *recvMsg = new char[10];
    
    while(working) {
	if(this->m_ptrSocket->recv(recvMsg, 4) == -1) {
	    cout<<"#Error when receive command: "<<endl;
	    this->m_ptrSocket->send("Failed", 6);
	    continue;
	} else {
	    this->m_ptrSocket->send("OK", 2);
	}
	
	//notify the command
	if("Classify" == recvMsg) {
	    this->classifyData();
	} else if("Result" == recvMsg) {
	    //do nothing now
	} else if("BasicInfo" == recvMsg) {
	    this->receiveBasicInfo();
	} else if("CnetroidInfo" == recvMsg){
	    this->receiveCentroidInfo();
	} else if("Data" == recvMsg) {
	    this->receiveData();
	} else if("Wait" == recvMsg) {
	    //just wait server, do nothing
	} else if("End!" == recvMsg) {
	    working = false;   //end
	} else {
	    cout<<"#Warning: the command%"<<recvMsg<<"%is not defined!"<<endl;
	    //this->m_ptrSocket->send("goon", 4);
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

}

bool Mapper::receiveBasicInfo() {

}

bool Mapper::receiveCentroidInfo() {

}

bool Mapper::receiveData() {

}





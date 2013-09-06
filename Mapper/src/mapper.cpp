#include "mapper.h"

Mapper::Mapper() {

}

Mapper::~Mapper() {

}

Mapper::Mapper(string host, int port) {
  this->m_myHost = host;
  this->m_myPort = port;
}

bool Mapper::runTask() {
    const string & foreignAddress = "127.0.0.1";
    unsigned short foreignPort = 9990;
  
    //step1: connect to server
    if(!this->establishConnect(foreignAddress, foreignPort)) {
	cout<<"#Error:Can't connect to server!"<<endl;
	return false;
    }
    
    //step2: wait for command
    bool working = true;
    string recvMsg;
    
    
    while(working) {
	if(this->m_ptrSocket->recv(recvMsg) == -1) {
	    cout<<"#Error when receive command: "<<endl;
	    this->m_ptrSocket->send("Failed");
	    continue;
	} else {
	    this->m_ptrSocket->send("OK");
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
	} else if("End" == recvMsg) {
	    working = false;   //end
	} else {
	    cout<<"#Warning: the command is not defined!"<<endl;
	}
    }
    
}

bool Mapper::establishConnect(const string& foreignAddress, short unsigned int foreignPort) {
    bool flag = true;
  
    try{ 	     
	this->m_ptrSocket = new SocketStream<TCPSocket>(foreignAddress, foreignPort);
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





#include "mapper.h"

Mapper::Mapper() {

}

Mapper::~Mapper() {

}

Mapper::Mapper(const string& host, unsigned short port) {
    this->m_myHost = host;
    this->m_myPort = port;
  
    this->m_dataSourceFile = "Data.text";
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
    
    cout<<"wait commands"<<endl;
    
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
	    this->sendOutPutToServer();
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
    
    //clean store space
    //init the store space
    for(int i = 0; i < this->m_numOfCluster; i++) {
	for(int j = 0; j < this->m_dataDimension; j++) {
	    this->m_avgCentroid[i].push_back(0);
	}
	
	this->m_numInCluster[i] = 0;
    }
    
    //open file
    ifstream inFile(this->m_dataSourceFile.c_str(), ios::in);
    if(!inFile) {
	cout<<"#Error: read source data file failed!"<<endl;
	return ;
    }
    
    //read data and classify
    for(int i = 0; i < this->m_lenOfData; i++) {
	vector<double> temp;
	for(int j = 0; j < this->m_dataDimension; j++) {
	    inFile>>temp[i];
	}
	
	this->classifyOneData(temp);
    }
    
    //calculate the average cnetroids
    for(int i = 0; i < this->m_numOfCluster; i++) {
	for(int j = 0; j < this->m_dataDimension; j++) {
	    this->m_avgCentroid[i][j] /= this->m_numInCluster[i];
	}
    }
    
    //send result to server
    try{
	this->m_ptrSocket->send("OK", 2);
    } catch(ClassException<Socket> e) {
	cout<<"#Error: when send back OK to sever\n\t\tMessage: "<<e.what()<<endl;
    }
}

void Mapper::classifyOneData(std::vector< double > data) {
    double dist = -1;		//the minimum distance from point to centroid 
    int result = -1;		//the id which the point belongs to	
    
    //decide the mimumn distance and the which cluster
    for(int i = 0; i < this->m_numOfCluster; i++) {
	double temp = 0;
	for(int j = 0; j < this->m_dataDimension; j++) {
	    temp += pow((data[j]-this->m_centroids[i][j]), 2);
	}
	
	temp = sqrt(temp);
	
	if(temp < dist || dist == -1) {
	    dist = temp;
	    result = i;	
	    
	}
    }
    
    //store into the map
    this->m_numInCluster[result] += 1;
    for(int i = 0; i < this->m_dataDimension; i++) {
	this->m_avgCentroid[result][i] += data[i];
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
    cout<<"Try to receive current Centroids information!"<<endl;
    
    for(int i = 0; i < m_numOfCluster; i++) {
	char *recvMsg = new char[1024];
	string recvStr;
	
	try {
	    this->m_ptrSocket->recv(recvMsg, 1024);   //receive data
	    recvStr = recvMsg;				//convert data
	    this->m_ptrSocket->send("OK", 2);		//feed back
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: receive current centroids data-"<<i<<" failed!\n\t\tMessage: "<<e.what()<<endl;
	    return false;
	}
	
	for(int j = 0; j < this->m_dataDimension; j++) {
	    int index = recvStr.find_first_of('%');		//decide end of sub-string
	    string subStr = recvStr.substr(0, index);		//get sub-string
	    recvStr.erase(0, index+1);				//cut sub-string
	    this->m_centroids[i][j] = atof(subStr.c_str());		//convert and store
	}
	
	delete recvMsg;
    }
    
    cout<<"Finish to receive current Centroids information!"<<endl;
    
    return true;
}

bool Mapper::receiveData() {
    
    return true;
}

bool Mapper::sendOutPutToServer() {
    cout<<"Try to send output data to server!"<<endl;
    
    //send all new new centroids to server
    for(int i = 0; i < this->m_numOfCluster; i++) {
	stringstream sendStr;
	
	sendStr<<this->m_numInCluster[i];
	
	//send number of data in cluster-i
	if(!(this->sendOneData(sendStr))){
	    return false;
	}
	
	//cleand buffer
	sendStr.clear();
	
	//send this centroid
	for(int j = 0; j < this->m_dataDimension; j++) {
	    sendStr<<this->m_avgCentroid[i][j]<<'%';
	}
	
	//send number of this cluster
	if(!(this->sendOneData(sendStr))){
	    return false;
	}
    }
  
    cout<<"Finish sending the data!"<<endl;
  
    return true;
}

bool Mapper::sendOneData(const stringstream& sendStr) {
    char *recvMsg = new char[1024];
    string recvStr;
    
    //send data
    try {
	this->m_ptrSocket->send(sendStr.str().c_str(), sendStr.str().length());
    } catch(ClassException<Socket> e) {
	cout<<"#Error: can't send centroids data in cluster!"<<endl;
	return false;
    }
    
    //receive feed back
    try {
	this->m_ptrSocket->recv(recvMsg, 2);
	recvStr = recvMsg;
    } catch(ClassException<Socket> e) {
	cout<<"#Error: can't receive feedback from server!"<<endl;
	return false;
    }
    
    //whether it is ok
    if("OK" != recvStr) {
	cout<<"#Error: receive feedback from server failed!"<<endl;
	return false;
    }
    
    return true;
}






#include "mapper.h"

Mapper::Mapper() {

}

Mapper::~Mapper() {

}

Mapper::Mapper(const string& host, unsigned short port) {
    this->m_myHost = host;
    this->m_myPort = port;
  
    //this->init();
}

void Mapper::init() {
    //need to be modified.
    switch(this->m_myPort) {
      case 5551:
	this->m_dataSourceFile = "./data/Data1.txt";
	this->m_resultFile = "./data/Result1.txt";
	break;
      case 5552:
	this->m_dataSourceFile = "./data/Data2.txt";
	this->m_resultFile = "./data/Result2.txt";
	break;
      case 5553:
	this->m_dataSourceFile = "./data/Data3.txt";
	this->m_resultFile = "./data/Result3.txt";
	break;
      default:
	break;
    }
    
    this->m_lenOfData = 4;
    
    vector<double> temp;
    for(int i = 0; i < this->m_dataDimension; i++) {
	temp.push_back(0);
    }
    for(int i = 0; i < this->m_numOfCluster; i++) {
	this->m_avgCentroid.insert(map< int, vector<double> >::value_type(i, temp));
	this->m_centroids.insert(map< int, vector<double> >::value_type(i, temp));
	this->m_numInCluster.insert(map< int, int >::value_type(i, 0));
    }
}

bool Mapper::runTask() {
    const string & foreignAddress = "127.0.0.1";
    unsigned short foreignPort = 5550;
  
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
	    sleep((this->m_myPort % 10));
	    this->m_ptrSocket->send("OK", 2);
	}
	
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
    
    //open the file to write result;
    ofstream outFile(this->m_resultFile.c_str(), ios::out);
    if(!outFile) {
	cout<<"#Error: open target file which stores the result failed!"<<endl;
	return ;
    }
    
    //open source data file
    ifstream inFile(this->m_dataSourceFile.c_str(), ios::in);
    if(!inFile) {
	cout<<"#Error: read source data file failed!"<<endl;
	return ;
    }
    
    //clean the store space
    vector<double> tempVector;
    for(int i = 0; i < this->m_dataDimension; i++) {
	tempVector.push_back(0);
    }
    for(int i = 0; i < this->m_numOfCluster; i++) {
	this->m_numInCluster[i] = 0;
	this->m_avgCentroid[i] = tempVector;
    }
    
    //read data and classify
    vector<double> temp;
    double tempValue;
    for(int i = 0; i < this->m_lenOfData; i++) {
	for(int j = 0; j < this->m_dataDimension; j++) {
	    inFile>>tempValue;
	    temp.push_back(tempValue);
	}
	
	this->classifyOneData(temp, outFile);
	temp.clear();
    }
    
    //close the file
    outFile.close();
    inFile.close();
    
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

void Mapper::classifyOneData(std::vector< double > data, ofstream& outFile) {
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
    vector<double> tempVector = this->m_avgCentroid[result];
    for(int i = 0; i < this->m_dataDimension; i++) {
	//this->m_avgCentroid[result][i] += data[i];
	tempVector[i] += data[i];
    }
    this->m_avgCentroid[result] = tempVector;
    
    //write into the file
    for(int i = 0; i < this->m_dataDimension; i++) {
	outFile<<data[i]<<' ';
    }
    outFile<<'\t'<<result<<endl;
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
    
    cout<<"now data: "<<m_dataDimension<<"\t\t"<<m_numOfCluster<<endl;
    
    //send back
    try {
	this->m_ptrSocket->send("OK", 2);
    } catch(ClassException<Socket> e) {
	cerr<<"#Execption: send message error!"<<endl;
	return false;
    }
    
    //init store space;
    this->init();
    
    return true;
}

bool Mapper::receiveCentroidInfo() {
    cout<<"Try to receive current Centroids information!"<<endl;
    
    for(int i = 0; i < m_numOfCluster; i++) {
	char *recvMsg = new char[1024];
	string recvStr;
	
	cout<<"try to receive centroids-"<<i<<endl;
	
	try {
	    this->m_ptrSocket->recv(recvMsg, 1024);   //receive data
	    recvStr = recvMsg;				//convert data
	    this->m_ptrSocket->send("OK", 2);		//feed back
	} catch(ClassException<Socket> e) {
	    cout<<"#Error: receive current centroids data-"<<i<<" failed!\n\t\tMessage: "<<e.what()<<endl;
	    return false;
	}
	
	cout<<"received data string: "<<recvStr<<endl;
	
	vector<double> tempValue;
	
	for(int j = 0; j < this->m_dataDimension; j++) {
	    int index = recvStr.find_first_of('%');		//decide end of sub-string
	    string subStr = recvStr.substr(0, index);		//get sub-string
	    recvStr.erase(0, index+1);				//cut sub-string
	    double value = atof(subStr.c_str());    
	    tempValue.push_back(value);				//convert and store
	}
	
	this->m_centroids[i] = tempValue;
	
	delete recvMsg;
	
	cout<<"Finished to receive centroids-"<<i<<endl;
    }
    
    cout<<"Finish to receive current Centroids information!"<<endl;
    
    //test code print centroids
    cout<<"now centroids!"<<endl;
    for(int i = 0; i < this->m_numOfCluster; i++) {
	for(int j = 0; j < this->m_dataDimension; j++) {
	    cout<<this->m_centroids[i][j]<<'\t';
	}
	cout<<endl;
    }
    cout<<endl;
    
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
	
	cout<<"try to send number data of cluster-"<<i<<"\t the string is: "<<sendStr.str()<<endl;
	
	//send number of data in cluster-i
	if(!(this->sendOneData(sendStr))){
	    return false;
	}
	
	cout<<"finished! send number data of cluster-"<<i<<endl;
	
	//cleand buffer
	sendStr.clear();
	
	//send this centroid
	for(int j = 0; j < this->m_dataDimension; j++) {
	    sendStr<<this->m_avgCentroid[i][j]<<'%';
	}
	
	cout<<"try to send part of centroids of cluster-"<<i<<"\tthe string is: "<<sendStr.str()<<endl;
	
	//send centroids of this cluster
	if(!(this->sendOneData(sendStr))){
	    return false;
	}
	
	cout<<"try to send part of centroids of cluster-"<<i<<endl;
    }
  
    cout<<"Finish sending the data!"<<endl;
  
    return true;
}

bool Mapper::sendOneData(const stringstream& sendStr) {
    char *recvMsg = new char[1024];
    string recvStr = "";
    
    //send data
    try {
	this->m_ptrSocket->send(sendStr.str().c_str(), sendStr.str().length());
    } catch(ClassException<Socket> e) {
	cout<<"#Error: can't send centroids data in cluster!"<<endl;
	return false;
    }
    
    //receive feed back
    try {
	this->m_ptrSocket->recv(recvMsg, 4);
	recvStr += recvMsg[0];
	recvStr += recvMsg[1];
    } catch(ClassException<Socket> e) {
	cout<<"#Error: can't receive feedback from server!"<<endl;
	return false;
    }
    
    //cout<<"Received feedback message: "<<recvStr<<endl;
    
    //whether it is ok
    if("OK" != recvStr) {
	cout<<"#Error!!!: receive feedback from server failed!"<<endl;
	return false;
    }
    
    delete recvMsg;
    
    return true;
}






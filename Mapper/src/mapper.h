#ifndef MAPPER_H
#define MAPPER_H

#include "net/SocketStream.hpp"
#include "net/Socket.h"

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <unistd.h>

using namespace std;
using namespace net;


class Mapper {

public:
    Mapper();
    
    virtual ~Mapper();
    
    /**
     * constructor
     * 
     */
    Mapper(const string& host, unsigned short port);
    
    /**
     * Do task of mapper. Connect to server, do calculate, store result, send back information
     * @return false--the task end with execption. true--without execption
     */
    bool runTask();
    
private:
    
    /**
     * init the store space and some variables
     */
    void init();
  
    /**
     * connect to server and wait for command
     */
    bool establishConnect(const string& foreignAddress, unsigned short port);
    
    /**
     * After connecting to server, receive the information of centroids.
     * Then this function wiil be invoked to classify data into different cluster.
     * Write temp result into file, and store the new centroid information.
     * Next send information back to server.
     */
    void classifyData();
    
    /**
     * read one data from file.
     * Just classify this data.
     */
    void classifyOneData(vector<double> data, ofstream& outFile);
    
    /**
     * Receive the raw data. 
     */
    bool receiveData();
    
    /**
     * Receive the baisc information, 
     * such as dimension, number of cluster, length of data
     */
    bool receiveBasicInfo();
    
    /**
     * Recieve the information of centroids which are updated in last process loop.
     */
    bool receiveCentroidInfo();
    
    /**
     * After classificaiton task. Send the output data/information back to server.
     */
    bool sendOutPutToServer();
  
    /**
     * The function is just responsible for send data
     * @param sendStr	the data need to be sent
     */
    bool sendOneData(const stringstream& sendStr);
    
private:
    //socket information
    string m_myHost;
    unsigned short m_myPort;
    
    //socket object
    //SocketStream<TCPSocket> *m_ptrSocket;
    TCPSocket *m_ptrSocket;
    
    //data structure used while calculating
    map<int, vector<double> > m_avgCentroid;	//store the new centroids of each cluster(a part of centroid information)
    map<int, int> m_numInCluster;        	//store the number of points in each cluster 
    map<int, vector<double> > m_centroids;	//centroids from server
    
    //basic information of this task
    int m_dataDimension;
    int m_numOfCluster;
    int m_lenOfData;
    
    //the directory and name of file store part data which is belongs to this client
    string m_dataSourceFile;
    //the file to write the result
    string m_resultFile;
};

#endif // MAPPER_H

#ifndef MAPPER_H
#define MAPPER_H

#include "net/SocketStream.hpp"
#include "net/Socket.h"

#include <iostream>
#include <map>
#include <vector>

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
  
private:
    //socket information
    string m_myHost;
    unsigned short m_myPort;
    
    //socket object
    SocketStream<TCPSocket> *m_ptrSocket;
    
    //data structure used while calculating
    map<int, vector<double> > m_avgCentroid;	//store the new centroids of each cluster(a part of centroid information)
    map<int, int> m_numOfCluster;        	//store the number of points in each cluster 
    map<int, vector<double> > m_centroids;	//centroids from server
  
};

#endif // MAPPER_H

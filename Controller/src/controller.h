#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "net/SocketStream.hpp"
#include "net/Socket.h"
#include <iostream>
#include <pthread.h>
#include <fstream>
#include <cmath>
#include <map>
#include <vector>
#include <cstdlib>

using namespace net;
using namespace std;

//define the status of process
#define NOP	0
#define WAIT	1
#define END	2
#define INIT	3
#define CAL	4

class Controller {

public:
    Controller();
    virtual ~Controller();
    
    /**
     * constructor
     */
    Controller(const string host, unsigned short port);
    
    /**
     * init all attributes
     */
    void init();
    
    /**
     * run the task of the main thread:
     * create the server, wait all clients, send commands, receive the result, chage status, loop
     * @return false--the task is ended with execption. true--without execption
     */
    bool runTask();
    
private:
    /**
     * Create a server and wait for clients
     * @return false--failed to create a TCP Socket Server   true--successful
     */
    bool establishServer();
    
    /**
     * After create a server successfully, wait all clients connect to server
     */
    bool waitAllClients();
    
    /**
     * Whether all clients are the special status
     * @param tartgetStatus	
     * @return true---all clients are in this status  false---are not
     */
    bool isAllClientsStatus(int tartgetStatus);
    
    /**
     * Whether the task can be stoped. 
     * When the change of controids less than throld, the task can be stoped.
     */
    bool canStopTask();
    
    /**
     * read data from file and calculate the init centroids
     */
    bool readDataFromFile();
    
    /**
     * The entry function of client thread
     */
    static void *clientThread(void *arg);
    
    /**
     * Send basic information to client which is belongs to this thread before doing classification.
     * such dimension of data, number of cluster...
     * @param client	the TCP Socket Object of client
     * @return whether the task is end with exception
     */
    static bool sendBasicInfoToClient(TCPSocket *client);
    
    /**
     * Ask client to classify the data in its area.
     * Receive the result from client which is been combined.
     * The result is not the classification information. It is the new centroids information
     * @param client	the TCP Socket Object of client
     * @return Whether the task finished successful
     */
    static bool askClientClassifyData(TCPSocket *client);
    
    static bool sendCommand(TCPSocket *client, int commandIndex);
    
private:
    //server information
    string m_myHost;
    unsigned short m_myPort;
    
    //source data file directory
    string m_sourceDataFile;
    string m_configurationFile;
    
    //server socket object
    TCPServerSocket *m_ptrServerScoket;
    
    //number of clients (mapper for calculating)
    int m_numOfMapper;
    
    //basic information
//    int m_dataDimension;
//    int m_numOfCluster;
//    int m_lenOfData;
    
    //store the information of centroids
    //map< int, vector<double> > m_currCentroids;
    
};

#endif // CONTROLLER_H

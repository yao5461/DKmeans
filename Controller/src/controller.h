#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "net/SocketStream.hpp"
#include "net/Socket.h"
#include <iostream>
#include <pthread.h>

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
     * The entry function of client thread
     */
    static void *clientThread(void *arg);
    
    /**
     * Send basic information to client which is belongs to this thread before doing classification.
     * such dimension of data, number of cluster...
     */
    static bool sendBasicInfoToClient();
    
    /**
     * Ask client to classify the data in its area.
     * Receive the result from client which is been combined.
     * The result is not the classification information. It is the new centroids information
     */
    static void askClientClassifyData();
    
private:
    //server information
    string m_myHost;
    unsigned short m_myPort;
    
    //server socket object
    TCPServerSocket *m_ptrServerScoket;
    
    //number of clients (mapper for calculating)
    int m_numOfMapper;
    
    //basic information
    int m_dataDimension;
    int m_numOfCluster;
    int m_lenOfData;
    
    //the current status of the main process
    int m_currMainThreadStatus;
    //the current status of the client processes
    int *m_currClientThreadsStatus;
    
    //mutex 
    pthread_mutex_t m_mainStatusMutex;
    pthread_mutex_t m_clientStatusMutex;
    pthread_mutex_t m_updateMutex;
};

#endif // CONTROLLER_H

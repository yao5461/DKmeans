#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "net/SocketStream.hpp"
#include "net/Socket.h"
#include <iostream>
#include <pthread.h>

using namespace net;
using namespace std;

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
    
    static void *clientThread(void *arg);
    
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
    
    //the current status of the
    int m_currStatus;
};

#endif // CONTROLLER_H

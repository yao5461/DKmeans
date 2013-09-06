#include "mapper.h"

int main(int argc, char **argv) {
    const string address = "127.0.0.1";
    unsigned short port = 9991;
  
    Mapper mapperProcess(address, port);
    
    mapperProcess.runTask();
    
    return 0;
}

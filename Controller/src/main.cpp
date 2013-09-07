#include "controller.h"

int main(int argc, char **argv) {
  
    Controller controllerProcess;
  
    controllerProcess.init();
    controllerProcess.runTask();
    
    return 0;
}

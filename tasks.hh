#include <Arduino.h>


class Tasks{    
    public:
        String data;
        String comp;
        ///////////////////////// METHODS ///////////////////////////////////////////
        Tasks(String data, String comp){
            this->data = data;
            this->comp = comp;
        }
        void tasksAppellation(char* Name[4], int StackSize[4],int Priority[4], int CoreID[4]);
};
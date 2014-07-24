/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfo/EventID.h"
#include <iostream> 

int main (int argc, char* argv[])  
{  

    std::cout << "testEventInfo: " << std::endl;
    EventID eid(1,2);
    eid.set_detector_mask(1, 2);
    std::cout << std::hex << eid.detector_mask0() << " " << eid.detector_mask1() << " " << eid.detector_mask() << std::endl;
    std::cout << eid << std::endl;
    
}       



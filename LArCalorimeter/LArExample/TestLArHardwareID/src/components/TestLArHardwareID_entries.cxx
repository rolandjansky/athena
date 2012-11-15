//
//  Entry file for ATHENA component libraries
//

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TestLArHardwareID/TestLArHWID_Algo.h"
#include "TestLArHardwareID/initTTMap_Algo.h"
#include "TestLArHardwareID/reinitTTMap_Algo.h"
#include "TestLArHardwareID/testTTMap_Algo.h"
#include "TestLArHardwareID/updateTTMap_Algo.h"


DECLARE_ALGORITHM_FACTORY( TestLArHWID_Algo ) 
DECLARE_ALGORITHM_FACTORY( initTTMap_Algo ) 
DECLARE_ALGORITHM_FACTORY( reinitTTMap_Algo ) 
DECLARE_ALGORITHM_FACTORY( testTTMap_Algo ) 
DECLARE_ALGORITHM_FACTORY( updateTTMap_Algo ) 

DECLARE_FACTORY_ENTRIES(TestLArHardwareID) {
    DECLARE_ALGORITHM( TestLArHWID_Algo )
    DECLARE_ALGORITHM( initTTMap_Algo ) 
    DECLARE_ALGORITHM( reinitTTMap_Algo ) 
    DECLARE_ALGORITHM( testTTMap_Algo ) 
    DECLARE_ALGORITHM( updateTTMap_Algo ) 
}

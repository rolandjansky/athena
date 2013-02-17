//
//  ATHENA component libraries
//

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArSim/LArHitMaker.h"
#include "LArSim/LArFakeClusterProducer.h"
#include "LArSim/LArFakeHitProducer.h"
#include "LArSim/LArHitReader.h"

DECLARE_ALGORITHM_FACTORY( LArHitMaker )
DECLARE_ALGORITHM_FACTORY( LArHitReader )
DECLARE_ALGORITHM_FACTORY( LArFakeClusterProducer )
DECLARE_ALGORITHM_FACTORY( LArFakeHitProducer )

DECLARE_FACTORY_ENTRIES(LArSim) {

    DECLARE_ALGORITHM( LArHitReader )
    DECLARE_ALGORITHM( LArHitMaker )
    DECLARE_ALGORITHM( LArFakeClusterProducer )
    DECLARE_ALGORITHM( LArFakeHitProducer )
    
}

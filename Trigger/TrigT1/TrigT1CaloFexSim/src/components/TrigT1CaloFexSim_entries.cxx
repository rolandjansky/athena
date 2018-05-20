
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1CaloFexSim/JGTowerReader.h"

DECLARE_ALGORITHM_FACTORY( JGTowerReader )


#include "TrigT1CaloFexSim/JGTowerMaker.h"
DECLARE_ALGORITHM_FACTORY( JGTowerMaker )

DECLARE_FACTORY_ENTRIES( TrigT1CaloFexSim ) 
{
  DECLARE_ALGORITHM( JGTowerMaker );
  DECLARE_ALGORITHM( JGTowerReader );
}

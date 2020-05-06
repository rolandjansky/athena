
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1CaloFexSim/JGTowerReader.h"

DECLARE_ALGORITHM_FACTORY( JGTowerReader )


#include "TrigT1CaloFexSim/JGTowerMaker.h"
DECLARE_ALGORITHM_FACTORY( JGTowerMaker )


#include "TrigT1CaloFexSim/JFexEleTau.h"
DECLARE_ALGORITHM_FACTORY( JFexEleTau )

DECLARE_FACTORY_ENTRIES( TrigT1CaloFexSim ) 
{
  DECLARE_ALGORITHM( JFexEleTau );
  DECLARE_ALGORITHM( JGTowerMaker );
  DECLARE_ALGORITHM( JGTowerReader );
}

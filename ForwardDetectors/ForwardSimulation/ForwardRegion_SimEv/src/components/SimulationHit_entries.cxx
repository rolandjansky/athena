#include "ForwardRegion_SimEv/SimulationHitCollection.h"
#include "ForwardRegion_SimEv/SimulationHit.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(SimulationHit)
DECLARE_ALGORITHM_FACTORY(SimulationHitCollection)

DECLARE_FACTORY_ENTRIES(ForwardRegion_SimEv) {
	DECLARE_ALGORITHM(SimulationHit)
	DECLARE_ALGORITHM(SimulationHitCollection)
}
 

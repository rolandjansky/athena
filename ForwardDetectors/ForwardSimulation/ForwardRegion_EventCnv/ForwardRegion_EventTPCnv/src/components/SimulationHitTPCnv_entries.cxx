#include "ForwardRegion_EventTPCnv/SimulationHitHit_p1.h"
#include "ForwardRegion_EventTPCnv/SimulationHitHitCnv_p1.h"
#include "ForwardRegion_EventTPCnv/SimulationHitHitCollection_p1.h"
#include "ForwardRegion_EventTPCnv/SimulationHitHitCollectionCnv_p1.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( SimulationHit_p1 )
DECLARE_ALGORITHM_FACTORY( SimulationHitHitCnv_p1 )
DECLARE_ALGORITHM_FACTORY( SimulationHitHitCollection_p1 )
DECLARE_ALGORITHM_FACTORY( SimulationHitHitCollectionCnv_p1 )

DECLARE_FACTORY_ENTRIES(ForwardRegion_EventTPCnv) {

	DECLARE_ALGORITHM( SimulationHit_p1 )
	DECLARE_ALGORITHM( SimulationHitHitCnv_p1 )
	DECLARE_ALGORITHM( SimulationHitHitCollection_p1 )
	DECLARE_ALGORITHM( SimulationHitHitCollectionCnv_p1 )
}


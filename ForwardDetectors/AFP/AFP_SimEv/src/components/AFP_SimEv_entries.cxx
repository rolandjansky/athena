#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_SimEv/AFP_TDSimHit.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "AFP_SimEv/AFP_SIDSimHit.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(AFP_TDSimHit)
DECLARE_ALGORITHM_FACTORY(AFP_TDSimHitCollection)
DECLARE_ALGORITHM_FACTORY(AFP_SIDSimHit)
DECLARE_ALGORITHM_FACTORY(AFP_SIDSimHitCollection)

DECLARE_FACTORY_ENTRIES(AFP_SimEv) {
	DECLARE_ALGORITHM(AFP_SIDSimHit)
	DECLARE_ALGORITHM(AFP_SIDSimHitCollection)
}
 
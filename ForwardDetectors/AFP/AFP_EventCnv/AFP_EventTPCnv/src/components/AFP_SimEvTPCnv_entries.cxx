#include "AFP_EventTPCnv/AFP_TDSimHit_p1.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCollection_p1.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCollectionCnv_p1.h"

#include "AFP_EventTPCnv/AFP_TDDigi_p1.h"
#include "AFP_EventTPCnv/AFP_TDDigiCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDDigiCollection_p1.h"
#include "AFP_EventTPCnv/AFP_TDDigiCollectionCnv_p1.h"

#include "AFP_EventTPCnv/AFP_SIDSimHit_p1.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCollection_p1.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCollectionCnv_p1.h"

#include "AFP_EventTPCnv/AFP_SiDigi_p1.h"
#include "AFP_EventTPCnv/AFP_SiDigiCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SiDigiCollection_p1.h"
#include "AFP_EventTPCnv/AFP_SiDigiCollectionCnv_p1.h"



#include "AFP_EventTPCnv/AFP_TDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEventCnv_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollection_p1.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollectionCnv_p1.h"

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvent_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEventCnv_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollection_p1.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollectionCnv_p1.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( AFP_TDSimHit_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDSimHitCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDSimHitCollection_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDSimHitCollectionCnv_p1 )

DECLARE_ALGORITHM_FACTORY( AFP_TDDigi_p1 )   
DECLARE_ALGORITHM_FACTORY( AFP_TDDigiCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDDigiCollection_p1 )   
DECLARE_ALGORITHM_FACTORY( AFP_TDDigiCollectionCnv_p1 )

DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHit_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHitCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHitCollection_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDSimHitCollectionCnv_p1 )

DECLARE_ALGORITHM_FACTORY( AFP_SiDigi_p1 )   
DECLARE_ALGORITHM_FACTORY( AFP_SiDigiCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SiDigiCollection_p1 )   
DECLARE_ALGORITHM_FACTORY( AFP_SiDigiCollectionCnv_p1 )


DECLARE_ALGORITHM_FACTORY( AFP_TDLocRecoEvent_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDLocRecoEventCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDLocRecoEvCollection_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_TDLocRecoEvCollectionCnv_p1 )

DECLARE_ALGORITHM_FACTORY( AFP_SIDLocRecoEvent_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDLocRecoEventCnv_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDLocRecoEvCollection_p1 )
DECLARE_ALGORITHM_FACTORY( AFP_SIDLocRecoEvCollectionCnv_p1 )


DECLARE_FACTORY_ENTRIES(AFP_EventTPCnv) {
	DECLARE_ALGORITHM( AFP_TDSimHit_p1 )
	DECLARE_ALGORITHM( AFP_TDSimHitCnv_p1 )
	DECLARE_ALGORITHM( AFP_TDSimHitCollection_p1 )
	DECLARE_ALGORITHM( AFP_TDSimHitCollectionCnv_p1 )

        DECLARE_ALGORITHM( AFP_TDDigi_p1 )
        DECLARE_ALGORITHM( AFP_TDDigiCnv_p1 )
        DECLARE_ALGORITHM( AFP_TDDigiCollection_p1 )
        DECLARE_ALGORITHM( AFP_TDDigiCollectionCnv_p1 )

	DECLARE_ALGORITHM( AFP_SIDSimHit_p1 )
	DECLARE_ALGORITHM( AFP_SIDSimHitCnv_p1 )
	DECLARE_ALGORITHM( AFP_SIDSimHitCollection_p1 )
	DECLARE_ALGORITHM( AFP_SIDSimHitCollectionCnv_p1 )

        DECLARE_ALGORITHM( AFP_SiDigi_p1 )
        DECLARE_ALGORITHM( AFP_SiDigiCnv_p1 )
        DECLARE_ALGORITHM( AFP_SiDigiCollection_p1 )
        DECLARE_ALGORITHM( AFP_SiDigiCollectionCnv_p1 )

	
	DECLARE_ALGORITHM( AFP_TDLocRecoEvent_p1 )
	DECLARE_ALGORITHM( AFP_TDLocRecoEventCnv_p1 )
	DECLARE_ALGORITHM( AFP_TDLocRecoEvCollection_p1 )
	DECLARE_ALGORITHM( AFP_TDLocRecoEvCollectionCnv_p1 )
	
	DECLARE_ALGORITHM( AFP_SIDLocRecoEvent_p1 )
	DECLARE_ALGORITHM( AFP_SIDLocRecoEventCnv_p1 )
	DECLARE_ALGORITHM( AFP_SIDLocRecoEvCollection_p1 )
	DECLARE_ALGORITHM( AFP_SIDLocRecoEvCollectionCnv_p1 )
}


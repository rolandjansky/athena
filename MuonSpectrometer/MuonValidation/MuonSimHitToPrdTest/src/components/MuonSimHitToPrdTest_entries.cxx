#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonSimHitToPrdTest/RPC_SimHitToPrdCBNTAlgo.h"
//#include "MuonSimHitToPrdTest/MDT_SimHitToPrdCBNTAlgo.h"

DECLARE_ALGORITHM_FACTORY(RPC_SimHitToPrdCBNTAlgo)
//DECLARE_ALGORITHM_FACTORY(MDT_SimHitToPrdCBNTAlgo)

DECLARE_FACTORY_ENTRIES(MuonSimHitToPrdTest) 
{
    DECLARE_ALGORITHM(RPC_DigitRdoPrepCBNTAlgo)
	//DECLARE_ALGORITHM(MDT_DigitRdoPrepCBNTAlgo)
}

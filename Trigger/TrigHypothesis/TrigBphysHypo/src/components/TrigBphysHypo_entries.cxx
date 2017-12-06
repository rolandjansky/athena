#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigL2BMuMuXHypo.h"
#include "../TrigL2BMuMuXFex.h"
#include "../TrigL2BMuMuHypo.h"
#include "../TrigL2MultiMuFex.h"
#include "../TrigL2MultiMuHypo.h"
#include "../TrigL2BMuMuFex.h"
#include "../TrigEFBMuMuXHypo.h"
#include "../TrigEFBMuMuXFex.h"
#include "../TrigEFBEEXFex.h"
#include "../TrigEFBMuMuHypo.h"
#include "../TrigEFMultiMuHypo.h"
//#include "../TrigL2DiMuHypo.h"
//#include "../TrigEFDiMuFex.h"
//#include "../TrigEFDiMuHypo.h"
//#include "TrigBphysHypo/TrigL2DsPhiPiHypo.h"
//#include "TrigBphysHypo/TrigL2DsPhiPiFex.h"
//#include "../TrigL2DiMuXHypo.h"
//#include "../TrigL2DiMuXFex.h"
#include "../TrigL2TrkMassFex.h"
#include "../TrigL2TrkMassHypo.h"
#include "../TrigEFTrkMassHypo.h"
#include "../TrigMultiTrkFex.h"
#include "../TrigBphysMuonCounter.h"
#include "../TrigBphysElectronCounter.h"

//#include "../TrigL2LooseMuonHypo.h"

#include "../TrigBphysHelperUtilsTool.h"
#include "../TrigEFBMuMuFex.h"
#include "../TrigEFMultiMuFex.h"
#include "../TrigEFTrkMassFex.h"

//#include "../TrigBphysL1DiMuComboFex.h"


//DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXHypo )
//DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuFex )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBEEXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuHypo )
//DECLARE_ALGORITHM_FACTORY( TrigL2DiMuHypo )
//DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiHypo )
//DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassHypo )
//DECLARE_ALGORITHM_FACTORY( TrigL2LooseMuonHypo )

DECLARE_TOOL_FACTORY( TrigBphysHelperUtilsTool )

DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuFex   )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigMultiTrkFex )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassFex )
DECLARE_ALGORITHM_FACTORY( TrigBphysMuonCounter )
DECLARE_ALGORITHM_FACTORY( TrigBphysElectronCounter )
//DECLARE_ALGORITHM_FACTORY( TrigBphysL1DiMuComboFex )


DECLARE_FACTORY_ENTRIES( TrigBphysHypo ) {
//    DECLARE_ALGORITHM( TrigL2DiMuXHypo )
//    DECLARE_ALGORITHM( TrigL2DiMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuXHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuHypo )
    DECLARE_ALGORITHM( TrigL2MultiMuFex )
    DECLARE_ALGORITHM( TrigL2MultiMuHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuFex )
    DECLARE_ALGORITHM( TrigEFBMuMuXHypo )
    DECLARE_ALGORITHM( TrigEFBMuMuXFex )
    DECLARE_ALGORITHM( TrigEFBEEXFex )
    DECLARE_ALGORITHM( TrigEFBMuMuHypo )
    DECLARE_ALGORITHM( TrigEFMultiMuHypo )
//    DECLARE_ALGORITHM( TrigL2DiMuHypo )
//    DECLARE_ALGORITHM( TrigL2DsPhiPiHypo )
//    DECLARE_ALGORITHM( TrigL2DsPhiPiFex )
    DECLARE_ALGORITHM( TrigL2TrkMassFex )
    DECLARE_ALGORITHM( TrigL2TrkMassHypo )
    DECLARE_ALGORITHM( TrigEFTrkMassHypo )
//    DECLARE_ALGORITHM( TrigL2LooseMuonHypo )

    DECLARE_TOOL( TrigBphysHelperUtilsTool )

    DECLARE_ALGORITHM( TrigEFBMuMuFex   )
    DECLARE_ALGORITHM( TrigEFMultiMuFex )
    DECLARE_ALGORITHM( TrigMultiTrkFex )
    DECLARE_ALGORITHM( TrigBphysMuonCounter )
    DECLARE_ALGORITHM( TrigBphysElectronCounter )
    DECLARE_ALGORITHM( TrigEFTrkMassFex )
//    DECLARE_ALGORITHM( TrigBphysL1DiMuComboFeX )

}


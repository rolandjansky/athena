#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigBphysHypo/TrigL2BMuMuXHypo.h"
#include "TrigBphysHypo/TrigL2BMuMuXFex.h"
#include "TrigBphysHypo/TrigL2BMuMuHypo.h"
#include "TrigBphysHypo/TrigL2MultiMuFex.h"
#include "TrigBphysHypo/TrigL2MultiMuHypo.h"
#include "TrigBphysHypo/TrigL2BMuMuFex.h"
#include "../TrigEFBMuMuXHypo.h"
#include "../TrigEFBMuMuXFex.h"
#include "../TrigEFBMuMuHypo.h"
#include "../TrigEFMultiMuHypo.h"
#include "TrigBphysHypo/TrigL2DiMuHypo.h"
#include "../TrigEFDiMuFex.h"
#include "../TrigEFDiMuHypo.h"
#include "TrigBphysHypo/TrigL2DsPhiPiHypo.h"
#include "TrigBphysHypo/TrigL2DsPhiPiFex.h"
#include "TrigBphysHypo/TrigL2JpsieeHypo.h"
#include "TrigBphysHypo/TrigL2JpsieeFex.h"
#include "TrigBphysHypo/TrigL2BgammaXHypo.h"
#include "TrigBphysHypo/TrigL2BgammaXFex.h"
#include "TrigBphysHypo/TrigL2DiMuXHypo.h"
#include "TrigBphysHypo/TrigL2DiMuXFex.h"
#include "TrigBphysHypo/TrigL2TrkMassFex.h"
#include "TrigBphysHypo/TrigL2TrkMassHypo.h"
#include "../TrigEFTrkMassHypo.h"

#include "TrigBphysHypo/TrigL2LooseMuonHypo.h"

#include "../TrigBphysHelperUtilsTool.h"
#include "../TrigEFBMuMuFex.h"
#include "../TrigEFMultiMuFex.h"
#include "../TrigEFTrkMassFex.h"

DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuFex )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiFex )
DECLARE_ALGORITHM_FACTORY( TrigL2JpsieeHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2JpsieeFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BgammaXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BgammaXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2LooseMuonHypo )

DECLARE_TOOL_FACTORY( TrigBphysHelperUtilsTool )

DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuFex   )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassFex )


DECLARE_FACTORY_ENTRIES( TrigBphysHypo ) {
    DECLARE_ALGORITHM( TrigL2DiMuXHypo )
    DECLARE_ALGORITHM( TrigL2DiMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuXHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuHypo )
    DECLARE_ALGORITHM( TrigL2MultiMuFex )
    DECLARE_ALGORITHM( TrigL2MultiMuHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuFex )
    DECLARE_ALGORITHM( TrigEFBMuMuXHypo )
    DECLARE_ALGORITHM( TrigEFBMuMuXFex )
    DECLARE_ALGORITHM( TrigEFBMuMuHypo )
    DECLARE_ALGORITHM( TrigEFMultiMuHypo )
    DECLARE_ALGORITHM( TrigL2DiMuHypo )
    DECLARE_ALGORITHM( TrigL2DsPhiPiHypo )
    DECLARE_ALGORITHM( TrigL2DsPhiPiFex )
    DECLARE_ALGORITHM( TrigL2JpsieeHypo )
    DECLARE_ALGORITHM( TrigL2JpsieeFex )
    DECLARE_ALGORITHM( TrigL2BgammaXHypo )
    DECLARE_ALGORITHM( TrigL2BgammaXFex )
    DECLARE_ALGORITHM( TrigL2TrkMassFex )
    DECLARE_ALGORITHM( TrigL2TrkMassHypo )
    DECLARE_ALGORITHM( TrigEFTrkMassHypo )
    DECLARE_ALGORITHM( TrigL2LooseMuonHypo )

    DECLARE_TOOL( TrigBphysHelperUtilsTool )

    DECLARE_ALGORITHM( TrigEFBMuMuFex   )
    DECLARE_ALGORITHM( TrigEFMultiMuFex )
    DECLARE_ALGORITHM( TrigEFTrkMassFex )
}


#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigBphysHypo/TrigL2BMuMuXHypo.h"
#include "TrigBphysHypo/TrigL2BMuMuXFex.h"
#include "TrigBphysHypo/TrigL2BMuMuHypo.h"
#include "TrigBphysHypo/TrigL2MultiMuFex.h"
#include "TrigBphysHypo/TrigL2MultiMuHypo.h"
#include "TrigBphysHypo/TrigL2BMuMuFex.h"
#include "TrigBphysHypo/TrigEFBMuMuXHypo.h"
#include "TrigBphysHypo/TrigEFBMuMuXHypo_xAOD.h"
#include "TrigBphysHypo/TrigEFBMuMuXFex.h"
#include "TrigBphysHypo/TrigEFBMuMuXFex_xAOD.h"
#include "TrigBphysHypo/TrigEFBMuMuHypo.h"
#include "TrigBphysHypo/TrigEFBMuMuHypo_xAOD.h"
#include "TrigBphysHypo/TrigEFBMuMuFex.h"
#include "TrigBphysHypo/TrigEFBMuMuFex_xAOD.h"
#include "TrigBphysHypo/TrigEFMultiMuHypo.h"
#include "TrigBphysHypo/TrigEFMultiMuHypo_xAOD.h"
#include "TrigBphysHypo/TrigEFMultiMuFex.h"
#include "TrigBphysHypo/TrigEFMultiMuFex_xAOD.h"
#include "TrigBphysHypo/TrigL2DiMuHypo.h"
#include "TrigBphysHypo/TrigEFDiMuFex.h"
#include "TrigBphysHypo/TrigEFDiMuHypo.h"
#include "TrigBphysHypo/TrigEFDiMuFex_xAOD.h"
#include "TrigBphysHypo/TrigEFDiMuHypo_xAOD.h"
#include "TrigBphysHypo/TrigL2DsPhiPiHypo.h"
#include "TrigBphysHypo/TrigL2DsPhiPiFex.h"
#include "TrigBphysHypo/TrigEFDsPhiPiFex.h"
#include "TrigBphysHypo/TrigEFDsPhiPiHypo.h"
#include "TrigBphysHypo/TrigL2JpsieeHypo.h"
#include "TrigBphysHypo/TrigL2JpsieeFex.h"
#include "TrigBphysHypo/TrigEFJpsieeHypo.h"
#include "TrigBphysHypo/TrigEFJpsieeFex.h"
#include "TrigBphysHypo/TrigL2BgammaXHypo.h"
#include "TrigBphysHypo/TrigL2BgammaXFex.h"
#include "TrigBphysHypo/TrigEFBgammaXHypo.h"
#include "TrigBphysHypo/TrigEFBgammaXFex.h"
#include "TrigBphysHypo/TrigL2DiMuXHypo.h"
#include "TrigBphysHypo/TrigL2DiMuXFex.h"
#include "TrigBphysHypo/TrigEFDiMuXHypo.h"
#include "TrigBphysHypo/TrigEFDiMuXFex.h"
#include "TrigBphysHypo/TrigL2TrkMassFex.h"
#include "TrigBphysHypo/TrigL2TrkMassHypo.h"
#include "TrigBphysHypo/TrigEFTrkMassFex.h"
#include "TrigBphysHypo/TrigEFTrkMassHypo.h"
#include "TrigBphysHypo/TrigEFTrkMassFex_xAOD.h"
#include "TrigBphysHypo/TrigEFTrkMassHypo_xAOD.h"

#include "TrigBphysHypo/TrigL2LooseMuonHypo.h"

DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DiMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDiMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDiMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BMuMuFex )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2MultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXHypo_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuXFex_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuHypo_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBMuMuFex_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuHypo_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFMultiMuFex_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigL2DiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDiMuFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDiMuHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DsPhiPiFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDsPhiPiFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDsPhiPiHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2JpsieeHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2JpsieeFex )
DECLARE_ALGORITHM_FACTORY( TrigEFJpsieeHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFJpsieeFex )
DECLARE_ALGORITHM_FACTORY( TrigL2BgammaXHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2BgammaXFex )
DECLARE_ALGORITHM_FACTORY( TrigEFBgammaXHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFBgammaXFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassFex )
DECLARE_ALGORITHM_FACTORY( TrigL2TrkMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassFex )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassFex_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigEFTrkMassHypo_xAOD )
DECLARE_ALGORITHM_FACTORY( TrigL2LooseMuonHypo )

DECLARE_FACTORY_ENTRIES( TrigBphysHypo ) {
    DECLARE_ALGORITHM( TrigL2DiMuXHypo )
    DECLARE_ALGORITHM( TrigL2DiMuXFex )
    DECLARE_ALGORITHM( TrigEFDiMuXHypo )
    DECLARE_ALGORITHM( TrigEFDiMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuXHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuXFex )
    DECLARE_ALGORITHM( TrigL2BMuMuHypo )
    DECLARE_ALGORITHM( TrigL2MultiMuFex )
    DECLARE_ALGORITHM( TrigL2MultiMuHypo )
    DECLARE_ALGORITHM( TrigL2BMuMuFex )
    DECLARE_ALGORITHM( TrigEFBMuMuXHypo )
    DECLARE_ALGORITHM( TrigEFBMuMuXFex )
    DECLARE_ALGORITHM( TrigEFBMuMuXHypo_xAOD )
    DECLARE_ALGORITHM( TrigEFBMuMuXFex_xAOD )
    DECLARE_ALGORITHM( TrigEFBMuMuHypo )
    DECLARE_ALGORITHM( TrigEFBMuMuHypo_xAOD )
    DECLARE_ALGORITHM( TrigEFBMuMuFex )
    DECLARE_ALGORITHM( TrigEFBMuMuFex_xAOD )
    DECLARE_ALGORITHM( TrigEFMultiMuHypo )
    DECLARE_ALGORITHM( TrigEFMultiMuHypo_xAOD )
    DECLARE_ALGORITHM( TrigEFMultiMuFex )
    DECLARE_ALGORITHM( TrigEFMultiMuFex_xAOD )
    DECLARE_ALGORITHM( TrigL2DiMuHypo )
    DECLARE_ALGORITHM( TrigEFDiMuFex )
    DECLARE_ALGORITHM( TrigEFDiMuHypo )
    DECLARE_ALGORITHM( TrigL2DsPhiPiHypo )
    DECLARE_ALGORITHM( TrigL2DsPhiPiFex )
    DECLARE_ALGORITHM( TrigEFDsPhiPiFex )
    DECLARE_ALGORITHM( TrigEFDsPhiPiHypo )
    DECLARE_ALGORITHM( TrigL2JpsieeHypo )
    DECLARE_ALGORITHM( TrigL2JpsieeFex )
    DECLARE_ALGORITHM( TrigEFJpsieeHypo )
    DECLARE_ALGORITHM( TrigEFJpsieeFex )
    DECLARE_ALGORITHM( TrigL2BgammaXHypo )
    DECLARE_ALGORITHM( TrigL2BgammaXFex )
    DECLARE_ALGORITHM( TrigEFBgammaXHypo )
    DECLARE_ALGORITHM( TrigEFBgammaXFex )
    DECLARE_ALGORITHM( TrigL2TrkMassFex )
    DECLARE_ALGORITHM( TrigL2TrkMassHypo )
    DECLARE_ALGORITHM( TrigEFTrkMassFex )
    DECLARE_ALGORITHM( TrigEFTrkMassHypo )
    DECLARE_ALGORITHM( TrigEFTrkMassFex_xAOD )
    DECLARE_ALGORITHM( TrigEFTrkMassHypo_xAOD )
    DECLARE_ALGORITHM( TrigL2LooseMuonHypo )
}


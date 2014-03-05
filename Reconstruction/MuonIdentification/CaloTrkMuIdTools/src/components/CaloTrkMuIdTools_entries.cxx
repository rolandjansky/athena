//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "CaloTrkMuIdTools/CaloMuonTag.h"
#include "CaloTrkMuIdTools/TrackEnergyInCaloTool.h"
#include "CaloTrkMuIdTools/TrackDepositInCaloTool.h"
#include "CaloTrkMuIdTools/CaloMuonLikelihoodTool.h"

DECLARE_TOOL_FACTORY( CaloMuonTag )
DECLARE_TOOL_FACTORY( TrackEnergyInCaloTool )
DECLARE_TOOL_FACTORY( TrackDepositInCaloTool )
DECLARE_TOOL_FACTORY( CaloMuonLikelihoodTool )

DECLARE_FACTORY_ENTRIES	( CaloTrkMuIdTools ) {
    DECLARE_TOOL( CaloMuonTag );
    DECLARE_TOOL( TrackEnergyInCaloTool );
    DECLARE_TOOL( TrackDepositInCaloTool );
    DECLARE_TOOL( CaloMuonLikelihoodTool );
}



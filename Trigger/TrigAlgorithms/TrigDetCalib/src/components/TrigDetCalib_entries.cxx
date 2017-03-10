#include "../TrigROBSelector.h"
#include "../TrigROBListWriter.h"
#include "../TrigSubDetListWriter.h"
#include "../TrigEtaHypo.h"
#include "../TrigCheckForTracks.h"
#include "../TrigCheckForMuons.h"
#include "../ScoutingStreamWriter.h"
#include "../TrigL1CaloOverflow.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TrigROBSelector )
DECLARE_ALGORITHM_FACTORY( TrigROBListWriter )
DECLARE_ALGORITHM_FACTORY( TrigSubDetListWriter )
DECLARE_ALGORITHM_FACTORY( TrigEtaHypo )
DECLARE_ALGORITHM_FACTORY( TrigCheckForTracks )
DECLARE_ALGORITHM_FACTORY( TrigCheckForMuons )
DECLARE_ALGORITHM_FACTORY( ScoutingStreamWriter )
DECLARE_ALGORITHM_FACTORY( TrigL1CaloOverflow )

DECLARE_FACTORY_ENTRIES( TrigDetCalib ) {
    DECLARE_TOOL( TrigROBSelector )
    DECLARE_ALGORITHM( TrigROBListWriter )
    DECLARE_ALGORITHM( TrigSubDetListWriter )
    DECLARE_ALGORITHM( TrigEtaHypo )
    DECLARE_ALGORITHM( TrigCheckForTracks )
    DECLARE_ALGORITHM( TrigCheckForMuons )
    DECLARE_ALGORITHM( ScoutingStreamWriter )
    DECLARE_ALGORITHM( TrigL1CaloOverflow )
}


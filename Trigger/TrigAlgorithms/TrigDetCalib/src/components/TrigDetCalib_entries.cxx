#include "TrigDetCalib/TrigROBSelector.h"
#include "TrigDetCalib/TrigROBListWriter.h"
#include "TrigDetCalib/TrigSubDetListWriter.h"
#include "TrigDetCalib/TrigEtaHypo.h"
#include "TrigDetCalib/TrigCheckForTracks.h"
#include "TrigDetCalib/ScoutingStreamWriter.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TrigROBSelector )
DECLARE_ALGORITHM_FACTORY( TrigROBListWriter )
DECLARE_ALGORITHM_FACTORY( TrigSubDetListWriter )
DECLARE_ALGORITHM_FACTORY( TrigEtaHypo )
DECLARE_ALGORITHM_FACTORY( TrigCheckForTracks )
DECLARE_ALGORITHM_FACTORY( ScoutingStreamWriter )

DECLARE_FACTORY_ENTRIES( TrigDetCalib ) {
    DECLARE_TOOL( TrigROBSelector )
    DECLARE_ALGORITHM( TrigROBListWriter )
    DECLARE_ALGORITHM( TrigSubDetListWriter )
    DECLARE_ALGORITHM( TrigEtaHypo )
    DECLARE_ALGORITHM( TrigCheckForTracks )
    DECLARE_ALGORITHM( ScoutingStreamWriter )
}


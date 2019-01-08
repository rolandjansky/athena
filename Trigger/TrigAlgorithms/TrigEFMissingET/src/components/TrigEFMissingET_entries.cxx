#include "TrigEFMissingET/EFMissingET.h"
#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigEFMissingET/EFMissingETFromCells.h"
#include "TrigEFMissingET/EFMissingETFromClusters.h"
#include "TrigEFMissingET/EFMissingETFromClustersPS.h"
#include "TrigEFMissingET/EFMissingETFromClustersPUC.h"
#include "TrigEFMissingET/EFMissingETFromClustersTracksPUC.h"
#include "TrigEFMissingET/EFMissingETFromFEBHeader.h"
#include "TrigEFMissingET/EFMissingETFromJets.h"
#include "TrigEFMissingET/EFMissingETFromTrackAndJets.h"
#include "TrigEFMissingET/EFMissingETFromTrackAndClusters.h"
//#include "TrigEFMissingET/EFMissingETFromLvl1Ppr.h"
#include "TrigEFMissingET/EFMissingETFlags.h"
#include "TrigEFMissingET/EFMissingETFromHelper.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( EFMissingET )
DECLARE_TOOL_FACTORY( EFMissingETBaseTool )
DECLARE_TOOL_FACTORY( EFMissingETFromCells )
DECLARE_TOOL_FACTORY( EFMissingETFromClusters )
DECLARE_TOOL_FACTORY( EFMissingETFromClustersPS )
DECLARE_TOOL_FACTORY( EFMissingETFromClustersPUC )
DECLARE_TOOL_FACTORY( EFMissingETFromClustersTracksPUC )
DECLARE_TOOL_FACTORY( EFMissingETFromFEBHeader )
DECLARE_TOOL_FACTORY( EFMissingETFromJets )
DECLARE_TOOL_FACTORY( EFMissingETFromTrackAndJets )
DECLARE_TOOL_FACTORY( EFMissingETFromTrackAndClusters )
//DECLARE_TOOL_FACTORY( EFMissingETFromLvl1Ppr )
DECLARE_TOOL_FACTORY( EFMissingETFlags )
DECLARE_TOOL_FACTORY( EFMissingETFromHelper )

DECLARE_FACTORY_ENTRIES(TrigEFMissingET) {
    DECLARE_ALGORITHM( EFMissingET );
    DECLARE_TOOL( EFMissingETBaseTool );
    DECLARE_TOOL( EFMissingETFromCells );
    DECLARE_TOOL( EFMissingETFromClusters );
    DECLARE_TOOL( EFMissingETFromClustersPS );
    DECLARE_TOOL( EFMissingETFromClustersPUC );
    DECLARE_TOOL( EFMissingETFromClustersTracksPUC );
    DECLARE_TOOL( EFMissingETFromFEBHeader );
    DECLARE_TOOL( EFMissingETFromJets );
    DECLARE_TOOL( EFMissingETFromTrackAndJets );
    DECLARE_TOOL( EFMissingETFromTrackAndClusters );
    //DECLARE_TOOL( EFMissingETFromLvl1Ppr );
    DECLARE_TOOL( EFMissingETFlags );
    DECLARE_TOOL( EFMissingETFromHelper );
}

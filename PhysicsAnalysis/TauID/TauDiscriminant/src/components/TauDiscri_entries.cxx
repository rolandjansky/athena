#include "TauDiscriminant/TauJetBDT.h"
#include "TauDiscriminant/TauEleBDT.h"
#include "TauDiscriminant/TauMuonVeto.h"
#include "TauDiscriminant/TauScoreFlatteningTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TauJetBDT )
DECLARE_TOOL_FACTORY( TauEleBDT )
DECLARE_TOOL_FACTORY( TauMuonVeto )
DECLARE_TOOL_FACTORY( TauScoreFlatteningTool )

DECLARE_FACTORY_ENTRIES(TauDiscriminant)
{
    DECLARE_TOOL( TauJetBDT )
    DECLARE_TOOL( TauEleBDT )
    DECLARE_TOOL( TauMuonVeto )
    DECLARE_TOOL( TauScoreFlatteningTool )
}

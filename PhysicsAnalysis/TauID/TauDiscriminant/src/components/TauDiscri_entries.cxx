#include "TauDiscriminant/TauDiscriBuilder.h"
#include "TauDiscriminant/TauCutsEleVeto.h"
#include "TauDiscriminant/TauCuts.h"
#include "TauDiscriminant/TauLLH.h"
#include "TauDiscriminant/TauJetBDT.h"
#include "TauDiscriminant/TauEleBDT.h"
#include "TauDiscriminant/TauPi0BDT.h"
#include "TauDiscriminant/TauMuonVeto.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TauDiscriBuilder )

DECLARE_TOOL_FACTORY( TauCutsEleVeto )
DECLARE_TOOL_FACTORY( TauCuts )
DECLARE_TOOL_FACTORY( TauLLH ) 
DECLARE_TOOL_FACTORY( TauJetBDT )
DECLARE_TOOL_FACTORY( TauEleBDT )
DECLARE_TOOL_FACTORY( TauPi0BDT )
DECLARE_TOOL_FACTORY( TauMuonVeto )

DECLARE_FACTORY_ENTRIES(TauDiscriminant)
{
    DECLARE_ALGORITHM( TauDiscriBuilder )
    
    DECLARE_TOOL( TauCutsEleVeto )
    DECLARE_TOOL( TauCuts )
    DECLARE_TOOL( TauLLH )
    DECLARE_TOOL( TauJetBDT )
    DECLARE_TOOL( TauEleBDT )
    DECLARE_TOOL( TauPi0BDT )
    DECLARE_TOOL( TauMuonVeto )
}

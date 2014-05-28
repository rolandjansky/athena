/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             based on MuGirl by Shlomit Tarem
***************************************************************************/
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigMuGirl/TrigMuGirl.h"
#include "TrigMuGirl/TrigPostFex.h"
#include "TrigMuGirl/TrigMuGirlAlg.h"
#include "TrigMuGirl/TrigMuGirlTool.h"


DECLARE_ALGORITHM_FACTORY( TrigMuGirl )
DECLARE_ALGORITHM_FACTORY( TrigMuGirlAlg )
DECLARE_ALGORITHM_FACTORY( TrigPostFex )
DECLARE_TOOL_FACTORY( TrigMuGirlTool )

DECLARE_FACTORY_ENTRIES( TrigMuGirl ) { 

    DECLARE_ALGORITHM( TrigMuGirl )
    DECLARE_ALGORITHM( TrigMuGirlAlg )
    DECLARE_ALGORITHM( TrigPostFex )
    DECLARE_TOOL( TrigMuGirlTool )

}


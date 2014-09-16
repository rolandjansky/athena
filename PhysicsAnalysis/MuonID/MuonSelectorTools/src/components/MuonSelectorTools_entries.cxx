#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonSelectorTools/MuonSelectionTool.h"
#include "../MuonSelectionAlg.h"


using namespace CP;


DECLARE_NAMESPACE_TOOL_FACTORY(CP, MuonSelectionTool )//DECLARE_TOOL_FACTORY( MuonSelectionTool ) is there a difference!?
DECLARE_NAMESPACE_ALGORITHM_FACTORY(CP, MuonSelectionAlg )

DECLARE_FACTORY_ENTRIES( MuonSelectorTools ) 
{
  DECLARE_NAMESPACE_ALGTOOL(CP,MuonSelectionTool)//DECLARE_ALGTOOL( CP::MuonSelectionTool )
    
  DECLARE_NAMESPACE_ALGORITHM( CP, MuonSelectionAlg )
}



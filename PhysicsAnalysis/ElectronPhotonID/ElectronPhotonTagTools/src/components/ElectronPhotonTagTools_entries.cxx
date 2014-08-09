#include "ElectronPhotonTagTools/ElectronTagTool.h"
#include "ElectronPhotonTagTools/EgammaTagTool.h"
#include "ElectronPhotonTagTools/PhotonTagTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_TOOL_FACTORY( EgammaTagTool )
DECLARE_TOOL_FACTORY( ElectronTagTool )
DECLARE_TOOL_FACTORY( PhotonTagTool )
 
DECLARE_FACTORY_ENTRIES( ElectronPhotonTagTools ) {
  DECLARE_TOOL( EgammaTagTool )
  DECLARE_TOOL( ElectronTagTool )
  DECLARE_TOOL( PhotonTagTool )
}



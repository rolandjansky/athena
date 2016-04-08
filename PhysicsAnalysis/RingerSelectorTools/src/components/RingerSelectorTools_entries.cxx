#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RingerSelectorTools/AsgElectronRingerSelector.h"

using namespace Ringer;

DECLARE_TOOL_FACTORY( AsgElectronRingerSelector   )
//DECLARE_TOOL_FACTORY( AsgPhotonRingerSelector     )

DECLARE_FACTORY_ENTRIES( RingerSelectorTools ){
  DECLARE_ALGTOOL( AsgElectronRingerSelector  )
  //DECLARE_ALGTOOL( AsgPhotonRingerSelector    )
}

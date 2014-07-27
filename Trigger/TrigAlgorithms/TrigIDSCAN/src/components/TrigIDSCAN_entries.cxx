////////////////////////////////////////////////////////////////////////////////
// IdScan
// -------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigIDSCAN/TrigIDSCAN.h"
#include "TrigInDetTrackUtils/TrigToTrkTrackTool.h"
#include "TrigIDSCAN/ROB_Preloader.h"
#include "TrigIDSCAN/ShifterTool.h"
 
DECLARE_TOOL_FACTORY( TrigToTrkTrackTool )
DECLARE_TOOL_FACTORY( ShifterTool )
DECLARE_ALGORITHM_FACTORY( ROB_Preloader )
DECLARE_ALGORITHM_FACTORY( TrigIDSCAN)
DECLARE_FACTORY_ENTRIES( TrigIDSCAN )
{
  DECLARE_TOOL( ShifterTool )
  DECLARE_ALGORITHM( ROB_Preloader )
  DECLARE_ALGORITHM( TrigIDSCAN )
}

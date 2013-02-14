#include "../LArBadChannelToolTest.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LArBadChannelTool/LArBadChannelMasker.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBAlg.h"
#include "LArBadChannelTool/LArBadChannelTimingAlg.h"
#include "LArBadChannelTool/LArBadChannel2Ascii.h"
#include "LArBadChannelTool/LArBadFeb2Ascii.h"

DECLARE_TOOL_FACTORY( LArBadChannelMasker )
DECLARE_TOOL_FACTORY( LArBadChanTool )
DECLARE_ALGORITHM_FACTORY( LArBadChannelToolTest )
DECLARE_ALGORITHM_FACTORY( LArBadChannelDBAlg )
DECLARE_ALGORITHM_FACTORY( LArBadChannelTimingAlg )
DECLARE_ALGORITHM_FACTORY( LArBadChannel2Ascii )
DECLARE_ALGORITHM_FACTORY( LArBadFeb2Ascii )

DECLARE_FACTORY_ENTRIES(LArBadChannelTool) {
   DECLARE_TOOL( LArBadChannelMasker )
   DECLARE_TOOL( LArBadChanTool )
   DECLARE_ALGORITHM( LArBadChannelToolTest )
   DECLARE_ALGORITHM( LArBadChannelDBAlg )
   DECLARE_ALGORITHM( LArBadChannelTimingAlg )
   DECLARE_ALGORITHM( LArBadChannel2Ascii )
   DECLARE_ALGORITHM( LArBadFeb2Ascii )
}
 

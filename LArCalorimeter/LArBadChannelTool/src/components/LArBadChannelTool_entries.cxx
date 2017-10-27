#include "../LArBadChannelToolTest.h"


#include "LArBadChannelTool/LArBadChannelMasker.h"
#include "LArBadChannelTool/LArBadChanTool.h"
#include "LArBadChannelTool/LArBadChannelDBAlg.h"
#include "LArBadChannelTool/LArBadChannelTimingAlg.h"
#include "LArBadChannelTool/LArBadChannel2Ascii.h"
#include "LArBadChannelTool/LArBadFeb2Ascii.h"
#include "LArBadChannelTool/LArBadChannelCondAlg.h"
#include "LArBadChannelTool/LArBadFebCondAlg.h"

DECLARE_TOOL_FACTORY( LArBadChannelMasker )
DECLARE_TOOL_FACTORY( LArBadChanTool )
DECLARE_ALGORITHM_FACTORY( LArBadChannelToolTest )
DECLARE_ALGORITHM_FACTORY( LArBadChannelDBAlg )
DECLARE_ALGORITHM_FACTORY( LArBadChannelTimingAlg )
DECLARE_ALGORITHM_FACTORY( LArBadChannel2Ascii )
DECLARE_ALGORITHM_FACTORY( LArBadFeb2Ascii )
DECLARE_ALGORITHM_FACTORY( LArBadChannelCondAlg )
DECLARE_ALGORITHM_FACTORY( LArBadFebCondAlg )


#include "../HTTRawHitsWrapperAlg.h"
#include "../HTTRawNtupleWrapperAlg.h"
#include "../HTTReadLogicalHitsAlg.h"
#include "../HTTLogicalHitsWrapperAlg.h"
#include "../HTTReadRawRandomHitsTool.h"
#include "../HTTInputHeaderTool.h"
#include "TrigHTTInput/HTTRawToLogicalHitsTool.h"
#include "../HTTDetectorTool.h"
#include "../HTTDumpDetStatusAlgo.h"
#include "../HTTOutputHeaderTool.h"
#include "../HTTDumpOutputStatAlg.h"


DECLARE_COMPONENT( HTTDetectorTool )
DECLARE_COMPONENT( HTTReadRawRandomHitsTool )
DECLARE_COMPONENT( HTTInputHeaderTool )
DECLARE_COMPONENT( HTTRawToLogicalHitsTool )
DECLARE_COMPONENT( HTTOutputHeaderTool )

DECLARE_COMPONENT( HTTDumpDetStatusAlgo )
DECLARE_COMPONENT( HTTRawHitsWrapperAlg )
DECLARE_COMPONENT( HTTRawNtupleWrapperAlg )
DECLARE_COMPONENT( HTTReadLogicalHitsAlg)
DECLARE_COMPONENT( HTTLogicalHitsWrapperAlg)
DECLARE_COMPONENT( HTTDumpOutputStatAlg )


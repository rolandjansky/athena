
## DPDMakerScript to be included in rec.DPDMakerScripts()

include.block("HSG2DPDUtils/hsg2_d3pdmaker_plain.py")
include("HSG2DPDUtils/hsg2_dpd_utils_fragment.py")

from AthenaCommon.Logging import logging
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner

import HSG2DPDUtils.HSG2DPDFlags

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

stream_d3pd_plain=HSG2physicsD3PD("StreamNTUP_HSG2",runArgs.outputNTUP_2LHSG2File)

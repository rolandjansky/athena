include.block("HSG2DPDUtils/hsg2_d3pdmaker_2L.prodJobOFragment.py")

import HSG2DPDUtils.HSG2DPDFlags

from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
streamName = prodFlags.Write2LHSG2D3PD.StreamName

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
fileName = buildFileName(prodFlags.Write2LHSG2D3PD)

from HSG2DPDUtils.HSG2D3PD import HSG2physicsD3PD

# Perform MV3 b-tagging when creating D3PD for 2013 Moriond analysis
if jobproperties.HSG2.doMV3BTagInD3PD():
    include("BTagging/BTagging_LoadTools.py")
    include("BTagging/BTagging_jobOptions.py")

stream_d3pd_plain=HSG2physicsD3PD(streamName,fileName)

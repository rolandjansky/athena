# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================

import TrigHTTUtils.HTTTagConfig as HTTTagConfig

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()



if hasattr(runArgs,"outputNTUP_HTTIPFile") :
    OutputNTUP_HTTIPFile = runArgs.outputNTUP_HTTIPFile
else :
    OutputNTUP_HTTIPFile = "httsim_rawhits_wrap.root"
print "Output file", OutputNTUP_HTTIPFile

from AthenaCommon.AppMgr import ToolSvc
from TrigHTTSGInput.TrigHTTSGInputConf import HTT_SGToRawHitsTool
HTTSGInput = HTT_SGToRawHitsTool( maxEta= 3.2, minPt= 0.8*GeV)
HTTSGInput.OutputLevel = DEBUG
HTTSGInput.ReadTruthTracks = True
ToolSvc += HTTSGInput

from TrigHTTSGInput.TrigHTTSGInputConf import HTTRawHitsWrapperAlg
wrapper = HTTRawHitsWrapperAlg(OutputLevel = DEBUG,
                             OutFileName = OutputNTUP_HTTIPFile)

wrapper.InputTool = HTTSGInput
theJob += wrapper

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()


print theJob
###############################################################

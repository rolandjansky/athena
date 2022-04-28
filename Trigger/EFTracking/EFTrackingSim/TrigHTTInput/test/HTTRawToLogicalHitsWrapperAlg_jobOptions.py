# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# TrigHTTWrapper job options file
#
#==============================================================


from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from PyJobTransformsCore.runargs import RunArguments
runArgs = RunArguments()

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

import os
import glob

#--------------------------------------------------------------
# HTT Includes
#--------------------------------------------------------------

import TrigHTTConfTools.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

tags = HTTTagConfig.getTags(stage='map')
map_tag = tags['map']
MapSvc = HTTMapConfig.addMapSvc(map_tag)

#--------------------------------------------------------------
# Arguments
#--------------------------------------------------------------

InputHTTRawHitFile = []
if 'InputHTTRawHitFile' in os.environ:
    for ex in os.environ['InputHTTRawHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            InputHTTRawHitFile += files
        else:
            InputHTTRawHitFile += [ex]
else:
    InputHTTRawHitFile = ["httsim_rawhits_wrap.root"]

 #output
OutputHTTRawHitFile = []
if 'OutputHTTRawHitFile' in os.environ :
     for ex in os.environ['OutputHTTRawHitFile'].split(','):
        files = glob.glob(ex)
        if files:
            OutputHTTRawHitFile += files
        else:
            OutputHTTRawHitFile += [ex]
else :
     OutputHTTRawHitFile = ["httsim_rawhits_wrap.OUT.root"]
msg = logging.getLogger('HTTRawToLogicalHitsWrapperAlg')
msg.info("OutHTTSim file %r ",  OutputHTTRawHitFile)



#--------------------------------------------------------------
# Create the components
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool, HTTOutputHeaderTool
HTTReadInput = HTTInputHeaderTool(OutputLevel = DEBUG)
HTTReadInput.InFileName=InputHTTRawHitFile
HTTReadInput.RWstatus="READ"
#HTTReadInput.ReadTruthTracks = True
ToolSvc += HTTReadInput

HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput", OutputLevel = DEBUG)
HTTWriteOutput.InFileName=OutputHTTRawHitFile
HTTWriteOutput.RWstatus="RECREATE"
ToolSvc += HTTWriteOutput



from TrigHTTInput.TrigHTTInputConf import HTTRawToLogicalHitsTool
HTTRawLogic = HTTRawToLogicalHitsTool(OutputLevel = DEBUG)
HTTRawLogic.SaveOptional = 2
ToolSvc += HTTRawLogic



from TrigHTTInput.TrigHTTInputConf import HTTLogicalHitsWrapperAlg
wrapper = HTTLogicalHitsWrapperAlg(OutputLevel = DEBUG)

wrapper.InputTool = HTTReadInput
wrapper.OutputTool = HTTWriteOutput
wrapper.RawToLogicalHitsTool = HTTRawLogic
wrapper.Clustering = True
wrapper.Monitor = True
theJob += wrapper

###############################################################

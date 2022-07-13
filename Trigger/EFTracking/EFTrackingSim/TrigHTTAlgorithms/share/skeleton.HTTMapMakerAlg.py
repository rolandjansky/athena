# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
import glob

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

theApp.EvtMax = runArgs.maxEvents

from GaudiSvc.GaudiSvcConf import THistSvc

#--------------------------------------------------------------
# HTT Setup
#--------------------------------------------------------------

from TrigHTTConfig.parseRunArgs import parseHTTArgs
from TrigHTTConfig.formatMessageSvc import HTT_OutputLevel, formatMessageSvc
import TrigHTTConfig.HTTTagConfig as HTTTagConfig
import TrigHTTConfig.HTTConfigCompInit as HTTConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig

formatMessageSvc()

HTT_OutputLevel=getattr(runArgs, 'OutputLevel', HTT_OutputLevel)
#HTT_OutputLevel = DEBUG

HTTInputFile = runArgs.InFileName
print "Input file:  ", HTTInputFile

tags = HTTTagConfig.getTags(stage='algo', options=parseHTTArgs(runArgs))
map_tag = tags['map']

def defaultFilename():
	return "MapMakerOutput"

OutFileName = getattr(runArgs, 'OutFileName', defaultFilename())
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MONITOROUT DATAFILE='"+OutFileName+"' OPT='RECREATE'"]

ES = HTTConfig.addEvtSelSvc(map_tag)
ES.OutputLevel = HTT_OutputLevel
MapSvc = HTTMapConfig.addMapSvc(map_tag)
MapSvc.OutputLevel = HTT_OutputLevel

#--------------------------------------------------------------
# Make the algorithm
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTMapMakerAlg

alg = HTTMapMakerAlg()
alg.OutputLevel = HTT_OutputLevel
alg.OutFileName = OutFileName
alg.region = getattr(runArgs, 'region', 0)
alg.KeyString = getattr(runArgs, 'KeyString', "strip,barrel,2")
alg.KeyString2 = getattr(runArgs, 'KeyString2', "")
alg.nSlices = getattr(runArgs, 'nSlices', -1)
alg.trim = getattr(runArgs, 'trim', 0.1)
alg.maxEvents = getattr(runArgs, 'maxEvents', 1000)

theJob += alg

#--------------------------------------------------------------
# Make hit tools
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool

InputTool = HTTInputHeaderTool("HTTReadInput")
InputTool.OutputLevel = HTT_OutputLevel
InputTool.InFileName = runArgs.InFileName

ToolSvc += InputTool
alg.InputTool = InputTool

# END TEST
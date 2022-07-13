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

# Input
HTTInputFile = []
if runArgs.InFileName:
	for ex in runArgs.InFileName.split(','):
		files = glob.glob(ex)
		if files:
			HTTInputFile += files
		else:
			HTTInputFile += [ex]
else:
    HTTInputFile = ["httsim_input.root"]
print "Input file:  ", HTTInputFile

tags = HTTTagConfig.getTags(stage='algo', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
algo_tag = tags['algo']

def defaultFilename():
    f = HTTTagConfig.getDescription(map_tag, bank_tag, algo_tag, filename=True)
    f = 'output__' +  map_tag['release'] + '-' + map_tag['geoTag'] + '__' + f + '.root'
    return f

OutFileName=getattr(runArgs, 'OutFileName', defaultFilename())
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MONITOROUT DATAFILE='"+OutFileName+"' OPT='RECREATE'"]

ES = HTTConfig.addEvtSelSvc(map_tag)
ES.OutputLevel = HTT_OutputLevel

MapSvc = HTTMapConfig.addMapSvc(map_tag)
MapSvc.OutputLevel = HTT_OutputLevel

#--------------------------------------------------------------
# Make the algorithm
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTOutputMonitorAlg

alg = HTTOutputMonitorAlg()
alg.OutputLevel = HTT_OutputLevel
alg.RunSecondStage = algo_tag['secondStage']
alg.histoPrintDetail = 2

theJob += alg

#--------------------------------------------------------------
# Make Read Output Tool
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool

OutputReader = HTTOutputHeaderTool()
OutputReader.OutputLevel = HTT_OutputLevel
OutputReader.InFileName = HTTInputFile
OutputReader.RWstatus="READ"
OutputReader.RunSecondStage = alg.RunSecondStage

ToolSvc += OutputReader
alg.ReadOutputTool = OutputReader

#--------------------------------------------------------------
# Make Monitor Tools and Data Flow Tool
#--------------------------------------------------------------

from TrigHTTMonitor.TrigHTTMonitorConf import HTTHitMonitorTool, HTTClusterMonitorTool, HTTRoadMonitorTool, HTTTrackMonitorTool, HTTPerformanceMonitorTool, HTTEventMonitorTool, HTTSecondStageMonitorTool, HTTMonitorUnionTool
from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTDataFlowTool

HitMonitor = HTTHitMonitorTool()
HitMonitor.OutputLevel = HTT_OutputLevel
HitMonitor.fastMon = algo_tag['fastMon']
HitMonitor.OutputMon = True
HitMonitor.RunSecondStage = alg.RunSecondStage
HitMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']

ClusterMonitor = HTTClusterMonitorTool()
ClusterMonitor.OutputLevel = HTT_OutputLevel
ClusterMonitor.Clustering = False
ClusterMonitor.Spacepoints = False
ClusterMonitor.RunSecondStage = alg.RunSecondStage
ClusterMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']

RoadMonitor = HTTRoadMonitorTool()
RoadMonitor.OutputLevel = HTT_OutputLevel
RoadMonitor.fastMon = algo_tag['fastMon']
RoadMonitor.RunSecondStage = alg.RunSecondStage
RoadMonitor.canExtendHistRanges = algo_tag['canExtendHistRanges']
RoadMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']

TrackMonitor = HTTTrackMonitorTool()
TrackMonitor.OutputLevel = HTT_OutputLevel
TrackMonitor.fastMon = algo_tag['fastMon']
TrackMonitor.OutputMon = True
TrackMonitor.DoMissingHitsChecks = False
TrackMonitor.RunSecondStage = alg.RunSecondStage

PerfMonitor = HTTPerformanceMonitorTool()
PerfMonitor.OutputLevel = HTT_OutputLevel
PerfMonitor.fastMon = algo_tag['fastMon']
PerfMonitor.RunSecondStage = alg.RunSecondStage
PerfMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']

EventMonitor = HTTEventMonitorTool()
EventMonitor.OutputLevel = HTT_OutputLevel
EventMonitor.fastMon = algo_tag['fastMon']
EventMonitor.OutputMon = True
EventMonitor.Clustering = False
EventMonitor.Spacepoints = False
EventMonitor.RunSecondStage = alg.RunSecondStage
EventMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']

SecondMonitor = HTTSecondStageMonitorTool()
SecondMonitor.OutputLevel = HTT_OutputLevel
SecondMonitor.fastMon = algo_tag['fastMon']
SecondMonitor.BarcodeFracCut = algo_tag['barcodeFracMatch']
SecondMonitor.Chi2ndofCut = 40.

MonitorUnion = HTTMonitorUnionTool()
MonitorUnion.OutputLevel = HTT_OutputLevel
MonitorTools = []
MonitorTools.append(HitMonitor)
MonitorTools.append(ClusterMonitor)
MonitorTools.append(RoadMonitor)
MonitorTools.append(TrackMonitor)
MonitorTools.append(PerfMonitor)
MonitorTools.append(EventMonitor)
if alg.RunSecondStage:
	MonitorTools.append(SecondMonitor)
MonitorUnion.MonitorTools = MonitorTools

DataFlowTool = HTTDataFlowTool()
DataFlowTool.OutputLevel = HTT_OutputLevel
DataFlowTool.RunSecondStage = alg.RunSecondStage

ToolSvc += MonitorUnion
ToolSvc += DataFlowTool

alg.MonitorUnionTool = MonitorUnion
alg.DataFlowTool = DataFlowTool

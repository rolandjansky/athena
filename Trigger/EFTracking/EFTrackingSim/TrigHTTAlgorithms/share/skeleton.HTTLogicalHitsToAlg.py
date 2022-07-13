# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------
# Headers and setup
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

from AthenaCommon.AppMgr import theApp, ToolSvc, ServiceMgr
from AthenaCommon.Constants import VERBOSE,DEBUG,INFO

theApp.EvtMax = runArgs.maxEvents

from GaudiSvc.GaudiSvcConf import THistSvc

from PerfMonComps.PerfMonFlags import jobproperties

#--------------------------------------------------------------
# HTT Setup
#--------------------------------------------------------------

from TrigHTTConfTools.parseRunArgs import parseHTTArgs
import TrigHTTConfTools.HTTTagConfig as HTTTagConfig
import TrigHTTMaps.HTTMapConfig as HTTMapConfig
import TrigHTTBanks.HTTBankConfig as HTTBankConfig
import TrigHTTAlgorithms.HTTAlgorithmConfig as HTTAlgorithmConfig

print("Input file:  ", runArgs.InFileName)
tags = HTTTagConfig.getTags(stage='algo', options=parseHTTArgs(runArgs))
map_tag = tags['map']
bank_tag = tags['bank']
algo_tag = tags['algo']

def defaultFilename():
    f = HTTTagConfig.getDescription(map_tag, bank_tag, algo_tag, filename=True) # deliberately omitting hit filtering tag for now
    f = 'loghits__' +  map_tag['release'] + '-' + map_tag['geoTag'] + '__' + f + '.root'
    return f

OutFileName=getattr(runArgs, 'OutFileName', defaultFilename())
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MONITOROUT DATAFILE='"+OutFileName+"' OPT='RECREATE'"]

MapSvc = HTTMapConfig.addMapSvc(map_tag)

HitFilteringTool = HTTMapConfig.addHitFilteringTool(map_tag)

BankSvc = HTTBankConfig.addBankSvc(map_tag, bank_tag)


# Important to do hit tracing if we have physics samples or events with pileup.
doHitTracing = (map_tag['withPU'] or map_tag['sampleType']=='LLPs')

#--------------------------------------------------------------
# Make the algorithm
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTLogicalHitsProcessAlg
from TrigHTTHough.TrigHTTHoughConf import HTTEtaPatternFilterTool, HTTPhiRoadFilterTool

alg = HTTLogicalHitsProcessAlg()
alg.HitFiltering = algo_tag['HitFiltering']
alg.writeOutputData = algo_tag['writeOutputData']
alg.Clustering = True
alg.tracking = algo_tag['doTracking']
alg.outputHitTxt = algo_tag['outputHitTxt']
alg.RunSecondStage = algo_tag['secondStage']
alg.DoMissingHitsChecks = algo_tag['DoMissingHitsChecks']

theJob += alg

#--------------------------------------------------------------
# Make hit tools
#--------------------------------------------------------------

from TrigHTTInput.TrigHTTInputConf import HTTRawToLogicalHitsTool
HTTRawLogic = HTTRawToLogicalHitsTool()
HTTRawLogic.SaveOptional = 2
if (map_tag['sampleType'] == 'skipTruth'):
    HTTRawLogic.SaveOptional = 1
HTTRawLogic.TowersToMap = [0] # TODO TODO why is this hardcoded?
ToolSvc += HTTRawLogic
alg.RawToLogicalHitsTool = HTTRawLogic

from TrigHTTInput.TrigHTTInputConf import HTTInputHeaderTool
InputTool = HTTInputHeaderTool("HTTReadInput")
InputTool.InFileName = runArgs.InFileName
ToolSvc += InputTool
alg.InputTool = InputTool

from TrigHTTInput.TrigHTTInputConf import HTTReadRawRandomHitsTool
InputTool2 = HTTReadRawRandomHitsTool("HTTReadRawRandomHitsTool")
InputTool2.InFileName = runArgs.InFileName[0]
ToolSvc += InputTool2
alg.InputTool2 = InputTool2


if algo_tag['EtaPatternRoadFilter']:
    print("Setting Up EtaPatternFilter")
    RoadFilter = HTTEtaPatternFilterTool()
    alg.FilterRoads=True
    if len(algo_tag['threshold']) != 1:
        raise NotImplementedError("EtaPatternRoadFilter does not support multi-value/neighboring bin treshold")
    RoadFilter.threshold=algo_tag['threshold'][0]
    RoadFilter.EtaPatterns=algo_tag['EtaPatternRoadFilter']
    ToolSvc += RoadFilter

if  algo_tag['PhiRoadFilter']!=None:
    print("Setting Up PhiRoadFilter")
    PhiRoadFilter = HTTPhiRoadFilterTool()
    alg.FilterRoads2=True
    if len(algo_tag['threshold'])!=1:
        raise NotImplementedError("PhiRoadFilter does not support multi-value/neighboring bin treshold")
    PhiRoadFilter.threshold=algo_tag['threshold'][0]
    PhiRoadFilter.window=HTTAlgorithmConfig.floatList(algo_tag['PhiRoadFilter'])
    ToolSvc += PhiRoadFilter

# writing down the output file
from TrigHTTInput.TrigHTTInputConf import HTTOutputHeaderTool
HTTWriteOutput = HTTOutputHeaderTool("HTTWriteOutput")
HTTWriteOutput.InFileName = ["test"]
HTTWriteOutput.RWstatus = "HEADER" # do not open file, use THistSvc
HTTWriteOutput.RunSecondStage = alg.RunSecondStage
ToolSvc += HTTWriteOutput
alg.OutputTool= HTTWriteOutput

#--------------------------------------------------------------
# Spacepoint tool
#--------------------------------------------------------------

from TrigHTTMaps.TrigHTTMapsConf import HTTSpacePointsTool, HTTSpacePointsTool_v2

spacepoints_version = 2
if spacepoints_version == 1:
    SPT = HTTSpacePointsTool()
    SPT.FilteringClosePoints = True
else:
    SPT = HTTSpacePointsTool_v2()
    SPT.FilteringClosePoints = False
    SPT.PhiWindow=0.008

SPT.Duplication = True
SPT.Filtering = algo_tag['SpacePointFiltering']
ToolSvc += SPT
alg.SpacePointTool = SPT

#--------------------------------------------------------------
# Make RF/TF tools
#--------------------------------------------------------------

from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTNNTrackTool, HTTOverlapRemovalTool, HTTTrackFitterTool 
from TrigHTTHough.TrigHTTHoughConf import HTTHoughRootOutputTool

if algo_tag['hough']:
    RF = HTTAlgorithmConfig.addHoughTool(map_tag, algo_tag,doHitTracing)
    alg.DoNNTrack = algo_tag['TrackNNAnalysis']
    if algo_tag['TrackNNAnalysis']:
        NNTrackTool = HTTNNTrackTool()
        ToolSvc += NNTrackTool
    alg.DoHoughRootOutput = algo_tag['hough_rootoutput']
    if algo_tag['hough_rootoutput']:
        rootOutTool = HTTHoughRootOutputTool()
        ToolSvc += rootOutTool
        HoughRootOutputName="HTTHoughOutput.root"
        ServiceMgr.THistSvc.Output += ["TRIGHTTHOUGHOUTPUT DATAFILE='"+HoughRootOutputName+"' OPT='RECREATE'"]
elif algo_tag['hough_1d']:
    RF = HTTHoughConfig.addHough1DShiftTool(map_tag, algo_tag)

TF_1st= HTTTrackFitterTool("HTTTrackFitterTool_1st")
HTTAlgorithmConfig.applyTag(TF_1st, algo_tag)
ToolSvc += TF_1st

alg.RoadFinder = RF
alg.TrackFitter_1st = TF_1st

OR_1st = HTTOverlapRemovalTool("HTTOverlapRemovalTool_1st")
OR_1st.ORAlgo = "Normal"
OR_1st.doFastOR = algo_tag['doFastOR']
OR_1st.NumOfHitPerGrouping = 5
# For invert grouping use the below setup
# OR1st.ORAlgo = "Invert"
# OR1st.NumOfHitPerGrouping = 3
if algo_tag['hough']:
    OR_1st.nBins_x = algo_tag['xBins'] + 2 * algo_tag['xBufferBins']
    OR_1st.nBins_y = algo_tag['yBins'] + 2 * algo_tag['yBufferBins']
    OR_1st.localMaxWindowSize = algo_tag['localMaxWindowSize']
    OR_1st.roadSliceOR = algo_tag['roadSliceOR']

#--------------------------------------------------------------
# Second stage fitting
#--------------------------------------------------------------

if algo_tag['secondStage']:

    TF_2nd = HTTTrackFitterTool("HTTTrackFitterTool_2nd")
    HTTAlgorithmConfig.applyTag(TF_2nd, algo_tag)
    TF_2nd.Do2ndStageTrackFit = True
    ToolSvc += TF_2nd
    alg.TrackFitter_2nd = TF_2nd

    OR_2nd = HTTOverlapRemovalTool("HTTOverlapRemovalTool_2nd")
    OR_2nd.DoSecondStage = True
    OR_2nd.ORAlgo = "Normal"
    OR_2nd.doFastOR = algo_tag['doFastOR']
    OR_2nd.NumOfHitPerGrouping = 5
    # For invert grouping use the below setup
    # OR2nd.ORAlgo = "Invert"
    # OR2nd.NumOfHitPerGrouping = 3
    ToolSvc += OR_2nd
    alg.OverlapRemoval_2nd = OR_2nd

    from TrigHTTAlgorithms.TrigHTTAlgorithmsConf import HTTExtrapolator
    HTTExtrapolatorTool = HTTExtrapolator()
    HTTExtrapolatorTool.Ncombinations = 16
    ToolSvc += HTTExtrapolatorTool
    alg.Extrapolator = HTTExtrapolatorTool

#--------------------------------------------------------------
# Configure LRT
#--------------------------------------------------------------

# We'll use this in other tools if LRT is requested
from TrigHTTConfTools import HTTConfigCompInit
evtSelLRT = HTTConfigCompInit.addEvtSelSvc(map_tag,"EvtSelLRTSvc")

from TrigHTTLRT.TrigHTTLRTConf import HTTLLPRoadFilterTool
thellpfilter = HTTLLPRoadFilterTool()
ToolSvc += thellpfilter

if algo_tag['lrt']:

    # Finish setting up that event selector
    evtSelLRT.doLRT = True
    evtSelLRT.minLRTpT = algo_tag['lrt_ptmin']
    evtSelLRT.OutputLevel=DEBUG

    # consistency checks
    assert algo_tag['lrt_use_basicHitFilter'] != algo_tag['lrt_use_mlHitFilter'], 'Inconsistent LRT hit filtering setup, need either ML of Basic filtering enabled'
    assert algo_tag['lrt_use_straightTrackHT'] != algo_tag['lrt_use_doubletHT'], 'Inconsistent LRT HT setup, need either double or strightTrack enabled'
    alg.doLRT = True
    alg.LRTHitFiltering = (not algo_tag['lrt_skip_hit_filtering'])

    # now set up the rest
    from TrigHTTLRT.TrigHTTLRTConf import HTTLLPRoadFilterTool
    if algo_tag['lrt_use_basicHitFilter']:
        alg.LRTRoadFilter = thellpfilter
    if algo_tag['lrt_use_doubletHT']:
        from TrigHTTLRT.TrigHTTLRTConf import addLRTDoubletHTTool
        doubletTool = addLRTDoubletHTTool(algo_tag)
        alg.LRTRoadFinder  = doubletTool
    elif algo_tag['lrt_use_straightTrackHT'] :
        alg.LRTRoadFinder = HTTAlgorithmConfig.addHough_d0phi0_Tool(map_tag, algo_tag, doHitTracing)

    # Use our event selector in the algorithm
    alg.eventSelector = evtSelLRT


ToolSvc += OR_1st
alg.OverlapRemoval_1st = OR_1st

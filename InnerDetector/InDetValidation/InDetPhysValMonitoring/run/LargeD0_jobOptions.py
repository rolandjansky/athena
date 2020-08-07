# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LargeD0_jobOptions.py 779296 2016-10-19 18:17:30Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)

import getpass
FNAME = "AOD.pool.root"
'''
FNAME = ["/n/atlas05/userdata/sche/MC15/xAOD/zprimemumu/emu_RecExCommon/r6/user.sche.LRT.r6.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.ESD.e4821_s2698_r6939_AOD/user.sche.8294387.AOD._000001.pool.root",
         "/n/atlas05/userdata/sche/MC15/xAOD/zprimemumu/emu_RecExCommon/r6/user.sche.LRT.r6.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.ESD.e4821_s2698_r6939_AOD/user.sche.8294387.AOD._000002.pool.root",
         "/n/atlas05/userdata/sche/MC15/xAOD/zprimemumu/emu_RecExCommon/r6/user.sche.LRT.r6.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.ESD.e4821_s2698_r6939_AOD/user.sche.8294387.AOD._000003.pool.root",
         "/n/atlas05/userdata/sche/MC15/xAOD/zprimemumu/emu_RecExCommon/r6/user.sche.LRT.r6.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.ESD.e4821_s2698_r6939_AOD/user.sche.8294387.AOD._000004.pool.root",
         "/n/atlas05/userdata/sche/MC15/xAOD/zprimemumu/emu_RecExCommon/r6/user.sche.LRT.r6.mc15_13TeV.301913.Pythia8EvtGen_A14NNPDF23LO_LLzprimemumu_m250t500.recon.ESD.e4821_s2698_r6939_AOD/user.sche.8294387.AOD._000005.pool.root"
         ]'''

# -- Glob, if necessary ('*' is FNAME)
if '*' in FNAME:
  import glob
  FNAME = glob.glob(FNAME)
  print FNAME
  pass

include( "AthenaPython/iread_file.py" )


# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/LargeD0PlotDefinitions.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

import InDetPhysValMonitoring.InDetPhysValDecoration
for decorator in InDetPhysValMonitoring.InDetPhysValDecoration.getDecorators() :
  topSequence += decorator


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "output"
topSequence += monMan

from InDetPhysValMonitoring.InDetPhysValDecoration import getInDetRttTruthSelectionTool
truthSelection = getInDetTruthSelectionTool(name = 'InDetLargeD0TruthSelectionTool',
                                            minPt = 1000.,
                                            maxBarcode = -1)

# @asogaard
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import TrackSelectionTool
trackSelection = TrackSelectionTool()
print trackSelection
#trackSelection.maxZImpact = -1
#trackSelection.minZImpact = -1
#trackSelection.maxPrimaryImpact = -1
#trackSelection.minPrimaryImpact = -1
#trackSelection.maxEta       = -1
#trackSelection.minEta       = -1
#trackSelection.minSiNotShared = -1 # < Rejects Large-D0 tracks
#trackSelection.maxShared      = -1 # < Rejects Large-D0 tracks
#trackSelection.maxBLayerSplitHits   = -1
#trackSelection.minPixelHits   = -1
#trackSelection.maxPixelHoles  = -1
#trackSelection.maxPixelOutliers  = -1
#trackSelection.maxHoles       = -1
#trackSelection.maxSCTHits       = -1
#trackSelection.minSCTHits     = 9
#trackSelection.maxSctHoles    = -1
#trackSelection.maxDoubleHoles = -1
#trackSelection.maxTRTOutliers       = -1
#trackSelection.maxTRTHighThresholdHits = -1
#trackSelection.minTRTHighThresholdHits = -1
#trackSelection.maxTRTHighThresholdOutliers = -1
ToolSvc += trackSelection
        
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValLargeD0Tool
tool2 = InDetPhysValLargeD0Tool()
tool2.TruthSelectionTool = truthSelection
# Specify Long-lived particle for efficiency plot
# Available options: Zprime, Wino, Gluino, or empty ("") for no selection 
#tool2.LongLivedParticle = "" # @asogaard
#tool2.SignalIds = [32]

ToolSvc += tool2


monMan.AthenaMonTools += [tool2]
#monMan.AthenaMonTools += [truthSelection]

from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
InDetBoundaryCheckTool = InDet__InDetBoundaryCheckTool(
    name="InDetBoundaryCheckTool"
)
ToolSvc += InDetBoundaryCheckTool

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, BoundaryCheckTool=InDetBoundaryCheckTool, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["output DATAFILE='output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = 10
#theApp.EvtMax = -1

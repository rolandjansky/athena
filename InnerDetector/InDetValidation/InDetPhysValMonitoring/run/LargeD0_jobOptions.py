# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: LargeD0_jobOptions.py 747079 2016-05-13 16:10:27Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)

import getpass
FNAME = "AOD.pool.root.1"

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
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/inDetPhysValMonitoringPlotDefinitions.hdef"

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValDecoratorAlg
decorators = InDetPhysValDecoratorAlg()
topSequence += decorators


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

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import TrackTruthSelectionTool
truthSelection = TrackTruthSelectionTool()
truthSelection.requireDecayBeforePixel = False
truthSelection.minPt = 1000.
ToolSvc += truthSelection

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
#trackSelection.minSCTHits     = -1
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
#tool2.SignalIds = "32"

ToolSvc += tool2


monMan.AthenaMonTools += [tool2]
#monMan.AthenaMonTools += [truthSelection]

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator,usePixel = True,useSCT= True,CountDeadModulesAfterLastHit = True)
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

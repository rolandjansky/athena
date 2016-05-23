# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 747937 2016-05-18 10:37:38Z sroe $

# Set up the reading of the input xAOD:

#"AOD.05522648._000044.pool.root.1" K-short dataset
#"ESD.05108991._000060.pool.root.1" original ttbar dataset 
#"ESD.05297574._000081.pool.root.1" new ttbar dataset (this one should enable residuals)
import getpass
FNAME = "AOD.pool.root"
if (getpass.getuser())=="mbaugh":
  #FNAME = "../rootfile_storage/ESD.large.pool.root"
  #FNAME = "../rootfile_storage/AOD.05522648._000044.pool.root.1"
  FNAME = "../rootfile_storage/ESD.05297574._000081.pool.root.1"
  print " Hello, Max"

#FNAME = "ESD.05297574._000081.pool.root.1"
#FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )


# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/hdef.xml"
ServiceMgr.HistogramDefinitionSvc.DefinitionFormat="text/xml"

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
monMan.FileKey = "M_output"
topSequence += monMan


from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()

#tool1.useTrackSelection = True
'''
#tool1.useTrackSelection = True
#tool1.onlyInsideOutTracks = True
tool1.TrackSelectionTool.CutLevel         = "Loose"
tool1.TrackSelectionTool.UseTrkTrackTools = True
tool1.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
tool1.TrackSelectionTool.Extrapolator     = InDetExtrapolator
'''

ToolSvc += tool1
monMan.AthenaMonTools += [tool1]


from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, usePixel = True,useSCT= True, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print InDetHoleSearchTool


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]


# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1
if (getpass.getuser())=="sroe":
  theApp.EvtMax = 1
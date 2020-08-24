# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AllAndTightSelections.py 789797 2016-12-15 06:07:11Z lmijovic $


FNAME = "AOD.pool.root"

include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import HistogramDefinitionSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=HistogramDefinitionSvc()
ServiceMgr.HistogramDefinitionSvc.DefinitionSource="../share/InDetPVMPlotDefRun2.xml"
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
monMan.FileKey = "M_output"
topSequence += monMan



from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool=InDet__InDetTrackSelectionTool()
ToolSvc += InDetTrackSelectorTool
InDetTrackSelectorTool.CutLevel = "TightPrimary"

#This section should control TTST  7-12-16                                                        

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
AthTruthSelectionTool = AthTruthSelectionTool()


print AthTruthSelectionTool
ToolSvc += AthTruthSelectionTool

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = AthTruthSelectionTool
tool1.useTrackSelection = False
#tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.FillTrackInJetPlots = False
print tool1
ToolSvc += tool1
#
tool2 = InDetPhysValMonitoringTool(name="TightPrimary")
tool2.TruthSelectionTool = AthTruthSelectionTool
tool2.useTrackSelection = True
tool2.TrackSelectionTool=InDetTrackSelectorTool
tool2.FillTrackInJetPlots = False
tool2.DirName = "TightPrimary/"
ToolSvc += tool2

monMan.AthenaMonTools += [tool1, tool2]

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
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = 10


# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id: PhysVal_jobOptions.py 795792 2017-02-06 14:45:08Z sroe $

# Set up the reading of the input xAOD:
import getpass
FNAME = "AOD.pool.root"
mode = "Fwd" #Set this to "Back" for backtracking
usingTrackSelection = False
#
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSlimming.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False);

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
monMan = AthenaMonManager( "PhysValMonManager",
  FileKey = "M_output",
  Environment = "altprod",
  ManualDataTypeSetup = True,
  DataType = "monteCarlo",
  ManualRunLBSetup = True,
  Run = 1,
  LumiBlock = 1  )
  
print (monMan)

topSequence += monMan



from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
InDetTrackSelectorTool=InDet__InDetTrackSelectionTool()
ToolSvc += InDetTrackSelectorTool
InDetTrackSelectorTool.CutLevel = "TightPrimary"

#This section should control TTST  7-12-16                                                        
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
AthTruthSelectionTool = AthTruthSelectionTool()

if mode=="Back":
  # max prod. vertex radius for secondaries [mm]
  AthTruthSelectionTool.minPt = 5000
  AthTruthSelectionTool.maxProdVertRadius = 4000 
  AthTruthSelectionTool.maxBarcode = -1
  AthTruthSelectionTool.hasNoGrandparent = True
  AthTruthSelectionTool.poselectronfromgamma = True
  os.environ["BACKTRACKDEBUG"] = "1"

print (AthTruthSelectionTool)

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = AthTruthSelectionTool
tool1.useTrackSelection = usingTrackSelection
tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.FillTrackInJetPlots = True
print (tool1)
tool1.ManagerName = 'PhysValMonManager'



monMan.AthenaMonTools += [tool1]

from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
InDetBoundaryCheckTool = InDet__InDetBoundaryCheckTool(
    name="InDetBoundaryCheckTool"
)
ToolSvc += InDetBoundaryCheckTool

from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
InDetHoleSearchTool = InDet__InDetTrackHoleSearchTool(name = "InDetHoleSearchTool", Extrapolator = InDetExtrapolator, BoundaryCheckTool=InDetBoundaryCheckTool, CountDeadModulesAfterLastHit = True)
ToolSvc += InDetHoleSearchTool
print (InDetHoleSearchTool)

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='M_output.root' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
theApp.EvtMax = -1
if (getpass.getuser())=="sroe":
  theApp.EvtMax = -1

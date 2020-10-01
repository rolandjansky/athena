# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Define a few configuration arguments that can be passed by the user via the command line.
# Avoid the need of hacking the job option file 
def GetCustomAthArgs():
    from AthenaCommon.AthArgumentParser import AthArgumentParser
    IDPVMparser = AthArgumentParser(description='Parser for IDPVM configuration')
    IDPVMparser.add_argument("--doTightPrimary", help='also run tight-primary plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInBJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doExpertPlots", help='run additional expert-level plots', action='store_true', default=False)
    IDPVMparser.add_argument("--outputFile", help='Name of output file',default="M_output.root")
    IDPVMparser.add_argument("--HSFlag", help='Hard-scatter flag - decides what is used for truth matching', choices=['HardScatter', 'All', 'PileUp'],default="HardScatter")
    return IDPVMparser.parse_args()


MyArgs = GetCustomAthArgs()

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doSlimming.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)

include( "AthenaPython/iread_file.py" )

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
                                                 
from InDetPhysValMonitoring.InDetPhysValMonitoringConf import AthTruthSelectionTool
AthTruthSelectionTool = AthTruthSelectionTool()

print (AthTruthSelectionTool)

from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool

tool1 = InDetPhysValMonitoringTool()
tool1.TruthSelectionTool = AthTruthSelectionTool
tool1.useTrackSelection = False
tool1.TrackSelectionTool=InDetTrackSelectorTool
tool1.PileupSwitch=MyArgs.HSFlag
tool1.FillTrackInJetPlots = MyArgs.doTracksInJets
tool1.FillTrackInBJetPlots = MyArgs.doTracksInBJets
tool1.SkillLevel = 201 if MyArgs.doExpertPlots else 1
print (tool1)
tool1.ManagerName = 'PhysValMonManager'
monMan.AthenaMonTools += [tool1]

if MyArgs.doTightPrimary:
  from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValMonitoringTool
  toolTP = InDetPhysValMonitoringTool("IDPVMTool_TightPrimary")
  toolTP.TruthSelectionTool = AthTruthSelectionTool
  toolTP.useTrackSelection = True
  toolTP.SubFolder = "TightPrimary/"
  toolTP.PileupSwitch=MyArgs.HSFlag
  toolTP.TrackSelectionTool=InDetTrackSelectorTool
  toolTP.FillTrackInJetPlots = MyArgs.doTracksInJets
  toolTP.FillTrackInBJetPlots = MyArgs.doTracksInBJets
  tool1.SkillLevel = 201 if MyArgs.doExpertPlots else 1
  print (toolTP)
  toolTP.ManagerName = 'PhysValMonManager'
  monMan.AthenaMonTools += [toolTP  ]

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["M_output DATAFILE='"+MyArgs.outputFile+"' OPT='RECREATE'"]

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000
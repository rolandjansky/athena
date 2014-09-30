from PFODQA.PFODQAConf import PhysValPFO

PhysValPFO_charged = PhysValPFO("PhysValPFO_charged")
PhysValPFO_charged.OutputLevel = WARNING
PhysValPFO_charged.DetailLevel = 10
PhysValPFO_charged.EnableLumi = False

from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool
RetrievePFOTool = CP__RetrievePFOTool()
ToolSvc += RetrievePFOTool
PhysValPFO_charged.RetrievePFOTool = RetrievePFOTool

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += PhysValPFO_charged
monMan.AthenaMonTools += [ PhysValPFO_charged ]

PhysValPFO_neutral_EM = PhysValPFO("PhysValPFO_neutral")
PhysValPFO_neutral_EM.OutputLevel = WARNING
PhysValPFO_neutral_EM.DetailLevel = 10
PhysValPFO_neutral_EM.EnableLumi = False
PhysValPFO_neutral_EM.useNeutralPFO = True
PhysValPFO_neutral_EM.fillEMHistograms = True
PhysValPFO_neutral_EM.RetrievePFOTool = RetrievePFOTool

ToolSvc += PhysValPFO_neutral_EM
monMan.AthenaMonTools += [ PhysValPFO_neutral_EM ]

PhysValPFO_neutral_LC = PhysValPFO("PhysValPFO_neutral_LC")
PhysValPFO_neutral_LC.OutputLevel = WARNING
PhysValPFO_neutral_LC.DetailLevel = 10
PhysValPFO_neutral_LC.EnableLumi = False
PhysValPFO_neutral_LC.useLCScale = True
PhysValPFO_neutral_LC.useNeutralPFO = True
PhysValPFO_neutral_LC.RetrievePFOTool = RetrievePFOTool

ToolSvc += PhysValPFO_neutral_LC
monMan.AthenaMonTools += [ PhysValPFO_neutral_LC ]

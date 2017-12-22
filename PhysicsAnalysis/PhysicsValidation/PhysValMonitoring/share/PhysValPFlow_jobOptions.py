#Please note that to access LC PFlow container, one should run the below tool to combine the two neutral containers into one
#That then puts a new container in StoreGate, which PhysValPFO_neutral could be updated to access
#from PFlowUtils.PFlowUtilsConf import CombinePFO

from PFODQA.PFODQAConf import PhysValPFO

PhysValPFO_charged = PhysValPFO("PhysValPFO_charged")
PhysValPFO_charged.OutputLevel = WARNING
PhysValPFO_charged.DetailLevel = 10
PhysValPFO_charged.EnableLumi = False

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += PhysValPFO_charged
monMan.AthenaMonTools += [ PhysValPFO_charged ]

PhysValPFO_neutral_EM = PhysValPFO("PhysValPFO_neutral")
PhysValPFO_neutral_EM.OutputLevel = WARNING
PhysValPFO_neutral_EM.DetailLevel = 10
PhysValPFO_neutral_EM.EnableLumi = False
PhysValPFO_neutral_EM.useNeutralPFO = True

ToolSvc += PhysValPFO_neutral_EM
monMan.AthenaMonTools += [ PhysValPFO_neutral_EM ]

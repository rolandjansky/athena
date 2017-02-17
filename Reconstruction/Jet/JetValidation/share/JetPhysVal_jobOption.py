# Inspired by PhysVal_jobOptions.py

# Set up the reading of the input xAOD:
FNAME = "AOD.pool.root"
#FNAME = "/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/AOD/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.AOD.e1728_s1581.20_0_0.039772.pool.root.1"
include( "AthenaPython/iread_file.py" )

# only for testing...
# import AthenaPoolCnvSvc.ReadAthenaPool
# ServiceMgr.EventSelector.InputCollections = [ FNAME ]


from JetValidation.PhysicsValidationHistos import athenaPhysValTool
tool1 = athenaPhysValTool()
tool1.EnableLumi = False
tool1.OutputLevel = WARNING
tool1.DetailLevel = 10

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += tool1

monMan = CfgMgr.AthenaMonManager("PhysValMonManager")
monMan.FileKey = "PhysVal"
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

monMan.AthenaMonTools += [ tool1 ]
topSequence += monMan



from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]


from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

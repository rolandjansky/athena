# $Id: PhysVal_jobOptions.py 597458 2014-05-16 13:00:31Z cristinz $

# Set up the reading of the input xAOD:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )
#import AthenaPoolCnvSvc.ReadAthenaPool
#ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Default 'do' list:
validations = ["InDet","PrimaryTracking", "SecondaryTracking", "Jet", "MET", "Muon", "SlowMuon", "Egamma", "Tau", "Btag",
               "SMZee", "SMZMet", "HSG6", "Top", "SUSY", "Exotics",
               "Example"]

# All false by defualt for now except example
for val in validations:
    vars()['do' + val] = False

doExample = True
#doInDet = True
#doMET = True
#doEgamma = True
#doMuon = True
#doTau = True
#doJet = True
#doSUSY = True
#doTop = True


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.FileKey = "PhysVal"
topSequence += monMan


for val in validations:
    if not vars()['do' + val]: continue
    include("PhysValMonitoring/PhysVal" + val + "_jobOptions.py")

for tool in monMan.AthenaMonTools:
    tool.EnableLumi = False
    
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["PhysVal DATAFILE='PhysVal.root' OPT='RECREATE'"]

#topSequence.PanTau_SeedBuilder_eflowRec.Enable = False
#topSequence.PanTau_SeedBuilder_CellBased.Enable = False

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

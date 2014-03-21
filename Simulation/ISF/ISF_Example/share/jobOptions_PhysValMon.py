# *************************
# Application configuration
# *************************
### This joboption bases on PhysicsAnalysis/PhysicsValidation/PhysValMon/share/PhysValMonFCT.py, Thanks to the author ####


##EvtMax = 400
##SkipEvents = 0
##InputCollections = ["ttbar_AtlfastII_AOD_400.pool.root"]
##OutputFileName = "Hist_50.root"
##DetDescrVersion = "ATLAS-GEO-20-00-01"
##TriggerSwitch  = "NONE"
##CounterCheck = False
##WriteReference = False
##ConditionsTag = "OFLCOND-MC12-SIM-00" 

options=locals()
options.setdefault('EvtMax',50)
options.setdefault('SkipEvents',0)
options.setdefault('InputCollections',"ttbar_AtlfastII_AOD_50.pool.root")  ###not exist in current dir, should be changed to an exist file 
options.setdefault('DetDescrVersion','ATLAS-GEO-20-00-01')
options.setdefault('OutputFileName','Hist_50.root')
options.setdefault('TriggerSwitch','NONE')
options.setdefault('CounterCheck',False)
options.setdefault('WriteReference',False)
options.setdefault('ConditionsTag','OFLCOND-MC12-SIM-00')


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput.set_Value_and_Lock([InputCollections])

print 'IACOPO Conditions ' + str(ConditionsTag)

TriggerOn = 0

if TriggerSwitch != "NONE":
    TriggerOn = 1

# *************************

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo = 'atlas'
GlobalFlags.DataSource = 'geant4'
GlobalFlags.InputFormat = 'pool'



include ("RecExCond/AllDet_detDescr.py")

from RecExConfig.RecFlags  import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteAOD=False
rec.doWriteESD=False

from AthenaCommon.AppMgr import theApp
IOVDbSvc = theApp.service('IOVDbSvc')
IOVDbSvc.GlobalTag = ConditionsTag

import JetRec.ParticleJetCompatibility

if TriggerOn == 1:
    # Set up trigger configuration service and metadata service it relies on, for analysis job without RecExCommon
    # set up trigger decision tool

        # set up metadata services ... NB comment if "aod" state is selected above
    from EventInfoMgt.EventInfoMgtConf import TagInfoMgr
    ServiceMgr += TagInfoMgr()
   #ServiceMgr.TagInfoMgr.AddGeoModelTags = False
    from IOVDbMetaDataTools.IOVDbMetaDataToolsConf import IOVDbMetaDataTool
    ToolSvc += IOVDbMetaDataTool( "IOVDbMetaDataTool" )
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
    ServiceMgr += MetaDataSvc( "MetaDataSvc" )
    ServiceMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
    ServiceMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]
    import IOVDbSvc.IOVDb


    from RecExConfig.RecFlags  import rec
    rec.readAOD=True
    rec.doWriteAOD=False
    rec.doWriteESD=False
    
#    from TrigDecisionTool.TrigDecisionConf import TrigDecisionTool
#    tdt = TrigDecisionTool()
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt = Trig__TrigDecisionTool()
    ToolSvc += tdt
    # flags needed for TriggerConfigGetter

    
    # To read AOD produced with 13.0.30 you need to change ds to aod:
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    TriggerFlags.configurationSourceList = ['ds']
    
    # set up trigger config service
    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg =  TriggerConfigGetter()

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = INFO

## read pool data


import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = [InputCollections]

## get the one and only one Athena Application Mgr
from AthenaCommon.AppMgr import theApp
theApp.EvtMax  = EvtMax

## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "AthenaMonManagerPV" )

# ************************
# Monitoring configuration
# ************************

## Setup the output file(s):
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally

if OutputFileName != 'NONE':
    svcMgr.THistSvc.Output += ["PhysValMon DATAFILE='" + OutputFileName + "' OPT='RECREATE'"]
else:
    svcMgr.THistSvc.Output += ["PhysValMon DATAFILE='PhysValMon.root' OPT='RECREATE'"]

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = topSequence.AthenaMonManagerPV

## FileKey must match that given to THistSvc
monMan.FileKey = "PhysValMon"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1
monMan.OutputLevel         = INFO

# ****************************************
# Configuration of PhysValMon tools
# ****************************************

from PhysValMon.PhysValMonConf import *

# configuration of the muon tools
# ****************************************

TopoClusterTool = PhysValMonTopoCluster("TopoClusterTool", TopoClusterContainer = "CaloCalTopoCluster")
TopoClusterTool.DoOnTheFlyCheck = CounterCheck
TopoClusterTool.WriteReference = WriteReference
ToolSvc += TopoClusterTool
monMan.AthenaMonTools += [ TopoClusterTool ]

MuidMuonTool = PhysValMonMuon("MuidMuonTool", MuonContainer = "MuidMuonCollection", DoTrigger = TriggerOn)
ToolSvc += MuidMuonTool
MuidMuonTool.DoOnTheFlyCheck = CounterCheck
MuidMuonTool.WriteReference = WriteReference
monMan.AthenaMonTools += [ MuidMuonTool ]


StacoMuonTool = PhysValMonMuon("StacoMuonTool", MuonContainer = "StacoMuonCollection", DoTrigger = TriggerOn)
StacoMuonTool.DoOnTheFlyCheck = CounterCheck
StacoMuonTool.WriteReference = WriteReference
ToolSvc += StacoMuonTool
monMan.AthenaMonTools += [ StacoMuonTool ]

# configuration of the electron tools
# ****************************************

import MCTruthClassifier.MCTruthClassifierBase


ElectronTool = PhysValMonElectron("ElectronTool", ElectronContainer = "ElectronAODCollection", DoTrigger = TriggerOn)
ElectronTool.DoOnTheFlyCheck = CounterCheck
ElectronTool.WriteReference = WriteReference
ToolSvc += ElectronTool
monMan.AthenaMonTools += [ ElectronTool ]

# configuration of the photon tools
# ****************************************

PhotonTool = PhysValMonPhoton("PhotonTool", PhotonContainer = "PhotonAODCollection", DoTrigger = TriggerOn)
PhotonTool.DoOnTheFlyCheck = CounterCheck
PhotonTool.WriteReference = WriteReference
ToolSvc += PhotonTool
monMan.AthenaMonTools += [ PhotonTool ]

# configuration of the jet tools
# ****************************************

#AntiKt4TowerJetTool = PhysValMonJet("AntiKt4TowerJetTool", JetContainer = "AntiKt4TowerAODJets", TruthJetContainer = "AntiKt4TruthJets", DoTrigger = TriggerOn)
#AntiKt4TowerJetTool.DoOnTheFlyCheck = CounterCheck
#AntiKt4TowerJetTool.WriteReference = WriteReference
AntiKt4TopoJetTool = PhysValMonJet("AntiKt4TopoJetTool", JetContainer = "AntiKt4TopoEMJets", TruthJetContainer = "AntiKt4TruthJets", DoTrigger = TriggerOn)
AntiKt4TopoJetTool.DoOnTheFlyCheck = CounterCheck
AntiKt4TopoJetTool.WriteReference = WriteReference

AntiKt6TowerJetTool = PhysValMonJet("AntiKt6TowerJetTool", JetContainer = "AntiKt6TopoEMJets", TruthJetContainer = "AntiKt6TruthJets", DoTrigger = TriggerOn)

#AntiKt4TowerJetTool.OutputLevel=VERBOSE
AntiKt4TopoJetTool.OutputLevel=VERBOSE

#ToolSvc += AntiKt4TowerJetTool
ToolSvc += AntiKt4TopoJetTool
ToolSvc += AntiKt6TowerJetTool

#monMan.AthenaMonTools += [ AntiKt4TowerJetTool ]
monMan.AthenaMonTools += [ AntiKt4TopoJetTool ]
monMan.AthenaMonTools += [ AntiKt6TowerJetTool]
#AntiKt4TowerJetBTaggingTool = PhysValMonBTagging("AntiKt4TowerJetBTaggingTool",
#JetContainer = "AntiKt4TowerJets", TruthParticleContainer = "Spc1MC" )

# AntiKt4TowerJetBTaggingTool.OutputLevel=1

#ToolSvc += AntiKt4TowerJetBTaggingTool
#monMan.AthenaMonTools += [ AntiKt4TowerJetBTaggingTool ]

# configuration of the met tools
# ****************************************

from AthenaCommon.Resilience import protectedInclude
protectedInclude("PhysValMon/PhysValMonMet_jobOptions.py")

# configuration of the tau tools
# ****************************************
TauRecContainerTool = PhysValMonTau("TauRecContainerTool", TauContainer = "TauRecContainer", DoTrigger = TriggerOn)
TauRecContainerTool.DoOnTheFlyCheck = CounterCheck
TauRecContainerTool.WriteReference = WriteReference
ToolSvc += TauRecContainerTool
monMan.AthenaMonTools += [ TauRecContainerTool ]

#configuration of the topoCluster toop

from AthenaCommon.Resilience import protectedInclude
protectedInclude("HiggsValidation/HiggsValidation_montools.py")

#  LocalWords:  TowerJetTool

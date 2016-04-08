# main jobOption


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.117050.PowhegPythia_P2011C_ttbar.merge.HITS.e2658_s1967_s1964/RDO.0119996._000032.pool.root.1"]

jp.AthenaCommonFlags.PoolESDOutput="theoutput_EFonly.ESD.pool.root"
if not 'rec.doWriteESD' in dir():
    rec.doWriteESD=True

jp.AthenaCommonFlags.EvtMax=-1

rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 
rec.doTrigger=True


def muonOnly():
    TriggerFlags.Slices_all_setOff()
#    TriggerFlags.MuonSlice.setAll()
    TriggerFlags.MuonSlice.signatures=[['mu0noL1_fsperf',          '',         [], ['Muon'], ['RATE:SingleMuon', 'BW:Muon'],-1]]

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doHLT=True

from TriggerMenu.menu.GenerateMenu import GenerateMenu
GenerateMenu.overwriteSignaturesWith(muonOnly)


# main jobOption

include ("RecExCommon/RecExCommon_topOptions.py")

StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = True

from MuonTruthAlgs.MuonTruthAlgsConf import MuonDetailedTrackTruthMaker
from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSelector
from TrkTruthAlgs.TrkTruthAlgsConf import TrackParticleTruthAlg

cols   =  ["HLT_TrackCollection_forCB","HLT_TrackCollection_MuonEFCombTrkTracks"]
colsTP =  ["HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles","HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles"]

topSequence += MuonDetailedTrackTruthMaker(name="TrigMuEFonlyDetailedTrackTruthMaker", TrackCollectionNames = cols,OutputLevel=INFO )

for i in range(0, len(cols) ):
    topSequence += TrackTruthSelector(name= cols[i] + "Selector",
                                          DetailedTrackTruthName   = cols[i] + "Truth",
                                          OutputName               = cols[i] + "Truth",OutputLevel=INFO )
    topSequence += TrackParticleTruthAlg(name = cols[i]+"TruthAlg",
                                         TrackTruthName=cols[i]+"Truth",
                                         TrackParticleName = colsTP[i],OutputLevel=INFO )
from MuonTruthAlgs.MuonTruthAlgsConf import Muon__MuonTruthDecorationAlg
topSequence += Muon__MuonTruthDecorationAlg("EFonlyMuonTruthDecorationAlg",
                                            MuonTruthParticleContainerName = "HLT_EFMuonTruthParticle",
                                            CreateTruthSegments = False,OutputLevel=INFO)
from MuonTruthAlgs.MuonTruthAlgsConf import MuonTruthAssociationAlg
topSequence += MuonTruthAssociationAlg("EFonlyMuonTruthAssociation",
                                             MuonContainerName = "HLT_xAOD__MuonContainer_MuonEFInfo",
                                             MuonTruthParticleContainerName = "HLT_EFMuonTruthParticle",OutputLevel=INFO)

doMon = True
if( doMon ):
 from MuonPhysValMonitoring.MuonPhysValMonitoringConf import MuonPhysValMonitoring__MuonPhysValMonitoringTool
 tool1 = MuonPhysValMonitoring__MuonPhysValMonitoringTool("EFonlyMuonPhysValMonitoringTool")
 tool1.EnableLumi = False
 tool1.DetailLevel = 1
 tool1.OutputLevel = INFO
 tool1.MuonContainerName =  "HLT_xAOD__MuonContainer_MuonEFInfo"
 tool1.MuonTruthParticleContainerName = "HLT_EFMuonTruthParticle"
       
 ToolSvc += tool1

 from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
 topSequence += AthenaMonManager( "PhysValMonManager" ,OutputLevel=DEBUG)

 from AthenaCommon.AppMgr import ServiceMgr
 from GaudiSvc.GaudiSvcConf import THistSvc
 ServiceMgr += THistSvc()
 svcMgr.THistSvc.Output += ["EFonlyMuonMonExample DATAFILE='EFonlyMuonMonExample.root' OPT='RECREATE'"]
        
 monMan = topSequence.PhysValMonManager
 monMan.ManualDataTypeSetup = True
 monMan.DataType            = "monteCarlo"
 monMan.Environment         = "altprod"
 monMan.ManualRunLBSetup    = True
 monMan.Run                 = 1
 monMan.LumiBlock           = 1
        
 monMan.FileKey = "EFonlyMuonMonExample"
 monMan.AthenaMonTools += [ tool1 ]




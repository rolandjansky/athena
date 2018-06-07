import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from glob import glob
#filelist = ["DAOD_JETM2.test.TruthAssoc.pool.root"]
#filelist = ["/afs/cern.ch/work/r/rsmith/public/METUtilities_testfiles/valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.recon.AOD.e3099_s1982_s1964_r6006_tid04628718_00/AOD.04628718._000158.pool.root.1"]
filelist = ["/sps/atlas/a/atrofymo/athena_test/athena/Reconstruction/MET/METReconstruction/share/xAOD3.pool.root"]
#xAOD3.pool_PFlowJet_2.root
#xAOD3.pool_PFlowJetRecoil_2.root
ServiceMgr.EventSelector.InputCollections = filelist

from METUtilities.METMakerConfig import getMETMakerAlg

import inspect
import os
print os.path.dirname(os.path.abspath(inspect.getsourcefile(getMETMakerAlg)))



#metAlg = getMETMakerAlg('AntiKt4EMTopo',20e3) # original
#metAlg.METName = 'MET_Reco_AntiKt4EMTopo' # original
#metAlg = getMETMakerAlg('AntiKt4EMPFlow',jetColl="AntiKt4EMPFlowJets")  
#metAlg = getMETMakerAlg('RecoilPFlow', jetColl="AntiKt4EMPFlowJets") 
#metAlg = getMETMakerAlg('RecoilPFlow', jetColl="RecoilPFlowJets") 

metAlg = getMETMakerAlg("AntiKt4EMPFlow",jetColl="AntiKt4EMPFlowJets",setJetMinWPtToInf=True)


# original:
#metAlg_truth = getMETMakerAlg('Truth_AntiKt4EMTopo',20e3)
#etAlg_truth.METSoftClName = 'SoftTruthAll'
#metAlg_truth.METSoftTrkName = 'SoftTruthCharged'
#metAlg_truth.METName = 'MET_Truth_AntiKt4EMTopo'

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += metAlg
#topSequence += metAlg_truth

write_xAOD = True
if write_xAOD:
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.METMaker.pool.root" )
    # original:
    #xaodStream.AddItem('xAOD::MissingETContainer#MET_Reco_AntiKt4EMTopo')
    #xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Reco_AntiKt4EMTopoAux.')

    # added 2:
    xaodStream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")

    xaodStream.AddItem("xAOD::JetContainer#AntiKt4EMPFlowJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.")

    xaodStream.AddItem("xAOD::PFOContainer#*ParticleFlowObjects*")
    xaodStream.AddItem("xAOD::PFOAuxContainer#*ParticleFlowObjects*")
    xaodStream.AddItem("xAOD::ShallowAuxContainer#*ParticleFlowObjects*")

    xaodStream.AddItem('xAOD::MissingETContainer_v1#MET_PFlow')
    xaodStream.AddItem('xAOD::MissingETAuxContainer_v1#MET_PFlowAux.')

    xaodStream.AddItem("xAOD::EventInfo#EventInfo")
    xaodStream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
    xaodStream.AddItem("xAOD::EventShape#*")
    xaodStream.AddItem("xAOD::EventShapeAuxInfo#*")

    xaodStream.AddItem("xAOD::MuonContainer#Muons")
    xaodStream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")

    xaodStream.AddItem("xAOD::ElectronContainer#Electrons")
    xaodStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")

    xaodStream.AddItem("xAOD::PhotonContainer#Photons")
    xaodStream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")

    xaodStream.AddItem("xAOD::TauJetContainer#TauJets")
    xaodStream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.")

    xaodStream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.")
    xaodStream.AddItem("xAOD::JetContainer#AntiKt4LCTopoJets")
    xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4LCTopoJetsAux.")
    xaodStream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoCluster")
    xaodStream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClusterAux.")
    xaodStream.AddItem("xAOD::CaloClusterContainer#egClusterCollection")
    xaodStream.AddItem("xAOD::CaloClusterAuxContainer#egClusterCollectionAux.")
    xaodStream.AddItem("xAOD::TrackParticleContainer#CombinedMuonTrackParticles")
    xaodStream.AddItem("xAOD::TrackParticleAuxContainer#CombinedMuonTrackParticlesAux.")
    xaodStream.AddItem("xAOD::TrackParticleContainer#ExtrapolatedMuonTrackParticles")
    xaodStream.AddItem("xAOD::TrackParticleAuxContainer#ExtrapolatedMuonTrackParticlesAux.")
    xaodStream.AddItem("xAOD::TrackParticleContainer#MuonSpectrometerTrackParticles")
    xaodStream.AddItem("xAOD::TrackParticleAuxContainer#MuonSpectrometerTrackParticlesAux.")
    xaodStream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
    xaodStream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux.")

    xaodStream.AddItem('xAOD::TrackParticleContainer#InDetTrackParticles*')
    xaodStream.AddItem('xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.')


    # original:
    #xaodStream.AddItem('xAOD::MissingETContainer#MET_Truth_AntiKt4EMTopo')
    #xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Truth_AntiKt4EMTopoAux.')
    # original:
    #xaodStream.AddItem('xAOD::MissingETContainer#MET_Truth')
    #xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_TruthAux.')
    #
    xaodStream.Print()

# from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
# HistoXAODStream = MSMgr.NewRootStream( streamName, fileName )

#from Valkyrie.JobOptCfg import ValgrindSvc
#svcMgr += ValgrindSvc( OutputLevel = DEBUG,
#                       ProfiledAlgs = ["METMakerAlg"],
#                       ProfiledIntervals = ["METMakerAlg.execute"])

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

ServiceMgr.MessageSvc.defaultLimit = 9999999
theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0

printint = max(theApp.EvtMax/250,1) if theApp.EvtMax>0 else 1000
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=printint)

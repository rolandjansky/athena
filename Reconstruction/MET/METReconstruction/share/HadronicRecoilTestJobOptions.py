import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from glob import glob


#filelist = glob("/atlas/data1/userdata/khoo/Data16/AOD_r21/data16_13TeV.00302347.express_express.recon.AOD.r9112/*")
#filelist = glob("/sps/atlas/a/atrofymo/data16_13TeV.00302347.express_express.recon.AOD.r9112/*")
filelist = glob("/sps/atlas/a/atrofymo/lowmu_AOD_mc_zee/mc16_13TeV/AOD.13048120._000001.pool.root.1")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = filelist
#athenaCommonFlags.FilesInput = [Inputfile]
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()


############################################################################
# Set up detector description for cell access

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.InputFilePeeker import inputFileSummary
#print inputFileSummary
if inputFileSummary['evt_type'][0] == 'IS_DATA':
    globalflags.DataSource = 'data'
else:
    globalflags.DataSource = 'geant4'

globalflags.DetDescrVersion = inputFileSummary['geometry']

from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.Calo_setOn()
if hasattr(DetFlags,'BField_on'): DetFlags.BField_setOn()
include('RecExCond/AllDet_detDescr.py')
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


####### CHSParticleFlowObjects
from JetRec.JetRecConf import JetToolRunner
from RecExConfig.AutoConfiguration import IsInInputFile
containerexists = IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects")

algexists = hasattr(topSequence,"jetalgCHSPFlow")

print "containerexists = ", containerexists
print "algexists       = ", algexists

if not (containerexists or algexists):
    from JetRec.JetRecStandard import jtm
    jtm += JetToolRunner("jetconstitCHSPFlow",
                         EventShapeTools=[],
                         Tools=[jtm.JetConstitSeq_PFlowCHS],
                         )
    print "adding JetToolRunner"
    from JetRec.JetRecConf import JetAlgorithm
    topSequence += JetAlgorithm("jetalgCHSPFlow", Tools=[jtm.jetconstitCHSPFlow])
    print "adding JetAlgorithm"   
#######

print "!!!!!!!!topSequence: ", topSequence


from GaudiSequencer.PyComps import PyEvtFilter
filterseq = CfgMgr.AthSequencer("AthFilterSeq")
#the following lines are examples, pick one...
# filterseq += PyEvtFilter("PVSoftTrkTail", evt_list=[
#         # 106239409,
#         # 103677144,
#         # 210091212,
#         # 647377331,
#         # 649083254,
#         # 679943194,
#         # 676957592,
#         # 931088975,
#         # 930867220,
#         # 932105457,
#         # 932761543,
#         # 500249532,
#         # 498552012,
#         # 8093981,
#         # 7747623,
#         # 9713934,
#         ])
topSequence += filterseq

############################################################################
# Set up an extra associator for testing
from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METAssocConfig import AssocConfig, METAssocConfig


import inspect
import os
print os.path.dirname(os.path.abspath(inspect.getsourcefile(AssocConfig)))

############################################################################
# Set up an extra associator for testing

JetType = 'PFlowJet'
associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Soft')]
cfg_akt4pf = METAssocConfig('NewAntiKt4EMPFlow',
#cfg_akt4pf = METAssocConfig('AntiKt4EMPFlow',
                            associators,
                            doPFlow=True,
                            doRecoil=False,
                            #modConstKey="JetETMissNeutralParticleFlowObjects" 
                            modConstKey="CHSParticleFlowObjects"
                            )

metFlags.METAssocConfigs()[cfg_akt4pf.suffix] = cfg_akt4pf
metFlags.METAssocOutputList().append(cfg_akt4pf.suffix)




#metFlags.AllowOverwrite = True 
JetType = 'PFlowJetRecoil'
associators = [AssocConfig(JetType),
               AssocConfig('Muon'),
               AssocConfig('Ele'),
               AssocConfig('Gamma'),
               AssocConfig('Soft')]
#cfg_akt4pf = METAssocConfig('RecoilPFlow',
cfg_akt4pfHR = METAssocConfig('NewAntiKt4EMPFlowHR', 
#cfg_akt4pf = METAssocConfig('AntiKt4EMPFlow',  
                            associators,
                            doPFlow=True,
                            doRecoil=True,
                            #modConstKey="JetETMissNeutralParticleFlowObjects"
                            modConstKey="CHSParticleFlowObjects"
                            )

metFlags.METAssocConfigs()[cfg_akt4pfHR.suffix] = cfg_akt4pfHR
metFlags.METAssocOutputList().append(cfg_akt4pfHR.suffix)



from METReconstruction.METAssocConfig import getMETAssocAlg

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
metAlg = getMETAssocAlg('METAssociation')
filterseq += metAlg

from METUtilities.METMakerConfig import getMETMakerAlg

#makerAlgEM = getMETMakerAlg("NewAntiKt4EMTopo",jetColl="AntiKt4EMTopoJets") # original
# ToolSvc.METMaker_NewAntiKt4EMTopo.OutputLevel=VERBOSE
#ToolSvc.METMaker_NewAntiKt4EMTopo.DoRemoveElecTrks=False # artur commented
#filterseq += makerAlgEM  # artur commented

makerAlgPF = getMETMakerAlg("NewAntiKt4EMPFlow",jetColl="AntiKt4EMPFlowJets",setJetMinWPtToInf=False)
ToolSvc.METMaker_NewAntiKt4EMPFlow.OutputLevel=VERBOSE
#ToolSvc.METMaker_NewAntiKt4EMPFlow.DoRemoveElecTrks=False
filterseq += makerAlgPF


makerAlgPF_HR = getMETMakerAlg("NewAntiKt4EMPFlowHR",jetColl="AntiKt4EMPFlowJets",setJetMinWPtToInf=True)
ToolSvc.METMaker_NewAntiKt4EMPFlowHR.OutputLevel=VERBOSE
# ToolSvc.METMaker_NewAntiKt4EMPFlowHR.DoRemoveElecTrks=False
filterseq += makerAlgPF_HR


write_xAOD = True
if write_xAOD:

    # The list of output containers/maps is autogenerated and stored in metFlags
    # This jO extracts them with the appropriate formatting
    from AthenaCommon.Resilience import protectedInclude
    protectedInclude("METReconstruction/METReconstructionOutputAODList_jobOptions.py")

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD3.pool.root" )
    for item in MissingETAODList:
        #print ' item = ', item
        xaodStream.AddItem(item)

    xaodStream.AddItem('xAOD::MissingETContainer#MET_Reference_Anti*')
    xaodStream.AddItem('xAOD::MissingETAuxContainer#MET_Reference_Anti*Aux.')

    # added:
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
    #xaodStream.AddItem("xAOD::ElectronContainer#ElectronCollection")
    #xaodStream.AddItem("xAOD::ElectronAuxContainer#ElectronCollectionAux.")
    xaodStream.AddItem("xAOD::ElectronContainer#Electrons")
    xaodStream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
    #xaodStream.AddItem("xAOD::PhotonContainer#PhotonCollection")
    #xaodStream.AddItem("xAOD::PhotonAuxContainer#PhotonCollectionAux.")
    xaodStream.AddItem("xAOD::PhotonContainer#Photons")
    xaodStream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")
    #xaodStream.AddItem("xAOD::TauJetContainer#TauRecContainer")
    #xaodStream.AddItem("xAOD::TauJetAuxContainer#TauRecContainerAux.")
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


    # xaodStream.AddAcceptAlgs( "PVSoftTrkTail" )
theApp.EvtMax = 10
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999
#ServiceMgr.MessageSvc.defaultLimit = 1




print '\n -----------------'

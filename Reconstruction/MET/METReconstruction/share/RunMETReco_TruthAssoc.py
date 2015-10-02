import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

filelist = ["/r04/atlas/khoo/Data_2015/xAOD/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264_tid05382619_00/AOD.05382619._000060.pool.root.1"]
ServiceMgr.EventSelector.InputCollections = filelist

## Truth association maps
from METReconstruction.METAssocConfig import METAssocConfig, AssocConfig
from METReconstruction.METRecoFlags import metFlags

# ############################################################################
# # AntiKt4LCTopo
# cfg_truthassoc_akt4lc = METAssocConfig('Truth_AntiKt4LCTopo',
#                                        [AssocConfig('Truth','AntiKt4LCTopoJets')],
#                                        doTruth=True
#                                        )

# metFlags.METAssocConfigs()[cfg_truthassoc_akt4lc.suffix] = cfg_truthassoc_akt4lc
# metFlags.METAssocOutputList().append(cfg_truthassoc_akt4lc.suffix)

############################################################################
# AntiKt4EMTopo
cfg_truthassoc_akt4em = METAssocConfig('Truth_AntiKt4EMTopo',
                                       [AssocConfig('Truth','AntiKt4EMTopoJets')],
                                       doTruth=True
                                       )

metFlags.METAssocConfigs()[cfg_truthassoc_akt4em.suffix] = cfg_truthassoc_akt4em
metFlags.METAssocOutputList().append(cfg_truthassoc_akt4em.suffix)

# JetType = 'EMJet'
# associators = [AssocConfig(JetType),
#                AssocConfig('Muon'),
#                AssocConfig('Ele'),
#                AssocConfig('Gamma'),
#                AssocConfig('Tau'),
#                AssocConfig('Soft')]

# cfg_assoc_akt4em = METAssocConfig('NewAntiKt4EMTopo',
#                                   associators)

# metFlags.METAssocConfigs()[cfg_assoc_akt4em.suffix] = cfg_assoc_akt4em
# metFlags.METAssocOutputList().append(cfg_assoc_akt4em.suffix)

# ############################################################################
# # AntiKt4EMPFlow
# cfg_truthassoc_akt4pf = METAssocConfig('Truth_AntiKt4EMPFlow',
#                                        [AssocConfig('Truth','AntiKt4EMPFlowJets')],
#                                        doTruth=True
#                                        )

# metFlags.METAssocConfigs()[cfg_truthassoc_akt4pf.suffix] = cfg_truthassoc_akt4pf
# metFlags.METAssocOutputList().append(cfg_truthassoc_akt4pf.suffix)

# Get the configuration directly from METRecoFlags
# Can also provide a dict of configurations or list of RecoTools or both
from METReconstruction.METAssocConfig import getMETAssocAlg
assocAlg = getMETAssocAlg('METAssociation')

from METUtilities.METMakerConfig import getMETMakerAlg
# metAlg = getMETMakerAlg('AntiKt4EMTopo',20e3)
# metAlg.METName = 'MET_Reco_AntiKt4EMTopo'

# metAlg_new = getMETMakerAlg('NewAntiKt4EMTopo',20e3,'AntiKt4EMTopoJets')
# metAlg_new.METName = 'MET_Reco_NewAntiKt4EMTopo'

metAlg_truth = getMETMakerAlg('Truth_AntiKt4EMTopo',20e3)
metAlg_truth.METSoftClName = 'SoftTruthAll'
metAlg_truth.METSoftTrkName = 'SoftTruthCharged'
#metAlg_truth.InputMuons=""
ToolSvc.METMaker_Truth_AntiKt4EMTopo.JetConstitScaleMom = ""
ToolSvc.METMaker_Truth_AntiKt4EMTopo.JetJvtCut=-1
metAlg_truth.METName = 'MET_Truth_AntiKt4EMTopo'

metAlg_truthconst = getMETMakerAlg('TruthConst_AntiKt4EMTopo',20e3)
metAlg_truthconst.METSoftClName = 'SoftTruthAll'
metAlg_truthconst.METSoftTrkName = 'SoftTruthCharged'
metAlg_truthconst.METMapName='METAssoc_Truth_AntiKt4EMTopo'
metAlg_truthconst.METCoreName='MET_Core_Truth_AntiKt4EMTopo'
ToolSvc.METMaker_TruthConst_AntiKt4EMTopo.DoJetTruth = True
ToolSvc.METMaker_TruthConst_AntiKt4EMTopo.JetConstitScaleMom = ""
ToolSvc.METMaker_TruthConst_AntiKt4EMTopo.JetJvtCut=-1
ToolSvc.METMaker_TruthConst_AntiKt4EMTopo.CorrectJetPhi=True
#ToolSvc.METMaker_TruthConst_AntiKt4EMTopo.OutputLevel=1
metAlg_truthconst.METName = 'MET_TruthConst_AntiKt4EMTopo'

metAlg_truthlep = getMETMakerAlg('TruthLep_AntiKt4EMTopo',20e3)
metAlg_truthlep.METSoftClName = 'SoftTruthAll'
metAlg_truthlep.METSoftTrkName = 'SoftTruthCharged'
metAlg_truthlep.METMapName='METAssoc_Truth_AntiKt4EMTopo'
metAlg_truthlep.METCoreName='MET_Core_Truth_AntiKt4EMTopo'
metAlg_truthlep.DoTruthLeptons=True
ToolSvc.METMaker_TruthLep_AntiKt4EMTopo.DoJetTruth = True
ToolSvc.METMaker_TruthLep_AntiKt4EMTopo.JetConstitScaleMom = ""
ToolSvc.METMaker_TruthLep_AntiKt4EMTopo.JetJvtCut=-1
ToolSvc.METMaker_TruthLep_AntiKt4EMTopo.CorrectJetPhi=True
#ToolSvc.METMaker_TruthLep_AntiKt4EMTopo.OutputLevel=1
metAlg_truthlep.METName = 'MET_TruthLep_AntiKt4EMTopo'

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += assocAlg
#topSequence += metAlg
#topSequence += metAlg_new
topSequence += metAlg_truth
topSequence += metAlg_truthconst
topSequence += metAlg_truthlep

# The tools are accessible via the configurations in metFlags
from AthenaCommon.AppMgr import ToolSvc

from Valkyrie.JobOptCfg import ValgrindSvc
svcMgr += ValgrindSvc( OutputLevel = DEBUG,
                       ProfiledAlgs = ["METReconstruction"],
                       ProfiledIntervals = ["METReconstruction.execute"])

from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True

write_xAOD = True
if write_xAOD:

    # The list of output containers/maps is autogenerated and stored in metFlags
    # This jO extracts them with the appropriate formatting
    from AthenaCommon.Resilience import protectedInclude
    protectedInclude("METReconstruction/METReconstructionOutputAODList_jobOptions.py")

    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
    # for config in ['AntiKt4LCTopo','AntiKt4EMTopo','AntiKt4EMPFlow']:
    #     MissingETAODList.append( 'xAOD::MissingETAssociationMap#METAssoc_'+config )
    #     MissingETAODList.append( 'xAOD::MissingETAuxAssociationMap#METAssoc_'+config+'Aux.' )
    #     MissingETAODList.append( 'xAOD::MissingETContainer#MET_Core_'+config )
    #     MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Core_'+config+'Aux.' )
    #     MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reference_'+config )
    #     MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reference_'+config+'Aux.' )
    MissingETAODList.append( 'xAOD::TruthParticleContainer#TruthParticles' )
    MissingETAODList.append( 'xAOD::TruthParticleAuxContainer#TruthParticlesAux.' )
    MissingETAODList.append( 'xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux.' )
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.' )
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.' )
    # MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reco_AntiKt4EMTopo' )
    # MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reco_AntiKt4EMTopoAux.' )
    # MissingETAODList.append( 'xAOD::MissingETContainer#MET_Reco_NewAntiKt4EMTopo' )
    # MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Reco_NewAntiKt4EMTopoAux.' )
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_Truth_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_Truth_AntiKt4EMTopoAux.' )
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_TruthConst_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_TruthConst_AntiKt4EMTopoAux.' )
    MissingETAODList.append( 'xAOD::MissingETContainer#MET_TruthLep_AntiKt4EMTopo' )
    MissingETAODList.append( 'xAOD::MissingETAuxContainer#MET_TruthLep_AntiKt4EMTopoAux.' )
    for item in MissingETAODList:
        xaodStream.AddItem(item)

theApp.EvtMax = -1
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 9999999

printint = max(theApp.EvtMax/250,1) if theApp.EvtMax>0 else 1000
svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=printint)

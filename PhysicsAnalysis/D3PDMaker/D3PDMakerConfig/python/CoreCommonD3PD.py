# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: CoreGenericD3PD.py
#
# 

#-------------------------------------
# general athena and D3PD imports
#-------------------------------------
from RecExConfig.RecFlags                              import rec
from AthenaCommon.AthenaCommonFlags                    import athenaCommonFlags
from AthenaCommon.AlgSequence                          import AlgSequence
from AthenaCommon.AppMgr                               import ServiceMgr, ToolSvc, theApp

import D3PDMakerCoreComps
from D3PDMakerCoreComps.resolveSGKey                   import testSGKey

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags,_string_prop
from D3PDMakerConfig.CommonD3PDMakerFlags              import CommonD3PDMakerFlags
from D3PDMakerConfig.GenericTriggerBitsD3PDObject      import GenericTriggerBitsD3PDObject
from D3PDMakerConfig.SUSYCustomMET                     import *
#from D3PDMakerConfig.PhotonCustomMET                   import *
from D3PDMakerConfig.CommonEventShapeD3PDObject        import EventShapeD3PDObject
from D3PDMakerConfig.CoreTruthFilterTool               import CoreTruthFilterTool
from SUSYD3PDMaker.SUSYD3PDFlags                       import SUSYD3PDFlags
import EventCommonD3PDMaker
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from EventCommonD3PDMaker.CutFlowMetadataConfig        import CutFlowMetadataConfig
from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from BackgroundD3PDMaker.BeamBackgroundD3PDObject      import BeamBackgroundD3PDObject
from BackgroundD3PDMaker.BackgroundWordD3PDObject      import BackgroundWordD3PDObject
from TriggerD3PDMaker.BunchStructureMetadata           import addBunchStructureMetadata


#-------------------------------------
# reco object imports
#-------------------------------------
import CaloD3PDMaker
from CaloD3PDMaker.MBTSD3PDObject                      import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject                  import MBTSTimeD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject          import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject         import CollisionDecisionD3PDObject
from CaloD3PDMaker.ClusterD3PDObject                   import ClusterD3PDObject
from CaloD3PDMaker.makeClusterFilter                   import makeClusterFilter

from egammaD3PDMaker.ElectronD3PDObject                import ElectronD3PDObject
from egammaD3PDMaker.ElectronD3PDObject                import ElectronClusterAssoc
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonClusterAssoc
from egammaD3PDMaker.egammaTriggerBitsD3PDObject       import egammaTriggerBitsD3PDObject
import BoostedTopD3PDMaker
from TrackD3PDMaker.V0D3PDObject                       import DefaultV0D3PDObject  
from JetRecTools.JetRecToolsConf                       import *
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from TrigJetD3PDMaker.EFJetD3PDObject                  import EFJetD3PDObject  
from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject
from TauD3PDMaker.TauD3PDObject                        import TauD3PDObject
from TauD3PDMaker.makeTrackFilterAlg                   import makeTrackFilterAlg
from TopInputsD3PDMaker.HforD3PDObject                 import HforD3PDObject
from TrackD3PDMaker.TrackD3PDMakerFlags                import TrackD3PDFlags
import TrackD3PDMaker
#if(SUSYD3PDFlags.do01LepFactoryNtuple() == False): TrackD3PDFlags.storeTrackUnbiasedIPAtPV = True
from TrackD3PDMaker.TrackD3PDObject                    import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDObject                    import TrackD3PDObject
from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject
from TrackD3PDMaker.TruthVertexD3PDObject              import TruthVertexD3PDObject
from TrackD3PDMaker.BeamSpotD3PDObject                 import BeamSpotD3PDObject
from QcdD3PDMaker.QcdTriggerBitsD3PDObject             import QcdTriggerBitsD3PDObject
from QcdD3PDMaker.QcdMuonSpShowerD3PDObject            import MuonSpShowerD3PDObject

import PhotonD3PDMaker

import JetD3PDMaker
from JetD3PDMaker.JetD3PDObject                      import JetD3PDObject

#-------------------------------------
# trigger imports
#-------------------------------------
import TriggerD3PDMaker
import TrigJetD3PDMaker
from TriggerD3PDMaker.JetROID3PDObject                 import JetROID3PDObject
from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PD                import TrigMETD3PDObjects
from TrigJetD3PDMaker.TrigJetD3PD                      import InitTrigD3PDObject
from TrigJetD3PDMaker.TrigJetD3PDObject                import TrigJetD3PDObject
from TrigTauD3PDMaker.TrigEFTauD3PDObject              import TrigEFTauD3PDObject
from TrigTauD3PDMaker.TrigL2TauD3PDObject              import TrigL2TauD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
#from D3PDMakerConfig.CommonTrigEgammaD3PD                    import CommonTrigEgammaD3PDObjects
from TrigMuonD3PDMaker.TrigMuonD3PD                    import TrigMuonD3PDObjects
#from D3PDMakerConfig.CommonTrigMuonD3PD                      import CommonTrigMuonD3PDObjects
from TrigBJetD3PDMaker.TrigBJetD3PD                    import TrigBJetD3PDObjects
from TrigDecisionTool.TrigDecisionToolConf             import Trig__TrigDecisionTool
from TriggerD3PDMaker.defineTriggerBits                import defineTriggerBits
from CaloD3PDMaker.MBTSTriggerBitsD3PDObject           import MBTSTriggerBitsD3PDObject
from TrigCaloD3PDMaker.TrigEMClusterD3PDObject         import TrigEMClusterD3PDObject 
from TrigInDetD3PDMaker.TrigInDetTrackD3PDObject       import TrigInDetTrackD3PDObject 
from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import METD3PDTriggerBitsObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
import JetSubstructureD3PDMaker
import TauD3PDMaker
from D3PDMakerCoreComps.IndexAssociation           import IndexAssociation
from D3PDMakerCoreComps.SimpleAssociation          import SimpleAssociation

if rec.doTruth():
    from egammaD3PDMaker.egammaTruthD3PDObject         import egammaTruthD3PDObject
    from EventCommonD3PDMaker.DRIndexMultiAssociation  import DRIndexMultiAssociation
    import TruthD3PDMaker
    from TruthD3PDMaker.TruthParticleD3PDObject        import TruthParticleD3PDObject
    from MuonD3PDMaker.TruthMuonD3PDObject             import TruthMuonD3PDObject
    from TruthD3PDMaker.GenEventD3PDObject             import GenEventD3PDObject
    from TruthD3PDMaker.GenVertexD3PDObject            import GenVertexD3PDObject
    from TruthD3PDMaker.GenParticleD3PDObject          import GenParticleD3PDObject
    from TruthD3PDMaker.GenParticleD3PDObject          import GenTruthTrackD3PDObject
    from TruthD3PDMaker.MCTruthClassifierConfig        import D3PDMCTruthClassifier

#-------------------------------------
# SUSY imports
#-------------------------------------
from SUSYD3PDMaker.SUSYD3PDMakerConf                   import * 
from SUSYD3PDMaker.SUSYHardProcessD3PDObject           import SUSYHardProcessD3PDObject 


#D3PDMakerFlags.TruthWriteHadrons = True

#-------------------------------------
# Silence Trig Navigation Warnings
#-------------------------------------
ToolSvc += Trig__TrigDecisionTool( 'TrigDecisionTool')
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR


topSequence = AlgSequence()

#addSUSYCustomMissingET( topSequence )
#PhotonCustomMET()

ExtendedJetD3PDObject = JetD3PDObject.copy()
myMuonD3PDObject = MuonD3PDObject.copy()
ExtendedTauD3PDObject = TauD3PDObject.copy()

#from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
#ClusterD3PDObject.defineBlock (99, 'CenterMagMoments', CMFT, Moments = [CMFT.CENTER_MAG, 'centermag'])


# make truth jets WIDTH and SPLIT variables for truth jets R=0.4/0.6
import JetSubStructure
from JetRec.JetMomentGetter import  make_JetMomentGetter
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from JetRec.JetRecConf import JetSorterTool
widthtool = JetWidthTool('JS_JetWidthTool')
assoc_tool_truth = JetSubStructure.JetAssociationTool(
    name = 'JetAssocToolTruth',
    JetAlgTools = [widthtool], OutputLevel = 3)
jetsubstructure_tool_truth = JetSubStructure.JetSubstructureTool(
    name = 'JetSubstructureToolTruth', UsedSignalState = 2, OutputLevel=3,
    SubstructureTools = [JetSubStructure.KtTruthLeptonSubstructureTool("mytruthleptontool", OutputLevel=3, GroomedjetsAuthor = [])])

#make_JetMomentGetter("AntiKt4TruthNewJets", [widthtool, assoc_tool_truth, jetsubstructure_tool_truth], topSequence)
#make_JetMomentGetter("AntiKt6TruthNewJets", [widthtool, assoc_tool_truth, jetsubstructure_tool_truth], topSequence)

# SPLIT variables for R=0.4/0.6 jets.
jetsubstructure_tool = JetSubStructure.JetSubstructureTool(
    name = 'JetSubstructureTool', UsedSignalState = 2, OutputLevel=3,
    SubstructureTools = [JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = []) ])
#make_JetMomentGetter("AntiKt4TopoEMJets", [jetsubstructure_tool], topSequence)
#make_JetMomentGetter("AntiKt6TopoEMJets", [jetsubstructure_tool], topSequence)
make_JetMomentGetter("AntiKt4LCTopoNewJets", [jetsubstructure_tool], topSequence) # replacing RetaggedJets with NewJets
#make_JetMomentGetter("AntiKt6LCTopoNewJets", [jetsubstructure_tool], topSequence) # replacing RetaggedJets with NewJets

if rec.doTruth() and CommonD3PDMakerFlags.doRecTruthJet() :
    make_JetMomentGetter("AntiKt4TruthNewJets", [jetsubstructure_tool], topSequence)
    #make_JetMomentGetter("AntiKt6TruthNewJets", [jetsubstructure_tool], topSequence)
    make_JetMomentGetter("AntiKt4TruthNewJets", [widthtool, assoc_tool_truth, jetsubstructure_tool_truth], topSequence)
    #make_JetMomentGetter("AntiKt6TruthNewJets", [widthtool, assoc_tool_truth, jetsubstructure_tool_truth], topSequence)

make_JetMomentGetter("AntiKt4TrackJets", [jetsubstructure_tool], topSequence) # there is no TrackZJets but only TrackJets inside CommonJetTrackZClusterTool_jobOptions.py
#make_JetMomentGetter("AntiKt6TrackJets", [jetsubstructure_tool], topSequence)

jetsubstructure_tool_track4 = JetSubStructure.JetSubstructureTool(
    name = 'JetSubstructureToolTrack', UsedSignalState = 2, OutputLevel=3,
    SubstructureTools = [JetSubStructure.KtHadSubstructureTool("myhadtool", GroomedjetsAuthor = []),
                         JetSubStructure.DipolarityTool("myDipolarityTool",GroomedjetsAuthor = []),
                         JetSubStructure.JetShapesSubstructureTool("myJetShapesTool", GroomedjetsAuthor = []),
                         JetSubStructure.NSubjettinessTool("myNSubjettinessTool", N=[1,2,3], R0=0.4,GroomedjetsAuthor = [])])
make_JetMomentGetter("AntiKt4TrackJets", [jetsubstructure_tool_track4],  topSequence)  # there is no TrackZJets but only TrackJets inside CommonJetTrackZClusterTool_jobOptions.py
from D3PDMakerConfig.CommonJSD3PD  import AddHadronicInfo
#AddHadronicInfo(ExtendedJetD3PDObject, ['Tau1','Tau2','Tau3', 'ZCUT12', 'ZCUT23','ZCUT34','Dip12', 'Dip13', 'Dip23','DipExcl12','PlanarFlow','Angularity'], theblockname = 'substructureblocks', level=99)


#from HSG5DPDUtils.HSG5D3PD  import addSTRUCTCalib
#addSTRUCTCalib('AntiKt4TopoEMJetsReTagged', input='Topo', mainParam=0.4)
#addSTRUCTCalib('AntiKt6TopoEMJetsReTagged', input='Topo', mainParam=0.6)
#addSTRUCTCalib('AntiKt4LCTopoJetsReTagged', input='Topo', mainParam=0.4)
#addSTRUCTCalib('AntiKt6LCTopoJetsReTagged', input='Topo', mainParam=0.6)


#from JetD3PDMaker import JetMomentFillerTool
#ExtendedJetD3PDObject.defineBlock(99, 'splitInfoBlock', JetMomentFillerTool,Moments= ['SPLIT12', 'SPLIT23', 'SPLIT34'])

#------------------------------------------------
# additional define blocks to fill more variables
#------------------------------------------------
#ElectronClusterAssoc.defineBlock (0, 'ClusterTime', CaloD3PDMaker.ClusterTimeFillerTool,prefix = 'cl_')
#PhotonClusterAssoc.defineBlock (0, 'ClusterTime', CaloD3PDMaker.ClusterTimeFillerTool,prefix = 'cl_')
#ExtendedJetD3PDObject.defineBlock(99, 'Uncertainties', JetD3PDMaker.JetMomentFillerTool, Moments=['SmearingFactor'])


# track isolation for electrons, using new pt track cut -> 0.5GeV
from SUSYD3PDMaker.SUSYD3PDMakerConf import SUSYD3PD__SUSYEleTrkIsoFillerTool as SUSYEleTrkIsoFillerTool
#ElectronD3PDObject.defineBlock(1, "Isolation_500MeV", SUSYEleTrkIsoFillerTool, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool")


try:
    from egammaTools.EMTrackIsolationToolBase import EMTrackIsolationToolBase
    theemtrackisolationtool = EMTrackIsolationToolBase("emtrackisolationtool")
    theemtrackisolationtool.useBremAssoc = True
    ToolSvc += theemtrackisolationtool
except Exception:
    print traceback.format_exc()
    
ElectronD3PDObject.defineBlock(99, "miniiso10", BoostedTopD3PDMaker.MiniIsolationElectronFillerTool, EMTrackIsolationTool=theemtrackisolationtool, KtDist=10., prefix="MI10_")
ElectronD3PDObject.defineBlock(99, "miniiso15", BoostedTopD3PDMaker.MiniIsolationElectronFillerTool, EMTrackIsolationTool=theemtrackisolationtool, KtDist=15., prefix="MI15_")

# track isolation for muons, using new pt track cut -> 0.5GeV
#from SUSYD3PDMaker.SUSYD3PDMakerConf import SUSYD3PD__SUSYMuTrkIsoFillerTool as SUSYMuTrkIsoFillerTool
#myMuonD3PDObject.defineBlock(1, "Isolation_500MeV", SUSYMuTrkIsoFillerTool, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool", NamePrefix="trk500MeV")
#myMuonD3PDObject.defineBlock(1, "Isolation_elstyletracks", SUSYMuTrkIsoFillerTool, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool7",NamePrefix="trkelstyle")


#TrackParticleD3PDObject.defineBlock(999, "Isolation_20", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.2,prefix="cone20_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_30", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.3,prefix="cone30_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4,prefix="cone40_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40_1GeV", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool1", prefix="cone40_ptmin1gev_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40_2GeV", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool2", prefix="cone40_ptmin2gev_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40_3GeV", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool3", prefix="cone40_ptmin3gev_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40_3GeV_hitschi", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool6", prefix="cone40_ptmin3gev_hitschi_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_40_4GeV", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool4", prefix="cone40_ptmin4gev_")
#TrackParticleD3PDObject.defineBlock(999, "Isolation_50_5GeV", TrackD3PDMaker.TrackIsolationFillerTool, ConeSize=.4, TrackIsolationTool="TrackIsolationTool/TrackIsolationTool5", prefix="cone40_ptmin5gev_")


#IndexMultiAssociation(ExtendedJetD3PDObject, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,'el_',level = 99,
#                      prefix='Electron_', blockname='elecblockindex', AssociationName='jetsubstructure_electrons',
#                      IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR','X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime', 'contained'], IndexedMomentsSuffix = "_E" )
#IndexMultiAssociation(ExtendedJetD3PDObject, JetSubstructureD3PDMaker.JetSubstructureTagJetINavigable4MomentumAssociationTool,'mu_',level = 99,
#                      prefix='Muon_', blockname='muonblockindex', AssociationName='jetsubstructure_muons',
#                      IndexedMoments=['X', 'Y', 'Z','MINIISO', 'MINIISO10','DR', 'X_Prime', 'Y_Prime','Z_Prime', 'DR_Prime','contained'], IndexedMomentsSuffix = "_U")



if rec.doTruth():
    
    from TruthD3PDAnalysis.truthParticleConfig          import truthParticleConfig
    truthParticleConfig(topSequence, sgkey=D3PDMakerFlags.TruthParticlesSGKey(), writeHadrons=True, writePartons=True, writeGeant=False)
    
JetROID3PDObject.defineBlock( 0, "myKinematics",
                              EventCommonD3PDMaker.FourMomFillerTool,
                              WriteM = False, WritePt = True,
                              prefix = "myKin_")
#TrackParticleD3PDObject.defineBlock( 99, "TJVAperigee",
#                                     TauD3PDMaker.TrackParticleAtTJVAFillerTool, prefix = 'atTJVA_')


# Special muon isolation variables
#from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Include import include
include( "MuonIsolationTools/MuonIsolationTool_jobOptions.py" )
myMuonD3PDObject.defineBlock(0, "miniiso10", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=10., prefix="MI10_")
myMuonD3PDObject.defineBlock(0, "miniiso15", BoostedTopD3PDMaker.MiniIsolationMuonFillerTool, MuIsolationTool=ToolSvc.MuonIsolationTool, KtDist=15., prefix="MI15_")


#Add new triggers at request of HSG6
defineTriggerBits( TauD3PDObject, 0, [ "EF_tau29T_medium1_xe55_tclcw", "EF_tau29T_medium1_xe55_tclcw_tight" ] )

#------------------------------------------------------------------
# Enable saving the object metadata for D3PDReader code generation
#------------------------------------------------------------------
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.SaveObjectMetadata = True


flags = CommonD3PDMakerFlags
    # ... high-pt tracks (nSCT>3; pt>4 GeV)

if rec.doTruth():
    # ... remove empty GenEvents
    from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
    smwzGenEvtFilterTool = D3PD__GenObjectsFilterTool( "smwzGenEvtFilterTool" )
    ToolSvc += smwzGenEvtFilterTool
    smwzGenEvtFilterTool.RemoveDummyEvents = True
    smwzGenEvtFilterTool.RemoveInTimePileUp = True
    smwzGenEvtFilterTool.Remove2BCPileUp = True
    smwzGenEvtFilterTool.Remove800nsPileUp = True
    smwzGenEvtFilterTool.RemoveCavernBkg = True
    smwzGenEvtFilterTool.RemoveEmptyEvents = True

    # ... heavy flavor truth information
    smwzhfGenVtxFilterTool = D3PD__GenObjectsFilterTool( "SMWZHFGenVtxFilterTool" )
    ToolSvc += smwzhfGenVtxFilterTool
    smwzhfGenVtxFilterTool.RemoveInTimePileUp = False
    smwzhfGenVtxFilterTool.Remove2BCPileUp = True
    smwzhfGenVtxFilterTool.Remove800nsPileUp = True
    smwzhfGenVtxFilterTool.RemoveCavernBkg =  True
    smwzhfGenVtxFilterTool.RemoveEmptyEvents = True
    smwzhfGenVtxFilterTool.RemoveDummyEvents = True
    smwzhfGenVtxFilterTool.RemoveUnrequestedParticles=True
    smwzhfGenVtxFilterTool.KeepBCHadrons=True
    
    smwzhfGenPartFilterTool = D3PD__GenObjectsFilterTool( "SMWZHFGenPartFilterTool" )
    ToolSvc += smwzhfGenPartFilterTool
    smwzhfGenPartFilterTool.RemoveInTimePileUp = False
    smwzhfGenPartFilterTool.Remove2BCPileUp = True
    smwzhfGenPartFilterTool.Remove800nsPileUp = True
    smwzhfGenPartFilterTool.RemoveCavernBkg =  True
    smwzhfGenPartFilterTool.RemoveEmptyEvents = True
    smwzhfGenPartFilterTool.RemoveDummyEvents = True
    smwzhfGenPartFilterTool.RemoveUnrequestedParticles=True
    smwzhfGenPartFilterTool.KeepBCQuarks=True
    smwzhfGenPartFilterTool.KeepBCQuarkAncestors=True
    smwzhfGenPartFilterTool.KeepBCHadrons=True
    smwzhfGenPartFilterTool.KeepBCHadronDecayChain=True
    smwzhfGenPartFilterTool.BCHadronsDescendantsPtCut=200
    smwzhfGenPartFilterTool.BCHadronsDescendantsBarcodeCut=300000
    smwzhfGenPartFilterTool.BCHadronsDescendantsEtaCut=5

    # ...  leptonic W/Z truth information
    smwzlepwzGenPartFilterTool = D3PD__GenObjectsFilterTool( "SMWZLepWZGenPartFilterTool" )
    ToolSvc += smwzlepwzGenPartFilterTool
    smwzlepwzGenPartFilterTool.RemoveInTimePileUp = True
    smwzlepwzGenPartFilterTool.Remove2BCPileUp = True
    smwzlepwzGenPartFilterTool.Remove800nsPileUp = True
    smwzlepwzGenPartFilterTool.RemoveCavernBkg =  True
    smwzlepwzGenPartFilterTool.RemoveEmptyEvents = True
    smwzlepwzGenPartFilterTool.RemoveDummyEvents = True
    smwzlepwzGenPartFilterTool.RemoveUnrequestedParticles=True
    smwzlepwzGenPartFilterTool.KeepLeptonicWZBosons=True
    smwzlepwzGenPartFilterTool.KeepLeptonicWZBosonDecayChains=True
    smwzlepwzGenPartFilterTool.KeepLeptonicWZBosonsParents=True


### for NTUP_PHOTON
#associate the electron to a track
import egammaD3PDMaker
from D3PDMakerCoreComps.IndexAssociation           import IndexAssociation
ElectronTrackParticleIndexAssociation = IndexAssociation(ElectronD3PDObject,
                                                             egammaD3PDMaker.egammaTrackParticleAssociationTool,
                                                             target="trk",
                                                             prefix="trk_",
                                                             level=1,
                                                             blockname="TrackIndex",
                                                             allowMissing=False)
GSFTrackParticlesInSample = testSGKey ('Rec::TrackParticleContainer', "GSFTrackParticleCandidate")
if GSFTrackParticlesInSample:
    from D3PDMakerCoreComps.IndexAssociation           import IndexAssociation
    ElectronTrackParticleIndexAssociation = IndexAssociation(ElectronD3PDObject,
                                                                 egammaD3PDMaker.egammaTrackParticleAssociationTool,
                                                                 target="GSF_trk",
                                                                 prefix="GSF_trk_",
                                                                 level=1,
                                                                 blockname="GSFTrackIndex",
                                                                 allowMissing=False)


# ********************************** begin add eraw57 and topocluster isolations ********************************* #
#CellsSGKey = "AODCellContainer"
## Some common tools for isolation calculations
## create tool CaloNoiseTool
#from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
#theCaloNoiseTool=CaloNoiseToolDefault('db')
#ToolSvc += theCaloNoiseTool

## add tool egammaiso
#from egammaCaloTools.egammaCaloToolsConf import egammaIso
#theegammaiso=egammaIso("PD3PD_egammaIso",
#                       CaloNoiseTool= theCaloNoiseTool)
#ToolSvc += theegammaiso

#from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster as cfrc
#caloFillRect = cfrc(name = "PhotonD3PD_CaloFillRectangularCluster",
#                    eta_size = 5,
#                    phi_size = 7,
#                    cells_name = CellsSGKey)
#ToolSvc += caloFillRect

#PhotonUDPrefix = "PhotonD3PD::PhotonUDPrefix_"
#IsolationVars                 = []
#Fill_EtCone_exclude_phi_strip = []
#Fill_Etconoise_Radii          = []
#Fill_Etconoise_Sigma          = []
#Fill_Etringnoise_Radii        = []
#Fill_Etringnoise_Sigma        = []
#Fill_EM_Layer_0               = []
#Fill_EM_Layer_1               = []
#Fill_EM_Layer_2               = []
#Fill_EM_Layer_3               = []
#Fill_HAD_Layer_0              = []
#Fill_HAD_Layer_1              = []
#Fill_HAD_Layer_2              = []
#Fill_TopoCluster = [] # These should be already filled!
#Fill_PositiveTopoCluster = [20, 30, 40, 50, 60]
#IsolationVars.extend(['topoPosEtcone20', '', 'float'])
#IsolationVars.extend(['topoPosEtcone30', '', 'float'])
#IsolationVars.extend(['topoPosEtcone40', '', 'float'])
#IsolationVars.extend(['topoPosEtcone50', '', 'float'])
#IsolationVars.extend(['topoPosEtcone60', '', 'float'])

#Fill_TopoEMCluster = [] # These should be already filled!
#Fill_PositiveTopoEMCluster = [20, 30, 40, 50, 60]
#IsolationVars.extend(['topoEMPosEtcone20', '', 'float'])
#IsolationVars.extend(['topoEMPosEtcone30', '', 'float'])
#IsolationVars.extend(['topoEMPosEtcone40', '', 'float'])
#IsolationVars.extend(['topoEMPosEtcone50', '', 'float'])
#IsolationVars.extend(['topoEMPosEtcone60', '', 'float'])

#preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
#DVGetter = D3PDMakerCoreComps.SGDataVectorGetterTool
#preseq += PhotonD3PDMaker.IsolationCalculatorAlg ("PhotonIsoAlg",
#                                                  EgammaGetter  = DVGetter ('PhotonIsoAlgPhotonGetter', 
#                                                                            TypeName = 'PhotonContainer',
#                                                                            SGKey = D3PDMakerFlags.PhotonSGKey()),
#                                                  ClusterGetter = DVGetter ('PhotonIsoAlgClusterGetter',
#                                                                            TypeName = 'CaloClusterContainer',
#                                                                            SGKey = D3PDMakerFlags.ClusterSGKey()),
#                                                  EMCaloIsolationTool           = theegammaiso,
#                                                  CaloFillRectangularClusterTool= caloFillRect, 
#                                                  UDPrefix                      = PhotonUDPrefix,
#                                                  CellContainerName             = CellsSGKey,
#                                                  Fill_TopoCluster              = Fill_TopoCluster,
#                                                  Fill_PositiveTopoCluster      = Fill_PositiveTopoCluster,
#                                                  Fill_TopoEMCluster            = Fill_TopoEMCluster,
#                                                  Fill_PositiveTopoEMCluster    = Fill_PositiveTopoEMCluster,
#                                                  Fill_EtCone_exclude_phi_strip = Fill_EtCone_exclude_phi_strip,
#                                                  Fill_EM_Layer_0               = Fill_EM_Layer_0 ,
#                                                  Fill_EM_Layer_1               = Fill_EM_Layer_1 ,
#                                                  Fill_EM_Layer_2               = Fill_EM_Layer_2 ,
#                                                  Fill_EM_Layer_3               = Fill_EM_Layer_3 ,
#                                                  Fill_HAD_Layer_0              = Fill_HAD_Layer_0,
#                                                  Fill_HAD_Layer_1              = Fill_HAD_Layer_1,
#                                                  Fill_HAD_Layer_2              = Fill_HAD_Layer_2,
#                                                  Fill_Etconoise_Radii          = Fill_Etconoise_Radii,
#                                                  Fill_Etconoise_Sigma          = Fill_Etconoise_Sigma,
#                                                  Fill_Etringnoise_Radii        = Fill_Etringnoise_Radii,
#                                                  Fill_Etringnoise_Sigma        = Fill_Etringnoise_Sigma
#                                                  )

## raw cluster variables
#IsolationVars.extend(['Eraw57'  , '', 'float',
#                      'etaraw57', '', 'float']);
#PhotonD3PDObject.defineBlock(1, "PhotonUDIsoVars",
#                             D3PDMakerCoreComps.UserDataFillerTool,
#                             UDPrefix = PhotonUDPrefix,
#                             Vars = IsolationVars)

# ********************************** end add eraw57 and topocluster isolations ********************************* #


## This part creates an index (and some other content) for the tracks associated with the primary
## conversion vertex, with the index pointing towards the "trk_" block

## Associate the photon with TrackParticles
#ConversionTrackParticleAssociation = IndexMultiAssociation(PhotonD3PDObject,
#                                                           PhotonD3PDMaker.ConversionTrackParticleAssociationTool,
#                                                           "trk",
#                                                           "convTrk_",
#                                                           level=1,
#                                                           blockname="ConversionTrackIndex",
#                                                           allowMissing=False)
## Associate the track particles with their TrackInfo
#ConversionTrackParticleTrackInfoAssociation = SimpleAssociation(ConversionTrackParticleAssociation,
#                                                                TrackD3PDMaker.TrackParticleInfoAssociationTool)
#ConversionTrackParticleTrackInfoAssociation.defineBlock(1, "ConversionTrackInfo", TrackD3PDMaker.TrackInfoFillerTool)



## This part is very similar to the above, but it does it for all conversion vertices
## associated with the photon.  So it makes vector<vector<vector<T>>>'s, again with the
## index pointing towards the trk_ block.

## In principle, this could replace the block above, since all the information in
## the previous block is contained in these vectors.

#from D3PDMakerCoreComps.ContainedMultiAssociation import ContainedMultiAssociation
#from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
## Associate the photon with secondary vertices
#ConversionVertexAssociation = ContainedVectorMultiAssociation(PhotonD3PDObject,
#                                                              PhotonD3PDMaker.ConversionVertexAssociationTool,
#                                                              prefix='vx_')
## Associate each secondary vertex with VxTrackAtVertex's
#ConversionVertexTrackAssociation = ContainedVectorMultiAssociation(ConversionVertexAssociation,
#                                                                   TrackD3PDMaker.VertexTrackAssociationTool,
#                                                                   prefix="convTrk_")

## Associate each VxTrackAtVertex with a TrackParticle
#ConversionVertexTrackTrackParticleAssociation = IndexAssociation(ConversionVertexTrackAssociation,
#                                                                 TrackD3PDMaker.VxTrackAtVertexTrackParticleAssociationTool,
#                                                                 "trk",
#                                                                 '',
#                                                                 level=1,
#                                                                 blockname="ConversionVertexTrackIndex",
#                                                                 allowMissing=False)
## Associate each TrackParticle with TrackInfo
#ConversionVertexTrackTrackParticleTrackInfoAssociation = SimpleAssociation(ConversionVertexTrackTrackParticleAssociation,
#                                                                           TrackD3PDMaker.TrackParticleInfoAssociationTool)
#ConversionVertexTrackTrackParticleTrackInfoAssociation.defineBlock(1, "ConversionVertexTrackInfo", TrackD3PDMaker.TrackInfoFillerTool)








# Track parameters at conversion vertex
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from egammaD3PDMaker.PhotonD3PDObject import ConversionVertexAssociation
VxTrackAtConversionVertexAssociationTool = ContainedVectorMultiAssociation(ConversionVertexAssociation,
                                                                           PhotonD3PDMaker.VxTrackAtVertexAssociationTool,
                                                                           prefix="paramsAtConvVtx_")
ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool = SimpleAssociation(VxTrackAtConversionVertexAssociationTool,
                                                                                     PhotonD3PDMaker.ConversionPerigeeAssociationTool)
ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool.defineBlock (1, 'Trk',
                                                                              TrackD3PDMaker.PerigeeFillerTool)
TrackParticleCovarAssoc = SimpleAssociation (ConversionVertexTrackTrackParticlePerigeeAtVertexAssociationTool,
                                             TrackD3PDMaker.PerigeeCovarianceAssociationTool)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     DiagCovariance = True)
TrackParticleCovarAssoc.defineBlock (3, 'TrkCovOffDiag',
                                     TrackD3PDMaker.CovarianceFillerTool,
                                     IsTrackPerigee = True,
                                     Error = False,
                                     OffDiagCovariance = True)


# ********* already added in PhotonD3PDObject, don't need here ************* #

# Associate the photon with secondary vertices
#from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
#ConversionVertexAssociation = ContainedVectorMultiAssociation(PhotonD3PDObject,
#                                                              egammaD3PDMaker.ConversionVertexAssociationTool,
#                                                              prefix='vx_')
#ConversionVertexAssociation.defineBlock(10, 'ConversionVertexPosition', TrackD3PDMaker.VertexPositionFillerTool)
#ConversionVertexAssociation.defineBlock(10, 'ConversionVertexKinematics', TrackD3PDMaker.VertexKineFillerTool)

# Associate each secondary vertex with VxTrackAtVertex's
#ConversionVertexTrackAssociation = ContainedVectorMultiAssociation(ConversionVertexAssociation,
#                                                                   TrackD3PDMaker.VertexTrackAssociationTool,
#                                                                   prefix="convTrk_")


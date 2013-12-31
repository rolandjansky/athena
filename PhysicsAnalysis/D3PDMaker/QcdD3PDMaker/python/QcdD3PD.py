# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file QcdD3PDMaker/python/QcdD3PD.py
# @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
# @date Apr, 2010
# @brief Construct a Qcd D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags                     import D3PDMakerFlags
from RecExConfig.RecFlags                               import rec
from RecExConfig.InputFilePeeker                        import inputFileSummary
from RecExConfig.ObjKeyStore                            import objKeyStore
from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation               import SimpleAssociation
from D3PDMakerCoreComps.IndexMultiAssociation           import IndexMultiAssociation
from D3PDMakerCoreComps.IndexAssociation                import IndexAssociation

from EventCommonD3PDMaker.EventInfoD3PDObject           import EventInfoD3PDObject
from TruthD3PDAnalysis.truthParticleConfig              import truthParticleConfig
from EventCommonD3PDMaker.DRIndexAssociation            import DRIndexAssociation
from EventCommonD3PDMaker.GenEventD3PDObject            import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject             import TruthParticleD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig              import LBMetadataConfig

from egammaD3PDMaker.ElectronD3PDObject                 import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                   import PhotonD3PDObject
from egammaD3PDAnalysis.egammaUserDataConfig            import egammaUserDataConfig
from MuonD3PDMaker.MuonD3PDObject                       import MuonD3PDObject
from TauD3PDMaker.TauD3PDObject                         import TauD3PDObject
from TauD3PDMaker.TruthTauD3PDObject                    import TruthTauD3PDObject
from JetD3PDMaker.JetD3PDObject                         import JetD3PDObject
from JetTagD3PDMaker.AddBTagD3PDInfo                    import addBTagInfoToJetObject
addBTagInfoToJetObject(JetD3PDObject, 200)

from MissingETD3PDMaker.MissingETGoodnessD3PDObject     import MissingETGoodnessD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject             import MissingETD3PDObject
from MissingETD3PDMaker.MissingETD3PDObject             import MissingETCompositionD3PDObject
from MissingETD3PDMaker.MissingETD3PDMakerFlags         import *

#from MinBiasD3PDMaker.BeamSpotD3PDObject                import BeamSpotD3PDObject
#from QcdD3PDMaker.QcdBeamSpotD3PDObject                 import QcdBeamSpotD3PDObject

from TrackD3PDMaker.TruthVertexD3PDObject               import TruthVertexD3PDObject
from QcdD3PDMaker.QcdEventShapeD3PDObject               import EventShapeD3PDObject
from QcdD3PDMaker.QcdMuonSpShowerD3PDObject             import MuonSpShowerD3PDObject

from TriggerD3PDMaker.JetROID3PDObject                  import JetROID3PDObject
from TriggerD3PDMaker.JetETROID3PDObject                import JetETROID3PDObject
from TriggerD3PDMaker.EnergySumROID3PDObject            import EnergySumROID3PDObject
from TriggerD3PDMaker.TrigDecisionD3PDObject            import TrigDecisionD3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PD                 import TrigMETD3PDObjects
from TriggerD3PDMaker.TrigConfMetadata                  import addTrigConfMetadata
from JetTagD3PDMaker.JetTagD3PDMakerKeys                import JetTagD3PDKeys

from TriggerD3PDMaker.BGCodeD3PDObject                  import BGCodeD3PDObject

from CaloD3PDMaker.LArCollisionTimeD3PDObject           import LArCollisionTimeD3PDObject
from CaloD3PDMaker.ClusterD3PDObject                    import ClusterD3PDObject
from CaloD3PDMaker.EMClusterD3PDObject                  import EMClusterD3PDObject
from CaloD3PDMaker.CellD3PDObject                       import CellD3PDObject
from CaloD3PDMaker.TowerD3PDObject                      import TowerD3PDObject
from CaloD3PDMaker.makeClusterFilter                    import makeClusterFilter

from CaloD3PDMaker.MBTSD3PDObject                       import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject                   import MBTSTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject          import CollisionDecisionD3PDObject
from TrackD3PDMaker.VertexD3PDObject                    import PrimaryVertexD3PDObject

from QcdD3PDMaker.QcdTriggerBitsD3PDObject              import QcdTriggerBitsD3PDObject

from TrigJetD3PDMaker.TrigJetD3PDObject                 import TrigJetD3PDObject
from TrigJetD3PDMaker.TrigJetD3PD                       import *
from TriggerD3PDMaker.BunchStructureMetadata            import addBunchStructureMetadata

from QcdD3PDMaker.QcdD3PDMakerFlags                     import QcdD3PDMakerFlags

from TrackD3PDMaker.TrackD3PDObject                     import TrackParticleD3PDObject
from TrackD3PDMaker.TrackD3PDMakerFlags                 import TrackD3PDFlags
from TauD3PDMaker.makeTrackFilterAlg                    import makeTrackFilterAlg

from AthenaCommon.AlgSequence                           import AlgSequence
topSequence = AlgSequence()
if rec.doTruth() and rec.readAOD():
    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    topSequence += TruthTausToSG()

def AddConstitIndex(object, typename='CaloCluster', target='cl_', level=0):
    import EventCommonD3PDMaker
    import D3PDMakerCoreComps
    ConstitAssoc = ContainedVectorMultiAssociation \
                   (object,
                    EventCommonD3PDMaker.NavigableConstituentAssociationTool,
                    'constit_',
                    blockname=typename+'NavigableConstituentAssociationTool',
                    TypeName = typename, WriteWeight = False,
                    level = level)
    
    ConstitAssoc.defineBlock (0, 'ConstitIndex'+typename, D3PDMakerCoreComps.IndexFillerTool, Target = target)

def AddConstitTruthIndex(object, typename='CaloCluster', target='cl_', level=0):
    import EventCommonD3PDMaker
    import D3PDMakerCoreComps
    import TruthD3PDMaker
    ConstitAssoc = ContainedVectorMultiAssociation \
                   (object,
                    EventCommonD3PDMaker.NavigableConstituentAssociationTool,
                    'constit_',
                    blockname=typename+'NavigableConstituentAssociationTool',
                    TypeName = typename, WriteWeight = False,
                    level = level)

    genPart = SimpleAssociation(ConstitAssoc,TruthD3PDMaker.TruthParticleGenParticleAssociationTool)
    trupart2 = IndexAssociation (genPart, TruthD3PDMaker.GenParticleTruthParticleAssociationTool,
                                 target = 'mc_',
                                 Target = 'mc_')

def AddTrackAssocIndex(obj, level=99):
    import JetTagD3PDMaker
    JetTrackAssoc = IndexMultiAssociation(obj, JetTagD3PDMaker.JetTagJetTrackAssociationTool, 'trk',
                                          prefix = 'TrackAssoc_',TrackAssocName="TrackAssoc",
                                          blockname='TrackAssocblockindex',
                                          level = level, FillVariables=False)

def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw

def QcdD3PD (alg = None,
             level = 10,
             file = 'qcd.root',
             tuplename = 'qcd',
             flags=QcdD3PDMakerFlags,
             stdElectronContainer = D3PDMakerFlags.ElectronSGKey(),
             stdPhotonContainer = D3PDMakerFlags.PhotonSGKey(),
             stdTrackParticleContainer = D3PDMakerFlags.TrackSGKey(),
             stdTruthParticleContainer = D3PDMakerFlags.TruthParticlesSGKey(),
             **kw):
    # define track and cluster filters
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

    # for HR
    if flags.doHR():
        import HadronicRecoil.Configurables as hrc
        preseq = hrc.add_hadronic_recoil_filters(preseq)
        #preseq += hrc.add_hadronic_recoil_filters(preseq)

    
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream(tuplename, file)

    #Event level info
    alg += EventInfoD3PDObject (**_args(10, 'EventInfo', kw))

    # LBMetaData
    alg.MetadataTools += [LBMetadataConfig()]

    # collision candidate
    if flags.doCollCan():
        alg += LArCollisionTimeD3PDObject (**_args(10, 'LArCollisionTime', kw))
        alg += MBTSTimeD3PDObject (**_args(10, 'MBTSTime', kw))
        alg += CollisionDecisionD3PDObject (**_args(10, 'CollisionDecision', kw))

    # BeamSpot
    #alg += QcdBeamSpotD3PDObject(**_args(10, 'QcdBeamSpot', kw,  sgkey = 'QcdD3PD::BeamSpotRecVertex'))
    #alg += BeamSpotD3PDObject(**_args(10, 'BeamSpot', kw,  sgkey = 'BeamSpotRecVertex'))

    # May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args(10, 'PrimaryVertex', kw,  allowMissing = True, sgkey = D3PDMakerFlags.VertexSGKey(), prefix = 'vxp_'))

    # The basic objects (el, mu, pho, tau)
    if flags.doElectron():
      if ( stdElectronContainer != 'None'):
        alg += ElectronD3PDObject (**_args(flags.electronLevel(), 'Electron', kw, sgkey = stdElectronContainer,
                                   include = flags.electronInclude(), exclude = flags.electronExclude()))
    if flags.doPhoton():
      if ( stdPhotonContainer != 'None'):
        alg += PhotonD3PDObject (**_args(flags.photonLevel(), 'Photon', kw, sgkey = stdPhotonContainer,
                                 include = flags.photonInclude(), exclude = flags.photonExclude()))
    if flags.doMuon():
        alg += MuonD3PDObject (**_args(flags.muonLevel(), 'StacoMuon', kw, sgkey = 'StacoMuonCollection',
                               prefix = 'mu_staco_', include = flags.muonInclude(), exclude = flags.muonExclude()))
        alg += MuonD3PDObject (**_args(flags.muonLevel(), 'MuidMuon', kw, sgkey = 'MuidMuonCollection',
                               prefix = 'mu_muid_', include = flags.muonInclude(), exclude = flags.muonExclude()))
    if flags.doTau():
        alg += TauD3PDObject (**_args(flags.tauLevel(), 'Tau', kw,
                              include = flags.tauInclude(), exclude = flags.tauExclude()))
    #Hadronic Recoil
    if flags.doHR():
        alg += ElectronD3PDObject (**_args(0, 'hr_el', kw,  sgkey='HR_selectedElectrons', prefix='hr_el_'))
        alg += MuonD3PDObject (**_args(0, 'hr_mu', kw, sgkey='HR_selectedMuons', prefix='hr_mu_'))
        # ... DR = 0.20
        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_20",  prefix = "hr_roughRecoil_20",exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_20", prefix = "hr_ueCorrection_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_20", prefix = "hr_corrRecoil_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_20",        prefix = "hr_MET_20", exclude=['MET_Regions','MET_Comps'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_20",   prefix = "hr_roughRecoil_track_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_20",  prefix = "hr_ueCorrection_track_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_20", prefix = "hr_corrRecoil_track_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_20", prefix = "hr_MET_track_20", exclude=['MET_Regions','MET_Comps'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_clusNoTrack_20", prefix = "hr_roughRecoil_clusNoTrack_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_clusNoTrack_20", prefix = "hr_ueCorrection_clusNoTrack_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_clusNoTrack_20", prefix = "hr_corrRecoil_clusNoTrack_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_clusNoTrack_20",        prefix = "hr_MET_clusNoTrack_20", exclude=['MET_Regions','MET_Comps'] )

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_20",  prefix = "hr_roughRecoil_Eflow_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_20", prefix = "hr_ueCorrection_Eflow_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_20", prefix = "hr_corrRecoil_Eflow_20", exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_20", prefix = "hr_MET_Eflow_20", exclude=['MET_Regions','MET_Comps'])

    # Jet information
    jetInclude = flags.jetInclude()
    jetIncludeTrack = jetInclude[:]
    jetIncludeTrack += ['Rec::TrackParticleNavigableConstituentAssociationTool']
    jetIncludeTower = jetInclude[:]
    jetIncludeTower += ['JVtx','JetVertexFraction']
    jetInclude += ['Samplings',
                   'CaloClusterNavigableConstituentAssociationTool',
                   'TrackAssocblockindex']
    jetIncludeNew  = jetInclude[:]
    jetIncludeLC   = jetInclude[:]
    jetIncludeSPCL = jetInclude[:]
    jetIncludeLCSPCL = jetInclude[:]
    jetIncludeSPCLB = jetInclude[:]
    jetIncludeLCSPCLB = jetInclude[:]

    jetIncludeLC += ['ConstituentScale','EMScale']
    jetIncludeSPCL += ['ConstituentScale','TracksMoments']
    jetIncludeSPCLB += ['ConstituentScale','TracksMoments',
                        JetTagD3PDKeys.TruthInfoBlockName(), JetTagD3PDKeys.BTagWeightsBlockName(),
                        JetTagD3PDKeys.SVInfoPlusBlockName(),JetTagD3PDKeys.SV0InfoPlusBlockName(),
                        JetTagD3PDKeys.IP3DInfoBaseBlockName(),
                        JetTagD3PDKeys.SV1InfoBaseBlockName(),
                        JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                        JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName()]
    jetIncludeLCSPCL += ['ConstituentScale','EMScale','TracksMoments']
    jetIncludeLCSPCLB += ['ConstituentScale','EMScale','TracksMoments',
                          JetTagD3PDKeys.TruthInfoBlockName(), JetTagD3PDKeys.BTagWeightsBlockName(),
                          JetTagD3PDKeys.SVInfoPlusBlockName(),JetTagD3PDKeys.SV0InfoPlusBlockName(),
                          JetTagD3PDKeys.IP3DInfoBaseBlockName(),
                          JetTagD3PDKeys.SV1InfoBaseBlockName(),
                          JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                          JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName()]
    if not flags.doSMQCD():
        jetIncludeSPCL += ['Special','TrackMF','JetLabel']
        jetIncludeSPCLB += ['Special','TrackMF','JetLabel']
        jetIncludeLCSPCL += ['Special','TrackMF','JetLabel']
        jetIncludeLCSPCLB += ['Special','TrackMF','JetLabel']

    jetExclude = flags.jetExclude()[:]

    #TopoJets (EM, GCW)
    AddConstitIndex(JetD3PDObject, typename='CaloCluster', target='cl_had_', level=99)
    AddTrackAssocIndex(JetD3PDObject, level=99)

    #if (not flags.doRecJet()) or flags.doRecJetOnlyAdditional():
    if (not flags.doRecJet()):
        if flags.doTopoEMJet():
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4TopoEM', kw, prefix = 'jet_AntiKt4TopoEM_', sgkey = 'AntiKt4TopoEMJets',
                                          include = jetIncludeSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6TopoEM', kw, prefix = 'jet_AntiKt6TopoEM_', sgkey = 'AntiKt6TopoEMJets',
                                          include = jetIncludeSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
    else:
        if flags.doTopoEMJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4TopoEM', kw, prefix = 'jet_AntiKt4TopoEM_', sgkey = 'AntiKt4TopoEMNewJets',
                                          include = jetIncludeSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6TopoEM', kw, prefix = 'jet_AntiKt6TopoEM_', sgkey = 'AntiKt6TopoEMNewJets',
                                          include = jetIncludeSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    if flags.doRecJet() and flags.doTopoEMLowPtJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"): # lowPt
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4TopoEMLowPt', kw, prefix = 'jet_AntiKt4TopoEMLowPt_', sgkey = 'AntiKt4TopoEMLowPtJets',
                                      include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6TopoEMLowPt', kw, prefix = 'jet_AntiKt6TopoEMLowPt_', sgkey = 'AntiKt6TopoEMLowPtJets',
                                      include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    if flags.doRecJet() and flags.doTopoJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"): # additional: Topo GCW
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4Topo', kw, prefix = 'jet_AntiKt4TopoNew_', sgkey = 'AntiKt4TopoNewJets',
                                      include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6Topo', kw, prefix = 'jet_AntiKt6TopoNew_', sgkey = 'AntiKt6TopoNewJets',
                                      include = jetInclude, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    #TowerJets (EM)
    if flags.doTowerJet() and rec.readAOD():
        alg += JetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt6Tower', kw, prefix = 'jet_AntiKt6Tower_', sgkey = 'AntiKt6TowerAODJets',
                                      include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    if not rec.readAOD() and flags.doRecJet() and flags.doTowerEMJet():
        # additional: TowerEM
        alg += JetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt4TowerEM', kw, prefix = 'jet_AntiKt4TowerEMNew_', sgkey = 'AntiKt4TowerEMNewJets',
                                      include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.towerJetLevel(), 'AntiKt6TowerEM', kw, prefix = 'jet_AntiKt6TowerEMNew_', sgkey = 'AntiKt6TowerEMNewJets',
                                      include = jetIncludeTower, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    #LCTopoJets
    #AddConstitIndex(JetD3PDObject, typename='CaloCluster', target='cl_had_', level=99)
    #if (not flags.doRecJet()) or flags.doRecJetOnlyAdditional():
    if (not flags.doRecJet()):
        if flags.doLCTopoJet():
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4LCTopo', kw, prefix = 'jet_AntiKt4LCTopo_', sgkey = 'AntiKt4LCTopoJets',
                                          include = jetIncludeLCSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6LCTopo', kw, prefix = 'jet_AntiKt6LCTopo_', sgkey = 'AntiKt6LCTopoJets',
                                          include = jetIncludeLCSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
    else:
        if flags.doLCTopoJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4LCTopo', kw, prefix = 'jet_AntiKt4LCTopo_', sgkey = 'AntiKt4LCTopoNewJets',
                                          include = jetIncludeLCSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
            alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6LCTopo', kw, prefix = 'jet_AntiKt6LCTopo_', sgkey = 'AntiKt6LCTopoNewJets',
                                          include = jetIncludeLCSPCLB, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
            

    if flags.doRecJet() and flags.doLCTopoJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"): # additional: LCTopo addional R-sizes
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt5LCTopo', kw, prefix = 'jet_AntiKt5LCTopo_', sgkey = 'AntiKt5LCTopoNewJets',
                                      include = jetIncludeLCSPCL, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    if flags.doRecJet() and  flags.doLCTopoLowPtJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"): # additional: LCTopoLowPt
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4LCTopoLowPt', kw, prefix = 'jet_AntiKt4LCTopoLowPt_', sgkey = 'AntiKt4LCTopoLowPtJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6LCTopoLowPt', kw, prefix = 'jet_AntiKt6LCTopoLowPt_', sgkey = 'AntiKt6LCTopoLowPtJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    if flags.doRecJet() and  flags.doLCTopoCBJet() and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"): # additional: LCTopo CB
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4LCTopoCB', kw, prefix = 'jet_AntiKt4LCTopoCB_', sgkey = 'AntiKt4LCTopoCBJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6LCTopoCB', kw, prefix = 'jet_AntiKt6LCTopoCB_', sgkey = 'AntiKt6LCTopoCBJets',
                                      include = jetIncludeLC, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    #GhostJets
    if ((not flags.doRecJet()) or flags.doRecJetOnlyAdditional()) and rec.readAOD():
        if flags.doGhostJet():
            alg += JetD3PDObject (**_args(0, 'AntiKt6GhostTower', kw, prefix = 'jet_AntiKt6GhostTower_', sgkey = 'AntiKt6GhostTowerAODJets',
                                          include = jetIncludeTower, exclude = jetExclude))
    else:
        if not rec.readAOD() and flags.doRecJet() and flags.doGhostJet():
            alg += JetD3PDObject (**_args(0, 'AntiKt4GhostTowerNew', kw, prefix = 'jet_AntiKt4GhostTowerNew_', sgkey = 'AntiKt4GhostTowerNewJets',
                                          include = jetIncludeTower, exclude = jetExclude))
            alg += JetD3PDObject (**_args(0, 'AntiKt6GhostTowerNew', kw, prefix = 'jet_AntiKt6GhostTowerNew_', sgkey = 'AntiKt6GhostTowerNewJets',
                                          include = jetIncludeTower, exclude = jetExclude))

    #TrackJets
    AddConstitIndex(JetD3PDObject, typename='Rec::TrackParticle', target='trk', level=99)
    if flags.doRecJet() and flags.doTrackJet():
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt4TrackZ', kw, prefix = 'jet_AntiKt4TrackZ_', sgkey = 'AntiKt4TrackZJets',
                                      include = jetIncludeTrack, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))
        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt6TrackZ', kw, prefix = 'jet_AntiKt6TrackZ_', sgkey = 'AntiKt6TrackZJets',
                                      include = jetIncludeTrack, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

        alg += JetD3PDObject (**_args(flags.slimJetLevel(), 'AntiKt5TrackZ', kw, prefix = 'jet_AntiKt5TrackZ_', sgkey = 'AntiKt5TrackZJets',
                                      include = jetIncludeTrack, exclude = jetExclude, JetVertexFraction_FromUD=True, JetVertexFraction_FillFullJVF=True))

    # MissingEt objects
    if flags.doMET():
        if flags.doRecMET():
            MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey = 'AntiKt4LCTopoJets'
            MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_"
            missEtIncludeNew = flags.missEtInclude()
            missEtIncludeNew += ['METComposition']
            #alg += MissingETCompositionD3PDObject ( level=4 )

            alg += MissingETD3PDObject (**_args(10, 'MissingET', kw, include=missEtIncludeNew, allowMissing = True))
            if ( stdTrackParticleContainer == 'GoodTracks'):
                if  flags.doRecJet():
                    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_default', suffix='default_', allowMissing =True,
                                                            jetSGKey='AntiKt4LCTopoNewJets', jetPrefix='jet_AntiKt4LCTopo_MET_',
                                                            trackSGKey='GoodTracks', trackPrefix='trk_GoodTracks_MET_')
                else:
                    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_default', suffix='default_', allowMissing =True,
                                                            jetSGKey='AntiKt4LCTopoJets', jetPrefix='jet_AntiKt4LCTopo_MET_',
                                                            trackSGKey='GoodTracks', trackPrefix='trk_GoodTracks_MET_')
            else:
                if  flags.doRecJet():
                    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_default', suffix='default_', allowMissing =True,
                                                            jetSGKey='AntiKt4LCTopoNewJets', jetPrefix='jet_AntiKt4LCTopo_MET_')
                else:
                    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_default', suffix='default_', allowMissing =True,
                                                            jetSGKey='AntiKt4LCTopoJets', jetPrefix='jet_AntiKt4LCTopo_MET_')


            #More involved custom MET
            if flags.doAdditionalMissingET():
                customMETs = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                              'MET_CellOut', 'MET_Cryo', 'MET_SoftJets',
                              'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                              'MET_CellOut_Eflow_STVF',
                              'MET_CellOut_Eflow_JetArea',
                              'MET_CellOut_Eflow_JetAreaJVF',
                              'MET_CellOut_Eflow_JetAreaRhoEta5JVF',
                              'MET_RefFinal_STVF' ]
                customMETs_Staco_wo_region = ['MET_RefMuon' ,'MET_RefMuon_Staco',
                                              'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_Muon_Total_Staco',
                                              'MET_RefMuon_Track', 'MET_RefMuon_Track_Staco']
                customMETs_Muid_wo_region = ['MET_RefMuon' ,'MET_RefMuon_Muid',
                                             'MET_Muon_Isol_Muid', 'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid',
                                             'MET_RefMuon_Track', 'MET_RefMuon_Track_Muid']
                customMETs_conflistStaco = ['_default', '_LCW_NI_eflow6']
                customMETs_conflistMuid =  ['_LCW_pt20']

                for conflist in customMETs_conflistStaco:
                    for custom in customMETs:
                        alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist, prefix=custom+conflist,
                                                            include=missEtIncludeNew, allowMissing = True))
                    for custom in customMETs_Staco_wo_region:
                        alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist, prefix=custom+conflist,
                                                            include=missEtIncludeNew, exclude = ['MET_Regions'], allowMissing = True))
                for conflist in customMETs_conflistMuid:
                    for custom in customMETs:
                        alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist, prefix=custom+conflist,
                                                            include=missEtIncludeNew, allowMissing = True))
                    for custom in customMETs_Muid_wo_region:
                        alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+conflist, prefix=custom+conflist,
                                                            include=missEtIncludeNew, exclude = ['MET_Regions'], allowMissing = True))

                if flags.doRecJet() and  flags.doLCTopoCBJet(): # additional: LCTopo CB
                    for custom in customMETs_Staco:
                        alg += MissingETD3PDObject (**_args(0, 'MissingET'+conflist, kw, sgkey = custom+'_LCW_CB_eflow', prefix=custom+'_LCW_CB_eflow',
                                                            include=missEtIncludeNew, allowMissing = True))
                    
        else:
            alg += MissingETD3PDObject (**_args(flags.missEtLevel(), 'MissingET', kw, allowMissing = True,
                                        include = flags.missEtInclude(), exclude = flags.missEtExclude()))

    # MissingEt Goodness
    if flags.doMissingETGoodness():
        alg += MissingETGoodnessD3PDObject(**_args(3, 'MissingETGoodness', kw, allowMissing = True))

    # track
    if flags.doTrack():
      if ( stdTrackParticleContainer != 'None'):
        alg += TrackParticleD3PDObject (**_args(10, 'TrackParticle', kw, sgkey=stdTrackParticleContainer)) # w/ filtering
        #alg += TrackParticleD3PDObject (**_args(10, 'TrackParticle', kw))

    #  Clusters (hadron scale)
    if flags.doClusterHad():
        from CaloD3PDMaker import ClusterMomentFillerTool as CMFT
        #              CMFT.HAD_WEIGHT, 'hadWeight']
        myMoments = [ CMFT.CENTER_LAMBDA, 'center_lambda',
                      CMFT.LATERAL, 'lateral',
                      CMFT.LONGITUDINAL, 'longitudinal',
                      CMFT.ISOLATION, 'isolation']
        alg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw, prefix='cl_had_',
                                          include='Moments', exclude='SamplingBasics',
                                          Kinematics_WriteE=True, Moments_Moments=myMoments ))

    #  Clusters (em scale )
    if flags.doClusterEM():
        alg += ClusterD3PDObject (**_args(0, 'CaloCalTopoCluster', kw, prefix='cl_em_', exclude='SamplingBasics',
                                          Kinematics_WriteE=True, Kinematics_SignalState=0 ))
        #                                  Kinematics_WriteE=True, Kinematics_WritePt=False, Kinematics_WriteM=False, Kinematics_WriteEtaPhi=False,
        #                                  Kinematics_SignalState=0 ))
        #alg += EMClusterD3PDObject(**_args(0, 'CaloCalTopoCluster', kw, sgkey='CaloCalTopoCluster', prefix='cl_',
        #include=['Moments'], exclude='SamplingBasics', Kinematics_SignalState=0, Moments_Moments=myMoments))

    #  Cells
    if flags.doCell():
        alg += CellD3PDObject (**_args(0, 'Cell', kw))

    #  Calo Towers
    if flags.doCaloTower():
        alg += TowerD3PDObject (**_args(0, 'CaloTower',kw, sgkey='CombinedTower', prefix='calo_'))

    #  Topo Towers
    if flags.doTopoTower():
        alg += TowerD3PDObject (**_args(1, 'TopoTower',kw, sgkey='TopoTower', prefix='topo_'))

    #  MuonSpShower
    if flags.doMuonSpShower2():
        alg += MuonSpShowerD3PDObject (**_args(10, 'MuonSpShower', kw))

    # EventShape
    if flags.doEventShape():
        alg += EventShapeD3PDObject(**_args(10, 'rho', kw))
    
    # Trigger
    if rec.doTrigger():
        alg += QcdTriggerBitsD3PDObject (**_args(10, 'TriggerBits', kw))
        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addBunchStructureMetadata( alg )
        addTrigConfMetadata( alg )
        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        if flags.doMoreTrig():
            TrigJetD3PDObjects( alg, level = 10, addNaviInfo = True, addConfigInfo = False )
            TrigMETD3PDObjects(alg)
    # Truth
    if rec.doTruth():
        D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)
        alg += GenEventD3PDObject (**_args(1, 'GenEvent', kw))
        if ( stdTruthParticleContainer != 'None'):
          alg += TruthParticleD3PDObject (**_args(1, 'TruthParticle', kw, sgkey = stdTruthParticleContainer, exclude=['child_TruthParticleChildAssociation','parent_TruthParticleParentAssociation']))
        alg += TruthVertexD3PDObject(**_args(1, 'TruthVertex', kw, sgkey=D3PDMakerFlags.TruthSGKey()))

        truthjetInclude = flags.truthJetInclude()
        truthjetInclude += [JetTagD3PDKeys.TruthInfoBlockName(),
                            'TruthParticleNavigableConstituentAssociationTool']

        if rec.readAOD():
            # trueTau
            TauTrueTauAssoc = DRIndexAssociation(
                parent = TauD3PDObject,
                type_name = 'TruthParticleContainer',
                default_sgkey = 'TrueHadronicTaus',
                default_drcut = 0.2,
                prefix = 'trueTauAssocSmall_',
                target = 'trueTau_' )
            # trueTau_tauAssoc_index
            TrueTauTauAssoc = DRIndexAssociation(
                parent = TruthTauD3PDObject,
                type_name = 'Analysis::TauJetContainer',
                default_sgkey = D3PDMakerFlags.TauSGKey(),
                default_drcut = 0.2,
                prefix = 'tauAssocSmall_',
                target = 'tau_' )
            alg += TruthTauD3PDObject(**_args(10, 'TrueHadronicTaus', kw))


        if flags.doTruthJet():
            if flags.doRecTruthJet():
                AddConstitTruthIndex(JetD3PDObject,typename='TruthParticle', target='mc_', level=99)
                #alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4Truth', kw, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_',
                #                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthNew', kw, sgkey='AntiKt4TruthNewJets', prefix='AntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt5TruthNew', kw, sgkey='AntiKt5TruthNewJets', prefix='AntiKt5Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthNew', kw, sgkey='AntiKt6TruthNewJets', prefix='AntiKt6Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithNoInt', kw, sgkey='AntiKt4TruthWithNoIntJets', prefix='AntiKt4TruthWithNoInt_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithNoInt', kw, sgkey='AntiKt6TruthWithNoIntJets', prefix='AntiKt6TruthWithNoInt_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithMuNoInt', kw, sgkey='AntiKt4TruthWithMuNoIntJets', prefix='AntiKt4TruthWithMuNoInt_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithMuNoInt', kw, sgkey='AntiKt6TruthWithMuNoIntJets', prefix='AntiKt6TruthWithMuNoInt_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4TruthWithMu', kw, sgkey='AntiKt4TruthWithMuJets', prefix='AntiKt4TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt6TruthWithMu', kw, sgkey='AntiKt6TruthWithMuJets', prefix='AntiKt6TruthWithMu_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
            else:
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'AntiKt4Truth', kw, sgkey='AntiKt4TruthJets', prefix='AntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))

        if flags.doTruthPileupJet():
            if objKeyStore.isInInput("JetCollection","InTimeAntiKt4TruthJets"):
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'InTimeAntiKt4Truth', kw, sgkey='InTimeAntiKt4TruthJets', prefix='InTimeAntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
            if objKeyStore.isInInput("JetCollection","OutOfTimeAntiKt4TruthJets"):
                alg += JetD3PDObject (**_args(flags.truthJetLevel(), 'OutOfTimeAntiKt4Truth', kw, sgkey='OutOfTimeAntiKt4TruthJets', prefix='OutOfTimeAntiKt4Truth_',
                                              include=truthjetInclude, exclude=flags.truthJetExclude()))
            pass

    return alg

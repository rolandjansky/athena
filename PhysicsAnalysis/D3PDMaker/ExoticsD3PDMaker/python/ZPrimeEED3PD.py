# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZPrimeEED3PD.py 531028 2012-12-18 13:40:00Z okepka $
#
# @file ExoticsD3PDMaker/python/ZPrimeEE.py
# @author Michiru Kaneda<Michiru.Kaneda@cern.ch>
# @date Jan, 2011
# @brief Construct a D3PD for Z prime (->ee) analysis


import D3PDMakerCoreComps

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from OutputStreamAthenaPool.MultipleStreamManager      import MSMgr

# event

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject

# EventShape variables
from QcdD3PDMaker.QcdEventShapeD3PDObject import EventShapeD3PDObject

# eletron/photon

from egammaD3PDMaker.ElectronD3PDObject                import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonD3PDObject

# muons

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject

# jet

from JetD3PDMaker.JetD3PDObject                        import JetD3PDObject

# calo

from CaloD3PDMaker.LArCollisionTimeD3PDObject          import LArCollisionTimeD3PDObject

# baseline track config

from TrackD3PDMaker.TrackD3PDMakerFlags                import TrackD3PDFlags

TrackD3PDFlags.doTruth = False
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
TrackD3PDFlags.storeHitTruthMatching = False
TrackD3PDFlags.storePixelHitsOnTrack = False
TrackD3PDFlags.storePixelHolesOnTrack = False
TrackD3PDFlags.storePixelOutliersOnTrack = False
TrackD3PDFlags.storeSCTHitsOnTrack = False
TrackD3PDFlags.storeSCTHolesOnTrack = False
TrackD3PDFlags.storeSCTOutliersOnTrack = False
TrackD3PDFlags.storeTRTHitsOnTrack = False
TrackD3PDFlags.storeTRTHolesOnTrack = False
TrackD3PDFlags.storeTRTOutliersOnTrack = False
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.storeTrackInfo = False
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeTrackSummary = False
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 1

# PV config

TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = False
TrackD3PDFlags.vertexPositionLevelOfDetails = 2

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject

# BeamSpot / MinBias

from TrackD3PDMaker.BeamSpotD3PDObject                 import BeamSpotD3PDObject

# MET

from MissingETD3PDMaker.MissingETD3PDMakerFlags        import MissingETD3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDObject            import *

MissingETD3PDMakerFlags.doCellOutEflow=True
MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey = 'AntiKt4LCTopoJets'
MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_"

# Trigger

from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TrigJetD3PDMaker.TrigJetD3PD                      import InitTrigD3PDObject
from TriggerD3PDMaker.EmTauROID3PDObject               import EmTauROID3PDObject
from TrigEgammaD3PDMaker.TrigElectronD3PDObject        import TrigElectronD3PDObject
from TrigEgammaD3PDMaker.TrigPhotonD3PDObject          import TrigPhotonD3PDObject
from TrigEgammaD3PDMaker.EFElectronD3PDObject          import EFElectronD3PDObject
from TrigEgammaD3PDMaker.EFPhotonD3PDObject            import EFPhotonD3PDObject
from ExoticsD3PDMaker.ZPrimeEETriggerBitsD3PDObject    import ZPrimeEETriggerBitsD3PDObject

from RecExConfig.RecFlags     import rec
from RecExConfig.ObjKeyStore  import cfgKeyStore
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def ZPrimeEED3PD (alg = None,
                  file = 'smwz.root',
                  tuplename = 'physics',
                  streamname = 'd3pdstream',
                  doSoftQCD = False,
                  **kw):

    # now configure the D3PD
    if not alg:
        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)

    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    # Electron/Photon blocks
    elInclude = ['HadLeakage','Layer1Shape','Layer1ShapeExtra','Layer2Shape','Layer3Shape',
             'Iso','IsoPtCorrected','ConvFlags','Conversion','TrkMatch','Retaphi','Rings','ElecDiscrim','Pointing',
             'Samplings','ClusterKin','FwdEVars','D3PD__egammaRawClusterAssociationTool',
             'RawSamplings','RawClusterKin','TrkInfo','Trk','TrkFitQuality','IDHits','TrackSummaryPID','Vertex',
             'trackImpactbeam','trackImpactSigbeam','trackImpactpv','trackImpactSigpv','trackOldImpactpvunbiased','trackImpactpvunbiased',
             'UDLayer1Shape','UDShowerDepth','IsIso','MVAIso',
             'JetMatch','JetKinematics','PAU','PAUVariables','EFIndex','L2Index','L1Index',]
    elExclude = []
    alg += ElectronD3PDObject         (**_args (0, 'Electron', kw,
                                                sgkey = kw.get('electronContainer', 'ElectronAODCollection'),
                                                include = elInclude, exclude = elExclude))

    phInclude = [ 'HadLeakage', 'Layer1Shape', 'Layer1ShapeExtra', 'Layer2Shape',
                  'Iso','IsoPtCorrected', 'Conversion', 'Retaphi',
                  'TopoClusterIsolationCones','Pointing',
                  'ClusterKin','Samplings','UDTopoCones','CorrIso','ConvIP','TopoMatch','TopoKinematics','TopoEMMatch','TopoEMKinematics']
    phExclude = []
    alg += PhotonD3PDObject           (**_args (0, 'Photon', kw,
                                                sgkey = kw.get('photonContainer', 'PhotonAODCollection'),
                                                include = phInclude, exclude = phExclude))

    
    # Muon blocks
    muInclude = ['AcceptMethods']
    muExclude = ['TrkInfo','EFCBInfoIndex','EFCBInfo', 'EFMGInfo', 'EFMEInfo','L2CBInfoIndex','L2CBInfo','L1InfoIndex','L1Info','AllAuthor','SelectionFlagging','CBTrackDetails','MGTrackDetails']

    #Muid
    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "MuidMuonCollection" ):
        alg += MuonD3PDObject             (**_args (0, 'MuidMuon', kw,
                                                    sgkey='MuidMuonCollection', prefix='mu_muid_',
                                                    include = muInclude,
                                                    exclude = muExclude,
                                                    allowMissing = True ))

    #Staco
    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" ):
        alg += MuonD3PDObject             (**_args (0, 'StacoMuon', kw,
                                                    sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                    include = muInclude,
                                                    exclude = muExclude,
                                                    allowMissing = True ))

    #Third chain
    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "Muons" ):
        alg += MuonD3PDObject             (**_args (0, 'Muons', kw,
                                                     sgkey='Muons', prefix='mu_',
                                                     include = muInclude,
                                                     exclude = muExclude,
                                                     allowMissing = True ))

    # Jet blocks
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

    alg += JetD3PDObject              (**_args ( 1, 'AK4TopoEMJet', kw,
                                                 sgkey='AntiKt4TopoEMJets', prefix='jet_AntiKt4TopoEM_',
                                                 include = [],
                                                 exclude = ['JESMoments','JVtx',
                                                            'El02Match','Mu02Match','L1Info','L2Info','EFInfo'],
                                                 allowMissing = True))

    # EventShape variables
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

    #MET
    metKeys=['MET_LocHadTopo','MET_RefFinal','MET_RefEle','MET_RefMuon','MET_RefMuon_Staco','MET_RefMuon_Muid','MET_CorrTopo','MET_RefFinal_em','MET_Topo','MET_Calib','MET_CellOut_Eflow_Muons','MET_CellOut_Eflow', 'MET_CellOut_Eflow_Muid','MET_MuonBoy','MET_Muons','MET_Muid','MET_RefMuons',
             'MET_RefFinal_STVF','MET_RefJet_JVF', 'MET_RefJet_JVFCut','MET_CellOut_Eflow_STVF','MET_CellOut_Eflow_JetArea']

    for key in metKeys:
        alg += MissingETD3PDObject        (**_args (-1, key, kw,
                                                    prefix=key,
                                                    sgkey=key,
                                                    include=['MET_Phi','MET_Et','MET_SumEt'],
                                                    exclude=[],
                                                    allowMissing=True))
    #EMFrac for MET_LocHadTopo
    alg += TopoMETD3PDObject               (**_args (-1, 'MET_LocHadTopo_SUMET_EMFrac', kw,
                                                prefix='MET_LocHadTopo_',
                                                name_prefix='MET_LocHadTopo_SUMET_EMFrac_',
                                                sgkey='MET_LocHadTopo',
                                                include=['MET_Calo_EMFracSET'],
                                                exclude=[],
                                                allowMissing=True))


    # Primary vertex block - May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))
    # Truth

    if rec.doTruth():

        from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject

        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args ( 1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))

        # TruthJets
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJet', kw,
                                                 sgkey='AntiKt4TruthJets',prefix='jet_AntiKt4Truth_',
                                                 allowMissing = True))
    if not rec.doTruth():
        alg += BeamSpotD3PDObject(10)


    # Trigger

    if D3PDMakerFlags.DoTrigger():

        alg += ZPrimeEETriggerBitsD3PDObject      (**_args (10, 'ZPrimeEETriggerBits', kw))

        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)

        # Egamma
        alg += InitTrigD3PDObject(1)
        alg += EmTauROID3PDObject(1, prefix = "trig_L1_emtau_",
                                  exclude=['ThrInfo'])
        alg += TrigElectronD3PDObject(1, name='trig_L2_el_', prefix='trig_L2_el_',
                                      sgkey='HLT_L2ElectronFex,HLT_L2IDCaloFex',
                                      allowMissing = True)
        alg += TrigPhotonD3PDObject(1, name='trig_L2_ph_', prefix='trig_L2_ph_')
        alg += EFElectronD3PDObject(1, name='trig_EF_el_', 
                                    sgkey='HLT_egamma_Electrons,HLT_egamma',
                                    prefix='trig_EF_el_',
                                    allowMissing = True)
        alg += EFPhotonD3PDObject(1, name='trig_EF_ph_', 
                                  sgkey='HLT_egamma_Photons,HLT_egamma',
                                  prefix='trig_EF_ph_',
                                  allowMissing = True)

# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

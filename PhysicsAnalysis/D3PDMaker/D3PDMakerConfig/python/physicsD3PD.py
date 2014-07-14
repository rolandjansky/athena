# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerConfig/python/physicsD3PD.py
# @author maarten boonekamp <maarten.boonekamp@cea.fr>
# @date Sep, 2009
# @brief Construct a physics D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey                   import testSGKey

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject

from egammaD3PDMaker.ElectronD3PDObject                import ElectronD3PDObject
from egammaD3PDMaker.GSFElectronD3PDObject             import GSFElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonD3PDObject
from egammaD3PDMaker.egammaTriggerBitsD3PDObject       import egammaTriggerBitsD3PDObject

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject

from JetD3PDMaker.JetD3PDObject                        import JetD3PDObject

from TauD3PDMaker.TauD3PDObject                        import TauD3PDObject

from CaloD3PDMaker.MBTSD3PDObject                      import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject                  import MBTSTimeD3PDObject
from CaloD3PDMaker.MBTSTriggerBitsD3PDObject           import MBTSTriggerBitsD3PDObject
from CaloD3PDMaker.ZDCTriggerBitsD3PDObject            import ZDCTriggerBitsD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject          import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject         import CollisionDecisionD3PDObject
from CaloD3PDMaker.ClusterD3PDObject                   import ClusterD3PDObject
from CaloD3PDMaker.EMClusterD3PDObject                 import EMClusterD3PDObject
from TrackD3PDMaker.TrackD3PDObject                    import TrackParticleD3PDObject

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject

from MissingETD3PDMaker.MissingETD3PDObject            import MissingETD3PDObject
from MissingETD3PDMaker.MissingETGoodnessD3PDObject    import MissingETGoodnessD3PDObject
from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import METD3PDTriggerBitsObject


from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
from RecExConfig.RecFlags                              import rec

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def physicsD3PD (alg = None,
                 trigalg = None,
                 file = 'physics.root',
                 tuplename = 'physics',
                 **kw):
    
    # Core algorithm
    # By default the user of this function should have created an algorithm already.
    # But for backwards compatibility let the function create its own algorithm if
    # needed...
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )

    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    
    # Segregate trigger decision flags in a separate tree.
    if not trigalg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        trigalg = MSMgr.NewRootStream( tuplename + ':' + tuplename + 'TrigDec',
                                       FileName = file,
                                       TreeName = tuplename + 'TrigDec')
    trigalg += EventInfoD3PDObject        (0)
    alg.Stream.trigDecisionTree = trigalg

    # Add bunch structure metadata.
    from TriggerD3PDMaker.BunchStructureMetadata \
         import addBunchStructureMetadata
    addBunchStructureMetadata (alg)

    # Make track jets, but we can only do this if we have vertices.
    if testSGKey ('VxContainer' ,'VxPrimaryCandidate'):
        from D3PDMakerConfig.createTrackJets import createTrackJets
        createTrackJets (AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName()))
        alg += JetD3PDObject (**_args (-1, 'TrackJet', kw,
                                       prefix='trackjet_',
                                       sgkey='AntiKtZ5TrackJets'))
        k = 'Electron_EgammaTrackJetSignedIPAndPTRel_target'
        if not kw.has_key(k):
            kw[k] = 'trackjet_'

    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))
    
    # Electron/Photon blocks
    
    alg += ElectronD3PDObject         (**_args (10, 'Electron', kw,
                        EgammaJetSignedIPAndPTRel_target='jet_antikt4h1topo_'))
    alg += PhotonD3PDObject           (**_args (10, 'Photon', kw,
                                 ConversionVertexTrackIndex_target = 'trk'))
    if testSGKey ('ElectronContainer', D3PDMakerFlags.GSFElectronSGKey()):
        alg += GSFElectronD3PDObject  (**_args (level, 'GSFElectron', kw,
                                    sgkey = D3PDMakerFlags.GSFElectronSGKey(),
                                    prefix = 'el_gsf_'))
    
    # Muon blocks
    
    alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                sgkey='MuidMuonCollection',
                                                prefix='mu_muid_',
                                                allowMissing = True))
    alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                sgkey='StacoMuonCollection',
                                                prefix='mu_staco_',
                                                allowMissing = True))
    alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                sgkey='Muons',
                                                prefix='mu_',
                                                allowMissing = True))
    
    # Tau block
    
    alg += TauD3PDObject              (**_args ( 1, 'Tau', kw))
    
    # Jet blocks
    
    alg += JetD3PDObject              (**_args ( 9, 'AK4TopoJet', kw,
                                                 sgkey='AntiKt4TopoEMJets,' +
                                                       'AntiKt4TopoJets,' +
                                                       'AntiKt4H1TopoJets',
                                                 prefix='jet_antikt4h1topo_',
                                                 ConstitIndex_Target='cl_',
                                                 exclude = ['Constituents'],
                                                 allowMissing = True))
    alg += JetD3PDObject              (**_args ( 6, 'AK4TowerJet', kw,
                                                 sgkey='AntiKt4TowerAODJets,' +
                                                       'AntiKt4TowerJets,' +
                                                       'AntiKt4H1TowerJets',
                                                 prefix='jet_antikt4h1tower_',
                                                 exclude = ['Constituents'],
                                                 allowMissing = True))
    alg += JetD3PDObject              (**_args ( 6, 'AK6TowerJet', kw,
                                                 sgkey='AntiKt6TowerAODJets,'+
                                                       'AntiKt6TowerJets,'+
                                                       'AntiKt6H1TowerJets',
                                                 prefix='jet_antikt6h1tower_',
                                                 exclude = ['Constituents'],
                                                 allowMissing = True))
    #alg += JetD3PDObject              (**_args ( 9, 'AK4TopoJetLC', kw,
    #                                             sgkey='AntiKt4LCTopoJets', prefix='jet_antikt4lctopo_'))
    #alg += JetD3PDObject              (**_args ( 9, 'AK6TopoJetLC', kw,
    #                                             sgkey='AntiKt6LCTopoMyJets', prefix='jet_antikt6lctopo_')) 
    #alg += JetD3PDObject              (**_args ( 9, 'AK6TopoJetH1', kw,
    #                                             sgkey='AntiKt6H1TopoMyJets', prefix='jet_antikt6h1topo_'))

    # MET blocks

    alg += MissingETD3PDObject        (**_args (10, 'MissingET', kw,
                                                allowMissing = True))
    alg += MissingETGoodnessD3PDObject(**_args ( 2, 'METGoodness', kw))

    # track and cluster blocks
    
    alg += ClusterD3PDObject          (**_args ( 1, 'Cluster', kw))
    alg += EMClusterD3PDObject        (**_args ( 2, 'EMCluster', kw))

    # Many of these are just being set to the defaults --- that's because
    # they're being changed in TrigEgamma...
    alg += TrackParticleD3PDObject    (**_args (
        3, 'TrackParticle', kw,
        TruthParticleTarget = 'mc_',
        storeTruthInfo = True,
        trackParametersAtGlobalPerigeeLevelOfDetails = 3,
        storeDiagonalCovarianceAsErrors = True))
    
    # MBTS 
    
    alg += MBTSD3PDObject             (**_args (10, 'MBTS', kw))
    alg += MBTSTimeD3PDObject         (**_args (10, 'MBTSTime', kw))
    alg += MBTSTriggerBitsD3PDObject  (**_args (10, 'MBTSTriggerBits', kw))
    alg += CollisionDecisionD3PDObject(**_args (10, 'CollisionDecision', kw))

    # ZDC
    alg += ZDCTriggerBitsD3PDObject   (**_args (10, 'ZDCTriggerBits', kw))

    # Primary vertex block - May be missing in single-beam data.
    
    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_',
                                             storeVertexTrackAssociation = True,
                                             storeDiagonalCovarianceAsErrors = True))
    
    # Truth
    
    if rec.doTruth():
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject
        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args ( 1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))
        alg += JetD3PDObject          (**_args ( 3, 'AK4TruthJet', kw,
                                                 sgkey='AntiKt4TruthJets',
                                                 prefix='jet_antikt4truth_',
                                                 allowMissing = True))
        #alg += JetD3PDObject          (**_args ( 3, 'AK6Ttruth', kw,
        #                                         sgkey='AntiKt6TruthJets',
        #                                         prefix='jet_antikt6truth_'))
        
    # Trigger
        
    if D3PDMakerFlags.DoTrigger():
        from TriggerD3PDMaker.TrigDecisionD3PDObject \
             import TrigDecisionD3PDObject
        alg += TrigDecisionD3PDObject (**_args (2, 'TrigDecision', kw))
        TrigEgammaD3PDObjects (alg, **_args (1, 'TrigEgamma', kw))

        alg += egammaTriggerBitsD3PDObject    (**_args ( 2, 'EgammaTriggerBits', kw))
        alg += MuonTriggerBitsD3PDObject      (**_args ( 2, 'MuonTriggerBits', kw))
        alg += METD3PDTriggerBitsObject       (**_args ( 2, 'METTriggerBits', kw))

        from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
        from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject

        alg += EnergySumROID3PDObject(**_args (2, 'EnergySumROI', kw,
                                               prefix = "trig_L1_esum_",
                                               allowMissing = True))

        # The LVL2 information:
        alg += TrigMETD3PDObject(**_args( 2, 'TrigMETL2', kw,
                                          prefix = "trig_L2_met_",
                                          sgkey = "HLT_T2MissingET" ))

        # The EF information:
        alg += TrigMETD3PDObject(**_args ( 2, 'TrigMETEF', kw,
                                           prefix = "trig_EF_met_",
                                           sgkey = "HLT_TrigEFMissingET" ))

        from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))


# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerConfig/python/egammaD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Construct an egamma D3PD.
#

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey          import testSGKey
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
from egammaD3PDMaker.ElectronD3PDObject       import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject         import PhotonD3PDObject
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
# from MuonD3PDMaker.MuonTriggerBitsD3PDObject  import MuonTriggerBitsD3PDObject
# from MissingETD3PDMaker.MissingETD3PDObject   import MissingETD3PDObject
# from MissingETD3PDMaker.MissingETGoodnessD3PDObject import MissingETGoodnessD3PDObject
from JetD3PDMaker.JetD3PDObject               import JetD3PDObject
# from D3PDMakerConfig.METD3PDObject            import METD3PDObject
from CaloD3PDMaker.MBTSD3PDObject             import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject         import MBTSTimeD3PDObject
# from CaloD3PDMaker.MBTSTriggerBitsD3PDObject  import MBTSTriggerBitsD3PDObject
# from CaloD3PDMaker.ZDCTriggerBitsD3PDObject   import ZDCTriggerBitsD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject import CollisionDecisionD3PDObject
# from EventCommonD3PDMaker.LBMetadataConfig    import LBMetadataConfig
from TruthD3PDMaker.TruthEventD3PDObject      import TruthEventD3PDObject
# from TruthD3PDMaker.GenEventD3PDObject        import GenEventD3PDObject
from TruthD3PDMaker.TruthParticleD3PDObject   import TruthParticleD3PDObject
# from TrackD3PDMaker.VertexD3PDObject          import PrimaryVertexD3PDObject
# from TrigEgammaD3PDMaker.TrigEgammaD3PD       import TrigEgammaD3PDObjects
# from TrigMuonD3PDMaker.TrigMuonD3PD           import TrigMuonD3PDObjects
from RecExConfig.RecFlags                     import rec
from RecExConfig.ObjKeyStore                  import cfgKeyStore


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw


def createxAOD (seq):
    from D3PDMakerConfig.makexAOD import makexAOD, setVxLinks
    makexAOD (seq, 'xAOD::TruthParticleContainer',
              'TruthParticle', 'GEN_AOD',
              xaod_truth_event_key = 'TruthEvent',
              xaod_truth_vertex_key = 'TruthVertex',
              xaod_truth_links_key = 'xAODTruthLinks')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'CaloCalTopoCluster')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'egClusterCollection')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'LArClusterEMFrwd')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'LArClusterEMSofte')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'EMTopoSW35')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'HLT')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'HLT_TrigCaloClusterMaker')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'HLT_TrigCaloClusterMaker_slw')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'MuonClusterCollection')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'Tau1P3PCellCluster')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'Tau1P3PEM012ClusterContainer')
    makexAOD (seq, 'xAOD::CaloClusterContainer',  'Tai1P3PPi0ClusterContainer')
    makexAOD (seq, 'xAOD::JetContainer',          'AntiKt4TopoEMJets')
    makexAOD (seq, 'xAOD::JetContainer',          'AntiKt4TruthJets')

    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'InDetTrackParticles', 'Tracks',
              truth_key = 'TrackTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'GSFInDetTrackParticles', 'GSFTracks',
              truth_key = 'GSFTrackTruthCollection')

    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'TrackParticles', 'TrackParticleCandidate',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'GSFTrackParticles', 'GSFTrackParticleCandidate',
              truth_key = 'GSFTrackParticleTruthCollection',
              trackmap_key = 'GSFTrackAssociation',
              base_xaod_key = 'TrackParticles',
              base_key = 'TrackParticleCandidate')

    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'MuonboyTrackParticles',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'StacoTrackParticles',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'MuidExtrTrackParticles',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'MuidCombTrackParticles',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'ExtrapolatedMuonSpectrometerParticles',
              truth_key = 'TrackParticleTruthCollection')
    makexAOD (seq, 'xAOD::TrackParticleContainer',
              'CombinedfitMuonparticles',
              truth_key = 'TrackParticleTruthCollection')

    makexAOD (seq, 'xAOD::VertexContainer', 'AllPhotonsVxCandidates',
              TPContainerName = 'GSFTrackParticles')
    makexAOD (seq, 'xAOD::VertexContainer', 'VxPrimaryCandidate',
              TPContainerName = 'TrackParticles')

    makexAOD (seq, 'xAOD::ElectronContainer',
              'ElectronCollection', 'ElectronAODCollection',
              xAODContainerFrwdName = 'FwdElectrons')
    #makexAOD (seq, 'xAOD::ElectronContainer',
    #          'HLT_egamma', 'HLT_egamma',
    #          xAODContainerFrwdName = 'HLT_egamma_fwd')
    makexAOD (seq, 'xAOD::PhotonContainer',
              'PhotonCollection', 'PhotonAODCollection',
              topo_cluster_xaod_key = 'EMTopoSW35',
              vertex_xaod_key = 'AllPhotonsVxCandidates')

    makexAOD (seq, 'xAOD::MuonContainer',
              'StacoMuonCollection',
              xaod_tp_key = 'TrackParticles',
              xaod_sa_key = 'MuonboyTrackParticles',
              xaod_cb_key = 'StacoTrackParticles')
    makexAOD (seq, 'xAOD::MuonContainer',
              'MuidMuonCollection',
              xaod_tp_key = 'TrackParticles',
              xaod_sa_key = 'MuidExtrTrackParticles',
              xaod_cb_key = 'MuidCombTrackParticles')
    makexAOD (seq, 'xAOD::MuonContainer',
              'CaloMuonCollection',
              xaod_tp_key = 'TrackParticles',
              xaod_sa_key = '',
              xaod_cb_key = '')
    #makexAOD (seq, 'xAOD::MuonContainer',
    #          'Muons',
    #          xaod_tp_key = 'TrackParticles',
    #          xaod_sa_key = 'ExtrapolatedMuonSpectrometerParticles',
    #          xaod_cb_key = 'CombinedfitMuonparticles')

    #setVxLinks (seq, 'TrackParticles', 'VxPrimaryCandidate')
    #setVxLinks (seq, 'GSFTrackParticles', 'VxPrimaryCandidate')
    return


# Make a container merging xAOD central and forward electrons.
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
class MergeElectrons (PyAthena.Alg):
    def __init__ (self, name = 'MergeElectrons', **kw):
        super (MergeElectrons, self).__init__ (name = name, **kw)
        return
    def execute (self):
        import ROOT
        sg=PyAthena.py_svc('StoreGateSvc')
        e1 = sg['ElectronCollection']
        e2 = sg['FwdElectrons']
        enew = ROOT.DataVector(ROOT.xAOD.Electron_v1) (1) #VIEW_ELEMENTS
        for e in e1: enew.push_back(e)
        for e in e2: enew.push_back(e)
        ROOT.SetOwnership (enew, False)
        sg.record (enew, 'AllElectrons')
        cfgKeyStore.addTransient ('xAOD::ElectronContainer', 'AllElectrons')

        #e1 = sg['StacoMuonCollection']
        e1 = sg.retrieve (ROOT.DataVector(ROOT.xAOD.Muon_v1), 'StacoMuonCollection')
        if e1.size() > 0:
            reg = ROOT.SG.AuxTypeRegistry.instance()
            auxids = list(e1[0].getAuxIDs())
            auxids = [(reg.getName(id), id) for id in auxids]
            auxids.sort()
            print 'aaa', auxids
            acc = ROOT.SG.AuxElement.TypelessConstAccessor ('Loose')
            print 'bbb1', acc.isAvailable(e1[0])
        # if e2.size() > 0:
        #     acc = ROOT.SG.AuxElement.TypelessConstAccessor ('Loose')
        #     print 'bbb2', acc.isAvailable(e2[0])
        return StatusCode.Success
        


def egammaD3PD (alg = None,
                trigalg = None,
                level = 10,
                file = 'egamma.root',
                tuplename = 'egamma',
                **kw):

    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    createxAOD (preseq)

    if not cfgKeyStore.isInInput ('xAOD::ElectronContainer',
                                  'AllElectrons'):
        preseq += MergeElectrons()

    # Core algorithm
    # By default the user of this function should have created an algorithm already.
    # But for backwards compatibility let the function create its own algorithm if
    # needed...
    if not alg:
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        alg = MSMgr.NewRootStream( tuplename, file )

    alg += EventInfoD3PDObject        (**_args (level, 'EventInfo', kw))

    # # Segregate trigger decision flags in a separate tree.
    # if not trigalg:
    #     from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    #     trigalg = MSMgr.NewRootStream( tuplename + ':' + tuplename + 'TrigDec',
    #                                    FileName = file,
    #                                    TreeName = tuplename + 'TrigDec')

    # trigalg += EventInfoD3PDObject        (0)
    # alg.Stream.trigDecisionTree = trigalg

    # # Add bunch structure metadata.
    # from TriggerD3PDMaker.BunchStructureMetadata \
    #      import addBunchStructureMetadata
    # addBunchStructureMetadata (alg)

    alg += LArCollisionTimeD3PDObject (**_args (level, 'LArCollisionTime', kw))
    alg += ElectronD3PDObject         (**_args (level, 'Electron', kw,
                                                EgammaJetSignedIPAndPTRel_target='jet_'))
    alg += PhotonD3PDObject           (**_args (level, 'Photon', kw))
        

    # # Additional muon variables for Zg skims.
    # if not MuonD3PDObject.allBlocknames().has_key("MuonScatteringAngleSignificance"):
    #     from AthenaCommon.AppMgr import ToolSvc
    #     muonScatteringSigTool=None
    #     if hasattr(ToolSvc, "MuonScatteringSigTool"):
    #         muonScatteringSigTool=ToolSvc.MuonScatteringSigTool
    #     from JetTagD3PDMaker import MuonScatteringAngleSignificanceFillerTool
    #     MuonD3PDObject.defineBlock (100, "MuonScatteringAngleSignificance",
    #                                 MuonScatteringAngleSignificanceFillerTool,
    #                                 ScatteringSigTool=muonScatteringSigTool)
    alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                sgkey='StacoMuonCollection,Muons', prefix='mu_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex",
                                                           "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex",
                                                           "MuonScatteringAngleSignificance"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo",
                                                           "L2CBInfo", "L1Info"],
                                                allowMissing = True ))
    
    alg += JetD3PDObject              (**_args (0, 'Jet', kw,
                                                include=['JetQual',
                                                         'DQMoments']))
    # alg += METD3PDObject              (**_args (level, 'MET', kw))
    # alg += MissingETD3PDObject        (**_args (level, 'MissingET', kw,
    #                                             allowMissing = True))
    # alg += MissingETGoodnessD3PDObject(**_args ( 2, 'METGoodness', kw))
    alg += MBTSD3PDObject             (**_args (level, 'MBTS', kw))
    alg += MBTSTimeD3PDObject         (**_args (level, 'MBTSTime', kw))
    # alg += MBTSTriggerBitsD3PDObject  (**_args (level, 'MBTSTriggerBits', kw))
    # alg += ZDCTriggerBitsD3PDObject   (**_args (level, 'ZDCTriggerBits', kw))
    alg += CollisionDecisionD3PDObject(**_args (level, 'CollisionDecision', kw))
    # if D3PDMakerFlags.DoTrigger():
    #     from TriggerD3PDMaker.TrigDecisionD3PDObject \
    #          import TrigDecisionD3PDObject
    #     alg += TrigDecisionD3PDObject (**_args (2, 'TrigDecision', kw))
    #     TrigEgammaD3PDObjects (alg, **_args (1, 'TrigEgamma', kw,
    #                                          TrigEMCluster_level = 2))
    #     TrigMuonD3PDObjects   (alg, 1,
    #                            TrigMuonEFIsolation_allowMissing = True)

    #     from TriggerD3PDMaker.EnergySumROID3PDObject import EnergySumROID3PDObject
    #     from TrigMissingETD3PDMaker.TrigMETD3PDObject import TrigMETD3PDObject

    #     alg += EnergySumROID3PDObject(**_args (level, 'EnergySumROI', kw,
    #                                            prefix = "trig_L1_esum_",
    #                                            allowMissing = True))

    #     # The LVL2 information:
    #     alg += TrigMETD3PDObject(**_args( level, 'TrigMETL2', kw,
    #                                       prefix = "trig_L2_met_",
    #                                       sgkey = "HLT_T2MissingET" ))
    #     if cfgKeyStore.isInInputFile( "TrigMissingETContainer", "HLT_L2MissingET_FEB" ):
    #         alg += TrigMETD3PDObject(**_args( level, 'TrigMETL2_FEB', kw,
    #                                           prefix = "trig_L2_feb_met_",
    #                                           sgkey = "HLT_L2MissingET_FEB" ))

    #     # The EF information:
    #     alg += TrigMETD3PDObject(**_args ( level, 'TrigMETEF', kw,
    #                                        prefix = "trig_EF_met_",
    #                                        sgkey = "HLT_TrigEFMissingET" ))

        
    #     from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
    #     alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))


    # # May be missing in single-beam data.
    # alg += PrimaryVertexD3PDObject (**_args (
    #     1, 'PrimaryVertex', kw,
    #     allowMissing = True,
    #     sgkey = D3PDMakerFlags.VertexSGKey(),
    #     prefix = 'vxp_',
    #     storeVertexTrackIndexAssociation = False,
    #     storeVertexTrackAssociation = True,
    #     storeDiagonalCovarianceAsErrors = True))

    if rec.doTruth():
    #     from MuonD3PDMaker.TruthMuonD3PDObject        import TruthMuonD3PDObject
    #     alg += TruthMuonD3PDObject    (**_args (level, 'TruthMuon', kw))
        alg += TruthEventD3PDObject     (**_args (1, 'TruthEvent', kw))
        alg += TruthParticleD3PDObject(**_args (level, 'TruthParticle', kw))

    # alg.MetadataTools += [LBMetadataConfig()]

    # if D3PDMakerFlags.FilterCollCand():
    #     from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
    #     alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

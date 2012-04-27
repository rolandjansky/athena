# $Id: MCPD3PDMaker.py 497319 2012-04-23 20:11:54Z dimicco $
#
# @file PhysicsAnalysis/D3PDMaker/PhysicsD3PDMaker/python/SMWZphysicsD3PD.py
# @author maarten boonekamp <maarten.boonekamp@cea.fr>
# @date Sep, 2009
# @brief Construct a Standard Model WZ physics D3PD.
#


import D3PDMakerCoreComps

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from OutputStreamAthenaPool.MultipleStreamManager      import MSMgr

# event

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject

# eletron/photon

from egammaD3PDMaker.ElectronD3PDObject                import ElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonD3PDObject
from egammaD3PDMaker.egammaTriggerBitsD3PDObject       import egammaTriggerBitsD3PDObject

# muons

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject

# jet

from JetD3PDMaker.JetD3PDObject                        import JetD3PDObject

# tau

from TauD3PDMaker.TauD3PDObject                        import TauD3PDObject
from TauD3PDMaker.makeTrackFilterAlg                   import makeTrackFilterAlg

# calo

from CaloD3PDMaker.MBTSD3PDObject                      import MBTSD3PDObject
from CaloD3PDMaker.MBTSTimeD3PDObject                  import MBTSTimeD3PDObject
from CaloD3PDMaker.MBTSTriggerBitsD3PDObject           import MBTSTriggerBitsD3PDObject
from CaloD3PDMaker.LArCollisionTimeD3PDObject          import LArCollisionTimeD3PDObject
from CaloD3PDMaker.CollisionDecisionD3PDObject         import CollisionDecisionD3PDObject
from CaloD3PDMaker.ClusterD3PDObject                   import ClusterD3PDObject
from CaloD3PDMaker.EMClusterD3PDObject                 import EMClusterD3PDObject
from CaloD3PDMaker.makeClusterFilter                   import makeClusterFilter

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

from TrackD3PDMaker.TrackD3PDObject                    import TrackParticleD3PDObject

# PV config

TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 2

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject

# BeamSpot / MinBias

from TrackD3PDMaker.BeamSpotD3PDObject                 import BeamSpotD3PDObject

# MET

from MissingETD3PDMaker.MissingETD3PDMakerFlags        import MissingETD3PDMakerFlags
from MissingETD3PDMaker.MissingETD3PDObject            import *
from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import *

# Trigger

from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
# from TrigMuonD3PDMaker.TrigMuonD3PD                    import TrigMuonD3PDObjects
from TriggerD3PDMaker.EnergySumROID3PDObject           import EnergySumROID3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PDObject          import TrigMETD3PDObject

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


def MCPphysicsD3PD (file,
                                    tuplename = 'physics',
                                    seq = topSequence,
                                    D3PDSvc = 'D3PD::RootD3PDSvc',
                                    streamNameRoot = None,
                                    **kw):

    # define track and cluster filters

    filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                 OutputTracksName='GoodTracks',
                                 ptCut=0.,nSCTPix=4)

    filter2 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                 OutputTracksName='HighPtTracks',
                                 ptCut=5000.,nSCTPix=4)
    
    filter3 = makeClusterFilter(InputName  = D3PDMakerFlags.ClusterSGKey(),
                                OutputName = 'HighPtClusters',
                                ptCut=10000.)

    # FIXME (Maarten) : I need to give the container name explicitly apparently
    filter4 = makeClusterFilter(InputName  = 'EMTopoCluster430',
                                OutputName = 'HighPtEMClusters',
                                ptCut=10000.)
    
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    preseq += filter1
    preseq += filter2
    preseq += filter3
    preseq += filter4

    


    # now configure the D3PD
    
    alg = MSMgr.NewRootStream(StreamName = streamNameRoot, FileName = file, TreeName = tuplename)

    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    # Eta rings of energy
    # FIXME brian crashing aod running
    if not rec.readAOD:
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsNonTileD3PDObject
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsD3PDObject

        alg += EtaRingsNonTileD3PDObject  ( **_args ( 0, 'EtaRings', kw) )
        alg += EtaRingsD3PDObject         ( **_args ( 0, 'EtaRings', kw) )
    
    # Electron/Photon blocks
    
#    alg += ElectronD3PDObject         (**_args (10, 'Electron', kw,
#                                                exclude = ['EMTrackFitDetails','EgammaJetSignedIPAndPTRelKin']))

    alg += PhotonD3PDObject           (**_args (10, 'Photon', kw))

    
    # Muon blocks

    alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                sgkey='Muons', prefix='mu_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                sgkey='MuidMuonCollection', prefix='mu_muid_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    alg += MuonD3PDObject             (**_args (0, 'CaloMuon', kw,
                                                sgkey='CaloMuonCollection', prefix='mu_calo_'))    

    # Tau block
    
#    alg += TauD3PDObject              (**_args ( 1, 'Tau', kw))
    

    # Jet blocks
    
    alg += JetD3PDObject              (**_args ( 3, 'AK4TopoEMJet', kw,
                                                 sgkey='AntiKt4TopoEMJets', prefix='jet_akt4topoem_',
                                                 include = ['BTag','TrueFlavorComponents','BTagComponents'],
                                                 allowMissing = True))
    alg += JetD3PDObject              (**_args ( 3, 'AK6TopoEMJet', kw,
                                                 sgkey='AntiKt6TopoEMJets', prefix='jet_akt6topoem_',
                                                 include = ['BTag','TrueFlavorComponents','BTagComponents'],
                                                 allowMissing = True))

    from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))


    # MET blocks
    # a whole mess to remove x,y components separately for all flavours

    alg += MissingETD3PDObject        (**_args (10, 'MissingET', kw,
                                                exclude=['MET_Base', 'MET_Base0', 'MET_Truth_Int',
                                                         'MET_RefFinal_Comps','MET_Calib_Comps','MET_CellOut_Comps',
                                                         'MET_CorrTopo_Comps','MET_Cryo_Comps','MET_CryoCone_Comps',
                                                         'MET_Final_Comps','MET_LocHadTopo_Comps',
                                                         'MET_LocHadTopoObj_Comps','MET_Muid_Comps',
                                                         'MET_Muid_Spectro_Comps','MET_Muid_Track_Comps',
                                                         'MET_MuonBoy_Comps','MET_MuonBoy_Spectro_Comps',
                                                         'MET_MuonBoy_Track_Comps','MET_MuonMuid_Comps',
                                                         'MET_Muon_Comps','MET_Muon_Isol_Muid_Comps',
                                                         'MET_Muon_Isol_Staco_Comps','MET_Muon_NonIsol_Muid_Comps',
                                                         'MET_Muon_NonIsol_Staco_Comps','MET_Muon_Total_Muid_Comps',
                                                         'MET_Muon_Total_Staco_Comps','MET_RefEle_Comps',
                                                         'MET_RefEle_em_Comps','MET_RefGamma_Comps',
                                                         'MET_RefGamma_em_Comps','MET_RefJet_Comps',
                                                         'MET_RefJet_em_Comps','MET_RefMuon_Comps',
                                                         'MET_RefMuon_Muid_Comps','MET_RefMuon_Staco_Comps',
                                                         'MET_RefMuon_Track_Muid_Comps','MET_RefMuon_Track_Staco_Comps',
                                                         'MET_RefMuon_Track_em_Comps','MET_RefMuon_Track_Comps',
                                                         'MET_RefMuon_em_Comps','MET_RefTau_Comps',
                                                         'MET_RefTau_em_Comps','MET_SoftJets_Comps',
                                                         'MET_SoftJets_em_Comps','MET_Topo_Comps',
                                                         'MET_TopoObj_Comps','MET_Track_Comps','MET_Composition'],
                                                allowMissing=True))


    # HadronicRecoil blocks

#    alg += ElectronD3PDObject(0,  sgkey = "HR_selectedElectrons", prefix = "hr_el_")
#    alg += MuonD3PDObject( 0,     sgkey = "HR_selectedMuons",     prefix = "hr_mu_" )
#    alg += MissingETD3PDObject(0, sgkey = "RoughRecoil",          prefix = "hr_roughRecoil")
#    alg += MissingETD3PDObject(0, sgkey = "ueCorrection",         prefix = "hr_ueCorrection")
    
    # track and cluster blocks

    # ... all clusters, very low LOD
    
    alg += ClusterD3PDObject          (**_args ( 0, 'Clusters1', kw,
                                                 exclude='SamplingBasics'))

    # ... higher LOD for pt>10 GeV
    
    alg += ClusterD3PDObject          (**_args ( 2, 'Clusters2', kw,
                                                 sgkey  = 'HighPtClusters',
                                                 prefix = 'clpt10_'))
    
    alg += ClusterD3PDObject          (**_args ( 3, 'Clusters3', kw,
                                                 sgkey  = 'HighPtEMClusters',
                                                 prefix = 'emclpt10_'))

    # ... good tracks only (nSCT>3; no pt cut)
    
    alg += TrackParticleD3PDObject    (**_args ( 3, 'Tracks1', kw,
                                                 sgkey  = 'GoodTracks',
                                                 label  = 'trk',
                                                 prefix = 'trk_'))

    # ... high-pt tracks (nSCT>3; pt>5 GeV)
    
    from TrackD3PDMaker.TrackD3PDMakerFlags                import TrackD3PDFlags as highPtFlags
    highPtFlags.doTruth = True
    highPtFlags.storeDiagonalCovarianceAsErrors = True
    highPtFlags.storeHitTruthMatching = True
    highPtFlags.storePixelHitsOnTrack = False
    highPtFlags.storePixelHolesOnTrack = False
    highPtFlags.storePixelOutliersOnTrack = False
    highPtFlags.storeSCTHitsOnTrack = False
    highPtFlags.storeSCTHolesOnTrack = False
    highPtFlags.storeSCTOutliersOnTrack = False
    highPtFlags.storeTRTHitsOnTrack = False
    highPtFlags.storeTRTHolesOnTrack = False
    highPtFlags.storeTRTOutliersOnTrack = False
    highPtFlags.storeTrackFitQuality = True
    highPtFlags.storeTrackMomentum = True
    highPtFlags.storeTrackSummary = True
    highPtFlags.trackParametersAtBeamSpotLevelOfDetails = 0
    highPtFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
    highPtFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3
    
    from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject    

    HighPtTrackParticleD3PDObject = TrackD3PDObject(_label='trkpt5',
                                                    _prefix='trkpt5_',
                                                    _sgkey='HighPtTracks',
                                                    typeName='Rec::TrackParticleContainer',
                                                    flags=highPtFlags)

    alg += HighPtTrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                                 sgkey  = 'HighPtTracks',
                                                 label  = 'trkpt5',
                                                 prefix = 'trkpt5_'))

    
    # Primary vertex block - May be missing in single-beam data.

    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))
    
    # MBTS 

    alg += MBTSD3PDObject             (**_args (10, 'MBTS', kw))
    alg += MBTSTimeD3PDObject         (**_args (10, 'MBTSTime', kw))
    alg += MBTSTriggerBitsD3PDObject  (**_args (10, 'MBTSTriggerBits', kw))
    #alg += CollisionDecisionD3PDObject(**_args (10, 'CollisionDecision', kw))
    
    # Truth

    if rec.doTruth():

        from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject

        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args ( 1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))

        # TruthJets
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJet', kw,
                                                 sgkey='AntiKt4TruthJets',prefix='jet_antikt4truth_'))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJet', kw,
                                                 sgkey='AntiKt6TruthJets', prefix='jet_antikt6truth_'))
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJetALL', kw,
                                                 sgkey='AntiKt4TruthJets_ALL', prefix='jet_antikt4truthALL_'))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJetALL', kw,
                                                 sgkey='AntiKt6TruthJets_ALL', prefix='jet_antikt6truthALL_'))
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJetWZ', kw,
                                                 sgkey='AntiKt4TruthJets_WZ', prefix='jet_antikt4truthWZ_'))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJetWZ', kw,
                                                 sgkey='AntiKt6TruthJets_WZ', prefix='jet_antikt6truthWZ_'))
        

    if not rec.doTruth():
        alg += BeamSpotD3PDObject(10)
    
    
    # Trigger

    if D3PDMakerFlags.DoTrigger():

        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
        
        # Bunch group info

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        
        TrigEgammaD3PDObjects (alg, 0)
#        TrigMuonD3PDObjects( alg, 0)

        # Esum

        alg += EnergySumROID3PDObject(**_args (2, 'EnergySumROI', kw,
                                               prefix = "trig_L1_esum_"))

        # The LVL2 information:

        alg += TrigMETD3PDObject(**_args( 2, 'TrigMETL2', kw,
                                          prefix = "trig_L2_met_",
                                          sgkey = "HLT_T2MissingET" ))
        # The EF information:

        alg += TrigMETD3PDObject(**_args ( 2, 'TrigMETEF', kw,
                                           prefix = "trig_EF_met_",
                                           sgkey = "HLT_TrigEFMissingET" ))


# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

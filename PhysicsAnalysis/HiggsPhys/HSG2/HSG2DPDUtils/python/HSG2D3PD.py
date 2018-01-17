# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Andrea Di Simone
# created: Jan 2011
# based on M. Boonekamp's SMWZphysicsD3PD.py 
# 17.2.X.Y

import D3PDMakerCoreComps


from D3PDMakerConfig import D3PDMakerFlags
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

# Perform MV3 b-tagging when creating D3PD for 2013 Moriond analysis
JetTagD3PD_CollectionPostfix=""
import HSG2DPDUtils.HSG2DPDFlags
from AthenaCommon.JobProperties import jobproperties
if jobproperties.HSG2.doMV3BTagInD3PD():
    from BTagging.BTaggingFlags import BTaggingFlags
    # BTaggingFlags.CalibrationTag="BTagCalibALL-07-02" # up to now it is not linked to the global flag
    # preExec="from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag='BTagCalibALL-07-02'"
    BTaggingFlags.Jets=[ "AntiKt4TopoEM", "AntiKt4LCTopo" ] # Follow PhysicsAnalysis/D3PDMaker/JetTagD3PDMaker/trunk/share/JetTagD3PD_prodFragmentCore.py 
    BTaggingFlags.Active=True
    BTaggingFlags.JetFitterCharm=True
    BTaggingFlags.MV3_bVSu=True # MV3 is not usable in its current state. (Susumu 2013-02-26)
    BTaggingFlags.MV3_bVSc=True # MV3 is not usable in its current state. (Susumu 2013-02-26)
    BTaggingFlags.MV3_cVSu=True # MV3 is not usable in its current state. (Susumu 2013-02-26)
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
    JetTagD3PDFlags.Taggers+=["JetFitterCharm","MV3_bVSu","MV3_bVSc","MV3_cVSu"] # MV3 is not usable in its current state. (Susumu 2013-02-26)
    # JetTagD3PDFlags.Taggers+=["JetFitterCharm"]
    JetTagD3PDFlags.JetFitterCharmTagInfo=True
    JetTagD3PD_CollectionPostfix="ReTagged"

# event
from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig


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
from JetTagD3PDMaker.JetTagD3PDMakerKeys               import JetTagD3PDKeys


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


# remove G4 truth
from D3PDMakerConfig.D3PDMakerFlags                 import D3PDMakerFlags
D3PDMakerFlags.TruthWritePartons = True
D3PDMakerFlags.TruthWriteHadrons = True
D3PDMakerFlags.TruthWriteGeant = False

# baseline track config
from TrackD3PDMaker.TrackD3PDMakerFlags                import TrackD3PDFlags
TrackD3PDFlags.doTruth = False
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = False
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
# Add trk_nBLHits, trk_nPixHits, trk_nSCTHits, trk_nTRTHits, trk_nTRTHighTHits, trk_nTRTXenonHits (start)
# for ZH(->inv) analysis
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.storeTrackSummary.FullInfo = False
TrackD3PDFlags.storeTrackSummary.IDHits = True
TrackD3PDFlags.storeTrackSummary.IDHoles = False
TrackD3PDFlags.storeTrackSummary.IDSharedHits = False
TrackD3PDFlags.storeTrackSummary.IDOutliers = False
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = False
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.InfoPlus = False
TrackD3PDFlags.storeTrackSummary.MuonHits = False
TrackD3PDFlags.storeTrackSummary.MuonHoles = False
TrackD3PDFlags.storeTrackSummary.ExpectBLayer = False
TrackD3PDFlags.storeTrackSummary.HitSum = False
TrackD3PDFlags.storeTrackSummary.HoleSum = False
TrackD3PDFlags.storeTrackSummary.HitPattern = False
TrackD3PDFlags.storeTrackSummary.SiHits = False
TrackD3PDFlags.storeTrackSummary.TRTRatio = False
TrackD3PDFlags.storeTrackSummary.PixeldEdx = False
TrackD3PDFlags.storeTrackSummary.ElectronPID = False
# Add trk_nBLHits, trk_nPixHits, trk_nSCTHits, trk_nTRTHits, trk_nTRTHighTHits, trk_nTRTXenonHits (end)
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtBeamLineLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 1

from TrackD3PDMaker.TrackD3PDObject                    import TrackParticleD3PDObject

# PV config
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 3

from TrackD3PDMaker.VertexD3PDObject                   import PrimaryVertexD3PDObject

# BeamSpot / MinBias
from TrackD3PDMaker.BeamSpotD3PDObject                 import BeamSpotD3PDObject

# MET
from MissingETD3PDMaker.MissingETD3PDMakerFlags        import MissingETD3PDMakerFlags
MissingETD3PDMakerFlags.doCellOutEflow=True 
MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_" # Change jet_antikt4LCtopo_MET_ to jet_AntiKt4LCTopo_MET_ 
MissingETD3PDMakerFlags.METMuonsPrefix = "mu_muon_MET_" # Change mu_MET_ to mu_muon_MET_ 
from MissingETD3PDMaker.MissingETD3PDObject            import *
from MissingETD3PDMaker.MissingETD3PDTriggerBitsObject import *
from MissingET.METRefGetter_plup import *

# Trigger
from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
from TrigMuonD3PDMaker.TrigMuonD3PD                    import TrigMuonD3PDObjects
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


def HSG2physicsD3PD (name,file,
                     tuplename = 'physics',
                     **kw):

    from AthenaCommon.AlgSequence import AlgSequence  # needed for attila's code
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags # needed for attila's code
    
    # define track and cluster filters
    filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                 OutputTracksName='GoodTracks'+name,
                                 ptCut=0.,nSCTPix=4)

    filter2 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                 OutputTracksName='HighPtTracks'+name,
                                 ptCut=5000.,nSCTPix=4)
    
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    preseq += filter1
    preseq += filter2

    # now configure the D3PD 
    from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
    alg = MSMgr.NewRootStream( name, file, TreeName=tuplename)
    
    
    #alg = D3PDMakerCoreComps.MakerAlg(tuplename, seq,
    #                                  file = file,
    #                                  D3PDSvc = D3PDSvc,
    #                                  streamNameRoot = streamNameRoot)
    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))


    from D3PDMakerCoreComps.D3PDObject         import make_SG_D3PDObject

    import EventCommonD3PDMaker

    # Electron/Photon block
    alg += ElectronD3PDObject         (**_args (10, 'Electron', kw,
                                                exclude = ['L1Index', 'L2Index' ,'EMTrackFitDetails','EgammaJetSignedIPAndPTRelKin']))
    # Photon block
    alg += PhotonD3PDObject           (**_args (10, 'Photon', kw))

    
    # Muon blocks
    alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                sgkey='MuidMuonCollection', prefix='mu_muid_',
                                                include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex" ],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    alg += MuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                sgkey='CaloMuonCollection', prefix='mu_calo_',
                                                include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    
    alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                sgkey='Muons', prefix='mu_muon_',
                                                include = ["EFCBInfoIndex","EFMGInfoIndex", "EFMEInfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True
                                                ))
    # Quadruplet vertex block
    from AthenaCommon.AlgSequence import AlgSequence  
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags 
    from HSG2VertexCreation.HSG2VertexReconstruction import HSG2VertexReconstruction 
    HSG2VertexReconstruction( "HSG2VertexReconstruction", 
                              AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )) 
    
    from HiggsD3PDMaker.HSG2VertexReconstruction import addHSG2VertexReconstruction
    addHSG2VertexReconstruction( alg, electron_target = "el_",
                                 muid_target = "mu_muid_",
                                 staco_target = "mu_staco_",
                                 calo_target = "mu_calo_" ,
                                 muon_target = "mu_muon_" )
    
    # Tau block
    alg += TauD3PDObject              (**_args ( 1, 'Tau', kw)) 
    

    # Jet blocks
    alg += JetD3PDObject              (**_args ( 3, 'AK4TopoEMJet', kw,
                                                 sgkey='AntiKt4TopoEMJets'+JetTagD3PD_CollectionPostfix, prefix='jet_akt4topoem_',
                                                 #include = ['BTag','TrueFlavorComponents','BTagComponents'],
                                                 include = [JetTagD3PDKeys.BTagWeightsBlockName(),
                                                            JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                                                            JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName(), 
                                                            JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
                                                            JetTagD3PDKeys.TruthInfoBlockName(),
                                                            "Constituents",
                                                            'TracksMoments', 'Samplings', # For GSC corrections
                                                            ],
                                                 allowMissing = True))
    # For VBF and ZH(->inv) analyses  
    alg += JetD3PDObject              (**_args ( 3, 'AK4LCTopoJet', kw,
                                                 sgkey='AntiKt4LCTopoJets'+JetTagD3PD_CollectionPostfix, prefix='jet_AntiKt4LCTopo_',
                                                 include = [JetTagD3PDKeys.BTagWeightsBlockName(),
                                                            JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
                                                            JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName(), 
                                                            JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
                                                            JetTagD3PDKeys.TruthInfoBlockName(),                      
                                                            "Constituents",
                                                            "ConstituentScale", # For ZH(->inv) analysis
                                                            'TracksMoments', 'Samplings', # For GSC corrections
                                                            ],
                                                 allowMissing = True))

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
                                                         'MET_TopoObj_Comps','MET_Track_Comps'],
                                                allowMissing=True))

    # Pileup-subtracted MET RefFinal for ZH(->inv) analysis 
    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_STVF', suffix='STVF_', allowMissing =True,
                                            jetSGKey='AntiKt4LCTopoJets'+JetTagD3PD_CollectionPostfix, jetPrefix='jet_AntiKt4LCTopo_MET_') 

    # For H->ZZ->llqq analysis
    # PhysicsAnalysis/D3PDMaker/PhysicsD3PDMaker/python/SMWZD3PD.py
    if jobproperties.HSG2.doHSG5METInD3PD():
        METRefAlg_HSG5 = make_METRefAlg(_suffix='_HSG5')
        METRefAlg_HSG5.sequence                    = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
        METRefAlg_HSG5.jet_JetInputCollectionKey   = "AntiKt4TopoEMJets"+JetTagD3PD_CollectionPostfix
        METRefAlg_HSG5.jet_JetPtCut                = 20.0*GeV
        METRefAlg_HSG5.jet_ApplyJetScale           = "No"
        METRefAlg_HSG5.jet_UseJetMomentForScale    = True
        METRefAlg_HSG5.jet_JetMomentForScale       = "EMJES"
        METRefAlg_HSG5.jet_RunSoftJetsTool         = False
        METRefAlg_HSG5.jet_SoftJetsPtCut           = 7.0*GeV
        METRefAlg_HSG5.jet_SoftJetsMaxPtCut        = 20.0*GeV
        METRefAlg_HSG5.photon_doPhotonTool         = False
        METRefAlg_HSG5.tau_doTauTool               = False
        METRefAlg_HSG5.jet_SoftJetsCalibType       = "EmScale"
        METRefAlg_HSG5.jet_ApplySoftJetsScale      = "No"
        METRefAlg_HSG5.jet_calibType               = "ExclRefCalib"
        METRefAlg_HSG5.ele_calibType               = "RefCalib"
        METRefAlg_HSG5.gamma_calibType             = "EmScale"
        METRefAlg_HSG5.cellout_calibType           = "Eflow"
        METRefAlg_HSG5.tau_calibType               = "EmScale"
        METRefAlg_HSG5.cryo_ApplyCorrection        = "Off"
        METRefAlg_HSG5.muon_container              = "StacoMuonCollection"
        METRefAlg_HSG5.muon_algorithm              = "Staco"
        METRefAlg_HSG5.muon_isolationAlg           = "dRJet"
        #    METRefAlg_HSG5.jet_ApplyJetJVF             = "Yes"
        #    METRefAlg_HSG5.plupSuppCorr                ='STVF'
        #    METRefAlg_HSG5.celloutCorrection           ='STVF'
        # print METRefAlg_HSG5
        METRefAlg_HSG5()

        customMETs = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                      'MET_CellOut', 'MET_Cryo', 'MET_SoftJets',
                      'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                      'MET_CellOut_Eflow_STVF', 'MET_CellOut_Eflow_JetArea',
                      'MET_RefFinal_STVF' ]
        for custom in customMETs:
            alg += MissingETD3PDObject (level=0, sgkey = custom+'_HSG5',prefix='MET_HSG5_'+custom[4:],
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
                                                 'MET_TopoObj_Comps','MET_Track_Comps'],
                                        allowMissing=True)
            alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_HSG5', suffix='HSG5_', allowMissing =True,
                                                    jetSGKey='AntiKt4TopoEMJets'+JetTagD3PD_CollectionPostfix, jetPrefix='jet_AntiKt4TopoEM_MET_')
    
    ### large-R jets
    ### Copied from D3PDMaker/QcdD3PDMaker/share/JetMetD3PD_prodJobOFragment.py
    if jobproperties.HSG2.doLargeRJetsInD3PD():
        from RecExConfig.ObjKeyStore import objKeyStore
        if objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
            preseqLRJ = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
            from QcdD3PDMaker.JSjets import createJSJets
            from QcdD3PDMaker.JSD3PD import JSD3PD
            from HSG2DPDUtils.GroomedJetsConfig import getGroomedJetsConfig
            dictsConfig = getGroomedJetsConfig()
            for dC in dictsConfig:
                xx = createJSJets(dC[0], dC[1], preseqLRJ)
                if xx[0] != None and xx[1] != None:
                    JSD3PD(xx, alg)

    # ... good tracks only (nSCT>3; no pt cut)
    alg += TrackParticleD3PDObject    (**_args ( 3, 'Tracks1', kw,
                                                 sgkey  = 'GoodTracks'+name,
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
    
    alg += TrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                             sgkey  = 'HighPtTracks'+name,
                                             label  = 'trkpt5',
                                             prefix = 'trkpt5_'))

    
    # Primary vertex block - May be missing in single-beam data.
    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_'))
    
    # Truth
    if rec.doTruth():

        from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject
        from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject

        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))
        alg += GenEventD3PDObject     (**_args ( 1, 'GenEvent', kw))
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))

        # TruthJets
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJet', kw,
                                                 sgkey='AntiKt4TruthJets',prefix='jet_antikt4truth_', allowMissing=True))

        # add Heavy flavour overlap
        from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
        alg += HforD3PDObject(**_args ( 0, 'HforD3PD', kw))

        # add mcVx information 
        from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject
        alg += TruthVertexD3PDObject(**_args (0, 'TruthVertex', kw))  

    if not rec.doTruth():
        alg += BeamSpotD3PDObject(10)
    
    
    
    if D3PDMakerFlags.DoTrigger():
                
        # Trigger Decision + metadata
        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)

        
        from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PDObjects
        TrigMETD3PDObjects( alg )

        from TrigEgammaD3PDMaker.TrigEgammaD3PD       import TrigEgammaD3PDObjects
        TrigMuonD3PDObjects( alg, 1)
        TrigEgammaD3PDObjects (alg, level=10)


    # Event metadata
    alg.MetadataTools += [LBMetadataConfig()]

    from EventCommonD3PDMaker.CutFlowMetadataConfig        import CutFlowMetadataConfig
    alg.MetadataTools += [CutFlowMetadataConfig(alg)]

    # Bunch train information
    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    addBunchStructureMetadata( alg )

    ## Silence the trigger navigation warnings about missing containers:
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, "TrigDecisionTool" ):
        from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
        pass
    ToolSvc.TrigDecisionTool.Navigation.OutputLevel=5

    
    # EventShape variables
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/D3PDVariablesForPileup#EventShape_variables
    from QcdD3PDMaker.QcdEventShapeD3PDObject import EventShapeD3PDObject
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

    return alg

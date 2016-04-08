# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# @file HSG5D3PD.py
# @author Robert Harrington <R.Harrington@ed.ac.uk> 
# @date Oct, 2011
# @brief Construct a boosted Higgs physics D3PD (copied from and based on SMWZD3PD).
#


import D3PDMakerCoreComps
from D3PDMakerCoreComps.resolveSGKey                   import testSGKey

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from OutputStreamAthenaPool.MultipleStreamManager      import MSMgr

# local flags
from HSG5Flags import *
from D3PDMakerCoreComps.IndexMultiAssociation          import IndexMultiAssociation


# event

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig
from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject

from D3PDMakerCoreComps.ContainedVectorMultiAssociation import ContainedVectorMultiAssociation


# eletron/photon

from egammaD3PDMaker.ElectronD3PDObject                import ElectronD3PDObject
from egammaD3PDMaker.GSFElectronD3PDObject             import GSFElectronD3PDObject
from egammaD3PDMaker.PhotonD3PDObject                  import PhotonD3PDObject
from egammaD3PDMaker.egammaTriggerBitsD3PDObject       import egammaTriggerBitsD3PDObject

# muons

from MuonD3PDMaker.MuonD3PDObject                      import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject               import MuonSegmentD3PDObject
from MuonD3PDMaker.MuonTriggerBitsD3PDObject           import MuonTriggerBitsD3PDObject

# jet

from JetD3PDMaker.JetD3PDObject                        import JetD3PDObject

# tau, not for exclusion

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

#TrackD3PDFlags.doTruth = False # True
TrackD3PDFlags.doTruth = True
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
#TrackD3PDFlags.storeHitTruthMatching = False # True
TrackD3PDFlags.storeHitTruthMatching = True
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
#TrackD3PDFlags.storeTrackSummary = False # True
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 0
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 1

# setting specifically for HSG5D3PD
TrackD3PDFlags.storeTrackSummary.IDSharedHits = True # IC
TrackD3PDFlags.storeDetailedTruth=False
TrackD3PDFlags.storeVertexKinematics=False
TrackD3PDFlags.storeVertexPurity=False
#TrackD3PDFlags.storeVertexTrackIndexAssociation=False
TrackD3PDFlags.storeVertexTrackIndexAssociation=True
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails=1
TrackD3PDFlags.vertexPositionLevelOfDetails=1
TrackD3PDFlags.storeTrackInfo=True # IC
TrackD3PDFlags.storeTrackSummary=True # IC
D3PDMakerFlags.TruthWriteHadrons = True
# end HSG5D3PD configuration

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

MissingETD3PDMakerFlags.doCellOutEflow=True
MissingETD3PDMakerFlags.METDefaultJetCollectionSGKey = 'AntiKt4LCTopoJets'
MissingETD3PDMakerFlags.METDefaultJetPrefix = "jet_AntiKt4LCTopo_MET_"

# Trigger

from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
from TrigMuonD3PDMaker.TrigMuonD3PD                    import TrigMuonD3PDObjects
from TriggerD3PDMaker.EnergySumROID3PDObject           import EnergySumROID3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PDObject          import TrigMETD3PDObject
from PhysicsD3PDMaker.SMWZTriggerBitsD3PDObject        import SMWZTriggerBitsD3PDObject

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

# Routine to write GSCFactor and WidthFraction jet moments
#def addSTRUCTCalib(jetcoll , **options):
#    from JetCalibTools.MakeCalibSequences import alternateCalibSequence
#    emSeq = alternateCalibSequence('CAL:STRUCT1',options)
#    StructSeq = emSeq.CalibToolSequence[-1]
#    StructSeq.MomentTag = "GSCFactor"
#    StructSeq.SimpleFactorCorrection = True
#    for t in StructSeq.CalibToolSequence[:-1]: #switch off all moments writing but the last.                                                
#        t.WriteMoments = False

    #from JetRec.JetGetters import getJetCalibrationTool                                                                                    
    #emToolFromDict = getJetCalibrationTool(emSeq, "STRUCTcalib",inPlaceCorr=False)                                                         

#    from JetMomentTools.JetMomentToolsConf import JetMomentsFromCalib
#    momTool = JetMomentsFromCalib()
#    momTool.Calibrator = emSeq

#    from JetRec.JetMomentGetter import  make_JetMomentGetter
#    make_JetMomentGetter( jetcoll , [momTool])


def HSG5D3PD (alg = None,
              file = 'hsg5.root',
              tuplename = 'physics',
              streamname = 'd3pdstream',
              flags=HSG5Flags,
              **kw):

    if flags.doTaus():
        # tau
        from TauD3PDMaker.TauD3PDObject                        import TauD3PDObject

    # define associations
    if flags.doPhotons() and not PhotonD3PDObject.allBlocknames().has_key('RecoveryMatch'):
        from PhotonD3PDMaker.PhotonRecoveryAssociation import PhotonRecoveryAssociation
        PhotonRecoveryAssoc = PhotonRecoveryAssociation(PhotonD3PDObject,
                                                        'ElectronContainer',
                                                        D3PDMakerFlags.ElectronSGKey(),
                                                        prefix = 'el_',
                                                        blockname = 'RecoveryMatch',
                                                        target = 'ph_ElectronRecovered_')
        

    # define track and cluster filters

    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

    if flags.doGoodTracks() and not hasattr( preseq, "GoodTracks" ):
        filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='GoodTracks',
                                     ptCut=0.,nSCTPix=4)
        preseq += filter1
                    
    if flags.doHighPtTracks() and not hasattr( preseq, "HighPtTracks" ):
        filter2 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='HighPtTracks',
                                     ptCut=5000.,nSCTPix=4)
        preseq += filter2


    if flags.doHighPtClusters() and not hasattr( preseq, "HighPtClusters" ):
        filter3 = makeClusterFilter(InputName  = D3PDMakerFlags.ClusterSGKey(),
                                    OutputName = 'HighPtClusters',
                                    ptCut=10000.)
        preseq += filter3

    if flags.doHighPtEMClusters() and not hasattr( preseq, "HighPtEMClusters" ):
        filter4 = makeClusterFilter(InputName  = D3PDMakerFlags.EMTopoClusterSGKey(),
                                    OutputName = 'HighPtEMClusters',
                                    ptCut=10000.)
        preseq += filter4

    # perform recoil calculation
     
    if flags.doHadronicRecoil(): 
        from HadronicRecoil.HadronicRecoilConf import HadronicRecoil__ElectronSelector
        from HadronicRecoil.HadronicRecoilConf import HadronicRecoil__MuonSelector
        from HadronicRecoil.HadronicRecoilConf import HadronicRecoil__EflowSelector
        from HadronicRecoil.HadronicRecoilConf import HadronicRecoil__RecoilCalculation
        from HadronicRecoil.HadronicRecoilConf import HadronicRecoil__EflowRecoil
        
        if not hasattr( preseq, "HadronicRecoil::ElectronSelector" ):
            preseq += HadronicRecoil__ElectronSelector()
        if not hasattr( preseq, "HadronicRecoil::MuonSelector" ):
            preseq += HadronicRecoil__MuonSelector()
        if not hasattr( preseq, "HadronicRecoil::EflowSelector" ):
            preseq += HadronicRecoil__EflowSelector()
        if not hasattr( preseq, "HadronicRecoil::RecoilCalculation" ):
            preseq += HadronicRecoil__RecoilCalculation( ConeSizes = [0.20] )
        if not hasattr( preseq, "EflowRecoil_20" ):
            preseq += HadronicRecoil__EflowRecoil("EflowRecoil_20", 
                                                  TrackRough       = "RoughRecoil_track_20", 
                                                  TrackUE          = "ueCorrection_track_20", 
                                                  TrackRef         = "RefinedRecoil_track_20", 
                                                  ClusNoTrackRough = "RoughRecoil_clusNoTrack_20", 
                                                  ClusNoTrackUE    = "ueCorrection_clusNoTrack_20", 
                                                  ClusNoTrackRef   = "RefinedRecoil_clusNoTrack_20", 
                                                  RoughRecoil_out  = "RoughRecoil_Eflow_20", 
                                                  ueCorrection_out = "ueCorrection_Eflow_20", 
                                                  RefinedRecoil_out= "RefinedRecoil_Eflow_20", 
                                                  MET_out          = "MET_HR_Eflow_20" ) 
            
    # now configure the D3PD
    
    if not alg:
        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)

    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    addBunchStructureMetadata (alg)

    # Eta rings of energy
    # FIXME brian crashing aod running
    if not rec.readAOD:
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsNonTileD3PDObject
        from CaloD3PDMaker.RapgapD3PDObject       import EtaRingsD3PDObject

        alg += EtaRingsNonTileD3PDObject  ( **_args ( 0, 'EtaRings', kw) )
        alg += EtaRingsD3PDObject         ( **_args ( 0, 'EtaRings', kw) )
    
    # Electron/Photon blocks
    
    alg += ElectronD3PDObject         (**_args (10, 'Electron', kw))

    if testSGKey ('ElectronContainer', D3PDMakerFlags.GSFElectronSGKey()):
        alg += GSFElectronD3PDObject  (**_args (10, 'GSFElectron', kw,
                                                sgkey = D3PDMakerFlags.GSFElectronSGKey(),
                                                prefix = 'el_gsf_'))

    if flags.doPhotons():
        print "adding photons"
        alg += PhotonD3PDObject           (**_args (10, 'Photon', kw))


    # Muon blocks

     ### add the scattering significance filler - to be removed when added in the MuonD3PDMaker
    if not MuonD3PDObject.allBlocknames().has_key("MuonScatteringAngleSignificance"):
        from AthenaCommon.AppMgr import ToolSvc
        muonScatteringSigTool=None
        if hasattr(ToolSvc, "MuonScatteringSigTool"):
            muonScatteringSigTool=ToolSvc.MuonScatteringSigTool
        from JetTagD3PDMaker import MuonScatteringAngleSignificanceFillerTool
        MuonD3PDObject.defineBlock (100, "MuonScatteringAngleSignificance",
                                    MuonScatteringAngleSignificanceFillerTool,
                                    ScatteringSigTool=muonScatteringSigTool)

    alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                sgkey='MuidMuonCollection', prefix='mu_muid_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True ))

    alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                           "L2CBInfoIndex", "L1InfoIndex"],
                                                exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                allowMissing = True ))
    
    from AthenaCommon.KeyStore import CfgItemList
    if CfgItemList("KeyStore_inputFile").has_item("Analysis::MuonContainer#Muons"):
        alg += MuonD3PDObject             (**_args (10, 'Muons', kw,
                                                    sgkey='Muons', prefix='mu_muons_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))

    alg += MuonD3PDObject             (**_args (0, 'CaloMuon', kw, sgkey='CaloMuonCollection', prefix='mu_calo_',
                                                include = ["MuonHitSummary", "Authors", "Likelihood",
                                                           "Isolation", "CaloEnergyLoss", "Quality"] ))
    
    # Tau block

    if flags.doTaus():
        alg += TauD3PDObject              (**_args ( 1, 'Tau', kw))



     # track and cluster blocks

    # ... all clusters, very low LOD
    
    alg += ClusterD3PDObject          (**_args ( 0, 'Clusters1', kw,
                                                 exclude='SamplingBasics'))

    # ... higher LOD for pt>10 GeV
    if flags.doHighPtClusters():
        alg += ClusterD3PDObject          (**_args ( 2, 'Clusters2', kw,
                                                     sgkey  = 'HighPtClusters',
                                                     prefix = 'clpt10_'))

    if flags.doHighPtEMClusters():    
        alg += ClusterD3PDObject          (**_args ( 3, 'Clusters3', kw,
                                                     sgkey  = 'HighPtEMClusters',
                                                     prefix = 'emclpt10_'))

    from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject    

    # ... good tracks only (nSCT>3; no pt cut)

    # use standard level of track info for 'GoodTracks'

    if flags.doGoodTracks():
        # use standard level of track info for 'GoodTracks'
        alg += TrackParticleD3PDObject    (**_args ( 3, 'Tracks1', kw,
                                                     sgkey  = 'GoodTracks',
                                                     label  = 'trk',
                                                     prefix = 'trk_'))
            
        # ... high-pt tracks (nSCT>3; pt>5 GeV)
            
    if flags.doHighPtTracks():
        
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
        highPtFlags.storeTrackSummary.IDOutliers = True
        highPtFlags.storeTrackSummary.PixelInfoPlus = True
        highPtFlags.storeTrackSummary.SCTInfoPlus = True
        highPtFlags.storeTrackSummary.TRTInfoPlus = True
        highPtFlags.storeTrackSummary.InfoPlus = True
        highPtFlags.trackParametersAtBeamSpotLevelOfDetails = 0
        highPtFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
        highPtFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3  

        HighPtTrackParticleD3PDObject = TrackD3PDObject(_label='trkpt5',
                                                        _prefix='trkpt5_',
                                                        _sgkey='HighPtTracks',
                                                        typeName='Rec::TrackParticleContainer',
                                                        flags=highPtFlags)
            
        alg += HighPtTrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                                       sgkey  = 'HighPtTracks',
                                                       label  = 'trkpt5',
                                                       prefix = 'trkpt5_'))

  
    # Jet blocks
    
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

    # include list modified for HSG5D3PD
    
    includelist = JetTagD3PDFlags.StdInfoInclude()

    from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
    includelist += [JetTagD3PDKeys.BTagWeightsBlockName()]
    #includelist += [JetTagD3PDKeys.JetFitterInfoBaseBlockName()] 
    #includelist += [JetTagD3PDKeys.JetFitterTagInfoBlockName()] 
    #includelist += [JetTagD3PDKeys.IP3DInfoBaseBlockName()] 
    #includelist += [JetTagD3PDKeys.IP3DIPInfoBaseBlockName()] 
    #includelist += [JetTagD3PDKeys.JetFitterCombInfoBaseBlockName()] 
    includelist += [JetTagD3PDKeys.JetTrackAssocBlockName()] 
    includelist += [JetTagD3PDKeys.JetTrackBlockName()] 
    includelist += [JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()] 
    includelist += [JetTagD3PDKeys.IPInfoPlusTrackBlockName()] 
    includelist += ['ConstituentScale']
    
    jetkw = kw
    if not jetkw.has_key ( 'AK4TopoEMJet_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target'):
        jetkw['AK4TopoEMJet_'+JetTagD3PDKeys.JetTrackAssocBlockName()+'_target']= "trk" 
    if not jetkw.has_key ( 'AK4TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target'):
        jetkw['AK4TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk" 

    alg += JetD3PDObject              (**_args ( 3, 'AK4TopoEMJet', jetkw,
                                                 sgkey='AntiKt4TopoEMJets', prefix='jet_AntiKt4TopoEM_',
                                                 include = includelist,
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))

    alg += JetD3PDObject              (**_args ( 3, 'AK6TopoEMJet', kw,
                                                 sgkey='AntiKt6TopoEMJets', prefix='jet_AntiKt6TopoEM_',
                                                 include = [JetTagD3PDFlags.StdInfoInclude(),'ConstituentScale'],
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))

    alg += JetD3PDObject              (**_args ( 3, 'AK4LCTopoJet', kw,
                                                 sgkey='AntiKt4LCTopoJets', prefix='jet_AntiKt4LCTopo_',
                                                 exclude=['Layer'],
                                                 include=['ConstituentScale'],
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))
    
    alg += JetD3PDObject              (**_args ( 3, 'AK6LCTopoJet', kw,
                                                 sgkey='AntiKt6LCTopoJets', prefix='jet_AntiKt6LCTopo_',
                                                 exclude=['Layer'],
                                                 include=['ConstituentScale'],
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))


    from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))


    # MET blocks
    # a whole mess to remove x,y components separately for all flavours

    if flags.doAllMissingET() :
        alg += MissingETD3PDObject        (**_args (10, 'MissingET', kw,
                                                    exclude=['MET_Base', 'MET_Base0', 'MET_Truth_Int',
                                                             'MET_RefFinal_Comps','MET_Calib_Comps','MET_CellOut_Comps',
                                                             'MET_CorrTopo_Comps','MET_Cryo_Comps','MET_CryoCone_Comps',
                                                             'MET_Final_Comps','MET_LocHadTopo_Comps',
                                                             'MET_LocHadTopoObj_Comps',
                                                             'MET_Muid_Comps','MET_Muid_Spectro_Comps','MET_Muid_Track_Comps',
                                                             'MET_MuonBoy_Comps','MET_MuonBoy_Spectro_Comps','MET_MuonBoy_Track_Comps',
                                                             'MET_Muons_Comps','MET_Muons_Spectro_Comps','MET_Muons_Track_Comps',
                                                             'MET_MuonMuid_Comps',
                                                             'MET_Muon_Comps','MET_Muon_Isol_Muid_Comps',
                                                             'MET_Muon_Isol_Staco_Comps','MET_Muon_NonIsol_Muid_Comps',
                                                             'MET_Muon_NonIsol_Staco_Comps','MET_Muon_Total_Muid_Comps',
                                                             'MET_Muon_Total_Staco_Comps','MET_RefEle_Comps',
                                                             'MET_RefEle_em_Comps','MET_RefGamma_Comps',
                                                             'MET_RefGamma_em_Comps','MET_RefJet_Comps',
                                                             'MET_RefJet_em_Comps','MET_RefMuon_Comps',
                                                             'MET_RefMuon_Muid_Comps','MET_RefMons_Comps','MET_RefMuon_Staco_Comps',
                                                             'MET_RefMuon_Track_Muid_Comps','MET_RefMuon_Track_Staco_Comps',
                                                             'MET_RefMuon_Track_em_Comps','MET_RefMuon_Track_Comps',
                                                             'MET_RefMuons_Track_Comps',
                                                             'MET_RefMuon_em_Comps','MET_RefTau_Comps',
                                                             'MET_RefTau_em_Comps','MET_SoftJets_Comps',
                                                             'MET_SoftJets_em_Comps','MET_Topo_Comps',
                                                             'MET_TopoObj_Comps','MET_Track_Comps','MET_Comps',
                                                             'MET_Track_Weights','MET_Cluster_Weights'],
                                                    allowMissing=True))
    else :
        alg += MissingETD3PDObject(0, sgkey = "MET_RefFinal",                prefix = "MET_RefFinal")

        # MuonBoy terms used in RefFinal
        alg += MissingETD3PDObject(0, sgkey = "MET_MuonBoy",                 prefix = "MET_MuonBoy")
        alg += MissingETD3PDObject(0, sgkey = "MET_RefMuon",                 prefix = "MET_RefMuon")
        alg += MissingETD3PDObject(0, sgkey = "MET_CellOut_Eflow",           prefix = "MET_CellOut_Eflow")

        # terms needed to calculate RefFinal with Muid instead of Staco
        alg += MissingETD3PDObject(0, sgkey = "MET_RefMuon_Muid",            prefix = "MET_RefMuon_Muid")
        alg += MissingETD3PDObject(0, sgkey = "MET_MuonMuid",                prefix = "MET_MuonMuid")
        alg += MissingETD3PDObject(0, sgkey = "MET_Muid",                    prefix = "MET_Muid")
        alg += MissingETD3PDObject(0, sgkey = "MET_CellOut_Eflow_Muid",      prefix = "MET_CellOut_Eflow_Muid")

        # terms needed to calculate RefFinal with third chain instead of Staco
        from AthenaCommon.KeyStore import CfgItemList
        keystore = CfgItemList("KeyStore_inputFile")
        if keystore.has_item("MissingET#MET_Muons"):
            alg += MissingETD3PDObject(0, sgkey = "MET_Muons",                    prefix = "MET_Muons")
        if keystore.has_item("MissingET#MET_RefMuons"):
            alg += MissingETD3PDObject(0, sgkey = "MET_RefMuons",                 prefix = "MET_RefMuons")
        if keystore.has_item("MissingET#MET_CellOut_Eflow_Muons"):
            alg += MissingETD3PDObject(0, sgkey = "MET_CellOut_Eflow_Muons",      prefix = "MET_CellOut_Eflow_Muons")
        
        # I have no idea what these are for
        alg += MissingETD3PDObject(0, sgkey = "MET_SoftJets",                prefix = "MET_SoftJets")
        alg += MissingETD3PDObject(0, sgkey = "MET_RefFinal_em",             prefix = "MET_RefFinal_em")
        alg += MissingETD3PDObject(0, sgkey = "MET_CellOut_em",              prefix = "MET_CellOut_em")
        alg += MissingETD3PDObject(0, sgkey = "MET_SoftJets_em",             prefix = "MET_SoftJets_em")
                   

    # HadronicRecoil blocks

    if flags.doHadronicRecoil():
        alg += ElectronD3PDObject(0,  sgkey = "HR_selectedElectrons",       prefix = "hr_el_")
        alg += MuonD3PDObject( 0,     sgkey = "HR_selectedMuons",           prefix = "hr_mu_" )
        
        alg += MissingETD3PDObject(0, sgkey = "RoughRecoil_20",                prefix = "hr_roughRecoil_20", 
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "ueCorrection_20",               prefix = "hr_ueCorrection_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "RefinedRecoil_20",              prefix = "hr_corrRecoil_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "MET_HR_20",                     prefix = "hr_MET_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        
        alg += MissingETD3PDObject(0, sgkey = "RoughRecoil_track_20",          prefix = "hr_roughRecoil_track_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "ueCorrection_track_20",         prefix = "hr_ueCorrection_track_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "RefinedRecoil_track_20",        prefix = "hr_corrRecoil_track_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "MET_HR_track_20",               prefix = "hr_MET_track_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        
        alg += MissingETD3PDObject(0, sgkey = "RoughRecoil_clusNoTrack_20",    prefix = "hr_roughRecoil_clusNoTrack_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "ueCorrection_clusNoTrack_20",   prefix = "hr_ueCorrection_clusNoTrack_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "RefinedRecoil_clusNoTrack_20",  prefix = "hr_corrRecoil_clusNoTrack_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "MET_HR_clusNoTrack_20",         prefix = "hr_MET_clusNoTrack_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        
        alg += MissingETD3PDObject(0, sgkey = "RoughRecoil_Eflow_20",          prefix = "hr_roughRecoil_Eflow_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "ueCorrection_Eflow_20",         prefix = "hr_ueCorrection_Eflow_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "RefinedRecoil_Eflow_20",        prefix = "hr_corrRecoil_Eflow_20",
                                   exclude=['MET_Regions','MET_Comps'] )  
        alg += MissingETD3PDObject(0, sgkey = "MET_HR_Eflow_20",               prefix = "hr_MET_Eflow_20", 
                                   exclude=['MET_Regions','MET_Comps'] )  


    """
    # track and cluster blocks

    # ... all clusters, very low LOD
    
    alg += ClusterD3PDObject          (**_args ( 0, 'Clusters1', kw,
                                                 exclude='SamplingBasics'))

    # ... higher LOD for pt>10 GeV
    if flags.doHighPtClusters():
        alg += ClusterD3PDObject          (**_args ( 2, 'Clusters2', kw,
                                                     sgkey  = 'HighPtClusters',
                                                     prefix = 'clpt10_'))

    if flags.doHighPtEMClusters():    
        alg += ClusterD3PDObject          (**_args ( 3, 'Clusters3', kw,
                                                     sgkey  = 'HighPtEMClusters',
                                                     prefix = 'emclpt10_'))

    from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject    

    # ... good tracks only (nSCT>3; no pt cut)

    # use standard level of track info for 'GoodTracks'

    if flags.doGoodTracks():
        # use standard level of track info for 'GoodTracks'
        alg += TrackParticleD3PDObject    (**_args ( 3, 'Tracks1', kw,
                                                     sgkey  = 'GoodTracks',
                                                     label  = 'trk',
                                                     prefix = 'trk_'))
            
        # ... high-pt tracks (nSCT>3; pt>5 GeV)
            
    if flags.doHighPtTracks():
        
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
        highPtFlags.storeTrackSummary.IDOutliers = True
        highPtFlags.storeTrackSummary.PixelInfoPlus = True
        highPtFlags.storeTrackSummary.SCTInfoPlus = True
        highPtFlags.storeTrackSummary.TRTInfoPlus = True
        highPtFlags.storeTrackSummary.InfoPlus = True
        highPtFlags.trackParametersAtBeamSpotLevelOfDetails = 0
        highPtFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
        highPtFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3  

        HighPtTrackParticleD3PDObject = TrackD3PDObject(_label='trkpt5',
                                                        _prefix='trkpt5_',
                                                        _sgkey='HighPtTracks',
                                                        typeName='Rec::TrackParticleContainer',
                                                        flags=highPtFlags)
            
        alg += HighPtTrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                                       sgkey  = 'HighPtTracks',
                                                       label  = 'trkpt5',
                                                       prefix = 'trkpt5_'))
    """       
    
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
                                                 sgkey='AntiKt4TruthJets',prefix='jet_AntiKt4Truth_',
                                                 allowMissing = True))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJet', kw,
                                                 sgkey='AntiKt6TruthJets', prefix='jet_AntiKt6Truth_',
                                                 allowMissing = True))
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJetALL', kw,
                                                 sgkey='AntiKt4TruthJets_ALL', prefix='jet_AntiKt4TruthALL_',
                                                 allowMissing = True))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJetALL', kw,
                                                 sgkey='AntiKt6TruthJets_ALL', prefix='jet_AntiKt6TruthALL_',
                                                 allowMissing = True))
        alg += JetD3PDObject          (**_args ( 1, 'AK4TruthJetWZ', kw,
                                                 sgkey='AntiKt4TruthJets_WZ', prefix='jet_AntiKt4TruthWZ_',
                                                 allowMissing = True))
        alg += JetD3PDObject          (**_args ( 1, 'AK6TruthJetWZ', kw,
                                                 sgkey='AntiKt6TruthJets_WZ', prefix='jet_AntiKt6TruthWZ_',
                                                 allowMissing = True))
        

    if not rec.doTruth():
        alg += BeamSpotD3PDObject(10)
    

    from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PDObjects
    TrigMETD3PDObjects(alg,10)
    
    # Trigger

    if D3PDMakerFlags.DoTrigger():

        alg += SMWZTriggerBitsD3PDObject      (**_args (10, 'SMWZTriggerBits', kw))
        
        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
        
        # Bunch group info

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        
        TrigEgammaD3PDObjects (alg, 1)
        TrigMuonD3PDObjects( alg, 1)

        # Esum

        #alg += EnergySumROID3PDObject(**_args (2, 'EnergySumROI', kw,
        #                                       prefix = "trig_L1_esum_"))

        # The LVL2 information:

        #alg += TrigMETD3PDObject(**_args( 2, 'TrigMETL2', kw,
        #                                  prefix = "trig_L2_met_",
        #                                  sgkey = "HLT_T2MissingET" ))
        # The EF information:

        #alg += TrigMETD3PDObject(**_args ( 2, 'TrigMETEF', kw,
        #                                   prefix = "trig_EF_met_",
        #                                   sgkey = "HLT_TrigEFMissingET" ))

# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

# Invoke routine to write GSCFactor and WidthFraction jet moments

    #addSTRUCTCalib('AntiKt4LCTopoJets', input='Topo', mainParam=0.4)
    #addSTRUCTCalib('AntiKt6LCTopoJets', input='Topo', mainParam=0.6)
    #addSTRUCTCalib('AntiKt4TopoEMJets', input='Topo', mainParam=0.4)
    #addSTRUCTCalib('AntiKt6TopoEMJets', input='Topo', mainParam=0.6)
    
    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

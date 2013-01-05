# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ZPrimeMMD3PD.py 531028 2012-12-18 13:40:00Z okepka $
#
# @file ExoticsD3PDMaker/python/ZPrimeMMD3PD.py
# @author Hulin Wang <Hulin.Wang@cern.ch>
# @author James Coggeshall <jcogges2@illinois.edu>
# @date April 2012
# @brief Construct a Exotics Zprime->MM D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerCoreComps.resolveSGKey                   import testSGKey

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
from egammaD3PDMaker.GSFElectronD3PDObject             import GSFElectronD3PDObject
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

TrackD3PDFlags.doTruth = True
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
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
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.storeTrackSummary.IDOutliers = True
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = True
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = True
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = True
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
from ExoticsD3PDMaker.ZPrimeMMTriggerBitsD3PDObject    import ZPrimeMMTriggerBitsD3PDObject

from TrigBJetD3PDMaker.TrigBJetD3PD                    import TrigBJetD3PDObjects



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


def ZPrimeMMD3PD (alg = None,
              file = 'smwz.root',
              tuplename = 'physics',
              streamname = 'd3pdstream',
              doSoftQCD = False,
              doThirdMuonChain = True,
              **kw):

    # define associations
    
    if not PhotonD3PDObject.allBlocknames().has_key('RecoveryMatch'):
        from PhotonD3PDMaker.PhotonRecoveryAssociation import PhotonRecoveryAssociation
        PhotonRecoveryAssoc = PhotonRecoveryAssociation(PhotonD3PDObject,
                                                        'ElectronContainer',
                                                        D3PDMakerFlags.ElectronSGKey(),
                                                        prefix = 'el_',
                                                        blockname = 'RecoveryMatch',
                                                        target = 'ph_ElectronRecovered_')
        

    # define track and cluster filters

    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

    if not hasattr( preseq, "GoodTracks" ):
        filter1 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='GoodTracks',
                                     ptCut=0.,nSCTPix=4)
        preseq += filter1

    if not hasattr( preseq, "HighPtTracks" ):
        filter2 = makeTrackFilterAlg(TracksName = D3PDMakerFlags.TrackSGKey(),
                                     OutputTracksName='HighPtTracks',
                                     ptCut=4000.,nSCTPix=4)
        preseq += filter2
    
    if not hasattr( preseq, "HighPtClusters" ):
        filter3 = makeClusterFilter(InputName  = D3PDMakerFlags.ClusterSGKey(),
                                    OutputName = 'HighPtClusters',
                                    ptCut=10000.)
        preseq += filter3

    if not hasattr( preseq, "HighPtEMClusters" ):
        filter4 = makeClusterFilter(InputName  = D3PDMakerFlags.EMTopoClusterSGKey(),
                                    OutputName = 'HighPtEMClusters',
                                    ptCut=10000.)
        preseq += filter4

    # perform recoil calculation


    import HadronicRecoil.Configurables as hrc
    preseq = hrc.add_hadronic_recoil_filters(preseq)

    # now configure the D3PD
    
    if not alg:
        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)
        
    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    addBunchStructureMetadata (alg)
    
    
    # Electron/Photon blocks
    
    alg += ElectronD3PDObject         (**_args (10, 'Electron', kw))

    if testSGKey ('ElectronContainer', D3PDMakerFlags.GSFElectronSGKey()):
        alg += GSFElectronD3PDObject  (**_args (10, 'GSFElectron', kw,
                                                sgkey = D3PDMakerFlags.GSFElectronSGKey(),
                                                prefix = 'el_gsf_'))

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

### Third muon chain variables!        
    if doThirdMuonChain:
        if cfgKeyStore.isInInput( "Analysis::MuonContainer", "Muons" ):
            alg += MuonD3PDObject             (**_args (10, 'Muon', kw,
                                                        sgkey='Muons', prefix='mu_',
                                                        include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                                   "L2CBInfoIndex", "L1InfoIndex",
                                                                   "MuonScatteringAngleSignificance"],
                                                        exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                        allowMissing = True ))
### Third muon chain variables!        


    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "MuidMuonCollection" ):
        alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                    sgkey='MuidMuonCollection', prefix='mu_muid_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex",
                                                               "MuonScatteringAngleSignificance"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))

    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" ):
        alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex",
                                                               "MuonScatteringAngleSignificance"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))

    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "CaloMuonCollection" ):
        alg += MuonD3PDObject             (**_args (0, 'CaloMuon', kw, sgkey='CaloMuonCollection', prefix='mu_calo_',
                                                    include = ["MuonHitSummary", "Authors", "Likelihood",
                                                               "Isolation", "CaloEnergyLoss", "Quality"] ))
    
    # Tau block
    
    alg += TauD3PDObject              (**_args ( 1, 'Tau', kw))
    

    # Jet blocks
    
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
    from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys

    ## add MSV target to the jetd3pd object properties 
    if not kw.has_key ('AK4TopoEMJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target'):
        kw['AK4TopoEMJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target']= JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel()
    if not kw.has_key ('AK6TopoEMJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target'):
        kw['AK6TopoEMJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target']= JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel()

    ## add track index for IPInfoPlus
    if not kw.has_key ('AK4TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6TopoEMJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk"
    ## add track index for SVInfoPlus
    if not kw.has_key ('AK4TopoEMJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4TopoEMJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6TopoEMJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6TopoEMJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target']= "trk"
    ## add track index for SV0InfoPlus
    if not kw.has_key ('AK4TopoEMJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4TopoEMJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6TopoEMJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6TopoEMJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target']= "trk"

  
    alg += JetD3PDObject              (**_args ( 3, 'AK4TopoEMJet', kw,
                                                 sgkey='AntiKt4TopoEMJets', prefix='jet_AntiKt4TopoEM_',
                                                 include = JetTagD3PDFlags.StdInfoInclude()
                                                 +[JetTagD3PDKeys.MultiSVInfoPlusBlockName()],
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))
    
    alg += JetD3PDObject              (**_args ( 3, 'AK6TopoEMJet', kw,
                                                 sgkey='AntiKt6TopoEMJets', prefix='jet_AntiKt6TopoEM_',
                                                 include = JetTagD3PDFlags.StdInfoInclude()
                                                 +[JetTagD3PDKeys.MultiSVInfoPlusBlockName()],
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))

    alg += JetD3PDObject              (**_args ( 3, 'AK4LCTopoJet', kw,
                                                 sgkey='AntiKt4LCTopoJets', prefix='jet_AntiKt4LCTopo_',
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))
    
    alg += JetD3PDObject              (**_args ( 3, 'AK6LCTopoJet', kw,
                                                 sgkey='AntiKt6LCTopoJets', prefix='jet_AntiKt6LCTopo_',
                                                 JetVertexFraction_FromUD=True,
                                                 JetVertexFraction_FillFullJVF=True,
                                                 allowMissing = True))

    ### add the MSV block
    MSVJetCollections = ['AntiKt4TopoEMJets', 'AntiKt6TopoEMJets' ]
    ### link multisv to tracks
    if not kw.has_key ('msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target'):
        kw['msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target']= "trk"

    from JetTagD3PDMaker.JetTagMSVVtxInJetD3PDObject import getJetTagMSVVtxInfoInJetD3PDObject
    alg += getJetTagMSVVtxInfoInJetD3PDObject(**_args(1,"msvvtxinfo", kw,
                                                      jetCollections=MSVJetCollections)
                                                )
 


    from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))

    # EventShape variables
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

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
                                                         'MET_TopoObj_Comps','MET_Track_Comps','MET_Comps',
                                                         'MET_Track_Weights','MET_Cluster_Weights'
                                                         'MET_RefFinal_STVF','MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                                                         'MET_CellOut_Eflow_STVF','MET_CellOut_Eflow_JetArea'],
                                                allowMissing=True))



    # ... DR = 0.15
 
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_15",                prefix = "hr_roughRecoil_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_15",               prefix = "hr_ueCorrection_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_15",              prefix = "hr_corrRecoil_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_15",                     prefix = "hr_MET_15",
                                exclude=['MET_Regions','MET_Comps'] )
 
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_15",          prefix = "hr_roughRecoil_Eflow_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_15",         prefix = "hr_ueCorrection_Eflow_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_15",        prefix = "hr_corrRecoil_Eflow_15",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_15",               prefix = "hr_MET_Eflow_15",
                                exclude=['MET_Regions','MET_Comps'] ) 

    # ... DR = 0.20

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_20",                prefix = "hr_roughRecoil_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_20",               prefix = "hr_ueCorrection_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_20",              prefix = "hr_corrRecoil_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_20",                     prefix = "hr_MET_20",
                                exclude=['MET_Regions','MET_Comps'] )
 
    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_20",          prefix = "hr_roughRecoil_track_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_20",         prefix = "hr_ueCorrection_track_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_20",        prefix = "hr_corrRecoil_track_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_20",               prefix = "hr_MET_track_20",
                                exclude=['MET_Regions','MET_Comps'] )    

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_clusNoTrack_20",    prefix = "hr_roughRecoil_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_clusNoTrack_20",   prefix = "hr_ueCorrection_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_clusNoTrack_20",  prefix = "hr_corrRecoil_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_clusNoTrack_20",         prefix = "hr_MET_clusNoTrack_20",
                                exclude=['MET_Regions','MET_Comps'] )  

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_20",          prefix = "hr_roughRecoil_Eflow_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_20",         prefix = "hr_ueCorrection_Eflow_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_20",        prefix = "hr_corrRecoil_Eflow_20",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_20",               prefix = "hr_MET_Eflow_20",
                                exclude=['MET_Regions','MET_Comps'] )

    # ... DR = 0.25

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_25",                prefix = "hr_roughRecoil_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_25",               prefix = "hr_ueCorrection_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_25",              prefix = "hr_corrRecoil_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_25",                     prefix = "hr_MET_25",
                                exclude=['MET_Regions','MET_Comps'] )

    alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_Eflow_25",          prefix = "hr_roughRecoil_Eflow_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_Eflow_25",         prefix = "hr_ueCorrection_Eflow_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_Eflow_25",        prefix = "hr_corrRecoil_Eflow_25",
                                exclude=['MET_Regions','MET_Comps'] )
    alg += MissingETD3PDObject( 0, sgkey = "MET_HR_Eflow_25",               prefix = "hr_MET_Eflow_25",
                                exclude=['MET_Regions','MET_Comps'] )
 

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

    from TrackD3PDMaker.TrackD3PDObject import TrackD3PDObject    

    # ... good tracks only (nSCT>3; no pt cut)

    # turn on detailed track info if writing SoftQCD version of D3PD
    if doSoftQCD :

        from TrackD3PDMaker.TrackD3PDMakerFlags                import TrackD3PDFlags as softQcdFlags

        softQcdFlags.doTruth = True
        softQcdFlags.storeDetailedTruth = False
        softQcdFlags.storeDiagonalCovarianceAsErrors = True
        softQcdFlags.storeHitTruthMatching = True
        softQcdFlags.storePixelHitsOnTrack = False
        softQcdFlags.storePixelHolesOnTrack = False
        softQcdFlags.storePixelOutliersOnTrack = False
        softQcdFlags.storeSCTHitsOnTrack = False
        softQcdFlags.storeSCTHolesOnTrack = False
        softQcdFlags.storeSCTOutliersOnTrack = False
        softQcdFlags.storeTRTHitsOnTrack = False
        softQcdFlags.storeTRTHolesOnTrack = False
        softQcdFlags.storeTRTOutliersOnTrack = False
        softQcdFlags.storeTrackFitQuality = True
        softQcdFlags.storeTrackInfo = True
        softQcdFlags.storeTrackMomentum = True
        softQcdFlags.storeTrackPredictionAtBLayer = True
        softQcdFlags.storeTrackSummary = True
        softQcdFlags.trackParametersAtBeamSpotLevelOfDetails = 0
        softQcdFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
        softQcdFlags.trackParametersAtPrimaryVertexLevelOfDetails = 2 

        SoftQCDTrackParticleD3PDObject = TrackD3PDObject(_label='trk',
                                                         _prefix='trk_',
                                                         _sgkey='GoodTracks',
                                                         typeName='Rec::TrackParticleContainer',
                                                         flags=softQcdFlags)

        alg += SoftQCDTrackParticleD3PDObject(**_args ( 3, 'Tracks1', kw,
                                                        sgkey  = 'GoodTracks',
                                                        label  = 'trk',
                                                        prefix = 'trk_'))

    else :

        # ... use standard level of track info for 'GoodTracks'

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
        highPtFlags.storeTrackSummary.IDOutliers = True
        highPtFlags.storeTrackSummary.PixelInfoPlus = True
        highPtFlags.storeTrackSummary.SCTInfoPlus = True
        highPtFlags.storeTrackSummary.TRTInfoPlus = True
        highPtFlags.storeTrackSummary.InfoPlus = True
        highPtFlags.trackParametersAtBeamSpotLevelOfDetails = 0
        highPtFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 2
        highPtFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3  

        HighPtTrackParticleD3PDObject = TrackD3PDObject(_label='trkpt4',
                                                        _prefix='trkpt4_',
                                                        _sgkey='HighPtTracks',
                                                        typeName='Rec::TrackParticleContainer',
                                                        flags=highPtFlags)

        alg += HighPtTrackParticleD3PDObject(**_args ( 3, 'Tracks2', kw,
                                                     sgkey  = 'HighPtTracks',
                                                     label  = 'trkpt4',
                                                     prefix = 'trkpt4_'))

    
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
        

        ### heavy flavor truth information
        from AthenaCommon.AppMgr import ToolSvc
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
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

        from TruthD3PDMaker.GenVertexD3PDObject import GenVertexD3PDObject
        alg += GenVertexD3PDObject( **_args(0, "hfgenvertex", kw, prefix='mchfvtx_',
                                            filter = smwzhfGenVtxFilterTool )
                                      )

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

        from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
        alg += GenParticleD3PDObject( **_args(10, "hfgenparticle", kw, prefix='mchfpart_',
                                              filter = smwzhfGenPartFilterTool,
                                              GenParticle_WriteMotherType=False,
                                              GenParticle_WriteMotherBarcode=False
                                              )
                                        )

        ### leptonic W/Z truth information
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

        from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
        alg += GenParticleD3PDObject( **_args(10, "lepwzgenparticle", kw, prefix='mclepwzpart_',
                                              filter = smwzlepwzGenPartFilterTool,
                                              label = "LepWZTruthD3PDObject",
                                              exclude = ["GenPartProdVertexAssoc","GenPartDecayVertexAssoc"],
                                              GenParticle_WriteMotherType=False,
                                              GenParticle_WriteMotherBarcode=False,
                                              GenPartMotherAssoc_target="LepWZTruthD3PDObject",
                                              GenPartChildAssoc_target="LepWZTruthD3PDObject"
                                              )
                                        )


    if not rec.doTruth():
        alg += BeamSpotD3PDObject(10)
    
    
    # Trigger

    if D3PDMakerFlags.DoTrigger():

        alg += ZPrimeMMTriggerBitsD3PDObject      (**_args (10, 'ZPrimeMMTriggerBits', kw))
        
        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
        
        # Bunch group info

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        
        TrigEgammaD3PDObjects (alg, 1)
        TrigMuonD3PDObjects( alg, 1)

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

        # The BJet information:
        TrigBJetD3PDObjects(alg,10, False, False, True, True, True)

# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

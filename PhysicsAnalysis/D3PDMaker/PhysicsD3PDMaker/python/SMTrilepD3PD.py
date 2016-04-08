# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: SMTrilepD3PD.py 531461 2012-12-20 21:43:15Z nedwards $
#
# @file PhysicsAnalysis/D3PDMaker/PhysicsD3PDMaker/python/SMTrilepD3PD.py
# @author maarten boonekamp <maarten.boonekamp@cea.fr>
# @date Sep, 2009
# @brief Construct a Standard Model WZ physics D3PD.
#


import D3PDMakerCoreComps
from D3PDMakerCoreComps.resolveSGKey                   import testSGKey

from D3PDMakerConfig.D3PDMakerFlags                    import D3PDMakerFlags

from OutputStreamAthenaPool.MultipleStreamManager      import MSMgr

# event

from EventCommonD3PDMaker.EventInfoD3PDObject          import EventInfoD3PDObject
from EventCommonD3PDMaker.LBMetadataConfig             import LBMetadataConfig

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

# Trigger

from TriggerD3PDMaker.TrigDecisionD3PDObject           import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata                 import addTrigConfMetadata
from TriggerD3PDMaker.BGCodeD3PDObject                 import BGCodeD3PDObject
#from TrigEgammaD3PDMaker.EFElectronD3PDObject          import EFElectronD3PDObject
from TrigEgammaD3PDMaker.TrigEgammaD3PD                import TrigEgammaD3PDObjects
from TrigMuonD3PDMaker.TrigMuonD3PD                import TrigMuonD3PDObjects
#from D3PDMakerCoreComps.D3PDObject import make_Void_D3PDObject
#from TrigMuonD3PDMaker.MuonFeatureD3PDObject import MuonFeatureD3PDObject
#from TrigMuonD3PDMaker.CombinedMuonFeatureD3PDObject import CombinedMuonFeatureD3PDObject
#from TrigMuonD3PDMaker.TrigMuonEFInfoD3PDObject import TrigMuonEFInfoD3PDObject
#from TriggerD3PDMaker.TrigRoiDescD3PDObject import TrigRoiDescD3PDObject
#from TriggerD3PDMaker.TrigNaviD3PDObject import \
        #ChainEntryD3PDObject, CombLinksD3PDObjects


from RecExConfig.RecFlags                              import rec
from AthenaCommon.AlgSequence                          import AlgSequence
topSequence = AlgSequence()


def _args (level, name, kwin, **kw):
    kw = kw.copy()
    kw['level'] = level
    for (k, v) in kwin.items():
        if k.startswith (name + '_'):
            kw[k[len(name)+1:]] = v
    return kw
      

def SMTrilepD3PD (alg                  = None,
                  #fileName              = 'smtrilep.root',
                   tuplename            = 'physics',
                  #streamname           = 'NTUP_SMTRILEP',
                   stdElectronContainer = 'None',
                   gsfElectronContainer = 'None',
                   stacoMuonContainer   = 'None',
                   muidMuonContainer    = 'None',
                   caloMuonContainer    = 'None',
                   thirdMuonContainer    = 'None',
                   photonContainer      = 'None',
                   jetContainer         = 'None',
                   jetLC4Container = 'None',
                   clusterContainer     = 'None',
                   tauContainer         = 'None',
                   trackContainer       = 'None',
                   addHadronicRecoil = False,
                   **kw):


    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

    # perform recoil calculation
    if addHadronicRecoil:
        import HadronicRecoil.Configurables as hrc
        preseq = hrc.add_hadronic_recoil_filters(preseq)


    # configure the D3PD
    if not alg:
        from AthenaCommon.JobProperties import jobproperties
        prodFlags = jobproperties.D3PDProdFlags
        from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
        fileName   = buildFileName( prodFlags.WriteSMTRILEPD3PD )
        streamName = prodFlags.WriteSMTRILEPD3PD.StreamName

        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = fileName, TreeName = tuplename)
        
    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    addBunchStructureMetadata (alg)

    # Electron/Photon blocks
    electron_excludes = [
        #'EMTrackFit',
        #'EMTrackFitDetailsBrem', 
        #'EMTrackFitDetailsRefit',
        'EMTrackFitDetailsRefitCov', 
        #'TopoClusterIsolationCones',
        'UDTopoCones', 'Rings', 'TrackIsoIP',
        'ElecDiscrim','TraversedMaterial', 'TrkCovDiag','TrkCovOffDiag',
        'TrackSummaryPID','UnrefitTrk', 'ElectronPointingVariables','MVAIso',
        'IsIso','ElectronClusterQuantities' ]

    
    if stdElectronContainer != 'None':
        alg += ElectronD3PDObject         (**_args (10, 'Electron', kw,
                                                    sgkey = stdElectronContainer,
                                                    prefix = 'el_',
                                                    exclude = electron_excludes,
                                                    allowMissing = True ))

    if (gsfElectronContainer != 'None') and testSGKey ('ElectronContainer', gsfElectronContainer):
        alg += GSFElectronD3PDObject  (**_args (1, 'GSFElectron', kw,
                                                sgkey = gsfElectronContainer,
                                                prefix = 'el_gsf_',
                                                exclude = electron_excludes,
                                                allowMissing = True ))

    D3PDMakerFlags.DoPAU = False
    if photonContainer != 'None':
        alg += PhotonD3PDObject           (**_args (0, 'Photon', kw,
                                                    sgkey = photonContainer,
                                                    prefix = 'ph_',
                                                    exclude = ['OQRecalc'],
                                                    allowMissing = True ))

    # Muon blocks

    # ... add the scattering significance filler - to be removed when added in the MuonD3PDMaker
    if not MuonD3PDObject.allBlocknames().has_key("MuonScatteringAngleSignificance"):
        from AthenaCommon.AppMgr import ToolSvc
        muonScatteringSigTool=None
        if hasattr(ToolSvc, "MuonScatteringSigTool"):
            muonScatteringSigTool=ToolSvc.MuonScatteringSigTool
        from JetTagD3PDMaker import MuonScatteringAngleSignificanceFillerTool
        MuonD3PDObject.defineBlock (100, "MuonScatteringAngleSignificance",
                                    MuonScatteringAngleSignificanceFillerTool,
                                    ScatteringSigTool=muonScatteringSigTool)

    std_muon_excludes = ['EFCBInfo', 'EFMGInfo', 'EFMEInfo', 'L2CBInfo', 'L1Info', 'MuonTimingFillerTool', 'Likelihood', 'MuonTimingFillerTool', 'PrimaryTrackDiagonalCovarianceAtPV', 'PrimaryTrackOffDiagonalCovarianceAtPV', 'IDTrackDiagonalCovarianceAtPV','IDTrackOffDiagonalCovarianceAtPV','METrackDiagonalCovarianceAtPV','METrackOffDiagonalCovarianceAtPV','TrkCovDiag','TrkCovOffDiag']
    calo_muon_excludes = ['EFCBInfo', 'EFMGInfo', 'EFMEInfo', 'L2CBInfo', 'L1Info', 'MuonTimingFillerTool', 'Likelihood', 'MuonTimingFillerTool', 'PrimaryTrackDiagonalCovarianceAtPV', 'PrimaryTrackOffDiagonalCovarianceAtPV', 'IDTrackDiagonalCovarianceAtPV','IDTrackOffDiagonalCovarianceAtPV','METrackDiagonalCovarianceAtPV','METrackOffDiagonalCovarianceAtPV','TrkCovDiag','TrkCovOffDiag', 'MGTrackDetails', 'MuGirlTrackBasics', 'CBTrackDetails', 'CombinedTrackBasics', 'MuonSpectrometerHitSummary']
        
    if stacoMuonContainer != 'None':
        alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey=stacoMuonContainer,
                                                    prefix='mu_staco_',
                                                    #include = ['EFCBInfoIndex', 'EFMGInfoIndex', 'EFMEInfoIndex',
                                                    #'MuonScatteringAngleSignificance'],
                                                    exclude = std_muon_excludes,
                                                    allowMissing = True ))
    
    if muidMuonContainer != 'None':
        alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                    sgkey=muidMuonContainer,
                                                    prefix='mu_muid_',
                                                    exclude = std_muon_excludes,
                                                    allowMissing = True ))
    
    if thirdMuonContainer != 'None':
        alg += MuonD3PDObject             (**_args (10, 'ThirdMuon', kw,
                                                    sgkey=thirdMuonContainer,
                                                    prefix='mu_',
                                                    exclude = std_muon_excludes,
                                                    allowMissing = True ))
    
    if caloMuonContainer != 'None':
        alg += MuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                    sgkey=caloMuonContainer,
                                                    prefix='mu_calo_',
                                                    exclude = calo_muon_excludes,
                                                    allowMissing = True ))
    
    # Jet block
    
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
    from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
    
    if jetContainer != 'None':
        alg += JetD3PDObject              (**_args ( 3, 'AK4TopoEMJet', kw,
                                                     sgkey=jetContainer,
                                                     allowMissing=True,
                                                     prefix='jet_AntiKt4TopoEM_',
                                                     JetVertexFraction_FromUD=True,
                                                     JetVertexFraction_FillFullJVF=True,
                                                     include = [ JetTagD3PDKeys.BTagWeightsBlockName(),
                                                                 JetTagD3PDKeys.TruthInfoBlockName() ] ))
    
    if jetLC4Container != 'None':
        alg += JetD3PDObject              (**_args ( 3, 'AK4LCJet', kw,
                                                     sgkey=jetContainer,
                                                     allowMissing=True,
                                                     prefix='jet_AntiKt4LCTopo_',
                                                     JetVertexFraction_FromUD=True,
                                                     JetVertexFraction_FillFullJVF=True,
                                                     include = [ JetTagD3PDKeys.BTagWeightsBlockName(),
                                                                 JetTagD3PDKeys.TruthInfoBlockName() ] ))


    # Tau block

    if tauContainer != 'None':
        alg += TauD3PDObject              (**_args ( 0, 'Tau', kw,
                                                     allowMissing=True,
                                                     sgkey=tauContainer ))

    
    # HF overlap removal (ALPGEN)

    from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))


    # MET blocks
    # a whole mess to remove x,y components separately for all flavours

    alg += MissingETD3PDObject   (**_args (10, 'MissingET', kw,
                                           exclude=['MET_Base','MET_Base0','MET_Truth',
                                                    'MET_RefFinal_Comps','MET_RefFinal_Regions',
                                                    'MET_LocHadTopo_Comps','MET_LocHadTopo_Regions',
                                                    'MET_CellOut_Comps','MET_CellOut_Regions',
                                                    'MET_Calib',
                                                    'MET_CorrTopo',
                                                    'MET_Cryo',
                                                    'MET_CryoCone',
                                                    'MET_Final',
                                                    'MET_LocHadTopoObj',
                                                    'MET_Muid',
                                                    'MET_Muid_Spectro',
                                                    'MET_Muid_Track',
                                                    'MET_MuonBoy',
                                                    'MET_MuonBoy_Spectro',
                                                    'MET_MuonBoy_Track',
                                                    'MET_MuonMuid',
                                                    'MET_Muon',
                                                    'MET_Muon_Isol_Muid',
                                                    'MET_Muon_Isol_Staco',
                                                    'MET_Muon_NonIsol_Muid',
                                                    'MET_Muon_NonIsol_Staco',
                                                    'MET_Muon_Total_Muid',
                                                    'MET_Muon_Total_Staco',
                                                    'MET_RefEle_Comps','MET_RefEle_Regions',
                                                    'MET_RefEle_em_Comps','MET_RefEle_em_Regions',
                                                    'MET_RefGamma_Comps','MET_RefGamma_Regions',
                                                    'MET_RefGamma_em_Comps','MET_RefGamma_em_Regions',
                                                    'MET_RefJet_Comps','MET_RefJet_Regions',
                                                    'MET_RefJet_em_Comps','MET_RefJet_em_Regions',
                                                    'MET_RefMuon_Comps','MET_RefMuon_Regions',
                                                    'MET_RefMuon_Muid_Comps','MET_RefMuon_Muid_Regions',
                                                    'MET_RefMuon_Staco_Comps','MET_RefMuon_Staco_Regions',
                                                    'MET_RefMuon_Track_Muid_Comps','MET_RefMuon_Track_Muid_Regions',
                                                    'MET_RefMuon_Track_Staco_Comps','MET_RefMuon_Track_Staco_Regions',
                                                    'MET_RefMuon_Track_em_Comps','MET_RefMuon_Track_em_Regions',
                                                    'MET_RefMuon_Track_Comps','MET_RefMuon_Track_Regions',
                                                    'MET_RefMuon_em_Comps','MET_RefMuon_em_Regions',
                                                    'MET_RefTau_Comps','MET_RefTau_Regions',
                                                    'MET_RefTau_em_Comps','MET_RefTau_em_Regions',
                                                    'MET_SoftJets_Comps','MET_SoftJets_Regions',
                                                    'MET_SoftJets_em_Comps','MET_SoftJets_em_Regions',
                                                    'MET_Topo',
                                                    'MET_TopoObj_Comps','MET_TopoObj_Regions',
                                                    'MET_Track_Comps','MET_Track_Regions',
                                                    'MET_Comps',
                                                    'MET_Track_Weights','MET_Cluster_Weights',
                                                    'MET_Regions','MET_Composition'],
                                           allowMissing=True ))


    # HadronicRecoil blocks

    if addHadronicRecoil:

        alg += ElectronD3PDObject(0,  sgkey = 'HR_selectedElectrons', prefix = 'hr_el_',allowMissing=True )
        alg += MuonD3PDObject( 0,     sgkey = 'HR_selectedMuons',     prefix = 'hr_mu_', allowMissing=True )

        # ... DR = 0.15
     
        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_15',                prefix = 'hr_roughRecoil_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_15',               prefix = 'hr_ueCorrection_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_15',              prefix = 'hr_corrRecoil_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_15',                     prefix = 'hr_MET_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
     
        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_Eflow_15',          prefix = 'hr_roughRecoil_Eflow_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_Eflow_15',         prefix = 'hr_ueCorrection_Eflow_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_Eflow_15',        prefix = 'hr_corrRecoil_Eflow_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_Eflow_15',               prefix = 'hr_MET_Eflow_15',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True ) 

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_15",          prefix = "hr_roughRecoil_track_15",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_15",         prefix = "hr_ueCorrection_track_15",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_15",        prefix = "hr_corrRecoil_track_15",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_15",               prefix = "hr_MET_track_15",
                                    exclude=['MET_Regions','MET_Comps'] ) 

        # ... DR = 0.20

        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_20',                prefix = 'hr_roughRecoil_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_20',               prefix = 'hr_ueCorrection_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_20',              prefix = 'hr_corrRecoil_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_20',                     prefix = 'hr_MET_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
     
        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_track_20',          prefix = 'hr_roughRecoil_track_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_track_20',         prefix = 'hr_ueCorrection_track_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_track_20',        prefix = 'hr_corrRecoil_track_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_track_20',               prefix = 'hr_MET_track_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )    

        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_clusNoTrack_20',    prefix = 'hr_roughRecoil_clusNoTrack_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_clusNoTrack_20',   prefix = 'hr_ueCorrection_clusNoTrack_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_clusNoTrack_20',  prefix = 'hr_corrRecoil_clusNoTrack_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_clusNoTrack_20',         prefix = 'hr_MET_clusNoTrack_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )  

        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_Eflow_20',          prefix = 'hr_roughRecoil_Eflow_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_Eflow_20',         prefix = 'hr_ueCorrection_Eflow_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_Eflow_20',        prefix = 'hr_corrRecoil_Eflow_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_Eflow_20',               prefix = 'hr_MET_Eflow_20',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )

        # ... DR = 0.25

        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_25',                prefix = 'hr_roughRecoil_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_25',               prefix = 'hr_ueCorrection_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_25',              prefix = 'hr_corrRecoil_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_25',                     prefix = 'hr_MET_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )

        alg += MissingETD3PDObject( 0, sgkey = 'RoughRecoil_Eflow_25',          prefix = 'hr_roughRecoil_Eflow_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'ueCorrection_Eflow_25',         prefix = 'hr_ueCorrection_Eflow_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'RefinedRecoil_Eflow_25',        prefix = 'hr_corrRecoil_Eflow_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
        alg += MissingETD3PDObject( 0, sgkey = 'MET_HR_Eflow_25',               prefix = 'hr_MET_Eflow_25',
                                    exclude=['MET_Regions','MET_Comps'],
                                    allowMissing=True )
     
        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_track_25",          prefix = "hr_roughRecoil_track_25",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_track_25",         prefix = "hr_ueCorrection_track_25",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_track_25",        prefix = "hr_corrRecoil_track_25",
                                    exclude=['MET_Regions','MET_Comps'] )
        alg += MissingETD3PDObject( 0, sgkey = "MET_HR_track_25",               prefix = "hr_MET_track_25",
                                    exclude=['MET_Regions','MET_Comps'] )


        # HR using truth leptons
        if rec.doTruth():
            
            ## ... DR = 0.15               
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_15",
                                        prefix = "hr_roughRecoil_trLep_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_15",
                                        prefix = "hr_ueCorrection_trLep_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_15",
                                        prefix = "hr_corrRecoil_trLep_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_15",
                                        prefix = "hr_MET_trLep_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_15",
                                        prefix = "hr_roughRecoil_trLep_Eflow_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_15",
                                        prefix = "hr_ueCorrection_trLep_Eflow_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_15",
                                        prefix = "hr_corrRecoil_trLep_Eflow_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_15",
                                        prefix = "hr_MET_trLep_Eflow_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_15",
                                        prefix = "hr_roughRecoil_trLep_track_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_15",
                                        prefix = "hr_ueCorrection_trLep_track_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_15",
                                        prefix = "hr_corrRecoil_trLep_track_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_15",
                                        prefix = "hr_MET_trLep_track_15",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            ## ... DR = 0.20
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_20",
                                        prefix = "hr_roughRecoil_trLep_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_20",
                                        prefix = "hr_ueCorrection_trLep_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_20",
                                        prefix = "hr_corrRecoil_trLep_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_20",
                                        prefix = "hr_MET_trLep_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_20",
                                        prefix = "hr_roughRecoil_trLep_Eflow_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_20",
                                        prefix = "hr_ueCorrection_trLep_Eflow_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_20",
                                        prefix = "hr_corrRecoil_trLep_Eflow_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_20",
                                        prefix = "hr_MET_trLep_Eflow_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_20",
                                        prefix = "hr_roughRecoil_trLep_track_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_20",
                                        prefix = "hr_ueCorrection_trLep_track_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_20",
                                        prefix = "hr_corrRecoil_trLep_track_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_20",
                                        prefix = "hr_MET_trLep_track_20",
                                        exclude=['MET_Regions','MET_Comps'] )
            
             ## ... DR = 0.25
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_25",
                                        prefix = "hr_roughRecoil_trLep_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_25",
                                        prefix = "hr_ueCorrection_trLep_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_25",
                                        prefix = "hr_corrRecoil_trLep_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_25",
                                        prefix = "hr_MET_trLep_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_Eflow_25",
                                        prefix = "hr_roughRecoil_trLep_Eflow_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_Eflow_25",
                                        prefix = "hr_ueCorrection_trLep_Eflow_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_25",
                                        prefix = "hr_corrRecoil_trLep_Eflow_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_Eflow_25",
                                        prefix = "hr_MET_trLep_Eflow_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            
            alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_TruthLep_track_25",
                                        prefix = "hr_roughRecoil_trLep_track_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_TruthLep_track_25",
                                        prefix = "hr_ueCorrection_trLep_track_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_25",
                                        prefix = "hr_corrRecoil_trLep_track_25",
                                        exclude=['MET_Regions','MET_Comps'] )
            alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_TruthLep_track_25",
                                        prefix = "hr_MET_trLep_track_25",
                                        exclude=['MET_Regions','MET_Comps'] )


    # Cluster block

    # ... all clusters, very low LOD

    if clusterContainer != 'None':
        alg += ClusterD3PDObject          (**_args ( 0, 'Clusters', kw,
                                                     sgkey = clusterContainer,
                                                     exclude='SamplingBasics',
                                                     allowMissing=True ))


    # Track block

    # ... good tracks only (nSCT>=6; no pt cut)

    if trackContainer != 'None':
        alg += TrackParticleD3PDObject (**_args ( 3, 'Tracks', kw,
                                                  sgkey  = trackContainer,
                                                  label  = 'trk',
                                                  prefix = 'trk_',
                                                  allowMissing=True ))

    
    # Primary vertex block - May be missing in single-beam data.

    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             prefix = 'vxp_',
                                             allowMissing=True ))
    
    # Truth

    if rec.doTruth():

        from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject
        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject
        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))

        from AthenaCommon.AppMgr import ToolSvc
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool

        # ... remove empty GenEvents

        smlightGenEvtFilterTool = D3PD__GenObjectsFilterTool( "smlightGenEvtFilterTool" )
        ToolSvc += smlightGenEvtFilterTool
        smlightGenEvtFilterTool.RemoveDummyEvents = True
        smlightGenEvtFilterTool.RemoveInTimePileUp = True
        smlightGenEvtFilterTool.Remove2BCPileUp = True
        smlightGenEvtFilterTool.Remove800nsPileUp = True
        smlightGenEvtFilterTool.RemoveCavernBkg = True
        smlightGenEvtFilterTool.RemoveEmptyEvents = True

        from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject
        alg += GenEventD3PDObject     (**_args ( 0, 'GenEvent', kw, filter = smlightGenEvtFilterTool ))

        

        # ... leptonic W/Z truth information

        smlightLepWZFilterTool = D3PD__GenObjectsFilterTool( "smlightLepWZFilterTool" )
        ToolSvc += smlightLepWZFilterTool

        smlightLepWZFilterTool.RemoveInTimePileUp = True
        smlightLepWZFilterTool.Remove2BCPileUp = True
        smlightLepWZFilterTool.Remove800nsPileUp = True
        smlightLepWZFilterTool.RemoveCavernBkg =  True
        smlightLepWZFilterTool.RemoveEmptyEvents = True
        smlightLepWZFilterTool.RemoveDummyEvents = True
        smlightLepWZFilterTool.RemoveUnrequestedParticles=True
        smlightLepWZFilterTool.KeepLeptonicWZBosons=True
        smlightLepWZFilterTool.KeepLeptonicWZBosonDecayChains=True
        smlightLepWZFilterTool.KeepLeptonicWZBosonsParents=True

        from TruthD3PDMaker.GenParticleD3PDObject import GenParticleD3PDObject
        alg += GenParticleD3PDObject( **_args(10, "lepwzgenparticle", kw, prefix='mclepwzpart_',
                                              filter = smlightLepWZFilterTool,
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

#        RedefineTriggerBits(alg, 'EF*')
#        alg += SMTrilepTriggerBitsD3PDObject      (**_args (10, 'SMTrilepTriggerBits', kw))

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
        
        # Bunch group info

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        
        TrigEgammaD3PDObjects (alg, 1)
        TrigMuonD3PDObjects( alg, 1) 
        
# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

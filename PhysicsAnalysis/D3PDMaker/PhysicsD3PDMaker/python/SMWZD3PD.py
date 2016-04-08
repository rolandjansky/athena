# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration



# $Id: SMWZD3PD.py 566871 2013-10-23 21:04:01Z ssnyder $
#
# @file PhysicsAnalysis/D3PDMaker/PhysicsD3PDMaker/python/SMWZD3PD.py
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

### EventShape variables  Bertrand
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
#from TrigMissingETD3PDMaker.TrigMETD3PDObject          import TrigMETD3PDObject
from TrigMissingETD3PDMaker.TrigMETD3PD                import TrigMETD3PDObjects
from PhysicsD3PDMaker.SMWZTriggerBitsD3PDObject        import SMWZTriggerBitsD3PDObject

from TrigBJetD3PDMaker.TrigBJetD3PD                    import TrigBJetD3PDObjects



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


def add_met1 (alg, cone, truthLep = False, add_pieces = []):
    exc = ['MET_Regions','MET_Comps']
    tl_key = 'TruthLep_' if truthLep else ''
    tl_pfx = 'trLep_' if truthLep else ''
    key0 = 'RefinedRecoil_' if truthLep else 'MET_HR_'

    for piece in ['', 'Eflow_', 'track_'] + add_pieces:
        key_suff = tl_key + piece + cone
        pfx_suff = tl_pfx + piece + cone

        alg += MissingETD3PDObject( 0, sgkey = "RoughRecoil_" + key_suff,
                                    prefix = "hr_roughRecoil_" + pfx_suff,
                                    exclude= exc)
        alg += MissingETD3PDObject( 0, sgkey = "ueCorrection_" + key_suff,
                                    prefix = "hr_ueCorrection_" + pfx_suff,
                                    exclude= exc)
        alg += MissingETD3PDObject( 0, sgkey = "RefinedRecoil_" + key_suff,
                                    prefix = "hr_corrRecoil_" + pfx_suff,
                                    exclude=exc )
        alg += MissingETD3PDObject( 0, sgkey = key0 + key_suff,
                                    prefix = "hr_MET_" + pfx_suff,
                                    exclude=exc )


    return
    
def add_met (alg):
    ### adding HSG5 MET
    from MissingET.METRefGetter_plup import *
    
    METRefAlg_HSG5 = make_METRefAlg(_suffix='_HSG5')
    METRefAlg_HSG5.sequence                    = AlgSequence( D3PDMakerFlags.PreD3PDAlgSeqName() )
    METRefAlg_HSG5.jet_JetInputCollectionKey   = "AntiKt4TopoEMJets"
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
    METRefAlg_HSG5.muon_container              = "MuidMuonCollection"
    METRefAlg_HSG5.muon_algorithm              = "Muid"
    METRefAlg_HSG5.muon_isolationAlg           = "dRJet"
    #    METRefAlg_HSG5.jet_ApplyJetJVF             = "Yes"
    #    METRefAlg_HSG5.plupSuppCorr                ='STVF'
    #    METRefAlg_HSG5.celloutCorrection           ='STVF'
    print METRefAlg_HSG5
    METRefAlg_HSG5()

    customMETs = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet',
                  'MET_CellOut', 'MET_Cryo', 'MET_SoftJets',
                  'MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                  'MET_CellOut_Eflow_STVF', 'MET_CellOut_Eflow_JetArea',
                  'MET_RefFinal_STVF' ]

    for custom in customMETs:
        alg += MissingETD3PDObject (level=0, sgkey = custom+'_HSG5',prefix='MET_HSG5_'+custom[4:],
                                    allowMissing=True)
    
    alg += MissingETCompositionD3PDObject ( level=4, sgkey = 'MET_RefComposition_HSG5', suffix='HSG5_', allowMissing =True,
                                            jetSGKey='AntiKt4TopoEMJets', jetPrefix='jet_AntiKt4TopoEMJet_MET_')

    # adding STVF weights
    #    alg += MissingETCompositionD3PDObject ( level=4,
    #                                        sgkey = 'MET_RefComposition_HSG5',
    #                                        suffix='STVF_',
    #                                        allowMissing=True,
    #                                        jetSGKey='AntiKt4TopoEMJets',
    #                                        jetPrefix='jet_AntiKt4TopoEMJets_MET_')
                                            # exclude = ['MET_Muid_Weights','MET_Muons_Weights']
                         
    # HadronicRecoil blocks

    alg += ElectronD3PDObject(0,  sgkey = "HR_selectedElectrons",       prefix = "hr_el_")
    alg += MuonD3PDObject( 0,     sgkey = "HR_selectedMuons",           prefix = "hr_mu_" )


    add_met1 (alg, '15')
    add_met1 (alg, '20', add_pieces = ['clusNoTrack_'])
    add_met1 (alg, '25')
   

    # HR using truth leptons
    if rec.doTruth():
        add_met1 (alg, '15', True)
        add_met1 (alg, '20', True)
        add_met1 (alg, '25', True)
    return


def SMWZD3PD (alg = None,
              file = 'smwz.root',
              tuplename = 'physics',
              streamname = 'd3pdstream',
              doSoftQCD = False,
              doThirdMuonChain = True,
              stdElectronContainer = D3PDMakerFlags.ElectronSGKey(),
              gsfElectronContainer = D3PDMakerFlags.GSFElectronSGKey(),
              muonContainer        = 'None', # can be 'Muons' for third muon chain
              stacoMuonContainer   = 'StacoMuonCollection',
              muidMuonContainer    = 'MuidMuonCollection',
              caloMuonContainer    = 'CaloMuonCollection',
              photonContainer      = D3PDMakerFlags.PhotonSGKey(),
              tauContainer         = D3PDMakerFlags.TauSGKey(),
              jetEM4Container      = 'AntiKt4TopoEMJets', #Commented by Bertrand
              jetEM6Container      = 'AntiKt6TopoEMJets',
              jetLC4Container      = 'AntiKt4LCTopoJets',
              jetLC6Container      = 'AntiKt6LCTopoJets',
              #jetEM4Container      = 'AntiKt4TopoEMJetsReTagged',
              #jetEM6Container      = 'AntiKt6TopoEMJetsReTagged',
              #jetLC4Container      = 'AntiKt4LCTopoJetsReTagged',
              #jetLC6Container      = 'AntiKt6LCTopoJetsReTagged',
              jetDetails           = 3, # for the moment only level 2 and 3 are defined
              METDetails           = 3, # for the moment only level 2 and 3 are defined
              **kw):

    if doThirdMuonChain and muonContainer == 'None':
        muonContainer = 'Muons'

    # define associations
    
    if not PhotonD3PDObject.allBlocknames().has_key('RecoveryMatch'):
        from PhotonD3PDMaker.PhotonRecoveryAssociation import PhotonRecoveryAssociation
        if (stdElectronContainer != 'None'):
            PhotonRecoveryAssoc = PhotonRecoveryAssociation(PhotonD3PDObject,
                                                            'ElectronContainer',
                                                            stdElectronContainer,
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


    # beam background stuff

    from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
    if not hasattr( preseq, "BeamBackgroundFiller" ):
        BeamBackgroundFiller=BeamBackgroundFiller()
        preseq += BeamBackgroundFiller



    # now configure the D3PD
    
    if not alg:
        alg = MSMgr.NewRootStream(StreamName = streamname, FileName = file, TreeName = tuplename)
        
    
    alg += EventInfoD3PDObject        (**_args (10, 'EventInfo', kw))
    alg += LArCollisionTimeD3PDObject (**_args (10, 'LArCollisionTime', kw))

    from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
    addBunchStructureMetadata (alg)

    from BackgroundD3PDMaker.BeamBackgroundD3PDObject import BeamBackgroundD3PDObject
    from BackgroundD3PDMaker.BackgroundWordD3PDObject import BackgroundWordD3PDObject
    alg += BackgroundWordD3PDObject (**_args(0, 'BkgWord', kw))
    alg += BeamBackgroundD3PDObject(1)

    
    # Electron/Photon blocks
    
    if (stdElectronContainer != 'None'):
        alg += ElectronD3PDObject         (**_args (10, 'Electron', kw, sgkey = stdElectronContainer))
        
    if (gsfElectronContainer != 'None') and testSGKey ('ElectronContainer', gsfElectronContainer):
        alg += GSFElectronD3PDObject  (**_args (10, 'GSFElectron', kw,
                                                sgkey = gsfElectronContainer,
                                                prefix = 'el_gsf_'))

    if (photonContainer != 'None'):
        alg += PhotonD3PDObject           (**_args (10, 'Photon', kw, sgkey = photonContainer))
            
    
    # Muon blocks

### Third muon chain variables!
    if (muonContainer != 'None'):
        alg += MuonD3PDObject             (**_args (10, 'Muon', kw,
                                                    sgkey=muonContainer, prefix='mu_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))
### Third muon chain variables!

    if (muidMuonContainer != 'None'):
        alg += MuonD3PDObject             (**_args (10, 'MuidMuon', kw,
                                                    sgkey=muidMuonContainer, prefix='mu_muid_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))

    if (stacoMuonContainer != 'None'):
        alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey=stacoMuonContainer, prefix='mu_staco_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex",
                                                               "L2CBInfoIndex", "L1InfoIndex"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info"],
                                                    allowMissing = True ))

    if (caloMuonContainer != 'None'):
        alg += MuonD3PDObject             (**_args (10, 'CaloMuon', kw,
                                                    sgkey=caloMuonContainer, prefix='mu_calo_',
                                                    include = ["MuonHitSummary", "Authors", "Likelihood",
                                                               "Isolation", "CaloEnergyLoss", "Quality"] ))
    
    # Tau block
    
    if (tauContainer != 'None'):
        alg += TauD3PDObject              (**_args ( 1, 'Tau', kw, sgkey = tauContainer))
    

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

  
      ## add MSV target to the jetd3pd object properties 
    if not kw.has_key ('AK4LCTopoJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target'):
        kw['AK4LCTopoJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target']= JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel()
    if not kw.has_key ('AK6LCTopoJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target'):
        kw['AK6LCTopoJet_'+JetTagD3PDKeys.MultiSVInfoPlusMSVVtxAssocBlockName()+'_target']= JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel()

    ## add track index for IPInfoPlus
    if not kw.has_key ('AK4LCTopoJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4LCTopoJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6LCTopoJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6LCTopoJet_'+JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName()+'_target']= "trk"
    ## add track index for SVInfoPlus
    if not kw.has_key ('AK4LCTopoJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4LCTopoJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6LCTopoJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6LCTopoJet_'+JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName()+'_target']= "trk"
    ## add track index for SV0InfoPlus
    if not kw.has_key ('AK4LCTopoJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target'):
        kw['AK4LCTopoJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target']= "trk"
    if not kw.has_key ('AK6LCTopoJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target'):
        kw['AK6LCTopoJet_'+JetTagD3PDKeys.SV0InfoPlusTrackAssocBlockName()+'_target']= "trk"

    if jetDetails == 3:
        jetInclude = JetTagD3PDFlags.StdInfoInclude()+[JetTagD3PDKeys.MultiSVInfoPlusBlockName()]
        jetInclude += [JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(), JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName()]
    else:
        jetInclude = [ JetTagD3PDKeys.BTagWeightsBlockName(), JetTagD3PDKeys.TruthInfoBlockName() ]


    if (jetEM4Container != 'None'):
        alg += JetD3PDObject              (**_args ( 4, 'AK4TopoEMJet', kw,
                                                     sgkey=jetEM4Container, prefix='jet_AntiKt4TopoEM_',
                                                     include = jetInclude+['BeamBackground','ConstituentScale','TracksMoments'],
                                                     JetVertexFraction_FromUD=False,
                                                     JetVertexFraction_FillFullJVF=False,
                                                     allowMissing = True))
    
    if (jetEM6Container != 'None'):
        alg += JetD3PDObject              (**_args ( 4, 'AK6TopoEMJet', kw,
                                                     sgkey=jetEM6Container, prefix='jet_AntiKt6TopoEM_',
                                                     include = jetInclude+['BeamBackground','ConstituentScale','TracksMoments'],
                                                     JetVertexFraction_FromUD=False,
                                                     JetVertexFraction_FillFullJVF=False,
                                                     allowMissing = True))

    if (jetLC4Container != 'None'):
        alg += JetD3PDObject              (**_args ( 4, 'AK4LCTopoJet', kw,
                                                     sgkey=jetLC4Container, prefix='jet_AntiKt4LCTopo_',
                                                     include = jetInclude+['BeamBackground','ConstituentScale','TracksMoments'],
                                                     JetVertexFraction_FromUD=False,
                                                     JetVertexFraction_FillFullJVF=False,
                                                     allowMissing = True))
    
    if (jetLC6Container != 'None'):
        alg += JetD3PDObject              (**_args ( 4, 'AK6LCTopoJet', kw,
                                                     sgkey=jetLC6Container, prefix='jet_AntiKt6LCTopo_',
                                                     include = jetInclude+['BeamBackground','ConstituentScale','TracksMoments'],
                                                     JetVertexFraction_FromUD=False,
                                                     JetVertexFraction_FillFullJVF=False,
                                                     allowMissing = True))

        
    ### add the MSV block
    MSVJetCollections = []
    if (jetEM4Container != 'None'):
        MSVJetCollections += [jetEM4Container]
    if (jetEM6Container != 'None'):
        MSVJetCollections += [jetEM6Container]
    if (jetLC4Container != 'None'):
        MSVJetCollections += [jetLC4Container]
    if (jetLC6Container != 'None'):
        MSVJetCollections += [jetLC6Container]
    ### link multisv to tracks
    if not kw.has_key ('msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target'):
        kw['msvvtxinfo_JetTag_JetMultiSVVtxAssoc_target']= "trk"

    from JetTagD3PDMaker.JetTagMSVVtxInJetD3PDObject import getJetTagMSVVtxInfoInJetD3PDObject
    alg += getJetTagMSVVtxInfoInJetD3PDObject(**_args(1,"msvvtxinfo", kw,
                                                      jetCollections=MSVJetCollections)
                                                )
 
    from TopInputsD3PDMaker.HforD3PDObject import HforD3PDObject
    alg += HforD3PDObject(**_args (0, 'HforInfo', kw))


    # MET blocks
    # a whole mess to remove x,y components separately for all flavours

    if METDetails >= 3:
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
                                                         'MET_Track_Weights','MET_Cluster_Weights'],
                                                allowMissing=True))
    elif METDetails > 0:
        alg += MissingETD3PDObject   (**_args (10, 'MissingET', kw,
                                           exclude=['MET_Base','MET_Base0','MET_Truth',
                                                    'MET_CellOut_Comps','MET_CellOut_Regions',
                                                    'MET_CellOut_em_Comps','MET_CellOut_em_Regions',
                                                    'MET_CellOut_Eflow_Comps','MET_CellOut_Eflow_Regions',
                                                    'MET_Calib',
                                                    'MET_CorrTopo',
                                                    'MET_Cryo',
                                                    'MET_CryoCone',
                                                    'MET_Final',
                                                    'MET_LocHadTopo_Comps',
                                                    'MET_LocHadTopoObj',
                                                    'MET_Muid',
                                                    'MET_Muid_Spectro',
                                                    'MET_Muid_Track',
                                                    'MET_MuonBoy_Regions',
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
                                                    'MET_Track_Weights','MET_Cluster_Weights',
                                                    'MET_Comps'
                                                    ],
                                           allowMissing=True ))


    if METDetails > 0:
        add_met (alg)


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
                                                         truthMapKey='TrackParticleTruthCollection',
                                                         SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
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
                                                        truthMapKey='TrackParticleTruthCollection',
                                                        SGKeyForTruth=D3PDMakerFlags.TrackSGKey(),
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


    ### EventShape variables  Bertrand
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

        
    # Truth

    if rec.doTruth():

        from TruthD3PDMaker.TruthParticleD3PDObject            import TruthParticleD3PDObject
        alg += TruthParticleD3PDObject(**_args ( 1, 'TruthParticle', kw))

        from MuonD3PDMaker.TruthMuonD3PDObject                 import TruthMuonD3PDObject
        alg += TruthMuonD3PDObject    (**_args ( 2, 'TruthMuon', kw))


        from AthenaCommon.AppMgr import ToolSvc
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool

        # ... remove empty GenEvents

        smwzGenEvtFilterTool = D3PD__GenObjectsFilterTool( "smwzGenEvtFilterTool" )
        ToolSvc += smwzGenEvtFilterTool
        smwzGenEvtFilterTool.RemoveDummyEvents = True
        smwzGenEvtFilterTool.RemoveInTimePileUp = True
        smwzGenEvtFilterTool.Remove2BCPileUp = True
        smwzGenEvtFilterTool.Remove800nsPileUp = True
        smwzGenEvtFilterTool.RemoveCavernBkg = True
        smwzGenEvtFilterTool.RemoveEmptyEvents = True

        from TruthD3PDMaker.GenEventD3PDObject                 import GenEventD3PDObject
        alg += GenEventD3PDObject     (**_args ( 0, 'GenEvent', kw, filter = smwzGenEvtFilterTool ))


        # ... TruthJets
        if not testSGKey ('JetCollection', 'AntiKt6TruthJets'):
            make_StandardJetGetter('AntiKt', 0.6, 'Truth')

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
        

        # ... heavy flavor truth information

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

        alg += SMWZTriggerBitsD3PDObject      (**_args (10, 'SMWZTriggerBits', kw))
        
        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(10, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
        
        # Bunch group info

        alg += BGCodeD3PDObject (**_args (2, 'BGCode', kw))

        # Egamma and Mu
        
        TrigEgammaD3PDObjects (alg, 1)
        TrigMuonD3PDObjects( alg, 1)

        # MET
        
        TrigMETD3PDObjects (alg, 2)

        # The BJet information:

        TrigBJetD3PDObjects(alg,10, False, False, True, True, True)

# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

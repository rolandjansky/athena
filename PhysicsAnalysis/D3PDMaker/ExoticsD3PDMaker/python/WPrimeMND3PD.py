# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: WPrimeMND3PD.py 498249 2012-04-27 21:53:57Z ntsirint$
#
# @file ExoticsD3PDMaker/python/WPrimeMND3PD.py
# @author Nikolaos Tsirintanis <Nikolaos.Tsirintanis@CERN.CH>
# @date July, 2012
# @brief Construct a D3PD for W prime (->munu) analysis


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
from ExoticsD3PDMaker.WPrimeMNTriggerBitsD3PDObject    import WPrimeMNTriggerBitsD3PDObject

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


def WPrimeMND3PD (alg = None,
              file = 'smwz.root',
              tuplename = 'physics',
              streamname = 'd3pdstream',
              doSoftQCD = False,
              doThirdMuonChain = True,
              **kw):
			  
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
    
    # Muon blocks

### Third muon chain variables!        
    if doThirdMuonChain:
        if cfgKeyStore.isInInput( "Analysis::MuonContainer", "Muons" ):
            alg += MuonD3PDObject             (**_args (10, 'Muon', kw,
                                                        sgkey='Muons', prefix='mu_',
                                                        include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex"],
                                                        exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info","SelectionFlagging",
	    													  "Likelihood","TrkMatchQuality","Quality","MuonTimingFillerTool","TrkParameters",
	    													  "TrkCovDiag","TrkCovOffDiag","TrkFitQuality","TrkInfoInDet","L2CBInfoIndex", "L1InfoIndex"],
                                                        allowMissing = True ))
### Third muon chain variables!        



    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" ):
        alg += MuonD3PDObject             (**_args (10, 'StacoMuon', kw,
                                                    sgkey='StacoMuonCollection', prefix='mu_staco_',
                                                    include = ["EFCBInfoIndex", "EFMGInfoIndex", "EFMEInfoIndex"],
                                                    exclude = ["EFCBInfo", "EFMGInfo", "EFMEInfo", "L2CBInfo", "L1Info","SelectionFlagging",
						    								  "Likelihood","TrkMatchQuality","Quality","MuonTimingFillerTool","TrkParameters",
						    								  "TrkCovDiag","TrkCovOffDiag","TrkFitQuality","TrkInfoInDet","L2CBInfoIndex", "L1InfoIndex"],
                                                    allowMissing = True ))

    # Jet blocks
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags

    alg += JetD3PDObject              (**_args ( 1, 'AK4TopoEMJet', kw,
                                                 sgkey='AntiKt4TopoEMJets', prefix='jet_AntiKt4TopoEM_',
                                                 include = [],
                                                 exclude = ['JESMoments','JVtx','OriginCorrection',
                                                            'El02Match','Mu02Match','L1Info','L2Info','EFInfo'],
                                                 allowMissing = True))
    

    # EventShape variables
    alg += EventShapeD3PDObject(**_args(0, 'rho', kw))

    

    alg += MissingETD3PDObject        (**_args (0, 'MissingET', kw,
                                                include=['MET_RefFinal_Comps','MET_CellOut_Comps','MET_REfFinal','MET_CellOut','MET_CellOut_Eflow',
                                                         'MET_LocHadTopo_Comps','MET_LocHadTopo',
                                                         'MET_LocHadTopoObj_Comps','MET_LocHadTopoObj',
                                                         'MET_MuonBoy_Comps','MET_MuonBoy',
                                                         'MET_MuonBoy_Track_Comps','MET_MuonBoy_Track',
                                                         'MET_RefMuon_Comps','MET_RefMuon','MET_RefMuon_em',
                                                         'MET_RefMuon_Track_em_Comps','MET_RefMuon_Track_Comps','MET_RefMuon_Track',
                                                         'MET_RefMuon_em_Comps','MET_SoftJets_Comps','MET_SoftJets','MET_RefMuon_em'
                                                         'MET_SoftJets_em_Comps','MET_Topo_Comps',
                                                         'MET_TopoObj_Comps','MET_Track_Comps','MET_Comps',
                                                         'MET_RefFinal_STVF','MET_RefJet_JVF', 'MET_RefJet_JVFCut',
                                                         'MET_CellOut_Eflow_STVF','MET_CellOut_Eflow_JetArea',
                                                         'MET_Truth_NonInt','MET_Truth_NonInt_Phi','MET_Truth_NonInt_Et','MET_Truth_NonInt_SumEt',
                                                         'MET_CellOut_Eflow_JetAreaRhoEta5JVF',
                                                         ],
                                                allowMissing=True))


    alg+=MissingETCompositionD3PDObject(**_args(4,"MissingETComposition",kw))
    

    
    # Primary vertex block - May be missing in single-beam data.

    alg += PrimaryVertexD3PDObject (**_args (1, 'PrimaryVertex', kw,
                                             allowMissing = True,
                                             sgkey = D3PDMakerFlags.VertexSGKey(),
                                             exclude = ["Error","DiagCovariance","Covariance","Purity","TrackAtVertex"],
                                             prefix = 'vxp_'))
        
    # Trigger

    if D3PDMakerFlags.DoTrigger():

        alg += WPrimeMNTriggerBitsD3PDObject      (**_args (10, 'WPrimeMNTriggerBits', kw))
        
        # Trigger Decision + metadata

        alg += TrigDecisionD3PDObject  (**_args(0, 'TrigDecision', kw))
        addTrigConfMetadata(alg)
		
        
        from TrigMuonD3PDMaker.TrigMuonEFInfoD3PDObject import TrigMuonEFInfoD3PDObject
        alg += TrigMuonEFInfoD3PDObject( **_args( -1, "TrigMuonEF", kw, include=["Decision"] ) )
        #Mu
       # TrigMuonD3PDObjects( alg, 1)

# Event metadata

    alg.MetadataTools += [LBMetadataConfig()]

    if D3PDMakerFlags.FilterCollCand():
        from CaloD3PDMaker.CollisionFilterAlg import CollisionFilterAlg
        alg.filterSeq += CollisionFilterAlg (tuplename + '_CollCandFilter')

    return alg

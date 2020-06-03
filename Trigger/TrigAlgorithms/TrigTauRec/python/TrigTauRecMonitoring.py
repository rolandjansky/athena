# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

errorcalolabels = 'NoROIDescr:NoCellCont:EmptyCellCont:NoClustCont:NoClustKey:EmptyClustCont:NoJetAttach:NoHLTtauAttach:NoHLTtauDetAttach:NoHLTtauXdetAttach'

errortracklabels = 'NoTrkCont:NoVtxCont'

class TrigTauRecOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigTauRecOnlineMonitoring"):
        super(TrigTauRecOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
        self.Histograms += [ defineHistogram('EF_nCand', type='TH1F', title=" Number of tau candidates;Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.) ]
        self.Histograms += [ defineHistogram('nRoI_EFTauCells', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.) ]
        self.Histograms += [ defineHistogram('nRoI_EFTauTracks', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.) ]

        self.Histograms += [ defineHistogram('EF_Et', type='TH1F', title=" EF Et had+em (at EM scale); Et had+em (at EM scale) [GeV]; nRoIs", xbins=100, xmin=0., xmax=500.) ]
        self.Histograms += [ defineHistogram('EF_EtFinal', type='TH1F', title=" EF Calibrated Et (tauJet->et()); EF Calibrated Et [GeV]; nRoIs", xbins=100, xmin=0., xmax=500.) ]        

        self.Histograms += [ defineHistogram('EF_EMRadius', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.) ]
        self.Histograms += [ defineHistogram('EF_HadRadius', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.) ]

        self.Histograms += [ defineHistogram('PhiL1', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=65, xmin=-3.1415936-0.098174/2., xmax=3.1415936+0.098174/2.) ]
        self.Histograms += [ defineHistogram('EtaL1', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=51, xmin=-2.55, xmax=2.55) ]       
        self.Histograms += [ defineHistogram('EtaL1, PhiL1', type='TH2F', title="L1 ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi ",
                                             xbins=51, xmin=-2.55, xmax=2.55,
                                             ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
        self.Histograms += [ defineHistogram('EtaEF, PhiEF', type='TH2F', title="EF ROI Eta vs Phi in TrigTauRecMerged FEX; #eta; #phi ",
                                             xbins=51, xmin=-2.55, xmax=2.55,
                                             ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
        self.Histograms += [ defineHistogram('EF_EtHad, EF_EtEm', type='TH2F', title="EF Et had vs Em in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                             xbins=30, xmin=0., xmax=150.,
                                             ybins=30, ymin=0., ymax=150.)]
        
        self.Histograms += [ defineHistogram('dPhiEFTau_RoI', type='TH1F', title="diff EF vs RoI phi ; Dphi; nRoIs", xbins=100, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('dEtaEFTau_RoI', type='TH1F', title="diff EF vs RoI eta ; Deta; nRoIs", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', type='TH2F', title="dEta vs dPhi in TrigTauRec FEX; Delta-eta; Delta-phi",
                                             xbins=40 , xmin=-0.2, xmax=0.2,
                                             ybins=40 , ymin=-0.2, ymax=0.2) ]

        self.Histograms += [ defineHistogram('EF_NTrk', type='TH1F', title=" Number of tracks;Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.) ]    
        self.Histograms += [ defineHistogram('EF_nWideTrk',        type='TH1F', title=" EF N Wide Tracks;N Wide Tracks; nRoIs"              , xbins=17, xmin=-2.0, xmax=15.) ]

        self.Histograms += [ defineHistogram('EF_EMFrac', type='TH1F', title=" EM Fraction;EM Fraction;nRoIs", xbins=70, xmin=-0.1, xmax=1.3) ]
        self.Histograms += [ defineHistogram('EF_IsoFrac', type='TH1F', title=" Isolation Fraction;Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2) ]
        self.Histograms += [ defineHistogram('EF_centFrac',        type='TH1F', title=" EF central Fraction;central Fraction; nRoIs"        , xbins=80, xmin=-0.4, xmax=1.2) ]

        self.Histograms += [ defineHistogram('EF_TrkAvgDist', type='TH1F', title=" Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EF_EtovPtLead', type='TH1F', title=" Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0) ]
        self.Histograms += [ defineHistogram('EF_ipSigLeadTrk',    type='TH1F', title=" EF IPsig Leading Track;iIPsig Leading Track; nRoIs" , xbins=100, xmin=-50., xmax=50) ]
        self.Histograms += [ defineHistogram('EF_trFlightPathSig', type='TH1F', title=" EF Flightpath sig Track;Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40) ]
        self.Histograms += [ defineHistogram('EF_massTrkSys',      type='TH1F', title=" EF Mass Trk Sys;Mass Trk Sys [GeV]; nRoIs"                , xbins=100, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('EF_dRmax',           type='TH1F', title=" EF dR max;dR max; nRoIs"                            , xbins=50, xmin=-0., xmax=0.25) ]
        self.Histograms += [ defineHistogram('EF_PSSFraction',        type='TH1F', title=" EF Presampler strip energy fraction;PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.) ]
        self.Histograms += [ defineHistogram('EF_EMPOverTrkSysP',     type='TH1F', title=" EF EMP over TrkSysP;EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0) ]
        self.Histograms += [ defineHistogram('EF_ChPiEMEOverCaloEME', type='TH1F', title=" EF EM energy of charged pions over calorimetric EM energy;ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.) ]
        self.Histograms += [ defineHistogram('EF_innerTrkAvgDist', type='TH1F', title=" EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5) ]
        self.Histograms += [ defineHistogram('EF_SumPtTrkFrac', type='TH1F', title=" EF Sum Pt Track Fraction; SumPtTrkFrac; nRoIs", xbins=40, xmin=-0.5, xmax=1.1) ]
 
        self.Histograms += [ defineHistogram('EF_ActualInteractions', type='TH1F', title=" Number of actual interaction per bunch crossing;ActualInteractions;nevents", xbins=80, xmin=0.0, xmax=80.) ]
        self.Histograms += [ defineHistogram('EF_AvgInteractions', type='TH1F', title=" Number of average interaction per bunch crossing;AvgInteractions;nevents", xbins=80, xmin=0.0, xmax=80.) ]

        self.Histograms += [ defineHistogram('EF_beamspot_x', type='TH1F', title=" Beamspot position;Beamspot x;nevents", xbins=50, xmin=-10.0, xmax=10.) ]
        self.Histograms += [ defineHistogram('EF_beamspot_y', type='TH1F', title=" Beamspot position;Beamspot y;nevents", xbins=50, xmin=-10.0, xmax=10.) ]
        self.Histograms += [ defineHistogram('EF_beamspot_z', type='TH1F', title=" Beamspot position;Beamspot z;nevents", xbins=100, xmin=-500.0, xmax=500.) ]


        self.Histograms += [ defineHistogram('EF_calo_errors', type='TH1F', title=" EF Calo Errors ;  ; nRoIs", xbins=10, xmin=-0.5, xmax=9.5 , labels=errorcalolabels) ]
        self.Histograms += [ defineHistogram('EF_track_errors', type='TH1F', title=" EF Track Errors ;  ; nRoIs", xbins=2, xmin=-0.5, xmax=2.5 , labels=errortracklabels) ]

        # RNN ID
        # scalar inputs
        self.Histograms += [ defineHistogram('EF_RNN_scalar_ptRatioEflowApprox', type='TH1F', title=" EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('EF_RNN_scalar_mEflowApprox', type='TH1F', title=" EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.) ]
        self.Histograms += [ defineHistogram('EF_RNN_scalar_pt_jetseed_log', type='TH1F', title=" EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.) ]
        # cluster inputs
        self.Histograms += [ defineHistogram('EF_RNN_Nclusters', type='TH1F', title=" EF RNN Nclusters; Number of clusters", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('EF_RNN_cluster_et_log', type='TH1F', title=" EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.) ]
        self.Histograms += [ defineHistogram('EF_RNN_cluster_dEta', type='TH1F', title=" EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('EF_RNN_cluster_dPhi', type='TH1F', title=" EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        # track inputs
        self.Histograms += [ defineHistogram('EF_RNN_Ntracks', type='TH1F', title=" EF RNN Ntracks; Number of tracks", xbins=12, xmin=0., xmax=12.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_pt_log', type='TH1F', title=" EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_dEta', type='TH1F', title=" EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_dPhi', type='TH1F', title=" EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_d0_abs_log', type='TH1F', title=" EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_z0sinThetaTJVA_abs_log', type='TH1F', title=" EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_nInnermostPixelHits', type='TH1F', title=" EF RNN track nInnermostPixelHits; nInnermostPixelHits", xbins=4, xmin=0., xmax=4.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_nPixelHits', type='TH1F', title=" EF RNN track nPixelHits; nPixelHits", xbins=11, xmin=0., xmax=11.) ]
        self.Histograms += [ defineHistogram('EF_RNN_track_nSCTHits', type='TH1F', title=" EF RNN track nSCTHits; nSCTHits", xbins=20, xmin=0., xmax=20.) ]
        # output
        self.Histograms += [ defineHistogram('EF_RNNJetScore', type='TH1F', title=" EF RNNJetScore; RNNJetScore", xbins=50, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EF_RNNJetScoreSigTrans', type='TH1F', title=" EF RNNJetScoreSigTrans; RNNJetScoreSigTrans", xbins=50, xmin=0., xmax=1.) ]



########## add validation specific histograms 
class TrigTauRecValidationMonitoring(TrigTauRecOnlineMonitoring):
    def __init__ (self, name="TrigTauRecValidationMonitoring"):
        super(TrigTauRecValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        





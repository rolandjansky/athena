# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def tauMonitoringCaloOnlyMVA():
   monTool = GenericMonitoringTool('MonTool')
  
   monTool.defineHistogram('track_errors', path='EXPERT', type='TH1F', title="CaloMVA Track Errors; nRoIs; nEntries", xbins=2, xmin=-0.5, xmax=1.5) 
   monTool.defineHistogram('calo_errors', path='EXPERT', type='TH1F', title="CaloMVA Calo Errors; nRoIs; nEntries", xbins=10, xmin=-0.5, xmax=9.5)
   monTool.defineHistogram('mEflowApprox', path="EXPERT", type='TH1F', title="CaloMVA RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.); nEntries", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT", type='TH1F', title="CaloMVA RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.); nEntries", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT', type='TH1F', title="CaloMVA RNN ptDetectorAxis; ptDetectorAxis; nEntries", xbins=50, xmin=1., xmax=1000.)
   monTool.defineHistogram('ptDetectorAxis_log', path='EXPERT', type='TH1F', title=" CaloMVA RNN ptDetectorAxis log; ptDetectorAxis_log; nEntries", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT', type='TH1F', title="CaloMVA RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed); nEntries", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber', path='EXPERT', type='TH1F', title="CaloMVA RNN cluster number; nRNN_Clusters; nEntries", xbins=15, xmin=0., xmax=15.)
   monTool.defineHistogram('cluster_et_log', path="EXPERT", type='TH1F', title="CaloMVA RNN cluster et_log; Log10(cluster E_{T}); nEntries", xbins=50, xmin=1., xmax=7.) 
   monTool.defineHistogram('cluster_dEta', path="EXPERT", type='TH1F', title="CaloMVA RNN cluster eta - L1 RoI eta; Deta(CaloMVA cluster, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi', path="EXPERT", type='TH1F', title="CaloMVA RNN cluster phi - L1 RoI phi; Dphi(CaloMVA cluster, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R', path="EXPERT",type='TH1F', title='cluster_SECOND_R_log10; cluster_SECOND_R_log10; nEntries',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA', path="EXPERT",type='TH1F', title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10; nEntries',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA', path="EXPERT",type='TH1F', title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10; nEntries',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber', path='EXPERT', type='TH1F', title=" CaloMVA RNN track number; nRNN_Tracks; nEntries", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="CaloMVA nRoI_Cells; nCells; nEntries", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('EtFinal', path='EXPERT', type='TH1F', title="Et Final; Et Final; nEntries", xbins=100, xmin=0, xmax=1000)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="CaloMVA RoI phi - L1 RoI phi; Dphi(CaloMVA RoI, L1 RoI); nEntries", xbins=100, xmin=-0.15, xmax=0.15)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="CaloMVA RoI eta - L1 RoI eta; Deta(CaloMVA RoI, L1 RoI); nEntries", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="Dphi(CaloMVA RoI, L1 tau) vs Deta(CaloMVA RoI, L1 RoI) in TrigTauRec FEX; Deta(CaloMVA RoI, L1 RoI); Dphi(CaloMVA RoI, L1 RoI)",
                                                           xbins=100 , xmin=-0.4, xmax=0.4,
                                                           ybins=100 , ymin=-0.15, ymax=0.15)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="CaloMVA EMRadius; EM radius; nEntries", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="CaloMVA HadRadius; Had radius; nEntries", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="CaloMVA Et EM vs Et Had in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction; EM Fraction; nEntries", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction; Isolation Fraction; nEntries", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="CaloMVA central Fraction; central Fraction; nEntries", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates; Number of tau candidates; nEntries", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI eta; L1 RoI eta; nEntries", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI phi; L1 RoI phi; nEntries", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 RoI phi vs eta in TrigTauRecMerged FEX; eta; phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="CaloMVA RoI eta; CaloMVA RoI eta; nEntries", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="CaloMVA RoI phi; CaloMVA RoI phi; nEntries", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="CaloMVA RoI phi vs CaloMVA RoI eta in TrigTauRecMerged FEX; eta; phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtFinal, EtaEF', path='EXPERT', type='TH2F', title="CaloMVA RoI eta vs Et Final; Et Final; CaloMVA RoI eta",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-2.6, ymax=2.6)
   monTool.defineHistogram('EtFinal, PhiEF', path='EXPERT', type='TH2F', title="CaloMVA RoI phi vs Et Final; Et Final; CaloMVA RoI phi",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('clustersMeanCenterLambda', path='EXPERT', type='TH1F', title='clustersMeanCenterLambda;clustersMeanCenterLambda;nEntries',xbins=40, xmin=0., xmax=2500.)
   monTool.defineHistogram('clustersMeanFirstEngDens', path='EXPERT', type='TH1F', title='clustersMeanFirstEngDens;clustersMeanFirstEngDens;nEntries',xbins=40, xmin=-8.7, xmax=-5.5)
   monTool.defineHistogram('clustersMeanSecondLambda', path='EXPERT', type='TH1F', title='clustersMeanSecondLambda;clustersMeanSecondLambda;nEntries',xbins=20, xmin=0., xmax=6e5)
   monTool.defineHistogram('clustersMeanPresamplerFrac', path='EXPERT', type='TH1F', title='clustersMeanPresamplerFrac;clustersMeanPresamplerFrac;nEntries',xbins=20, xmin=0., xmax=0.2)
   monTool.defineHistogram('clustersMeanEMProbability', path='EXPERT', type='TH1F', title='clustersMeanEMProbability;clustersMeanEMProbability;nEntries',xbins=20, xmin=0., xmax=1.)
   monTool.defineHistogram('etaDetectorAxis', path='EXPERT', type='TH1F', title="CaloMVA RNN etaDetectorAxis; etaDetectorAxis", xbins=100, xmin=-2.6, xmax=2.6)

   return monTool

def tauMonitoringPrecisionMVA():
   monTool = GenericMonitoringTool('MonTool')

   monTool.defineHistogram('track_errors', path='EXPERT', type='TH1F', title="Precision Track Errors; nRoIs; nEntries", xbins=2, xmin=-0.5, xmax=1.5)
   monTool.defineHistogram('calo_errors', path='EXPERT', type='TH1F', title="Precision Calo Errors; nRoIs; nEntries", xbins=10, xmin=-0.5, xmax=9.5)
   monTool.defineHistogram('mEflowApprox', path="EXPERT", type='TH1F', title="Precision RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.); nEntries", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT", type='TH1F', title="Precision RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.); nEntries", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT', type='TH1F', title="Precision RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed); nEntries", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT', type='TH1F', title="Precision RNN ptDetectorAxis; ptDetectorAxis; nEntries", xbins=50, xmin=1., xmax=1000.)
   monTool.defineHistogram('ptDetectorAxis_log', path='EXPERT', type='TH1F', title="Precision RNN ptDetectorAxis log; ptDetectorAxis_log; nEntries", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('RNN_clusternumber', path='EXPERT', type='TH1F', title="Precision RNN cluster number; nRNN_Clusters; nEntries", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log', path="EXPERT", type='TH1F', title="Precision RNN cluster et_log; Log10(cluster E_{T}); nEntries", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta', path="EXPERT", type='TH1F', title="Precision RNN cluster eta - L1 RoI eta; Deta(Precision cluster, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi', path="EXPERT", type='TH1F', title="Precision RNN cluster phi - L1 RoI phi; Dphi(Precision cluster, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R', path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10; nEntries',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA', path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10; nEntries',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA', path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10; nEntries',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber', path='EXPERT', type='TH1F', title="Precision RNN track number; nRNN_Tracks; nEntries", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log', path='EXPERT', type='TH1F', title="Precision RNN track pt_log; Log10(track p_{T}); nEntries", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta', path='EXPERT', type='TH1F', title="Precision RNN track eta - L1 RoI eta; Deta(Precision track, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi', path='EXPERT', type='TH1F', title="Precision RNN track phi - L1 RoI phi; Dphi(Precision track, L1 RoI); nEntries", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log', path='EXPERT',type='TH1F', title="Precision RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6); nEntries", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log', path='EXPERT', type='TH1F', title="Precision RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6); nEntries", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors', path='EXPERT', type='TH1F', title="Precision RNN track nPixelHitsPlusDeadSensors; nPixelHits; nEntries", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors', path='EXPERT', type='TH1F', title="Precision RNN track nSCTHitsPlusDeadSensors; nSCTHits; nEntries", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="Precision nRoI_Cells; nCells; nEntries", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('nRoI_EFTauTracks', path='EXPERT', type='TH1F', title="Precision nRoI_Tracks; NTracks; nEntries", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('EtFinal', path='EXPERT', type='TH1F', title="Et Final; Et Final; nEntries", xbins=100, xmin=0, xmax=1000)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="Precision RoI phi - L1 RoI phi; Dphi(Precision RoI, L1 RoI); nEntries", xbins=100, xmin=-0.15, xmax=0.15)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="Precision RoI eta - L1 RoI eta; Deta(Precision RoI, L1 RoI); nEntries", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="Dphi(Precision RoI, L1 RoI) vs Deta(Precision RoI, L1 RoI) in TrigTauRec FEX; Deta(Precision RoI, L1 RoI); Dphi(Precision RoI, L1 RoI)",
                                                           xbins=100 , xmin=-0.4, xmax=0.4,
                                                           ybins=100 , ymin=-0.15, ymax=0.15)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="Precision EMRadius; EM radius; nEntries", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="Precision HadRadius; Had radius; nEntries", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="Precision Et EM vs Et Had in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction; EM Fraction; nEntries", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction; Isolation Fraction; nEntries", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="Precision central Fraction; central Fraction; nEntries", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nWideTrk', path='EXPERT', type='TH1F', title="Precision  nWide_Tracks; nWide_Tracks; nEntries", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('ipSigLeadTrk', path='EXPERT', type='TH1F', title="Precision IPsig Leading Track; IPsig Leading Track; nEntries", xbins=100, xmin=-50., xmax=50)
   monTool.defineHistogram('trFlightPathSig', path='EXPERT', type='TH1F', title="Precision Flightpath sig Track; Flightpath sig Track; nEntries", xbins=100, xmin=-20., xmax=40)
   monTool.defineHistogram('massTrkSys', path='EXPERT', type='TH1F', title="Precision Mass Trk Sys; Mass Trk Sys [GeV]; nEntries", xbins=100, xmin=0., xmax=50.)
   monTool.defineHistogram('dRmax', path='EXPERT', type='TH1F', title="Precision dR max; dR max; nEntries", xbins=50, xmin=-0., xmax=0.25)
   monTool.defineHistogram('NTrk', path='EXPERT', type='TH1F', title="Number of tracks; Number of tracks; nEntries", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('TrkAvgDist', path='EXPERT', type='TH1F', title="Track Average Distance; TrkAvgDist; nEntries", xbins=41, xmin=-0.01, xmax=0.4)
   monTool.defineHistogram('EtovPtLead', path='EXPERT', type='TH1F', title="Et over lead track Pt; EtovPtLead; nEntries", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('PSSFraction', path='EXPERT', type='TH1F', title="Precision Presampler strip energy fraction; PSS energy fraction; nEntries", xbins=50, xmin=-0.5, xmax=1.)
   monTool.defineHistogram('EMPOverTrkSysP', path='EXPERT', type='TH1F', title="Precision EMP over TrkSysP; EMP over TrkSysP; nEntries", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('ChPiEMEOverCaloEME', path='EXPERT', type='TH1F', title="Precision EM energy of charged pions over calorimetric EM energy; ChPiEME over CaloEME; nEntries", xbins=40, xmin=-20., xmax=20.)
   monTool.defineHistogram('innerTrkAvgDist', path='EXPERT', type='TH1F', title="Precision inner track average distance; innerTrkAvgDist; nEntries", xbins=40, xmin=-0.05, xmax=0.5)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates; Number of tau candidates; nEntries", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI eta; L1 RoI eta; nEntries", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI phi; L1 RoI phi; nEntries", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 RoI phi vs L1 RoI eta in TrigTauRecMerged FEX; eta; phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="Precision RoI eta; Precision RoI eta; nEntries", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="Precision RoI phi; Precision RoI phi; nEntries", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="Precision RoI phi vs Precision RoI eta in TrigTauRecMerged FEX; eta; phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('vertex_x', path='EXPERT', type='TH1F', title="Precision RoI Vertex x; Vertex x; nEntries", xbins=100, xmin=-1, xmax=1)
   monTool.defineHistogram('vertex_y', path='EXPERT', type='TH1F', title="Precision RoI Vertex y; Vertex y; nEntries", xbins=100, xmin=-2, xmax=0)
   monTool.defineHistogram('vertex_z', path='EXPERT', type='TH1F', title="Precision RoI Vertex z; Vertex z; nEntries", xbins=120, xmin=-120, xmax=120)
   monTool.defineHistogram('RNNJetScore_0p', path='EXPERT', type='TH1F', title="Precision RNN score for 0 prong taus; RNN score; nEntries", xbins=40, xmin=0, xmax=1) 
   monTool.defineHistogram('RNNJetScoreSigTrans_0p', path='EXPERT', type='TH1F', title="Precision RNN score for 0 prong taus; RNN score sig trans; nEntries", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScore_1p', path='EXPERT', type='TH1F', title="Precision RNN score for 1 prong taus; RNN score; nEntries", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScoreSigTrans_1p', path='EXPERT', type='TH1F', title="Precision RNN score for 1 prong taus; RNN score sig trans; nEntries", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScore_mp', path='EXPERT', type='TH1F', title="Precision RNN score for multi prong taus; RNN score; nEntries", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScoreSigTrans_mp', path='EXPERT', type='TH1F', title="Precision RNN score for multi prong taus; RNN score sig trans; nEntries", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('EtFinal, EtaEF', path='EXPERT', type='TH2F', title="Precision RoI eta vs Et Final; Et Final; Precision RoI eta",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-2.6, ymax=2.6)
   monTool.defineHistogram('EtFinal, PhiEF', path='EXPERT', type='TH2F', title="Precision RoI phi vs Et Final; Et Final; Precision RoI phi",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('clustersMeanCenterLambda', path='EXPERT', type='TH1F', title='clustersMeanCenterLambda;clustersMeanCenterLambda;nEntries',xbins=40, xmin=0., xmax=2500.)
   monTool.defineHistogram('clustersMeanFirstEngDens', path='EXPERT', type='TH1F', title='clustersMeanFirstEngDens;clustersMeanFirstEngDens;nEntries',xbins=40, xmin=-8.7, xmax=-5.5)
   monTool.defineHistogram('clustersMeanSecondLambda', path='EXPERT', type='TH1F', title='clustersMeanSecondLambda;clustersMeanSecondLambda;nEntries',xbins=20, xmin=0., xmax=6e5)
   monTool.defineHistogram('clustersMeanPresamplerFrac', path='EXPERT', type='TH1F', title='clustersMeanPresamplerFrac;clustersMeanPresamplerFrac;nEntries',xbins=20, xmin=0., xmax=0.2)
   monTool.defineHistogram('clustersMeanEMProbability', path='EXPERT', type='TH1F', title='clustersMeanEMProbability;clustersMeanEMProbability;nEntries',xbins=20, xmin=0., xmax=1.)
   monTool.defineHistogram('etaDetectorAxis', path='EXPERT', type='TH1F', title="Precision RNN etaDetectorAxis; etaDetectorAxis", xbins=100, xmin=-2.6, xmax=2.6)

   return monTool

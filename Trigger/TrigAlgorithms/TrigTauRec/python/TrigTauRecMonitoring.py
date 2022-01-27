# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

def tauMonitoringCaloOnlyMVA():
   monTool = GenericMonitoringTool('MonTool')
  
   monTool.defineHistogram('track_errors', path='EXPERT', type='TH1F', title="EF Track Errors; nRoIs", xbins=2, xmin=-0.5, xmax=1.5) 
   monTool.defineHistogram('calo_errors', path='EXPERT', type='TH1F', title="EF Calo Errors; nRoIs", xbins=10, xmin=-0.5, xmax=9.5)
   monTool.defineHistogram('mEflowApprox', path="EXPERT", type='TH1F', title="EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT", type='TH1F', title="EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT', type='TH1F', title="EF RNN ptDetectorAxis; ptDetectorAxis", xbins=50, xmin=1., xmax=1000.)
   monTool.defineHistogram('ptDetectorAxis_log', path='EXPERT', type='TH1F', title=" EF RNN ptDetectorAxis log; ptDetectorAxis_log", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT', type='TH1F', title="EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('RNN_clusternumber', path='EXPERT', type='TH1F', title="EF RNN cluster number; N RNNClusters", xbins=15, xmin=0., xmax=15.)
   monTool.defineHistogram('cluster_et_log', path="EXPERT", type='TH1F', title="EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.) 
   monTool.defineHistogram('cluster_dEta', path="EXPERT", type='TH1F', title="EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi', path="EXPERT", type='TH1F', title="EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R', path="EXPERT",type='TH1F', title='cluster_SECOND_R_log10; cluster_SECOND_R_log10; Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA', path="EXPERT",type='TH1F', title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10; Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA', path="EXPERT",type='TH1F', title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10; Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber', path='EXPERT', type='TH1F', title=" EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('EtFinal', path='EXPERT', type='TH1F', title="Et Final; Et Final; nRoIs", xbins=100, xmin=0, xmax=1000)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="RoI vs diff EF phi; Dphi; nRoIs", xbins=100, xmin=-0.15, xmax=0.15)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="RoI vs diff EF eta; Deta; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dPhi vs dEta in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=100 , xmin=-0.4, xmax=0.4,
                                                           ybins=100 , ymin=-0.15, ymax=0.15)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et Em vs Et had in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction; EM Fraction; nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction; Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction; central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates; Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Phi vs Eta in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF RoI Eta; EF RoI Eta; nRoIs", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF RoI Phi; EF RoI Phi; nRoIs", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF RoI Phi vs Eta in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtFinal, EtaEF', path='EXPERT', type='TH2F', title="Eta EF vs Et Final; Et Final; Eta EF",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-2.6, ymax=2.6)
   monTool.defineHistogram('EtFinal, PhiEF', path='EXPERT', type='TH2F', title="Phi EF vs Et Final; Et Final; Phi EF",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('clustersMeanCenterLambda', path='EXPERT', type='TH1F', title='clustersMeanCenterLambda;clustersMeanCenterLambda;nevents',xbins=40, xmin=0., xmax=2500.)
   monTool.defineHistogram('clustersMeanFirstEngDens', path='EXPERT', type='TH1F', title='clustersMeanFirstEngDens;clustersMeanFirstEngDens;nevents',xbins=40, xmin=-8.7, xmax=-5.5)
   monTool.defineHistogram('clustersMeanSecondLambda', path='EXPERT', type='TH1F', title='clustersMeanSecondLambda;clustersMeanSecondLambda;nevents',xbins=20, xmin=0., xmax=6e5)
   monTool.defineHistogram('clustersMeanPresamplerFrac', path='EXPERT', type='TH1F', title='clustersMeanPresamplerFrac;clustersMeanPresamplerFrac;nevents',xbins=20, xmin=0., xmax=0.2)
   monTool.defineHistogram('clustersMeanEMProbability', path='EXPERT', type='TH1F', title='clustersMeanEMProbability;clustersMeanEMProbability;nevents',xbins=20, xmin=0., xmax=1.)
   monTool.defineHistogram('etaDetectorAxis', path='EXPERT', type='TH1F', title="EF RNN etaDetectorAxis; etaDetectorAxis", xbins=100, xmin=-2.6, xmax=2.6)
   
   return monTool

def tauMonitoringPrecisionMVA():
   monTool = GenericMonitoringTool('MonTool')

   monTool.defineHistogram('track_errors', path='EXPERT', type='TH1F', title="EF Track Errors; nRoIs", xbins=2, xmin=-0.5, xmax=1.5)
   monTool.defineHistogram('calo_errors', path='EXPERT', type='TH1F', title="EF Calo Errors; nRoIs", xbins=10, xmin=-0.5, xmax=9.5)
   monTool.defineHistogram('mEflowApprox', path="EXPERT", type='TH1F', title="EF RNN scalar mEflowApprox; Log10(max(mEflowApprox 140.)", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('ptRatioEflowApprox', path="EXPERT", type='TH1F', title="EF RNN scalar ptRatioEflowApprox; min(ptRatioEflowApprox 4.)", xbins=50, xmin=0., xmax=4.)
   monTool.defineHistogram('pt_jetseed_log', path='EXPERT', type='TH1F', title="EF RNN scalar pt_jetseed_log; Log10(tau.ptJetSeed)", xbins=50, xmin=3.5, xmax=7.)
   monTool.defineHistogram('ptDetectorAxis', path='EXPERT', type='TH1F', title="EF RNN ptDetectorAxis; ptDetectorAxis", xbins=50, xmin=1., xmax=1000.)
   monTool.defineHistogram('ptDetectorAxis_log', path='EXPERT', type='TH1F', title="EF RNN ptDetectorAxis log; ptDetectorAxis_log", xbins=50, xmin=0., xmax=5.)
   monTool.defineHistogram('RNN_clusternumber', path='EXPERT', type='TH1F', title="EF RNN cluster number; N RNNClusters", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('cluster_et_log', path="EXPERT", type='TH1F', title="EF RNN cluster et_log; Log10(cluster E_{T})", xbins=50, xmin=1., xmax=7.)
   monTool.defineHistogram('cluster_dEta', path="EXPERT", type='TH1F', title="EF RNN cluster dEta; #Delta#eta(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_dPhi', path="EXPERT", type='TH1F', title="EF RNN cluster dPhi; #Delta#phi(cluster tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('cluster_log_SECOND_R', path="EXPERT",type='TH1F',title='cluster_SECOND_R_log10; cluster_SECOND_R_log10; Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_SECOND_LAMBDA', path="EXPERT",type='TH1F',title='cluster_SECOND_LAMBDA_log10; cluster_SECOND_LAMBDA_log10; Events',xbins=50,xmin=-3,xmax=7)
   monTool.defineHistogram('cluster_CENTER_LAMBDA', path="EXPERT",type='TH1F',title='cluster_CENTER_LAMBDA_log10; cluster_CENTER_LAMBDA_log10; Events',xbins=50,xmin=-2,xmax=5)
   monTool.defineHistogram('RNN_tracknumber', path='EXPERT', type='TH1F', title="EF RNN track number; N RNNTracks", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('track_pt_log', path='EXPERT', type='TH1F', title="EF RNN track pt_log; Log10(track p_{T})", xbins=50, xmin=2.7, xmax=7.)
   monTool.defineHistogram('track_dEta', path='EXPERT', type='TH1F', title="EF RNN track dEta; #Delta#eta(track tau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_dPhi', path='EXPERT', type='TH1F', title="EF RNN track dPhi; #Delta#phi(track xtau)", xbins=50, xmin=-0.5, xmax=0.5)
   monTool.defineHistogram('track_d0_abs_log', path='EXPERT',type='TH1F', title="EF RNN track d0_abs_log; Log10(Abs(track.d0()) + 1e-6)", xbins=50, xmin=-6.1, xmax=2.)
   monTool.defineHistogram('track_z0sinThetaTJVA_abs_log', path='EXPERT', type='TH1F', title="EF RNN track z0sinThetaTJVA_abs_log; Log10(Abs(track.z0sinThetaTJVA(tau)) + 1e-6)", xbins=50, xmin=-6.1, xmax=4.)
   monTool.defineHistogram('track_nPixelHitsPlusDeadSensors', path='EXPERT', type='TH1F', title="EF RNN track nPixelHitsPlusDeadSensors; nPixelHits", xbins=11, xmin=0., xmax=11.)
   monTool.defineHistogram('track_nSCTHitsPlusDeadSensors', path='EXPERT', type='TH1F', title="EF RNN track nSCTHitsPlusDeadSensors; nSCTHits", xbins=20, xmin=0., xmax=20.)
   monTool.defineHistogram('nRoI_EFTauCells', path='EXPERT', type='TH1F', title="EF N RoI cells; N Cells; nRoIs", xbins=100, xmin=0., xmax=6000.)
   monTool.defineHistogram('nRoI_EFTauTracks', path='EXPERT', type='TH1F', title="EF N RoI Tracks; N Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('EtFinal', path='EXPERT', type='TH1F', title="Et Final; Et Final; nRoIs", xbins=100, xmin=0, xmax=1000)
   monTool.defineHistogram('dPhiEFTau_RoI', path='EXPERT', type='TH1F', title="RoI vs diff EF phi; Dphi; nRoIs", xbins=100, xmin=-0.15, xmax=0.15)
   monTool.defineHistogram('dEtaEFTau_RoI', path='EXPERT', type='TH1F', title="RoI vs diff EF eta; Deta; nRoIs", xbins=100, xmin=-0.4, xmax=0.4)
   monTool.defineHistogram('dEtaEFTau_RoI, dPhiEFTau_RoI', path='EXPERT', type='TH2F', title="dPhi vs dEta in TrigTauRec FEX; Delta-eta; Delta-phi",
                                                           xbins=100 , xmin=-0.4, xmax=0.4,
                                                           ybins=100 , ymin=-0.15, ymax=0.15)
   monTool.defineHistogram('EMRadius', path='EXPERT', type='TH1F', title="EF EMRadius; EM radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('HadRadius', path='EXPERT', type='TH1F', title="EF HadRadius; Had radius; nRoIs", xbins=50, xmin=-0.1, xmax=1.)
   monTool.defineHistogram('EtHad, EtEm', path='EXPERT', type='TH2F', title="EF Et Em vs Et had in TrigTauRecMerged FEX; Et Had (at EM scale) [GeV]; Et EM (at EM scale) [GeV]",
                                                xbins=30, xmin=0., xmax=150.,
                                                ybins=30, ymin=0., ymax=150.)
   monTool.defineHistogram('EMFrac', path='EXPERT', type='TH1F', title="EM Fraction; EM Fraction; nRoIs", xbins=70, xmin=-0.1, xmax=1.3)
   monTool.defineHistogram('IsoFrac', path='EXPERT', type='TH1F', title="Isolation Fraction; Isolation Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('centFrac', path='EXPERT', type='TH1F', title="EF central Fraction; central Fraction; nRoIs", xbins=80, xmin=-0.4, xmax=1.2)
   monTool.defineHistogram('nWideTrk', path='EXPERT', type='TH1F', title="EF N Wide Tracks; N Wide Tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('ipSigLeadTrk', path='EXPERT', type='TH1F', title="EF IPsig Leading Track; iIPsig Leading Track; nRoIs", xbins=100, xmin=-50., xmax=50)
   monTool.defineHistogram('trFlightPathSig', path='EXPERT', type='TH1F', title="EF Flightpath sig Track; Flightpath sig Track; nRoIs", xbins=100, xmin=-20., xmax=40)
   monTool.defineHistogram('massTrkSys', path='EXPERT', type='TH1F', title="EF Mass Trk Sys; Mass Trk Sys [GeV]; nRoIs", xbins=100, xmin=0., xmax=50.)
   monTool.defineHistogram('dRmax', path='EXPERT', type='TH1F', title="EF dR max; dR max; nRoIs", xbins=50, xmin=-0., xmax=0.25)
   monTool.defineHistogram('NTrk', path='EXPERT', type='TH1F', title="Number of tracks; Number of tracks; nRoIs", xbins=17, xmin=-2.0, xmax=15.)
   monTool.defineHistogram('TrkAvgDist', path='EXPERT', type='TH1F', title="Track Average Distance; TrkAvgDist; nRoIs", xbins=41, xmin=-0.01, xmax=0.4)
   monTool.defineHistogram('EtovPtLead', path='EXPERT', type='TH1F', title="Et over lead track Pt; EtovPtLead; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('PSSFraction', path='EXPERT', type='TH1F', title="EF Presampler strip energy fraction; PSS energy fraction; nRoIs", xbins=50, xmin=-0.5, xmax=1.)
   monTool.defineHistogram('EMPOverTrkSysP', path='EXPERT', type='TH1F', title="EF EMP over TrkSysP; EMP over TrkSysP; nRoIs", xbins=41, xmin=-0.5, xmax=20.0)
   monTool.defineHistogram('ChPiEMEOverCaloEME', path='EXPERT', type='TH1F', title="EF EM energy of charged pions over calorimetric EM energy; ChPiEME over CaloEME; nRoIs", xbins=40, xmin=-20., xmax=20.)
   monTool.defineHistogram('innerTrkAvgDist', path='EXPERT', type='TH1F', title="EF inner track average distance; innerTrkAvgDist; nRoIs", xbins=40, xmin=-0.05, xmax=0.5)
   monTool.defineHistogram('nCand', path='EXPERT', type='TH1F', title="Number of tau candidates; Number of tau candidates; nevents", xbins=10, xmin=-1.0, xmax=9.)
   monTool.defineHistogram('EtaL1', path='EXPERT', type='TH1F', title="L1 RoI Eta; L1 RoI Eta; nRoIs", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiL1', path='EXPERT', type='TH1F', title="L1 RoI Phi; L1 RoI Phi; nRoIs", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaL1, PhiL1', path='EXPERT', type='TH2F', title="L1 ROI Phi vs Eta in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('EtaEF', path='EXPERT', type='TH1F', title="EF RoI Eta; EF RoI Eta; nRoIs", xbins=100, xmin=-2.6, xmax=2.6)
   monTool.defineHistogram('PhiEF', path='EXPERT', type='TH1F', title="EF RoI Phi; EF_RoI Phi; nRoIs", xbins=100, xmin=-3.2, xmax=3.2)
   monTool.defineHistogram('EtaEF, PhiEF', path='EXPERT', type='TH2F', title="EF RoI Phi vs Eta in TrigTauRecMerged FEX; #eta; #phi",
                                           xbins=100, xmin=-2.6, xmax=2.6,
                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('vertex_x', path='EXPERT', type='TH1F', title="EF RoI Vertex x; Vertex x; nRoIS", xbins=100, xmin=-1, xmax=1)
   monTool.defineHistogram('vertex_y', path='EXPERT', type='TH1F', title="EF RoI Vertex y; Vertex y; nRoIS", xbins=100, xmin=-2, xmax=0)
   monTool.defineHistogram('vertex_z', path='EXPERT', type='TH1F', title="EF RoI Vertex z; Vertex z; nRoIS", xbins=120, xmin=-120, xmax=120)
   monTool.defineHistogram('RNNJetScore_0p', path='EXPERT', type='TH1F', title="EF RNN score for 0 prong taus; RNN score; nTaus", xbins=40, xmin=0, xmax=1) 
   monTool.defineHistogram('RNNJetScoreSigTrans_0p', path='EXPERT', type='TH1F', title="EF RNN score for 0 prong taus; RNN score sig trans; nTaus", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScore_1p', path='EXPERT', type='TH1F', title="EF RNN score for 1 prong taus; RNN score; nTaus", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScoreSigTrans_1p', path='EXPERT', type='TH1F', title="EF RNN score for 1 prong taus; RNN score sig trans; nTaus", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScore_mp', path='EXPERT', type='TH1F', title="EF RNN score for multi prong taus; RNN score; nTaus", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('RNNJetScoreSigTrans_mp', path='EXPERT', type='TH1F', title="EF RNN score for multi prong taus; RNN score sig trans; nTaus", xbins=40, xmin=0, xmax=1)
   monTool.defineHistogram('EtFinal, EtaEF', path='EXPERT', type='TH2F', title="Eta EF vs Et Final; Et Final; Eta EF",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-2.6, ymax=2.6)
   monTool.defineHistogram('EtFinal, PhiEF', path='EXPERT', type='TH2F', title="Phi Ef vs Et Final; Et Final; Phi EF",
                                                           xbins=100 , xmin=0, xmax=1000,
                                                           ybins=100, ymin=-3.2, ymax=3.2)
   monTool.defineHistogram('clustersMeanCenterLambda', path='EXPERT', type='TH1F', title='clustersMeanCenterLambda;clustersMeanCenterLambda;nevents',xbins=40, xmin=0., xmax=2500.)
   monTool.defineHistogram('clustersMeanFirstEngDens', path='EXPERT', type='TH1F', title='clustersMeanFirstEngDens;clustersMeanFirstEngDens;nevents',xbins=40, xmin=-8.7, xmax=-5.5)
   monTool.defineHistogram('clustersMeanSecondLambda', path='EXPERT', type='TH1F', title='clustersMeanSecondLambda;clustersMeanSecondLambda;nevents',xbins=20, xmin=0., xmax=6e5)
   monTool.defineHistogram('clustersMeanPresamplerFrac', path='EXPERT', type='TH1F', title='clustersMeanPresamplerFrac;clustersMeanPresamplerFrac;nevents',xbins=20, xmin=0., xmax=0.2)
   monTool.defineHistogram('clustersMeanEMProbability', path='EXPERT', type='TH1F', title='clustersMeanEMProbability;clustersMeanEMProbability;nevents',xbins=20, xmin=0., xmax=1.)
   monTool.defineHistogram('etaDetectorAxis', path='EXPERT', type='TH1F', title="EF RNN etaDetectorAxis; etaDetectorAxis", xbins=100, xmin=-2.6, xmax=2.6)

   return monTool

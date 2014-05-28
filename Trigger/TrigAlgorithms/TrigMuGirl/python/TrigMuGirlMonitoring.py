# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class TrigMuGirlValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuGirlValidationMonitoring"):
        super(TrigMuGirlValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('MuonsPt', type='TH1F', title="TrigMuGirl muons pT", xbins=1000, xmin=0, xmax=200000) ]
        self.Histograms += [ defineHistogram('MuonsCotTh', type='TH1F', title="TrigMuGirl Muons CotTh", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsPhi', type='TH1F', title="TrigMuGirl Muons phi", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('MuonsMdtHits', type='TH1F', title="TrigMuGirl Muons MDT Hits", xbins=100, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('MuonsRpcPhiHits', type='TH1F', title="TrigMuGirl Muons RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsRpcEtaHits', type='TH1F', title="TrigMuGirl Muons RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsCscPhiHits', type='TH1F', title="TrigMuGirl Muons CSC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsCscEtaHits', type='TH1F', title="TrigMuGirl Muons CSC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsMdtHitsInSeg', type='TH1F', title="TrigMuGirl Muons MDT Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsRpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsMdtSegs', type='TH1F', title="TrigMuGirl Muons MDT Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsRpcSegs', type='TH1F', title="TrigMuGirl Muons RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsCscSegs', type='TH1F', title="TrigMuGirl Muons CSC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsTgcSegs', type='TH1F', title="TrigMuGirl Muons TGC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcSegs', type='TH1F', title="TrigMuGirl Muons TGC RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsBeta', type='TH1F', title="TrigMuGirl Muons beta", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsNNBarrel', type='TH1F', title="TrigMuGirl Muons barrel NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsNNEndcap', type='TH1F', title="TrigMuGirl Muons endcap NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsMeasTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Meas time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsPropTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Prop time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsTof', type='TH1F', title="TrigMuGirl Muons RPC Hits TOF", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsDist', type='TH1F', title="TrigMuGirl Muons RPC Hits Distance", xbins=100, xmin=0, xmax=50) ]


class TrigMuGirlOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuGirlOnlineMonitoring"):
        super(TrigMuGirlOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('MuonsPt', type='TH1F', title="TrigMuGirl muons pT", xbins=1000, xmin=0, xmax=200000) ]
        self.Histograms += [ defineHistogram('MuonsCotTh', type='TH1F', title="TrigMuGirl Muons CotTh", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsPhi', type='TH1F', title="TrigMuGirl Muons phi", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('MuonsMdtHits', type='TH1F', title="TrigMuGirl Muons MDT Hits", xbins=100, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('MuonsRpcPhiHits', type='TH1F', title="TrigMuGirl Muons RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsRpcEtaHits', type='TH1F', title="TrigMuGirl Muons RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsCscPhiHits', type='TH1F', title="TrigMuGirl Muons CSC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsCscEtaHits', type='TH1F', title="TrigMuGirl Muons CSC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsMdtHitsInSeg', type='TH1F', title="TrigMuGirl Muons MDT Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsRpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('MuonsMdtSegs', type='TH1F', title="TrigMuGirl Muons MDT Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsRpcSegs', type='TH1F', title="TrigMuGirl Muons RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsCscSegs', type='TH1F', title="TrigMuGirl Muons CSC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsTgcSegs', type='TH1F', title="TrigMuGirl Muons TGC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsTgcRpcSegs', type='TH1F', title="TrigMuGirl Muons TGC RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('MuonsBeta', type='TH1F', title="TrigMuGirl Muons beta", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsNNBarrel', type='TH1F', title="TrigMuGirl Muons barrel NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsNNEndcap', type='TH1F', title="TrigMuGirl Muons endcap NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsMeasTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Meas time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsPropTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Prop time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsTof', type='TH1F', title="TrigMuGirl Muons RPC Hits TOF", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('MuonsRPCHitsDist', type='TH1F', title="TrigMuGirl Muons RPC Hits Distance", xbins=100, xmin=0, xmax=50) ]

class TrigMuGirlToolMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuGirlToolMonitoring"):
        super(TrigMuGirlToolMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] )

        # TrigMuGirl
        self.Histograms += [ defineHistogram('EF_mgtrks_pt', type='TH1F', title="TrigMuGirl muons pT", xbins=1000, xmin=0, xmax=200000) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_cotTh', type='TH1F', title="TrigMuGirl Muons CotTh", xbins=100, xmin=-10, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_phi', type='TH1F', title="TrigMuGirl Muons phi", xbins=100, xmin=-4, xmax=4) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_mdtHits', type='TH1F', title="TrigMuGirl Muons MDT Hits", xbins=100, xmin=0, xmax=50) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcPhiHits', type='TH1F', title="TrigMuGirl Muons RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcEtaHits', type='TH1F', title="TrigMuGirl Muons RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_cscPhiHits', type='TH1F', title="TrigMuGirl Muons CSC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_cscEtaHits', type='TH1F', title="TrigMuGirl Muons CSC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcRpcPhiHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Phi Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcRpcEtaHits', type='TH1F', title="TrigMuGirl Muons TGC RPC Eta Hits", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_mdtHitsInSeg', type='TH1F', title="TrigMuGirl Muons MDT Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcRpcHitsInSeg', type='TH1F', title="TrigMuGirl Muons TGC RPC Hits in Segments", xbins=30, xmin=0, xmax=30) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_mdtSegs', type='TH1F', title="TrigMuGirl Muons MDT Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcSegs', type='TH1F', title="TrigMuGirl Muons RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_cscSegs', type='TH1F', title="TrigMuGirl Muons CSC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcSegs', type='TH1F', title="TrigMuGirl Muons TGC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_tgcRpcSegs', type='TH1F', title="TrigMuGirl Muons TGC RPC Segments", xbins=10, xmin=0, xmax=10) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_beta', type='TH1F', title="TrigMuGirl Muons beta", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_nnBarrel', type='TH1F', title="TrigMuGirl Muons barrel NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_nnEndcap', type='TH1F', title="TrigMuGirl Muons endcap NN", xbins=120, xmin=0, xmax=1.2) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcHitsMeasTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Meas time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcHitsPropTime', type='TH1F', title="TrigMuGirl Muons RPC Hits Prop time", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcHitsTof', type='TH1F', title="TrigMuGirl Muons RPC Hits TOF", xbins=200, xmin=0, xmax=100) ]
        self.Histograms += [ defineHistogram('EF_mgtrks_rpcHitsDist', type='TH1F', title="TrigMuGirl Muons RPC Hits Distance", xbins=100, xmin=0, xmax=50) ]




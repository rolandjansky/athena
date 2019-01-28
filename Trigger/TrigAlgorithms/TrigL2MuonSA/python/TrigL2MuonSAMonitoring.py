# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

class TrigL2MuonSAMonitoring(GenericMonitoringTool):
    def __init__ (self, name = "TrigL2MuonSAMonitoring"):
        super(TrigL2MuonSAMonitoring, self).__init__( name )
    
        self.HistPath = name
        self.Histograms = [ defineHistogram('InnMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the INNER road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5),
                            defineHistogram('MidMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the MIDDLE road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5),
                            defineHistogram('OutMdtHits',    type='TH1F', path='EXPERT', title="Hit multiplicity in the OUTER road; MDT hits", xbins=50, xmin=-0.5, xmax=50.5),
                            defineHistogram('FitResiduals',  type='TH1F', path='EXPERT', title="Fit Residual; Residuals (cm)", xbins=400, xmin=-0.4, xmax=0.4),
                            defineHistogram('Efficiency',    type='TH1F', path='EXPERT', title="Track finding efficiency", xbins=2, xmin=-0.5, xmax=1.5),
                            defineHistogram('Address',       type='TH1F', path='EXPERT', title="S_address;S_address", xbins=5, xmin=-1.5, xmax=3.5 ),
                            defineHistogram('AbsPt',         type='TH1F', path='EXPERT', title="absolute P_{T};P_{T} (GeV)", xbins=100, xmin=0, xmax=100 ),
                            defineHistogram('TrackPt',       type='TH1F', path='EXPERT', title="P_{T};P_{T} (GeV)", xbins=100, xmin=-100, xmax=100 ),
                            defineHistogram('AbsPt, SagInv', type='TH2F', path='EXPERT', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})", xbins=50, xmin=0, xmax=100, ybins=15, ymin=0, ymax=3 ),
                            defineHistogram('Sagitta',       type='TH1F', path='EXPERT', title="Reconstructed Sagitta; Sagitta (cm)", xbins=100, xmin=-10., xmax=10.),
                            defineHistogram('ResInner',      type='TH1F', path='EXPERT', title="Residual from Trigger track in INNER Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. ),
                            defineHistogram('ResMiddle',     type='TH1F', path='EXPERT', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. ),
                            defineHistogram('ResOuter',      type='TH1F', path='EXPERT', title="Residual from Trigger track in OUTER Station; Residuals (cm)", xbins=100, xmin=-10., xmax=10. ),
                            defineHistogram('TrackEta, TrackPhi',         type='TH2F', path='EXPERT', title="Distribution of reconstructed LVL2 tracks; Eta; Phi", xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ),
                            defineHistogram('FailedRoIEta, FailedRoIPhi', type='TH2F', path='EXPERT', title="Location of LVL2 track failure; Eta; Phi", xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ),
                            defineHistogram('TIME_Total',                 type='TH1F', path='EXPERT', title="Total processing time (us)", xbins=100, xmin=0, xmax=100000 ),
                            defineHistogram('TIME_Data_Preparator',       type='TH1F', path='EXPERT', title="Data preparator time (us)", xbins=100, xmin=0, xmax=50000 ),
                            defineHistogram('TIME_Pattern_Finder',        type='TH1F', path='EXPERT', title="Pattern finder time (us)", xbins=100, xmin=0, xmax=5000 ),
                            defineHistogram('TIME_Station_Fitter',        type='TH1F', path='EXPERT', title="Station fitter time (us)", xbins=100, xmin=0, xmax=5000 ),
                            defineHistogram('TIME_Track_Fitter',          type='TH1F', path='EXPERT', title="Track fitter time (us)", xbins=100, xmin=0, xmax=300 ),
                            defineHistogram('TIME_Track_Extrapolator',    type='TH1F', path='EXPERT', title="Track extrapolator time (us)", xbins=100, xmin=0, xmax=300 ),
                            defineHistogram('TIME_Calibration_Streamer',  type='TH1F', path='EXPERT', title="Calibration streamer time (us)", xbins=100, xmin=0, xmax=50000 ),
                            defineHistogram('InvalidRpcRoINumber',        type='TH1F', path='EXPERT', title="RoI Number of Invalid RPC RoI; RoI Number", xbins=150, xmin=-0.5, xmax=150.5) ]
    


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
	
class TrigL2MuonSAValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonSAValidationMonitoring"):
        super(TrigL2MuonSAValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('InnMdtHits', type='TH1F', title="Hit multiplicity in the INNER road; MDT hits",
                                            xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('MidMdtHits', type='TH1F', title="Hit multiplicity in the MIDDLE road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('OutMdtHits', type='TH1F', title="Hit multiplicity in the OUTER road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('FitResiduals', type='TH1F', title="Fit Residual; Residuals (cm)",
                                             xbins=400, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Efficiency', type='TH1F', title="Track finding efficiency",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('Address', type='TH1F', title="S_address;S_address",
                                             xbins=5, xmin=-1.5, xmax=3.5 ) ]
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
        self.Histograms += [ defineHistogram('TrackPt', type='TH1F', title="P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=-100, xmax=100 ) ]
        self.Histograms += [ defineHistogram('AbsPt, SagInv', type='TH2F', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})",
                                             xbins=50, xmin=0, xmax=100, ybins=15, ymin=0, ymax=3 ) ]
        self.Histograms += [ defineHistogram('Sagitta', type='TH1F', title="Reconstructed Sagitta; Sagitta (cm)",
                                             xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('ResInner', type='TH1F', title="Residual from Trigger track in INNER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResMiddle', type='TH1F', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResOuter', type='TH1F', title="Residual from Trigger track in OUTER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('FailedRoIEta, FailedRoIPhi', type='TH2F', title="Location of LVL2 track failure; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('InvalidRpcRoINumber', type='TH1F', title="RoI Number of Invalid RPC RoI; RoI Number",
                                            xbins=150, xmin=-0.5, xmax=150.5) ]
        
class TrigL2MuonSAOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonSAOnlineMonitoring"):
        super(TrigL2MuonSAOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('InnMdtHits', type='TH1F', title="Hit multiplicity in the INNER road; MDT hits",
                                            xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('MidMdtHits', type='TH1F', title="Hit multiplicity in the MIDDLE road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('OutMdtHits', type='TH1F', title="Hit multiplicity in the OUTER road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('FitResiduals', type='TH1F', title="Fit Residual; Residuals (cm)",
                                             xbins=400, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Efficiency', type='TH1F', title="Track finding efficiency",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('Address', type='TH1F', title="S_address;S_address",
                                             xbins=5, xmin=-1.5, xmax=3.5 ) ]
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
        self.Histograms += [ defineHistogram('TrackPt', type='TH1F', title="P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=-100, xmax=100 ) ]
        self.Histograms += [ defineHistogram('AbsPt, SagInv', type='TH2F', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})",
                                             xbins=50, xmin=0, xmax=100, ybins=15, ymin=0, ymax=3 ) ]
        self.Histograms += [ defineHistogram('Sagitta', type='TH1F', title="Reconstructed Sagitta; Sagitta (cm)",
                                             xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('ResInner', type='TH1F', title="Residual from Trigger track in INNER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResMiddle', type='TH1F', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResOuter', type='TH1F', title="Residual from Trigger track in OUTER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('FailedRoIEta, FailedRoIPhi', type='TH2F', title="Location of LVL2 track failure; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('InvalidRpcRoINumber', type='TH1F', title="RoI Number of Invalid RPC RoI ; RoI Number",
                                            xbins=150, xmin=-0.5, xmax=150.5) ]


class TrigL2MuonSACosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2MuonSACosmicMonitoring"):
        super(TrigL2MuonSACosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('InnMdtHits', type='TH1F', title="Hit multiplicity in the INNER road; MDT hits",
                                            xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('MidMdtHits', type='TH1F', title="Hit multiplicity in the MIDDLE road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('OutMdtHits', type='TH1F', title="Hit multiplicity in the OUTER road; MDT hits",
                                             xbins=50, xmin=-0.5, xmax=50.5) ]
        self.Histograms += [ defineHistogram('FitResiduals', type='TH1F', title="Fit Residual; Residuals (cm)",
                                             xbins=400, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Efficiency', type='TH1F', title="Track finding efficiency",
                                             xbins=2, xmin=-0.5, xmax=1.5) ]
        self.Histograms += [ defineHistogram('Address', type='TH1F', title="S_address;S_address",
                                             xbins=5, xmin=-1.5, xmax=3.5 ) ]
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
        self.Histograms += [ defineHistogram('TrackPt', type='TH1F', title="P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=-100, xmax=100 ) ]
        self.Histograms += [ defineHistogram('AbsPt, SagInv', type='TH2F', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})",
                                             xbins=50, xmin=0, xmax=100, ybins=15, ymin=0, ymax=3 ) ]
        self.Histograms += [ defineHistogram('Sagitta', type='TH1F', title="Reconstructed Sagitta; Sagitta (cm)",
                                             xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('ResInner', type='TH1F', title="Residual from Trigger track in INNER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResMiddle', type='TH1F', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResOuter', type='TH1F', title="Residual from Trigger track in OUTER Station; Residuals (cm)",
                                             xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('FailedRoIEta, FailedRoIPhi', type='TH2F', title="Location of LVL2 track failure; Eta; Phi",
                                             xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('InvalidRpcRoINumber', type='TH1F', title="RoI Number of Invalid RPC RoI; RoI Number",
                                            xbins=150, xmin=-0.5, xmax=150.5) ]
        

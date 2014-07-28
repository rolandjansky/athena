# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigmuFastValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuFastValidationMonitoring"):
        super(TrigmuFastValidationMonitoring, self).__init__(name)
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
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
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
        self.Histograms += [ defineHistogram('CalEvtSize', type='TH1F', title="Size of the muon cablibration event; Byte",
                                            xbins=100, xmin=0., xmax=2000. ) ]
        self.Histograms += [ defineHistogram('BufFreeCnt', type='TH1F', title="Buffer space availability",
                                            xbins=2, xmin=-0.5, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('PADhit, SEChit', type='TH2F', title="Distribution of Muon RoIs into the barrel PADS; PAD number + 1; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADrec, SECrec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADeff, SECeff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]


        self.Histograms += [ defineHistogram('TGCEtaHit, TGCSecHit', type='TH2F', title="Distribution of Muon RoIs into the endcap; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaRec, TGCSecRec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into endcap PADS; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaEff, TGCSecEff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into endcap PADS; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]

        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('L1EmuFailedEta, L1EmuFailedPhi', type='TH2F', title="Distribution of L1 emulation failed RoIs; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('BackExtrFailedEta, BackExtrFailedPhi', type='TH2F', title="Distribution of backExtrapolator failed RoIs; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]


class TrigmuFastOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuFastOnlineMonitoring"):
        super(TrigmuFastOnlineMonitoring, self).__init__(name)
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
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
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
        self.Histograms += [ defineHistogram('CalEvtSize', type='TH1F', title="Size of the muon cablibration event; Byte",
                                            xbins=100, xmin=0., xmax=2000. ) ]
        self.Histograms += [ defineHistogram('BufFreeCnt', type='TH1F', title="Buffer space availability",
                                            xbins=2, xmin=-0.5, xmax=1.5 ) ]

        self.Histograms += [ defineHistogram('PADhit, SEChit', type='TH2F', title="Distribution of Muon RoIs into the barrel PADS; PAD number + 1; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADrec, SECrec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADeff, SECeff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]

        self.Histograms += [ defineHistogram('TGCEtaHit, TGCSecHit', type='TH2F', title="Distribution of Muon RoIs into the endcap; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaRec, TGCSecRec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into endcap PADS; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaEff, TGCSecEff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into endcap PADS; RoIEta; RoISector",
                                             xbins=52, xmin=-51.5, xmax=52.5, ybins=24, ymin=-.5, ymax=191.5 ) ]

        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('L1EmuFailedEta, L1EmuFailedPhi', type='TH2F', title="Distribution of L1 emulation failed RoIs; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]
        self.Histograms += [ defineHistogram('BackExtrFailedEta, BackExtrFailedPhi', type='TH2F', title="Distribution of backExtrapolator failed RoIs; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]


class TrigmuFastCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigmuFastCosmicMonitoring"):
        super(TrigmuFastCosmicMonitoring, self).__init__(name)
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
        self.Histograms += [ defineHistogram('AbsPt', type='TH1F', title="absolute P_{T};P_{T} (GeV)",
                                             xbins=100, xmin=0, xmax=100 ) ]
        self.Histograms += [ defineHistogram('AbsPt, SagInv', type='TH2F', title="1/s as a function of P_{T}; P_{T} (GeV); 1/s (cm^{-1})",
                                            xbins=100, xmin=0, xmax=100, ybins=30, ymin=0, ymax=3 ) ]
        self.Histograms += [ defineHistogram('Sagitta', type='TH1F', title="Reconstructed Sagitta; Sagitta (cm)",
                                            xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('ResInner', type='TH1F', title="Residual from Trigger track in INNER Station; Residuals (cm)",
                                            xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResMiddle', type='TH1F', title="Residual from Trigger track in MIDDLE Station; Residuals (cm)",
                                            xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('ResOuter', type='TH1F', title="Residual from Trigger track in OUTER Station; Residuals (cm)",
                                            xbins=100, xmin=-10., xmax=10. ) ]
        self.Histograms += [ defineHistogram('CalEvtSize', type='TH1F', title="Size of the muon cablibration event; Byte",
                                            xbins=100, xmin=0., xmax=2000. ) ]
        self.Histograms += [ defineHistogram('BufFreeCnt', type='TH1F', title="Buffer space availability",
                                            xbins=2, xmin=-0.5, xmax=1.5 ) ]
					    
        self.Histograms += [ defineHistogram('LVL1Eta, EmuEta', type='TH2F', title="LVL1 res vs Emulation: Eta",
                                             xbins=50, xmin=-2.5, xmax=2.5, ybins=50, ymin=-2.5, ymax=2.5 ) ]
        self.Histograms += [ defineHistogram('LVL1Phi, EmuPhi', type='TH2F', title="LVL1 res vs Emulation: Phi",
                                             xbins=63, xmin=0, xmax=6.29, ybins=63, ymin=0, ymax=6.29 ) ]
        self.Histograms += [ defineHistogram('LVL1Pt, EmuPt', type='TH2F', title="LVL1 res vs Emulation: Pt",
                                             xbins=10, xmin=0, xmax=10, ybins=10, ymin=0, ymax=10 ) ]

        self.Histograms += [ defineHistogram('PADhit, SEChit', type='TH2F', title="Distribution of Muon RoIs into the barrel PADS; PAD number + 1; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADrec, SECrec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]
        self.Histograms += [ defineHistogram('PADeff, SECeff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into barrel PADS; (PAD number + 1)*side; Trigger Sectors",
                                            xbins=15, xmin=-7.5, xmax=7.5, ybins=32, ymin=-.5, ymax=31.5 ) ]

        self.Histograms += [ defineHistogram('TGCEtaHit, TGCSecHit', type='TH2F', title="Distribution of Muon RoIs into the endcap; RoIEta; RoISector",
                                             xbins=105, xmin=-52.5, xmax=52.5, ybins=48, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaRec, TGCSecRec', type='TH2F', title="Distribution of reconstructed LVL1 triggers into endcap PADS; RoIEta; RoISector",
                                             xbins=105, xmin=-52.5, xmax=52.5, ybins=48, ymin=-.5, ymax=191.5 ) ]
        self.Histograms += [ defineHistogram('TGCEtaEff, TGCSecEff', type='TH2F', title="Distribution of reconstructed LVL2 tracks into endcap PADS; RoIEta; RoISector",
                                             xbins=105, xmin=-52.5, xmax=52.5, ybins=48, ymin=-.5, ymax=191.5 ) ]

        self.Histograms += [ defineHistogram('TrackEta, TrackPhi', type='TH2F', title="Distribution of reconstructed LVL2 tracks; Eta; Phi",
                                            xbins=108, xmin=-2.7, xmax=2.7, ybins=96, ymin=-3.1416, ymax=3.1416 ) ]


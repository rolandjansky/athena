# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigL2DsPhiPiFexValidationMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiFexValidation_RoI"):
        super(TrigL2DsPhiPiFexValidationMonitoring_RoI, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="L2DsPhipiFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2DsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2DsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2DsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2DsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2DsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="L2DsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1200.) ]
        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="L2DsPhipiFex FitPhiMass; FitPhiMass [MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="L2DsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="L2DsPhipiFex FitDsMass; FitDsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Ds; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob_max', type='TH1F', title="ValidationMonitoring_RoI Chi2prob_max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_DsPass', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max_DsPass', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi max for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]

class TrigL2DsPhiPiFexOnlineMonitoring_RoI(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiFexOnline_RoI"):
        super(TrigL2DsPhiPiFexOnlineMonitoring_RoI, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="L2DsPhipiFex Ntrack",
                                           xbins=50, xmin=0.0, xmax=50.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2DsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2DsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2DsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2DsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2DsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="L2DsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1200.) ]
        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="L2DsPhipiFex FitPhiMass; FitPhiMass [MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="L2DsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="L2DsPhipiFex FitDsMass; FitDsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob', type='TH1F', title="OnlineMonitoring_RoI Chi2prob Phi; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob', type='TH1F', title="OnlineMonitoring_RoI Chi2prob Ds; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob_max', type='TH1F', title="OnlineMonitoring_RoI Chi2prob_max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_DsPass', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max_DsPass', type='TH1F', title="ValidationMonitoring_RoI Chi2prob Phi max for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]


class TrigL2DsPhiPiFexValidationMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiFexValidation_FS"):
        super(TrigL2DsPhiPiFexValidationMonitoring_FS, self).__init__(name)
        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="L2DsPhipiFex Ntrack",
                                           xbins=10, xmin=0.0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2DsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2DsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2DsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2DsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2DsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="L2DsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1200.) ]
        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="L2DsPhipiFex FitPhiMass; FitPhiMass [MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="L2DsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="L2DsPhipiFex FitDsMass; FitDsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob', type='TH1F', title="ValidationMonitoring_FS Chi2prob Ds; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob_max', type='TH1F', title="ValidationMonitoring_FS Chi2prob_max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_DsPass', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max_DsPass', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi max for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]


class TrigL2DsPhiPiFexOnlineMonitoring_FS(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2DsPhiPiFexOnline_FS"):
        super(TrigL2DsPhiPiFexOnlineMonitoring_FS, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('Ntrack', type='TH1F', title="L2DsPhipiFex Ntrack",
                                           xbins=10, xmin=0.0, xmax=100.0) ]
        self.Histograms += [ defineHistogram('trackPt', type='TH1F', title="L2DsPhipiFex trackPt; P_{t} [GeV]",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('dEta', type='TH1F', title="L2DsPhipiFex track deta ",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dPhi', type='TH1F', title="L2DsPhipiFex track dphi",
                                           xbins=50, xmin=0.0, xmax=1.0) ]
        self.Histograms += [ defineHistogram('dz0', type='TH1F', title="L2DsPhipiFex track dz0",
                                           xbins=50, xmin=0.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('Ptsum', type='TH1F', title="L2DsPhipiFex track pTsum",
                                           xbins=50, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('PhiMass', type='TH1F', title="L2DsPhipiFex PhiMass; PhiMass[MeV]",
                                           xbins=100, xmin=970., xmax=1200.) ]
        self.Histograms += [ defineHistogram('PhiFitMass', type='TH1F', title="L2DsPhipiFex FitPhiMass; FitPhiMass [MeV]",
                                           xbins=100, xmin=970., xmax=1070.) ]
        self.Histograms += [ defineHistogram('DsMass', type='TH1F', title="L2DsPhipiFex DsMass; DsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('DsFitMass', type='TH1F', title="L2DsPhipiFex FitDsMass; FitDsMass [MeV]",
                                           xbins=100, xmin=1750., xmax=2200.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob', type='TH1F', title="OnlineMonitoring_FS Chi2prob Phi; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob', type='TH1F', title="OnlineMonitoring_FS Chi2prob Ds; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('DsChi2prob_max', type='TH1F', title="OnlineMonitoring_FS Chi2prob_max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_DsPass', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi max; probability",
                                           xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('PhiChi2prob_max_DsPass', type='TH1F', title="ValidationMonitoring_FS Chi2prob Phi max for Ds Cand; probability",
                                           xbins=100, xmin=0., xmax=1.) ]


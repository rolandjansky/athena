# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigEFCaloHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigL2ElectronHypoOnlineMonitoring"):
        super(TrigEFCaloHypoOnlineMonitoring, self).__init__(name)
        self.defineTarget( "Online" )

        self.Histograms += [ defineHistogram('EnergyBE0', type='TH1F', title="EF Calo Hypo Energy BE0;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE1', type='TH1F', title="EF Calo Hypo Energy BE1;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE2', type='TH1F', title="EF Calo Hypo Energy BE2;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE3', type='TH1F', title="EF Calo Hypo Energy BE3;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="EF Calo Hypo Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ECalib', type='TH1F', title="EF Calo Hypo Calib Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ERes', type='TH1F', title="EF Calo Hypo Energy Resolution ;E_{raw}-E_{calib};Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('EtaCalo', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhiCalo', type='TH1F', title="EF Calo Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]


##########   Create instance for validation  - same as online here
class TrigEFCaloHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFCaloHypoValidationMonitoring"):
        super(TrigEFCaloHypoValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('EnergyBE0', type='TH1F', title="EF Calo Hypo Energy BE0;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE1', type='TH1F', title="EF Calo Hypo Energy BE1;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE2', type='TH1F', title="EF Calo Hypo Energy BE2;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE3', type='TH1F', title="EF Calo Hypo Energy BE3;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="EF Calo Hypo Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ECalib', type='TH1F', title="EF Calo Hypo Calib Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ERes', type='TH1F', title="EF Calo Hypo Energy Resolution ;E_{raw}-E_{calib};Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('EtaCalo', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhiCalo', type='TH1F', title="EF Calo Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]

class TrigEFCaloCalibFexOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFCaloCalibFexOnlineMonitoring"):
        super(TrigEFCaloCalibFexOnlineMonitoring, self).__init__(name)
        self.defineTarget( "Online" )

        self.Histograms += [ defineHistogram('EnergyBE0', type='TH1F', title="EF Calo Hypo Energy BE0;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE1', type='TH1F', title="EF Calo Hypo Energy BE1;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE2', type='TH1F', title="EF Calo Hypo Energy BE2;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE3', type='TH1F', title="EF Calo Hypo Energy BE3;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="EF Calo Hypo Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ECalib', type='TH1F', title="EF Calo Hypo Calib Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ERes', type='TH1F', title="EF Calo Hypo Energy Resolution ;E_{raw}-E_{calib};Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('EtaCalo', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhiCalo', type='TH1F', title="EF Calo Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]


##########   Create instance for validation  - same as online here
class TrigEFCaloCalibFexValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFCaloCalibFexValidationMonitoring"):
        super(TrigEFCaloCalibFexValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('EnergyBE0', type='TH1F', title="EF Calo Hypo Energy BE0;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE1', type='TH1F', title="EF Calo Hypo Energy BE1;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE2', type='TH1F', title="EF Calo Hypo Energy BE2;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('EnergyBE3', type='TH1F', title="EF Calo Hypo Energy BE3;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="EF Calo Hypo Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ECalib', type='TH1F', title="EF Calo Hypo Calib Energy ;E;Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('ERes', type='TH1F', title="EF Calo Hypo Energy Resolution ;E_{raw}-E_{calib};Nevents", xbins=80, xmin=-0.4, xmax=0.4) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('EtaCalo', type='TH1F', title="EF Calo Hypo #eta^{calo} ; #eta^{calo};Nevents", xbins=200, xmin=-2.5, xmax=2.5) ]
        self.Histograms += [ defineHistogram('PhiCalo', type='TH1F', title="EF Calo Hypo #phi^{calo} ; #phi^{calo};Nevents", xbins=320, xmin=-3.2, xmax=3.2) ]




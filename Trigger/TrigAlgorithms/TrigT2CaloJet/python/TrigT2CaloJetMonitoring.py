# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Definitions of the monitoring classes  



from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 


class TrigT2CaloJetValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloJet_Validation"):
        super(TrigT2CaloJetValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
# this target contains all the histograms necessary for validation

        self.Histograms = [ defineHistogram('dR', type='TH1F', title="L2CaloJet FEX dR between roi and jet ;dR; nevents", xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="L2CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="L2CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="L2CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="L2CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="L2CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="L2CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]

        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram ('ConversionErrors' ,type='TH1F',title='Conversion Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('Eta, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
        self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
        
class TrigT2CaloJetOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloJet_Online"):
        """ defines hisograms for online """
        super(TrigT2CaloJetOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
# right now this target has the same list of histograms as "Validation"

        self.Histograms = [ defineHistogram('dR', type='TH1F', title="L2CaloJet FEX dR between roi and jet ;dR; nevents", xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="L2CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="L2CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="L2CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="L2CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="L2CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="L2CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram ('ConversionErrors' ,type='TH1F',title='Conversion Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('Eta, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
        self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]


class TrigT2CaloJetCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloJet_Cosmic"):
        """ defines hisograms for online """
        super(TrigT2CaloJetCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
# this target contains the same list of histograms as for "Online", but the axis range for plots of Et and etc. are smaller 

        self.Histograms = [ defineHistogram('dR', type='TH1F', title="L2CaloJet FEX dR between roi and jet ;dR; nevents", xbins=100, xmin=-10., xmax=10.) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="L2CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="L2CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="L2CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="L2CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="L2CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="L2CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="L2CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="L2CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="L2CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="L2CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="L2CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=300000.) ]
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="L2CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="L2CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="L2CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="L2CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram ('ConversionErrors' ,type='TH1F',title='Conversion Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        self.Histograms += [ defineHistogram ('Eta, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Eta;#eta; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, ConversionErrors' ,type='TH2F',title='ConversionErrors vs Phi;#phi; Conversion Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
        self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]


class TrigT2CaloJetTimeMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloJet_TimeUnpack"):
        super(TrigT2CaloJetTimeMonitoring, self).__init__(name)
        self.defineTarget("Time")
        self.Histograms += [ defineHistogram('LArUnpck', type='TH1F', title="L2CaloJet Unpacking time (LAr)", xbins=100, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('TileUnpck', type='TH1F', title="L2CaloJet Unpacking time (Tile)", xbins=100, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('HECUnpck', type='TH1F', title="L2CaloJet Unpacking time (HEC)", xbins=100, xmin=0., xmax=50.) ]

class TrigT2L1CaloJetValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1CaloJet_Validation"):
        super(TrigT2L1CaloJetValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        # this target contains all the histograms necessary for validation
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="T2L1CaloJet FEX number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2L1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2L1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="T2L1CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="T2L1CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="T2L1CaloJet FEX nLeadingTowers;nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf', type='TH1F', title="T2L1CaloJet FEX emf;EMF; nevents", xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2L1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2L1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('nTowers', type='TH1F', title="T2L1CaloJet number of L1 towers;number of L1 towers; nevents", xbins=101, xmin=-0.5, xmax=100.5) ]
        self.Histograms += [ defineHistogram('UnpckTime', type='TH1F', title="T2L1CaloJet Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('L1_UnpckTime', type='TH1F', title="T2L1CaloJet L1 Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('FastJetTime', type='TH1F', title="T2L1CaloJet FastJet time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('JetFindingTime', type='TH1F', title="T2L1CaloJet Jet finding time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('OverallTime', type='TH1F', title="T2L1CaloJet Total time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('RoITime', type='TH1F', title="T2L1CaloJet RoI making time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2L1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2L1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2L1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="T2L1CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="T2L1CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2L1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2L1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',      type='TH2F', title="T2L1CaloJet Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,L1_UnpckTime',      type='TH2F', title="T2L1CaloJet L1 Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,FastJetTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,JetFindingTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,OverallTime', type='TH2F', title="T2L1CaloJet Total time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime', type='TH2F', title="T2L1CaloJet RoI making time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
        
class TrigT2L1CaloJetOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1CaloJet_Online"):
        """ defines hisograms for online """
        super(TrigT2L1CaloJetOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        # right now this target has the same list of histograms as "Validation"
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="T2L1CaloJet FEX number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2L1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2L1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="T2L1CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="T2L1CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="T2L1CaloJet FEX nLeadingTowers;nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf', type='TH1F', title="T2L1CaloJet FEX emf;EMF; nevents", xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2L1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2L1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('nTowers', type='TH1F', title="T2L1CaloJet number of L1 towers;number of L1 towers; nevents", xbins=101, xmin=-0.5, xmax=100.5) ]
        self.Histograms += [ defineHistogram('UnpckTime', type='TH1F', title="T2L1CaloJet Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('L1_UnpckTime', type='TH1F', title="T2L1CaloJet L1 Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('FastJetTime', type='TH1F', title="T2L1CaloJet FastJet time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('JetFindingTime', type='TH1F', title="T2L1CaloJet Jet finding time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('OverallTime', type='TH1F', title="T2L1CaloJet Total time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('RoITime', type='TH1F', title="T2L1CaloJet RoI making time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2L1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2L1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2L1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="T2L1CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="T2L1CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2L1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2L1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',      type='TH2F', title="T2L1CaloJet Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,L1_UnpckTime',      type='TH2F', title="T2L1CaloJet L1 Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,FastJetTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,JetFindingTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,OverallTime', type='TH2F', title="T2L1CaloJet Total time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime', type='TH2F', title="T2L1CaloJet RoI making time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
class TrigT2L1CaloJetCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1CaloJet_Cosmic"):
        """ defines hisograms for online """
        super(TrigT2L1CaloJetCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
    # this target contains the same list of histograms as for "Online", but the axis range for plots of Et and etc. are smaller 
        self.Histograms = [ defineHistogram('nJets', type='TH1F', title="T2L1CaloJet FEX number of jets;N; nevents", xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2L1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2L1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eem', type='TH1F', title="T2L1CaloJet FEX Eem;Eem [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Ehad', type='TH1F', title="T2L1CaloJet FEX Ehad;Ehad [MeV]; nevents", xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers', type='TH1F', title="T2L1CaloJet FEX nLeadingTowers;nLeadingTowers; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf', type='TH1F', title="T2L1CaloJet FEX emf;EMF; nevents", xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2L1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2L1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('nTowers', type='TH1F', title="T2L1CaloJet number of L1 towers;number of L1 towers; nevents", xbins=101, xmin=-0.5, xmax=100.5) ]
        self.Histograms += [ defineHistogram('UnpckTime', type='TH1F', title="T2L1CaloJet Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('L1_UnpckTime', type='TH1F', title="T2L1CaloJet L1 Unpacking time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('FastJetTime', type='TH1F', title="T2L1CaloJet FastJet time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('JetFindingTime', type='TH1F', title="T2L1CaloJet Jet finding time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('OverallTime', type='TH1F', title="T2L1CaloJet Total time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('RoITime', type='TH1F', title="T2L1CaloJet RoI making time; time [ms]; nevents", xbins=200, xmin=0., xmax=200.) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2L1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2L1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2L1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Eem', type='TH2F', title="T2L1CaloJet EM ene vs. eta;#eta;Eem [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ehad', type='TH2F', title="T2L1CaloJet Had ene vs. eta;#eta;Ehad [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Etem', type='TH2F', title="T2L1CaloJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Ethad', type='TH2F', title="T2L1CaloJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]", xbins=100, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2L1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2L1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',      type='TH2F', title="T2L1CaloJet Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,L1_UnpckTime',      type='TH2F', title="T2L1CaloJet L1 Unpack time vs. nTowers;nTowers;time [ms]",      xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,FastJetTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,JetFindingTime', type='TH2F', title="T2L1CaloJet Jet finding time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,OverallTime', type='TH2F', title="T2L1CaloJet Total time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime', type='TH2F', title="T2L1CaloJet RoI making time vs. nTowers;nTowers;time [ms]", xbins=101, xmin=0., xmax=500., ybins=100, ymin=0., ymax=200.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
class TrigT2L1UnpackingValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1Unpacking_Validation"):
        super(TrigT2L1UnpackingValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        # this target contains all the histograms necessary for validation
        self.Histograms += [ defineHistogram('nTowers',        type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nEMTowers',      type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nHADTowers',     type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nTowers_zoom',   type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nEMTowers_zoom', type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nHADTowers_zoom',type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('Et',             type='TH1F', title="T2L1Unpacking FEX Et;Et [MeV]; nevents",                         xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',           type='TH1F', title="T2L1Unpacking FEX Etem;EM Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',          type='TH1F', title="T2L1Unpacking FEX Ethad;Had Et [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eta',            type='TH1F', title="T2L1Unpacking FEX Eta;Eta; nevents",                             xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaEM',          type='TH1F', title="T2L1Unpacking FEX EM Eta;EM Eta; nevents",                       xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Eta;HAD Eta; nevents",                     xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',            type='TH1F', title="T2L1Unpacking FEX Phi;Phi; nevents",                             xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiEM',          type='TH1F', title="T2L1Unpacking FEX EM Phi;EM Phi; nevents",                       xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Phi;HAD Phi; nevents",                     xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('L1UnpckTime',    type='TH1F', title="T2L1Unpacking L1 unpacking time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('UnpckTime',      type='TH1F', title="T2L1Unpacking All unpacking time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime',type='TH1F', title="T2L1Unpacking Calibration time; time [ms]; nevents",             xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',        type='TH1F', title="T2L1Unpacking RoI making time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',    type='TH1F', title="T2L1Unpacking Total time; time [ms]; nevents",                   xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',        type='TH2F', title="T2L1Unpacking phi vs. eta;#eta;#phi",                            xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaEM,PhiEM',    type='TH2F', title="T2L1Unpacking EM phi vs. eta;EM #eta;EM #phi",                   xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaHAD,PhiHAD',  type='TH2F', title="T2L1Unpacking HAD phi vs. eta;HAD #eta;HAD #phi",                xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',         type='TH2F', title="T2L1Unpacking Et vs. eta;#eta;Et [MeV]",                         xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #eta;EM #eta;E_{T}^{em} [MeV]",       xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #eta;HAD #eta;E_{T}^{had} [MeV]",    xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',         type='TH2F', title="T2L1Unpacking Et vs. phi;#phi;Et [MeV]",                         xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #phi;EM #phi;E_{T}^{em} [MeV]",       xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #phi;HAD #phi;E_{T}^{had} [MeV]",    xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,L1UnpckTime',     type='TH2F', title="T2L1Unpacking L1 unpack time vs. nTowers;nTowers;time [ms]",         xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',       type='TH2F', title="T2L1Unpacking All unpack time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,CalibrationTime', type='TH2F', title="T2L1Unpacking Tower calibration time vs. nTowers;nTowers;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime',         type='TH2F', title="T2L1Unpacking RoI making time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,TotalTime',     type='TH2F', title="T2L1Unpacking Total time vs. nTowers;nTowers;time [ms]",             xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
       
        
class TrigT2L1UnpackingOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1Unpacking_Online"):
        """ defines hisograms for online """
        super(TrigT2L1UnpackingOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        # right now this target has the same list of histograms as "Validation"
        self.Histograms += [ defineHistogram('nTowers',        type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nEMTowers',      type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nHADTowers',     type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nTowers_zoom',   type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nEMTowers_zoom', type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nHADTowers_zoom',type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('Et',             type='TH1F', title="T2L1Unpacking FEX Et;Et [MeV]; nevents",                         xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',           type='TH1F', title="T2L1Unpacking FEX Etem;EM Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',          type='TH1F', title="T2L1Unpacking FEX Ethad;Had Et [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eta',            type='TH1F', title="T2L1Unpacking FEX Eta;Eta; nevents",                             xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaEM',          type='TH1F', title="T2L1Unpacking FEX EM Eta;EM Eta; nevents",                       xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Eta;HAD Eta; nevents",                     xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',            type='TH1F', title="T2L1Unpacking FEX Phi;Phi; nevents",                             xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiEM',          type='TH1F', title="T2L1Unpacking FEX EM Phi;EM Phi; nevents",                       xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Phi;HAD Phi; nevents",                     xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('L1UnpckTime',    type='TH1F', title="T2L1Unpacking L1 unpacking time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('UnpckTime',      type='TH1F', title="T2L1Unpacking All unpacking time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime',type='TH1F', title="T2L1Unpacking Calibration time; time [ms]; nevents",             xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',        type='TH1F', title="T2L1Unpacking RoI making time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',    type='TH1F', title="T2L1Unpacking Total time; time [ms]; nevents",                   xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',        type='TH2F', title="T2L1Unpacking phi vs. eta;#eta;#phi",                            xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaEM,PhiEM',    type='TH2F', title="T2L1Unpacking EM phi vs. eta;EM #eta;EM #phi",                   xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaHAD,PhiHAD',  type='TH2F', title="T2L1Unpacking HAD phi vs. eta;HAD #eta;HAD #phi",                xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',         type='TH2F', title="T2L1Unpacking Et vs. eta;#eta;Et [MeV]",                         xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #eta;EM #eta;E_{T}^{em} [MeV]",       xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #eta;HAD #eta;E_{T}^{had} [MeV]",    xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',         type='TH2F', title="T2L1Unpacking Et vs. phi;#phi;Et [MeV]",                         xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #phi;EM #phi;E_{T}^{em} [MeV]",       xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #phi;HAD #phi;E_{T}^{had} [MeV]",    xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,L1UnpckTime',     type='TH2F', title="T2L1Unpacking L1 unpack time vs. nTowers;nTowers;time [ms]",         xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',       type='TH2F', title="T2L1Unpacking All unpack time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,CalibrationTime', type='TH2F', title="T2L1Unpacking Tower calibration time vs. nTowers;nTowers;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime',         type='TH2F', title="T2L1Unpacking RoI making time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,TotalTime',     type='TH2F', title="T2L1Unpacking Total time vs. nTowers;nTowers;time [ms]",             xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
       
        
class TrigT2L1UnpackingCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2L1Unpacking_Cosmic"):
        """ defines hisograms for online """
        super(TrigT2L1UnpackingCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
        # this target contains the same list of histograms as for "Online", but the axis range for plots of Et and etc. are smaller 
        self.Histograms += [ defineHistogram('nEMTowers',      type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nHADTowers',     type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nTowers',        type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('nTowers_zoom',   type='TH1F', title="T2L1Unpacking number of L1 towers;number of L1 towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nEMTowers_zoom', type='TH1F', title="T2L1Unpacking number of L1 EM towers;number of L1 EM towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('nHADTowers_zoom',type='TH1F', title="T2L1Unpacking number of L1 HAD towers;number of L1 HAD towers; nevents", xbins=50, xmin=0., xmax=50.) ]
        self.Histograms += [ defineHistogram('Et',             type='TH1F', title="T2L1Unpacking FEX Et;Et [MeV]; nevents",                         xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',           type='TH1F', title="T2L1Unpacking FEX Etem;EM Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',          type='TH1F', title="T2L1Unpacking FEX Ethad;Had Et [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('Eta',            type='TH1F', title="T2L1Unpacking FEX Eta;Eta; nevents",                             xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaEM',          type='TH1F', title="T2L1Unpacking FEX EM Eta;EM Eta; nevents",                       xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('EtaHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Eta;HAD Eta; nevents",                     xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',            type='TH1F', title="T2L1Unpacking FEX Phi;Phi; nevents",                             xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiEM',          type='TH1F', title="T2L1Unpacking FEX EM Phi;EM Phi; nevents",                       xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('PhiHAD',         type='TH1F', title="T2L1Unpacking FEX HAD Phi;HAD Phi; nevents",                     xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('L1UnpckTime',    type='TH1F', title="T2L1Unpacking L1 unpacking time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('UnpckTime',      type='TH1F', title="T2L1Unpacking All unpacking time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime',type='TH1F', title="T2L1Unpacking Calibration time; time [ms]; nevents",             xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',        type='TH1F', title="T2L1Unpacking RoI making time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',    type='TH1F', title="T2L1Unpacking Total time; time [ms]; nevents",                   xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',        type='TH2F', title="T2L1Unpacking phi vs. eta;#eta;#phi",                            xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaEM,PhiEM',    type='TH2F', title="T2L1Unpacking EM phi vs. eta;EM #eta;EM #phi",                   xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('EtaHAD,PhiHAD',  type='TH2F', title="T2L1Unpacking HAD phi vs. eta;HAD #eta;HAD #phi",                xbins=100, xmin=-5., xmax=5.,        ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',         type='TH2F', title="T2L1Unpacking Et vs. eta;#eta;Et [MeV]",                         xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #eta;EM #eta;E_{T}^{em} [MeV]",       xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('EtaHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #eta;HAD #eta;E_{T}^{had} [MeV]",    xbins=100, xmin=-5., xmax=5.,        ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',         type='TH2F', title="T2L1Unpacking Et vs. phi;#phi;Et [MeV]",                         xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiEM,EtEM',     type='TH2F', title="T2L1Unpacking EM E_{T} vs. #phi;EM #phi;E_{T}^{em} [MeV]",       xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('PhiHAD,EtHAD',   type='TH2F', title="T2L1Unpacking HAD E_{T} vs. #phi;HAD #phi;E_{T}^{had} [MeV]",    xbins=100, xmin=-3.1416, xmax=3.1416,ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nTowers,L1UnpckTime',     type='TH2F', title="T2L1Unpacking L1 unpack time vs. nTowers;nTowers;time [ms]",         xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,UnpckTime',       type='TH2F', title="T2L1Unpacking All unpack time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,CalibrationTime', type='TH2F', title="T2L1Unpacking Tower calibration time vs. nTowers;nTowers;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,RoITime',         type='TH2F', title="T2L1Unpacking RoI making time vs. nTowers;nTowers;time [ms]",        xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nTowers,TotalTime',     type='TH2F', title="T2L1Unpacking Total time vs. nTowers;nTowers;time [ms]",             xbins=100, xmin=0., xmax=1000.,   ybins=100, ymin=0., ymax=100.) ]
       
class TrigT2CaloFastJetValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloFastJet_Validation"):
        super(TrigT2CaloFastJetValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        # this target contains all the histograms necessary for validation
        self.Histograms = [  defineHistogram('nJets',           type='TH1F', title="T2CaloFastJet FEX number of jets;N;nevents",                xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('nGrid',           type='TH1F', title="T2CaloFastJet FEX number of inputs;N;nevents",              xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('Et',              type='TH1F', title="T2CaloFastJet FEX Et;Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',            type='TH1F', title="T2CaloFastJet FEX Et EM;Etem [MeV]; nevents",               xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',           type='TH1F', title="T2CaloFastJet FEX Et HAD;Ethad [MeV]; nevents",             xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('E',               type='TH1F', title="T2CaloFastJet FEX E;E [MeV]; nevents",                      xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EEM',             type='TH1F', title="T2CaloFastJet FEX Eem;Eem [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EHAD',            type='TH1F', title="T2CaloFastJet FEX Ehad;Ehad [MeV]; nevents",                xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers',  type='TH1F', title="T2CaloFastJet FEX nLeadingTowers;nLeadingTowers; nevents",  xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf',             type='TH1F', title="T2CaloFastJet FEX emf;EMF; nevents",                        xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta',             type='TH1F', title="T2CaloFastJet FEX Eta;Eta; nevents",                        xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',             type='TH1F', title="T2CaloFastJet FEX Phi;Phi; nevents",                        xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('UnpackTime',      type='TH1F', title="T2CaloFastJet Unpacking time; time [ms]; nevents",          xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('FastJetTime',     type='TH1F', title="T2CaloFastJet FastJet time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('puSubTime',       type='TH1F', title="T2CaloFastJet FastJet pu sub time; time [ms]; nevents",     xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CleaningTime',    type='TH1F', title="T2CaloFastJet Cleaning time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime', type='TH1F', title="T2CaloFastJet Calibration time; time [ms]; nevents",        xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',         type='TH1F', title="T2CaloFastJet RoI making time; time [ms]; nevents",         xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',     type='TH1F', title="T2CaloFastJet Total time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',         type='TH2F', title="T2CaloFastJet phi vs. eta;#eta;#phi",                       xbins=100, xmin=-5., xmax=5.,           ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',          type='TH2F', title="T2CaloFastJet Et vs. eta;#eta;Et [MeV]",                    xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]",     xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]",   xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,E',           type='TH2F', title="T2CaloFastJet ene vs. eta;#eta;ene [MeV]",                  xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EEM',         type='TH2F', title="T2CaloFastJet EM ene vs. eta;#eta;Eem [MeV]",               xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EHAD',        type='TH2F', title="T2CaloFastJet Had ene vs. eta;#eta;Ehad [MeV]",             xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E',           type='TH2F', title="T2CaloFastJet ene vs. phi;#phi;ene [MeV]",                  xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',          type='TH2F', title="T2CaloFastJet Et vs. phi;#phi;Et [MeV]",                    xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]",     xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]",   xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nGrid,UnpackTime',      type='TH2F', title="T2CaloFastJet Unpack time vs. nGrid;nGrid;time [ms]",       xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nGrid;nGrid;time [ms]",  xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nJets;nJets;time [ms]",  xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CalibrationTime', type='TH2F', title="T2CaloFastJet Jet calibration time vs. nJets;nJets;time [ms]",xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,RoITime',         type='TH2F', title="T2CaloFastJet RoI making time vs. nJets;nJets;time [ms]",   xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nGrid;nGrid;time [ms]",        xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nJets;nJets;time [ms]",        xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
class TrigT2CaloFastJetOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloFastJet_Online"):
        super(TrigT2CaloFastJetOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
        # this target contains all the histograms necessary for Online
        self.Histograms = [  defineHistogram('nJets',           type='TH1F', title="T2CaloFastJet FEX number of jets;N;nevents",                xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('nGrid',           type='TH1F', title="T2CaloFastJet FEX number of inputs;N;nevents",              xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('Et',              type='TH1F', title="T2CaloFastJet FEX Et;Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',            type='TH1F', title="T2CaloFastJet FEX Et EM;Etem [MeV]; nevents",               xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',           type='TH1F', title="T2CaloFastJet FEX Et HAD;Ethad [MeV]; nevents",             xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('E',               type='TH1F', title="T2CaloFastJet FEX E;E [MeV]; nevents",                      xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EEM',             type='TH1F', title="T2CaloFastJet FEX Eem;Eem [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EHAD',            type='TH1F', title="T2CaloFastJet FEX Ehad;Ehad [MeV]; nevents",                xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers',  type='TH1F', title="T2CaloFastJet FEX nLeadingTowers;nLeadingTowers; nevents",  xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf',             type='TH1F', title="T2CaloFastJet FEX emf;EMF; nevents",                        xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta',             type='TH1F', title="T2CaloFastJet FEX Eta;Eta; nevents",                        xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',             type='TH1F', title="T2CaloFastJet FEX Phi;Phi; nevents",                        xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('UnpackTime',      type='TH1F', title="T2CaloFastJet Unpacking time; time [ms]; nevents",          xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('FastJetTime',     type='TH1F', title="T2CaloFastJet FastJet time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('puSubTime',       type='TH1F', title="T2CaloFastJet FastJet pu sub time; time [ms]; nevents",     xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CleaningTime',    type='TH1F', title="T2CaloFastJet Cleaning time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime', type='TH1F', title="T2CaloFastJet Calibration time; time [ms]; nevents",        xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',         type='TH1F', title="T2CaloFastJet RoI making time; time [ms]; nevents",         xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',     type='TH1F', title="T2CaloFastJet Total time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',         type='TH2F', title="T2CaloFastJet phi vs. eta;#eta;#phi",                       xbins=100, xmin=-5., xmax=5.,           ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',          type='TH2F', title="T2CaloFastJet Et vs. eta;#eta;Et [MeV]",                    xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]",     xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]",   xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,E',           type='TH2F', title="T2CaloFastJet ene vs. eta;#eta;ene [MeV]",                  xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EEM',         type='TH2F', title="T2CaloFastJet EM ene vs. eta;#eta;Eem [MeV]",               xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EHAD',        type='TH2F', title="T2CaloFastJet Had ene vs. eta;#eta;Ehad [MeV]",             xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E',           type='TH2F', title="T2CaloFastJet ene vs. phi;#phi;ene [MeV]",                  xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',          type='TH2F', title="T2CaloFastJet Et vs. phi;#phi;Et [MeV]",                    xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]",     xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]",   xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nGrid,UnpackTime',      type='TH2F', title="T2CaloFastJet Unpack time vs. nGrid;nGrid;time [ms]",       xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nGrid;nGrid;time [ms]",  xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nJets;nJets;time [ms]",  xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CalibrationTime', type='TH2F', title="T2CaloFastJet Jet calibration time vs. nJets;nJets;time [ms]",xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,RoITime',         type='TH2F', title="T2CaloFastJet RoI making time vs. nJets;nJets;time [ms]",   xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nGrid;nGrid;time [ms]",        xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nJets;nJets;time [ms]",        xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
        
class TrigT2CaloFastJetCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2CaloFastJet_Cosmic"):
        super(TrigT2CaloFastJetCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
        # this target contains all the histograms necessary for Cosmic
        self.Histograms = [  defineHistogram('nJets',           type='TH1F', title="T2CaloFastJet FEX number of jets;N;nevents",                xbins=21, xmin=-.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('nGrid',           type='TH1F', title="T2CaloFastJet FEX number of inputs;N;nevents",              xbins=100, xmin=0., xmax=1000.) ]
        self.Histograms += [ defineHistogram('Et',              type='TH1F', title="T2CaloFastJet FEX Et;Et [MeV]; nevents",                    xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtEM',            type='TH1F', title="T2CaloFastJet FEX Et EM;Etem [MeV]; nevents",               xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EtHAD',           type='TH1F', title="T2CaloFastJet FEX Et HAD;Ethad [MeV]; nevents",             xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('E',               type='TH1F', title="T2CaloFastJet FEX E;E [MeV]; nevents",                      xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EEM',             type='TH1F', title="T2CaloFastJet FEX Eem;Eem [MeV]; nevents",                  xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('EHAD',            type='TH1F', title="T2CaloFastJet FEX Ehad;Ehad [MeV]; nevents",                xbins=100, xmin=0., xmax=100000.) ]
        self.Histograms += [ defineHistogram('nLeadingTowers',  type='TH1F', title="T2CaloFastJet FEX nLeadingTowers;nLeadingTowers; nevents",  xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Emf',             type='TH1F', title="T2CaloFastJet FEX emf;EMF; nevents",                        xbins=100, xmin=-0.1, xmax=1.1) ]
        self.Histograms += [ defineHistogram('Eta',             type='TH1F', title="T2CaloFastJet FEX Eta;Eta; nevents",                        xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi',             type='TH1F', title="T2CaloFastJet FEX Phi;Phi; nevents",                        xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('UnpackTime',      type='TH1F', title="T2CaloFastJet Unpacking time; time [ms]; nevents",          xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('FastJetTime',     type='TH1F', title="T2CaloFastJet FastJet time; time [ms]; nevents",            xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('puSubTime',       type='TH1F', title="T2CaloFastJet FastJet pu sub time; time [ms]; nevents",     xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CleaningTime',    type='TH1F', title="T2CaloFastJet Cleaning time; time [ms]; nevents",           xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('CalibrationTime', type='TH1F', title="T2CaloFastJet Calibration time; time [ms]; nevents",        xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('RoITime',         type='TH1F', title="T2CaloFastJet RoI making time; time [ms]; nevents",         xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('TotalTime',     type='TH1F', title="T2CaloFastJet Total time; time [ms]; nevents",              xbins=200, xmin=0., xmax=100.) ]
        self.Histograms += [ defineHistogram('Eta,Phi',         type='TH2F', title="T2CaloFastJet phi vs. eta;#eta;#phi",                       xbins=100, xmin=-5., xmax=5.,           ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Et',          type='TH2F', title="T2CaloFastJet Et vs. eta;#eta;Et [MeV]",                    xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #eta;#eta;E_{T}^{em} [MeV]",     xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #eta;#eta;E_{T}^{had} [MeV]",   xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,E',           type='TH2F', title="T2CaloFastJet ene vs. eta;#eta;ene [MeV]",                  xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EEM',         type='TH2F', title="T2CaloFastJet EM ene vs. eta;#eta;Eem [MeV]",               xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Eta,EHAD',        type='TH2F', title="T2CaloFastJet Had ene vs. eta;#eta;Ehad [MeV]",             xbins=100, xmin=-5., xmax=5.,           ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,E',           type='TH2F', title="T2CaloFastJet ene vs. phi;#phi;ene [MeV]",                  xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,Et',          type='TH2F', title="T2CaloFastJet Et vs. phi;#phi;Et [MeV]",                    xbins=64, xmin=-3.1416, xmax=3.1416,    ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtEM',        type='TH2F', title="T2CaloFastJet EM E_{T} vs. #phi;#phi;E_{T}^{em} [MeV]",     xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('Phi,EtHAD',       type='TH2F', title="T2CaloFastJet HAD E_{T} vs. #phi;#phi;E_{T}^{had} [MeV]",   xbins=100, xmin=-3.1416, xmax=3.1416,   ybins=100 , ymin=0., ymax=100000.) ]
        self.Histograms += [ defineHistogram('nGrid,UnpackTime',      type='TH2F', title="T2CaloFastJet Unpack time vs. nGrid;nGrid;time [ms]",       xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nGrid;nGrid;time [ms]",  xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,FastJetTime',     type='TH2F', title="T2CaloFastJet Jet finding time vs. nJets;nJets;time [ms]",  xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,puSubTime',       type='TH2F', title="T2CaloFastJet pu subtraction time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nGrid;nGrid;time [ms]", xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CleaningTime',    type='TH2F', title="T2CaloFastJet Jet cleaning time vs. nJets;nJets;time [ms]", xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,CalibrationTime', type='TH2F', title="T2CaloFastJet Jet calibration time vs. nJets;nJets;time [ms]",xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,RoITime',         type='TH2F', title="T2CaloFastJet RoI making time vs. nJets;nJets;time [ms]",   xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nGrid,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nGrid;nGrid;time [ms]",        xbins=100, xmin=0., xmax=1000., ybins=100, ymin=0., ymax=100.) ]
        self.Histograms += [ defineHistogram('nJets,TotalTime',     type='TH2F', title="T2CaloFastJet Total time vs. nJets;nJets;time [ms]",        xbins=21, xmin=-.5, xmax=20.5, ybins=100, ymin=0., ymax=100.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
class TrigT2VirtualL1CaloJetValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2VirtualL1CaloJet_Validation"):
        super(TrigT2VirtualL1CaloJetValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
# this target contains all the histograms necessary for validation

        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2VirtualL1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2VirtualL1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2VirtualL1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2VirtualL1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2VirtualL1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 
        
class TrigT2VirtualL1CaloJetOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2VirtualL1CaloJet_Online"):
        """ defines hisograms for online """
        super(TrigT2VirtualL1CaloJetOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
# right now this target has the same list of histograms as "Validation"

        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2VirtualL1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2VirtualL1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2VirtualL1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2VirtualL1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2VirtualL1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]
 

class TrigT2VirtualL1CaloJetCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2VirtualL1CaloJet_Cosmic"):
        """ defines hisograms for online """
        super(TrigT2VirtualL1CaloJetCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
# this target contains the same list of histograms as for "Online", but the axis range for plots of Et and etc. are smaller 

        self.Histograms += [ defineHistogram('E', type='TH1F', title="T2VirtualL1CaloJet FEX E;E [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Et', type='TH1F', title="T2VirtualL1CaloJet FEX Et;Et [MeV]; nevents", xbins=100, xmin=0., xmax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="T2VirtualL1CaloJet FEX Eta;Eta; nevents", xbins=100, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="T2VirtualL1CaloJet FEX Phi;Phi; nevents", xbins=64, xmin=-3.1416, xmax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,Phi', type='TH2F', title="T2VirtualL1CaloJet phi vs. eta;#eta;#phi", xbins=100, xmin=-5., xmax=5., ybins=64, ymin=-3.1416, ymax=3.1416) ]
        self.Histograms += [ defineHistogram('Eta,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. eta;#eta;ene [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,E', type='TH2F', title="T2VirtualL1CaloJet ene vs. phi;#phi;ene [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Eta,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. eta;#eta;Et [MeV]", xbins=100, xmin=-5., xmax=5., ybins=100 , ymin=0., ymax=1000000.) ]
        self.Histograms += [ defineHistogram('Phi,Et', type='TH2F', title="T2VirtualL1CaloJet Et vs. phi;#phi;Et [MeV]", xbins=64, xmin=-3.1416, xmax=3.1416, ybins=100 , ymin=0., ymax=1000000.) ]
        #self.Histograms += [ defineHistogram ('AlgorithmErrors', type='TH1F',title='Algorithm Errors bit mask' ,xbins=10,xmin=0,xmax=10)]
        #self.Histograms += [ defineHistogram ('Eta, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Eta;#eta; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=100,xmin=-5.,xmax=5.)]
        #self.Histograms += [ defineHistogram ('Phi, AlgorithmErrors' ,type='TH2F',title='AlgorithmErrors vs Phi;#phi; AlgorithmErrors Errors' ,ybins=6,ymin=0,ymax=6,xbins=64,xmin=-3.1416,xmax=3.1416)]

# T2AllRoiUnpacking histograms    [begin]
fexName = "T2AllRoiUnpacking"
hUnpckTime = defineHistogram(varname='UnpckTime', type='TH1F',
                             title='%s tot unpacking time; time [ms]'%fexName,
                             xbins=40, xmin=0., xmax=40)
hCellUnpckTime = defineHistogram(varname='CellUnpckTime', type='TH1F',
                                 title='%s unpacking time (addCell only); time [ms]'%fexName,
                                 xbins=50, xmin=0., xmax=10)
hRoITime = defineHistogram(varname='RoITime', type='TH1F',
                           title='%s time to setup output TE; time [ms]'%fexName,
                           xbins=50, xmin=0., xmax=10)    
hMergeTime = defineHistogram(varname='MergeTime', type='TH1F',
                             title='%s time to merge the EM cells; time [ms]'%fexName,
                             xbins=50, xmin=0., xmax=10)    
hTotalTime = defineHistogram('TotalTime', type='TH1F',
                             title='%s Total time; time [ms]; nevents'%fexName,
                             xbins=100, xmin=0., xmax=100.)
# T2AllRoiUnpacking histograms    [end]

class T2AllRoiUnpackingTimeOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name = "T2AllRoiUnpackingTimeOnlineMonitoring"):
        super(T2AllRoiUnpackingTimeOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms = [hUnpckTime, hCellUnpckTime, hRoITime, hMergeTime, hTotalTime,]

class T2AllRoiUnpackingTimeValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name = "T2AllRoiUnpackingTimeValidationMonitoring"):
        super(T2AllRoiUnpackingTimeValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        # validation can have additional histograms (online should be the minimal subset)
        self.Histograms = [hUnpckTime, hCellUnpckTime, hRoITime, hMergeTime, hTotalTime,]

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Definitions of the monitoring classes



from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigLoFRemovalValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemoval_Validation"):
        super(TrigLoFRemovalValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
# this target contains all the histograms necessary for validation
       
        self.Histograms =  [ defineHistogram('Et', type='TH1F', title="TrigLoFRemoval Fex Et; E_{T} [MeV]; entries", xbins=30, xmin=0., xmax=600000.) ]
        self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="TrigLoFRemoval FEX Et vs. Eta;#eta;E_{T} [MeV]", xbins=10, xmin=-5., xmax=5., ybins=50, ymin=0, ymax=600000.) ]
        self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="TrigLoFRemoval FEX Et vs. Phi;#phi(rad);E_{T} [MeV]", xbins=32, xmin=-3.2, xmax=3.2, ybins=50, ymin=0, ymax=600000.) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="TrigLoFRemoval FEX NJet; Number of jets; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="TrigLoFRemoval FEX Eta;Eta; nevents", xbins=10, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="TrigLoFRemoval FEX Phi;Phi; nevents", xbins=32, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="TrigLoFRemoval FEX Phi vs. Eta;#eta;#phi(rad)", xbins=10, xmin=-5., xmax=5., ybins=32, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval FEX CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]


class TrigLoFRemovalOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemoval_Online"):
        """ defines hisograms for online """
        super(TrigLoFRemovalOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
# this target contains the list of monitoring histograms to be made online

        self.Histograms =  [ defineHistogram('Et', type='TH1F', title="EF Jet Transverse Energy; Et[MeV]; nevents", xbins=30, xmin=0., xmax=600000.) ]
        self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="EF Jet Transverse Energy vs. Pseudorapidity;#eta;E_{T} [MeV]", 
          xbins=10, xmin=-5., xmax=5., ybins=50, ymin=0, ymax=600000.) ]
        self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="EF Jet Transverse Energy vs. Azimuth;#phi(rad);E_{T} [MeV]", 
          xbins=32, xmin=-3.2, xmax=3.2, ybins=50, ymin=0, ymax=600000.) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="EF Jet Multiplicity per RoI; Number of jets; nevents", 
          xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="EF Jet Pseudorapidity ;#eta; nevents", 
          xbins=10, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="EF Jet Azimuth;#phi(rad); nevents", xbins=32, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="EF Jet Azimuth vs. Pseudorapidity;#eta;#phi(rad)", 
          xbins=10, xmin=-5., xmax=5., ybins=32, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval FEX CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]
       
        
class TrigLoFRemovalCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemoval_Cosmic"):
        """ defines hisograms for cosmics"""
        super(TrigLoFRemovalCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")
# this target contains the same list of histograms as for "Online", but the axis range for plots of Et and etc. are smaller 

        self.Histograms =  [ defineHistogram('Et', type='TH1F', title="TrigLoFRemoval Fex Et; Et [MeV]; nevents", xbins=30, xmin=0., xmax=300000.) ]
        self.Histograms += [ defineHistogram('Eta, Et', type='TH2F', title="TrigLoFRemoval FEX Et vs. Eta;#eta;E_{T} [MeV]", xbins=10, xmin=-5., xmax=5., ybins=30, ymin=0, ymax=300000.) ]
        self.Histograms += [ defineHistogram('Phi, Et', type='TH2F', title="TrigLoFRemoval FEX Et vs. Phi;#phi(rad);E_{T} [MeV]", xbins=32, xmin=-3.2, xmax=3.2, ybins=30, ymin=0, ymax=300000.) ]
        self.Histograms += [ defineHistogram('NJet', type='TH1F', title="TrigLoFRemoval FEX NJet; Number of jets; nevents", xbins=21, xmin=-0.5, xmax=20.5) ]
        self.Histograms += [ defineHistogram('Eta', type='TH1F', title="TrigLoFRemoval FEX Eta;Eta; nevents", xbins=10, xmin=-5., xmax=5.) ]
        self.Histograms += [ defineHistogram('Phi', type='TH1F', title="TrigLoFRemoval FEX Phi;Phi; nevents", xbins=32, xmin=-3.2, xmax=3.2) ]
        self.Histograms += [ defineHistogram('Eta, Phi', type='TH2F', title="TrigLoFRemoval FEX Phi vs. Eta;#eta;#phi(rad)", xbins=10, xmin=-5., xmax=5., ybins=32, ymin=-3.2, ymax=3.2) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval FEX CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]

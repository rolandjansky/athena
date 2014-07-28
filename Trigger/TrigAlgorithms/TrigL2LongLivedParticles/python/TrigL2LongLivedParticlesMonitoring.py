# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#MUON CLUSTER
################ Create Instance for Online Monitoring (basic histograms)
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuonClusterOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muClu_Online"):
        super(TrigMuonClusterOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")
 
        self.Histograms += [ defineHistogram('RoIEta', type='TH1F', title="MuClu: Eta of the muon RoI cluster; eta; nevents", xbins=60, xmin=-3., xmax=3.) ]
        self.Histograms += [ defineHistogram('RoIPhi', type='TH1F', title="MuClu: Phi of the muon RoI cluster; phi; nevents", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931) ]
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
 
##########   Create instance for validation (additional specific histograms)
class TrigMuonClusterValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muClu_Validation"):
        super(TrigMuonClusterValidationMonitoring, self).__init__(name)
 
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('RoIEta', type='TH1F', title="MuClu: Eta of the muon RoI cluster; eta; nevents", xbins=60, xmin=-3., xmax=3.) ]
        self.Histograms += [ defineHistogram('RoIPhi', type='TH1F', title="MuClu: Phi of the muon RoI cluster; phi; nevents", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931) ]
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ] 
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
         
##########   Create instance for cosmics (additional specific histograms)
class TrigMuonClusterCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="muClu_Cosmic"):
        super(TrigMuonClusterCosmicMonitoring, self).__init__(name)
 
        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('RoIEta', type='TH1F', title="MuClu: Eta of the muon RoI cluster; eta; nevents", xbins=60, xmin=-3., xmax=3.) ]
        self.Histograms += [ defineHistogram('RoIPhi', type='TH1F', title="MuClu: Phi of the muon RoI cluster; phi; nevents", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931) ]
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]


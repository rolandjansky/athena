# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

######### MUON CLUSTER HYPO
class MuonClusterHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonClusterHypoOnlineMonitoring"):
        super(MuonClusterHypoOnlineMonitoring, self).__init__(name)
        
        self.defineTarget("Online") 
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
                                    
class MuonClusterHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonClusterHypoValidationMonitoring"):
        super(MuonClusterHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]   
 
class MuonClusterHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonClusterHypoCosmicMonitoring"):
        super(MuonClusterHypoCosmicMonitoring, self).__init__(name)
        
        self.defineTarget("Cosmic")      
        self.Histograms += [ defineHistogram('NumClu',   type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=10, xmin=0., xmax=10.) ]
        self.Histograms += [ defineHistogram('NumJet',   type='TH1F', title="MuClu: Jets with Log(H/E)<0.5 in the muon RoI cluster; number of Jets; nevents", xbins=21, xmin=-1., xmax=20.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuClu: Tracks with Pt>1GeV in the muon RoI cluster; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]


######## HVJetAllCuts HYPO
class TrigL2HVJetHypoAllCutsValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoAllCuts_Validation"):
        super(TrigL2HVJetHypoAllCutsValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        
        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypoAllCuts cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigL2HVJetHypoAllCutsOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoAllCuts_Online"):
        """ defines histograms for online """
        super(TrigL2HVJetHypoAllCutsOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypoAllCuts cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigL2HVJetHypoAllCutsCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoAllCuts_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2HVJetHypoAllCutsCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypoAllCuts cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]



######## HVJet HYPO
class TrigL2HVJetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypo_Validation"):
        super(TrigL2HVJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")


        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ] 

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigL2HVJetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypo_Online"):
        """ defines histograms for online """
        super(TrigL2HVJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigL2HVJetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2HVJetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]
        self.Histograms += [ defineHistogram('HECf', type='TH1F', title="L2JetHypo hecf; hecf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('EMf', type='TH1F', title="L2JetHypo emf; emf; nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('nLeadingCells', type='TH1F', title="L2JetHypo nLeadingCells; nLeadingCells; nevents", xbins=400, xmin=0., xmax=400.) ]
        self.Histograms += [ defineHistogram('fabs_jetQuality', type='TH1F', title="L2JetHypo jetQuality; fabs(jetQuality); nevents", xbins=100, xmin=0., xmax=1.) ]
        self.Histograms += [ defineHistogram('fabs_jetTimeCells', type='TH1F', title="L2JetHypo jetTimeCells; fabs(jetTimeCells)[ns]; nevents", xbins=200, xmin=0., xmax=200.) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]

######## HVJetTrk HYPO
class TrigL2HVJetHypoTrkValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoTrk_Validation"):
        super(TrigL2HVJetHypoTrkValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")


        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]

class TrigL2HVJetHypoTrkOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoTrk_Online"):
        """ defines histograms for online """
        super(TrigL2HVJetHypoTrkOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigL2HVJetHypoTrkCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2HVJetHypoTrk_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigL2HVJetHypoTrkCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('NumberOfTracks', type='TH1F', title="L2HVJetHypo Tracks in ID; number of tracks; events", xbins=20, xmin=0.0, xmax=20.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="L2HVJetHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]



#MuonJet
################ Create Instance for Online Monitoring (basic histograms)
class MuonJetHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonJetHypo_Online"):
        super(MuonJetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms += [ defineHistogram('JetEne',   type='TH1F', title="MuonJet: Jets energy; Jet energy (GeV); nevents", xbins=100, xmin=-1., xmax=500.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuonJet: Tracks with Pt>1GeV in the Jet cone; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
        self.Histograms += [ defineHistogram('deltaR',   type='TH1F', title="MuonJet: DeltaR between LVL1 muon and Jet; deltaR; nevents", xbins=51, xmin=-1., xmax=50.) ]

##########   Create instance for validation (additional specific histograms)
class MuonJetHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonJetHypo_Validation"):
        super(MuonJetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('JetEne',   type='TH1F', title="MuonJet: Jets energy; Jet energy (GeV); nevents", xbins=100, xmin=-1., xmax=500.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuonJet: Tracks with Pt>1GeV in the Jet cone; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
        self.Histograms += [ defineHistogram('deltaR',   type='TH1F', title="MuonJet: DeltaR between LVL1 muon and Jet; deltaR; nevents", xbins=51, xmin=-1., xmax=50.) ]


##########   Create instance for cosmics (additional specific histograms)
class MuonJetHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="MuonJetHypo_Cosmic"):
        super(MuonJetHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms += [ defineHistogram('JetEne',   type='TH1F', title="MuonJet: Jets energy; Jet energy (GeV); nevents", xbins=100, xmin=-1., xmax=500.) ]
        self.Histograms += [ defineHistogram('NumTrk',   type='TH1F', title="MuonJet: Tracks with Pt>1GeV in the Jet cone; number of Tracks; nevents", xbins=51, xmin=-1., xmax=50.) ]
        self.Histograms += [ defineHistogram('deltaR',   type='TH1F', title="MuonJet: DeltaR between LVL1 muon and Jet; deltaR; nevents", xbins=51, xmin=-1., xmax=50.) ]



######## LoF
class TrigLoFRemovalHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemovalHypoOnlineMonitoring"):
        super(TrigLoFRemovalHypoOnlineMonitoring, self).__init__(name)

        cutNames = ['No cell cont', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.defineTarget("Online")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval HYPO CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]

class TrigLoFRemovalHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemovalHypoValidationMonitoring"):
        super(TrigLoFRemovalHypoValidationMonitoring, self).__init__(name)

        cutNames = ['No cell cont', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.defineTarget("Validation")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval HYPO CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]

class TrigLoFRemovalHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigLoFRemovalHypoCosmicMonitoring"):
        super(TrigLoFRemovalHypoCosmicMonitoring, self).__init__(name)

        cutNames = ['No cell cont', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.defineTarget("Cosmic")
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]
        self.Histograms += [ defineHistogram('CellContainerSize', type='TH1F', title="TrigLoFRemoval HYPO CellContainerSize;CellContainerSize; nevents", xbins=100, xmin=0.0, xmax=100.0) ]


######## Calo-ratio HYPO
class TrigCaloRatioHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigCaloRatioHypo_Validation"):
        super(TrigCaloRatioHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="L2HVJetHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="L2HVJetHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="TrigCaloRatio cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigCaloRatioHypoOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="CaloRatioHypo_Online"):
        """ defines histograms for online """
        super(TrigCaloRatioHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="CaloRatioHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="CaloRatioHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="CaloRatioHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]


class TrigCaloRatioHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="CaloRatioHypo_Cosmic"):
        """ defines histograms for cosmic """
        super(TrigCaloRatioHypoCosmicMonitoring, self).__init__(name)

        self.defineTarget("Cosmic")

        self.Histograms = [ defineHistogram('JetEt', type='TH1F', title="CaloRatioHypo Jet Et; Et(GeV); events", xbins=200, xmin=0.0, xmax=200.0) ]
        self.Histograms += [ defineHistogram('logRatio', type='TH1F', title="CaloRatioHypo Log(H/E); Log(H/E); events", xbins=100, xmin=-10.0, xmax=10.0) ]

        cutNames = ['No Jet obj', 'Rejected', 'Accepted']
        labelsDescription = ''
        for a in cutNames:
            labelsDescription += a+':'

        self.Histograms += [ defineHistogram('CutCounter', type='TH1F', title="CaloRatioHypo cut counter", xbins=3, xmin=-1.5, xmax=1.5, labels=labelsDescription) ]

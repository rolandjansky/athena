# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigBjetHypoMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBjetHypoMonitoring"):
        super(TrigBjetHypoMonitoring, self).__init__(name)
         
        self.Histograms = [ defineHistogram('CutCounter', type='TH1F', title="Bjet Hypo cut counter",
                                            xbins=4, xmin=-1.5, xmax=2.5,
                                            labels='No Bjet obj:No usable beam spot:Rejected:Accepted') ]
        self.Histograms += [ defineHistogram('Method', type='TH1F', title="Method used to perform the cut",
                                             xbins=4, xmin=0.0, xmax=4.0,
                                             labels='CHI2:IP2D:IP3D:COMB') ]

class TrigBjetHypoAllTEMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigBjetHypoAllTEMonitoring"):
        super(TrigBjetHypoAllTEMonitoring, self).__init__(name)
         
        self.Histograms = [ defineHistogram('CutCode', type='TH1F', title="Bjet HypoAllTE cut code",
                                            xbins=10, xmin=-1.5, xmax=8.5,
                                            labels='UnInitialzed:Fail:Accept:FailBS:TooFewTEs:TooManyTEs:NoBjets:NoBTagging:NoJet') ]

        self.Histograms = [ defineHistogram('BSCode', type='TH1F', title="Bjet HypoAllTE BS code",
                                            xbins=10, xmin=-1.5, xmax=8.5,
                                            labels='UnInitialzed:ValidBS:BSInvalidReject:BSInValidAccept:NoBSInfo:DontUseBS') ]


        self.Histograms = [ defineHistogram('nInputJets', type='TH1F', title="Bjet HypoAllTE nInputJets",
                                            xbins=100, xmin=-0.5, xmax=99.5,
                                            ) ]



class TrigEFBjetHypoValidationMonitoring(TrigBjetHypoMonitoring):
    def __init__ (self, name="TrigEFBjetHypoValidationMonitoring"):
        super(TrigEFBjetHypoValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")



class TrigEFBjetHypoOnlineMonitoring(TrigBjetHypoMonitoring):
    def __init__ (self, name="TrigEFBjetHypoOnlineMonitoring"):
        super(TrigEFBjetHypoOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")


class TrigEFBjetHypoAllTEValidationMonitoring(TrigBjetHypoAllTEMonitoring):
    def __init__ (self, name="TrigEFBjetHypoAllTEValidationMonitoring"):
        super(TrigEFBjetHypoAllTEValidationMonitoring, self).__init__(name)

        self.defineTarget("Validation")


class TrigEFBjetHypoAllTEOnlineMonitoring(TrigBjetHypoAllTEMonitoring):
    def __init__ (self, name="TrigEFBjetHypoAllTEOnlineMonitoring"):
        super(TrigEFBjetHypoAllTEOnlineMonitoring, self).__init__(name)

        self.defineTarget("Online")


        



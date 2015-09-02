# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################
# EF Electron Track Hypothesis Algorithm Configuration:
# Phillip Urquijo <Phillip.Urquijo@cern.ch>
# (xAOD) Ryan Mackenzie White <ryan.white@cern.ch>
##############################

from TrigEgammaHypo.TrigEgammaHypoConf import TrigEFTrackHypo
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig

####
#### !!!!!!!!!!!!!!!!!!!!!!!!!!
# we have to do something with this, where was this defined before?
from AthenaCommon.SystemOfUnits import GeV

# monitoring
class EFTrackHypo_OnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFTrackHypo_OnlineMonitoring"):
        super(EFTrackHypo_OnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [MeV]",
                                             xbins = 100, xmin=0.0, xmax=50000)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPart',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=50.)]


class EFTrackHypo_ValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="EFTrackHypo_ValidationMonitoring"):
        super(EFTrackHypo_ValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")

        self.Histograms += [ defineHistogram('pt',
                                             type='TH1F',
                                             title="Track p_{T} (GeV); p_{T} [MeV]",
                                             xbins = 100, xmin=0.0, xmax=50000)]
        self.Histograms += [ defineHistogram('Phi',
                                             type='TH1F',
                                             title="Phi of track; #phi",
                                             xbins = 100, xmin=-3.2, xmax=3.2)]
        self.Histograms += [ defineHistogram('d0',
                                             type='TH1F',
                                             title="d_{0} of track; d_{0}",
                                             xbins = 100, xmin=-1.5, xmax=1.5)]
        self.Histograms += [ defineHistogram('z0',
                                             type='TH1F',
                                             title="z_{0} of track; z_{0}",
                                             xbins = 100, xmin=-100, xmax=100)]
        self.Histograms += [ defineHistogram('numTrkPart',
                                             type='TH1F',
                                             title="Number of input Tracks; Number of tracks",
                                             xbins = 100, xmin=0., xmax=50.)]
      
       

#-------------------------------------------
# --- DC14 Menu
# --- Following classes to be used with TriggerMenu
# --- Rely on TM to configure 
# --- RMWhite 2014
# --- Comment, no track variable cuts defined in any hypo. 
# --- No Cut only -- threshold passed from TM chain name
class EFTrackHypo_e_NoCut (TrigEFTrackHypo):
    __slots__ = []
    def __init__(self, name,threshold):
        super( EFTrackHypo_e_NoCut, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = True
        self.AthenaMonTools = [ EFTrackHypo_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]

# Track pt cut
# No threshold set
class EFTrackHypo_e (TrigEFTrackHypo):
    __slots__ = []
    def __init__(self, name,threshold):
        super( EFTrackHypo_e, self ).__init__( name )

        # AcceptAll flag: if true take events regardless of cuts
        self.AcceptAll = False
        self.AthenaMonTools = [ EFTrackHypo_ValidationMonitoring(), TrigTimeHistToolConfig("Time") ]
#-------------------------------------------


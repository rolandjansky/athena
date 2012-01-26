# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc
from TrigMonitorBase.TrigGenericMonitoringToolConfig import *
from TrigTimeMonitor.TrigTimeHistToolConfig import *
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class CosmicTrtHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
   def __init__ (self, name="CosmicTrtHypoCosmicMonitoring"):
       super(CosmicTrtHypoCosmicMonitoring, self).__init__(name)
       self.defineTarget("Cosmic")
       self.Histograms += [ defineHistogram('numtracks_hypo',
                                            type='TH1F',
                                            title="Number of tracks after hypo",
                                            xbins = 20, xmin=0.0, xmax=20)]

       self.Histograms += [ defineHistogram('nStraw_TRT_hypo','TH1F','EXPERT',
                                            name+' NStraw_TRT hits of tracks after hypo', 100, 0.0,100)]
       self.Histograms += [ defineHistogram('nTR_TRT_hypo','TH1F','EXPERT',
                                            name+' NTR_TRT hits of tracks after hypo', 100, 0.0,100)]
       
       self.Histograms += [
          defineHistogram('AcceptedTrack_a0', 'TH1F', 'EXPERT',
                          name+' Accepted segment a0', 100, -1000, 1000), 
          defineHistogram('AcceptedTrack_pt', 'TH1F', 'EXPERT',
                          name+' Accepted segment pt ',
                          100, 0, 40000),
          defineHistogram('AcceptedTrack_phi0', 'TH1F', 'EXPERT',
                          name+' Accepted segment #phi0', 100, -3.2, 3.2),
          defineHistogram('AcceptedTrack_z0', 'TH1F', 'EXPERT',
                          name+' Accepted segment z0', 100, -1000, 1000),
          defineHistogram('AcceptedTrack_eta', 'TH1F', 'EXPERT',
                          name+' Accepted segment #eta', 100, -5, 5), 
          ]
       
      
       
       
       

class CombinedIDHTHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name='CombinedIDHTHypoCosmic'):
        super(CombinedIDHTHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget('Cosmic')
        self.Histograms += [ defineHistogram('HistIDdPhi_Accepted',
                                             type='TH1F',
                                             title="Accepted track #phi (rad.)",
                                             xbins = 32, xmin=-3.15, xmax=3.15)]
        self.Histograms += [
            defineHistogram('HistIDdEta_Accepted', 'TH1F', 'EXPERT',
                            name+'Accepted track #eta ',
                            32, -3,3)]
        
        
class CombinedIDMuonHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name='CombinedIDMuonHypoCosmic'):
        super(CombinedIDMuonHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget('Cosmic')
        self.Histograms += [
            defineHistogram('HistIDMuondPhi_Accepted', 'TH1F', 'EXPERT',
                            name+'Accepted track #phi (rad.)',
                            32, -3.1415926535897931, 3.1415926535897931),
            defineHistogram('HistIDMuondTheta_Accepted', 'TH1F', 'EXPERT',
                            name+'Accepted track #theta ',
                            32, -3.1415926535897931,3.1415926535897931),
            defineHistogram('HistIDMuondEta_Accepted', 'TH1F', 'EXPERT',
                            name+'Accepted track #eta ',
                            32, -3,3)
            ]
        
        
        
class CosmicMuonHypoCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name='CosmicMuonHypoCosmic'):
        super(CosmicMuonHypoCosmicMonitoring, self).__init__(name)
        self.defineTarget(['Cosmic', 'Online'])
        self.Histograms += [
            defineHistogram('AcceptedMuonEta', 'TH1F', 'EXPERT',
                            name+'Accepted muon #eta', 60, -3, 3), 
            defineHistogram('AcceptedMuonPhi', 'TH1F', 'EXPERT',
                            name+'Accepted muon #phi (rad.)',
                            32, -3.1415926535897931, 3.1415926535897931),
            defineHistogram('AcceptedMuonDCA_XY', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{XY} (cm)', 50, 0, 1000.0),
            defineHistogram('AcceptedMuonDCA_Z', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{Z} (cm)',
                            100, -2000.0, 2000.0),
            defineHistogram('AcceptedMuonDCA_3D', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{3D} (cm)',
                            50, 0.0, 1000.0)
            ]

class CosmicMuonHypoValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__(self, name='CosmicMuonHypoValidation'):
        super(CosmicMuonHypoValidationMonitoring, self).__init__(name)
        self.defineTarget('Validation')
        self.Histograms += [
            defineHistogram('AcceptedMuonEta', 'TH1F', 'EXPERT',
                            name+'Accepted muon #eta', 60, -3, 3), 
            defineHistogram('AcceptedMuonPhi', 'TH1F', 'EXPERT',
                            name+'Accepted muon #phi (rad.)',
                            32, -3.1415926535897931, 3.1415926535897931),
            defineHistogram('AcceptedMuonDCA_XY', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{XY} (cm)', 50, 0, 1000.0),
            defineHistogram('AcceptedMuonDCA_Z', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{Z} (cm)',
                            100, -2000.0, 2000.0),
            defineHistogram('AcceptedMuonDCA_3D', 'TH1F', 'EXPERT',
                            name+'Accepted muon DCA_{3D} (cm)',
                            50, 0.0, 1000.0)
            ]

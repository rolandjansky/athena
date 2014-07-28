# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" T2TauFinal  Monitoring """

__author__  = 'O.Igonkina'
__version__=""
__doc__="Implementation of T2TauFinal  Monitoring"

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig 

class T2TauFinalOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="T2TauFinalOnlineMonitoring"):
        super(T2TauFinalOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        
    def setDefaults(self, handle):
        super(TrigGenericMonitoringToolConfig, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
            myName = "SomeAlgo"
        else:
            myName=handle.getName()
            
        if hasattr( handle, 'Histograms' ) and not 'EtCombined' in handle.Histograms:
            handle.Histograms +=  [ defineHistogram('EtCombined',         type='TH1F', title="L2Tau FEX TrigTau Et (Combined);EtCombined; nevents",
                                                    xbins=54, xmin=-12000,  xmax=150000) ]
            
        if hasattr( handle, 'Histograms' ) and not 'SimpleEtFlow' in handle.Histograms:
            handle.Histograms +=  [ defineHistogram('SimpleEtFlow',       type='TH1F', title="L2Tau FEX TrigTau vec(tracks+EM0+EM2);EtFlow; nevents",
                                                    xbins=54, xmin=-12000., xmax=150000.) ]
                   
        if hasattr( handle, 'Histograms' ) and not 'NMatchedTracks' in handle.Histograms:
            handle.Histograms += [ defineHistogram('NMatchedTracks',      type='TH1F', title="L2Tau FEX N Matched Tracks;N Matched Tracks; nevents",
                                                   xbins=16, xmin=-0.5, xmax=15.5) ]

        if hasattr( handle, 'Histograms' ) and not 'DeltaEtaTkClust' in handle.Histograms:
           handle.Histograms += [ defineHistogram('DeltaEtaTkClust',      type='TH1F', title="L2Tau FEX #Delta #eta between tracks and cluster;#Delta #eta; nevents",
                                                  xbins=80, xmin=-0.4, xmax=0.4) ]

        if hasattr( handle, 'Histograms' ) and not 'DeltaPhiTkClust' in handle.Histograms:
           handle.Histograms += [ defineHistogram('DeltaPhiTkClust',      type='TH1F', title="L2Tau FEX #Delta #varphi between tracks and cluster;#Delta #varphi; nevents",
                                                  xbins=100, xmin=-0.4, xmax=0.4) ]

        ## if hasattr( handle, 'Histograms' ) and not 'EtCalibCluster' in handle.Histograms:
##            handle.Histograms += [ defineHistogram('EtCalibCluster',       type='TH1F', title="L2Tau FEX EtCalibCluster;;EtCalibCluster nevents",
##                                                   xbins=54, xmin=-12000., xmax=150000.) ]
        if hasattr( handle, 'Histograms' ) and not 'dEtaTrigTau_cluster' in handle.Histograms:
           handle.Histograms += [ defineHistogram('dEtaTrigTau_cluster',  type='TH1F', title="L2CaloTau FEX Eta_TrigTau - Eta_TauCluster; #Delta#eta; nevents",
                                                  xbins=40, xmin=-0.4, xmax=0.4) ]
        if hasattr( handle, 'Histograms' ) and not 'dPhiTrigTau_cluster' in handle.Histograms:
            handle.Histograms += [ defineHistogram('dPhiTrigTau_cluster', type='TH1F', title="L2CaloTau FEX Phi_TrigTau - Phi_TauCluster; #Delta#phi; nevents",
                                                   xbins=40, xmin=-0.4, xmax=0.4) ]
        if hasattr( handle, 'Histograms' ) and not 'DeltaRTkClust' in handle.Histograms:
            handle.Histograms += [ defineHistogram('DeltaRTkClust',       type='TH1F', title="L2Tau FEX distance between tracks and cluster;#sqrt{#Delta #varphi^{2} + #Delta #eta^{2}}; nevents",
                                                   xbins=50, xmin=0, xmax=0.5) ]

        if hasattr( handle, 'Histograms' ) and not 'Eta_Vs_Phi' in handle.Histograms:
            handle.Histograms += [ defineHistogram('Eta, Phi',             type='TH2F', title="T2TauFinal Tau Eta vs Phi; #eta; #varphi; nevents",
                                                   xbins=51, xmin=-2.55, xmax=2.55,
                                                   ybins=65, ymin=-3.1415936-0.098174/2., ymax=3.1415936+0.098174/2.)]
            
        if hasattr( handle, 'Histograms' ) and not 'trkAvgDist' in handle.Histograms:
            handle.Histograms += [ defineHistogram('trkAvgDist',             type='TH1F', title="T2TauFinal trkAvgDist; trkAvgDist; nevents",
                                                   xbins=41, xmin=-0.01, xmax=0.4)]
            
        if hasattr( handle, 'Histograms' ) and not 'etOverPtLeadTrk' in handle.Histograms:
            handle.Histograms += [ defineHistogram('etOverPtLeadTrk',             type='TH1F', title="T2TauFinal etOverPtLeadTrk; etOverPtLeadTrk; nevents",
                                                   xbins=41, xmin=-0.5, xmax=20)]

        

########## add validation specific histograms 
class T2TauFinalValidationMonitoring(T2TauFinalOnlineMonitoring):
    def __init__ (self, name="T2TauFinalValidationMonitoring"):
        super(T2TauFinalValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        
    def setDefaults(self, handle):
        super(T2TauFinalValidationMonitoring, self).setDefaults( handle)
        if not hasattr( handle, 'name' ):
           myName = "SomeAlgo"
        else:
           myName=handle.getName()
           
        

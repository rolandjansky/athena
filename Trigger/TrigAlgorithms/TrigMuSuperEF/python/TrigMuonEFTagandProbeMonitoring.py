# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration                                                       

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

class TrigMuonEFTagandProbeMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMuonEFTagandProbeMonitoring"):
        super(TrigMuonEFTagandProbeMonitoring, self).__init__(name)
        self.defineTarget( ["Online", "Validation"] ) 


#Profile Histograms for producing efficiency plots online - built by threshold number

        for i in range(1,7):

            #Phi barrel
            self.Histograms += [ defineHistogram('EF_Phi_Total_thr' + str(i) + '_b, EF_Etaphi_Eff_thr' + str(i) + '_b', type='TProfile', title="L1 Trigger Efficiency by #phi in threshold " + str(i) + " Barrel;Muon #phi [Rad];Efficiency",
                                                 xbins=35, xmin=-3.5, xmax=3.5) ]
            #Phi endcap
            self.Histograms += [ defineHistogram('EF_Phi_Total_thr' + str(i) + '_e, EF_Etaphi_Eff_thr' + str(i) + '_e', type='TProfile', title="L1 Trigger Efficiency by #phi in threshold " + str(i) + " Endcap;Muon #phi [Rad];Efficiency",
                                                 xbins=35, xmin=-3.5, xmax=3.5) ]


#Additional Profile histograms for combined barrel and endcap eta plots - built by threshold number

            #Eta combined barrel + endcap
            self.Histograms += [ defineHistogram('EF_Eta_Total_thr' + str(i) + ', EF_Eta_Eff_thr' + str(i) + '', type='TProfile', title="L1 Trigger Efficiency by #eta in threshold " + str(i) + ";Muon #eta;Efficiency",
                                                 xbins=35, xmin=-3.5, xmax=3.5) ]


#2D Profile Histograms for producing efficiency plots of eta v phi online - built by threshold number ([31/05/18] only using thresholds 1,5, and 6 to limit memory allocation, thresholds 5&6 are identical in endcap)

        for i in [1,5,6]:
            #Eta + Phi barrel
            self.Histograms += [ defineHistogram('EF_Eta_Total_thr' + str(i) + '_b, EF_Phi_Total_thr' + str(i) + '_b, EF_Etaphi_Eff_thr' + str(i) + '_b', type='TProfile2D', title="L1 Trigger Efficiency in #eta and #phi Threshold " + str(i) + " Barrel;Muon #eta ;Muon #phi",
                                                 xbins=11, xmin=-1.1, xmax=1.1, ybins=8, ymin=-3.4, ymax=3.4, zmin=-0.1,zmax=1.1) ]
        for i in [1,6]:
            #Eta + Phi endcap
            self.Histograms += [ defineHistogram('EF_Eta_Total_thr' + str(i) + '_e, EF_Phi_Total_thr' + str(i) + '_e, EF_Etaphi_Eff_thr' + str(i) + '_e', type='TProfile2D', title="L1 Trigger Efficiency in #eta and #phi Threshold " + str(i) + " Endcap;Muon #eta ;Muon #phi",
                                                 xbins=27, xmin=-2.7, xmax=2.7, ybins=8, ymin=-3.4, ymax=3.4, zmin=-0.1,zmax=1.1) ]

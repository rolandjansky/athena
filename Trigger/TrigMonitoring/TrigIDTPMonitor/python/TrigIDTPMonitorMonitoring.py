# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig


class TrigIDTPMonitorMonitorBase(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigIDTPMonitorMonitorBase"):
        super(TrigIDTPMonitorMonitorBase, self).__init__(name)
        #self.defineTarget("Validation")
                
        self.Histograms += [ defineHistogram('deltaMass', type='TH1F', title="TrigIDTPMonitor mass difference of t&p to Z mass; deltaM [MeV]",
                                             xbins=50, xmin=-20000, xmax=20000) ]
        
        self.Histograms += [ defineHistogram('ID-TagMass', type='TH1F', title="TrigIDTPMonitor InvMass PT ID and Tag; calcinvMass [MeV]",
                                             xbins=20, xmin=60000, xmax=130000) ]
        
        self.Histograms += [ defineHistogram('Efficiencies',type='TH1F', title="TrigIDTPMonitor Overall Eff; strat",
                                             xbins=3, xmin=0, xmax=3, labels="Probe Muon: Precision Tracking: FastTrackFinder") ]
        
        ######Profile Histograms######
        self.Histograms += [ defineHistogram('InvMassD, PTfound', type='TProfile', title="TrigIDTPMonitor InvMass of BPhysObject PT Efficiency; M_{#mu#mu} [MeV]",
                                             xbins=25, xmin=80000, xmax=100000) ]

        self.Histograms += [ defineHistogram('InvMassTPD, PTfound', type='TProfile', title="TrigIDTPMonitor Calculated InvMass of Tag and Probe PT Efficiency; M_{#mu#mu} [MeV]",
                                             xbins=25, xmin=80000, xmax=100000) ]

        self.Histograms += [ defineHistogram('ProbePTD, PTfound', type="TProfile", title="TrigIDTPMonitor Probe pT PT Efficiency; p_{T} [MeV]",
                                             xbins=30, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram('ProbeEtaD, PTfound', type="TProfile", title="TrigIDTPMonitor Probe eta PT Efficiency; #eta",
                                             xbins=15, xmin=-3, xmax=3) ]

        self.Histograms += [ defineHistogram('ProbeD0D, PTfound', type="TProfile", title="TrigIDTPMonitor Probe d0 PT Efficiency; d0 [mm]",
                                             xbins=16, xmin=-4, xmax=4) ]
        
        self.Histograms += [ defineHistogram('ProbePhiD, PTfound', type="TProfile", title="TrigIDTPMonitor Probe Phi PT Efficiency; #phi [rad]",
                                             xbins=15, xmin=-3.2, xmax=3.2) ]

        self.Histograms += [ defineHistogram('InvMassD, FTFfound', type='TProfile', title="TrigIDTPMonitor InvMass of BPhysObject FTF Efficiency; M_{#mu#mu} [MeV]",
                                             xbins=25, xmin=80000, xmax=100000) ]

        self.Histograms += [ defineHistogram('InvMassTPD, FTFfound', type="TProfile", title="TrigIDTPMonitor Calculated InvMass of Tag and Probe FTF Efficiency; M_{#mu#mu} [MeV]",
                                             xbins=25, xmin=80000, xmax=100000) ]

        self.Histograms += [ defineHistogram('ProbePTD, FTFfound', type="TProfile", title="TrigIDTPMonitor Probe pT FTF Efficiency; p_{T} [MeV]",
                                             xbins=20, xmin=0, xmax=100000) ]

        self.Histograms += [ defineHistogram('ProbeEtaD, FTFfound', type="TProfile", title="TrigIDTPMonitor Probe eta FTF Efficiency; #eta",
                                             xbins=15, xmin=-3, xmax=3) ]

        self.Histograms += [ defineHistogram('ProbeD0D, FTFfound', type="TProfile", title="TrigIDTPMonitor Probe d0 FTF Efficiency; d0 [mm]",
                                             xbins=16, xmin=-4, xmax=4) ]
        
        self.Histograms += [ defineHistogram('ProbePhiD, FTFfound', type="TProfile", title="TrigIDTPMonitor Probe Phi FTF Efficiency; #phi [rad]",
                                             xbins=15, xmin=-3.2, xmax=3.2) ]


class TrigIDTPMonitorOnlineMonitor(TrigIDTPMonitorMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorOnlineMonitor"):
        super(TrigIDTPMonitorOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")

class TrigIDTPMonitorValidationMonitor(TrigIDTPMonitorMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorValidationMonitor"):
        super(TrigIDTPMonitorValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")
##############################################################################################################################################################################################################################
#For Z->ee monitoring class
class TrigIDTPMonitorElectronMonitorBase(TrigGenericMonitoringToolConfig):

    def __init__ (self, name="TrigIDTPMonitorElectronMonitorBase"):
        super(TrigIDTPMonitorElectronMonitorBase, self).__init__(name)
        #self.defineTarget("Validation")
        #Minv of tag electron - probe track (PT and FTF)
        #hists created for all probe tracks and for optimal track in probe region only
        self.Histograms += [ defineHistogram('PTInvMass', type='TH1F', title="InvMass of Tag-Probe pair (PT track); invMass [MeV]",
                                             xbins=50, xmin=0, xmax=200000)]
        self.Histograms += [ defineHistogram('PTOptInvMass', type='TH1F', title="InvMass of Tag-Probe pair (Optimum PT track); invMass [MeV]",
                                             xbins=50, xmin=0, xmax=200000)]
        self.Histograms += [ defineHistogram('FTFInvMass', type='TH1F', title="InvMass of Tag-Probe pair (FTF track); invMass [MeV]",
                                             xbins=50, xmin=0, xmax=200000)]
        self.Histograms += [ defineHistogram('FTFOptInvMass', type='TH1F', title="InvMass of Tag-Probe pair (Optimum FTF track); invMass [MeV]",
                                             xbins=50, xmin=0, xmax=200000)]

        #Minv of tag track - probe track pair (PT and FTF)
        self.Histograms += [ defineHistogram('PTInvMassTrack', type='TH1F', title="InvMass of TagTrack-Probe pair (PT track); invMass [MeV]",
                                             xbins=50, xmin=0, xmax=200000)]
        self.Histograms += [ defineHistogram('FTFInvMassTrack', type='TH1F', title="InvMass of TagTrack-Probe pair (FTF track); invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000)]

        #Minv of tag electron - probe cluster pair (before and after Z->ee selection cuts)
        self.Histograms += [ defineHistogram('InvMassCluster', type='TH1F', title="InvMass of Tag-Probe pair (Probe Cluster); invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000)]
        self.Histograms += [ defineHistogram('InvMassCluster_before_cuts', type='TH1F', title="InvMass of Tag-Probe pair (Probe Cluster before cuts); invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000)]

        #Minv of tag track - probe cluster pair (before and after Z->ee selection cuts)
        self.Histograms += [ defineHistogram('InvMassClusterTagTrack', type='TH1F', title="InvMass of TagTrack-Probe pair (Probe Cluster); invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000)]
        self.Histograms += [ defineHistogram('InvMassClusterTagTrack_before_cuts', type='TH1F', title="InvMass of TagTrack-Probe pair (Probe Cluster before cuts); invMass [MeV]",
                                             xbins=30, xmin=65000, xmax=115000)]

        #deltaR between probe track and probe cluster (PT and FTF)
        #hists created for all probe tracks and for optimal track in probe region only
        self.Histograms += [ defineHistogram('PTdeltaR', type='TH1F', title="Difference in R between all probe track (PT) and probe cluster; dR",
                                             xbins=50, xmin=-0.01, xmax=0.25)]
        self.Histograms += [ defineHistogram('PTOptdeltaR', type='TH1F', title="Difference in R between optimum probe track (PT) and probe cluster; dR",
                                             xbins=50, xmin=-0.01, xmax=0.25)]
        self.Histograms += [ defineHistogram('FTFdeltaR', type='TH1F', title="Difference in R between all probe track (FTF) and probe cluster; dR",
                                             xbins=50, xmin=-0.01, xmax=0.25)]
        self.Histograms += [ defineHistogram('FTFOptdeltaR', type='TH1F', title="Difference in R between optimum probe track (FTF) and probe cluster; dR",
                                             xbins=50, xmin=-0.01, xmax=0.25)]

        #Cluster topological info (pT, phi and eta - before and after Z->ee selection cuts)
        #probe clusters
        self.Histograms += [ defineHistogram('clusterPT', type='TH1F', title="Transverse momentum of cluster; pT [MeV]",
                                             xbins=30, xmin=0, xmax=115000)]
        self.Histograms += [ defineHistogram('clusterPT_before_cuts', type='TH1F', title="Transverse momentum of cluster (before cuts); pT [MeV]",
                                             xbins=30, xmin=0, xmax=115000)]
        self.Histograms += [ defineHistogram('clusterPhi', type='TH1F', title="Phi of cluster; phi [radians]",
                                             xbins=30, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('clusterPhi_before_cuts', type='TH1F', title="Phi of cluster (before cuts); phi [radians]",
                                             xbins=30, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('clusterEta', type='TH1F', title="Eta of cluster; eta [radians]",
                                             xbins=50, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('clusterEta_before_cuts', type='TH1F', title="Eta of cluster (before cuts); eta [radians]",
                                             xbins=50, xmin=-3.5, xmax=3.5)]
        #tag clusters (phi and eta - before and after Z->ee selection cuts)
        self.Histograms += [ defineHistogram('tagClusterPhi', type='TH1F', title="Phi of tag cluster; phi [radians]",
                                             xbins=30, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('tagClusterPhi_before_cuts', type='TH1F', title="Phi of tag cluster (before cuts); phi [radians]",
                                             xbins=30, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('tagClusterEta', type='TH1F', title="Eta of tag cluster; eta [radians]",
                                             xbins=50, xmin=-3.5, xmax=3.5)]
        self.Histograms += [ defineHistogram('tagClusterEta_before_cuts', type='TH1F', title="Eta of tag cluster (before cuts); eta [radians]",
                                             xbins=50, xmin=-3.5, xmax=3.5)]

        #Difference in phi between tag and probe (before and after selection cuts)
        self.Histograms += [ defineHistogram('deltaPhi', type='TH1F', title="Difference in phi between tag electron and probe cluster; delta phi [radians]",
                                             xbins=30, xmin=0, xmax=6.4)]
        self.Histograms += [ defineHistogram('deltaPhi_before_cuts', type='TH1F', title="Difference in phi between tag electron and probe cluster (before cuts); delta phi [radians]",
                                             xbins=30, xmin=0, xmax=6.4)]


        #Difference in eta between track and cluster
        #Probe (PT and FTF)
        self.Histograms += [ defineHistogram('PTClusterDeltaEta', type='TH1F', title="Difference in eta between probe track (PT) and probe cluster; delta eta [radians]",
                                             xbins=30, xmin=-0.2, xmax=0.2)]
        self.Histograms += [ defineHistogram('FTFClusterDeltaEta', type='TH1F', title="Difference in eta between probe track (FTF) and probe cluster; delta eta [radians]",
                                             xbins=30, xmin=-0.2, xmax=0.2)]
        #Tag
        self.Histograms += [ defineHistogram('TagDeltaEta', type='TH1F', title="Difference in eta between tag track and tag cluster; delta eta [radians]",
                                             xbins=30, xmin=-0.2, xmax=0.2)]

        #Difference in phi between track and cluster
        #Probe (PT and FTF)
        self.Histograms += [ defineHistogram('PTClusterDeltaPhi', type='TH1F', title="Difference in phi between probe track (PT) and probe cluster; delta phi [radians]",
                                             xbins=50, xmin=-0.1, xmax=0.1)]
        self.Histograms += [ defineHistogram('FTFClusterDeltaPhi', type='TH1F', title="Difference in phi between probe track (FTF) and probe cluster; delta phi [radians]",
                                             xbins=50, xmin=-0.1, xmax=0.1)]
        #Tag (measured distribution)
        self.Histograms += [ defineHistogram('TagDeltaPhi', type='TH1F', title="Difference in phi between tag track and tag cluster; delta phi [radians]",
                                             xbins=50, xmin=-0.05, xmax=0.05)]
        #Tag (expected distribution due to bending in magnetic field)
        self.Histograms += [ defineHistogram('TagExpectedDeltaPhi', type='TH1F', title="Expected Difference in phi between tag track and tag cluster; delta phi [radians]",
                                             xbins=50, xmin=-0.05, xmax=0.05)]

        #Tag topological information
        self.Histograms += [ defineHistogram('tagpT', type='TH1F', title="Transverse momentum of tag; pT [MeV]",
                                             xbins=30, xmin=10000, xmax=100000)]
        self.Histograms += [ defineHistogram('tagTrackpT', type='TH1F', title="Transverse momentum of tag track; pT [MeV]",
                                             xbins=30, xmin=10000, xmax=100000)]
        self.Histograms += [ defineHistogram('tagPoverE', type='TH1F', title="Ratio P(tagTrack)/E(tagElectron)",
                                             xbins=50, xmin=0, xmax=1)]

        #Difference in z0 between tag and probe (PT) tracks for efficient candidates only
        self.Histograms += [ defineHistogram('PTEfficientCombDeltaZ0', type='TH1F', title="dZ0 between tag and probe (PT) tracks for efficient events (combined criteria); dz0 [mm]",
                                             xbins=100, xmin=-1, xmax=1)] 

        #Number of hits in inner detectors
        self.Histograms += [ defineHistogram('PTnumberOfPixelHits', type='TH1F', title="Number of hits in pixel detector (Probe PT); Hits",
                                             xbins=8, xmin=0, xmax=8)]
        self.Histograms += [ defineHistogram('PTnumberOfSCTHits', type='TH1F', title="Number of hits in SCT detector (Probe PT); Hits",
                                             xbins=16, xmin=0, xmax=16)]
        self.Histograms += [ defineHistogram('PTnumberOfTRTHits', type='TH1F', title="Number of hits in TRT detector (Probe PT); Hits",
                                             xbins=30, xmin=0, xmax=60)]

        #2D - Minv of tag electron-probe track (PT) vs the difference in phi between probe track and probe cluster (for positrons only)
        self.Histograms += [ defineHistogram('PTOptInvMass,PTClusterDeltaPhiPositron', type='TH2F', title="PT Optimum Track Invariant Mass (positrons) vs dPhi; Minv [MeV]; dPhi",
                                             xbins=50, xmin=0, xmax=200000,
                                             ybins=50, ymin=-0.1, ymax=0.1)]                             

        
        ######Profile Histograms######
        #PRECISION TRACKER
        #1)Minv Criteria - 40 Gev < Minv < 200 GeV
        #Efficiency distributions in bins of: 'probe cluster' pT, phi, eta, Minv and #pixel, #SCT, #TRT and tagZ0
        self.Histograms += [ defineHistogram('probePtEfficiency, PTfound', type='TProfile', title="PT Probe Cluster Transverse Momentum Efficiency Profile; pT [MeV]",
                                             xbins=15, xmin=10000, xmax=90000,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probePhiEfficiency, PTfound', type='TProfile', title="PTProbe Cluster Phi Efficiency Profile; phi [radians]",
                                             xbins=9, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeEtaEfficiency, PTfound', type='TProfile', title="PT Probe Cluster Eta Efficiency Profile; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeInvMassEfficiency, PTfound', type='TProfile', title="PT Probe Cluster Invariant Mass Efficiency Profile; Minv [MeV]",
                                             xbins=20, xmin=80000, xmax=100000,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('PTnumberOfPixelHits, PTfound', type='TProfile', title="PT Number of hits in pixel detector Efficiency Profile; Hits",
                                             xbins=8, xmin=0, xmax=8,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('PTnumberOfSCTHits, PTfound', type='TProfile', title="PT Number of hits in SCT detector Efficiency Profile; Hits",
                                             xbins=16, xmin=0, xmax=16,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('PTnumberOfTRTHits, PTfound', type='TProfile', title="PT Number of hits in TRT detector Efficiency Profile; Hits",
                                             xbins=30, xmin=0, xmax=60,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('tagZ0, PTfound', type='TProfile', title="Tag Z0 Efficiency Profile; Z0 [mm]",
                                             xbins=40, xmin=-100, xmax=100,
                                             ymin=-0.1, ymax=1.1) ]

        #2)deltaR Criteria - dR < 0.1
        #Efficiency distributions in bins of probe cluster: pT, phi, eta and Minv
        self.Histograms += [ defineHistogram('probePtEfficiency, PTfoundAlt', type='TProfile', title="ALTERNATIVE:PT Probe Cluster Transverse Momentum Efficiency Profile; pT [MeV]",
                                             xbins=15, xmin=10000, xmax=90000,
                                             ymin=-0.1, ymax=1.1) ]

        self.Histograms += [ defineHistogram('probePhiEfficiency, PTfoundAlt', type='TProfile', title="ALTERNATIVE:PTProbe Cluster Phi Efficiency Profile; phi [radians]",
                                             xbins=9, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeEtaEfficiency, PTfoundAlt', type='TProfile', title="ALTERNATIVE:PT Probe Cluster Eta Efficiency Profile; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeInvMassEfficiency, PTfoundAlt', type='TProfile', title="ALTERNATIVE:PT Probe Cluster Invariant Mass Efficiency Profile; Minv [MeV]",
                                             xbins=20, xmin=80000, xmax=100000,
                                             ymin=-0.1, ymax=1.1) ]

        #3)Combined Criteria - 40 Gev < Minv < 200 GeV || dR < 0.1
        #Efficiency distributions in bins of probe cluster: pT, phi, eta and Minv
        self.Histograms += [ defineHistogram('probePtEfficiency, PTfoundCombined', type='TProfile', title="COMBINED:PT Probe Cluster Transverse Momentum Efficiency Profile; pT [MeV]",
                                             xbins=15, xmin=10000, xmax=90000,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probePhiEfficiency, PTfoundCombined', type='TProfile', title="COMBINED:PTProbe Cluster Phi Efficiency Profile; phi [radians]",
                                             xbins=9, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeEtaEfficiency, PTfoundCombined', type='TProfile', title="COMBINED:PT Probe Cluster Eta Efficiency Profile; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeInvMassEfficiency, PTfoundCombined', type='TProfile', title="COMBINED:PT Probe Cluster Invariant Mass Efficiency Profile; Minv [MeV]",
                                             xbins=20, xmin=80000, xmax=100000,
                                             ymin=-0.1, ymax=1.1) ]

        #FAST TRACK FINDER
        #1)Combined Criteria - 40 Gev < Minv < 200 GeV || dR < 0.1
        #Efficiency distributions in bins of probe cluster: pT, phi, eta and Minv
        self.Histograms += [ defineHistogram('probePtEfficiency, FTFfound', type='TProfile', title="FTF Probe Cluster Transverse Momentum Efficiency Profile; pT [MeV]",
                                             xbins=15, xmin=10000, xmax=90000,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probePhiEfficiency, FTFfound', type='TProfile', title="FTF Probe Cluster Phi Efficiency Profile; phi [radians]",
                                             xbins=9, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeEtaEfficiency, FTFfound', type='TProfile', title="FTF Probe Cluster Eta Efficiency Profile; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('probeInvMassEfficiency, FTFfound', type='TProfile', title="FTF Probe Cluster Invariant Mass Efficiency Profile; Minv [MeV]",
                                             xbins=20, xmin=80000, xmax=100000,
                                             ymin=-0.1, ymax=1.1) ]
        

        #2D PROFILE HISTOGRAMS TO ALLOW FOR MONITORING OF THE DETECTOR IN eta/phi SPACE
        #PT
        self.Histograms += [ defineHistogram('probeEtaEfficiency, probePhiEfficiency, PTfoundCombined', type='TProfile2D', title="Efficiency Profile of detector region (PT); eta; phi",
                                             xbins=10, xmin=-3, xmax=3,
                                             ybins=10, ymin=-3.14, ymax=3.14, zmin=-0.1, zmax=1.1) ]
        #FTF
        self.Histograms += [ defineHistogram('probeEtaEfficiency, probePhiEfficiency, FTFfound', type='TProfile2D', title="Efficiency Profile of detector region (FTF); eta; phi",
                                             xbins=10, xmin=-3, xmax=3,
                                             ybins=10, ymin=-3.14, ymax=3.14, zmin=-0.1, zmax=1.1) ]

        #MONITORING THE IBL
        #Innermost detector hit efficiency profiles in bins of probe track (PT): eta and phi
        self.Histograms += [ defineHistogram('PTeta, PTpixelFound', type='TProfile', title="Expected && Found hit (IBL) for Precision Tracker vs eta; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('PTphi, PTpixelFound', type='TProfile', title="Expected && Found hit (IBL) for Precision Tracker vs phi; phi [radians]",
                                             xbins=14, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        #2D profile histogram of inner detector region
        self.Histograms += [ defineHistogram('PTeta, PTphi, PTpixelFound', type='TProfile2D', title="Expected && Found hit (IBL) for region; eta; phi",
                                             xbins=10, xmin=-3, xmax=3,
                                             ybins=14, ymin=-3.14, ymax=3.14, zmin=-0.1, zmax=1.1) ]
        #Next to Innermost detector hit efficiency profiles in bins of probe track (PT): eta and phi
        self.Histograms += [ defineHistogram('PTeta, PTpixelNextToFound', type='TProfile', title="Next to innermost layer of detector - Expected && Found hit (L0) for Precision Tracker vs eta; eta [radians]",
                                             xbins=15, xmin=-3, xmax=3,
                                             ymin=-0.1, ymax=1.1) ]
        self.Histograms += [ defineHistogram('PTphi, PTpixelNextToFound', type='TProfile', title="Next to innermost layer of detector - Expected && Found hit (L0) for Precision Tracker vs phi; phi [radians]",
                                             xbins=22, xmin=-3.14, xmax=3.14,
                                             ymin=-0.1, ymax=1.1) ]
        #2D profile histogram of inner detector region
        self.Histograms += [ defineHistogram('PTeta, PTphi, PTpixelNextToFound', type='TProfile2D', title="Next to innermost layer of detector - Expected && Found hit (L0) for region; eta; phi",
                                             xbins=10, xmin=-3, xmax=3,
                                             ybins=22, ymin=-3.14, ymax=3.14, zmin=-0.1, zmax=1.1) ]
    

class TrigIDTPMonitorElectronOnlineMonitor(TrigIDTPMonitorElectronMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorElectronOnlineMonitor"):
        super(TrigIDTPMonitorElectronOnlineMonitor, self).__init__(name)
        self.defineTarget("Online")

class TrigIDTPMonitorElectronValidationMonitor(TrigIDTPMonitorElectronMonitorBase):
    def __init__ (self, name="TrigIDTPMonitorElectronValidationMonitor"):
        super(TrigIDTPMonitorElectronValidationMonitor, self).__init__(name)
        self.defineTarget("Validation")

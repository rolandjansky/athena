# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

bitNames = [ "ErrParityL1",          # bit  0
             "ErrL1mult",            # bit  1
             "ErrMuon",              # bit  2
             "spare",                # bit  3
             "L1OverflowExEy",       # bit  4
             "L1OverflowSumEt",      # bit  5
             "spare",                # bit  6
             "METinBadPhiRegion",    # bit  7
             "METinBadRegion",       # bit  8
             "ObjInPhiRegion",       # bit  9
             "ObjInRegion",          # bit 10
             "ObjInCrack",           # bit 11
             "PhiCorrJet1",          # bit 12
             "PhiCorrJet2",          # bit 13
             "PhiCorrJet3",          # bit 14
             "CompError",            # bit 15
             "EMB_A_Missing",        # bit 16
             "EMB_C_Missing",        # bit 17
             "EME_A_Missing",        # bit 18
             "EME_C_Missing",        # bit 19
             "HEC_A_Missing",        # bit 20
             "HEC_C_Missing",        # bit 21
             "FCAL_A_Missing",       # bit 22
             "FCAL_C_Missing",       # bit 23
             "TileB_A_Missing",      # bit 24
             "TileB_C_Missing",      # bit 25
             "TileE_A_Missing",      # bit 26
             "TileE_C_Missing",      # bit 27
             "L1Calo_Missing",       # bit 28
             "GlobBigMEtSEtRatio",   # bit 29
             "spare",                # bit 30
             "GlobError"             # bit 31
             ]

bitLabels = ''
for a in bitNames:
    bitLabels += a+':'

### histogram definitions
# linear-scale
hEx_Muon_lin    = defineHistogram('Muon_Ex', type='TH1F', title="Muon E_{x};E^{#mu}_{x} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
hEy_Muon_lin    = defineHistogram('Muon_Ey', type='TH1F', title="Muon E_{y};E^{#mu}_{y} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
hSumEt_Muon_lin = defineHistogram('Muon_SumEt', type='TH1F', title="Muon Sum |E_{T}|;SumE^{#mu}_{T} (GeV)", xbins=305, xmin=-27., xmax=1803.)
#
#
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are +/- 1.189 TeV
hEx_Muon_log = defineHistogram('Muon_Ex_log',   type='TH1F', title="Muon E_{x};sgn(E^{#mu}_{x}) log_{10}(E^{#mu}_{x}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
hEy_Muon_log = defineHistogram('Muon_Ey_log',   type='TH1F', title="Muon E_{y};sgn(E^{#mu}_{y}) log_{10}(E^{#mu}_{y}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
#
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are -74.99 GeV and 2.371 TeV
hSumEt_Muon_log  = defineHistogram('Muon_SumEt_log', type='TH1F', title="Sum |E^{#mu}_{T}|;log_{10}(SumE^{#mu}_{T}/GeV)",   xbins=35, xmin=-1.875, xmax=3.375)

### Online 
class TrigMissingETMuonOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMissingETMuonOnlineMonitoring"):
        super(TrigMissingETMuonOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
     
        self.Histograms =  [ hEx_Muon_lin, hEy_Muon_lin, hSumEt_Muon_lin ]
        self.Histograms += [ hEx_Muon_log, hEy_Muon_log, hSumEt_Muon_log ]

### Cosmic
class TrigMissingETMuonCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigMissingETMuonCosmicMonitoring"):
        super(TrigMissingETMuonCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        
        self.Histograms =  [ hEx_Muon_lin, hEy_Muon_lin, hSumEt_Muon_lin ]
        self.Histograms += [ hEx_Muon_log, hEy_Muon_log, hSumEt_Muon_log ] 



#########################################################################
#
#  If you ever remove histograms from Online - move them into Validation
#
#########################################################################
class TrigMissingETMuonValidationMonitoring( TrigMissingETMuonOnlineMonitoring ):
    def __init__ (self, name="TrigMissingETMuonValidationMonitoring"):
        super(TrigMissingETMuonValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
     
        self.Histograms =  [ hEx_Muon_lin, hEy_Muon_lin, hSumEt_Muon_lin ]
        self.Histograms += [ hEx_Muon_log, hEy_Muon_log, hSumEt_Muon_log ] 



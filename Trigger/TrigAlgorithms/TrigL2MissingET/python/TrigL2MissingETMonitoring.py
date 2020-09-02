# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

################# Validation, DQ checks
from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig
from builtins import range

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
hEx_lin_L1    = defineHistogram('L1_METx', type='TH1F', title="L1 Measured E_{x};E_{x} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
hEy_lin_L1    = defineHistogram('L1_METy', type='TH1F', title="L1 Measured E_{y};E_{y} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
hEx_lin_L2    = defineHistogram('L2_METx', type='TH1F', title="L2 Missing E_{x};ME_{x} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
hEy_lin_L2    = defineHistogram('L2_METy', type='TH1F', title="L2 Missing E_{y};ME_{y} (GeV)", xbins=199, xmin=-298.5, xmax=298.5)
#
hMET_lin_L1   = defineHistogram('L1_MET', type='TH1F', title="L1 E_{T};E_{T} (GeV)",            xbins=205, xmin=-13.5, xmax=601.5)
hMET_lin_L2   = defineHistogram('L2_MET', type='TH1F', title="L2 |Missing E_{T}|;ME_{T} (GeV)", xbins=205, xmin=-13.5, xmax=601.5)
hSumEt_lin_L1 = defineHistogram('L1_SumEt', type='TH1F', title="L1 Sum |E_{T}|;SumE_{T} (GeV)", xbins=305, xmin=-27., xmax=1803.)
hSumEt_lin_L2 = defineHistogram('L2_SumEt', type='TH1F', title="L2 Sum |E_{T}|;SumE_{T} (GeV)", xbins=305, xmin=-27., xmax=1803.)
#
hMETPhi_L1    = defineHistogram('L1_MET_phi', type='TH1F', title="L1 MET #phi;MET #phi (rad)", xbins= 32, xmin=-3.1416, xmax=3.1416)
hMETPhi_L2    = defineHistogram('L2_MET_phi', type='TH1F', title="L2 MET #phi;MET #phi (rad)", xbins= 32, xmin=-3.1416, xmax=3.1416)
#
#
hMETstatus = defineHistogram('L2_MET_status', type='TH1I', title="L2 MET Status", xbins= 32, xmin=-0.5, xmax=31.5, labels=bitLabels, opt="kVec")
# histogram for recording L2 significance
h_XS_L1       = defineHistogram('L1_XS',  type='TH1F', title="L1 Significance; (XS/GeV^{1/2})", xbins=40, xmin=-0.025,   xmax=20.025)
h_XS_L2       = defineHistogram('L2_XS',  type='TH1F', title="L2 Significance; (XS/GeV^{1/2})", xbins=40, xmin=-0.025,   xmax=20.025)
#
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are +/- 1.189 TeV
hEx_log_L1 = defineHistogram('L1_MEx_log',   type='TH1F', title="L1 Measured E_{x};sgn(E_{x}) log_{10}(E_{x}/GeV)",  xbins=41, xmin=-3.075, xmax=3.075)
hEy_log_L1 = defineHistogram('L1_MEy_log',   type='TH1F', title="L1 Measured E_{y};sgn(E_{y}) log_{10}(E_{y}/GeV)",  xbins=41, xmin=-3.075, xmax=3.075)
hEx_log_L2 = defineHistogram('L2_MEx_log',   type='TH1F', title="L2 Missing E_{x};sgn(ME_{x}) log_{10}(ME_{x}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
hEy_log_L2 = defineHistogram('L2_MEy_log',   type='TH1F', title="L2 Missing E_{y};sgn(ME_{y}) log_{10}(ME_{y}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
#
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are -74.99 GeV and 2.371 TeV
hMET_log_L1    = defineHistogram('L1_MET_log',   type='TH1F', title="L1 E_{T};log_{10}(E_{T}/GeV)",            xbins=35, xmin=-1.875, xmax=3.375)
hMET_log_L2    = defineHistogram('L2_MET_log',   type='TH1F', title="L2 |Missing E_{T}|;log_{10}(ME_{T}/GeV)", xbins=35, xmin=-1.875, xmax=3.375)
hSumEt_log_L1  = defineHistogram('L1_SumEt_log', type='TH1F', title="L1 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",   xbins=40, xmin=-1.875, xmax=4.125)
hSumEt_log_L2  = defineHistogram('L2_SumEt_log', type='TH1F', title="L2 Sum |E_{T}|;log_{10}(SumE_{T}/GeV)",   xbins=40, xmin=-1.875, xmax=4.125)
#

### Online 
class T2CaloMissingETOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2CaloMissingETOnlineMonitoring"):
        super(T2CaloMissingETOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        label_lba=''
        label_lbc=''
        label_eba=''
        label_ebc=''
        for i in range(64):
           if i==63 :
              label_la = '%(b)s%(n)02d' % {'b':'LBA', 'n':(i+1)}  
              label_lc = '%(b)s%(n)02d' % {'b':'LBC', 'n':(i+1)}  
              label_ea = '%(b)s%(n)02d' % {'b':'EBA', 'n':(i+1)}  
              label_ec = '%(b)s%(n)02d' % {'b':'EBC', 'n':(i+1)}  
           else:
              label_la = '%(b)s%(n)02d:' % {'b':'LBA', 'n':(i+1)}  
              label_lc = '%(b)s%(n)02d:' % {'b':'LBC', 'n':(i+1)}  
              label_ea = '%(b)s%(n)02d:' % {'b':'EBA', 'n':(i+1)}  
              label_ec = '%(b)s%(n)02d:' % {'b':'EBC', 'n':(i+1)}  
           label_lba+=label_la
           label_lbc+=label_lc
           label_eba+=label_ea
           label_ebc+=label_ec

        self.Histograms += [ defineHistogram('Met',type='TH1F', title="Met;(Met/GeV)", xbins=205, xmin=-13.5, xmax=601.5) ]
        self.Histograms += [ defineHistogram('E',type='TH1F', title="E;(E/GeV)", xbins=305, xmin=-27., xmax=1803.) ]
        self.Histograms += [ defineHistogram('Et',type='TH1F', title="E_{T};(E_{T}/GeV)", xbins=305, xmin=-27., xmax=1803.) ]
        self.Histograms += [ defineHistogram('Tile_nLBA,Tile_LBA', type='TH2F', title='L2 Calo Missing E_{T} in LBA ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_lba,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nLBC,Tile_LBC', type='TH2F', title='L2 Calo Missing E_{T} in LBC ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_lbc,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nEBA,Tile_EBA', type='TH2F', title='L2 Calo Missing E_{T} in EBA ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_eba,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nEBC,Tile_EBC', type='TH2F', title='L2 Calo Missing E_{T} in EBC ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_ebc,opt='kLBN') ]
#        self.Histograms += [ defineHistogram('TotalTime',type='TH1F', title="TotalTime", xbins=1000, xmin=-10, xmax=9999) ]

class T2CaloMissingETValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2CaloMissingETValidationMonitoring"):
        super(T2CaloMissingETValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        label_lba=''
        label_lbc=''
        label_eba=''
        label_ebc=''
        for i in range(64):
           if i==63 :
              label_la = '%(b)s%(n)02d' % {'b':'LBA', 'n':(i+1)}
              label_lc = '%(b)s%(n)02d' % {'b':'LBC', 'n':(i+1)}
              label_ea = '%(b)s%(n)02d' % {'b':'EBA', 'n':(i+1)}
              label_ec = '%(b)s%(n)02d' % {'b':'EBC', 'n':(i+1)}
           else:
              label_la = '%(b)s%(n)02d:' % {'b':'LBA', 'n':(i+1)}
              label_lc = '%(b)s%(n)02d:' % {'b':'LBC', 'n':(i+1)}
              label_ea = '%(b)s%(n)02d:' % {'b':'EBA', 'n':(i+1)}
              label_ec = '%(b)s%(n)02d:' % {'b':'EBC', 'n':(i+1)}
           label_lba+=label_la
           label_lbc+=label_lc
           label_eba+=label_ea
           label_ebc+=label_ec

        self.Histograms += [ defineHistogram('Met',type='TH1F', title="Met;(Met/GeV)", xbins=205, xmin=-13.5, xmax=601.5) ]
        self.Histograms += [ defineHistogram('E',type='TH1F', title="E;(E/GeV)", xbins=305, xmin=-27., xmax=1803.) ]
        self.Histograms += [ defineHistogram('Et',type='TH1F', title="E_{T};(E_{T}/GeV)", xbins=305, xmin=-27., xmax=1803.) ]
        self.Histograms += [ defineHistogram('Tile_nLBA,Tile_LBA', type='TH2F', title='L2 Calo Missing E_{T} in LBA ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_lba,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nLBC,Tile_LBC', type='TH2F', title='L2 Calo Missing E_{T} in LBC ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_lbc,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nEBA,Tile_EBA', type='TH2F', title='L2 Calo Missing E_{T} in EBA ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_eba,opt='kLBN') ]
        self.Histograms += [ defineHistogram('Tile_nEBC,Tile_EBC', type='TH2F', title='L2 Calo Missing E_{T} in EBC ; drawer ; Tile E_{T}', xbins=64, xmin=0, xmax=64, ybins=50, ymin=0.,ymax=500., labels=label_ebc,opt='kLBN') ]
#        self.Histograms += [ defineHistogram('TotalTime',type='TH1F', title="TotalTime", xbins=1000, xmin=-10, xmax=9999) ]


### Online 
class T2MissingETOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MissingETOnlineMonitoring"):
        super(T2MissingETOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
     
        self.Histograms  = [ hMETstatus ]    
        self.Histograms += [ hEx_lin_L1, hEy_lin_L1, hMET_lin_L1, hSumEt_lin_L1, h_XS_L1, hMETPhi_L1 ]    
        self.Histograms += [ hEx_lin_L2, hEy_lin_L2, hMET_lin_L2, hSumEt_lin_L2, h_XS_L2, hMETPhi_L2 ]
        self.Histograms += [ hEx_log_L1, hEy_log_L1, hMET_log_L1, hSumEt_log_L1 ]
        self.Histograms += [ hEx_log_L2, hEy_log_L2, hMET_log_L2, hSumEt_log_L2 ]

### Cosmic
class T2MissingETCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="L2MissingETCosmicMonitoring"):
        super(T2MissingETCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")
     
        self.Histograms  = [ hMETstatus ]    
        self.Histograms += [ hEx_lin_L1, hEy_lin_L1, hMET_lin_L1, hSumEt_lin_L1, h_XS_L1, hMETPhi_L1 ]    
        self.Histograms += [ hEx_lin_L2, hEy_lin_L2, hMET_lin_L2, hSumEt_lin_L2, h_XS_L2, hMETPhi_L2 ]
        self.Histograms += [ hEx_log_L1, hEy_log_L1, hMET_log_L1, hSumEt_log_L1 ]
        self.Histograms += [ hEx_log_L2, hEy_log_L2, hMET_log_L2, hSumEt_log_L2 ]


#########################################################################
#
#  If you ever remove histograms from Online - move them into Validation
#
#########################################################################
class T2MissingETValidationMonitoring( T2MissingETOnlineMonitoring ):
    def __init__ (self, name="L2MissingETValidationMonitoring"):
        super(T2MissingETValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
     
        self.Histograms  = [ hMETstatus ]    
        self.Histograms += [ hEx_lin_L1, hEy_lin_L1, hMET_lin_L1, hSumEt_lin_L1, h_XS_L1, hMETPhi_L1 ]    
        self.Histograms += [ hEx_lin_L2, hEy_lin_L2, hMET_lin_L2, hSumEt_lin_L2, h_XS_L2, hMETPhi_L2 ]
        self.Histograms += [ hEx_log_L1, hEy_log_L1, hMET_log_L1, hSumEt_log_L1 ]
        self.Histograms += [ hEx_log_L2, hEy_log_L2, hMET_log_L2, hSumEt_log_L2 ]


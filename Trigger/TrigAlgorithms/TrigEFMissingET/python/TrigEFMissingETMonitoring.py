# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Definitions of the monitoring classes

from TrigMonitorBase.TrigGenericMonitoringToolConfig import defineHistogram, TrigGenericMonitoringToolConfig

### labels
compNames = [ "PreSamplB", "EMB1", "EMB2", "EMB3",   # LAr barrel
              "PreSamplE", "EME1", "EME2", "EME3",   # LAr EM endcap 
              "HEC0",      "HEC1", "HEC2", "HEC3",   # Hadronic end cap cal.
              "TileBar0", "TileBar1", "TileBar2",    # Tile barrel
              "TileGap1", "TileGap2", "TileGap3",    # Tile gap (ITC & scint)
              "TileExt0", "TileExt1", "TileExt2",    # Tile extended barrel
              "FCalEM",   "FCalHad2", "FCalHad3",    # Forward cal endcap
              "TCLCW", "TCLCWB1", "TCLCWB2",         # TC em, Tile barrel 
              "TCLCWE1", "TCLCWE2",                  # Forward endcap
              "TCEM", "TCEMB1", "TCEMB2",            # TC LC, Tile barrel
              "TCEME1", "TCEME2",                    # Forward endcap
              "Jets",                                # Jets 
              "Muons" ]                              # Muons 

## bits assigned in EFMissingETBaseTool.cxx
## remember to update also the 2D comp. status histo. in EFMissingETFlags::start()
bitNames = [ "Processing",         # bit  0
             "ErrBSconv",          # bit  1
             "ErrMuon",            # bit  2
             "ErrFEB",             # bit  3
             "Skipped",            # bit  4
             "CompBigMEtSEtRatio", # bit  5
             "BadCompEnergy",      # bit  6
             "BadEnergyRatio",     # bit  7
             "spare",              # bit  8
             "BadCellQuality",     # bit  9
             "BadCellEnergy",      # bit 10
             "BadCellTime",        # bit 11
             "NoMuonTrack",        # bit 12
             "spare",              # bit 13
             "Processed",          # bit 14
             "CompError",          # bit 15
             "EMB_A_Missing",      # bit 16
             "EMB_C_Missing",      # bit 17
             "EME_A_Missing",      # bit 18
             "EME_C_Missing",      # bit 19
             "HEC_A_Missing",      # bit 20
             "HEC_C_Missing",      # bit 21
             "FCAL_A_Missing",     # bit 22
             "FCAL_C_Missing",     # bit 23
             "TileB_A_Missing",    # bit 24
             "TileB_C_Missing",    # bit 25
             "TileE_A_Missing",    # bit 26
             "TileE_C_Missing",    # bit 27
             "BadEMfraction",      # bit 28
             "GlobBigMEtSEtRatio", # bit 29
             "ObjInCrack",         # bit 30
             "GlobError"           # bit 31
             ]

compLabels = ''
for a in compNames:
    compLabels += a+':'

bitLabels = ''
for a in bitNames:
    bitLabels += a+':'


### histogram definitions 
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# central bin is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are +/- 1.189 TeV
hEx_log    = defineHistogram('EF_MEx_log',   type='TH1F', title="EF Missing E_{x};sgn(ME_{x}) log_{10}(ME_{x}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
hEy_log    = defineHistogram('EF_MEy_log',   type='TH1F', title="EF Missing E_{y};sgn(ME_{y}) log_{10}(ME_{y}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
hEz_log    = defineHistogram('EF_MEz_log',   type='TH1F', title="EF Missing E_{z};sgn(ME_{z}) log_{10}(ME_{z}/GeV)", xbins=41, xmin=-3.075, xmax=3.075)
#
# signed log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# bin centered on zero is +/- 0.075 and contains values whose abs < 1.189 GeV
# limits are -74.99 GeV and 2.371 TeV 
hSumEt_log = defineHistogram('EF_SumEt_log', type='TH1F', title="EF Sum E_{T};log_{10}(SumE_{T}/GeV)",   xbins=40, xmin=-1.875, xmax=4.125)
hSumE_log  = defineHistogram('EF_SumE_log',  type='TH1F', title="EF Sum E;log_{10}(SumE/GeV)",           xbins=40, xmin=-1.875, xmax=4.125)
#
# unsigned (i.e. normal) log-scale: 0.15/bin = 1.413X/bin (factor of ~2 each two bins)
# limits are 708 MeV and 2.818 TeV
hMET_log   = defineHistogram('EF_MET_log',   type='TH1F', title="EF |Missing E_{T}|;log_{10}(ME_{T}/GeV)", xbins=24, xmin=-0.15, xmax=3.45)
hME_log    = defineHistogram('EF_ME_log',    type='TH1F', title="EF |Missing Energy|;log_{10}(ME/GeV)", xbins=24, xmin=-0.15, xmax=3.45)
 
# linear-scale
hEx_lin    = defineHistogram('EF_MEx_lin',   type='TH1F', title="EF Missing E_{x};ME_{x} (GeV)",                    xbins=199, xmin=-298.5,   xmax=298.5)
hEy_lin    = defineHistogram('EF_MEy_lin',   type='TH1F', title="EF Missing E_{y};ME_{y} (GeV)",                    xbins=199, xmin=-298.5,   xmax=298.5)
hEz_lin    = defineHistogram('EF_MEz_lin',   type='TH1F', title="EF Missing E_{z};ME_{z} (GeV)",                    xbins=199, xmin=-298.5,   xmax=298.5)
hMET_lin   = defineHistogram('EF_MET_lin',   type='TH1F', title="EF |Missing E_{T}|;ME_{T} (GeV)",                 xbins=205, xmin=-13.5,    xmax=601.5)
hME_lin    = defineHistogram('EF_ME_lin',    type='TH1F', title="EF |Missing E|;ME (GeV)",                         xbins=205, xmin=-13.5,    xmax=601.5)
hSumEt_lin = defineHistogram('EF_SumEt_lin', type='TH1F', title="EF Sum E_{T};SumE_{T} (GeV)",       xbins=305, xmin=-27.,     xmax=1803.)
hSumE_lin  = defineHistogram('EF_SumE_lin',  type='TH1F', title="EF Sum E;SumE (GeV)",               xbins=305, xmin=-27.,     xmax=3500.)
hXS        = defineHistogram('EF_XS',        type='TH1F', title="EF Significance; (XS/GeV^{1/2})",         xbins=40,  xmin=-0.025,   xmax=20.025)
hMETPhi    = defineHistogram('EF_MET_phi',    type='TH1F', title="EF MET #phi;MET #phi (rad)", xbins= 32, xmin=-3.1416, xmax=3.1416)

hTimeEMRegSel     = defineHistogram('EMRegSelTime',     type='TH1F', title="EM RegionSelector time (ms)",     xbins=100, xmin=0, xmax=200) 
hTimeEMLoadCol    = defineHistogram('EMLoadColTime',    type='TH1F', title="EM LoadCollections time (ms)",    xbins=100, xmin=0, xmax=200) 
hTimeEMLoop       = defineHistogram('EMLoopTime',       type='TH1F', title="EM Channels loop time (ms)",      xbins=100, xmin=0, xmax=300) 
hTimeHECRegSel    = defineHistogram('HECRegSelTime',    type='TH1F', title="HEC RegionSelector time (ms)",    xbins=100, xmin=0, xmax=100) 
hTimeHECLoadCol   = defineHistogram('HECLoadColTime',   type='TH1F', title="HEC LoadCollections time (ms)",   xbins=100, xmin=0, xmax=50) 
hTimeHECLoop      = defineHistogram('HECLoopTime',      type='TH1F', title="HEC Channels loop time (ms)",     xbins=100, xmin=0, xmax=50) 
hTimeTileRegSel   = defineHistogram('TileRegSelTime',   type='TH1F', title="Tile RegionSelector time (ms)",   xbins=100, xmin=0, xmax=10) 
hTimeTileLoadCol  = defineHistogram('TileLoadColTime',  type='TH1F', title="Tile LoadCollections time (ms)",  xbins=100, xmin=0, xmax=10) 
hTimeTileLoop     = defineHistogram('TileLoopTime',     type='TH1F', title="Tile Channels loop time (ms)",    xbins=100, xmin=0, xmax=10) 
hTimeFCalRegSel   = defineHistogram('FCalRegSelTime',   type='TH1F', title="FCal RegionSelector time (ms)",   xbins=100, xmin=0, xmax=10) 
hTimeFCalLoadCol  = defineHistogram('FCalLoadColTime',  type='TH1F', title="FCal LoadCollections time (ms)",  xbins=100, xmin=0, xmax=10) 
hTimeFCalLoop     = defineHistogram('FCalLoopTime',     type='TH1F', title="FCal Channels loop time (ms)",    xbins=100, xmin=0, xmax=10) 

# hTotalTime        = defineHistogram('TotalTime',        type='TH1F', title="Total time (ms)",                 xbins=100, xmin=0, xmax=500) 
hTotalTimeEM      = defineHistogram('TotalEMTime',      type='TH1F', title="EM total time (ms)",              xbins=100, xmin=0, xmax=300) 
hTotalTimeHEC     = defineHistogram('TotalHECTime',     type='TH1F', title="HEC total time (ms)",             xbins=100, xmin=0, xmax=100) 
hTotalTimeTile    = defineHistogram('TotalTileTime',    type='TH1F', title="Tile total time (ms)",            xbins=100, xmin=0, xmax=50)
hTotalTimeFCal    = defineHistogram('TotalFCalTime',    type='TH1F', title="FCal total time (ms)",            xbins=100, xmin=0, xmax=50) 
hTotalTimeRegSel  = defineHistogram('TotalRegSelTime',  type='TH1F', title="RegionSelector total time (ms)",  xbins=100, xmin=0, xmax=200)
hTotalTimeLoadCol = defineHistogram('TotalLoadColTime', type='TH1F', title="LoadCollections total time (ms)", xbins=100, xmin=0, xmax=200)
hTotalTimeLoop    = defineHistogram('TotalLoopTime',    type='TH1F', title="Channels loop total time (ms)",   xbins=100, xmin=0, xmax=300)

hMETStatus = defineHistogram('EF_MET_status', type='TH1I', title="EF MET Status", xbins= 32, xmin=-0.5, xmax=31.5, labels=bitLabels, opt="kVec") 

hCompEx     = defineHistogram('compN, compEx', type='TH2F',
                             title="EF MissingE_{x} VS component;;sgn(ME_{x}) log_{10}(E_{x}/GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=41, ymin=-3.075, ymax=3.075, labels=compLabels)
hCompEy     = defineHistogram('compN, compEy', type='TH2F',
                             title="EF Missing E_{y} VS component;;sgn(ME_{y}) log_{10}(E_{y}/GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=41, ymin=-3.075, ymax=3.075, labels=compLabels)
hCompEz     = defineHistogram('compN, compEz', type='TH2F',
                             title="EF Missing E_{z} VS component;;sgn(ME_{z}) log_{10}(E_{z}/GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=41, ymin=-3.075, ymax=3.075, labels=compLabels)
hCompEt     = defineHistogram('compN, compEt', type='TH2F',
                              title="EF MissingE_{T} VS component;;log_{10}(ME_{T}/GeV)",
                              xbins=36,  xmin=-0.5, xmax=35.5, ybins=24, ymin=-0.15, ymax=3.45, labels=compLabels)
hCompSumEt  = defineHistogram('compN, compSumEt', type='TH2F',
                             title="EF Sum |E_{T}| VS component;;log_{10}(SumE_{T}/GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=35, ymin=-1.875, ymax=3.375, labels=compLabels)
hCompSumE   = defineHistogram('compN, compSumE', type='TH2F',
                             title="EF Sum |E| VS component;;log_{10}(SumE/GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=35, ymin=-1.875, ymax=3.375, labels=compLabels)

hCompEt_lin = defineHistogram('compN, compEt_lin', type='TH2F',
                             title="EF MissingE_{T} VS component;;ME_{T} (GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=205, ymin=-13.5, ymax=601.5, labels=compLabels)
hCompSumEt_lin= defineHistogram('compN, compSumEt_lin', type='TH2F',
                             title="EF Sum |E_{T}| VS component;;SumE_{T} (GeV)",
                             xbins=36,  xmin=-0.5, xmax=35.5, ybins=305, ymin=-27,   ymax=1803,  labels=compLabels)

# for a in bitNames:
#     compLabels += a+':'

#hCompStatus = defineHistogram('compN, compStatus', type='TH2I',
#                              title="Event status bits VS component",
#                              xbins=36,  xmin=-0.5, xmax=35.5, ybins=16, ymin=-0.5, ymax=15.5,
#                              labels=compLabels, opt="kVec")


### monitoring classes


class TrigEFMissingETValidationMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETValidationMonitoring"):
        super(TrigEFMissingETValidationMonitoring, self).__init__(name)
        self.defineTarget("Validation")
        # measurement
        self.Histograms += [ hEx_log, hEy_log, hEz_log, hMET_log, hME_log, hSumEt_log, hSumE_log ]
        self.Histograms += [ hEx_lin, hEy_lin, hEz_lin, hMET_lin, hME_lin, hSumEt_lin, hSumE_lin ]
        self.Histograms += [ hXS, hMETPhi, hMETStatus]
        self.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
        self.Histograms += [ hCompEt_lin, hCompSumEt_lin ]
        # timers
        self.Histograms += [ hTimeEMRegSel,   hTimeEMLoadCol,   hTimeEMLoop ]
        self.Histograms += [ hTimeHECRegSel,  hTimeHECLoadCol,  hTimeHECLoop ]
        self.Histograms += [ hTimeTileRegSel, hTimeTileLoadCol, hTimeTileLoop ]
        self.Histograms += [ hTimeFCalRegSel, hTimeFCalLoadCol, hTimeFCalLoop ]
#         self.Histograms += [ hTotalTime,   hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeEM, hTotalTimeHEC,    hTotalTimeTile ]


class TrigEFMissingETValidationMonitoring_alt(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETValidationMonitoring_alt"):
        super(TrigEFMissingETValidationMonitoring_alt, self).__init__(name)
        self.defineTarget("Validation")
        # measurement
        self.Histograms += [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin, hMETStatus ]
        self.Histograms += [ hMETPhi ]


class TrigEFMissingETValidationMonitoring_other(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETValidationMonitoring_other"):
        super(TrigEFMissingETValidationMonitoring_other, self).__init__(name)
        self.defineTarget("Validation")
        # measurement
        self.Histograms += [ hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin ]
        self.Histograms += [ hMETPhi ]


class TrigEFMissingETOnlineMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETOnlineMonitoring"):
        """ defines histograms for online """
        super(TrigEFMissingETOnlineMonitoring, self).__init__(name)
        self.defineTarget("Online")
        # measurement
        self.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin, hSumEt_lin ]
        self.Histograms += [ hXS, hMETPhi, hMETStatus]
        self.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
        self.Histograms += [ hCompEt_lin, hCompSumEt_lin ]
        # timers
#         self.Histograms += [ hTotalTime,   hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeEM, hTotalTimeHEC,    hTotalTimeTile ]


class TrigEFMissingETOnlineMonitoring_alt(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETOnlineMonitoring_alt"):
        """ defines histograms for online """
        super(TrigEFMissingETOnlineMonitoring_alt, self).__init__(name)
        self.defineTarget("Online")
        # measurement
        self.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin, hMETStatus ]
        self.Histograms += [ hMETPhi ]


class TrigEFMissingETOnlineMonitoring_other(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETOnlineMonitoring_other"):
        """ defines histograms for online """
        super(TrigEFMissingETOnlineMonitoring_other, self).__init__(name)
        self.defineTarget("Online")
        # measurement
        self.Histograms  = [ hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin ]
        self.Histograms += [ hMETPhi ]


class TrigEFMissingETCosmicMonitoring(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETCosmicMonitoring"):
        """ defines histograms for cosmic """
        super(TrigEFMissingETCosmicMonitoring, self).__init__(name)
        self.defineTarget("Cosmic")
        # measurement
        self.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin, hSumEt_lin ]
        self.Histograms += [ hMETPhi, hMETStatus]
        self.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
        self.Histograms += [ hCompEt_lin, hCompSumEt_lin ]
        # timers
#         self.Histograms += [ hTotalTime,   hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeRegSel, hTotalTimeLoadCol, hTotalTimeLoop ]
        self.Histograms += [ hTotalTimeEM, hTotalTimeHEC,    hTotalTimeTile ]


class TrigEFMissingETCosmicMonitoring_alt(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETCosmicMonitoring_alt"):
        """ defines histograms for cosmic """
        super(TrigEFMissingETCosmicMonitoring_alt, self).__init__(name)
        self.defineTarget("Cosmic")
        # measurement
        self.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin ]
        self.Histograms += [ hMETPhi, hMETStatus ]

class TrigEFMissingETCosmicMonitoring_other(TrigGenericMonitoringToolConfig):
    def __init__ (self, name="TrigEFMissingETCosmicMonitoring_other"):
        """ defines histograms for cosmic """
        super(TrigEFMissingETCosmicMonitoring_other, self).__init__(name)
        self.defineTarget("Cosmic")
        # measurement
        self.Histograms  = [ hMET_log, hSumEt_log ]
        self.Histograms += [ hMET_lin ]
        self.Histograms += [ hMETPhi ]


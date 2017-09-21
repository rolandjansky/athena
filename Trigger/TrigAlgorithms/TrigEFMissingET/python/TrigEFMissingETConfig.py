# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigEFMissingET.TrigEFMissingETConf import EFMissingET

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromFEBHeader
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCells
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClusters
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersPS
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersPUC
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromJets
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromTrackAndJets
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFlags
from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromHelper

# Import CaloNoiseToolDefault for EFMissingETFromCells
from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool=CaloNoiseToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc+=theCaloNoiseTool

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.SystemOfUnits import nanosecond

from TriggerJobOpts.TriggerFlags import TriggerFlags

class EFMissingETBase (EFMissingET):
    __slots__ = []
    def __init__(self, name):
        super( EFMissingETBase, self ).__init__(name)



##### loop over cells, no noise suppression #####
class EFMissingET_Fex_allCells (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_allCells"):
        super(EFMissingET_Fex_allCells, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET"

        # tools
        febTool =    EFMissingETFromFEBHeader("TheFEBTool")
        cellTool =   EFMissingETFromCells("TheCellTool")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        cellTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #
        cellTool.CaloNoiseTool=theCaloNoiseTool
        cellTool.DoCellNoiseSuppression = False

        #### settings for robustness checks:
        # |energy| thresholds to make cell-level robustness checks = 2 rms
        cellTool.MinCellSNratio = []
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplB, EMB1, EMB2, EMB3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplE, EME1, EME2, EME3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # HEC0, HEC1, HEC2, HEC3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileBar0, TileBar1, TileBar2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileGap0, TileGap1, TileGap2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileExt0, TileExt1, TileExt2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # FCalEM, FCalHad1, FCalHad2
        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
#        self.Tools += [ febTool ]
        self.Tools += [ cellTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]       
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]      
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_other, TrigEFMissingETOnlineMonitoring_other, TrigEFMissingETCosmicMonitoring_other
        validation = TrigEFMissingETValidationMonitoring_other()
        online = TrigEFMissingETOnlineMonitoring_other()
        cosmic = TrigEFMissingETCosmicMonitoring_other()

        self.AthenaMonTools = [ validation, online, cosmic]



##### loop over cells with noise suppression #####
class EFMissingET_Fex_noiseSupp (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_noiseSupp"):
        super(EFMissingET_Fex_noiseSupp, self).__init__(name)

        # name of TrigMissingET object
        #self.MissingETOutputKey = "TrigEFMissingET_noiseSupp"
        # Use the following if not run together with 2-sided
        self.MissingETOutputKey = "TrigEFMissingET"

        # tools
        febTool =    EFMissingETFromFEBHeader("TheFEBTool")
        cellTool =   EFMissingETFromCells("TheCellTool")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        cellTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #
        cellTool.CaloNoiseTool=theCaloNoiseTool
        cellTool.DoCellNoiseSuppression = True

        #### settings for robustness checks:
        # |energy| thresholds to make cell-level robustness checks = 2 rms
        cellTool.MinCellSNratio = []
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplB, EMB1, EMB2, EMB3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplE, EME1, EME2, EME3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # HEC0, HEC1, HEC2, HEC3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileBar0, TileBar1, TileBar2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileGap0, TileGap1, TileGap2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileExt0, TileExt1, TileExt2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # FCalEM, FCalHad1, FCalHad2
        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
#        self.Tools += [ febTool ]
        self.Tools += [ cellTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]   
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]        
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_other, TrigEFMissingETOnlineMonitoring_other, TrigEFMissingETCosmicMonitoring_other
        validation = TrigEFMissingETValidationMonitoring_other()
        online = TrigEFMissingETOnlineMonitoring_other()
        cosmic = TrigEFMissingETCosmicMonitoring_other()

        self.AthenaMonTools = [ validation, online, cosmic]



##### loop over cells with noise suppression #####
class EFMissingET_Fex_2sidednoiseSupp (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_2sidednoiseSupp"):
        super(EFMissingET_Fex_2sidednoiseSupp, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET"

        # tools
        febTool =    EFMissingETFromFEBHeader("TheFEBTool")
        cellTool =   EFMissingETFromCells("TheCellTool")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        cellTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #
        cellTool.CaloNoiseTool=theCaloNoiseTool
        cellTool.DoCellNoiseSuppression = True
        cellTool.CaloNoiseOneSidedCut = -5.0

        #### settings for robustness checks:
        # |energy| thresholds to make cell-level robustness checks = 2 rms
        cellTool.MinCellSNratio = []
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplB, EMB1, EMB2, EMB3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplE, EME1, EME2, EME3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # HEC0, HEC1, HEC2, HEC3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileBar0, TileBar1, TileBar2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileGap0, TileGap1, TileGap2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileExt0, TileExt1, TileExt2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # FCalEM, FCalHad1, FCalHad2
        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
#        self.Tools += [ febTool ]
        self.Tools += [ cellTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]     
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons

        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring, TrigEFMissingETOnlineMonitoring, TrigEFMissingETCosmicMonitoring
        validation = TrigEFMissingETValidationMonitoring()
        online = TrigEFMissingETOnlineMonitoring()
        cosmic = TrigEFMissingETCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("EFMissingET_Time")
        time.TimerHistLimits = [0, 250]

        self.AthenaMonTools = [ validation, online, cosmic, time]




##### loop over LAR FEBs and Tile cells (with noise suppression) #####
class EFMissingET_Fex_FEB (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_FEB"):
        super(EFMissingET_Fex_FEB, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_FEB"

        # tools
        febTool =    EFMissingETFromFEBHeader("TheFEBTool")
        cellTool =   EFMissingETFromCells("TheCellTool")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        cellTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #
        cellTool.CaloNoiseTool=theCaloNoiseTool
        cellTool.DoCellNoiseSuppression = True

        #### settings for robustness checks:
        # |energy| thresholds to make cell-level robustness checks = 2 rms
        cellTool.MinCellSNratio = []
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplB, EMB1, EMB2, EMB3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # PreSamplE, EME1, EME2, EME3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0,5.0 ] # HEC0, HEC1, HEC2, HEC3
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileBar0, TileBar1, TileBar2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileGap0, TileGap1, TileGap2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # TileExt0, TileExt1, TileExt2
        cellTool.MinCellSNratio += [ 5.0,5.0,5.0 ] # FCalEM, FCalHad1, FCalHad2
        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
        self.Tools += [ febTool ]
        #self.Tools += [ cellTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_alt, TrigEFMissingETOnlineMonitoring_alt, TrigEFMissingETCosmicMonitoring_alt
        validation = TrigEFMissingETValidationMonitoring_alt()
        online = TrigEFMissingETOnlineMonitoring_alt()
        cosmic = TrigEFMissingETCosmicMonitoring_alt()

        self.AthenaMonTools = [ validation, online, cosmic]



##### Use topo. clusters for noise suppression #####
class EFMissingET_Fex_topoClusters (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_topoClusters"):
        super(EFMissingET_Fex_topoClusters, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_topocl"
        self.doTopoClusters = True

        # tools
        clusterTool = EFMissingETFromClusters("TheClusterTool")
        clusterTool2 = EFMissingETFromClusters("TheClusterTool2")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        clusterTool.ParentFexName = name
        clusterTool2.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name

        # Add uncalibrated clusters into permanent object?
        clusterTool2.SaveUncalibrated = True

        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
        self.Tools += [ clusterTool ]
        self.Tools += [ clusterTool2 ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_alt, TrigEFMissingETOnlineMonitoring_alt, TrigEFMissingETCosmicMonitoring_alt
        validation = TrigEFMissingETValidationMonitoring_alt()
        online = TrigEFMissingETOnlineMonitoring_alt()
        cosmic = TrigEFMissingETCosmicMonitoring_alt()

        self.AthenaMonTools = [ validation, online, cosmic]


##### Use topo. clusters for noise suppression -- w/ pileup subtraction #####
class EFMissingET_Fex_topoClustersPS (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_topoClustersPS"):
        super(EFMissingET_Fex_topoClustersPS, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_topocl_PS"
        self.doTopoClusters = True

        # tools
        clusterTool = EFMissingETFromClustersPS("TheClusterToolPS")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        clusterTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name

        # ClusterTool options
        clusterTool.SubtractPileup = True
        clusterTool.PileupDebug = False
        clusterTool.PileupNumRings = 10
        clusterTool.PileupNumStdDev = 2.0

        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
        self.Tools += [ clusterTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_other, TrigEFMissingETOnlineMonitoring_other, TrigEFMissingETCosmicMonitoring_other
        validation = TrigEFMissingETValidationMonitoring_other()
        online = TrigEFMissingETOnlineMonitoring_other()
        cosmic = TrigEFMissingETCosmicMonitoring_other()

        self.AthenaMonTools = [ validation, online, cosmic]

##### Use topo. clusters for noise suppression #####
class EFMissingET_Fex_topoClustersPUC (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_topoClustersPUC"):
        super(EFMissingET_Fex_topoClustersPUC, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_topocl_PUC"
        self.doTopoClusters = True
        self.doPUC = True

        # tools
        clusterTool = EFMissingETFromClustersPUC("TheClusterToolPUC")
        flagTool =   EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        clusterTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        
        clusterTool.SubtractPileup = True


        is2016 = (TriggerFlags.run2Config() == '2016')


        clusterTool.use2016Algo = is2016
# N.B. - defaults for 2016 running: nSigma = 3.2 and varRhoScale = 4.0
# N.B. - defaults for 2017 running: nSigma = 5.0 and varRhoScale = 1.0
        clusterTool.nSigma = 3.2 if is2016 else 5.0
        clusterTool.varRhoScale = 4.0 if is2016 else 1.0
        clusterTool.aveEclusPU = 10000.0
        clusterTool.towerWidthInput = 0.7
        clusterTool.EtaRange = 5.0
        clusterTool.resE = 15.81
        clusterTool.resEfloor = 50.0
        clusterTool.trimFactor = 0.90

        # fraction of energy deposited in EM samplings
        flagTool.MaxEMfraction = 1.0
        flagTool.MinEMfraction = 0.0
        flagTool.MaxTileGapEratio = 1.0
        # max/min energy ratios from each subdet.
        flagTool.MaxSumEratioInEMB = 1.0
        flagTool.MaxSumEratioInEME = 1.0
        flagTool.MaxSumEratioInHEC = 1.0
        flagTool.MaxSumEratioInTileBar = 1.0
        flagTool.MaxSumEratioInTileGap = 1.0
        flagTool.MaxSumEratioInTileExt = 1.0
        flagTool.MaxSumEratioInFCal = 1.0
        flagTool.MinSumEratioInEMB = 0.0
        flagTool.MinSumEratioInEME = 0.0
        flagTool.MinSumEratioInHEC = 0.0
        flagTool.MinSumEratioInTileBar = 0.0
        flagTool.MinSumEratioInTileGap = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        flagTool.MinSumEratioInTileExt = 0.0
        # max/min comp energies
        flagTool.MaxCompE = []
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCompE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCompE = []
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCompE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell energies
        flagTool.MaxCellE = []
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV,1e4*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellE += [ 1e4*GeV,1e4*GeV,1e4*GeV ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellE = []
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV,0.0*GeV ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellE += [ 0.0*GeV,0.0*GeV,0.0*GeV ] # FCalEM, FCalHad1, FCalHad2
        # max/min cell times
        flagTool.MaxCellTime = []
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond,+10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MaxCellTime += [ +10*nanosecond,+10*nanosecond,+10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        flagTool.MinCellTime = []
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # PreSamplE, EME1, EME2, EME3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond,-10*nanosecond ] # HEC0, HEC1, HEC2, HEC3
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileBar0, TileBar1, TileBar2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileGap0, TileGap1, TileGap2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # TileExt0, TileExt1, TileExt2
        flagTool.MinCellTime += [ -10*nanosecond,-10*nanosecond,-10*nanosecond ] # FCalEM, FCalHad1, FCalHad2
        # max cell chi-square
        flagTool.WorstCellQuality = []
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplB, EMB1, EMB2, EMB3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # PreSamplE, EME1, EME2, EME3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3,4e3 ] # HEC0, HEC1, HEC2, HEC3
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileBar0, TileBar1, TileBar2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileGap0, TileGap1, TileGap2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # TileExt0, TileExt1, TileExt2
        flagTool.WorstCellQuality += [ 4e3,4e3,4e3 ] # FCalEM, FCalHad1, FCalHad2

        ## chain of tools
        self.Tools = []
        self.Tools += [ clusterTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        # component flags (-1 means skip)
        self.ComponentFlags = []
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentFlags += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentFlags += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentFlags += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentFlags += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentFlags += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentFlags += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentFlags += [ 0 ]       # EM Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Had Topo
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # Jet
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0,0 ]
        self.ComponentFlags += [ 0 ]       # PUC
        self.ComponentFlags += [ 0 ]       # PUC prior correction
        self.ComponentFlags += [ 0 ]       # Muons
        # calibration: constant term (MeV)
        self.ComponentCalib0 = []
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib0 += [ 0,0,0,0 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib0 += [ 0,0,0,0 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib0 += [ 0,0,0 ]   # TileBar0, TileBar1, TileBar2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileGap0, TileGap1, TileGap2
        self.ComponentCalib0 += [ 0,0,0 ]   # TileExt0, TileExt1, TileExt2
        self.ComponentCalib0 += [ 0,0,0 ]   # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib0 += [ 0 ]       # EM Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Had Topo
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # Jet
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0, 0 ]
        self.ComponentCalib0 += [ 0 ]       # PUC
        self.ComponentCalib0 += [ 0 ]       # PUC prior correction
        self.ComponentCalib0 += [ 0 ]       # Muons
        # calibration: multiplicative constant
        self.ComponentCalib1 = []
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplB, EMB1, EMB2, EMB3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # PreSamplE, EME1, EME2, EME3
        self.ComponentCalib1 += [ 1.00,1.00,1.00,1.00 ] # HEC0, HEC1, HEC2, HEC3
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileBar0, TileBar1, TileBar2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileGap0, TileGap1, TileGap2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # TileExt0, TileExt1, TileExt2
        self.ComponentCalib1 += [ 1.00,1.00,1.00 ]      # FCalEM, FCalHad1, FCalHad2
        self.ComponentCalib1 += [ 1.00 ]                # EM Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Had Topo
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # Jet
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00,1.00 ]
        self.ComponentCalib1 += [ 1.00 ]                # PUC
        self.ComponentCalib1 += [ 1.00 ]                # PUC prior correction
        self.ComponentCalib1 += [ 1.00 ]                # Muons


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_other, TrigEFMissingETOnlineMonitoring_other, TrigEFMissingETCosmicMonitoring_other
        validation = TrigEFMissingETValidationMonitoring_other()
        online = TrigEFMissingETOnlineMonitoring_other()
        cosmic = TrigEFMissingETCosmicMonitoring_other()

        self.AthenaMonTools = [ validation, online, cosmic]

##### loop over jets #####
class EFMissingET_Fex_Jets (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_Jets", extraCalib = ""):
        super(EFMissingET_Fex_Jets, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_mht{0}".format(extraCalib)
        self.doJets = True
                
        # tools
        febTool    = EFMissingETFromFEBHeader("TheFEBTool") 
        jetTool    = EFMissingETFromJets("TheJetTool")
        flagTool   = EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        jetTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #
        
        ## Configuration of jet fex
        jetTool.EtaSeparation = 2.2
        jetTool.CentralpTCut = 0.0
        jetTool.ForwardpTCut = 0.0
        jetTool.ApplyTileGap3Correction = TriggerFlags.run2Config() != '2016' # Do not apply the TileGap3 correction for 2015+2016 data
        
        ## chain of tools
        self.Tools = []
        self.Tools += [ jetTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]


        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_alt, TrigEFMissingETOnlineMonitoring_alt, TrigEFMissingETCosmicMonitoring_alt
        validation = TrigEFMissingETValidationMonitoring_alt()
        online = TrigEFMissingETOnlineMonitoring_alt()
        cosmic = TrigEFMissingETCosmicMonitoring_alt()

        self.AthenaMonTools = [ validation, online, cosmic]

##### loop over tracks and jets #####
class EFMissingET_Fex_TrackAndJets (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_TrackAndJets"):
        super(EFMissingET_Fex_TrackAndJets, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_trkmht"
        self.doJets = True
        self.doTracks = True

        # tools
        febTool    = EFMissingETFromFEBHeader("TheFEBTool")
        jetTool    = EFMissingETFromTrackAndJets("TheTrackAndJetTool")
        flagTool   = EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        jetTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #

        ## Configuration of jet fex
        jetTool.EtaSeparation = 2.2
        jetTool.CentralpTCut = 25 #GeV
        jetTool.ForwardpTCut = 0.0
        jetTool.TrackpTCut = 1 #GeV
        jetTool.CentralJetJVTCut = 0.9
        jetTool.TrackSelectionTool.CutLevel = "Loose"
        jetTool.TrackSelectionTool.maxZ0SinTheta = 1.5
        jetTool.TrackSelectionTool.maxD0overSigmaD0 = 3

        ## chain of tools
        self.Tools = []
        self.Tools += [ jetTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_alt, TrigEFMissingETOnlineMonitoring_alt, TrigEFMissingETCosmicMonitoring_alt
        validation = TrigEFMissingETValidationMonitoring_alt()
        online = TrigEFMissingETOnlineMonitoring_alt()
        cosmic = TrigEFMissingETCosmicMonitoring_alt()

        self.AthenaMonTools = [ validation, online, cosmic]

##### loop over tracks and jets #####
class EFMissingET_Fex_FTKTrackAndJets (EFMissingETBase):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex_FTKTrackAndJets"):
        super(EFMissingET_Fex_FTKTrackAndJets, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET_trkmhtFTK"
        self.doJets = True
        self.doTracks = True

        # tools
        febTool    = EFMissingETFromFEBHeader("TheFEBTool")
        jetTool    = EFMissingETFromTrackAndJets("TheTrackAndJetTool")
        flagTool   = EFMissingETFlags("TheFlagsTool")
        helperTool = EFMissingETFromHelper("TheHelperTool")
        #
        febTool.ParentFexName = name
        jetTool.ParentFexName = name
        flagTool.ParentFexName = name
        helperTool.ParentFexName = name
        #

        ## Configuration of jet fex
        jetTool.EtaSeparation = 2.2
        jetTool.CentralpTCut = 25 #GeV
        jetTool.ForwardpTCut = 0.0
        jetTool.TrackpTCut = 1 #GeV
        jetTool.CentralJetJVTCut = 0.9
        jetTool.TrackSelectionTool.CutLevel = "Loose"
        jetTool.TrackSelectionTool.maxZ0SinTheta = 1.5
        jetTool.TrackSelectionTool.maxD0overSigmaD0 = 3

        ## chain of tools
        self.Tools = []
        self.Tools += [ jetTool ]
        self.Tools += [ flagTool ]
        self.Tools += [ helperTool ]

        from TrigEFMissingET.TrigEFMissingETMonitoring import TrigEFMissingETValidationMonitoring_alt, TrigEFMissingETOnlineMonitoring_alt, TrigEFMissingETCosmicMonitoring_alt
        validation = TrigEFMissingETValidationMonitoring_alt()
        online = TrigEFMissingETOnlineMonitoring_alt()
        cosmic = TrigEFMissingETCosmicMonitoring_alt()

        self.AthenaMonTools = [ validation, online, cosmic]


##### THE DEFAULT FEX #####
class EFMissingET_Fex (EFMissingET_Fex_2sidednoiseSupp):
    __slots__ = []
    def __init__ (self, name="EFMissingET_Fex"):
        super(EFMissingET_Fex, self).__init__(name)

        # name of TrigMissingET object
        self.MissingETOutputKey = "TrigEFMissingET"

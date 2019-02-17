# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigCaloRec.TrigCaloRecConf import TrigCaloClusterMakerMT
from TrigCaloRec.TrigCaloRecConf import TrigCaloTowerMakerMT
from TrigCaloRec.TrigCaloRecConf import TrigCaloCellMakerMT
from TrigCaloRec.TrigCaloRecConf import TrigCaloCellMaker
from TrigCaloRec.TrigCaloRecConf import TrigFullCaloCellMaker
from TrigCaloRec.TrigCaloRecConf import TrigCaloTowerMaker
from TrigCaloRec.TrigCaloRecConf import TrigCaloClusterMaker
from TrigCaloRec.TrigCaloRecConf import TrigCaloTopoTowerAlgorithm
from TrigCaloRec.TrigCaloRecConf import TrigCaloCell2ClusterMapper
from TrigCaloRec.TrigCaloRecConf import TrigFullCaloClusterMaker
from TrigCaloRec.TrigCaloRecConf import RoILArEMCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoILArHadCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoITileCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoIFCalEmCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoIFCalHadCellContMaker
from TrigCaloRec.TrigCaloRecConf import FullCaloCellContMaker
from TrigCaloRec.TrigCaloRecConf import TrigLArNoisyROAlg
from TrigCaloRec.TrigCaloRecConf import TrigL1BSTowerHypo
from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigCaloRec.TrigCaloCellMakerMonitoring import TrigCaloCellMakerValidationMonitoring, TrigCaloCellMakerFCalValidationMonitoring, TrigCaloCellMakerOnlineMonitoring, TrigCaloCellMakerCosmicMonitoring
from TrigCaloRec.TrigCaloCellMakerMonitoring import TrigFullCaloCellMakerValidationMonitoring, TrigFullCaloCellMakerOnlineMonitoring, TrigFullCaloCellMakerCosmicMonitoring
from TrigCaloRec.TrigCaloTowerMakerMonitoring import TrigCaloTowerMakerValidationMonitoring, TrigCaloTowerMakerOnlineMonitoring, TrigCaloTowerMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigCaloClusterMakerValidationMonitoring, TrigCaloClusterMakerOnlineMonitoring, TrigCaloClusterMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigFullCaloClusterMakerValidationMonitoring, TrigFullCaloClusterMakerOnlineMonitoring, TrigFullCaloClusterMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigL1BSTowerHypoOnlineMonitoring
from CaloClusterCorrection.common import *
from CaloUtils.CaloUtilsConf import *

# MT stuff
from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker as _HLTCaloCellMaker


from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE
from AthenaCommon.SystemOfUnits import GeV,MeV,deg
from AthenaCommon.Logging import logging
import traceback

mlog = logging.getLogger ('TrigCaloRecConfig')

class TrigCaloCell2ClusterMapperBase (TrigCaloCell2ClusterMapper):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloCell2ClusterMapperBase, self ).__init__(name)

class TrigCaloCellMakerBase (TrigCaloCellMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloCellMakerBase, self ).__init__(name)

        cellvalidation = TrigCaloCellMakerValidationMonitoring()
        cellonline = TrigCaloCellMakerOnlineMonitoring()
        cellcosmic = TrigCaloCellMakerCosmicMonitoring()

        celltime = TrigTimeHistToolConfig("TrigCaloCellMaker_Time")

        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic]

class TrigCaloCellMakerFCalBase (TrigCaloCellMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloCellMakerFCalBase, self ).__init__(name)

        cellvalidation = TrigCaloCellMakerFCalValidationMonitoring()
        cellonline = TrigCaloCellMakerOnlineMonitoring()
        cellcosmic = TrigCaloCellMakerCosmicMonitoring()

        celltime = TrigTimeHistToolConfig("TrigCaloCellMaker_Time")

        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic]

# use the TrigCaloCellMaker as base class, yet 
# use the fullcalo monitoring 
class TrigFullCaloCellMakerFCalBase (TrigCaloCellMaker): 
    __slots__ = [] 
    def __init__(self, name): 
        super( TrigFullCaloCellMakerFCalBase, self ).__init__(name) 

        cellvalidation = TrigFullCaloCellMakerValidationMonitoring() 
        cellonline = TrigFullCaloCellMakerOnlineMonitoring() 
        cellcosmic = TrigFullCaloCellMakerCosmicMonitoring() 
 
        celltime = TrigTimeHistToolConfig("TrigFullCaloCellMaker_Time") 
 
        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic] 
 

class TrigFullCaloCellMakerBase (TrigFullCaloCellMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigFullCaloCellMakerBase, self ).__init__(name)

        fullcellvalidation = TrigFullCaloCellMakerValidationMonitoring()
        fullcellonline = TrigFullCaloCellMakerOnlineMonitoring()
        fullcellcosmic = TrigFullCaloCellMakerCosmicMonitoring()

        fullcelltime = TrigTimeHistToolConfig("TrigFullCaloCellMaker_Time")

        self.AthenaMonTools = [ fullcellvalidation, fullcellonline, fullcelltime, fullcellcosmic]


class TrigCaloTowerMakerBase (TrigCaloTowerMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloTowerMakerBase, self ).__init__(name)

        towervalidation = TrigCaloTowerMakerValidationMonitoring()
        toweronline = TrigCaloTowerMakerOnlineMonitoring()
        towercosmic = TrigCaloTowerMakerCosmicMonitoring()

        towertime = TrigTimeHistToolConfig("TrigCaloTowerMaker_Time")
      #  towertime.TimerPerObjHistLimits = [0,1000]
      #  towertime.TimerHistLimits  = [0,1000]
        towertime.TimerHistLimits  = [0,40]

        self.AthenaMonTools = [ towervalidation, toweronline, towertime, towercosmic]


class TrigCaloClusterMakerBase (TrigCaloClusterMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloClusterMakerBase, self ).__init__(name)

        clvalidation = TrigCaloClusterMakerValidationMonitoring()
        clonline = TrigCaloClusterMakerOnlineMonitoring()
        clcosmic = TrigCaloClusterMakerCosmicMonitoring()

        cltime = TrigTimeHistToolConfig("TrigCaloClusterMaker_Time")
      #  cltime.TimerPerObjHistLimits = [0,1000]
      #  cltime.TimerHistLimits  = [0,1000]

        self.AthenaMonTools = [ clvalidation, clonline, cltime, clcosmic]

class TrigCaloTopoTowerMakerBase (TrigCaloTopoTowerAlgorithm):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloTopoTowerMakerBase, self ).__init__(name)

class TrigFullCaloClusterMakerBase (TrigFullCaloClusterMaker):
    __slots__ = []
    def __init__(self, name):
        super( TrigFullCaloClusterMakerBase, self ).__init__(name)

        clvalidation = TrigFullCaloClusterMakerValidationMonitoring()
        clonline = TrigFullCaloClusterMakerOnlineMonitoring()
        clcosmic = TrigFullCaloClusterMakerCosmicMonitoring()

        cltime = TrigTimeHistToolConfig("TrigFullCaloClusterMaker_Time")

        self.AthenaMonTools = [ clvalidation, clonline, cltime, clcosmic]


class TrigCaloCellMaker_eGamma (TrigCaloCellMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_eGamma'):
        super(TrigCaloCellMaker_eGamma, self).__init__(name)

        self.eta_size = 16
        self.phi_size = 16
 
        self.CreateRoiForID = True
        self.EtaWidthForID  = 0.1
        self.PhiWidthForID  = 0.1
       
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0

        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roitilehadcellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

class TrigCaloCellMaker_eGamma_LargeRoI (TrigCaloCellMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_eGamma_LargeRoI'):
        super(TrigCaloCellMaker_eGamma_LargeRoI, self).__init__(name)

        self.eta_size = 24
        self.phi_size = 24

        self.CreateRoiForID = True
        self.EtaWidthForID  = 0.1
        self.PhiWidthForID  = 0.1

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool
	
        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
	
        self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roitilehadcellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]



class TrigCaloCellMaker_eGamma_cells (TrigCaloCellMaker_eGamma):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_eGamma_cells'):
        super(TrigCaloCellMaker_eGamma_cells, self).__init__(name)

        self.PersistencyPrescaleFactor=1
        self.PersistencyKeyName="TrigCaloCellMaker_eGamma"
        
class TrigCaloCellMaker_jet (TrigCaloCellMakerFCalBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_jet', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMaker_jet, self).__init__(name)

        self.eta_size = eta_size
        self.phi_size = phi_size

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalemcellcontmaker = RoIFCalEmCellContMaker()
        roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
        roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        roilaremcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilaremcellcontmaker.AbsEinSigma = AbsE
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilarhadcellcontmaker.AbsEinSigma = AbsE
        roifcalemcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roitilehadcellcontmaker,
                         roifcalemcellcontmaker,
                         roifcalhadcellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

        #self.OutputLevel=INFO


class TrigCaloCellMaker_super (TrigCaloCellMaker_jet):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_super', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMaker_super, self).__init__(name)
        self.TrustRoiLimits = True


class TrigCaloCellMaker_jet_super (TrigCaloCellMakerFCalBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_jet_super', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMaker_jet_super, self).__init__(name)

        self.TrustRoiLimits = True

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalemcellcontmaker = RoIFCalEmCellContMaker()
        roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
        roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        roilaremcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilaremcellcontmaker.AbsEinSigma = AbsE
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilarhadcellcontmaker.AbsEinSigma = AbsE
        roifcalemcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roitilehadcellcontmaker,
                         roifcalemcellcontmaker,
                         roifcalhadcellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

        #self.OutputLevel=INFO                                                                                                                                       



class TrigCaloCellMaker_jet_fullcalo (TrigFullCaloCellMakerFCalBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_jet_fullcallo', eta_size=64, phi_size=64, doNoise=1, AbsE=False, doPers=False):
        super(TrigCaloCellMaker_jet_fullcalo, self).__init__(name)

        # these aren't actually used in this instance
        # self.eta_size = eta_size
        # self.phi_size = phi_size
        
        # self.FullScanEnabled = True

        self.TrustRoiLimits = True

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        fullcalocellcontmaker = FullCaloCellContMaker(DoLArCellsNoiseSuppression = doNoise, AbsEinSigma = AbsE,CaloNoiseTool = theCaloNoiseTool)

        self.TCRTools = [fullcalocellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName()]
                               #CaloCellContainerFinalizerTool().getFullName(),
                               #CaloCellContainerCheckerTool().getFullName()]

        if doPers:
           self.PersistencyPrescaleFactor=1
           self.PersistencyKeyName="TrigCaloCellMaker_jet_fullcalo"

        #self.OutputLevel=INFO


class TrigCaloCellMaker_fullcalo (TrigFullCaloCellMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_fullcalo'):
        super(TrigCaloCellMaker_fullcalo, self).__init__(name)

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        fullcalocellcontmaker = FullCaloCellContMaker()
        fullcalocellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.TCRTools = [fullcalocellcontmaker]
                         
        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName()]
        #                       CaloCellContainerFinalizerTool().getFullName(),
        #                       CaloCellContainerCheckerTool().getFullName()]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName()]
        #self.OutputLevel=INFO

        #### configure TrigDataAccess for loadFullCollections
        from TrigT2CaloCommon.TrigT2CaloCommonConfig import TrigDataAccess
        ToolSvc+=TrigDataAccess()
        ToolSvc.TrigDataAccess.loadFullCollections = True


class TrigCaloCellMaker_tau (TrigCaloCellMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_tau'):
        super(TrigCaloCellMaker_tau, self).__init__(name)

        self.eta_size = 32
        self.phi_size = 32

        self.CreateRoiForID = True
        self.EtaWidthForID  = 0.2
        self.PhiWidthForID  = 0.2

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CheckCellWithinRoI = True
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
	
        self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roitilehadcellcontmaker]

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

class TrigCaloCellMaker_tau_cells (TrigCaloCellMaker_tau):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_tau_cells'):
        super(TrigCaloCellMaker_tau_cells, self).__init__(name)

        self.PersistencyPrescaleFactor=1
        self.PersistencyKeyName="TrigCaloCellMaker_tau"
        
class TrigCaloTowerMaker_eGamma (TrigCaloTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMaker_eGamma'):
        super(TrigCaloTowerMaker_eGamma, self).__init__(name)

        #input to  LArTowerMBuilder:  Cells in LArEM 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False

        
        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",
                                              CellContainerName = "RoIEMCalo",
                                              IncludedCalos     = [ "LAREM" ],
                                              OutputLevel=ERROR
                                              )
        #lartowerbuilder.CellContainerName = "RoIEMCalo" #useless
        #lartowerbuilder.IncludedCalos     = [ "LAREM" ]
        #lartowerbuilder.OutputLevel=ERROR
        self +=lartowerbuilder
        self.TowerMakerTools=[ lartowerbuilder.getFullName() ]
        self.NumberOfPhiTowers=256
        self.NumberOfEtaTowers=200
        self.EtaMin=-2.5
        self.EtaMax=2.5
        self.DeltaEta=0.5
        self.DeltaPhi=0.5
        

class TrigCaloTowerMaker_jet (TrigCaloTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMaker_jet'):
        super(TrigCaloTowerMaker_jet, self).__init__(name)

        #input to  TileTowerBuilder:  cells in TILE
        try:
            from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
        except:
            mlog.error("could not get handle to TileTowerBuilderTool Quit")
            print traceback.format_exc()
            return False
        
        # input to LArTowerBuilder:  cells in LArEM and LARHEC 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool or/and LArFCalTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False


        larcmbtwrbldr = LArTowerBuilderTool("LArCmbTwrBldr",
                                            CellContainerName = "RoIEMCalo",
                                            IncludedCalos     = [ "LAREM", "LARHEC" ],
                                            OutputLevel=ERROR
                                            )

        #larcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #larcmbtwrbldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
        #larcmbtwrbldr.OutputLevel=ERROR
        
        fcalcmbtwrbldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr",
                                                 CellContainerName = "RoIEMCalo",
                                                 MinimumEt         = 0.*MeV,
                                                 OutputLevel=ERROR
                                                 )

        #fcalcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #fcalcmbtwrbldr.MinimumEt         = 0.*MeV
        #fcalcmbtwrbldr.OutputLevel=ERROR

        tilecmbtwrbldr = TileTowerBuilderTool("TileCmbTwrBldr",
                                              CellContainerName = "RoIEMCalo",
                                              DumpTowers        = FALSE,
                                              DumpWeightMap     = FALSE
                                              )
        #tilecmbtwrbldr.CellContainerName = "RoIEMCalo"
        #tilecmbtwrbldr.DumpTowers        = FALSE
        #tilecmbtwrbldr.DumpWeightMap     = FALSE

        
        self +=larcmbtwrbldr
        self +=fcalcmbtwrbldr
        self +=tilecmbtwrbldr
        self.NumberOfPhiTowers=64
        self.NumberOfEtaTowers=100
        self.EtaMin=-5.0
        self.EtaMax=5.0
        self.DeltaEta=1.2
        self.DeltaPhi=1.2
        self.TowerMakerTools = [ tilecmbtwrbldr.getFullName(), larcmbtwrbldr.getFullName(), fcalcmbtwrbldr.getFullName() ]
        
        
class TrigCaloTowerMaker_tau (TrigCaloTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMaker_tau'):
        super(TrigCaloTowerMaker_tau, self).__init__(name)

        #input to  LArTowerMBuilder:  Cells in LArEM 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False

        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",
                                              CellContainerName = "RoIEMCalo",
                                              IncludedCalos     = [ "LAREM" ],
                                              OutputLevel=ERROR     
                                              )

        #lartowerbuilder.CellContainerName = "RoIEMCalo"
        #lartowerbuilder.IncludedCalos     = [ "LAREM" ]
        #lartowerbuilder.OutputLevel=ERROR

        self +=lartowerbuilder
        self.NumberOfPhiTowers=256
        self.NumberOfEtaTowers=200
        self.EtaMin=-2.5
        self.EtaMax=2.5
        self.DeltaEta=0.8
        self.DeltaPhi=0.8
        self.TowerMakerTools=[lartowerbuilder.getFullName()]
        
class TrigCaloClusterMaker_slw (TrigCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_slw'):
        super(TrigCaloClusterMaker_slw, self).__init__(name)

        try:
            from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
        except:
            mlog.error("TrigCaloRec: could not get handle to make_CaloClusterBuilderSW Quit")
            print traceback.format_exc()
            return False
        
        
        trigslw= make_CaloClusterBuilderSW ("trigslw",
                                            tower_container = "LArTowerEM",
                                            eta_size = 3,
                                            phi_size = 5,
                                            #eta_sizep = 3,
                                            #phi_sizep = 3,
                                            e_threshold = 2.5 * GeV,
                                            FillClusterCells = False,
                                            #nextra = 0,
                                            #eta_SeedGrid = 5,
                                            #phi_SeedGrid = 5,
                                            eta_Duplicate = 5,
                                            phi_Duplicate = 5
                                            )
        self +=trigslw        
        self.ClusterMakerTools=[ trigslw.getFullName() ]
        self.ClustersOutputName="TriggerClustersegSW"

        try:
            from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
            for tool in make_CaloSwCorrections ("ele37"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
            for tool in make_CaloSwCorrections ("ele55"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloSwCorrections_compat Quit")
            print traceback.format_exc()
            return False

class TrigCaloClusterMaker_slw_fullcalo (TrigFullCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_slw_fullcalo'):
        super(TrigCaloClusterMaker_slw_fullcalo, self).__init__(name)

        try:
            from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
        except:
            mlog.error("TrigCaloRec: could not get handle to make_CaloClusterBuilderSW Quit")
            print traceback.format_exc()
            return False

        
        trigslw= make_CaloClusterBuilderSW ("trigslw",
                                            tower_container = "LArTowerEM",
                                            eta_size = 3,
                                            phi_size = 5,
                                            #eta_sizep = 3,
                                            #phi_sizep = 3,
                                            e_threshold = 2.5 * GeV,
                                            FillClusterCells = False,
                                            #nextra = 0,
                                            #eta_SeedGrid = 5,
                                            #phi_SeedGrid = 5,
                                            eta_Duplicate = 5,
                                            phi_Duplicate = 5
                                            )
        self +=trigslw        
        self.ClusterMakerTools=[ trigslw.getFullName() ]
        self.ClustersOutputName="TriggerClustersegSW"
        
        try:
            from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
            for tool in make_CaloSwCorrections ("ele37"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
            for tool in make_CaloSwCorrections ("ele55"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloSwCorrections_compat Quit")
            print traceback.format_exc()
            return False

class TrigCaloClusterMaker_topo (TrigCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_topo', doMoments=True, doLC=True ):
        super(TrigCaloClusterMaker_topo, self).__init__(name)

        try:
            from CaloUtils.CaloUtilsConf import CaloLCWeightTool, CaloLCClassificationTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
            from CaloRec.CaloTopoClusterFlags import jobproperties
            from AthenaCommon.SystemOfUnits import deg
            from AthenaCommon.AlgSequence import AlgSequence
            from AthenaCommon.GlobalFlags import globalflags
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            print traceback.format_exc()
            return False

        print jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib()
        # tools used by tools
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault() #flag='tool', name='myCaloNoiseToolDefault')
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        if doLC:
        
          TrigLCClassify   = CaloLCClassificationTool("TrigLCClassify")
          TrigLCClassify.ClassificationKey   = "EMFracClassify"
          TrigLCClassify.UseSpread = False
          TrigLCClassify.MaxProbability = 0.5
          TrigLCClassify.StoreClassificationProbabilityInAOD = True

          TrigLCWeight = CaloLCWeightTool("TrigLCWeight")
          TrigLCWeight.CorrectionKey       = "H1ClusterCellWeights"
          TrigLCWeight.SignalOverNoiseCut  = 2.0
          TrigLCWeight.CaloNoiseTool       = theCaloNoiseTool 
          TrigLCWeight.UseHadProbability   = True

          TrigLCOut     = CaloLCOutOfClusterTool("TrigLCOut")
          TrigLCOut.CorrectionKey       = "OOCCorrection"
          TrigLCOut.UseEmProbability    = False
          TrigLCOut.UseHadProbability   = True

          TrigLCOutPi0  = CaloLCOutOfClusterTool("TrigLCOutPi0")
          TrigLCOutPi0.CorrectionKey    = "OOCPi0Correction"
          TrigLCOutPi0.UseEmProbability  = True
          TrigLCOutPi0.UseHadProbability = False

          TrigLCDeadMaterial   = CaloLCDeadMaterialTool("TrigLCDeadMaterial")
          TrigLCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
          #TrigLCDeadMaterial.SignalOverNoiseCut  = 1.0
          TrigLCDeadMaterial.ClusterRecoStatus   = 0
          TrigLCDeadMaterial.WeightModeDM        = 2
          #TrigLCDeadMaterial.CaloNoiseTool       = theCaloNoiseTool
          TrigLCDeadMaterial.UseHadProbability   = True

          # correction tools using tools
          TrigLocalCalib = CaloClusterLocalCalib ("TrigLocalCalib")
          TrigLocalCalib.ClusterClassificationTool     = [TrigLCClassify]
          #TrigLocalCalib.ClusterRecoStatus             = [2]
          TrigLocalCalib.ClusterRecoStatus             = [1,2]
          TrigLocalCalib.LocalCalibTools               = [TrigLCWeight]

          TrigLocalCalib += TrigLCClassify
          TrigLocalCalib += TrigLCWeight

          TrigOOCCalib   = CaloClusterLocalCalib ("TrigOOCCalib")
          #TrigOOCCalib.ClusterRecoStatus   = [2]
          TrigOOCCalib.ClusterRecoStatus   = [1,2]
          TrigOOCCalib.LocalCalibTools     = [TrigLCOut]

          TrigOOCCalib += TrigLCOut

          TrigOOCPi0Calib   = CaloClusterLocalCalib ("TrigOOCPi0Calib")
          #OOCPi0Calib.ClusterRecoStatus   = [1]
          TrigOOCPi0Calib.ClusterRecoStatus   = [1,2]
          TrigOOCPi0Calib.LocalCalibTools     = [TrigLCOutPi0]

          TrigOOCPi0Calib += TrigLCOutPi0


          TrigDMCalib    = CaloClusterLocalCalib ("TrigDMCalib")
          TrigDMCalib.ClusterRecoStatus   = [1,2]
          TrigDMCalib.LocalCalibTools     = [TrigLCDeadMaterial]

          TrigDMCalib += TrigLCDeadMaterial

       
        if doMoments:
 
          # correction tools not using tools
          TrigTopoMoments = CaloClusterMomentsMaker ("TrigTopoMoments")
          TrigTopoMoments.OutputLevel = INFO
          TrigTopoMoments.MaxAxisAngle = 20*deg
          TrigTopoMoments.CaloNoiseTool = theCaloNoiseTool
          TrigTopoMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
          TrigTopoMoments.UsePileUpNoise = True
          TrigTopoMoments.MinBadLArQuality = 4000
          TrigTopoMoments.MomentsNames = ["FIRST_PHI" 
                                          ,"FIRST_ETA"
                                          ,"SECOND_R" 
                                          ,"SECOND_LAMBDA"
                                          ,"DELTA_PHI"
                                          ,"DELTA_THETA"
                                          ,"DELTA_ALPHA" 
                                          ,"CENTER_X"
                                          ,"CENTER_Y"
                                          ,"CENTER_Z"
                                          ,"CENTER_MAG"
                                          ,"CENTER_LAMBDA"
                                          ,"LATERAL"
                                          ,"LONGITUDINAL"
                                          ,"FIRST_ENG_DENS" 
                                          ,"ENG_FRAC_EM" 
                                          ,"ENG_FRAC_MAX" 
                                          ,"ENG_FRAC_CORE" 
                                          ,"FIRST_ENG_DENS" 
                                          ,"SECOND_ENG_DENS" 
                                          ,"ISOLATION"
                                          ,"ENG_BAD_CELLS"
                                          ,"N_BAD_CELLS"
                                          ,"N_BAD_CELLS_CORR"
                                          ,"BAD_CELLS_CORR_E"
                                          ,"BADLARQ_FRAC"
                                          ,"ENG_POS"
                                          ,"SIGNIFICANCE"
                                          ,"CELL_SIGNIFICANCE"
                                          ,"CELL_SIG_SAMPLING"
                                          ,"AVG_LAR_Q"
                                          ,"AVG_TILE_Q"
                                          ]          
        #TrigLockVariables = CaloClusterLockVars("TrigLockVariables")
        #TrigLockVariables.FixBasicEnergy = True
        #TrigLockVariables.LockedSamplingVariables = []
        #TrigLockVariables.LockedSamplingVariables += [
        #    "Energy", "Max_Energy"]
        #TrigLockVariables.LockedSamplingVariables += [
        #    "Eta", "Phi", "Delta_Eta",
        #    "Delta_Phi", "Max_Eta", "Max_Phi"
        #    ]
        
        # maker tools
        TrigTopoMaker = CaloTopoClusterMaker("TrigTopoMaker")

        TrigTopoMaker.CellsName = "AllCalo"
        TrigTopoMaker.CalorimeterNames=["LAREM",
                                        "LARHEC",
                                        "LARFCAL",
                                        "TILE"]
        # cells from the following samplings will be able to form
        # seeds. By default no sampling is excluded
        TrigTopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                           "PreSamplerE", "EME1", "EME2", "EME3",
                                           "HEC0", "HEC1","HEC2", "HEC3",
                                           "TileBar0", "TileBar1", "TileBar2",
                                           "TileExt0", "TileExt1", "TileExt2",
                                           "TileGap1", "TileGap2", "TileGap3",
                                           "FCAL0", "FCAL1", "FCAL2"]

        TrigTopoMaker.CaloNoiseTool=theCaloNoiseTool
        TrigTopoMaker.UseCaloNoiseTool=True
        TrigTopoMaker.UsePileUpNoise=True
        TrigTopoMaker.NeighborOption = "super3D"
        TrigTopoMaker.RestrictHECIWandFCalNeighbors  = False
        TrigTopoMaker.CellThresholdOnEorAbsEinSigma     =    0.0
        TrigTopoMaker.NeighborThresholdOnEorAbsEinSigma =    2.0  #instead of 2
        TrigTopoMaker.SeedThresholdOnEorAbsEinSigma     =    4.0  #instead of 4
        # note Et or AbsEt
        #TrigTopoMaker.NeighborCutsInAbsE              = False
        #TrigTopoMaker.CellCutsInAbsE                 = False
        #
        # the following cut must be set to TRUE in order to make double
        # sided cuts on the seed and the cluster level ( neighbor and cell
        # cuts are always double sided)
        #
        TrigTopoMaker.SeedCutsInAbsE                 = True
        TrigTopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
        # the following Et thresholds are ignored in case UsePileUpNoise
        # is TRUE
        #
        #
        #CellThresholdOnEtorAbsEt = 0.0*MeV
        #NeighborThresholdOnEtorAbsEt = 100.0*MeV
        #SeedThresholdOnEtorAbsEt = 200.0*MeV

        # use 2-gaussian or single gaussian noise for TileCal
        TrigTopoMaker.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()

        TrigTopoSplitter = CaloTopoClusterSplitter("TrigTopoSplitter")        
        # cells from the following samplings will be able to form local
        # maxima. The excluded samplings are PreSamplerB, EMB1,
        # PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
        # two rear FCal samplings.
        #
        TrigTopoSplitter.SamplingNames = ["EMB2", "EMB3",
                                          "EME2", "EME3",
                                          "FCAL0"]
        # cells from the following samplings will also be able to form
        # local maxima but only if they are not overlapping in eta and phi
        # with local maxima in previous samplings from the primary list.
        #
        TrigTopoSplitter.SecondarySamplingNames = ["EMB1","EME1",
                                                   "TileBar0","TileBar1","TileBar2",
                                                   "TileExt0","TileExt1","TileExt2",
                                                   "HEC0","HEC1","HEC2","HEC3",
                                                   "FCAL1","FCAL2"]
        TrigTopoSplitter.ShareBorderCells = True
        TrigTopoSplitter.RestrictHECIWandFCalNeighbors  = False
        #
        # the following options are not set, since these are the default
        # values
        #
        # NeighborOption                = "super3D",
        # NumberOfCellsCut              = 4,
        # EnergyCut                     = 500*MeV,


        # cluster maker
        #TrigCaloTopoCluster = CaloClusterMaker ("TrigCaloTopoCluster")
        #TrigCaloTopoCluster.ClustersOutputName="CaloCalTopoCluster"
        #TrigCaloTopoCluster.ClusterMakerTools = [

        self.ClustersOutputName="CaloCalTopoCluster"
        
        if not doMoments:
          self.ClusterMakerTools = [ TrigTopoMaker.getFullName(), TrigTopoSplitter.getFullName()]
        else:
          self.ClusterMakerTools = [ TrigTopoMaker.getFullName(), TrigTopoSplitter.getFullName(),  TrigTopoMoments.getFullName()]

        # do not use BadChannelListCorr since this is not used for jet and tau in offline
        #TrigBadChannelListCorr = CaloClusterBadChannelListCorr()
        #self.ClusterCorrectionTools += [TrigBadChannelListCorr.getFullName()]
        
        self += TrigTopoMaker
        self += TrigTopoSplitter
        #self += TrigBadChannelListCorr
        if doMoments:
          self += TrigTopoMoments

        self.ClusterCorrectionTools = [  ]
        #self.ClusterCorrectionTools = [ TrigLockVariables.getFullName() ]
        #self += TrigLockVariables

        if doLC:
          self.ClusterCorrectionTools += [ TrigLocalCalib.getFullName(),
                                           TrigOOCCalib.getFullName(),
                                           TrigOOCPi0Calib.getFullName(),
                                           TrigDMCalib.getFullName()]
          self += TrigLocalCalib
          self += TrigOOCCalib
          self += TrigOOCPi0Calib
          self += TrigDMCalib

        #
        # pool/cool part
        #
        if doLC:  
          if globalflags.DetDescrVersion().startswith("Rome"):
             self.TrigLocalCalib.TrigLCClassify.MaxProbability = 0.85
             self.TrigLocalCalib.TrigLCClassify.UseNormalizedEnergyDensity = False
          else:
             self.TrigLocalCalib.TrigLCClassify.MaxProbability = 0.50
             self.TrigLocalCalib.TrigLCClassify.UseNormalizedEnergyDensity = True

        from CaloRec import CaloClusterTopoCoolFolder

class TrigCaloTopoTowerMaker_jet (TrigCaloTopoTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTopoTowerMaker_jet'):
        super(TrigCaloTopoTowerMaker_jet, self).__init__(name)

        # configure CaloTopoTowerAlgorithm here
        try:
            from CaloUtils.CaloUtilsConf import CaloTopoTowerBuilderTool
            theCaloTopoTowerBuilderTool=CaloTopoTowerBuilderTool("TrigTopoTowerTwrBldr")
        except:
            mlog.error("could not get handle to CaloTopoTowerBuilderTool Quit")
            print traceback.format_exc()
            return False
        
        self.TowerBuilderTools= [ theCaloTopoTowerBuilderTool] 

        #setting properties of the algorithm
        self.MinimumCellEnergy      = -1000000000.0
        self.MinimumClusterEnergy   = -1000000000.0
        self.CellEnergySignificance = -1.
        self.UseCaloNoiseTool       = False
        self.OutputTowerContainerName = "TrigCaloTopoTowerMaker_jet"


def AddFolderCheck(folder, tag):
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    folders = svcMgr.IOVDbSvc.Folders
    found=False
    for text in folders:
        if text.find("/CALO/HadCalibration/"+folder)>=0:
            found=True
    if ( not found ):
        conddb.addFolder("CALO","/CALO/HadCalibration/"+folder+tag)

class TrigCaloClusterMaker_EMtopo (TrigCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_EMtopo'):
        super(TrigCaloClusterMaker_EMtopo, self).__init__(name)

        try:
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
            from CaloRec.CaloTopoClusterFlags import jobproperties 
            emtopocluster = CaloTopoClusterMaker("EMTrigTopoCluster")
            emtoposplitter = CaloTopoClusterSplitter("EMTrigTopoSplitter")
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            print traceback.format_exc()
            return False
        
        emtopocluster.CellsNames=["RoIEMCalo"]
        emtopocluster.CalorimeterNames=[ "LAREM" ]
        #
        # cells from the following samplings will be able to form seeds. By default 
        # no sampling is excluded 
        #
        emtopocluster.OutputLevel=VERBOSE
        emtopocluster.SeedSamplingNames = [
            "PreSamplerB", "EMB1", "EMB2", 
            "PreSamplerE", "EME1", "EME2"  ]
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool
        emtopocluster.CaloNoiseTool=theCaloNoiseTool

        emtopocluster.UseCaloNoiseTool=True
        emtopocluster.UsePileUpNoise=False
        
        emtopocluster.NeighborOption                 = "all3D"
        emtopocluster.CellThresholdOnEorAbsEinSigma     =    0.0
        emtopocluster.NeighborThresholdOnEorAbsEinSigma =    2.0
        emtopocluster.SeedThresholdOnEorAbsEinSigma     =    4.0
        # note E or AbsE 
        #
        # the following cut must be set to TRUE in order to make double sided
        # cuts on the seed and the cluster level ( neighbor and cell cuts are 
        # always double sided)
        #
        emtopocluster.SeedCutsInAbsE                 = TRUE
        emtopocluster.ClusterEtorAbsEtCut            = 1*GeV
        #
        # the following Et thresholds are ignored in case UsePileUpNoise is TRUE
        #
        # emtopocluster.CellThresholdOnAbsEt           =    0.0*MeV  
        # emtopocluster.NeighborThresholdOnAbsEt       =  100.0*MeV
        # emtopocluster.SeedThresholdOnEtorAbsEt       =  200.0*MeV # note Et or AbsEt
        
        
        #
        # cells from the following samplings will be able to form local maxima. The 
        # excluded samplings are PreSamplerB, EMB1, PreSamplerE, EME1,
        # and all samplings from Tile, HEC and FCAL
        #
        emtoposplitter.OutputLevel=VERBOSE
        emtoposplitter.SamplingNames = ["EMB2", "EME2"]
        emtoposplitter.SecondarySamplingNames = ["EMB1", "EME1"]
        emtoposplitter.ShareBorderCells = True
        emtoposplitter.RestrictHECIWandFCalNeighbors  = False        
        # the following options are not set, since these are the default values
        # the following options are not set, since these are the default values
        #
        # emtoposplitter.NumberOfCellsCut              = 4
        # emtoposplitter.EtDensityCut                  = 500*MeV/(600000*mm3) # this corresponds to 500 MeV in a typical EM Layer 2 cell
        
        
        try:
            from CaloRec.CaloRecConf import CaloClusterMomentsMaker
            emtopomoments = CaloClusterMomentsMaker("EMTrigTopoMoments")
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloClusterMomentsMaker. Quit")
            print traceback.format_exc()
            return False
        
        emtopomoments.OutputLevel = INFO
        emtopomoments.MaxAxisAngle = 20*deg
        emtopomoments.CaloNoiseTool = theCaloNoiseTool
        emtopomoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
        emtopomoments.UsePileUpNoise = True
        emtopomoments.MinBadLArQuality = 4000
        emtopomoments.MomentsNames = ["FIRST_PHI" 
                                      ,"FIRST_ETA"
                                      ,"SECOND_R" 
                                      ,"SECOND_LAMBDA"
                                      ,"DELTA_PHI"
                                      ,"DELTA_THETA"
                                      ,"DELTA_ALPHA" 
                                      ,"CENTER_X"
                                      ,"CENTER_Y"
                                      ,"CENTER_Z"
                                      ,"CENTER_MAG"
                                      ,"CENTER_LAMBDA"
                                      ,"LATERAL"
                                      ,"LONGITUDINAL"
                                      ,"FIRST_ENG_DENS" 
                                      ,"ENG_FRAC_EM" 
                                      ,"ENG_FRAC_MAX" 
                                      ,"ENG_FRAC_CORE" 
                                      ,"FIRST_ENG_DENS" 
                                      ,"SECOND_ENG_DENS" 
                                      ,"ISOLATION"
                                      ,"ENG_BAD_CELLS"
                                      ,"N_BAD_CELLS"
                                      ,"N_BAD_CELLS_CORR"
                                      ,"BAD_CELLS_CORR_E"
                                      ,"BADLARQ_FRAC"
                                      ,"ENG_POS"
                                      ,"SIGNIFICANCE"
                                      ,"CELL_SIGNIFICANCE"
                                      ,"CELL_SIG_SAMPLING"
                                      ,"AVG_LAR_Q"
                                      ,"AVG_TILE_Q"
                                      ]          
        
        self.ClusterMakerTools = [emtopocluster.getFullName(), emtoposplitter.getFullName(), emtopomoments.getFullName()]
        #self.ClusterMakerTools = [emtopocluster.getFullName(), emtoposplitter.getFullName()]
        #self.ClusterCorrectionTools = [ emtopomoments.getFullName() ]
        self.ClustersOutputName="TriggerClustersegTopo"
        self += emtopocluster
        self += emtoposplitter
        self += emtopomoments

class TrigLArNoisyROAlgConfig (TrigLArNoisyROAlg):
     __slots__ = []
     def __init__(self, name='TrigLArNoisyROAlgConfig'):
         super( TrigLArNoisyROAlgConfig, self ).__init__(name)
         self.BadChanPerFEB=20
         self.CellQualityCut=1000

class TrigL1BSTowerHypoConfig (TrigL1BSTowerHypo):
    __slots__ = []
    def __init__(self, name='TrigL1BSTowerHypoConfig'):
        super( TrigL1BSTowerHypoConfig, self ).__init__(name)

        toweronline = TrigL1BSTowerHypoOnlineMonitoring()

        towertime = TrigTimeHistToolConfig("TrigL1BSTower_Time")

        self.AthenaMonTools = [ toweronline, towertime]


### Predefined HLTCalo Alg Sequences (For Menu Development): ###

def TopoClustering_ROI():
    
    te_in  = ""
    te_out = ""
    
    algseq = [TrigCaloCellMaker_tau(), TrigCaloClusterMaker_topo()]
    
    return [te_in, algseq, te_out]

def TopoClustering_PS():
    
    te_in  = ""
    te_out = "TopoClusters_PS"
    
    algseq = [TrigCaloCellMaker_jet_super(doNoise=0, AbsE=True), TrigCaloClusterMaker_topo()]
    
    return [te_in, algseq, te_out]

def TopoClustering_FS():     
    
    te_in  = ""
    te_out = "TopoClusters_FS"
    
    algseq = [PESA__DummyUnseededAllTEAlgo("DummyAlgo"), TrigCaloCellMaker_jet_fullcalo(doNoise=0, AbsE=True, doPers=True), TrigCaloClusterMaker_topo()]
    
    return [te_in, algseq, te_out]

def SLW_Clustering():
    
    te_in  = ""
    te_out = ""
    
    algseq = [TrigCaloCellMaker_eGamma(), TrigCaloTowerMaker_eGamma(), TrigCaloClusterMaker_slw()]
    
    return [te_in, algseq, te_out]


# MT clases


class TrigCaloCellMakerMTBase (TrigCaloCellMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloCellMakerMTBase, self ).__init__(name)
        
        #        cellvalidation = TrigCaloCellMakerValidationMonitoring()
#        cellonline = TrigCaloCellMakerOnlineMonitoring()
#        cellcosmic = TrigCaloCellMakerCosmicMonitoring()

#        celltime = TrigTimeHistToolConfig("TrigCaloCellMaker_Time")

#        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic]




class TrigCaloCellMakerFCalMTBase (TrigCaloCellMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloCellMakerFCalMTBase, self ).__init__(name)

#        cellvalidation = TrigCaloCellMakerFCalValidationMonitoring()
#        cellonline = TrigCaloCellMakerOnlineMonitoring()
#        cellcosmic = TrigCaloCellMakerCosmicMonitoring()

#        celltime = TrigTimeHistToolConfig("TrigCaloCellMaker_Time")

#        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic]

# use the TrigCaloCellMaker as base class, yet 
# use the fullcalo monitoring 

class TrigFullCaloCellMakerMTFCalBase (TrigCaloCellMakerMT): 
    __slots__ = [] 
    def __init__(self, name): 
        super( TrigFullCaloCellMakerMTFCalBase, self ).__init__(name) 

#        cellvalidation = TrigFullCaloCellMakerValidationMonitoring() 
#        cellonline = TrigFullCaloCellMakerOnlineMonitoring() 
#        cellcosmic = TrigFullCaloCellMakerCosmicMonitoring() 
 
#        celltime = TrigTimeHistToolConfig("TrigFullCaloCellMaker_Time") 
 
#        self.AthenaMonTools = [ cellvalidation, cellonline, celltime, cellcosmic] 
 



class TrigCaloTowerMakerMTBase (TrigCaloTowerMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloTowerMakerMTBase, self ).__init__(name)

#        towervalidation = TrigCaloTowerMakerValidationMonitoring()
#        toweronline = TrigCaloTowerMakerOnlineMonitoring()
  #      towercosmic = TrigCaloTowerMakerCosmicMonitoring()

#        towertime = TrigTimeHistToolConfig("TrigCaloTowerMaker_Time")
      #  towertime.TimerPerObjHistLimits = [0,1000]
      #  towertime.TimerHistLimits  = [0,1000]
 #       towertime.TimerHistLimits  = [0,40]

#        self.AthenaMonTools = [ towervalidation, toweronline, towertime, towercosmic]


class TrigCaloClusterMakerMTBase (TrigCaloClusterMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloClusterMakerMTBase, self ).__init__(name)

        # clvalidation = TrigCaloClusterMakerValidationMonitoring()
        # clonline = TrigCaloClusterMakerOnlineMonitoring()
        # clcosmic = TrigCaloClusterMakerCosmicMonitoring()

        # cltime = TrigTimeHistToolConfig("TrigCaloClusterMaker_Time")

#        self.AthenaMonTools = [ clvalidation, clonline, cltime, clcosmic]


class TrigCaloCellMakerMT_eGamma (TrigCaloCellMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_eGamma'):
        super(TrigCaloCellMakerMT_eGamma, self).__init__(name)

        self.eta_size = 16
        self.phi_size = 16
 
        self.CreateRoiForID = True
        self.EtaWidthForID  = 0.1
        self.PhiWidthForID  = 0.1
       
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0

        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool


        self.ContainerTools = []
        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]


class TrigCaloCellMakerMT_jet (TrigCaloCellMakerFCalMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_jet', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMakerMT_jet, self).__init__(name)

        self.eta_size = eta_size
        self.phi_size = phi_size

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalemcellcontmaker = RoIFCalEmCellContMaker()
        roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
        roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        roilaremcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilaremcellcontmaker.AbsEinSigma = AbsE
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilarhadcellcontmaker.AbsEinSigma = AbsE
        roifcalemcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

        #self.OutputLevel=INFO


class TrigCaloCellMakerMT_super (TrigCaloCellMakerMT_jet):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_super', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMakerMT_super, self).__init__(name)
        self.TrustRoiLimits = True


class TrigCaloCellMakerMT_jet_super (TrigCaloCellMakerFCalMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_jet_super', eta_size=64, phi_size=64, doNoise=1, AbsE = False):
        super(TrigCaloCellMakerMT_jet_super, self).__init__(name)

        self.TrustRoiLimits = True

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalemcellcontmaker = RoIFCalEmCellContMaker()
        roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
        roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        roilaremcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilaremcellcontmaker.AbsEinSigma = AbsE
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roilarhadcellcontmaker.AbsEinSigma = AbsE
        roifcalemcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = doNoise
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

        #self.OutputLevel=INFO                                                                                                                                       



class TrigCaloCellMakerMT_jet_fullcalo (TrigFullCaloCellMakerMTFCalBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_jet_fullcalo', eta_size=64, phi_size=64, doNoise=1, AbsE=False, doPers=False):
        super(TrigCaloCellMakerMT_jet_fullcalo, self).__init__(name)


        print "****JTB configuring TrigCaloCellMakerMT_jet_fullcalo"

        # these aren't actually used in this instance
        # self.eta_size = eta_size
        # self.phi_size = phi_size
        
        self.FullScanEnabled = True

        self.TrustRoiLimits = True

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        fullcalocellcontmaker = FullCaloCellContMaker(DoLArCellsNoiseSuppression = doNoise, AbsEinSigma = AbsE,CaloNoiseTool = theCaloNoiseTool)

        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName()]
                               #CaloCellContainerFinalizerTool().getFullName(),
                               #CaloCellContainerCheckerTool().getFullName()]

        if doPers:
           self.PersistencyPrescaleFactor=1
           self.PersistencyKeyName="TrigCaloCellMakerMT_jet_fullcalo"

        #self.OutputLevel=INFO




class TrigCaloCellMakerMT_tau (TrigCaloCellMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMaker_tau'):
        super(TrigCaloCellMakerMT_tau, self).__init__(name)

        self.eta_size = 32
        self.phi_size = 32

        self.CreateRoiForID = True
        self.EtaWidthForID  = 0.2
        self.PhiWidthForID  = 0.2

        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        roilaremcellcontmaker  = RoILArEMCellContMaker()
        roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
        roilarhadcellcontmaker = RoILArHadCellContMaker()
        roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
        roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0
        roitilehadcellcontmaker = RoITileCellContMaker()
        roitilehadcellcontmaker.CheckCellWithinRoI = True
        roitilehadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
	
        self.ContainerTools = [CaloCellContainerCorrectorTool("LArCellCorrectorTool").getFullName(),
                               CaloCellContainerFinalizerTool().getFullName(),
                               CaloCellContainerCheckerTool().getFullName()]

class TrigCaloCellMakerMT_tau_cells (TrigCaloCellMakerMT_tau):
    __slots__ = []
    def __init__ (self, name='TrigCaloCellMakerMT_tau_cells'):
        super(TrigCaloCellMakerMT_tau_cells, self).__init__(name)

        self.PersistencyPrescaleFactor=1
        self.PersistencyKeyName="TrigCaloCellMakerMT_tau"
        
class TrigCaloTowerMakerMT_eGamma (TrigCaloTowerMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMakerMT_eGamma'):
        super(TrigCaloTowerMakerMT_eGamma, self).__init__(name)

        #input to  LArTowerMBuilder:  Cells in LArEM 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False

        
        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",
                                              CellContainerName = "RoIEMCalo",
                                              IncludedCalos     = [ "LAREM" ],
                                              OutputLevel=ERROR
                                              )
        #lartowerbuilder.CellContainerName = "RoIEMCalo" #useless
        #lartowerbuilder.IncludedCalos     = [ "LAREM" ]
        #lartowerbuilder.OutputLevel=ERROR
        self +=lartowerbuilder
        self.TowerMakerTools=[ lartowerbuilder.getFullName() ]
        self.NumberOfPhiTowers=256
        self.NumberOfEtaTowers=200
        self.EtaMin=-2.5
        self.EtaMax=2.5
        self.DeltaEta=0.5
        self.DeltaPhi=0.5
        


class TrigCaloTowerMakerMT_jet (TrigCaloTowerMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMakerMT_jet'):
        super(TrigCaloTowerMakerMT_jet, self).__init__(name)

        #input to  TileTowerBuilder:  cells in TILE
        try:
            from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
        except:
            mlog.error("could not get handle to TileTowerBuilderTool Quit")
            print traceback.format_exc()
            return False
        
        # input to LArTowerBuilder:  cells in LArEM and LARHEC 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool or/and LArFCalTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False


        larcmbtwrbldr = LArTowerBuilderTool("LArCmbTwrBldr",
                                            CellContainerName = "RoIEMCalo",
                                            IncludedCalos     = [ "LAREM", "LARHEC" ],
                                            OutputLevel=ERROR
                                            )

        #larcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #larcmbtwrbldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
        #larcmbtwrbldr.OutputLevel=ERROR
        
        fcalcmbtwrbldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr",
                                                 CellContainerName = "RoIEMCalo",
                                                 MinimumEt         = 0.*MeV,
                                                 OutputLevel=ERROR
                                                 )

        #fcalcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #fcalcmbtwrbldr.MinimumEt         = 0.*MeV
        #fcalcmbtwrbldr.OutputLevel=ERROR

        tilecmbtwrbldr = TileTowerBuilderTool("TileCmbTwrBldr",
                                              CellContainerName = "RoIEMCalo",
                                              DumpTowers        = FALSE,
                                              DumpWeightMap     = FALSE
                                              )
        #tilecmbtwrbldr.CellContainerName = "RoIEMCalo"
        #tilecmbtwrbldr.DumpTowers        = FALSE
        #tilecmbtwrbldr.DumpWeightMap     = FALSE

        
        self +=larcmbtwrbldr
        self +=fcalcmbtwrbldr
        self +=tilecmbtwrbldr
        self.NumberOfPhiTowers=64
        self.NumberOfEtaTowers=100
        self.EtaMin=-5.0
        self.EtaMax=5.0
        self.DeltaEta=1.2
        self.DeltaPhi=1.2
        self.TowerMakerTools = [ tilecmbtwrbldr.getFullName(), larcmbtwrbldr.getFullName(), fcalcmbtwrbldr.getFullName() ]
        
        
class TrigCaloTowerMakerMT_tau (TrigCaloTowerMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMakerMT_tau'):
        super(TrigCaloTowerMakerMT_tau, self).__init__(name)

        #input to  LArTowerMBuilder:  Cells in LArEM 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        except:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            print traceback.format_exc()
            return False

        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",
                                              CellContainerName = "RoIEMCalo",
                                              IncludedCalos     = [ "LAREM" ],
                                              OutputLevel=ERROR     
                                              )

        #lartowerbuilder.CellContainerName = "RoIEMCalo"
        #lartowerbuilder.IncludedCalos     = [ "LAREM" ]
        #lartowerbuilder.OutputLevel=ERROR

        self +=lartowerbuilder
        self.NumberOfPhiTowers=256
        self.NumberOfEtaTowers=200
        self.EtaMin=-2.5
        self.EtaMax=2.5
        self.DeltaEta=0.8
        self.DeltaPhi=0.8
        self.TowerMakerTools=[lartowerbuilder.getFullName()]
        

class TrigCaloClusterMakerMT_slw (TrigCaloClusterMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMakerMT_slw', cells="cells", towers="calotowers"):
        super(TrigCaloClusterMakerMT_slw, self).__init__(name)

        try:
            from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
        except:
            mlog.error("TrigCaloRec: could not get handle to make_CaloClusterBuilderSW Quit")
            print traceback.format_exc()
            return False
        
        
        trigslw= make_CaloClusterBuilderSW ("trigslw",
                                            tower_container = towers,
                                            eta_size = 3,
                                            phi_size = 5,
                                            #eta_sizep = 3,
                                            #phi_sizep = 3,
                                            e_threshold = 2.5 * GeV,
                                            FillClusterCells = False,
                                            #nextra = 0,
                                            #eta_SeedGrid = 5,
                                            #phi_SeedGrid = 5,
                                            eta_Duplicate = 5,
                                            phi_Duplicate = 5
                                            )
        trigslw .CaloCellContainer=cells
        self +=trigslw      
        self.ClusterMakerTools=[ trigslw.getFullName() ]

        try:
            from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
            for tool in make_CaloSwCorrections ("ele37"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
                print " Adding tool ", tool.getFullName()
            for tool in make_CaloSwCorrections ("ele55"):
                self += tool
                self.ClusterCorrectionTools += [tool.getFullName()]
                print " Adding tool ", tool.getFullName()
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloSwCorrections_compat Quit")
            print traceback.format_exc()
            return False



class TrigCaloClusterMakerMT_topo (TrigCaloClusterMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMakerMT_topo', cells="cells",doMoments=True, doLC=True ):
        super(TrigCaloClusterMakerMT_topo, self).__init__(name)

        self.Cells=cells
        
        try:
            from CaloUtils.CaloUtilsConf import CaloLCWeightTool, CaloLCClassificationTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            from CaloClusterCorrection.CaloClusterBadChannelListCorr import CaloClusterBadChannelListCorr
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
            from CaloRec.CaloTopoClusterFlags import jobproperties
            from AthenaCommon.SystemOfUnits import deg
            from AthenaCommon.AlgSequence import AlgSequence
            from AthenaCommon.GlobalFlags import globalflags
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            print traceback.format_exc()
            return False

        print jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib()
        # tools used by tools
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault() #flag='tool', name='myCaloNoiseToolDefault')
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool

        if doLC:
        
          TrigLCClassify   = CaloLCClassificationTool("TrigLCClassify")
          TrigLCClassify.ClassificationKey   = "EMFracClassify"
          TrigLCClassify.UseSpread = False
          TrigLCClassify.MaxProbability = 0.5
          TrigLCClassify.StoreClassificationProbabilityInAOD = True

          TrigLCWeight = CaloLCWeightTool("TrigLCWeight")
          TrigLCWeight.CorrectionKey       = "H1ClusterCellWeights"
          TrigLCWeight.SignalOverNoiseCut  = 2.0
          TrigLCWeight.CaloNoiseTool       = theCaloNoiseTool 
          TrigLCWeight.UseHadProbability   = True

          TrigLCOut     = CaloLCOutOfClusterTool("TrigLCOut")
          TrigLCOut.CorrectionKey       = "OOCCorrection"
          TrigLCOut.UseEmProbability    = False
          TrigLCOut.UseHadProbability   = True

          TrigLCOutPi0  = CaloLCOutOfClusterTool("TrigLCOutPi0")
          TrigLCOutPi0.CorrectionKey    = "OOCPi0Correction"
          TrigLCOutPi0.UseEmProbability  = True
          TrigLCOutPi0.UseHadProbability = False

          TrigLCDeadMaterial   = CaloLCDeadMaterialTool("TrigLCDeadMaterial")
          TrigLCDeadMaterial.HadDMCoeffKey       = "HadDMCoeff2"
          #TrigLCDeadMaterial.SignalOverNoiseCut  = 1.0
          TrigLCDeadMaterial.ClusterRecoStatus   = 0
          TrigLCDeadMaterial.WeightModeDM        = 2
          #TrigLCDeadMaterial.CaloNoiseTool       = theCaloNoiseTool
          TrigLCDeadMaterial.UseHadProbability   = True

          # correction tools using tools
          TrigLocalCalib = CaloClusterLocalCalib ("TrigLocalCalib")
          TrigLocalCalib.ClusterClassificationTool     = [TrigLCClassify]
          #TrigLocalCalib.ClusterRecoStatus             = [2]
          TrigLocalCalib.ClusterRecoStatus             = [1,2]
          TrigLocalCalib.LocalCalibTools               = [TrigLCWeight]

          TrigLocalCalib += TrigLCClassify
          TrigLocalCalib += TrigLCWeight

          TrigOOCCalib   = CaloClusterLocalCalib ("TrigOOCCalib")
          #TrigOOCCalib.ClusterRecoStatus   = [2]
          TrigOOCCalib.ClusterRecoStatus   = [1,2]
          TrigOOCCalib.LocalCalibTools     = [TrigLCOut]

          TrigOOCCalib += TrigLCOut

          TrigOOCPi0Calib   = CaloClusterLocalCalib ("TrigOOCPi0Calib")
          #OOCPi0Calib.ClusterRecoStatus   = [1]
          TrigOOCPi0Calib.ClusterRecoStatus   = [1,2]
          TrigOOCPi0Calib.LocalCalibTools     = [TrigLCOutPi0]

          TrigOOCPi0Calib += TrigLCOutPi0


          TrigDMCalib    = CaloClusterLocalCalib ("TrigDMCalib")
          TrigDMCalib.ClusterRecoStatus   = [1,2]
          TrigDMCalib.LocalCalibTools     = [TrigLCDeadMaterial]

          TrigDMCalib += TrigLCDeadMaterial

       
        if doMoments:
 
          # correction tools not using tools
          TrigTopoMoments = CaloClusterMomentsMaker ("TrigTopoMoments")
          TrigTopoMoments.OutputLevel = INFO
          TrigTopoMoments.MaxAxisAngle = 20*deg
          TrigTopoMoments.CaloNoiseTool = theCaloNoiseTool
          TrigTopoMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
          TrigTopoMoments.UsePileUpNoise = True
          TrigTopoMoments.MinBadLArQuality = 4000
          TrigTopoMoments.MomentsNames = ["FIRST_PHI" 
                                          ,"FIRST_ETA"
                                          ,"SECOND_R" 
                                          ,"SECOND_LAMBDA"
                                          ,"DELTA_PHI"
                                          ,"DELTA_THETA"
                                          ,"DELTA_ALPHA" 
                                          ,"CENTER_X"
                                          ,"CENTER_Y"
                                          ,"CENTER_Z"
                                          ,"CENTER_MAG"
                                          ,"CENTER_LAMBDA"
                                          ,"LATERAL"
                                          ,"LONGITUDINAL"
                                          ,"FIRST_ENG_DENS" 
                                          ,"ENG_FRAC_EM" 
                                          ,"ENG_FRAC_MAX" 
                                          ,"ENG_FRAC_CORE" 
                                          ,"FIRST_ENG_DENS" 
                                          ,"SECOND_ENG_DENS" 
                                          ,"ISOLATION"
                                          ,"ENG_BAD_CELLS"
                                          ,"N_BAD_CELLS"
                                          ,"N_BAD_CELLS_CORR"
                                          ,"BAD_CELLS_CORR_E"
                                          ,"BADLARQ_FRAC"
                                          ,"ENG_POS"
                                          ,"SIGNIFICANCE"
                                          ,"CELL_SIGNIFICANCE"
                                          ,"CELL_SIG_SAMPLING"
                                          ,"AVG_LAR_Q"
                                          ,"AVG_TILE_Q"
                                          ]          
        #TrigLockVariables = CaloClusterLockVars("TrigLockVariables")
        #TrigLockVariables.FixBasicEnergy = True
        #TrigLockVariables.LockedSamplingVariables = []
        #TrigLockVariables.LockedSamplingVariables += [
        #    "Energy", "Max_Energy"]
        #TrigLockVariables.LockedSamplingVariables += [
        #    "Eta", "Phi", "Delta_Eta",
        #    "Delta_Phi", "Max_Eta", "Max_Phi"
        #    ]
        
        # maker tools
        TrigTopoMaker = CaloTopoClusterMaker("TrigTopoMaker")

        TrigTopoMaker.CellsName = cells
        TrigTopoMaker.CalorimeterNames=["LAREM",
                                        "LARHEC",
                                        "LARFCAL",
                                        "TILE"]
        # cells from the following samplings will be able to form
        # seeds. By default no sampling is excluded
        TrigTopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                           "PreSamplerE", "EME1", "EME2", "EME3",
                                           "HEC0", "HEC1","HEC2", "HEC3",
                                           "TileBar0", "TileBar1", "TileBar2",
                                           "TileExt0", "TileExt1", "TileExt2",
                                           "TileGap1", "TileGap2", "TileGap3",
                                           "FCAL0", "FCAL1", "FCAL2"]

        TrigTopoMaker.CaloNoiseTool=theCaloNoiseTool
        TrigTopoMaker.UseCaloNoiseTool=True
        TrigTopoMaker.UsePileUpNoise=True
        TrigTopoMaker.NeighborOption = "super3D"
        TrigTopoMaker.RestrictHECIWandFCalNeighbors  = False
        TrigTopoMaker.CellThresholdOnEorAbsEinSigma     =    0.0
        TrigTopoMaker.NeighborThresholdOnEorAbsEinSigma =    2.0  #instead of 2
        TrigTopoMaker.SeedThresholdOnEorAbsEinSigma     =    4.0  #instead of 4
        # note Et or AbsEt
        #TrigTopoMaker.NeighborCutsInAbsE              = False
        #TrigTopoMaker.CellCutsInAbsE                 = False
        #
        # the following cut must be set to TRUE in order to make double
        # sided cuts on the seed and the cluster level ( neighbor and cell
        # cuts are always double sided)
        #
        TrigTopoMaker.SeedCutsInAbsE                 = True
        TrigTopoMaker.ClusterEtorAbsEtCut            = 0.0*MeV
        # the following Et thresholds are ignored in case UsePileUpNoise
        # is TRUE
        #
        #
        #CellThresholdOnEtorAbsEt = 0.0*MeV
        #NeighborThresholdOnEtorAbsEt = 100.0*MeV
        #SeedThresholdOnEtorAbsEt = 200.0*MeV

        # use 2-gaussian or single gaussian noise for TileCal
        TrigTopoMaker.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()

        TrigTopoSplitter = CaloTopoClusterSplitter("TrigTopoSplitter")        
        # cells from the following samplings will be able to form local
        # maxima. The excluded samplings are PreSamplerB, EMB1,
        # PreSamplerE, EME1, all Tile samplings, all HEC samplings and the
        # two rear FCal samplings.
        #
        TrigTopoSplitter.SamplingNames = ["EMB2", "EMB3",
                                          "EME2", "EME3",
                                          "FCAL0"]
        # cells from the following samplings will also be able to form
        # local maxima but only if they are not overlapping in eta and phi
        # with local maxima in previous samplings from the primary list.
        #
        TrigTopoSplitter.SecondarySamplingNames = ["EMB1","EME1",
                                                   "TileBar0","TileBar1","TileBar2",
                                                   "TileExt0","TileExt1","TileExt2",
                                                   "HEC0","HEC1","HEC2","HEC3",
                                                   "FCAL1","FCAL2"]
        TrigTopoSplitter.ShareBorderCells = True
        TrigTopoSplitter.RestrictHECIWandFCalNeighbors  = False
        #
        # the following options are not set, since these are the default
        # values
        #
        # NeighborOption                = "super3D",
        # NumberOfCellsCut              = 4,
        # EnergyCut                     = 500*MeV,


        # cluster maker
        #TrigCaloTopoCluster = CaloClusterMaker ("TrigCaloTopoCluster")
        #TrigCaloTopoCluster.ClustersOutputName="CaloCalTopoCluster"
        #TrigCaloTopoCluster.ClusterMakerTools = [

        if not doMoments:
          self.ClusterMakerTools = [ TrigTopoMaker.getFullName(), TrigTopoSplitter.getFullName()]
        else:
          self.ClusterMakerTools = [ TrigTopoMaker.getFullName(), TrigTopoSplitter.getFullName(),  TrigTopoMoments.getFullName()]

        # do not use BadChannelListCorr since this is not used for jet and tau in offline
        #TrigBadChannelListCorr = CaloClusterBadChannelListCorr()
        #self.ClusterCorrectionTools += [TrigBadChannelListCorr.getFullName()]
        
        self += TrigTopoMaker
        self.TrigTopoMaker.OutputLevel=5
        self += TrigTopoSplitter
        self.TrigTopoSplitter.OutputLevel=5
        #self += TrigBadChannelListCorr
        if doMoments:
          self += TrigTopoMoments
          self.TrigTopoMoments.OutputLevel=5

        self.ClusterCorrectionTools = [  ]
        #self.ClusterCorrectionTools = [ TrigLockVariables.getFullName() ]
        #self += TrigLockVariables

        if doLC:
          self.ClusterCorrectionTools += [ TrigLocalCalib.getFullName(),
                                           TrigOOCCalib.getFullName(),
                                           TrigOOCPi0Calib.getFullName(),
                                           TrigDMCalib.getFullName()]
          self += TrigLocalCalib
          self += TrigOOCCalib
          self += TrigOOCPi0Calib
          self += TrigDMCalib

        #
        # pool/cool part
        #
        if doLC:  
          if globalflags.DetDescrVersion().startswith("Rome"):
             self.TrigLocalCalib.TrigLCClassify.MaxProbability = 0.85
             self.TrigLocalCalib.TrigLCClassify.UseNormalizedEnergyDensity = False
          else:
             self.TrigLocalCalib.TrigLCClassify.MaxProbability = 0.50
             self.TrigLocalCalib.TrigLCClassify.UseNormalizedEnergyDensity = True

        from CaloRec import CaloClusterTopoCoolFolder




class TrigCaloClusterMakerMT_EMtopo (TrigCaloClusterMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMakerMT_EMtopo'):
        super(TrigCaloClusterMakerMT_EMtopo, self).__init__(name)

        try:
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker, CaloClusterMaker, CaloCell2ClusterMapper
            from CaloRec.CaloTopoClusterFlags import jobproperties 
            emtopocluster = CaloTopoClusterMaker("EMTrigTopoCluster")
            emtoposplitter = CaloTopoClusterSplitter("EMTrigTopoSplitter")
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            print traceback.format_exc()
            return False
        
        emtopocluster.CellsNames=["RoIEMCalo"]
        emtopocluster.CalorimeterNames=[ "LAREM" ]
        #
        # cells from the following samplings will be able to form seeds. By default 
        # no sampling is excluded 
        #
        emtopocluster.OutputLevel=VERBOSE
        emtopocluster.SeedSamplingNames = [
            "PreSamplerB", "EMB1", "EMB2", 
            "PreSamplerE", "EME1", "EME2"  ]
        from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        theCaloNoiseTool=CaloNoiseToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc+=theCaloNoiseTool
        emtopocluster.CaloNoiseTool=theCaloNoiseTool

        emtopocluster.UseCaloNoiseTool=True
        emtopocluster.UsePileUpNoise=False
        
        emtopocluster.NeighborOption                 = "all3D"
        emtopocluster.CellThresholdOnEorAbsEinSigma     =    0.0
        emtopocluster.NeighborThresholdOnEorAbsEinSigma =    2.0
        emtopocluster.SeedThresholdOnEorAbsEinSigma     =    4.0
        # note E or AbsE 
        #
        # the following cut must be set to TRUE in order to make double sided
        # cuts on the seed and the cluster level ( neighbor and cell cuts are 
        # always double sided)
        #
        emtopocluster.SeedCutsInAbsE                 = TRUE
        emtopocluster.ClusterEtorAbsEtCut            = 1*GeV
        #
        # the following Et thresholds are ignored in case UsePileUpNoise is TRUE
        #
        # emtopocluster.CellThresholdOnAbsEt           =    0.0*MeV  
        # emtopocluster.NeighborThresholdOnAbsEt       =  100.0*MeV
        # emtopocluster.SeedThresholdOnEtorAbsEt       =  200.0*MeV # note Et or AbsEt
        
        
        #
        # cells from the following samplings will be able to form local maxima. The 
        # excluded samplings are PreSamplerB, EMB1, PreSamplerE, EME1,
        # and all samplings from Tile, HEC and FCAL
        #
        emtoposplitter.OutputLevel=VERBOSE
        emtoposplitter.SamplingNames = ["EMB2", "EME2"]
        emtoposplitter.SecondarySamplingNames = ["EMB1", "EME1"]
        emtoposplitter.ShareBorderCells = True
        emtoposplitter.RestrictHECIWandFCalNeighbors  = False        
        # the following options are not set, since these are the default values
        # the following options are not set, since these are the default values
        #
        # emtoposplitter.NumberOfCellsCut              = 4
        # emtoposplitter.EtDensityCut                  = 500*MeV/(600000*mm3) # this corresponds to 500 MeV in a typical EM Layer 2 cell
        
        
        try:
            from CaloRec.CaloRecConf import CaloClusterMomentsMaker
            emtopomoments = CaloClusterMomentsMaker("EMTrigTopoMoments")
        except:
            mlog.error("TrigCaloRec: could not get handle to CaloClusterMomentsMaker. Quit")
            print traceback.format_exc()
            return False
        
        emtopomoments.OutputLevel = INFO
        emtopomoments.MaxAxisAngle = 20*deg
        emtopomoments.CaloNoiseTool = theCaloNoiseTool
        emtopomoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
        emtopomoments.UsePileUpNoise = True
        emtopomoments.MinBadLArQuality = 4000
        emtopomoments.MomentsNames = ["FIRST_PHI" 
                                      ,"FIRST_ETA"
                                      ,"SECOND_R" 
                                      ,"SECOND_LAMBDA"
                                      ,"DELTA_PHI"
                                      ,"DELTA_THETA"
                                      ,"DELTA_ALPHA" 
                                      ,"CENTER_X"
                                      ,"CENTER_Y"
                                      ,"CENTER_Z"
                                      ,"CENTER_MAG"
                                      ,"CENTER_LAMBDA"
                                      ,"LATERAL"
                                      ,"LONGITUDINAL"
                                      ,"FIRST_ENG_DENS" 
                                      ,"ENG_FRAC_EM" 
                                      ,"ENG_FRAC_MAX" 
                                      ,"ENG_FRAC_CORE" 
                                      ,"FIRST_ENG_DENS" 
                                      ,"SECOND_ENG_DENS" 
                                      ,"ISOLATION"
                                      ,"ENG_BAD_CELLS"
                                      ,"N_BAD_CELLS"
                                      ,"N_BAD_CELLS_CORR"
                                      ,"BAD_CELLS_CORR_E"
                                      ,"BADLARQ_FRAC"
                                      ,"ENG_POS"
                                      ,"SIGNIFICANCE"
                                      ,"CELL_SIGNIFICANCE"
                                      ,"CELL_SIG_SAMPLING"
                                      ,"AVG_LAR_Q"
                                      ,"AVG_TILE_Q"
                                      ]          
        
        self.ClusterMakerTools = [emtopocluster.getFullName(), emtoposplitter.getFullName(), emtopomoments.getFullName()]
        #self.ClusterMakerTools = [emtopocluster.getFullName(), emtoposplitter.getFullName()]
        #self.ClusterCorrectionTools = [ emtopomoments.getFullName() ]
        self += emtopocluster
        self += emtoposplitter
        self += emtopomoments


class HLTCaloCellMaker (_HLTCaloCellMaker):
    __slots__ = []
    def __init__(self, name):
        super( HLTCaloCellMaker, self ).__init__(name)
        self.ExtraInputs=[('TileEMScale','ConditionStore+TileEMScale'),('TileBadChannels','ConditionStore+TileBadChannels')]
        self.ExtraInputs+=[( 'LArOnOffIdMapping' , 'ConditionStore+LArOnOffIdMap' )]


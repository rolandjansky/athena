# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from TrigCaloRec.TrigCaloRecConf import (TrigCaloClusterMakerMT,
                                         TrigCaloTowerMakerMT,
                                         TrigCaloClusterCalibratorMT,
                                         TrigCaloCellMaker,
                                         TrigFullCaloCellMaker,
                                         TrigCaloTowerMaker,
                                         TrigCaloClusterMaker,
                                         TrigCaloTopoTowerAlgorithm,
                                         TrigCaloCell2ClusterMapper,
                                         TrigFullCaloClusterMaker,
                                         RoILArEMCellContMaker,
                                         RoILArHadCellContMaker,
                                         RoITileCellContMaker,
                                         RoIFCalEmCellContMaker,
                                         RoIFCalHadCellContMaker,
                                         FullCaloCellContMaker,
                                         TrigLArNoisyROAlg,
                                         TrigL1BSTowerHypo)

from CaloRec.CaloRecConf import (CaloCellContainerCorrectorTool,
                                 CaloCellContainerFinalizerTool,
                                 CaloCellContainerCheckerTool)

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigCaloRec.TrigCaloCellMakerMonitoring import TrigCaloCellMakerValidationMonitoring, TrigCaloCellMakerFCalValidationMonitoring, TrigCaloCellMakerOnlineMonitoring, TrigCaloCellMakerCosmicMonitoring
from TrigCaloRec.TrigCaloCellMakerMonitoring import TrigFullCaloCellMakerValidationMonitoring, TrigFullCaloCellMakerOnlineMonitoring, TrigFullCaloCellMakerCosmicMonitoring
from TrigCaloRec.TrigCaloTowerMakerMonitoring import TrigCaloTowerMakerValidationMonitoring, TrigCaloTowerMakerOnlineMonitoring, TrigCaloTowerMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigCaloClusterMakerValidationMonitoring, TrigCaloClusterMakerOnlineMonitoring, TrigCaloClusterMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigFullCaloClusterMakerValidationMonitoring, TrigFullCaloClusterMakerOnlineMonitoring, TrigFullCaloClusterMakerCosmicMonitoring
from TrigCaloRec.TrigCaloClusterMakerMonitoring import TrigL1BSTowerHypoOnlineMonitoring

# MT stuff
from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker as _HLTCaloCellMaker

from AthenaCommon.Constants import ERROR
from AthenaCommon.SystemOfUnits import GeV, MeV, deg
import traceback

from AthenaCommon.Logging import logging
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

        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        CaloNoiseCondAlg()

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

        if doPers:
           self.PersistencyPrescaleFactor=1
           self.PersistencyKeyName="TrigCaloCellMaker_jet_fullcalo"



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
        except Exception:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            traceback.print_exc()
            return False

        
        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",  # noqa: ATL900 (OutputLevel)
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
        except Exception:
            mlog.error("could not get handle to TileTowerBuilderTool Quit")
            traceback.print_exc()
            return False
        
        # input to LArTowerBuilder:  cells in LArEM and LARHEC 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool
        except Exception:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool or/and LArFCalTowerBuilderTool. Quit")
            traceback.print_exc()
            return False


        larcmbtwrbldr = LArTowerBuilderTool("LArCmbTwrBldr",  # noqa: ATL900 (OutputLevel)
                                            CellContainerName = "RoIEMCalo",
                                            IncludedCalos     = [ "LAREM", "LARHEC" ],
                                            OutputLevel=ERROR
                                            )

        #larcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #larcmbtwrbldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
        #larcmbtwrbldr.OutputLevel=ERROR
        
        fcalcmbtwrbldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr",  # noqa: ATL900 (OutputLevel)
                                                 CellContainerName = "RoIEMCalo",
                                                 MinimumEt         = 0.*MeV,
                                                 OutputLevel=ERROR
                                                 )

        #fcalcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #fcalcmbtwrbldr.MinimumEt         = 0.*MeV
        #fcalcmbtwrbldr.OutputLevel=ERROR

        tilecmbtwrbldr = TileTowerBuilderTool("TileCmbTwrBldr",
                                              CellContainerName = "RoIEMCalo",
                                              DumpTowers        = False,
                                              DumpWeightMap     = False
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
        except Exception:
            mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool. Quit")
            traceback.print_exc()
            return False

        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",  # noqa: ATL900 (OutputLevel)
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
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to make_CaloClusterBuilderSW Quit")
            traceback.print_exc()
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
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to CaloSwCorrections_compat Quit")
            traceback.print_exc()
            return False

class TrigCaloClusterMaker_slw_fullcalo (TrigFullCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_slw_fullcalo'):
        super(TrigCaloClusterMaker_slw_fullcalo, self).__init__(name)

        try:
            from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to make_CaloClusterBuilderSW Quit")
            traceback.print_exc()
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
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to CaloSwCorrections_compat Quit")
            traceback.print_exc()
            return False

class TrigCaloClusterMaker_topo (TrigCaloClusterMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMaker_topo', doMoments=True, doLC=True ):
        super(TrigCaloClusterMaker_topo, self).__init__(name)
        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        CaloNoiseCondAlg()

        try:
            from CaloUtils.CaloUtilsConf import CaloLCWeightTool, CaloLCClassificationTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
            from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker
            from CaloRec.CaloTopoClusterFlags import jobproperties
            from AthenaCommon.SystemOfUnits import deg
            from AthenaCommon.GlobalFlags import globalflags
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            traceback.print_exc()
            return False

        # tools used by tools
        #from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
        #theCaloNoiseTool=CaloNoiseToolDefault() #flag='tool', name='myCaloNoiseToolDefault')
        #from AthenaCommon.AppMgr import ToolSvc
        #ToolSvc+=theCaloNoiseTool

        if doLC:
          from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
          #For LCWeightsTool needs electronic noise
          CaloNoiseCondAlg(noisetype="electronicNoise")   
          TrigLCClassify   = CaloLCClassificationTool("TrigLCClassify")
          TrigLCClassify.ClassificationKey   = "EMFracClassify"
          TrigLCClassify.UseSpread = False
          TrigLCClassify.MaxProbability = 0.5
          TrigLCClassify.StoreClassificationProbabilityInAOD = True

          TrigLCWeight = CaloLCWeightTool("TrigLCWeight")
          TrigLCWeight.CorrectionKey       = "H1ClusterCellWeights"
          TrigLCWeight.SignalOverNoiseCut  = 2.0
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
          TrigTopoMoments.MaxAxisAngle = 20*deg
          TrigTopoMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
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

        from CaloRec import CaloClusterTopoCoolFolder  # noqa: F401

class TrigCaloTopoTowerMaker_jet (TrigCaloTopoTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTopoTowerMaker_jet'):
        super(TrigCaloTopoTowerMaker_jet, self).__init__(name)

        # configure CaloTopoTowerAlgorithm here
        try:
            from CaloUtils.CaloUtilsConf import CaloTopoTowerBuilderTool
            theCaloTopoTowerBuilderTool=CaloTopoTowerBuilderTool("TrigTopoTowerTwrBldr")
        except Exception:
            mlog.error("could not get handle to CaloTopoTowerBuilderTool Quit")
            traceback.print_exc()
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
            from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker
            from CaloRec.CaloTopoClusterFlags import jobproperties 
            emtopocluster = CaloTopoClusterMaker("EMTrigTopoCluster")
            emtoposplitter = CaloTopoClusterSplitter("EMTrigTopoSplitter")
            emtopomoments = CaloClusterMomentsMaker("EMTrigTopoMoments")
        except Exception:
            mlog.error("TrigCaloRec: could not get handle to CaloTopoClusterMaker and/or CaloTopoClusterSplitter. Quit")
            traceback.print_exc()
            return False
        
        emtopocluster.CellsNames=["RoIEMCalo"]
        emtopocluster.CalorimeterNames=[ "LAREM" ]
        #
        # cells from the following samplings will be able to form seeds. By default 
        # no sampling is excluded 
        #
        emtopocluster.SeedSamplingNames = [
            "PreSamplerB", "EMB1", "EMB2", 
            "PreSamplerE", "EME1", "EME2"  ]
        
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
        emtopocluster.SeedCutsInAbsE                 = True
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
        emtoposplitter.SamplingNames = ["EMB2", "EME2"]
        emtoposplitter.SecondarySamplingNames = ["EMB1", "EME1"]
        emtoposplitter.ShareBorderCells = True
        emtoposplitter.RestrictHECIWandFCalNeighbors  = False        
        # the following options are not set, since these are the default values
        # the following options are not set, since these are the default values
        #
        # emtoposplitter.NumberOfCellsCut              = 4
        # emtoposplitter.EtDensityCut                  = 500*MeV/(600000*mm3) # this corresponds to 500 MeV in a typical EM Layer 2 cell

        emtopomoments.MaxAxisAngle = 20*deg
        emtopomoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
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


# MT classes

class TrigCaloTowerMakerMTBase (TrigCaloTowerMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloTowerMakerMTBase, self ).__init__(name)


class TrigCaloClusterMakerMTBase (TrigCaloClusterMakerMT):
    __slots__ = []
    def __init__(self, name):
        super( TrigCaloClusterMakerMTBase, self ).__init__(name)

        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool('MonTool')

        # Set range variables
        maxNumberOfClusters=50.0
        maxProcTime=4500.0
        if ( "FS" in name ):
            maxNumberOfClusters=1200.0
            maxProcTime=150000

        # Define histograms
        monTool.defineHistogram('container_size', path='EXPERT', type='TH1F',  title="Container Size; Number of Clusters; Number of Events", xbins=50, xmin=0.0, xmax=maxNumberOfClusters)
        monTool.defineHistogram('TIME_execute', path='EXPERT', type='TH1F', title="Total Execution Time; Execution time [ us ] ; Number of runs", xbins=100, xmin=0.0, xmax=maxProcTime)
        monTool.defineHistogram('TIME_ClustMaker', path='EXPERT', type='TH1F', title="Cluster Maker Time; Execution time [ us ] ; Number of runs", xbins=100, xmin=0.0, xmax=maxProcTime)
        monTool.defineHistogram('TIME_ClustCorr', path='EXPERT', type='TH1F', title="Cluster Correction Time; Execution time [ us ] ; Number of runs", xbins=100, xmin=0.0, xmax=100)
        monTool.defineHistogram('Et', path='EXPERT', type='TH1F',  title="Cluster E_T; E_T [ MeV ] ; Number of Clusters", xbins=135, xmin=-200.0, xmax=2500.0)
        monTool.defineHistogram('Eta', path='EXPERT', type='TH1F', title="Cluster #eta; #eta ; Number of Clusters", xbins=100, xmin=-2.5, xmax=2.5)
        monTool.defineHistogram('Phi', path='EXPERT', type='TH1F', title="Cluster #phi; #phi ; Number of Clusters", xbins=64, xmin=-3.2, xmax=3.2)
        monTool.defineHistogram('Eta,Phi', path='EXPERT', type='TH2F', title="Number of Clusters; #eta ; #phi ; Number of Clusters", xbins=100, xmin=-2.5, xmax=2.5, ybins=128, ymin=-3.2, ymax=3.2)
        monTool.defineHistogram('clusterSize', path='EXPERT', type='TH1F', title="Cluster Type; Type ; Number of Clusters", xbins=13, xmin=0.5, xmax=13.5)
        monTool.defineHistogram('signalState', path='EXPERT', type='TH1F', title="Signal State; Signal State ; Number of Clusters", xbins=4, xmin=-1.5, xmax=2.5)
        monTool.defineHistogram('size', path='EXPERT', type='TH1F', title="Cluster Size; Size [Cells] ; Number of Clusters", xbins=125, xmin=0.0, xmax=250.0)
        monTool.defineHistogram('N_BAD_CELLS', path='EXPERT', type='TH1F', title="N_BAD_CELLS; N_BAD_CELLS ; Number of Clusters", xbins=250, xmin=0.5, xmax=250.5)
        monTool.defineHistogram('ENG_FRAC_MAX', path='EXPERT', type='TH1F', title="ENG_FRAC_MAX; ENG_FRAC_MAX ; Number of Clusters", xbins=50, xmin=0.0, xmax=1.1)

        self.MonTool = monTool


class TrigCaloTowerMakerMT_eGamma (TrigCaloTowerMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMakerMT_eGamma'):
        super(TrigCaloTowerMakerMT_eGamma, self).__init__(name)

        from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",  # noqa: ATL900 (OutputLevel)
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

        # input to LArTowerBuilder:  cells in LArEM and LARHEC 
        from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool

        larcmbtwrbldr = LArTowerBuilderTool("LArCmbTwrBldr", # noqa: ATL900 (OutputLevel)
                                            CellContainerName = "RoIEMCalo",
                                            IncludedCalos     = [ "LAREM", "LARHEC" ],
                                            OutputLevel=ERROR
                                            )

        #larcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #larcmbtwrbldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
        #larcmbtwrbldr.OutputLevel=ERROR
        
        fcalcmbtwrbldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr",  # noqa: ATL900 (OutputLevel)
                                                 CellContainerName = "RoIEMCalo",
                                                 MinimumEt         = 0.*MeV,
                                                 OutputLevel=ERROR
                                                 )

        #fcalcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #fcalcmbtwrbldr.MinimumEt         = 0.*MeV
        #fcalcmbtwrbldr.OutputLevel=ERROR

        #input to  TileTowerBuilder:  cells in TILE
        from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
        tilecmbtwrbldr = TileTowerBuilderTool("TileCmbTwrBldr",
                                              CellContainerName = "RoIEMCalo",
                                              DumpTowers        = False,
                                              DumpWeightMap     = False
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
        from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
        lartowerbuilder = LArTowerBuilderTool("LArTowerBuilder",  # noqa: ATL900 (OutputLevel)
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

        from CaloRec.CaloRecMakers import make_CaloClusterBuilderSW
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


        from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections
        for tool in make_CaloSwCorrections ("ele37"):
            self += tool
            self.ClusterCorrectionTools += [tool.getFullName()]
            mlog.info("Adding tool %s", tool.getFullName())
        for tool in make_CaloSwCorrections ("ele55"):
            self += tool
            self.ClusterCorrectionTools += [tool.getFullName()]
            mlog.info("Adding tool %s", tool.getFullName())


class TrigCaloClusterMakerMT_topo (TrigCaloClusterMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMakerMT_topo', cells="cells",doMoments=True, doLC=True ):
        super(TrigCaloClusterMakerMT_topo, self).__init__(name)

        self.Cells=cells

        from CaloUtils.CaloUtilsConf import CaloLCWeightTool, CaloLCClassificationTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
        from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
        from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker
        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        from CaloRec.CaloTopoClusterFlags import jobproperties
        from AthenaCommon.SystemOfUnits import deg
        from AthenaCommon.GlobalFlags import globalflags

        # tools used by tools

        if doLC:
          #For LCWeightsTool needs electronic noise
          CaloNoiseCondAlg(noisetype="electronicNoise") 
          TrigLCClassify   = CaloLCClassificationTool("TrigLCClassify")
          TrigLCClassify.ClassificationKey   = "EMFracClassify"
          TrigLCClassify.UseSpread = False
          TrigLCClassify.MaxProbability = 0.5
          TrigLCClassify.StoreClassificationProbabilityInAOD = True

          TrigLCWeight = CaloLCWeightTool("TrigLCWeight")
          TrigLCWeight.CorrectionKey       = "H1ClusterCellWeights"
          TrigLCWeight.SignalOverNoiseCut  = 2.0
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
          TrigTopoMoments.MaxAxisAngle = 20*deg
          TrigTopoMoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
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
        CaloNoiseCondAlg()
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
        self.TrigTopoMaker.OutputLevel=ERROR  # noqa: ATL900
        self += TrigTopoSplitter
        self.TrigTopoSplitter.OutputLevel=ERROR  # noqa: ATL900
        #self += TrigBadChannelListCorr
        if doMoments:
          self += TrigTopoMoments
          self.TrigTopoMoments.OutputLevel=ERROR  # noqa: ATL900

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

        from CaloRec import CaloClusterTopoCoolFolder  # noqa: F401




class TrigCaloClusterMakerMT_EMtopo (TrigCaloClusterMakerMTBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloClusterMakerMT_EMtopo'):
        super(TrigCaloClusterMakerMT_EMtopo, self).__init__(name)

        from CaloRec.CaloRecConf import CaloTopoClusterMaker, CaloTopoClusterSplitter, CaloClusterMomentsMaker
        from CaloRec.CaloTopoClusterFlags import jobproperties
        emtopocluster = CaloTopoClusterMaker("EMTrigTopoCluster")
        emtoposplitter = CaloTopoClusterSplitter("EMTrigTopoSplitter")
        emtopomoments = CaloClusterMomentsMaker("EMTrigTopoMoments")
        
        emtopocluster.CellsNames=["RoIEMCalo"]
        emtopocluster.CalorimeterNames=[ "LAREM" ]
        #
        # cells from the following samplings will be able to form seeds. By default 
        # no sampling is excluded 
        #
        emtopocluster.SeedSamplingNames = [
            "PreSamplerB", "EMB1", "EMB2", 
            "PreSamplerE", "EME1", "EME2"  ]
        
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
        emtopocluster.SeedCutsInAbsE                 = True
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
        emtoposplitter.SamplingNames = ["EMB2", "EME2"]
        emtoposplitter.SecondarySamplingNames = ["EMB1", "EME1"]
        emtoposplitter.ShareBorderCells = True
        emtoposplitter.RestrictHECIWandFCalNeighbors  = False        
        # the following options are not set, since these are the default values
        # the following options are not set, since these are the default values
        #
        # emtoposplitter.NumberOfCellsCut              = 4
        # emtoposplitter.EtDensityCut                  = 500*MeV/(600000*mm3) # this corresponds to 500 MeV in a typical EM Layer 2 cell
        
        emtopomoments.MaxAxisAngle = 20*deg
        emtopomoments.TwoGaussianNoise = jobproperties.CaloTopoClusterFlags.doTwoGaussianNoise()
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
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        monTool = GenericMonitoringTool('MonTool')
        maxNumberOfCells=1600.0
        maxProcTime=800.0
        monitorCells=True
        if ( "FS" in name ):
          maxNumberOfCells=240000
          maxProcTime=160000
          monitorCells=False
        monTool.defineHistogram('Cells_N', path='EXPERT', type='TH1F',  title="Cells N; NCells; events", xbins=40, xmin=0.0, xmax=maxNumberOfCells)
        monTool.defineHistogram('TIME_exec', path='EXPERT', type='TH1F', title="Cells time; time [ us ] ; Nruns", xbins=80, xmin=0.0, xmax=maxProcTime)
        if ( monitorCells ):
          monTool.defineHistogram('Cells_eT', path='EXPERT', type='TH1F',  title="Cells E_T; E_T [ GeV ] ; Nclusters", xbins=100, xmin=0.0, xmax=100.0)
          monTool.defineHistogram('Cells_eta', path='EXPERT', type='TH1F', title="Cells #eta; #eta ; Nclusters", xbins=100, xmin=-2.5, xmax=2.5)
          monTool.defineHistogram('Cells_phi', path='EXPERT', type='TH1F', title="Cells #phi; #phi ; Nclusters", xbins=128, xmin=-3.2, xmax=3.2)
        self.MonTool = monTool
        self.monitorCells = monitorCells

class TrigCaloClusterCalibratorMT_LC(TrigCaloClusterCalibratorMT):
    """ Class to set up the default configurations for LC calibrations """

    def __init__(self, name="TrigCaloClusterCalibratorMT_LC", **kwargs):
        super(TrigCaloClusterCalibratorMT_LC, self).__init__(name, **kwargs)

        from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
        from CaloUtils.CaloUtilsConf import CaloLCClassificationTool, CaloLCWeightTool, CaloLCOutOfClusterTool, CaloLCDeadMaterialTool
        from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterLocalCalib
        from AthenaCommon.GlobalFlags import globalflags

        # Need electronic noise for LCWeights
        CaloNoiseCondAlg(noisetype="electronicNoise")

        # Figure out the detector version
        det_version_is_rome = globalflags.DetDescrVersion().startswith("Rome")

        self.ClusterCorrectionTools = []

        # Set up the tools
        self += CaloClusterLocalCalib(
            "TrigLocalCalib",
            ClusterRecoStatus = [1, 2])
        self.TrigLocalCalib += CaloLCClassificationTool(
            "TrigLCClassify",
            ClassificationKey = "EMFracClassify",
            UseSpread = False,
            MaxProbability = 0.85 if det_version_is_rome else 0.5,
            UseNormalizedEnergyDensity = not det_version_is_rome,
            StoreClassificationProbabilityInAOD = True)
        self.TrigLocalCalib.ClusterClassificationTool = [self.TrigLocalCalib.TrigLCClassify]
        self.TrigLocalCalib += CaloLCWeightTool(
            "TrigLCWeight",
            CorrectionKey = "H1ClusterCellWeights",
            SignalOverNoiseCut = 2.0,
            UseHadProbability = True)
        self.TrigLocalCalib.LocalCalibTools = [self.TrigLocalCalib.TrigLCWeight]
        self.ClusterCorrectionTools.append(self.TrigLocalCalib)

        self += CaloClusterLocalCalib(
            "TrigOOCCalib",
            ClusterRecoStatus = [1, 2])
        self.TrigOOCCalib += CaloLCOutOfClusterTool(
            "TrigLCOut",
            CorrectionKey = "OOCCorrection",
            UseEmProbability = False,
            UseHadProbability = True)
        self.TrigOOCCalib.LocalCalibTools = [self.TrigOOCCalib.TrigLCOut]
        self.ClusterCorrectionTools.append(self.TrigOOCCalib)

        self += CaloClusterLocalCalib(
            "TrigOOCPi0Calib",
            ClusterRecoStatus = [1, 2])
        self.TrigOOCPi0Calib += CaloLCOutOfClusterTool(
            "TrigLCOutPi0",
            CorrectionKey = "OOCPi0Correction",
            UseEmProbability = True,
            UseHadProbability = False)
        self.TrigOOCPi0Calib.LocalCalibTools = [self.TrigOOCPi0Calib.TrigLCOutPi0]
        self.ClusterCorrectionTools.append(self.TrigOOCPi0Calib)

        self += CaloClusterLocalCalib(
            "TrigDMCalib",
            ClusterRecoStatus = [1, 2])
        self.TrigDMCalib += CaloLCDeadMaterialTool(
            "TrigLCDeadMaterial",
            HadDMCoeffKey = "HadDMCoeff2",
            ClusterRecoStatus = 0,
            WeightModeDM = 2,
            UseHadProbability = True)
        self.TrigDMCalib.LocalCalibTools = [self.TrigDMCalib.TrigLCDeadMaterial]
        self.ClusterCorrectionTools.append(self.TrigDMCalib)

        # Also set up the monitoring
        from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool
        self.MonTool = GenericMonitoringTool("MonTool")
        self.MonTool.defineHistogram('Et', path='EXPERT', type='TH1F',  title="Cluster E_T; E_T [ MeV ] ; Number of Clusters", xbins=135, xmin=-200.0, xmax=2500.0)
        self.MonTool.defineHistogram('Eta', path='EXPERT', type='TH1F', title="Cluster #eta; #eta ; Number of Clusters", xbins=100, xmin=-2.5, xmax=2.5)
        self.MonTool.defineHistogram('Phi', path='EXPERT', type='TH1F', title="Cluster #phi; #phi ; Number of Clusters", xbins=64, xmin=-3.2, xmax=3.2)
        self.MonTool.defineHistogram('Eta,Phi', path='EXPERT', type='TH2F', title="Number of Clusters; #eta ; #phi ; Number of Clusters", xbins=100, xmin=-2.5, xmax=2.5, ybins=128, ymin=-3.2, ymax=3.2)

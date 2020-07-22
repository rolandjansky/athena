# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import MeV
from AthenaCommon.Constants import ERROR

from TrigHIRec.TrigHIRecConf import TrigHIClusterMaker
from TrigHIRec.TrigHIRecConf import TrigHIEventShapeMaker

from TrigCaloRec.TrigCaloRecConfig  import TrigCaloTowerMakerBase

class TrigCaloTowerMaker_hijet (TrigCaloTowerMakerBase):
    __slots__ = []
    def __init__ (self, name='TrigCaloTowerMaker_hijet'):
        super(TrigCaloTowerMaker_hijet, self).__init__(name)

        #input to  TileTowerBuilder:  cells in TILE
        try:
            from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
        except Exception:
            #mlog.error("could not get handle to TileTowerBuilderTool Quit")
            #print traceback.format_exc()
            return False

        # input to LArTowerBuilder:  cells in LArEM and LARHEC 
        try:
            from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool,LArFCalTowerBuilderTool
        except Exception:
            #mlog.error("TrigCaloTowerMaker: could not get handle to LArTowerBuilderTool or/and LArFCalTowerBuilderTool. Quit")
            #print traceback.format_exc()
            return False


        larcmbtwrbldr = LArTowerBuilderTool("LArCmbTwrBldr",   # noqa: ATL900  (assigning OutputLevel)
                                            CellContainerName = "AllCalo",
                                            IncludedCalos     = [ "LAREM", "LARHEC" ],
                                            OutputLevel=ERROR
                                            )

        #larcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #larcmbtwrbldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
        #larcmbtwrbldr.OutputLevel=ERROR

        fcalcmbtwrbldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr",   # noqa: ATL900  (assigning OutputLevel)
                                                 CellContainerName = "AllCalo",
                                                 MinimumEt         = 0.*MeV,
                                                 OutputLevel=ERROR
                                                 )
        #fcalcmbtwrbldr.CellContainerName = "RoIEMCalo"
        #fcalcmbtwrbldr.MinimumEt         = 0.*MeV
        #fcalcmbtwrbldr.OutputLevel=ERROR

        tilecmbtwrbldr = TileTowerBuilderTool("TileCmbTwrBldr",
                                              CellContainerName = "AllCalo",
                                              )


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



class TrigHIClusterMaker_hijet(TrigHIClusterMaker):
    __slots__ = []
    def __init__ (self, name='TrigHIClusterMaker_hijet'):
        super(TrigHIClusterMaker_hijet, self).__init__(name)

        self.OutputContainerKey = name # hicluster_name        



class TrigHIEventShapeMaker_hijet(TrigHIEventShapeMaker):
    __slots__ = []
    def __init__ (self, name='TrigHIEventShapeMaker_hijet'):
        super(TrigHIEventShapeMaker_hijet, self).__init__(name)

#    def __init__(self, name,
#                 **kwds):
#        TrigHLTJetRecConf.TrigHIEventShapeMaker.__init__(self, name, **kwds)

        self.UseCaloCell = False
        self.InputCellKey = "HLT_CaloCellContainer_TrigCaloCellMaker_jet_fullcalo"	#"AllCalo"
        self.InputTowerKey = "HLT_CaloTowerContainer_TrigCaloTowerMaker"	#"CombinedTowers"
        self.HIEventShapeContainerKey = "TrigHIEventShape" # name # hicluster_name



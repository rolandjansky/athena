# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrigCaloValidation.TrigCaloValidationConf import TrigCaloValidation
from TrigCaloRec.TrigCaloRecConf import RoILArEMCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoILArHadCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoITileCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoIFCalEmCellContMaker
from TrigCaloRec.TrigCaloRecConf import RoIFCalHadCellContMaker
from TrigCaloRec.TrigCaloRecConf import FullCaloCellContMaker

class TrigCaloValidationConfig (TrigCaloValidation):
   __slots__ = []
   def __init__ (self,name="TrigCaloValidationConfig"):
      super(TrigCaloValidationConfig,self).__init__(name)

      from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
      theCaloNoiseTool=CaloNoiseToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc+=theCaloNoiseTool

      roilaremcellcontmaker  = RoILArEMCellContMaker()
      roilaremcellcontmaker.CaloNoiseTool = theCaloNoiseTool
      roilaremcellcontmaker.OutputLevel=0
      roilarhadcellcontmaker = RoILArHadCellContMaker()
      roilarhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
      roilarhadcellcontmaker.OutputLevel=0
      roilaremcellcontmaker.DoLArCellsNoiseSuppression = 0
      roilarhadcellcontmaker.DoLArCellsNoiseSuppression = 0
      roifcalemcellcontmaker = RoIFCalEmCellContMaker()
      roifcalemcellcontmaker.CaloNoiseTool = theCaloNoiseTool
      roifcalemcellcontmaker.DoLArCellsNoiseSuppression = 0
      roifcalhadcellcontmaker = RoIFCalHadCellContMaker()
      roifcalhadcellcontmaker.CaloNoiseTool = theCaloNoiseTool
      roifcalhadcellcontmaker.DoLArCellsNoiseSuppression = 0


      self.TCRTools = [roilaremcellcontmaker,
                         roilarhadcellcontmaker,
                         roifcalemcellcontmaker,
                         roifcalhadcellcontmaker,
                         RoITileCellContMaker()]

class TrigFullCaloValidationConfig (TrigCaloValidation):
   __slots__ = []
   def __init__ (self,name="TrigFullCaloValidationConfig"):
      super(TrigFullCaloValidationConfig,self).__init__(name)

      from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
      theCaloNoiseTool=CaloNoiseToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc+=theCaloNoiseTool

      fullcalocellcontmaker = FullCaloCellContMaker()
      fullcalocellcontmaker.CaloNoiseTool = theCaloNoiseTool
      fullcalocellcontmaker.OutputLevel = 0
      fullcalocellcontmaker.DoLArCellsNoiseSuppression = 0

      self.TCRTools = [fullcalocellcontmaker]


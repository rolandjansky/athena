# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def CaloCellPedestalCorrCfg(configFlags):


   result=DetDescrCnvSvcCfg(configFlags)
   isMC=configFlags.Input.isMC
   theCaloCellPedestalCorr=CompFactory.CaloCellPedestalCorr()

   if not isMC:
      theCaloCellPedestalCorr.isMC=False
      if configFlags.Common.isOnline:
         folder  = '/CALO/Pedestal/CellPedestal'
         result.merge(addFolders(configFlags,folder,'CALO_ONL',className="CondAttrListCollection"))
      else:
         #regular offline case
         folder= '/CALO/Ofl/Pedestal/CellPedestal'
         result.merge(addFolders(configFlags,folder,'CALO_OFL',className="CondAttrListCollection"))
   
      thePedCorrCondAlg=CompFactory.CaloCellPedCorrCondAlg(PedestalShiftFolder=folder)
   else:
      theCaloCellPedestalCorr.isMC=True


   if not configFlags.Common.isOnline:
      if configFlags.Calo.Cell.doPileupOffsetBCIDCorr:
         from CaloRec.CaloBCIDAvgAlgConfig import CaloBCIDAvgAlgCfg
         result.merge(CaloBCIDAvgAlgCfg(configFlags))
         theCaloCellPedestalCorr.CaloBCIDAverageKey="CaloBCIDAverage"
      else:
         theCaloCellPedestalCorr.CaloBCIDAverageKey=""
         if not isMC:
            lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
            result.merge(addFolders(configFlags,lumiFolder,'TRIGGER_ONL',className="CondAttrListCollection"))
            thePedCorrCondAlg.Luminosity = -1
            thePedCorrCondAlg.LumiFolderName = lumiFolder
   
   if not isMC:
      result.addCondAlgo(thePedCorrCondAlg)

   result.setPrivateTools(theCaloCellPedestalCorr)
   return result


def CaloCellNeighborsAverageCorrCfg(flags):
    """Return component accumulator with configured private Calo cell neighbors average correction tool

    Arguments:
        flags  -- Athena configuration flags (ConfigFlags)
    """
    acc = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(flags))

    from TileGeoModel.TileGMConfig import TileGMCfg
    acc.merge(TileGMCfg(flags))

    CaloCellNeighborsAverageCorr=CompFactory.CaloCellNeighborsAverageCorr
    caloCellNeighborsAverageCorrection = CaloCellNeighborsAverageCorr(testMode = False)

    acc.setPrivateTools( caloCellNeighborsAverageCorrection )
    return acc


def CaloCellTimeCorrCfg(configFlags):
   CaloCellTimeCorrTool=CompFactory.CaloCellTimeCorrTool
   result=ComponentAccumulator()
   folder= "/LAR/TimeCorrectionOfl/CellTimeOffset"
   result.merge(addFolders(configFlags,[folder,], "LAR_OFL", className="AthenaAttributeList"))
   result.setPrivateTools(CaloCellTimeCorrTool(Folder=folder))
   return result


def CaloEnergyRescalerCfg(configFlags):
   CaloCellEnergyRescaler=CompFactory.CaloCellEnergyRescaler
   result=ComponentAccumulator()
   folder="/LAR/CellCorrOfl/EnergyCorr"
   result.merge(addFolders(configFlags,[folder,], "LAR_OFL", className="AthenaAttributeList"))
   result.setPrivateTools(CaloCellEnergyRescaler(Folder=folder))
   return result
   

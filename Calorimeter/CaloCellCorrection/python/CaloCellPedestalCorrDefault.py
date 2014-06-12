# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def CaloCellPedestalCorrDefault(name='CaloCellPedestalCorr'):
   import traceback
   from AthenaCommon.Logging import logging 
   mlog = logging.getLogger( 'CaloCellPedestalCorrDefault' )
   from AthenaCommon.AppMgr import ToolSvc
   from AthenaCommon.GlobalFlags import globalflags

   try:
        from CaloCellCorrection.CaloCellCorrectionConf import CaloCellPedestalCorr
        theCaloCellPedestalCorr =  CaloCellPedestalCorr("CaloCellPedestalCorr")
   except:
       mlog.error("could not get handle to CaloCellPedestalCorr Quit")
       print traceback.format_exc()
   ToolSvc += theCaloCellPedestalCorr

   try:
       from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
       from CaloIdentifier import SUBCALO
       thePedestalTool = CaloCellContainerCorrectorTool("PedestalTool",
                   CaloNums = [ SUBCALO.NSUBCALO],
                   CellCorrectionToolNames = [theCaloCellPedestalCorr] )
   except:
        mlog.error("could not get handle to CaloCellContainerCorrectorTool Quit")
        print traceback.format_exc()
        return False


   if globalflags.DataSource()=='data' :
       from IOVDbSvc.CondDB import conddb
       if conddb.isOnline:
           folder  = '/CALO/Pedestal/CellPedestal'
           conddb.addFolder('CALO_ONL',folder)
       else:
           folder= '/CALO/Ofl/Pedestal/CellPedestal'
           conddb.addFolder('CALO_OFL',folder)
       theCaloCellPedestalCorr.FolderName = folder
       theCaloCellPedestalCorr.isMC=False
   else:
       theCaloCellPedestalCorr.isMC=True

   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
   from CaloRec.CaloCellFlags import jobproperties
   if globalflags.DataSource()=='data' and (not jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr()) and (not athenaCommonFlags.isOnline()):
      lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
      conddb.addFolder('TRIGGER_ONL',lumiFolder);
      theCaloCellPedestalCorr.Luminosity = -1
      theCaloCellPedestalCorr.LumiFolderName = lumiFolder

   if jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr() and (not athenaCommonFlags.isOnline()):
       from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
       theCaloLumiBCIDTool = CaloLumiBCIDToolDefault()
       ToolSvc += theCaloLumiBCIDTool
       theCaloCellPedestalCorr.LumiBCIDTool = theCaloLumiBCIDTool
   

   return thePedestalTool

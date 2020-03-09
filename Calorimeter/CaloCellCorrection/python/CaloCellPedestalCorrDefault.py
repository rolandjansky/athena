# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
       traceback.print_exc()
   #ToolSvc += theCaloCellPedestalCorr

   if globalflags.DataSource()=='data' :
       from IOVDbSvc.CondDB import conddb
       if conddb.isOnline:
           folder  = '/CALO/Pedestal/CellPedestal'
           conddb.addFolder('CALO_ONL',folder,className="CondAttrListCollection")
       else:
           folder= '/CALO/Ofl/Pedestal/CellPedestal'
           conddb.addFolder('CALO_OFL',folder,className="CondAttrListCollection")
       theCaloCellPedestalCorr.PedestalShiftFolder = folder
       theCaloCellPedestalCorr.isMC=False
   else:
       theCaloCellPedestalCorr.isMC=True

   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
   from CaloRec.CaloCellFlags import jobproperties
   if globalflags.DataSource()=='data' and (not jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr()) and (not athenaCommonFlags.isOnline()):
      lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
      conddb.addFolder('TRIGGER_ONL',lumiFolder,className="CondAttrListCollection");
      theCaloCellPedestalCorr.Luminosity = -1
      theCaloCellPedestalCorr.LumiFolderName = lumiFolder

   if jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr() and (not athenaCommonFlags.isOnline()):
      from CaloRec.CaloBCIDAvgAlgDefault import CaloBCIDAvgAlgDefault
      CaloBCIDAvgAlgDefault()
      theCaloCellPedestalCorr.CaloBCIDAverageKey="CaloBCIDAverage"
   else:
      theCaloCellPedestalCorr.CaloBCIDAverageKey=""


   return theCaloCellPedestalCorr

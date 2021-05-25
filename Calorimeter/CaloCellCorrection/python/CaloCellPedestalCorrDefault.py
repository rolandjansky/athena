# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def CaloCellPedestalCorrDefault(name='CaloCellPedestalCorr'):
   import traceback
   from AthenaCommon.Logging import logging 
   mlog = logging.getLogger( 'CaloCellPedestalCorrDefault' )
   from AthenaCommon.GlobalFlags import globalflags
   from AthenaCommon.AlgSequence import AthSequencer
   condSeq = AthSequencer ('AthCondSeq')

   try:
      from CaloCellCorrection.CaloCellCorrectionConf import CaloCellPedestalCorr
      theCaloCellPedestalCorr =  CaloCellPedestalCorr("CaloCellPedestalCorr")
   except Exception:
      mlog.error("could not get handle to CaloCellPedestalCorr Quit")
      traceback.print_exc()

   if globalflags.DataSource()=='data' :
       from IOVDbSvc.CondDB import conddb
       if conddb.isOnline:
           folder  = '/CALO/Pedestal/CellPedestal'
           conddb.addFolder('CALO_ONL',folder,className="CondAttrListCollection")
       else:
           folder= '/CALO/Ofl/Pedestal/CellPedestal'
           conddb.addFolder('CALO_OFL',folder,className="CondAttrListCollection")
       from CaloCellCorrection.CaloCellCorrectionConf import CaloCellPedCorrCondAlg
       condSeq+=CaloCellPedCorrCondAlg(PedestalShiftFolder=folder)
       theCaloCellPedestalCorr.isMC=False
   else:
       theCaloCellPedestalCorr.isMC=True

   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
   from CaloRec.CaloCellFlags import jobproperties
   if globalflags.DataSource()=='data' and (not jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr()) and (not athenaCommonFlags.isOnline()):
      lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
      conddb.addFolder('TRIGGER_ONL',lumiFolder,className="CondAttrListCollection")
      condSeq.CaloCellPedCorrCondAlg.Luminosity = -1
      condSeq.CaloCellPedCorrCondAlg.LumiFolderName = lumiFolder

   if jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr() and (not athenaCommonFlags.isOnline()):
      from CaloRec.CaloBCIDAvgAlgDefault import CaloBCIDAvgAlgDefault
      CaloBCIDAvgAlgDefault()
      theCaloCellPedestalCorr.CaloBCIDAverageKey="CaloBCIDAverage"
   else:
      theCaloCellPedestalCorr.CaloBCIDAverageKey=""


   return theCaloCellPedestalCorr

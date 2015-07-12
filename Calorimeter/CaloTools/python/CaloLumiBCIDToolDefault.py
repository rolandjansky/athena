# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *

def CaloLumiBCIDToolDefault(name='CaloLumiBCIDToolDefault'):
    mlog = logging.getLogger( 'CaloLumiBCIDToolDefault' )

    from CaloTools.CaloToolsConf import CaloLumiBCIDTool
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource()=='data':
      from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
      theLumiTool = LuminosityToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc += theLumiTool
      from IOVDbSvc.CondDB import conddb
      #conddb.addFolder("","/LAR/ElecCalibOfl/LArPileupShape<db>sqlite://;schema=/afs/cern.ch/user/g/gunal/public/DB/bcid/shape.db;dbname=COMP200</db><key>LArShape32</key><tag>LARElecCalibOflLArPileupShape-mc</tag>")
      #conddb.addFolder("","/LAR/ElecCalibOfl/LArPileupAverage<db>sqlite://;schema=/afs/cern.ch/user/g/gunal/public/DB/bcid/LArPileupAverage-data.db;dbname=COMP200</db><tag>LARElecCalibOflLArPileupAverage-data11-00</tag>")
      if athenaCommonFlags.isOnline:
         conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupShape<key>LArShape32</key>")
         conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupAverage")
      else:
         conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>")
         conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupAverage")
      theTool = CaloLumiBCIDTool(name,isMC=False,LumiTool=theLumiTool,keyShape="LArShape32")
    else:
      from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
      theOFCTool = LArOFCToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc += theOFCTool
      from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
      theBunchCrossingTool = BunchCrossingTool()
      from IOVDbSvc.CondDB import conddb
      conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/Shape")
      #conddb.addFolder("","/LAR/ElecCalibMC/LArPileupAverage<db>sqlite://;schema=/afs/cern.ch/user/g/gunal/public/DB/bcid/LArPileupAverage-mc.db;dbname=OFLP200</db><tag>LARElecCalibMCLArPileupAverage-mc11b-00</tag>")
      conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/LArPileupAverage")
      theTool = CaloLumiBCIDTool(name,
                      isMC=True,
                      LArOFCTool = theOFCTool,BunchCrossingTool = theBunchCrossingTool)
    return theTool

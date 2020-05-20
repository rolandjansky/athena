# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *

def CaloLumiBCIDSCToolDefault(name='CaloLumiBCIDSCToolDefault'):
    mlog = logging.getLogger( 'CaloLumiBCIDSCToolDefault' )
    print 'trying here'

    from CaloTools.CaloToolsConf import CaloLumiBCIDTool
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource()=='data':
      print 'tool not available for data by now'
      from GaudiKernel.GaudiHandles import PublicToolHandle, PublicToolHandleArray
      theTool = PublicToolHandle('')
    else:
      from LArRecUtils.LArOFCSCToolDefault import LArOFCSCToolDefault
      theOFCTool = LArOFCSCToolDefault()
      from AthenaCommon.AppMgr import ToolSvc
      ToolSvc += theOFCTool
      from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
      theBunchCrossingTool = BunchCrossingTool()
      from IOVDbSvc.CondDB import conddb
      conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/Shape")
      conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/MinBias")
      conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/LArPileupAverage<tag>LARElecCalibMCSCLArPileupAverage-IOVDEP-00</tag>")
      theTool = CaloLumiBCIDTool(name,
                      isMC=True,
                      isSC=True,
                      LArOFCTool = theOFCTool,BunchCrossingTool = theBunchCrossingTool,
                      keyMinBiasAverge='LArMinBiasAverageSC',
                      keyShape='LArShapeSC'
                      )
    return theTool

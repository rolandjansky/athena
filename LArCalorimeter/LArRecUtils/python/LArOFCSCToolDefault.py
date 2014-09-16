# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArOFCTool
            
def LArOFCSCToolDefault (name="LArOFCSCToolDefault", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArOFCTool (**kw)

    mlog = logging.getLogger( 'LArOFCSCToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    if globalflags.DataSource()=='data':
        return false
    else:
        tool.MCSym = False
        tool.IsMC  = True
        tool.IsSuperCell  = True
        tool.FromDatabase = False
        tool.keyShape = "LArShapeSC"
        tool.keyNoise = "LArNoiseSC"
        tool.keyPedestal = "LArPedestalSC"

        tool.UseDelta = 0 ### avoid unforseed options

        ###
        #tool.Dump = True
        
    from LArRecUtils.LArAutoCorrTotalSCToolDefault import LArAutoCorrTotalSCToolDefault
    theAutoCorrTool = LArAutoCorrTotalSCToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theAutoCorrTool
    tool.AutoCorrTool = theAutoCorrTool
    from IOVDbSvc.CondDB import conddb
    if ( conddb.isMC and not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') ) :
        conddb.addFolder("LAR_OFL","<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")
    if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Pedestal') ) :
        conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCPedestal-000</tag>/LAR/ElecCalibMCSC/Pedestal")
    if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Noise') ) :
        conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCNoise-000</tag>/LAR/ElecCalibMCSC/Noise")
    if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Shape') ) :
        conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCShape-000</tag>/LAR/ElecCalibMCSC/Shape")
    return tool
    

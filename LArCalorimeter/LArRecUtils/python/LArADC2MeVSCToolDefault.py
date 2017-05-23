# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from LArConditionsCommon.LArCondFlags import larCondFlags


def LArADC2MeVSCToolDefault (name="LArADC2MeVSCToolDefault", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArADC2MeVTool (**kw)

    mlog = logging.getLogger( 'LArADC2MeVSCToolDefault::__init__ ' )
    mlog.info("entering")

    tool.IsSC=True
    tool.MCSym = False
    
    tool.UseFEBGainTresholds=True

    # do the configuration
    if globalflags.DataSource()=='data':
        # to be changed to True when everything is ready
        mlog.info("in data case")
        tool.UseMphysOverMcal = True
        tool.UseHVScaleCorr = False
        
        if larCondFlags.useLArFEBGainThresholds():
            from LArRecUtils.LArFEBConfigReaderDefault import LArFEBConfigReaderDefault
            theLArFebConfigReader=LArFEBConfigReaderDefault()
            #theLArFebConfigReader.OutputLevel=DEBUG
            ServiceMgr.ToolSvc+=theLArFebConfigReader
            tool.FebConfigReader=theLArFebConfigReader
            tool.UseFEBGainTresholds=True
    else:
        mlog.info("in MC case")
        tool.UseMphysOverMcal = False
        tool.UseHVScaleCorr = False
        tool.keyADC2DAC='LArRampSC'
        tool.keyDAC2uA='LArDAC2uASC'
        tool.keyuA2MeV='LAruA2MeVSC'
        from IOVDbSvc.CondDB import conddb
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/DAC2uA') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCDAC2uA-000</tag>/LAR/ElecCalibMCSC/DAC2uA")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/uA2MeV') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCuA2MeV-000</tag>/LAR/ElecCalibMCSC/uA2MeV")
        if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Ramp') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCRamp-000</tag>/LAR/ElecCalibMCSC/Ramp")
    return tool

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArParabolaPeakRecoTool
            
def LArParabolaPeakRecoToolDefault (name="LArParabolaPeakRecoToolDefault",
                                    **kw):
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArParabolaPeakRecoTool (**kw)

    mlog = logging.getLogger( 'LArParabolaPeakRecoToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    tool.correctBias=True
    return tool
        

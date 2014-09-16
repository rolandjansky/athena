# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
            
def LArOFPeakRecoToolDefault (name="LArOFPeakRecoToolDefault", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArOFPeakRecoTool (**kw)

    mlog = logging.getLogger( 'LArOFPeakRecoToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    if globalflags.DataSource()=='data':
        tool.MCSym = False
    else:
        tool.MCSym = True
    return tool


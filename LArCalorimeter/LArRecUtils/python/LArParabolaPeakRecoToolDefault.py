# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

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
        

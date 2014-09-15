# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonAlignModuleToolGetter.py
## @brief:  Creates and configures MuonAlignModuleTool.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## MuonAlignModuleToolGetter creates and configures MuonAlignModuleTool, but 
#  does not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MuonAlignModuleToolGetter ( Configured ) :

    _name = 'MuonAlignModuleToolGetter'

    ## creates and configures MuonAlignModuleTool using pre-set flags.
    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonAlignModuleTool
        muonAlignModuleTool = Muon__MuonAlignModuleTool(OutputLevel = align.OutputLevel(),
                                                        AlignModuleListType = mualign.alignModuleListType())
                
        self._MuonAlignModuleToolHandle = muonAlignModuleTool

        mlog.info(" now adding to ToolSvc" )
        from __main__ import ToolSvc
        ToolSvc += self.MuonAlignModuleToolHandle()

        mlog.info(" leaving configure" )

        return True

    ## returns handle to MuonAlignModuleTool
    def MuonAlignModuleToolHandle ( self ) :
        return self._MuonAlignModuleToolHandle

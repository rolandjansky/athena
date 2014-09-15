# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonAlignDBToolGetter.py
## @brief:  Creates and configures MuonAlignDBTool.
## @author: Robert Harrington <roberth@cern.ch>
## @date:   3/7/2011

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
from AthenaCommon.Constants import *

## MuonAlignDBToolGetter creates and configures MuonAlignDBTool, but does
#  not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MuonAlignDBToolGetter ( Configured ) :

    _name = 'MuonAlignDBToolGetter'

    ## create and configure MuonAlignDBTool using pre-set flags
    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        muonIO = MuonAlignmentIOFiles()

        # create tool instance
        
        from __main__ import ToolSvc
        
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonAlignDBTool
        muonAlignDBTool = Muon__MuonAlignDBTool(OutputLevel = DEBUG, #align.OutputLevel(),
                                                ASZTOutputFile = "asztOutput.txt",
                                                AlternateASZTFile = str(''),
                                                SigmaOutputFile = '',
                                                SigmaInputFile = '',
                                                UseMultiLevelDbTool = mualign.doMultiLevelAlignment()) 
        
        if mualign.useAlternateASZT() : 
            muonAlignDBTool.AlternateASZTFile = "asztInput.txt"
        self._MuonAlignDBToolHandle = muonAlignDBTool
        if mualign.doMultiLevelAlignment :
            MultiLevelAlignmentDbTool = ToolSvc.MultiLevelAlignmentDbTool
        else:
            from MuonCondTool.MuonCondToolConf import MultiLevelAlignmentDbTool
            MultiLevelAlignmentDbTool = MultiLevelAlignmentDbTool("MultiLevelAlignmentDbTool")
            MultiLevelAlignmentDbTool.ABLineDbTool = ToolSvc.MGM_AlignmentDbTool
            
        mlog.info(" leaving configure" )

        return True

    ## return handle to MuonAlignDBTool
    def MuonAlignDBToolHandle ( self ) :
        return self._MuonAlignDBToolHandle

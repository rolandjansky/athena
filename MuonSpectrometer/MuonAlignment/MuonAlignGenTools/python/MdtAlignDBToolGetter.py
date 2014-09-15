# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MdtAlignDBToolGetter.py
## @brief:  Creates and configures MdtAlignDBTool.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
from AthenaCommon.Constants import *

## MdtAlignDBToolGetter creates and configures MdtAlignDBTool, but does
#  not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MdtAlignDBToolGetter ( Configured ) :

    _name = 'MdtAlignDBToolGetter'

    ## create and configure MdtAlignDBTool using pre-set flags
    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        muonIO = MuonAlignmentIOFiles()

        # create tool instance
        
        from __main__ import ToolSvc
        
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MdtAlignDBTool
        muonAlignDBTool = Muon__MdtAlignDBTool(OutputLevel = DEBUG, #align.OutputLevel(),
                                               ABLineDbTool = ToolSvc.MGM_AlignmentDbTool,
                                               #MultiLevelAlignmentDbTool = ToolSvc.MGM_AlignmentDbTool,
                                                #ASZTOutputFile = muonIO.asztOutputFile(),
                                                ASZTOutputFile = "asztOutput.txt",
                                                AlternateASZTFile = str(''),
                                                SigmaOutputFile = '',
                                                SigmaInputFile = '',
                                                UseMultiLevelDbTool = mualign.doMultiLevelAlignment()) 
                                                #UseMultiLevelDbTool = True)
                                                #StoreL3Params = False)
        
        if mualign.useAlternateASZT() : 
            #muonAlignDBTool.AlternateASZTFile = muonIO.asztInputFile()
            muonAlignDBTool.AlternateASZTFile = "asztInput.txt"
        self._MdtAlignDBToolHandle = muonAlignDBTool
        if mualign.doMultiLevelAlignment :
            MultiLevelAlignmentDbTool = ToolSvc.MultiLevelAlignmentDbTool
        else:
            from MuonCondTool.MuonCondToolConf import MultiLevelAlignmentDbTool
            MultiLevelAlignmentDbTool = MultiLevelAlignmentDbTool("MultiLevelAlignmentDbTool")
            MultiLevelAlignmentDbTool.ABLineDbTool = ToolSvc.MGM_AlignmentDbTool

        mlog.info(" leaving configure" )

        return True

    ## return handle to MdtAlignDBTool
    def MdtAlignDBToolHandle ( self ) :
        return self._MdtAlignDBToolHandle

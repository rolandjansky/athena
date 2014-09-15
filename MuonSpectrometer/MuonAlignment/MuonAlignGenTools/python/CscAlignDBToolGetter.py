# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file:   CscAlignDBToolGetter.py
## @brief:  Creates and configures CscAlignDBTool.
## @author: Prolay Mal (prolay@physics.arizona.edu)
## @date:   03/28/2010

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign
from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
from AthenaCommon.Constants import *

## CscAlignDBToolGetter creates and configures CscAlignDBTool, but does
#  not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class CscAlignDBToolGetter ( Configured ) :

    _name = 'CscAlignDBToolGetter'

    ## create and configure MdtAlignDBTool using pre-set flags
    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        muonIO = MuonAlignmentIOFiles()

        # create tool instance

        from __main__ import ToolSvc

        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__CscAlignDBTool
        muonAlignDBTool = Muon__CscAlignDBTool(OutputLevel = align.OutputLevel(),
                                               ABLineDbTool = ToolSvc.MGM_AlignmentDbTool,
                                               ASZTOutputFile = muonIO.asztOutputFile(),
                                               AlternateASZTFile = str(''),
                                               SigmaOutputFile = '',
                                               SigmaInputFile = '',
                                               UseMultiLevelDbTool = mualign.doMultiLevelAlignment()) 
        if mualign.useAlternateASZT() : 
            muonAlignDBTool.AlternateASZTFile = muonIO.asztInputFile()
        self._CscAlignDBToolHandle = muonAlignDBTool

        if mualign.doMultiLevelAlignment :
            MultiLevelAlignmentDbTool = ToolSvc.MultiLevelAlignmentDbTool
        else:
            from MuonCondTool.MuonCondToolConf import MultiLevelAlignmentDbTool
            MultiLevelAlignmentDbTool = MultiLevelAlignmentDbTool("MultiLevelAlignmentDbTool")
            MultiLevelAlignmentDbTool.ABLineDbTool = ToolSvc.MGM_AlignmentDbTool

        mlog.info(" leaving configure" )

        return True

    ## return handle to CscAlignDBTool
    def CscAlignDBToolHandle ( self ) :
        return self._CscAlignDBToolHandle

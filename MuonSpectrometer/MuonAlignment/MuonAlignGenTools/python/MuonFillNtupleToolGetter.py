# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonFillNtupleToolGetter.py
## @brief:  Creates and configures MuonFillNtupleTool.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## MuonFillNtupleToolGetter creates and configures MuonFillNtupleTool, but 
#  does not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MuonFillNtupleToolGetter ( Configured ) :

    _name = 'MuonFillNtupleToolGetter'

    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from TrkAlignGenTools.AlignResidualCalculatorGetter import AlignResidualCalculatorGetter
        resCalc = AlignResidualCalculatorGetter().ResidualPullCalculatorHandle()
        
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonFillNtupleTool
        from MuonAlignGenTools.MuonAlignmentIOFiles import MuonAlignmentIOFiles
        muonIO = MuonAlignmentIOFiles()
        muonFillNtupleTool = Muon__MuonFillNtupleTool(OutputLevel = align.OutputLevel(),
                                                      ResidualPullCalculator = resCalc,
                                                      MuonContainer = mualign.MuonContainerName())

        from MuonAlignGenTools.MuonAlignModuleToolGetter import MuonAlignModuleToolGetter
        muonAlignModuleTool = MuonAlignModuleToolGetter().MuonAlignModuleToolHandle()
        muonFillNtupleTool.AlignModuleTool = muonAlignModuleTool
        muonFillNtupleTool.BigNtuple = mualign.doBigNtuple()

        
        from MuTagTools.MuTagToolsConf import MuTagEDMHelper
        ConfiguredMuTagEDMHelper = MuTagEDMHelper("ConfiguredMuTagEDMHelper")
        from __main__ import ToolSvc
        ToolSvc += ConfiguredMuTagEDMHelper        
        muonFillNtupleTool.muTagEDMHelperTool = ConfiguredMuTagEDMHelper

        self._MuonFillNtupleToolHandle = muonFillNtupleTool

        mlog.info(" leaving configure" )

        return True

    def MuonFillNtupleToolHandle ( self ) :
        return self._MuonFillNtupleToolHandle

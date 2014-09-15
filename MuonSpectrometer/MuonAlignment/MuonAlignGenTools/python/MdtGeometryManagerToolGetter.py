# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MdtGeometryManagerToolGetter.py
## @brief:  Creates and configures MdtGeometryManagerTool.
## @author: Robert Harrington <roberth@cern.ch>
## @date:   02/22/2011

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## Created and configures MdtGeometryManagerTool as a public tool, added 
#  to ToolSvc
class MdtGeometryManagerToolGetter ( Configured ) :

    _name = 'MdtGeometryManagerToolGetter'

    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MdtGeometryManagerTool
        mdtGeometryManagerTool = Muon__MdtGeometryManagerTool(OutputLevel = align.OutputLevel(),
                                                               MdtAlignParameters = mualign.mdtAlignParams())
        self._MdtGeometryManagerToolHandle = mdtGeometryManagerTool

        mlog.info(" now adding to ToolSvc" )
        from __main__ import ToolSvc
        ToolSvc += self.MdtGeometryManagerToolHandle()

        mlog.info(" leaving configure" )

        return True

    def MdtGeometryManagerToolHandle ( self ) :
        return self._MdtGeometryManagerToolHandle

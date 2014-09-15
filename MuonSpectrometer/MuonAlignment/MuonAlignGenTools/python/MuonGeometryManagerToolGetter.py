# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonGeometryManagerToolGetter.py
## @brief:  Creates and configures MuonGeometryManagerTool.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## Created and configures MuonGeometryManagerTool as a public tool, added 
#  to ToolSvc
class MuonGeometryManagerToolGetter ( Configured ) :

    _name = 'MuonGeometryManagerToolGetter'

    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonGeometryManagerTool, Muon__MdtGeometryManagerTool, Muon__CscGeometryManagerTool, Muon__TgcGeometryManagerTool

        # MDT
        mdtGeometryManagerTool =  Muon__MdtGeometryManagerTool(OutputLevel = align.OutputLevel(),
                                                               AlignLevel = align.alignmentLevel(),
                                                               AlignTraX=mualign.mdtAlignTraX(),
                                                               AlignTraY=mualign.mdtAlignTraY(),
                                                               AlignTraZ=mualign.mdtAlignTraZ(),
                                                               AlignRotX=mualign.mdtAlignRotX(),
                                                               AlignRotY=mualign.mdtAlignRotY(),
                                                               AlignRotZ=mualign.mdtAlignRotZ())
        self._MdtGeometryManagerToolHandle = mdtGeometryManagerTool

        # CSC
        cscGeometryManagerTool =  Muon__CscGeometryManagerTool(OutputLevel = align.OutputLevel(),
                                                               AlignLevel = align.alignmentLevel())
        self._CscGeometryManagerToolHandle = cscGeometryManagerTool

        # TGC
        tgcGeometryManagerTool =  Muon__TgcGeometryManagerTool(OutputLevel = align.OutputLevel(),
                                                               AlignLevel = align.alignmentLevel())
        self._TgcGeometryManagerToolHandle = tgcGeometryManagerTool
        
        mlog.info(" now adding geometryManagerTools to ToolSvc" )
        from __main__ import ToolSvc
        ToolSvc += self.MdtGeometryManagerToolHandle()
        ToolSvc += self.CscGeometryManagerToolHandle()
        ToolSvc += self.TgcGeometryManagerToolHandle()

        muonGeometryManagerTool = Muon__MuonGeometryManagerTool(OutputLevel = align.OutputLevel(),
                                                                AlignLevel = align.alignmentLevel(),
                                                                MdtGeometryManager = mdtGeometryManagerTool,
                                                                CscGeometryManager = cscGeometryManagerTool,
                                                                TgcGeometryManager = tgcGeometryManagerTool,
                                                                doModuleSelection = mualign.doModuleSelection(),
                                                                AlignMdt = mualign.alignMDT(),
                                                                AlignTgc = False, #mualign.alignTGC(),
                                                                AlignCsc = False, #mualign.alignCSC(),
                                                                ModuleSelection = mualign.modulesToAlign())
        self._MuonGeometryManagerToolHandle = muonGeometryManagerTool
        
        mlog.info(" now adding MuonGeometryManagerTool to ToolSvc" )
        ToolSvc += self.MuonGeometryManagerToolHandle()

        mlog.info(" leaving configure" )

        return True

    def MuonGeometryManagerToolHandle ( self ) :
        return self._MuonGeometryManagerToolHandle

    def MdtGeometryManagerToolHandle ( self ) :
        return self._MdtGeometryManagerToolHandle

    def CscGeometryManagerToolHandle ( self ) :
        return self._CscGeometryManagerToolHandle

    def TgcGeometryManagerToolHandle ( self ) :
        return self._TgcGeometryManagerToolHandle

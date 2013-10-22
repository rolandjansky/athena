# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   GlobalChi2AlignToolGetter.py
## @brief:  to instantiate the GlobalChi2AlignTool
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## GlobalChi2AlignToolGetter creates and configures the 
#  GlobalChi2AlignTool with teh default configuration.  The tool
#  is not added to the ToolSvc so user must use as a private tool
#  or add to ToolSvc.
class GlobalChi2AlignToolGetter( Configured ) :

    _name = 'GlobalChi2AlignToolGetter'

    ## create and configure tool
    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from TrkAlignGenTools.MatrixToolGetter import MatrixToolGetter
        matrixTool = MatrixToolGetter().MatrixToolHandle()
        
        from TrkGlobalChi2AlignTools.TrkGlobalChi2AlignToolsConf import Trk__GlobalChi2AlignTool
        globalChi2AlignTool = Trk__GlobalChi2AlignTool("GlobalChi2AlignTool",
                                                       OutputLevel = align.OutputLevel(),
                                                       MatrixTool = matrixTool,
                                                       DoTree = align.writeNtuple())
                                                       #SecondDerivativeCut = align.secondDerivativeCut())

        if align.derivativeType=='Numerical':
            globalChi2AlignTool.WriteActualSecDeriv = True

        self._globalChi2AlignToolHandle = globalChi2AlignTool

        #mlog.info(" now adding to ToolSvc" )
        #from __main__ import ToolSvc
        #ToolSvc += self.GlobalChi2AlignToolHandle()
        
        mlog.info(" leaving configure")

        return True

    ## return handle for GlobalChi2AlignTool
    def GlobalChi2AlignToolHandle(self):
        return self._globalChi2AlignToolHandle

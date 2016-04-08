# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   AlignSolveGetter.py
## @brief:  to instantiate the AlignSolve
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009


from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## AlignSolveGetter configures the AlignSolve with default tools and
#  options and adds to topSequence.  (Detector-specific tools 
#  must be configured after AlignSolve is created.)
class AlignSolveGetter ( Configured ) :

    _name = 'AlignSolveGetter'

    ## create and configure AlignSolve
    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :')

        from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__AlignSolve
        alignAlg = Trk__AlignSolve("AlignSolve",
                                 OutputLevel = align.OutputLevel(),
                                 AlignTool = self.getAlignTool())

        self._alignAlgHandle = alignAlg

        # add to topSequence
        mlog.info(" now adding to topSequence" )
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.AlignSolveHandle()

        mlog.info(" leaving configure" )

        return True

    ## returns handle to AlignSolve
    def AlignSolveHandle(self):
        return self._alignAlgHandle

    ## returns align tool
    def getAlignTool(self) :
        
        if align.alignmentType() == 'GlobalChi2' :
            from TrkGlobalChi2AlignTools.GlobalChi2AlignToolGetter import GlobalChi2AlignToolGetter
            return GlobalChi2AlignToolGetter().GlobalChi2AlignToolHandle()
        
        else:
            print "only global chi2 supported for now!"
            return
          

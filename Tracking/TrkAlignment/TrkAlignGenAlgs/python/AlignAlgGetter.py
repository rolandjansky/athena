# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   AlignAlgGetter.py
## @brief:  to instantiate the AlignAlg
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009


from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## AlignAlgGetter configures the AlignAlg with default tools and
#  options and adds to topSequence.  (Detector-specific tools 
#  must be configured after AlignAlg is created.)
class AlignAlgGetter ( Configured ) :

    _name = 'AlignAlgGetter'

    ## create and configure AlignAlg
    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :')

        from TrkAlignGenTools.AlignTrackCreatorGetter import AlignTrackCreatorGetter
        alignTrackCreator = AlignTrackCreatorGetter().AlignTrackCreatorHandle()

        from TrkAlignGenTools.AlignTrackDresserGetter import AlignTrackDresserGetter
        alignTrackDresser = AlignTrackDresserGetter().AlignTrackDresserHandle()
        
        from TrkAlignGenTools.AlignTrackDresserGetter import AlignTrackDresserGetter
        alignTrackDresser = AlignTrackDresserGetter().AlignTrackDresserHandle()

        from TrkAlignGenAlgs.TrkAlignGenAlgsConf import Trk__AlignAlg
        alignAlg = Trk__AlignAlg("AlignAlg",
                                 OutputLevel = align.OutputLevel(),
                                 AlignTrackCreator = alignTrackCreator,
                                 AlignTrackDresser = alignTrackDresser,
                                 AlignTool = self.getAlignTool(),
                                 WriteNtuple = align.writeNtuple(),
                                 FileName = align.ntupleFileName(),
                                 FilePath = align.ntupleFilePath(),
                                 SolveOnly = align.solveOnly())

        self._alignAlgHandle = alignAlg

        # add to topSequence
        mlog.info(" now adding to topSequence" )
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += self.AlignAlgHandle()

        mlog.info(" leaving configure" )

        return True

    ## returns handle to AlignAlg
    def AlignAlgHandle(self):
        return self._alignAlgHandle

    ## returns align tool
    def getAlignTool(self) :
        
        if align.alignmentType() == 'GlobalChi2' :
            from TrkGlobalChi2AlignTools.GlobalChi2AlignToolGetter import GlobalChi2AlignToolGetter
            return GlobalChi2AlignToolGetter().GlobalChi2AlignToolHandle()
        
        else:
            print "only global chi2 supported for now!"
            return
          

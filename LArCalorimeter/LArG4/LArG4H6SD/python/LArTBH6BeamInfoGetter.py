# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging        import logging
from RecExConfig.Configured      import Configured
from RecExConfig.ObjKeyStore     import objKeyStore
from AthenaCommon.OldStyleConfig import Algorithm
import traceback
import sys

#
# These should probably be in a common library.
#
def _makeconf (cls, name = None, **kwargs):
    if name != None:
        x = cls(name)
    else:
        x = cls()
    for (k,v) in kwargs.items():
        setattr (x, k, v)
    return x


class LArTBH6BeamInfoGetter (Configured):
    # Must be set
    #_name = xxx
    #_suffix = xxx

    def configure (self):
        # Make a logger.
        clsname = self.__class__.__name__
        mlog = logging.getLogger ('%s:configure : ' % clsname)

        maindict = sys.modules['__main__'].__dict__

        # Create the configurable.
        try:        
            from LArG4H6SD.LArG4H6SDConf import LArTBH6BeamInfo                
            theLArTBH6BeamInfo = LArTBH6BeamInfo()
        except:
            mlog.error("could not import LArG4H6SD.LArTBH6BeamInfo")
            print traceback.format_exc()
            return False
        self._LArTBH6BeamInfoHandle = theLArTBH6BeamInfo ;

        theLArTBH6BeamInfo.HitsContainer    = ["LArTBFrontHitCollection"]
        theLArTBH6BeamInfo.PrimaryTrackOnly = TRUE
        theLArTBH6BeamInfo.PrimaryParticle  = 999

        # now add algorithm to topSequence
        from __main__ import topSequence
        topSequence += theLArTBH6BeamInfo;

        return True

    def handle(self):
        return self._handle



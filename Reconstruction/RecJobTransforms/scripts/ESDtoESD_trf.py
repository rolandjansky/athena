#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Rerun reco on ESD and write out another ESD using RecExCommon."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class ESDtoESDJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('Rolf Seuster','seuster at cern.ch')],
                                skeleton='RecJobTransforms/skeleton.ESDtoESD.py' ,
                                help = __doc__,lastInChain=lastInChain )


    def matchEvents(self):
        return self.matchEventsExpectEqual("inputESDFile","outputESDFile")


# executed if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = ESDtoESDJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

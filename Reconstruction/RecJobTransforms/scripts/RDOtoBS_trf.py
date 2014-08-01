#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Run reconstruction and make BS's from RDO using RecExCommon."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class RDOtoBSJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('David Cote','david.cote@cern.ch')],
                                skeleton='RecJobTransforms/skeleton.RDOtoBS.py' ,
                                help = __doc__,lastInChain=lastInChain )


    def matchEvents(self):
        #Note: _namedArgs has lower case keys 
        if self._namedArgs.has_key("inputbsfile"):
            self.logger().warning("MatchEvents currently not executed for input bytestream because of potentially skipped truncated events.")
            return

        return self.matchEventsExpectEqual("inputRDOFile","outputBSFile")


# execute it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = RDOtoBSJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

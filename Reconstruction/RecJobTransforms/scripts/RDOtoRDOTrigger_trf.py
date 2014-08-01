#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Run trigger only from RDO to RDOprime using RecExCommon."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class RDOtoRDOTriggerJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('Graeme A Stewart','graeme.andrew.stewart@cern.ch')],
                                skeleton='RecJobTransforms/skeleton.RDOtoRDOtrigger.py' ,
                                help = __doc__,lastInChain=lastInChain)


    def matchEvents(self):
        return self.matchEventsExpectEqual("inputRDOFile","outputRDOFile")


def main():
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = RDOtoRDOTriggerJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())


# execute it if not imported
if __name__ == '__main__':
    main()

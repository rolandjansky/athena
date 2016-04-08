#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Make DPD from ESD."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class ESDtoDPDJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('David Cote','david.cote@cern.ch')],
                                skeleton='PATJobTransforms/skeleton.ESDtoDPD_trf.py' ,
                                help = __doc__,lastInChain=lastInChain )

    def matchEvents(self):
        self.logger().info("MatchEvents is not executed for DPD outputs.")
        return


#----------------------------------------------------------------------
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = ESDtoDPDJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge RAW->ESD and ESD->AOD monitoring histograms."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class DQHistogramMergeJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors = [ Author('David Cote', 'David.Cote@cern.ch')],
                                skeleton='PATJobTransforms/skeleton.DQHistogramMerge_trf.py',
                                help = __doc__,lastInChain=lastInChain)

    def matchEvents(self):
        self.logger().info("MatchEvents is not executed for DQHistogramMerge.")
        return

    def forceSingleProcess(self):
        import os
        os.putenv('ATHENA_PROC_NUMBER','0')
        for opt in self._athenaOptions:
            if opt.split('=')[0]=='--nprocs' :
                self._athenaOptions.remove(opt)
                break

#----------------------------------------------------------------------
if __name__ == '__main__':    
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = DQHistogramMergeJobTransform(inDic)          
    sys.exit(trf.exeArgDict(inDic).exitCode())


#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Make TAG's from AOD's"""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class AODtoTAGJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors = [ Author('Tulay Cuhadar Donszelmann', 'tcuhadar@cern.ch'),
                                            Author('David Cote', 'David.Cote@cern.ch')],
                                skeleton='PATJobTransforms/skeleton.AODtoTAG_trf.py',
                                help = __doc__, lastInChain=lastInChain)

    def matchEvents(self):
        return self.matchEventsExpectEqual("inputAODFile","outputTAGFile")

#----------------------------------------------------------------------
if __name__ == '__main__':    
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = AODtoTAGJobTransform(inDic)          
    sys.exit(trf.exeArgDict(inDic).exitCode())


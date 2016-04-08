#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__=""" Merging of HIT files """

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

class MergeHITSJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                              authors = [ Author('Jack Cranshaw','Jack.Cranshaw@cern.ch'),
                                          Author('Borut Kersevan', 'Borut.Kersevan@cern.ch') ] ,
                              skeleton='PATJobTransforms/skeleton.MergeHIT.py' ,
                              help = __doc__, lastInChain=lastInChain )

##         #add arguments
##         self.add( InputHitsFileArg() )
##         self.add( InputLogsFileArg() )
##         self.add( OutputHitsFileArg() )
##         self.add( MaxEventsArg() )
##         self.add( SkipEventsArg() )
##         self.add( GeometryVersionArg() )
##         self.add( JobConfigArg(package='SimuJobTransforms'), default='NONE' )
##         self.add( DBReleaseArg(), default='NONE' )
##         self.add( IgnoreConfigErrorArg(), default=False )

##         # avoid sqlite nfs lock problems
##         self.add( SQLiteSupport() )


# execute transform it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = MergeHITSJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

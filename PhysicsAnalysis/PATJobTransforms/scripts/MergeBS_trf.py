#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """BS->BS"""

from PyJobTransformsCore.trf import JobTransform,Author,JobReport
from PATJobTransforms.ArgDicTools import DefaultConfigFromSysArgv

ListOfDefaultPositionalKeys=['maxEvents','inputBSFile','outputBSFile','preInclude','postInclude','preExec','postExec','--ignoreunknown','--athenaopts']

class MergeBSJobTransform( JobTransform ):
    def __init__(self,inDic):
        JobTransform.__init__(self,                              
                              authors = [ Author('David Cote', 'David.Cote@cern.ch')],
                              skeleton='PATJobTransforms/skeleton.MergeBS.py',
                              help = __doc__ )

        if not isinstance(inDic,dict):
            raise TypeError("inDic has %s but should be a dictionary." %type(inDic))

        #Default configuration?
        if inDic.has_key('defaultFromPositionalValues'):
            inDic=DefaultConfigFromSysArgv(ListOfDefaultPositionalKeys,inDic)

        from PATJobTransforms.ConfigDicUtils import AutoConfigureFromDic
        self.inDic=inDic
        AutoConfigureFromDic(self,inDic)
        return

#----------------------------------------------------------------------
if __name__ == '__main__':    
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = MergeBSJobTransform(inDic)
    exitCode=trf.exeArgDict(inDic).exitCode()
    sys.exit(exitCode)


#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Make AOD's from ESD's using RecExCommon."""

import glob
import tarfile

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trfutil import jiveXMLtgz

class ESDtoAODJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('David Cote','david.cote@cern.ch')],
                                skeleton='RecJobTransforms/skeleton.ESDtoAOD.py' ,
                                help = __doc__,lastInChain=lastInChain )
        
        self._addPostRunAction(self, prepend=True)


    def matchEvents(self):
        return self.matchEventsExpectEqual("inputESDFile","outputAODFile")
   
    
    def postRunAction(self):
        # Merge JiveXML files into a zip archive
        print "Executing postRunActions for ESDtoAODJobTransform"
        if 'outputTXT_JIVEXMLTGZFile' in self.inDic:
            jiveXMLtgz(self.inDic['outputTXT_JIVEXMLTGZFile'])


# executed if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = ESDtoAODJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

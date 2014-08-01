#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Run reconstruction and make ESD's from RDO or BS using RecExCommon."""

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trfutil import jiveXMLtgz

class RAWtoESDJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic,lastInChain=True):
        BaseOfBasicTrf.__init__(self,inDic,
                                authors=[Author('David Cote','david.cote@cern.ch')],
                                skeleton='RecJobTransforms/skeleton.RAWtoESD.py' ,
                                help = __doc__, lastInChain=lastInChain )

        self._addPostRunAction(self, prepend=True)


    def matchEvents(self):
        #Note: _namedArgs has lower case keys 
        if self._namedArgs.has_key("inputbsfile"):
            self.logger().warning("MatchEvents currently not executed for input bytestream because of potentially skipped truncated events.")
            return

        return self.matchEventsExpectEqual("inputRDOFile","outputESDFile")

    
    def postRunAction(self):
        # Merge JiveXML files into a zip archive
        print "Executing postRunActions for RAWtoESDJobTransform"
        if 'outputTXT_JIVEXMLTGZFile' in self.inDic:
            jiveXMLtgz(self.inDic['outputTXT_JIVEXMLTGZFile'])


# execute it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = RAWtoESDJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

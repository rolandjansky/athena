#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Filters out Hits which can be neglected at higher pileup luminosities from HIT files.
Writes out a new HIT file with the appropriate sub-set of Hits/Hit Containers."""

from SimuJobTransforms.Configuration      import *
from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trf import JobTransform

ListOfDefaultPositionalKeys = ['inputHitsFile', 'outputHitsFile', 'maxEvents', 'skipEvents', 'DBRelease', 'preInclude', 'preExec', 'postInclude', 'postExec', 'geometryVersion', 'conditionsTag', 'TruthReductionScheme', 'IgnoreConfigError', 'AMITag', '--ignoreerrors', '--athenaopts', '--omitvalidation']

class FilterHITJobTransform( BaseOfBasicTrf ):
    def __init__(self, inputDic):
        BaseOfBasicTrf.__init__(self,
                                inputDic,
                                name = "FilterHit_trf",
                                authors = [ Author('John Chapman', 'John.Chapman@cern.ch') ] ,
                                skeleton='SimuJobTransforms/skeleton.FilterHit.py' ,
                                help = __doc__ )
        #Help?
        if inputDic.has_key('-h'):
            if 'ListOfDefaultPositionalKeys' in globals():
                print """
This design has one exception: ProdSys (Tier1), which can only use keys:

%s

To bypass an argument in positional mode, give value: 'NONE'.

Please use CmdToPositional.py to translate a key=val command into a positional one.

""" % ", ".join(ListOfDefaultPositionalKeys)

    def addAllArgs(self):
        from SimuJobTransforms.Configuration import addAllFilterArgs
        addAllFilterArgs(self, inDic)

    def doPreRunActions(self):
        JobTransform.doPreRunActions(self)
        if hasattr(self,'_maxEventsStrategy'):
            self._maxEventsStrategy = 'INPUTEVENTS'
        else:
            print "WARNING  FilterHITJobTransform has no attribute \'_maxEventsStrategy\'."
        
# execute it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic = BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = FilterHITJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())    

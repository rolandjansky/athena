#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Run Geant4 simulation on generated events and produce a HITS file.
"""

from SimuJobTransforms.Configuration      import *
from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trf import JobTransform

ListOfDefaultPositionalKeys = ['inputEvgenFile', 'outputEvgenFile', 'outputHitsFile', 'maxEvents', 'skipEvents', 'DBRelease', 'preInclude', 'preExec', 'postInclude', 'postExec', 'geometryVersion', 'conditionsTag', 'IgnoreConfigError', 'AMITag', 'randomSeed', 'physicsList', 'postInitInclude', 'firstEvent', 'CosmicFilterVolume', 'CosmicFilterVolume2', 'CosmicPtSlice', 'beamType', 'DataRunNumber', '--ignoreerrors', '--athenaopts', '--omitvalidation', 'LucidOn', 'ALFAOn', 'ZDCOn', 'AFPOn', 'FwdRegionOn', 'enableLooperKiller', 'DBContent', 'useISF']

class EVGENtoHITJobTransform( BaseOfBasicTrf ):
    def __init__(self, inputDic):
        BaseOfBasicTrf.__init__(self,
                            inputDic,
                            name = "AtlasG4_trf",
                            authors = [ Author('John Chapman', 'John.Chapman@cern.ch') ] ,
                            skeleton = 'SimuJobTransforms/skeleton.EVGENtoHIT_AtlasG4_trf.py' ,
                            help = __doc__ )

        #Help?
        if inputDic.has_key('-h'):
            if 'ListOfDefaultPositionalKeys' in globals():
                print """
This design has one exception: ProdSys (Tier1), which can only use keys:

%s

To bypass an argument in positional mode, give value: 'NONE'.

Please use CmdToPositional.py to translate a key=val command into a positional one.

See wiki page for details: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/SimuJobTransforms

""" % ", ".join(ListOfDefaultPositionalKeys)

    def addAllArgs(self):
        from SimuJobTransforms.Configuration  import addAllG4Args
        addAllG4Args(self,inDic)

    def doPreRunActions(self):
        JobTransform.doPreRunActions(self)
        if hasattr(self,'_maxEventsStrategy'):
            self._maxEventsStrategy = 'ABORT'
        else:
            print "WARNING EVGENtoHITJobTransform has no attribute \'_maxEventsStrategy\'."

# Execute it if not imported
if __name__ == '__main__':
    # Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic = BuildDicFromCommandLine(sys.argv)
    # Construct and execute the transform
    trf = EVGENtoHITJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

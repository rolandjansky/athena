#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Run digitization on a HITS file and produce an RDO file. Pile-up is optional."""

from SimuJobTransforms.Configuration      import *
from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author
from PyJobTransformsCore.trf import JobTransform

ListOfDefaultPositionalKeys = ['inputHitsFile', 'outputRDOFile', 'tmpRDO', 'maxEvents', 'skipEvents', 'DBRelease', 'preInclude', 'preExec', 'postInclude', 'postExec', 'geometryVersion', 'conditionsTag', 'IgnoreConfigError', 'AMITag', 'testPileUpConfig', 'beamType', 'digiSeedOffset1', 'digiSeedOffset2', 'LowPtMinbiasHitsFile', 'HighPtMinbiasHitsFile', 'cavernHitsFile', 'beamHaloHitsFile', 'beamGasHitsFile', 'digiRndmSvc', 'samplingFractionDbTag', 'numberOfLowPtMinBias', 'numberOfHighPtMinBias', 'numberOfBeamHalo', 'numberOfBeamGas', 'numberOfCavernBkg', 'bunchSpacing', 'pileupInitialBunch', 'pileupFinalBunch', 'doAllNoise', 'AddCaloDigi', 'DataRunNumber', 'LucidOn', 'ALFAOn', 'ZDCOn', 'AFPOn', 'FwdRegionOn', 'triggerConfig', 'jobNumber' ,'--ignoreerrors', '--athenaopts', '--omitvalidation']

class HITtoRDOJobTransform( BaseOfBasicTrf ):
    """Run digitization on a HITS file and produce an RDO file. Pile-up is optional."""
    def __init__(self, inputDic):
        BaseOfBasicTrf.__init__(self,
                                inputDic,
                                name = "Digi_trf",
                                authors = [ Author('John Chapman', 'John.Chapman@cern.ch') ] ,
                                skeleton='SimuJobTransforms/skeleton.HITtoRDO.py' ,
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
        from SimuJobTransforms.Configuration import addAllDigiArgs
        addAllDigiArgs(self,inDic)

    def doPreRunActions(self):
        JobTransform.doPreRunActions(self)
        if hasattr(self,'_maxEventsStrategy'):
            self._maxEventsStrategy = 'INPUTEVENTS'
        else:
            print "WARNING HITtoRDOJobTransform has no attribute \'_maxEventsStrategy\'."

# execute it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic = BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = HITtoRDOJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())    

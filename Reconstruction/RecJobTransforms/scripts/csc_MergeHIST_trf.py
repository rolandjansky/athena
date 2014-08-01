#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Merge several input DMQ monitoring histogram files into a single output file. For merging of NTUPs please use MergeNTUP_trf.py"""

from PyJobTransformsCore.trf import *
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trfutil import *
from RecJobTransforms.RecConfig import recConfig

class MergeHISTTransform( JobTransform ):
    def __init__(self,lastInChain=True):
        JobTransform.__init__(self,
                              authors = [ Author('Max Baak', 'mbaak@cern.ch') ] ,
                              skeleton='RecJobTransforms/skeleton.csc_MergeHIST_trf.py' ,
                              help = __doc__ ,
                              config = recConfig,lastInChain=lastInChain)

        ## add arguments
        self.add( InputMonitorHistArg(name='inputROOTFile'), default='NONE' )
        self.add( MonitorHistArg(name='outputROOTFile'), default='NONE' )
        self.add( JobConfigArg(package='RecJobTransforms'), default='NONE' )
        

## execute it if not imported
if __name__ == '__main__':
    trf = MergeHISTTransform()
    sys.exit(trf.exeSysArgs().exitCode())

## Example of command lines are:
## csc_MergeHIST_trf.py inputROOTFile=$file1,$file2 outputROOTFile=$file3


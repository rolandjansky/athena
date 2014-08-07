#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ =""" Generate Validation D3PDs """

import os
import subprocess

from PATJobTransforms.BaseOfBasicTrf import BaseOfBasicTrf
from PyJobTransformsCore.trf import Author

ListOfDefaultPositionalKeys=['maxEvents','inputESDFile','inputAODFile','preInclude','postInclude','preExec','postExec','--ignoreerrors','d3pdVal','outputNTUP_PHYSVALFile']

#List of root files created by the transform (for merging)
rootfiles= [ 'PhysVal_InDetPerf.root', 'PhysVal_BackTracking.root', 'PhysVal_MissingET.root', 'PhysVal_Jets.root', 'PhysVal_Tau.root', 'PhysVal_Electrons.root','PhysVal_MUONSPLACEHOLDER.root','PhysVal_Btag.root', 'PhysVal_SUSY.root', 'PhysVal_MonTop.root', 'PhysVal_Zee.root', 'PhysVal_Exotics.root', 'PhysVal_HSG6.root', 'PhysVal_Photons.root'] 

class ValidationD3PDJobTransform( BaseOfBasicTrf ):
    def __init__(self,inDic):
        BaseOfBasicTrf.__init__(self,inDic,
                                # Original author Steven Beale
                                authors = [ Author('Graeme Stewart','graeme.andrew.stewart@cern.ch') ],
                                skeleton='PATJobTransforms/skeleton.ValidateD3PD_trf.py' ,
                                help = __doc__ )
        
        # Add the postRunAction associated with the transform.
        self._addPostRunAction(self, prepend=True)

    def postRunAction(self):
        # Merge the individual perf NTUPs into one
        inlist = [ ]
        for file in rootfiles:
            if os.path.exists(file):
                inlist.append(file)
  
        if 'outputNTUP_PHYSVALFile' in self.inDic:
            cmd = ['hadd', self.inDic['outputNTUP_PHYSVALFile']]
        else:
            print 'WARNING: No outputNTUP_PHYSVALFile name given - falling back to "PhysVal.root"'
            cmd = ['hadd' , 'PhysVal.root'] 
        cmd.extend(inlist)

        print "Merging root files: '%s'" % cmd 

        proc = subprocess.Popen(args = cmd,bufsize = 1, shell = False, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
        while proc.poll() is None:
            line = proc.stdout.readline()
            if line:
                print line

        rc=proc.returncode

        if not rc==0:
            raise RuntimeError("hadd returned with value {0:d} instead of 0. Stopping!".format(rc))

        print "Merging finished"


if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = ValidationD3PDJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())

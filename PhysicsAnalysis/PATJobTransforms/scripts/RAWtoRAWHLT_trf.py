#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Read in raw data (byte stream), run HLT, and write out raw data with new HLT fragments."""

from PyJobTransformsCore.trf import JobTransform,Author
from PyJobTransformsCore.full_trfarg import SQLiteSupport
from RecJobTransforms.RecConfig import recConfig
from PATJobTransforms.BSutils import BSutils
import os

class RAWtoRAWHLTJobTransform( JobTransform ):
    def __init__(self,inDic,lastInChain=True):
        JobTransform.__init__(self,
                              authors=[Author('Simon George','S.George@rhul.ac.uk')],
                              skeleton='PATJobTransforms/skeleton.runHLT.py' ,
                              help = __doc__,lastInChain=lastInChain)

        if not isinstance(inDic,dict):
            raise TypeError("inDic has %s but should be a dictionary." %type(inDic))

        from PATJobTransforms.ConfigDicUtils import AutoConfigureFromDic
        self.inDic=inDic
        AutoConfigureFromDic(self,inDic)
        
        #add other options and features
        self.add( SQLiteSupport() )

    def doPreRunActions(self):
        """ Cleans the run directory from old BS file """
        outputbsfilearg  = self.getArgument("outputBSFile").value()
        os.path.walk(os.curdir, BSutils.match_deleteOld_BS_datafile, '(data_test|'+outputbsfilearg+').*\.data')
        # default prerun actions
        JobTransform.doPreRunActions(self)

    def doPostRunActions(self):
        """ Renames the BS ouput file to the name requested in the transform """
        outputbsfilearg  = self.getArgument("outputBSFile").value()
        print "RAWtoRAWHLT_trf.py: Try to rename outputBSFile"
        os.path.walk(os.curdir, BSutils.match_rename_BS_datafile, ['(data_test|'+outputbsfilearg+').*\.data',outputbsfilearg] )
        # finish with default post-run actions
        JobTransform.doPostRunActions(self)

# execute it if not imported
if __name__ == '__main__':
    #Special preparation for command-line
    import sys
    from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine
    inDic=BuildDicFromCommandLine(sys.argv)
    #Construct and execute the transform
    trf = RAWtoRAWHLTJobTransform(inDic)
    sys.exit(trf.exeArgDict(inDic).exitCode())


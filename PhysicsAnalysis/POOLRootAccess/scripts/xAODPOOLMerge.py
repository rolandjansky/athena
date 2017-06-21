#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ==============================================================================
# File: xAODPOOLMerge.py
# Author: Karsten Koeneke <karsten.koeneke@cern.ch>
# Description: This file is a python executable that is meant for merging on the grid
# ==============================================================================

import user  # look for .pythonrc.py for user init
import os,sys

#===================================
# The main function... everything is done here
#===================================
if __name__ == '__main__':
    # Check that we have more than two command line arguments
    # (argument 0 is the script name, argument on is -o)
    if len(sys.argv) <=3 :
        print 'xAODPOOLMerge.py     ERROR Only got three arguments given... at least four expected! Argument List:', str(sys.argv)
        print "xAODPOOLMerge.py     ERROR exiting..."
        exit(1)

    # Now, build the string of input files
    outFile = sys.argv[2]
    inFileString = ""
    for i in xrange(3,len(sys.argv)):
        inFile = sys.argv[i]
        inFileString += inFile+","
        pass
    inFileString = inFileString.rstrip(",")

    # Now, build the command to run
    runCommand = "athena.py -c "
    runCommand += "\"OUTFILE='"+outFile
    runCommand += "';INFILELIST='"
    runCommand += inFileString
    runCommand += "';\""
    runCommand += " POOLRootAccess/AODMerge.py "
    print "xAODPOOLMerge.py     INFO Going to run: ", runCommand
    exitCode = os.system( runCommand )
    print "xAODPOOLMerge.py     INFO Exit code from Athena process:", exitCode
    sys.exit(exitCode)

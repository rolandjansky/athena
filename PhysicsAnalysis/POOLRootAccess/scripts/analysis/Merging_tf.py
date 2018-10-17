#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# ==============================================================================
# File: Merging_tf.py
# Author: Will Buttinger (will@cern.ch)
# Description: This is a mimic of the Merging_tf.py script, but ships only in the AthAnalysis releases
# ==============================================================================

import user
import os,sys


#===================================
# The main function... everything is done here
#===================================
if __name__ == '__main__':
    #we only use the inputAODFile and outputAODFile arguments
    #so look for those in the args 

    inputFiles=""
    outputFile=""
    for i in xrange(1,len(sys.argv)):

        theArg = sys.argv[i].split("=",1)
        if theArg[0].strip()=="inputAODFile":
            inputFiles=theArg[1].strip()
        elif theArg[0].strip()=="outputAODFile":
            outputFile=theArg[1].strip()
        
    if inputFiles=="":
        print "ERROR ... inputAODFile not found in arguments"
        exit(1)
    
    if outputFile=="":
        print "ERROR ... outputAODFile not found in arguments"
        exit(1)

    # Now, build the command to run
    runCommand = "athena.py -c "
    runCommand += "\"OUTFILE='"+outputFile
    runCommand += "';INFILELIST='"
    runCommand += inputFiles
    runCommand += "';\""
    runCommand += " POOLRootAccess/AODMerge.py "
    print "Merging_tf.py     INFO Going to run: ", runCommand
    exitCode = os.system( runCommand )
    print "Merging_tf.py     INFO Exit code from Athena process:", exitCode
    sys.exit(exitCode)


    

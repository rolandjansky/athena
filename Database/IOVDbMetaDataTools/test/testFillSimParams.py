#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    testFillSimParams.py
# @purpose: script to test creating a local SQLite COOL db and fill folders with
#           simulation and/or digitization parameters 
# @author:  RD Schaffer <R.D.Schaffer@cern.ch>
# @date:    April 2008
#
#
__version__ = "$Id: testFillSimParams.py,v 1.1 2008-05-03 08:45:54 schaffer Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"


import sys,os
import string
os.environ['CLING_STANDARD_PCH'] = 'none' #See bug ROOT-10789
from PyCool import cool,coral
from IOVDbMetaDataTools import ParameterDbFiller

from optparse import OptionParser

def doParamTest(beginRun, endRun, simParams, digitParams, digitParams64):

    # create dbs for sim and digit parameters
    print "doParamTest: Running parameter test:"
    print "doParamTest: begin/end run", beginRun, endRun
    print "doParamTest: simParams    ", simParams
    print "doParamTest: digitParams  ", digitParams
    print "doParamTest: digitParams64", digitParams64

    if len(simParams) % 2 != 0:
        print "doParamTest: Must have name/value pairs for simulation \
        parameters. Found:", simParams
        str(parser.print_help() or "")
        sys.exit(1)

    if len(digitParams) % 2 != 0:
        print "doParamTest: Must have name/value pairs for digitization \
        parameters. Found:", digitParams
        str(parser.print_help() or "")
        sys.exit(1)
        
    if len(digitParams64) % 2 != 0:
        print "doParamTest: Must have name/value pairs for digitization 64 \
        parameters. Found:", digitParams64
        str(parser.print_help() or "")
        sys.exit(1)
        
    if len(simParams) ==0 and  len(digitParams) == 0:
        print "doParamTest: Must provide sim parameters and/or digit parameters"
        str(parser.print_help() or "")
        sys.exit(1)

    #sys.exit(1)

    hasSimParams     = len(simParams) > 0
    hasDigitParams   = len(digitParams) > 0
    hasDigitParams64 = len(digitParams64) > 0

    dbFiller = ParameterDbFiller.ParameterDbFiller()

    # create db for simulation parameters
    if hasSimParams:
        # set iov
        dbFiller.setBeginRun(beginRun)
        dbFiller.setEndRun(endRun)
        # set parameters
        for i in range(len(simParams)/2):
            dbFiller.addSimParam(simParams[2*i], simParams[2*i+1])
        # generate db
        dbFiller.genSimDb()
        print "doParamTest: created simulation db"
        
    # create db for digitization parameters
    if hasDigitParams:
        # set iov
        dbFiller.setBeginRun(beginRun)
        dbFiller.setEndRun(endRun)
        # set parameters
        for i in range(len(digitParams)/2):
            dbFiller.addDigitParam(digitParams[2*i], digitParams[2*i+1])
            pass
        if hasDigitParams64:
            # set parameters
            for i in range(len(digitParams64)/2):
                dbFiller.addDigitParam64(digitParams64[2*i], digitParams64[2*i+1])
                pass
            pass
        # generate db
        dbFiller.genDigitDb()
        print "doParamTest: created digitization db"


if __name__ == "__main__":

    # First test - only simulation:
    beginRun      = 1234
    endRun        = 4567
    simParams     = []
    digitParams   = []
    digitParams64 = []
    simParams  += ['k_a']
    simParams  += ['v_a']
    simParams  += ['k_b']
    simParams  += ['v_b']
    simParams  += ['k_c']
    simParams  += ['v_c1 v_c1 v_c1']
    print "*** Test 1 ***"
    doParamTest(beginRun, endRun, simParams, digitParams, digitParams64)

    # First test - add on digitization:
    digitParams  += ['k_d_a']
    digitParams  += ['v_d_a']
    digitParams  += ['k_d_b']
    digitParams  += ['v_d_b']
    digitParams  += ['k_d_c']
    digitParams  += ['v_d_c1 v_d_c1 v_d_c1']

    digitParams64  += ['k_d_e']
    digitParams64  += ['v_d_e2 v_d_e2 v_d_e2 v_d_e2']

    print "*** Test 2 ***"
    doParamTest(beginRun, endRun, simParams, digitParams, digitParams64)

    


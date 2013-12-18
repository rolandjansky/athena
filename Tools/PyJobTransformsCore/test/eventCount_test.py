#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys

testData = os.getenv('TESTDATA')
if not testData:
    print "ERROR: TESTDATA not set"
    sys.exit(10)

inputList = [os.path.join(testData,'DC3.007218.singlepart_mu20_ATLAS-DC3-07.aod.pool.root')]
#inputList = [os.path.join(testData,'DC3.007218.singlepart_mu20_ATLAS-DC3-07.rdo.pool.root')]

from PyJobTransformsCore.basic_trfarg import InputDataFileArg,PoolDataFile
arg = InputDataFileArg("count events",PoolDataFile())
arg.setLoggerLevel('ALL')
arg.setValue( inputList )
arg.getGUID()
arg.eventCount()

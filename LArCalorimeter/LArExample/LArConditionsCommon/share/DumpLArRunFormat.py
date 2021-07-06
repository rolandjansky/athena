#!/bin/env python

import sys
if len(sys.argv)!=2:
        print("Syntax",sys.argv[0],'<run>')
        sys.exit(-1)
run=int(sys.argv[1])

from LArConditionsCommon.LArRunFormat import *

myformat=getLArFormatForRun(run)

if (myformat is not None):
      print(" LAr run configuration: Nsamples:%d  GainType:%d  Latency:%d  FirstSample:%d  Format:%s  runType:%s" % (myformat.nSamples(),myformat.gainType(),myformat.latency(),myformat.firstSample(),myformat.stringFormat(),myformat.stringRunType()))
else:
      print(" LAr run infomation not available")

#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.analysis
import os
import sys

if len(sys.argv) != 2:
    print 'Use like this:'
    print 'python %s %s' % (sys.argv[0], '410000')
    print sys.exit(1)

id = 0
try:
    id = int(sys.argv[1])
except:
    print sys.argv[1], 'is not a number'
    print 'Use like this:'
    print 'python %s %s' % (sys.argv[0], '410000')
    sys.exit()

filename = 'XSection-MC15-13TeV.data'
tdp = TopExamples.analysis.TopDataPreparation(os.getenv('ROOTCOREBIN') + '/data/TopDataPreparation/' + filename)

print 'Using', tdp.getFilename()
print ''

if tdp.hasID(id):
    print 'ID: %s' % id
    print '  Cross-section: %f (%f * %f)' % (tdp.getXsection(id), tdp.getRawXsection(id), tdp.getKfactor(id))
    print '  Shower       : %s' % tdp.getShower(id)
else:
    print 'ID: %s not available' % id

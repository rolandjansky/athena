#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.analysis
import TopExamples.grid
import TopExamples.logger as logger
import DerivationTags
import Data_rel21
import MC16_TOPQ1

inputDirectory = '/tmp/YOURUSERNAME/DownloadFolder/'
runDirectory   = 'test1/'
analysis       = 'Dilepton'

print '\n\n'
print logger.OKGREEN + 'MC simulation' + logger.ENDC
names = [
'TOPQ1_ttbar_PowPy8',
]

samples = TopExamples.grid.Samples(names)
TopExamples.analysis.check(analysis, inputDirectory, runDirectory, samples)

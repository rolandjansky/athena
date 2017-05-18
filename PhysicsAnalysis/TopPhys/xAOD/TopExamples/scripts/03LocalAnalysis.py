#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import TopExamples.analysis
import TopExamples.grid
import TopExamples.logger as logger
import DerivationTags
import Data
import MC15c_TOPQ1

inputDirectory = '/ptmp/mpp/knand/TopAnalysis_13TeV/Downloads_2.4.24/Downloads/'
runDirectory   = 'test1/'
analysis       = 'Dilepton'

#For data
print logger.OKGREEN + 'Data' + logger.ENDC
names = ['Data15_TOPQ1', 'Data16_TOPQ1']
samples = TopExamples.grid.Samples(names)
TopExamples.analysis.check(analysis, inputDirectory, runDirectory, samples)

print '\n\n'
print logger.OKGREEN + 'MC simulation' + logger.ENDC
names = [
    'TOPQ1_ttbar_PowPy6',
    'TOPQ1_singleTop_PowPy6',                                                                                                                                                           
    'TOPQ1_ttV',                                                                                                                                                                        
    'TOPQ1_Diboson_Sherpa',                                                                                                                                                             
    'TOPQ1_Wjets_Sherpa22',                                                                                                                                                             
    'TOPQ1_Zjets_Sherpa22',  
]

samples = TopExamples.grid.Samples(names)
TopExamples.analysis.check(analysis, inputDirectory, runDirectory, samples)

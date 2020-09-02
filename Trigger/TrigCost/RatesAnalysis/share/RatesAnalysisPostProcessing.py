#!/usr/bin/env python
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file RatesPostProcessing.py
@author T. Martin
@date 2020-02-04
@brief Script to consume merged rates histograms from the RatesAnalysis package and produce structured CSV, JSON output.
'''

import ROOT
from RatesAnalysis.Util import getMetadata, populateTriggers, getGlobalGroup, toJson
from AthenaCommon.Logging import logging

def main():
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--file', default='RatesHistograms.root', 
                      help='Input ROOT file to generate output from, run hadd first if you have more than one')
  parser.add_argument('--outputTag', default='LOCAL', 
                      help='Tag identifying this processing to be used in the output folder name (any underscores will be removed)')
  parser.add_argument('--outputJSONFile', default='rates.json', 
                      help='JSON file of rates for use with the RuleBook')
  parser.add_argument('--userDetails',
                      help='User supplied metadata string giving any extra details about this run.')                      
  args = parser.parse_args()
  log = logging.getLogger('RatesPostProcessing')

  inputFile = ROOT.TFile(args.file, 'READ')

  metadata = getMetadata(inputFile)
  normHist = inputFile.Get('normalisation')

  if normHist is None or metadata is None:
    log.error('Cannot locate normHist, or metadata in top level of ntuple.')
    exit()

  metadata['normalisation'] = normHist.GetBinContent(1)
  metadata['n_evts'] = normHist.GetBinContent(2)

  HLTGlobalGroup = getGlobalGroup(inputFile, 'Main')
  L1GlobalGroup = getGlobalGroup(inputFile, 'L1')

  L1Triggers = populateTriggers(inputFile, metadata, L1GlobalGroup, 'L1_')
  HLTTriggers = populateTriggers(inputFile, metadata, HLTGlobalGroup, 'HLT_')

  log.info("Exporting " + args.outputJSONFile)
  toJson(args.outputJSONFile, metadata, L1Triggers, HLTTriggers)
  
if __name__== "__main__":
  main()

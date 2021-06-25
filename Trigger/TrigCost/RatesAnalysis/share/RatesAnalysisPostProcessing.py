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
from RatesAnalysis.Util import getMetadata, populateTriggers, getGlobalGroup, toJson, toCSV
from AthenaCommon.Logging import logging


def getTableName(name):
  tabName = "Table_Rate_"
  if name == "HLT" or name == "L1":
    tabName += "Chain" + name
  else:
    tabName += name

  tabName += "_HLT_All.csv"

  return tabName 

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
  metadata['details'] = args.userDetails

  HLTGlobalGroup = getGlobalGroup(inputFile, 'RATE_GLOBAL_HLT')
  L1GlobalGroup = getGlobalGroup(inputFile, 'RATE_GLOBAL_L1')

  L1Triggers = populateTriggers(inputFile, metadata, L1GlobalGroup, 'ChainL1')
  HLTTriggers = populateTriggers(inputFile, metadata, HLTGlobalGroup, 'ChainHLT')
  AllGlobalGroups = populateTriggers(inputFile, metadata, HLTGlobalGroup, 'Group')

  L1Table = getTableName("L1")
  HLTTable = getTableName("HLT")
  GroupTable = getTableName("Group")

  log.info("Exporting " + args.outputJSONFile)
  toJson(args.outputJSONFile, metadata, L1Triggers, HLTTriggers)
  log.info("Exporting " + HLTTable)
  toCSV(HLTTable, metadata, HLTTriggers)
  log.info("Exporting " + L1Table)
  toCSV(L1Table, metadata, L1Triggers)
  log.info("Exporting " + GroupTable)
  toCSV(GroupTable, metadata, AllGlobalGroups)
  
if __name__== "__main__":
  main()

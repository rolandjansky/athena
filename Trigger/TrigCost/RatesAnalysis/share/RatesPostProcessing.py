#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file RatesPostProcessing.py
@author T. Martin
@date 2020-02-04
@brief Script to consume merged rates histograms from the RatesAnalysis package and produce structured CSV, JSON output.
'''

import ROOT
import json
from enum import Enum

class RatesBins(Enum):
  RATE_BIN_OR = 1
  RATE_BIN_AND = 2
  EXPRESS_BIN = 3
  UNIQUE_BIN = 4

class RatesTrigger:
  def __init__(self, name, data, denominator, masterGroup):
    self.name = name
    self.rateNumerator    = data.GetBinContent(RatesBins.RATE_BIN_OR.value)
    self.rateNumeratorErr = data.GetBinError(RatesBins.RATE_BIN_OR.value)

    self.expressNumerator    = data.GetBinContent(RatesBins.EXPRESS_BIN.value)
    self.expressNumeratorErr = data.GetBinError(RatesBins.EXPRESS_BIN.value)

    self.uniqueNumerator    = data.GetBinContent(RatesBins.UNIQUE_BIN.value)
    self.uniqueNumeratorErr = data.GetBinError(RatesBins.UNIQUE_BIN.value)

    self.uniqueMasterNumerator    = masterGroup.GetBinContent(RatesBins.RATE_BIN_OR.value)
    self.uniqueMasterNumeratorErr = masterGroup.GetBinError(RatesBins.RATE_BIN_OR.value)

    self.rateDenominator = denominator

    self.rate    = self.rateNumerator / self.rateDenominator
    self.rateErr = self.rateNumeratorErr / self.rateDenominator

    self.rateExpress    = self.expressNumerator / self.rateDenominator
    self.rateExpressErr = self.expressNumeratorErr / self.rateDenominator

    self.rateUnique    = (self.uniqueMasterNumerator - self.uniqueNumerator) / self.rateDenominator
    self.rateUniqueErr = 0 if self.rateNumerator == 0 else (self.rateNumeratorErr / self.rateNumerator) * self.rateUnique 

  def export(self, jsonDict):
    myDict = {}
    myDict['express_stream'] = self.rateExpress
    myDict['chain_prescale'] = 1
    myDict['lower_chain_name'] = ''
    myDict['evts_passed'] = 0
    myDict['evts_passed_weighted'] = 0
    myDict['efficiency'] = 0
    myDict['efficiency_err'] = 0
    myDict['prescaled_efficiency'] = 0
    myDict['prescaled_efficiency_err'] = 0
    myDict['rate'] = self.rate
    myDict['rate_err'] = self.rateErr
    myDict['unique_fraction'] = 0
    myDict['unique_rate'] = self.rateUnique  
    jsonDict[self.name] = myDict

def toJson(fileName, L1Triggers, HLTTriggers):
  l1 = {}
  for trig in L1Triggers:
    trig.export(l1)
  hlt = {}
  for trig in HLTTriggers:
    trig.export(hlt)

  level = {}
  level['L1'] = l1
  level['HLT'] = hlt

  jsonDict = {}
  jsonDict['Luminosity'] = 2e34
  jsonDict['PredictionLumi'] = 2e34
  jsonDict['n_evts'] = 0
  jsonDict['level'] = level

  with open(fileName, 'w') as outFile:
    json.dump(obj=jsonDict, fp=outFile, indent=2, sort_keys=True)


def main():
  import sys
  from argparse import ArgumentParser
  parser = ArgumentParser()
  parser.add_argument('--file', default='RatesHistograms.root', 
                      help='Input histogram ntuple to generate from, run hadd first if you have more than one')
  parser.add_argument('--outputTag', default='LOCAL', 
                      help='Tag identifying this processing to be used in the output folder name (any underscores will be removed)')
  parser.add_argument('--userDetails',
                      help='User supplied metadata string giving any extra details about this run.')                      
  args = parser.parse_args()

  inputHistogramFile = ROOT.TFile(args.file, 'READ')

  normHist = None
  bcidHist = None
  HLTGlobalGroup = None
  HLTExpressGroup = None
  L1GlobalGroup = None
  for key in inputHistogramFile.GetListOfKeys():
    if key.GetName() == 'normalisation':
      normHist = key.ReadObj()
    elif key.GetName() == 'bcid':
      bcidHist = key.ReadObj()
    elif key.GetName() == 'Globals':
      for globalsKey in key.ReadObj().GetListOfKeys():
        for hist in globalsKey.ReadObj().GetListOfKeys():
          if hist.GetName() == 'data':
            if globalsKey.GetName() == 'Main':
              HLTGlobalGroup = hist.ReadObj()
            elif globalsKey.GetName() == 'Express':
              HLTExpressGroup = hist.ReadObj()
            elif globalsKey.GetName() == 'L1':
              L1GlobalGroup = hist.ReadObj()
            break

  if normHist is None or bcidHist is None:
    print('Error. Cannot locate normHist or bcidHist in top level of ntuple.')
    exit()

  normalisation = normHist.GetBinContent(1)

  L1Triggers = []
  HLTTriggers = []
  Groups = []

  for key in inputHistogramFile.GetListOfKeys():
    if key.GetName() == 'Triggers':
      for triggerKey in key.ReadObj().GetListOfKeys():
        data = None
        for hist in triggerKey.ReadObj().GetListOfKeys():
          if hist.GetName() == 'data':
            data = hist.ReadObj()
            break
        if triggerKey.GetName().startswith('L1_'):
          trig = RatesTrigger(triggerKey.GetName(), data, normalisation, L1GlobalGroup)
          L1Triggers.append( trig )
        elif triggerKey.GetName().startswith('HLT_'):
          trig = RatesTrigger(triggerKey.GetName(), data, normalisation, HLTGlobalGroup)
          HLTTriggers.append( trig )
        else:
          print("Unknown trigger level for " + triggerKey.GetName())

  toJson('rates.json', L1Triggers, HLTTriggers)
  
if __name__== "__main__":
  main()

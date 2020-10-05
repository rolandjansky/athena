#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file RatesTrigger.py
@brief Accumulator class to buffer data for a single trigger and export this to JSON or CSV
'''

import math
from enum import Enum

class RatesBins(Enum):
  ACTIVE_RAW_BIN  = 1
  ACTIVE_WEIGHTED_BIN = 2
  PASS_RAW_BIN = 3
  PASS_WEIGHTED_OR_BIN = 4
  PASS_WEIGHTED_AND_BIN = 5
  EXPRESS_BIN = 6
  UNIQUE_BIN = 7

class RatesTrigger:
  def __init__(self, name, metadata, data, masterGroup):
    self.name = name
    # Total weighted events passing the trigger
    self.passWeighted    = data.GetBinContent(RatesBins.PASS_WEIGHTED_OR_BIN.value)
    self.passWeightedErr = data.GetBinError(RatesBins.PASS_WEIGHTED_OR_BIN.value)

    # Total un-weighted events passing the trigger
    self.passRaw = data.GetBinContent(RatesBins.PASS_RAW_BIN.value)

    # Total weighted events passing the trigger, using the express prescale
    self.passExpressWeighted    = data.GetBinContent(RatesBins.EXPRESS_BIN.value)
    self.passExpressWeightedErr = data.GetBinError(RatesBins.EXPRESS_BIN.value)

    # Total weighted events passing a full hypothetical menu which *excluded* this trigger
    self.passUniqueWeighted    = data.GetBinContent(RatesBins.UNIQUE_BIN.value)
    self.passUniqueWeightedErr = data.GetBinError(RatesBins.UNIQUE_BIN.value)

    # Total weighted events passing the full menu (full L1 for L1 chains, full HLT for HLT chains)
    self.passMasterWeighted = masterGroup.GetBinContent(RatesBins.PASS_WEIGHTED_OR_BIN.value)

    # Total un-weighted number of events in which the trigger was active (always active if L1) 
    self.activeRaw = data.GetBinContent(RatesBins.ACTIVE_RAW_BIN.value)

    # Total weighted number of events in which the trigger was active
    self.activeWeighted    = data.GetBinContent(RatesBins.ACTIVE_WEIGHTED_BIN.value)
    self.activeWeightedErr = data.GetBinError(RatesBins.ACTIVE_WEIGHTED_BIN.value)

    # Wall-time in seconds. No error on this.
    self.rateDenominator = metadata['normalisation']

    # Trigger's rate is total weighted passing events normalised to wall-time
    self.rate    = self.passWeighted / self.rateDenominator
    self.rateErr = self.passWeightedErr / self.rateDenominator

    # Trigger's efficiency is the fraction of active events which are passed. Error propagated.
    self.efficiency    = self.passWeighted / self.activeWeighted if self.activeWeighted > 0.0 else 0.0
    self.efficiencyErr = 0
    if self.passWeighted != 0 and self.activeWeighted != 0:
      fracErr = math.sqrt( math.pow(self.passWeightedErr/self.passWeighted,2) + math.pow(self.activeWeightedErr/self.activeWeighted,2) )
      self.efficiencyErr = self.efficiency * fracErr

    # Rate using express prescales
    self.rateExpress    = self.passExpressWeighted / self.rateDenominator
    self.rateExpressErr = self.passExpressWeightedErr / self.rateDenominator

    # Unique rate requires the subtraction of the (all minus this trigger) total from the (all triggers) total
    # The error is taken as a fractional error on the main rate calc
    self.rateUnique    = (self.passMasterWeighted - self.passUniqueWeighted) / self.rateDenominator
    self.rateUniqueErr = 0 
    if self.passWeighted != 0:
      self.rateUniqueErr = (self.passWeightedErr / self.passWeighted) * self.rateUnique 

    # The total rate of all triggers
    self.masterRate = self.passMasterWeighted / self.rateDenominator

    # What fraction of the total rate is unique to this trigger
    self.uniqueFraction = 0 if self.masterRate == 0 else self.rateUnique / self.masterRate 

    # Some menu-derived metadata
    prescales = metadata['prescales']
    lowers = metadata['lowers']
    self.prescale = prescales[name]
    self.lower = lowers[name]

  def export(self, exportDict):
    myDict = {}
    myDict['express_stream'] = self.rateExpress
    myDict['chain_prescale'] = self.prescale
    myDict['lower_chain_name'] = self.lower
    myDict['evts_passed'] = self.passRaw
    myDict['evts_passed_weighted'] = self.passWeighted
    myDict['efficiency'] = self.efficiency
    myDict['efficiency_err'] = self.efficiencyErr
    myDict['rate'] = self.rate
    myDict['rate_err'] = self.rateErr
    myDict['unique_fraction'] = self.uniqueFraction
    myDict['unique_rate'] = self.rateUnique  
    exportDict[self.name] = myDict

#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#

'''
@file Util.py
@brief Utility functions used by RatesPostProcessing
'''

def toCSV(fileName, HLTTriggers):
  import csv
  
  with open(fileName, mode='w') as outputCSV_file:
    rates_csv_writer = csv.writer(outputCSV_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

    rates_csv_writer.writerow(['Name','Active Time [s]','Group','Weighted PS Rate [Hz]','Weighted PS Rate Err [Hz]','Prescale','ID','Raw Active Events','Raw Pass Events','Active Events','Input Rate [Hz]','Pass Fraction before PS [%]','Pass Fraction after PS [%]','Pass Weighted PS'])
    rates_csv_writer.writerow(['Integrated length of all lumi blocks which contributed events to this rates prediction.','The group this chain belongs to.','Rate after applying all prescale(s) as weights.','Error on rate after applying all prescale(s) as weights','The prescale of this chain. Only displayed for simple combinations.','The CPTID or HLT Chain ID','Raw underlying statistics on the number events processed for this chain.','Raw underlying statistics on the number events passed by this chain.','Number of events in which the chain - or at least one chain in the combination - was executed.','Input rate to this chain or combination of chains. At L1 this will be the collision frequency for the bunch pattern.','Fraction of events which pass this trigger before prescale.','Fraction of events which pass this trigger after prescale.','Number of events this chain or combination passed after applying prescales as weighting factors.'])

    for trig in HLTTriggers:
      rates_csv_writer.writerow([trig.name,'1','',trig.rate,trig.rateErr,trig.prescale,'ID',trig.activeRaw,trig.rate,'0','0',trig.passWeighted*trig.prescale])
    
    

def toJson(fileName, metadata, L1Triggers, HLTTriggers):
  import json
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
  jsonDict['PredictionLumi'] = metadata['targetLumi']
  jsonDict['n_evts'] = metadata['n_evts']
  jsonDict['AtlasProject'] = metadata['AtlasProject']
  jsonDict['AtlasVersion'] = metadata['AtlasVersion']
  jsonDict['triggerMenuSetup'] = metadata['masterKey']
  jsonDict['L1PrescaleSet'] = metadata['lvl1PrescaleKey']
  jsonDict['HLTPrescaleSet'] = metadata['hltPrescaleKey']
  jsonDict['bunchgroup'] = metadata['bunchGroups']
  jsonDict['level'] = level

  with open(fileName, 'w') as outFile:
    json.dump(obj=jsonDict, fp=outFile, indent=2, sort_keys=True)


def getMetadata(inputFile):
  metatree = inputFile.Get("metadata")
  if metatree is None:
    return None

  metatree.GetEntry(0)
  metadata = {}

  metadata['targetMu'] = metatree.targetMu
  metadata['targetBunches'] = metatree.targetBunches
  metadata['targetLumi'] = metatree.targetLumi

  metadata['masterKey'] = metatree.masterKey
  metadata['lvl1PrescaleKey'] = metatree.lvl1PrescaleKey
  metadata['hltPrescaleKey'] = metatree.hltPrescaleKey

  metadata['AtlasProject'] = str(metatree.AtlasProject)
  metadata['AtlasVersion'] = str(metatree.AtlasVersion)

  prescales = {}
  lowers = {}
  for i in range(0, metatree.triggers.size()):
    prescales[metatree.triggers.at(i)] = metatree.prescales.at(i)
    lowers[metatree.triggers.at(i)] = str(metatree.lowers.at(i))
  metadata['prescales'] = prescales
  metadata['lowers'] = lowers
  
  bunchGroups = []
  for bg in metatree.bunchGroups:
    bunchGroups.append(bg)
  metadata['bunchGroups'] = bunchGroups

  return metadata


def populateTriggers(inputFile, metadata, globalGroup, filter):
  from .RatesTrigger import RatesTrigger
  triggerList = []
  for key in inputFile.GetListOfKeys():
    if key.GetName() == 'Triggers':
      for triggerKey in key.ReadObj().GetListOfKeys():
        if triggerKey.GetName().startswith(filter):
          for hist in triggerKey.ReadObj().GetListOfKeys():
            if hist.GetName() == 'data':
              triggerList.append( RatesTrigger(triggerKey.GetName(), metadata, hist.ReadObj(), globalGroup) )
  return triggerList


def getGlobalGroup(inputFile, filter):
  for key in inputFile.GetListOfKeys():
    if key.GetName() == 'Globals':
      for globalsKey in key.ReadObj().GetListOfKeys():
        if globalsKey.GetName() == filter:
          for hist in globalsKey.ReadObj().GetListOfKeys():
            if hist.GetName() == 'data':
              return hist.ReadObj()

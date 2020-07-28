#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#

'''
@file Util.py
@brief Utility functions used by RatesPostProcessing
'''

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

#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#

'''
@file Util.py
@brief Utility functions used by RatesPostProcessing
'''

def toCSV(fileName, metadata, HLTTriggers, readL1=False):
  import csv

  with open(fileName, mode='w') as outputCSV_file:
    rates_csv_writer = csv.writer(outputCSV_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

    rates_csv_writer.writerow(['Name','Active Time [s]','Group','Weighted PS Rate [Hz]','Weighted PS Rate Err [Hz]', \
                                      'Unique Rate [Hz]','Unique Rate Err [Hz]','Express Rate [Hz]','Express Rate Err [Hz]','Prescale','Express Prescale','ID', \
                                      'Raw Active Events','Raw Pass Events','Active Events','Input Rate [Hz]','Pass Fraction after PS [%]','Pass Weighted PS'])
    rates_csv_writer.writerow(['Trigger name','Integrated length of all lumi blocks which contributed events to this rates prediction.','The group this chain belongs to.','Rate after applying all prescale(s) as weights.','Error on rate after applying all prescale(s) as weights','Total rate without this chain rate','Error on unique rate','Express stream rate','Error on express rate','The prescale of this chain. Only displayed for simple combinations.','The prescale of the chain including express prescale','The CPTID or HLT Chain ID','Raw underlying statistics on the number events processed for this chain.','Raw underlying statistics on the number events passed by this chain.','Number of events in which the chain - or at least one chain in the combination - was executed.','Input rate to this chain or combination of chains. At L1 this will be the collision frequency for the bunch pattern.','Fraction of events which pass this trigger after prescale.','Number of events this chain or combination passed after applying prescales as weighting factors.'])

    for trig in HLTTriggers:

      group_name = chain_id = ""
      if "ChainL1" in fileName:
        group_name = "None"
        chain_id = metadata["itemID"][trig.name]
      elif "ChainHLT" in fileName:
        group_name = metadata["chainGroup"][trig.name]
        chain_id = metadata["chainID"][trig.name]
      elif "Group" in fileName:
        chain_id = 0
        group_name = "All" if "GLOBAL" in trig.name else group_name

      if float(trig.rateDenominator)==0:
        print("float(trig.rateDenominator) is ZERO! This shouldn't happen")
      if float(trig.activeWeighted)==0:
        passFrac_afterPS=0
      else:
        passFrac_afterPS=100*float(trig.passWeighted)/float(trig.activeWeighted)

      rates_csv_writer.writerow([trig.name,"%.4f" % trig.rateDenominator,group_name,"%.4f" % trig.rate,"%.4f" % trig.rateErr, \
          "%.4f" % trig.rateUnique,"%.4f" % trig.rateUniqueErr, "%.4f" % trig.rateExpress,"%.4f" % trig.rateExpressErr, trig.prescale, trig.expressPrescale, chain_id, \
          "%.0f" % trig.activeRaw,"%.0f" % trig.passRaw,"%.4f" % trig.activeWeighted,"%.4f" % (float(trig.activeWeighted)/float(trig.rateDenominator)),"%.4f" % passFrac_afterPS,"%.4f" % trig.passWeighted])
      
    

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


  metajsonData = [
    {'PredictionLumi' : metadata['targetLumi']},
    {'TargetMu' : metadata['targetMu']},
    {'RunNumber' : metadata['runNumber']},
    {'NEvents' : metadata['n_evts']},
    {'Details' : metadata['details']}
  ]

  metajsonDict = {}
  metajsonDict['text'] = 'metadata'
  metajsonDict['children'] = metajsonData
  
  with open('metadata.json', 'w') as outMetaFile:
    json.dump(obj=metajsonDict, fp=outMetaFile, indent=2, sort_keys=True)


def getMetadata(inputFile):
  metatree = inputFile.Get("metadata")
  if metatree is None:
    return None

  metatree.GetEntry(0)
  metadata = {}

  metadata['runNumber'] = metatree.runNumber

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
  express = {}
  for i in range(0, metatree.triggers.size()):
    prescale = metatree.prescales.at(i)
    expressPrescale = metatree.express.at(i)
    # Handle group prescale values
    prescales[metatree.triggers.at(i)] = prescale if prescale > -1 else "Multiple"
    lowers[metatree.triggers.at(i)] = str(metatree.lowers.at(i))
    express[metatree.triggers.at(i)] = expressPrescale if expressPrescale > -1 else "Multiple"

  metadata['prescales'] = prescales
  metadata['lowers'] = lowers
  metadata['express'] = express
  
  chainid = {}
  chaingroup = {}
  for i in range(0, metatree.hltChainIDGroup.size()):
    chainid[metatree.hltChainIDGroup.at(i).at(0)] = metatree.hltChainIDGroup.at(i).at(1)
    chaingroup[metatree.hltChainIDGroup.at(i).at(0)] = metatree.hltChainIDGroup.at(i).at(2)

  metadata['chainID'] = chainid
  metadata['chainGroup'] = chaingroup

  itemid = {}
  for i in range(0, metatree.l1ItemID.size()):
    itemid[metatree.l1ItemID.at(i).at(0)] = metatree.l1ItemID.at(i).at(1)

  metadata['itemID'] = itemid

  bunchGroups = []
  for bg in metatree.bunchGroups:
    bunchGroups.append(bg)
  metadata['bunchGroups'] = bunchGroups

  return metadata


def populateTriggers(inputFile, metadata, globalGroup, filter):
  # Fix groups' names that are also not GLOBAL
  def getTriggerName(name, filter):
    if "Group" in filter and "GLOBAL" not in name:
      return name.replace('_', ':', 1)
    else:
      return name

  from .RatesTrigger import RatesTrigger
  triggerList = []
  for key in inputFile.GetListOfKeys():
    if key.GetName() == 'All':
      for subdirKey in key.ReadObj().GetListOfKeys():
        if filter not in subdirKey.GetName(): continue
        for triggerKey in subdirKey.ReadObj().GetListOfKeys():
            for hist in triggerKey.ReadObj().GetListOfKeys():
              if hist.GetName() == 'data':
                triggerList.append( RatesTrigger(getTriggerName(triggerKey.GetName(), filter), metadata, hist.ReadObj(), globalGroup) )
  return triggerList


def getGlobalGroup(inputFile, filter):
  for key in inputFile.GetListOfKeys():
    if key.GetName() == 'All':
      for subdirKey in key.ReadObj().GetListOfKeys():
        if not subdirKey.GetName() == "Rate_Group_HLT" : pass
        for globalsKey in subdirKey.ReadObj().GetListOfKeys():
          if filter in globalsKey.GetName():
            for hist in globalsKey.ReadObj().GetListOfKeys():
              if hist.GetName() == 'data':
                return hist.ReadObj()

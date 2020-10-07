# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import operator
import json 
def addLHE3Weights(seq, pref = '', var_dict = {}):
  
  from AthenaCommon.AppMgr import ToolSvc
  from ReweightUtils.ReweightUtilsConf import * 

  sumOfWeightsAlg = SumOfWeightsAlg(name = pref+"LHE3SumWeightsAlg")

  listTools = [] 
  mcWeight = []
  for var,index in sorted(var_dict.items(), key=operator.itemgetter(1)):
    mcWeight.append( McEventWeight(name = pref+"LHE3Weight_"+var.replace('.','').replace(' ','').replace('/',''), McEventWeightIndex = index, UseTruthEvents = True))

    ToolSvc += mcWeight[-1]
    
    listTools.append(mcWeight[-1])
    
  sumOfWeightsAlg.WeightTools = listTools
  seq += sumOfWeightsAlg


from DerivationFrameworkCore.DerivationFrameworkMaster import *

# skip this in datfrom AthenaCommon.GlobalFlags import globalflags
if DerivationFrameworkHasTruth:

  mcweight_dict_orig = dict()
  mcweight_dict = dict()
  try:
    mcweight_dict_orig = inputFileSummary['metadata']['/Generation/Parameters']['HepMCWeightNames']
  except:
    print 'Could not retrieve HepMCWeightNames /Generation/Parameters metadata.'
  
  # Recent versions of MadGraph are writing the dictionary as a flat string
  # The next few lines ensure the string is reformatted as a dictionary
  # before it gets to addLHE3Weights
  if type(mcweight_dict_orig) is str:
    # json.loads requires double quotes around the key
    mcweight_dict_orig = mcweight_dict_orig.replace('\n', '').replace('\'','\"')
    mcweight_dict_orig = json.loads(mcweight_dict_orig)
    # The "keys" in MG are long input cards and not suitable for storage.
    # So we just store the index in str form...
    for key in mcweight_dict_orig.keys():
      val = mcweight_dict_orig[key]
      newkey = str(val)
      mcweight_dict[newkey] = val
  else:
    mcweight_dict = mcweight_dict_orig
        
  addLHE3Weights(DerivationFrameworkJob, "", mcweight_dict)


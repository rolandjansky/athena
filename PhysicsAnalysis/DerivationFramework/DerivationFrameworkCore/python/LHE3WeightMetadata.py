# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import operator

def addLHE3Weights(seq, pref = '', var_dict = {}):
  
  from AthenaCommon.AppMgr import ToolSvc
  from ReweightUtils.ReweightUtilsConf import * 

  sumOfWeightsAlg = SumOfWeightsAlg(name = pref+"LHE3SumWeightsAlg")

  listTools = [] 
  mcWeight = []
  for var,index in sorted(var_dict.items(), key=operator.itemgetter(1)):
    mcWeight.append( McEventWeight(name = pref+"LHE3Weight_"+var.replace('.','').replace(' ',''), McEventWeightIndex = index, UseTruthEvents = True))

    ToolSvc += mcWeight[-1]
    
    listTools.append(mcWeight[-1])
    
  sumOfWeightsAlg.WeightTools = listTools
  seq += sumOfWeightsAlg


from DerivationFrameworkCore.DerivationFrameworkMaster import *

# skip this in datfrom AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource() == 'geant4':

  mcweight_dict = dict()
  try:
    mcweight_dict = inputFileSummary['metadata']['/Generation/Parameters']['HepMCWeightNames']
  except:
    print 'Could not retrieve HepMCWeightNames /Generation/Parameters metadata.'
    
  addLHE3Weights(DerivationFrameworkJob, "", mcweight_dict)


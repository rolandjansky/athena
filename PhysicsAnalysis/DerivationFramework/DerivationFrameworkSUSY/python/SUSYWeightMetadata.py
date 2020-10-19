# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def addSUSYWeights(seq, pref = ""):

  from AthenaCommon.AppMgr import ToolSvc
  from ReweightUtils.ReweightUtilsConf import SumOfWeightsAlg,McEventWeight
  from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import SUSYIDWeight

  #Load standard generator event weight
  #mcEventWeightNom = McEventWeight(name = pref+"mcWNom")
  #ToolSvc += mcEventWeightNom

  sumOfWeightsAlg = SumOfWeightsAlg(name = pref+"SUSYSumWeightsAlg")

  #Load all potential SUSY process IDs
  # following Prospino conventions
  # https://twiki.cern.ch/twiki/bin/view/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs 
  listTools = [] 
  susyWeight = []
  for i in range(0, 225):

    if i==0: #flat sum of all processes (i.e. sum the weight no matter what)
      susyWeight.append( McEventWeight(name = pref+"SUSYWeight_ID"+"_"+str(i), UseTruthEvents = True))
    else: #add weight only to keeper associated to current process id
      susyWeight.append( SUSYIDWeight(name = pref+"SUSYWeight_ID"+"_"+str(i), SUSYProcID = i, UseTruthEvents = True))

    ToolSvc += susyWeight[i]
    
    listTools.append(susyWeight[i])
    
  sumOfWeightsAlg.WeightTools = listTools
  seq += sumOfWeightsAlg

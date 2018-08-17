# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################################################################
# SliceDef file for Muon chains
###########################################################################
__author__  = 'C. Bernius'
__doc__="Definition of muon chains for AthenaMT" 

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

from TriggerMenuMT.HLTMenuConfig.Menu.MenuUtils import *

from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestRecoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoAlg
from TrigUpgradeTest.TrigUpgradeTestConf import HLTTest__TestHypoTool


def generateChainDefs(chainDict):
    
    listOfChainDicts = splitChainDict(chainDict)
    listOfChainDefs=[]

    #define the list of keywords from Signature dicts to base the grouping on
    groupCategories = []

    for chainDict in listOfChainDicts:
        chainName = chainDict['chainName']
        chainL1Item = chainDict['L1item']
        chainPart = chainDict['chainParts']
        chainPartL1Item = self.chainPart['L1item']
        chainthreshhold = chainPart['threshold']
        
        nsteps = 2
        allChainSteps=[]
        for step in nsteps:
            stepname="Step"+str(step)+"_"+chainName
            
            cstep=ChainStep(stepname, [SequenceHypoTool(muStep(str(step), chainL1Item), stepHypoTool(step, chainthreshold))])
            allChainSteps.append(cstep)

        MuonChainDef = Chain(chainName,chainL1Item,ChainSteps=allChainSteps)

        listOfChainDefs += MuonChainDef


    return listOfChainDefs

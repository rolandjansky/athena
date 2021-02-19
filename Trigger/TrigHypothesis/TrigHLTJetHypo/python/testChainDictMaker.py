# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Make chain dicts for testing jet hypo config modules"""

from __future__ import print_function

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (
    SingleJetGroup,
    MultiJetGroup,
    MuonJetGroup,
    PhysicsStream)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

from chainDict2jetLabel import chainDict2jetLabel 
from TrigJetHypoToolConfig import (trigJetHypoToolFromDict,
                                   nodesFromLabel,
                                   tree2tools,)

from TrigHLTJetHypo.ConditionsToolSetterFastReduction import (
    ConditionsToolSetterFastReduction,
)

from TrigHLTJetHypo.FastReductionAlgToolFactory import (
    FastReductionAlgToolFactory,)

from TrigHLTJetHypo.treeVisitors import TreeChecker

import sys

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG

logger = logging.getLogger(__name__)

logger.setLevel(DEBUG)




chains = [
    ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j80_j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_2j80_3j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_j0_aggSEP1000htSEP30etSEP0eta320_L1J15',
              l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

    ChainProp(name='HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20',
              l1SeedThresholds=['FSNOSEED']*3,
              groups=MultiJetGroup),

    ChainProp(name='HLT_10j40_L1J15',
              l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

    ChainProp(name='HLT_j0_aggSEP1000htSEP30etSEP0eta320_L1J20',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j0_fbdjshared_L1J20', groups=SingleJetGroup),
        
    ChainProp(name='HLT_j40_j0_aggSEP50htSEP10etSEP0eta320_L1J20',
              l1SeedThresholds=['FSNOSEED']*2,
              groups=MultiJetGroup),

    ChainProp(name='HLT_j0_fbdjnosharedSEP10etSEP20etSEP34massSEP50fbet_L1J20',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j60_prefilterSEP100ceta90SEP100nphi50_L1J20',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j45_pf_ftf_preselj20_L1J15', groups=SingleJetGroup),
    
    ChainProp(name='HLT_j85_ftf_prefilterSEP300ceta210SEP300nphi10_L1J20',
              groups=SingleJetGroup),
        
    ChainProp(name='HLT_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20', groups=SingleJetGroup),

    ChainProp(name='HLT_2mu6_2j50_0eta490_j0_dijetSEP50j1etSEP50j2etSEP900djmass_L1MJJ-500-NFF',l1SeedThresholds=['MU6','FSNOSEED', 'FSNOSEED'],stream=[PhysicsStream], groups=MuonJetGroup),
    

    ChainProp(name='HLT_2mu6_2j50_0eta490_j0_dijetSEP50j1etSEP50j2etSEP900djmass_L1MJJ-500-NFF',l1SeedThresholds=['MU6','FSNOSEED', 'FSNOSEED'],stream=[PhysicsStream], groups=MuonJetGroup),
    
    ChainProp(name='HLT_2mu6_j80_2j50_0eta490_j0_dijetSEP50j1etSEP50j2etSEP900djmass_L1MJJ-500-NFF',l1SeedThresholds=['MU6','FSNOSEED', 'FSNOSEED', 'FSNOSEED'],stream=[PhysicsStream], groups=MuonJetGroup),

    # bad chain, missing '_j0'
    ChainProp(name='HLT_2mu6_2j50_0eta490_dijetSEP50j1etSEP50j2etSEP900djmass_L1MJJ-500-NFF',l1SeedThresholds=['MU6','FSNOSEED'],stream=[PhysicsStream], groups=MuonJetGroup),
]

def testChainDictMaker(idict):

    if idict is not None:
        chain_props = [chains[idict]]
    else:
        chain_props = chains

    logger.debug(chain_props)
    result = []
    for cp in chain_props:
        chain_dict = dictFromChainName(cp)
        result.append((cp.name, chain_dict))

    return result


def list_chains():
    for i, c in enumerate(chains):
        logger.debug('%2d ' + c.name, i)


if __name__ == '__main__':

    debug = True
    idict = None
    if len(sys.argv) > 1:
        idict = int(sys.argv[1])

    if idict is not None and idict < 0:
        list_chains()
        sys.exit()

    dicts = testChainDictMaker(idict)
    for d in dicts:
        logger.debug(str(d))

    logger.debug('\n chain_labels:\n')
    
    for d in dicts:
        logger.debug (d[0])
        logger.debug (chainDict2jetLabel(d[1])+ '\n')

        
    logger.debug ('\n node trees:\n')
    
    for d in dicts:
        logger.debug(d[0])
        label = chainDict2jetLabel(d[1])
        chain_name = d[1]['chainName']


        forest = nodesFromLabel(label)

        algToolFactory = FastReductionAlgToolFactory()
        for tree in forest:
            toolSetter=ConditionsToolSetterFastReduction(algToolFactory)
            
            logger.debug(tree2tools(tree,
                                    toolSetter=toolSetter,
                                    checker=TreeChecker()).dump() + '\n')
        

    logger.debug ('\nMaking TrigJetHypoTool for each dictiomary\n')
    for d in dicts:
        logger.debug (d[0])
        logger.debug (str(trigJetHypoToolFromDict(d[1]))+'\n')

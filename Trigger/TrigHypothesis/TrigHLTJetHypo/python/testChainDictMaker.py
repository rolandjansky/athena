# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Make chain dicts for testing jet hypo config modules"""

from __future__ import print_function

from TriggerMenuMT.HLTMenuConfig.Menu.Physics_pp_run3_v1 import (
        SingleJetGroup,
        MultiJetGroup)

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName

    
def testChainDictMaker():

    chain_props = [
        ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
                  groups=SingleJetGroup),

        ChainProp(name='HLT_j80_j60_L1J15',
                  l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

        ChainProp(name='HLT_j0_HTSEP1000htSEP100etSEP0eta320_L1J15',
                  l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),


        ChainProp(name='HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20',
                  l1SeedThresholds=['FSNOSEED']*3,
                  groups=MultiJetGroup),

        ChainProp(name='HLT_j0_vbenfSEP30etSEP34mass35SEP50fbet_L1J20',
                  l1SeedThresholds=['FSNOSEED'],
                  groups=MultiJetGroup),
    ]

    result = []
    for cp in chain_props:
        chain_dict = dictFromChainName(cp)
        result.append((cp.name, chain_dict))

    return result

if __name__ == '__main__':
    dicts = testChainDictMaker()
    for d in dicts:
        print('')
        print (d)

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Make chain dicts for testing jet hypo config modules"""

from __future__ import print_function

from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (
    SingleJetGroup,
    MultiJetGroup,
)

DevGroup = ['Development']


from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName

from TrigHLTJetHypo.hypoToolDisplay import hypoToolDisplay

import sys

from pprint import pprint

# from TrigHLTJetHypo.xmlConfig import hypotool_from_chaindict
from TrigHLTJetHypo.hypoConfigBuilder import hypotool_from_chaindict

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
logger = logging.getLogger( __name__)

logger.setLevel(DEBUG)

chains = [
    ChainProp(name='HLT_j0_DIJET80j12ptXX700djmassXXdjdphi260_L1J20',
              l1SeedThresholds=['FSNOSEED'],groups=MultiJetGroup),

    ChainProp(name='HLT_j0_DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta_L1J20',
              l1SeedThresholds=['FSNOSEED'],groups=MultiJetGroup),
    
    ChainProp(name='HLT_j85_050momemfrac100_L1J20', groups=SingleJetGroup),
    
    ChainProp(name='HLT_j80_CLEANlb_L1J15', groups=SingleJetGroup),

    ChainProp(name='HLT_j80_CLEANllp_L1J15', groups=SingleJetGroup),
 
    ChainProp(name='HLT_j420_subresjesgscIS_ftf_L1J100',groups=SingleJetGroup),

    ChainProp(name='HLT_j0_perf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup),

    ChainProp(name='HLT_j0_perf_pf_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup),

    ChainProp(name='HLT_j0_perf_pf_ftf_nojcalib_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup),

    ChainProp(name='HLT_j0_perf_a10sd_cssk_pf_nojcalib_ftf_L1RD0_FILLED', l1SeedThresholds=['FSNOSEED'], stream=['Main'], groups=['PS:Online']+SingleJetGroup),

    ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j80_j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_2j80_3j60_L1J15',
              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

    ChainProp(name='HLT_j0_HT1000_L1J20', groups=SingleJetGroup),


     ChainProp(name='HLT_j70_0eta320_j50_0eta490_j0_DIJET70j12ptXX1000djmassXXdjdphi200XX400djdeta__L1MJJ-500-NFF',

               l1SeedThresholds=['FSNOSEED']*3,
               groups=MultiJetGroup),

    ChainProp(name='HLT_10j40_L1J15',
              l1SeedThresholds=['FSNOSEED'], groups=MultiJetGroup),

    ChainProp(name='HLT_j0_fbdjshared_L1J20', groups=SingleJetGroup),
        
    ChainProp(name='HLT_j40_j0_HT50XX10etXX0eta320_L1J20',
              l1SeedThresholds=['FSNOSEED']*2,
              groups=MultiJetGroup),

    ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20',
              groups=SingleJetGroup),

    ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20',
                  groups=SingleJetGroup),

    ChainProp(name='HLT_j45_pf_ftf_preselj20_L1J15', groups=SingleJetGroup),
    
    ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20',
              groups=SingleJetGroup),
        
    ChainProp(name='HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', groups=SingleJetGroup),

    ChainProp(name='HLT_j0_DIJET80j12ptXX0j12eta240XX700djmass_PTRANGE2r3_L1J20', groups=SingleJetGroup),

    ChainProp(name='HLT_j80_j60_SHARED_j40__L1J15', groups=MultiJetGroup, l1SeedThresholds=['FSNOSEED']*3,),

    ChainProp(name='HLT_j0_HT1000_j0_DIJET80j12ptXX0j12eta240XX700djmass_L1J20', l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),

     ChainProp(name='HLT_2j35_0eta240_roiftf_2j35_0eta240_85bdips_roiftf_presel4c35_L14J15p0ETA25', l1SeedThresholds=['FSNOSEED','FSNOSEED'], groups=MultiJetGroup+DevGroup),
]

def testChainDictMaker(idict):

    if idict >= 0:
        chain_props = [chains[idict]]
    else:
        chain_props = chains

    result = []
    for cp in chain_props:
        logger.debug(cp)
        chain_dict = dictFromChainName(cp)
        result.append((cp.name, chain_dict))

    return result


def list_chains():
    for i, c in enumerate(chains):
        logger.debug('%2d ' + c.name, i)


if __name__ == '__main__':
    import argparse
    import os

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '-i',
        '--iprop',
        help='index of chainProp to run, -1: do all',
        type=int)

    parser.add_argument('-d', '--dot', help='write out a dot file',
                        action='store_true')

    parser.add_argument(
        '--dotdir',
        help='specify directory to which dot file is to be written [$HOME]',
        default=os.environ['HOME'])

   
    args = parser.parse_args()

    if args.iprop is None:
        list_chains()
        sys.exit()

    iprop = args.iprop
    dicts = testChainDictMaker(iprop)
    for d in dicts:
        pprint(d)

    do_dot = args.dot
    if args.dotdir:
        do_dot = True
        
    for cn, d in dicts:
        hypo_tool = hypotool_from_chaindict(d)
        hypoToolDisplay(hypo_tool,
                        do_dot=do_dot,
                        dotdir=args.dotdir)

        

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""Make chain dicts for testing jet hypo config modules"""

from __future__ import print_function

from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import (
    SingleJetGroup,
    MultiJetGroup)

from TriggerMenuMT.HLT.Menu.ChainDefInMenu import ChainProp
from TriggerMenuMT.HLT.Menu.DictFromChainName import dictFromChainName

from TrigHLTJetHypo.hypoConfigBuilder import hypotool_from_chaindict
from TrigHLTJetHypo.HypoToolAnalyser import HypoToolAnalyser

import unittest

from AthenaCommon.Logging import logging
from AthenaCommon.Constants import DEBUG
logger = logging.getLogger( __name__)

logger.setLevel(DEBUG)

class HypoToolStructure(unittest.TestCase):
    def test_structure(self):
        testData = [
            {'prop':
             ChainProp(name='HLT_j420_subresjesgscIS_ftf_L1J100',
                       groups=SingleJetGroup),
             'connections': dict(((0, [1]),
                                  (1, [2, 10]),
                                  (2, [3, 5, 8, 9]),
                                  (3, [4]),
                                  (5, [6, 7]))),
             'values': []
             },
            
            {'prop':
             ChainProp(name='HLT_j260_320eta490_L1J75_31ETA49',
                       groups=SingleJetGroup),
             'connections': dict(((0, [1]),
                                  (1, [2, 10]),
                                  (2, [3, 5, 8, 9]),
                                  (3, [4]),
                                  (5, [6, 7]))),
             'values': []
             },
            
            
            {'prop':
             ChainProp(name='HLT_j80_j60_L1J15',
                       l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
             'connections': dict(((0, [1]),
                                  (1, [2, 14]),
                                  (2, [3, 5, 8, 11,12, 13]),
                                  (3, [4]),
                                  (5, [6, 7]),
                                  (8, [9, 10]))),
             'values': []
             },
            
        {
            'prop': ChainProp(name='HLT_2j80_3j60_L1J15',
                              l1SeedThresholds=['FSNOSEED']*2, groups=MultiJetGroup),
            'connections': dict(((0, [1]), (1, [2, 14]),
                                 (2, [3, 5, 8, 11, 12, 13]),
                                 (3, [4]),
                                 (5, [6, 7]),
                                 (8, [9,10]))),
             'values': []
        },

        {
            'prop': ChainProp(name='HLT_j0_HT1000_L1J20',
                              groups=SingleJetGroup),
            
            'connections': dict(((0, [1]),
                                 (1, [2, 12]),
                                 (2, [3, 5, 7, 8]),
                                 (3, [4]),
                                 (5, [6]),
                                 (8, [9]),
                                 (9, [10, 11]))),
             'values': []
        },

        {
            'prop':  ChainProp(
                name='HLT_j70_0eta320_1j50_0eta490_j0_DIJET70j12etXX1000djmassXXdjdphi200XX400djdeta__L1MJJ-500-NFF',
                l1SeedThresholds=['FSNOSEED']*3,
                groups=MultiJetGroup),
            
            'connections': dict(((0, [1]),
                                 (1, [2, 14, 31]),
                                 (2, [3, 5, 8, 11, 12, 13]),
                                 (3, [4]),
                                 (5, [6, 7]),
                                 (8, [9, 10]),
                                 (14, [15, 17, 21, 24, 27, 28, 29, 30]),
                                 (15, [16]),
                                 (17, [18, 19, 20]),
                                 (21, [22, 23]),
                                 (24, [25, 26]))),
             'values': []
        },

            {
                'prop': ChainProp(name='HLT_10j40_L1J15',
                                  l1SeedThresholds=['FSNOSEED'],
                                  groups=MultiJetGroup),
         
                'connections': dict(((0, [1]),
                                     (1, [2, 10]),
                                     (2, [3, 5, 8, 9]),
                                     (3, [4]),
                                     (5, [6, 7]))),
             'values': [{'nodeid': 5,
                         'multiplicity': 10}]
            },

            {
                'prop': ChainProp(name='HLT_j0_FBDJSHARED_L1J20',
                                  groups=SingleJetGroup),
        
                'connections': dict(((0, [1]),
                                     (1, [2, 14, 30]),
                                     (2, [3, 5, 8, 11, 12, 13]),
                                     (3, [4]),
                                     (5, [6, 7]),
                                     (8, [9, 10]),
                                     (14, [15, 17, 20, 23, 26, 27, 28, 29]),
                                     (15, [16]),
                                     (17, [18, 19]),
                                     (20, [21, 22]),
                                     (23, [24, 25]))),
             'values': []
            },
            
            {
                'prop': ChainProp(name='HLT_j40_0eta320_j0_HT50XX10etXX0eta320_L1J20',
                                  l1SeedThresholds=['FSNOSEED']*2,
                                  groups=MultiJetGroup),
        
                'connections': dict(((0, [1]),
                                     (1, [2, 10, 20]),
                                     (2, [3, 5, 8, 9]),
                                     (3, [4]),
                                     (5, [6, 7]),
                                     (10, [11, 13, 15, 16]),
                                     (11, [12]),
                                     (13, [14]),
                                     (16, [17,]),
                                     (17, [18, 19]))),
             'values': []
            },
            
            {
                'prop': ChainProp(name='HLT_j0_FBDJNOSHARED10etXX20etXX34massXX50fbet_L1J20',
                                  groups=SingleJetGroup),
                
                'connections': dict(((0, [1]),
                                     (1, [2, 26]),
                                     (2, [3, 5, 8, 11, 14, 17, 20, 21, 22, 23, 24, 25]),
                                     (3, [4]),
                                     (5, [6, 7]),
                                     (8, [9, 10]),
                                     (11, [12, 13]),
                                     (14, [15, 16]),
                                     (17, [18, 19]))),
                'values': []
            },


            {
                'prop': ChainProp(name='HLT_j45_pf_ftf_preselj20_L1J15',
                                  groups=SingleJetGroup),
        
                'connections': dict(((0, [1]),
                                     (1, [2, 10]),
                                     (2, [3, 5, 8, 9]),
                                     (3, [4]),
                                     (5, [6, 7]))),
             'values': []
            },
            
            {
                'prop': ChainProp(name='HLT_j85_ftf_MASK300ceta210XX300nphi10_L1J20',
                                  groups=SingleJetGroup),
                
                
                'connections': dict(((0, [1]),
                                     (1, [2, 10]),
                                     (2, [3, 5, 8, 9]),
                                     (3, [4]),
                                     (5, [6, 7]),
                                     (10, [11]),
                                     (11, [12, 13]))),
             'values': []
            },

    
            {
                'prop': ChainProp(name='HLT_j0_DIJET80j12etXX0j12eta240XX700djmass_L1J20',
                                  groups=SingleJetGroup),

        
        
                'connections': dict(((0, [1]),
                                     (1, [2, 17]),
                                     (2, [3, 5, 7, 10, 13, 14, 15, 16]),
                                     (3, [4]),
                                     (5, [6]),
                                     (7, [8, 9]),
                                     (10, [11, 12]))),

                'values': [{'nodeid': 6, 'min': '700000.0', 'max': 'inf'},
                           {'nodeid': 8, 'min': '80000.0', 'max': 'inf'},
                           {'nodeid': 9, 'min': '0.0', 'max': '2.4'},
                           {'nodeid': 11, 'min': '80000.0', 'max': 'inf'},
                           {'nodeid': 12, 'min': '0.0', 'max': '2.4'},

                           ]

            }, 
            
        
        ]
        
        for td in testData:
            chain_dict =chain_dict = dictFromChainName(td['prop'])
            # set chain part indices (kludge)
            cpi = 0
            for cp in chain_dict['chainParts']:
                cp['chainPartIndex'] = cpi
                cpi += 1
            chain_name = chain_dict['chainName']
            tool = hypotool_from_chaindict(chain_dict)
            
            analyser = HypoToolAnalyser(tool)
            self.assertTrue(analyser.connections == td['connections'],
                            'fail for case %s expected %s; saw %s' %(
                                chain_name,
                                str(td['connections']),
                                str(analyser.connections))) 

            # check selected attributes of inner (nested) tools.

            for ref_values in td['values']:

                # find the inner (nested) subtool
                # from the tool list in the analyser
                
                nodeid = ref_values['nodeid']
                del ref_values['nodeid']
                subtool = analyser.node_table[nodeid]

                # check selected attributes of the subtool
                
                for attrname, testval in ref_values.items():
                    toolval = getattr(subtool, attrname)
                    self.assertEqual(toolval, testval,
                                    msg='%s:  node: %d expected %s saw %s' % (
                                        chain_name,
                                        nodeid,
                                        str(testval),
                                        str(toolval)))
                    

if __name__ == '__main__':

    unittest.main()




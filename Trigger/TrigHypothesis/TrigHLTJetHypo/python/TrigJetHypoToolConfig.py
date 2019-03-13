# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TrigHLTJetHypo.TrigHLTJetHypoConf import (TrigJetHypoToolMT,
                                               TrigJetHypoToolConfig_EtaEt)

from  TrigHLTJetHypo.ToolSetter import ToolSetter
from  TrigHLTJetHypo.treeVisitors import TreeParameterExpander
from  TrigHLTJetHypo.chainDict2jetLabel import (make_simple_label,
                                                make_vbenf_label)
from  TrigHLTJetHypo.ChainLabelParser import ChainLabelParser

from GaudiKernel.Constants import (VERBOSE,
                                   DEBUG,
                                   INFO,
                                   WARNING,
                                   ERROR,
                                   FATAL,)


def  trigJetHypoToolFromDict(chain_dict):
    """Produce  a jet trigger hypo tool from a chainDict"""

    print 'trigJetHypoToolFromDict starts'
    chain_label = ''    
    if 'vbenf' in chain_dict['chainParts'][0]['hypoScenario']:
        assert len(chain_dict['chainParts']) == 1
        chain_label = make_vbenf_label(chain_dict)
    else:
        chain_label = make_simple_label(chain_dict)
    parser = ChainLabelParser(chain_label)
    tree = parser.parse()

    #expand strings of cuts to a cut dictionary
    visitor = TreeParameterExpander()
    tree.accept(visitor)
    visitor.report()

    # create - possibly nested - tools

    # chain name in run 2 dicts were missing the 'HLT_' prefix
    # but it seems it is necessary to run the hypos in AthenaMT ?...?
    
    chain_name = chain_dict['chainName']
    if not chain_name.startswith('HLT_'):
        chain_name = 'HLT_' + chain_name

    print 'trigJetHypoToolFromDict chain_name', chain_name
    visitor = ToolSetter(chain_name)
    tree.accept(visitor)
    visitor.report()

    print 'Dumping jet config for', chain_name
    print tree.dump()
    tool = tree.tool
    tool.OutputLevel = DEBUG
    return tool


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

        chainNameDecoder = DictFromChainName.DictFromChainName()
        # chain_names = ('HLT_j85', 'HLT_j35_0eta320')
        chain_names = ('HLT_j0hs_vbenf',)
        wid = max(len(c) for c in chain_names)
        for chain_name in chain_names:
            chain_dict = chainNameDecoder.getChainDict(chain_name)
            tool = trigJetHypoToolFromDict(chain_dict)
            self.assertIsNotNone(tool) 
            print '%s' % chain_name.rjust(wid), tool

if __name__ == '__main__':
    unittest.main()

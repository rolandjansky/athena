from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetHypoToolMT, TrigJetHypoToolConfig_EtaEt
import re

re_EtEta0 = re.compile(
    r'^HLT_j(?P<thresh>\d+)(_(?P<etalo>\d{3})eta(?P<etahi>\d{3}))?$')
# re_EtEta1 = re.compile(r'^HLT_j\d+(_\d{1-3}eta\d{3})?_L1')
# re_EtEta2 = re.compile(r'^HLT_j\d+(_\d{3}eta\d{3})?_(jes|nojcalib|lcw)*$')


def decodeEtEta(match, chain):
    """Create a hypo tool for the Et - Eta scenario"""
    default = {'etalo': '0', 'etahi': '320'}
    conf_dict = match.groupdict()
    for k, v in default.items():
        if k not in conf_dict: conf_dict[k] = v
        if conf_dict[k] is None: conf_dict[k] = v

    conf_tool = TrigJetHypoToolConfig_EtaEt(name=chain+"config")
    conf_tool.EtThresholds = [float(conf_dict['thresh'])]
    conf_tool.eta_mins = [float(conf_dict['etalo'])]
    conf_tool.eta_maxs = [float(conf_dict['etahi'])]
    conf_tool.asymmetricEtas = [0]
    conf_tool.OutputLevel = 0
    return conf_tool


def  trigJetHypoToolFromDict(chainDict):
    return trigJetHypoToolFromName( chainDict['chainName'], chainDict['chainName'])

def  trigJetHypoToolFromName(name, jetconfig):
    """Configure a jet hypo tool from chain name.

    Delegate to functions according to the hypo scenartio."""
    scenario_dict = {re_EtEta0: decodeEtEta}

    chain = jetconfig
    for k, v in scenario_dict.items():
        match = k.match(chain)
        if match:
            hypo_tool = TrigJetHypoToolMT(chain)
            hypo_tool.HypoConfigurer = v(match, chain)
            return hypo_tool

    msg = 'trigJetHypoToolFromName(%s) - decode error' % chain
    raise NotImplementedError(msg)


import unittest
class TestStringMethods(unittest.TestCase):
    def testValidConfigs(self):
        # EtaEt hypos
        # from MC_pp_v7 import  TriggerFlags.JetSlice.signatures
        # exception or any other issue will make the ctest for this package fail
        chains = ('HLT_j85', 'HLT_j35_320eta490')
        wid = max(len(c) for c in chains)
        for c in chains:
            tool = trigJetHypoToolFromName(c, c)
            self.assertIsNotNone( tool ) 
            print '%s' % c.rjust(wid), tool

    def testInvalidConfig(self):
        with self.assertRaises(NotImplementedError):
            tool = trigJetHypoToolFromName('HLT_nonsense', 'HLT_nonsense')


if __name__ == '__main__':
    unittest.main()

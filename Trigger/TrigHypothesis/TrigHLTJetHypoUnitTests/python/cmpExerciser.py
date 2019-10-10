# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from TrigHLTJetHypoUnitTests.TrigHLTJetHypoUnitTestsConf import (
    JetHypoExerciserCompareAlg,
    SimpleHypoJetVectorGenerator,)

from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

from TrigHLTJetHypo.TrigJetHypoToolConfig import (
    trigJetHypoToolHelperFromDict,
    trigJetHypoToolHelperFromDict_,)

from TrigHLTJetHypo.test_cases import test_strings

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigHLTJetHypo.ConditionsToolSetterTree import ConditionsToolSetterTree
from TrigHLTJetHypo.ConditionsToolSetterFlowNetwork \
    import ConditionsToolSetterFlowNetwork

class CombinationsTests(object):

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
    ):
        self.n_sgnl = n_sgnl
        self.n_bkgd = n_bkgd
        self.bkgd_etmax = bkgd_etmax
        self.chain_name = 'HLT_j80_L1J20'
        
    def make_chain_dict(self):
        """ChainDict to excercise modifications to CombinationsHelperTool"""
        
        chainNameDecoder = DictFromChainName.DictFromChainName()
        
        #make a chain dict to be perverted:
        # its hypoScenario will be overwritten by the value
        # 'combinationsTest'. This will result in a hardwired chain label
        # being used.
        chain_dict = chainNameDecoder.getChainDict(self.chain_name)
        assert len(chain_dict['chainParts']) == 1
        
        chain_dict['chainParts'][0]['hypoScenario'] = 'combinationsTest'
        
        return chain_dict

    def make_helper_tool(self):
        chain_dict = self._make_chain_dict()
        print(chain_dict['chainParts'][0])
        return trigJetHypoToolHelperFromDict(chain_dict)


    def make_event_generator(self):
        generator = SimpleHypoJetVectorGenerator()

        generator.ets = [80000. + 1000.*i for i in range(self.n_sgnl)]
        generator.etas = [0.5* pow(-1, i) for i in range(self.n_sgnl)]

        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator

    def logfile_name(self):
        return  self.chain_name + '_b' + str(self.n_bkgd) + '_combs.log'

    
class FlowNetworkQFBCompTests(CombinationsTests) :

    def __init__(self,
                 label,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
                 ):
        # useEtaEtNotEtaE = False 
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_FlowNetworkQFBComp'
        self.chain_label = label
        
    def make_helper_tools(self):

        setter0 = ConditionsToolSetterTree('treeConditionsTreeSetter')
        helper0 =  trigJetHypoToolHelperFromDict_(self.chain_label,
                                                  self.chain_name,
                                                  setter0)


        
        setter1 = ConditionsToolSetterFlowNetwork('fnConditionsTreeSetter')
        helper1 =  trigJetHypoToolHelperFromDict_(self.chain_label,
                                                  self.chain_name,
                                                  setter1)

        return (helper0, helper1)

    def logfile_name(self, chain_name):
        return chain_name + '_s' + str(self.n_sgnl) + '_b' + \
            str(self.n_bkgd)

    


    
def JetHypoExerciserCompareCfg(label,
                               n_signal,
                               n_background,
                               bkgdEmax):


    test_conditions = FlowNetworkQFBCompTests(label,
                                              n_signal,
                                              n_background,
                                              bkgdEmax,)

    print(test_conditions.__dict__)
    chain_name = test_conditions.chain_name

    ht0, ht1 = test_conditions.make_helper_tools()

    # print('ht0 = ', ht0)
    # print('ht1 = ', ht1)
    
    jetHypoExerciserAlg=JetHypoExerciserCompareAlg("JetHypoCompareExerciser")
    jetHypoExerciserAlg.JetHypoHelperTool0 = ht0
    # jetHypoExerciserAlg.JetHypoHelperTool1 = ht0
    jetHypoExerciserAlg.JetHypoHelperTool1 = ht1
  
    jetHypoExerciserAlg.event_generator = test_conditions.make_event_generator()
    jetHypoExerciserAlg.visit_debug = True

    lfn = test_conditions.logfile_name(chain_name)

    jetHypoExerciserAlg.logname = lfn

    
    print(jetHypoExerciserAlg)

    result=ComponentAccumulator()
    result.addEventAlgo(jetHypoExerciserAlg)
    return result



if __name__=="__main__":

    n_signal = 6
    n_background = 0
    bkgdEmax = 0.
    label_ind = 20
    label = test_strings[label_ind]

    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    cfg=MainServicesSerialCfg()
    cfg.merge(JetHypoExerciserCompareCfg(label,
                                         n_signal,
                                         n_background,
                                         bkgdEmax)
    )

    cfg.setAppProperty("EvtMax", 10)
    cfg.run()

    #f=open("HelloWorld.pkl","w")
    #cfg.store(f)
    #f.close()


# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from TrigHLTJetHypoUnitTests.TrigHLTJetHypoUnitTestsConf import (
    JetHypoExerciserAlg,
    SimpleHypoJetVectorGenerator,)

from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

from TrigHLTJetHypo.TrigJetHypoToolConfig import (
    trigJetHypoToolHelperFromDict,
    trigJetHypoToolHelperFromDict_,)

from TrigHLTJetHypo.test_cases import test_strings

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigHLTJetHypo.ConditionsToolSetterTree import ConditionsToolSetterTree as ConditionsToolSetter

class PartitionvsFlowNetworkTests(object):

    def __init__(self,
                 n_conds=6,
                 n_bkgd=4,
                 bkgd_etmax=50000.,   # MeV
                 run_with_partitions=False):
        
        self.n_conds = n_conds
        self.n_bkgd = n_bkgd
        self.bkgd_etmax = bkgd_etmax
        self.run_with_partitions = run_with_partitions

    
    def make_chain_dict(self):
        """Chaindicts for Partion vs flownetwork strudies"""
    
        chainNameDecoder = DictFromChainName.DictFromChainName()
        self.chain_name = 'HLT'
        for i in range(self.n_conds):
            self.chain_name += '_j80'

        
        chain_dict = chainNameDecoder.getChainDict(self.chain_name)
        assert len(chain_dict['chainParts']) == self.n_conds

        if (self.run_with_partitions):
            for cp in chain_dict['chainParts']:
                cp['hypoScenario'] = 'simple_partition'

        return chain_dict

    
    def make_event_generator(self):
        generator = SimpleHypoJetVectorGenerator()
    
        generator.ets = [80000. + 1000.*i for i in range(self.n_sgnl)]
        generator.etas = [0.5] * self.n_sgnl
        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator

    def logfile_name(self, chain_name):
        if (self.run_with_partitions):
            lfn = self.chain_name + '_b' + str(self.n_bkgd) + '_part.log'
        else:
            lfn = self.chain_name + '_b' + str(self.n_bkgd) + '.log'

        return lfn

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
        generator.etas = [0.5] * self.n_sgnl

        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator

    def logfile_name(self):
        return  self.chain_name + '_b' + str(self.n_bkgd) + '_combs.log'

    
class PartitionsTests(CombinationsTests) :

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
    ):
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)

    def _make_chain_dict(self):
        """ChainDict to excercise modifications to CombinationsHelperTool"""
        
        chain_dict = CombinationsTests.make_chain_dict(self)
        chain_dict['chainParts'][0]['hypoScenario'] = 'partitionsTest'
        
        return chain_dict

    def make_helper_tool(self):
        chain_dict = self._make_chain_dict()
        print(chain_dict['chainParts'][0])
        return trigJetHypoToolHelperFromDict(chain_dict)


    def logfile_name(self, chain_name):
        return  chain_name + '_b' + str(self.n_bkgd) + '_parts.log'


class SimpleConditionsTests(CombinationsTests) :

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=20000.,  # MeV
    ):
        # useEtaEtNotEtaE = False 
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_DijetConditionTests'

    def make_helper_tool(self):
        chain_label = """
        z([]
            simple([(10et, 0eta320)(20et)])
        )"""


        toolSetter = ConditionsToolSetter(self.chain_name)
        return trigJetHypoToolHelperFromDict_(chain_label,
                                              self.chain_name,
                                              toolSetter=toolSetter)


    def logfile_name(self, chain_name):
        return chain_name +  '_s' + str(self.n_sgnl) + '_b' + str(self.n_bkgd)+'.log'

    def make_event_generator(self):
        generator = SimpleHypoJetVectorGenerator()

        generator.ets = [80000. + 1000.*i for i in range(self.n_sgnl)]
        generator.etas = [0.5] * self.n_sgnl

        # alternate eta signs to get high mass
        factor = 1
        for i in range(len(generator.etas)):
            generator.etas[i] *= factor
            factor *= -1

        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator

class ConditionsTests(CombinationsTests) :

    def __init__(self,
                 chain_label,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=20000.,  # MeV
                 label_ind=0
    ):
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_ConditionTests'
        self.chain_label = chain_label
        self.label_ind = label_ind

    def make_helper_tool(self):

        toolSetter = ConditionsToolSetter(self.chain_name)
        return trigJetHypoToolHelperFromDict_(self.chain_label,
                                              self.chain_name,
                                              toolSetter=toolSetter)


    def logfile_name(self, chain_name):
        return '%s_s%d_b%d_l%d' % (chain_name,
                                   self.n_sgnl,
                                   self.n_bkgd,
                                   self.label_ind)


    def make_event_generator(self):
        generator = SimpleHypoJetVectorGenerator()

        generator.ets = [80000. + 1000.*i for i in range(self.n_sgnl)]
        generator.etas = [0.5] * self.n_sgnl


        # alternate eta signs to get high mass
        factor = 1
        for i in range(len(generator.etas)):
            generator.etas[i] *= factor
            factor *= -1

        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator


    
def JetHypoExerciserCfg(label,
                        n_signal,
                        n_background,
                        bkgdEmax,
                        label_ind=0):

    # test_conditions = FlowNetworkTests_1(n_sgnl=1, n_bkgd=0)
    # test_conditions = SimpleFlowNetworkTests(n_sgnl=4, n_bkgd=0)
    # test_conditions = FlowNetworkVsPartitionsTestsDijets(n_sgnl=4, n_bkgd=0)
    # test_conditions = FlowNetworkVsCombinationsTests(n_sgnl=4, n_bkgd=0)
    # test_conditions = SimpleConditionsTests(n_sgnl=4, n_bkgd=0)
    test_conditions = ConditionsTests(label,
                                      n_signal,
                                      n_background,
                                      bkgdEmax,
                                      label_ind)

    print(test_conditions.__dict__)
    # test_conditions =  CombinationsTests()
    chain_name = test_conditions.chain_name

    ht = test_conditions.make_helper_tool()

    print('ht = ', ht)
    
    jetHypoExerciserAlg=JetHypoExerciserAlg("JetHypoExerciser")
    jetHypoExerciserAlg.JetHypoHelperTool = ht
    jetHypoExerciserAlg.event_generator = test_conditions.make_event_generator()
    jetHypoExerciserAlg.visit_debug = True

    lfn = test_conditions.logfile_name(chain_name)

    jetHypoExerciserAlg.logname = lfn

    
    print(jetHypoExerciserAlg)

    result=ComponentAccumulator()
    result.addEventAlgo(jetHypoExerciserAlg)
    return result



if __name__=="__main__":

    n_signal = 4
    n_background = 0
    bkgdEmax = 0.
    label_ind = 2

    label = test_strings[label_ind]

    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    ConfigFlags.Exec.MaxEvents=10
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(JetHypoExerciserCfg(label,
                                  n_signal,
                                  n_background,
                                  bkgdEmax,
                                  label_ind)
    )

    cfg.run()

    #f=open("HelloWorld.pkl","wb")
    #cfg.store(f)
    #f.close()


from __future__ import print_function

from TrigHLTJetHypoUnitTests.TrigHLTJetHypoUnitTestsConf import (
    JetHypoExerciserAlg,
    SimpleHypoJetVectorGenerator,)

from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

from TrigHLTJetHypo.TrigJetHypoToolConfig import (
    trigJetHypoToolHelperFromDict,
    trigJetHypoToolHelperFromDict_)

from AthenaCommon.JobProperties import jobproperties
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


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

    
    def make_chain_dict():
        """Chaindicts for Partion vs flownetwork strudies"""
    
        chainNameDecoder = DictFromChainName.DictFromChainName()
        self.chain_name = 'HLT'
        for i in range(self.n_conds):
            self.chain_name += '_j80'

        
        chain_dict = chainNameDecoder.getChainDict(self.chain_name)
        assert len(chain_dict['chainParts']) == n_conds

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
        if (run_with_partitions):
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
        chain_dict = self._make_chain_dict();
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
        chain_dict = self._make_chain_dict();
        print(chain_dict['chainParts'][0])
        return trigJetHypoToolHelperFromDict(chain_dict)


    def logfile_name(self, chain_name):
        return  chain_name + '_b' + str(self.n_bkgd) + '_parts.log'

class FlowNetworkVsPartitionsTests(CombinationsTests) :

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
    ):
        useEtaEtNotEtaE = false 
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_FNvsPartition'

    def make_helper_tool(self):
        chain_label = """
        agree([]
        simple([(80et)(81et)(82et)(83et)])
        
        partgen(
        []
        simple([(80et)(81et)])
        simple([(82et)(83et)]))
        )"""
        
        return trigJetHypoToolHelperFromDict_(chain_label,
                                              self.chain_name)


    def logfile_name(self, chain_name):
        return chain_name + '_s' + str(self.n_sgnl) + '_b' + \
            str(self.n_bkgd) + '.log'

    
class FlowNetworkVsPartitionsTestsDijets(CombinationsTests) :

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
    ):
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_FNvsPartitionDijets'

    def make_helper_tool(self):
        chain_label = """agree([]
                         dijet([(80mass)(81mass)])
                         partgen([]
                         dijet([(80mass)])
                         dijet([(81mass)]))
        )"""
      
        return trigJetHypoToolHelperFromDict_(chain_label,
                                              self.chain_name)


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


class FlowNetworkVsCombinationsTests(CombinationsTests) :

    def __init__(self,
                 n_sgnl=4,
                 n_bkgd=4,
                 bkgd_etmax=50000.,  # MeV
    ):
        CombinationsTests.__init__(self, n_sgnl, n_bkgd, bkgd_etmax)
        self.chain_name = 'HLT_FNvsCombination'

    def make_helper_tool(self):
        chain_label = """
        agree([]
        simple([(80et)(81et)(82et)(83et)])
        
        combgen(
        []
        simple([(80et)(81et)])
        simple([(82et)(83et)]))
        )"""
        # ))"""
        
        return trigJetHypoToolHelperFromDict_(chain_label,
                                              self.chain_name)

    def make_event_generator(self):
        generator = SimpleHypoJetVectorGenerator()

        # setup signal so that partitions would fail, but combinations pass.
        # for n_sgnl = 4 and 2 EtaEt conditions, this means:
        # create 4 jets such that only one pair passes the lower EtaEt cut.
        ets = (30000., 31000., 85000., 86000.)
        etas = (0.49, -0.5, 0.5, -0.5)
        if self.n_sgnl == 4:
            generator.ets = ets[:]
            generator.etas = etas[:]
        elif self.n_sgnl == 3:
            generator.ets = ets[:3]
            generator.etas = etas[:3]
        else:
            msg = self.__class__.__name__ + " n_sgnl =" + n_sgnl
            msg += " legal values = 3, 4"
            raise RuntimeError(msg)


        generator.n_bkgd = self.n_bkgd
        generator.bkgd_etmax = self.bkgd_etmax

        return generator

    def logfile_name(self, chain_name):
        return chain_name + '_s' + str(self.n_sgnl) + '_b' + \
            str(self.n_bkgd) + '.log'

    
def JetHypoExerciserCfg():

    # test_conditions =  PartitionsTests()
    # test_conditions = FlowNetworkVsPartitionsTestsDijets(n_sgnl=3, n_bkgd=10)
    test_conditions = FlowNetworkVsCombinationsTests(n_sgnl=4, n_bkgd=0)

    print(test_conditions.__dict__)
    # test_conditions =  CombinationsTests()
    generator = test_conditions.make_event_generator()
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
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    #log.setLevel(DEBUG)

    

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    cfg=MainServicesSerialCfg()
    cfg.merge(JetHypoExerciserCfg())
    cfg.setAppProperty("EvtMax", 1)
    cfg.run()

    #f=open("HelloWorld.pkl","w")
    #cfg.store(f)
    #f.close()


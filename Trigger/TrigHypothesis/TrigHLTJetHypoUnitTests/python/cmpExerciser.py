# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from TrigHLTJetHypoUnitTests.TrigHLTJetHypoUnitTestsConf import (
        JetHypoExerciserCompareAlg,
        SimpleHypoJetVectorGenerator,
        RandomSignalHypoJetVectorGenerator,
        AlwaysSameHelperTool,
)

from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

from TrigHLTJetHypo.TrigJetHypoToolConfig import (
    trigJetHypoToolHelperFromDict,
    trigJetHypoToolHelperFromDict_,)

from TrigHLTJetHypo.test_cases import test_strings

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from TrigHLTJetHypo.ConditionsToolSetterTree import ConditionsToolSetterTree

from TrigHLTJetHypo.ConditionsToolSetterFastReduction import \
        ConditionsToolSetterFastReduction

import os


def make_simple_event_generator(n_nsignal=4,
                                n_bkgd=0,
                                bkgd_etmax=50000.,  # MeV
                                useEtNotE=True):

        generator = SimpleHypoJetVectorGenerator()

        generator.ets = [80000. + 1000.*i + 500. for i in range(n_signal)]
        generator.etas = [0.5* pow(-1, i) for i in range(n_signal)]

        generator.n_bkgd = n_bkgd
        generator.bkgd_etmax = bkgd_etmax

        # generator.bkgd_etamax = 1.0

        generator.useEtaEtNotEtaE = useEtNotE

        return generator

    
class CombinationsTests(object):

    def __init__(self):
            pass
    
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

    def logfile_name(self):
        return  self.chain_name + '_b' + str(self.n_bkgd) + '_combs.log'

    
class FastReductionLabeledCompTests(CombinationsTests) :

    def __init__(self,
                 label,
                 fn_frag,
                 mult_string):

        CombinationsTests.__init__(self)
        self.chain_name = 'HLT_' + fn_frag
        self.chain_label = label
        self.logname =  self.chain_name + mult_string + '.log'
        
    def make_helper_tools(self):

        setter1 = ConditionsToolSetterTree('dummyChainName')
        helper1 =  trigJetHypoToolHelperFromDict_(self.chain_label,
                                                  self.chain_name,
                                                  setter1)

        # setter1 = ConditionsToolSetterTree('treeConditionsTreeSetter')
        # helper1 =  trigJetHypoToolHelperFromDict_(self.chain_label,
        #                                           self.chain_name,
        #                                          setter1)
        
        # setter0 = ConditionsToolSetterFastReduction('frConditionsTreeSetter')
        # helper0 =  trigJetHypoToolHelperFromDict_(self.chain_label,
        #                                            self.chain_name,
        #                                            setter0)

        helper0 = AlwaysSameHelperTool()
        helper0.passflag = False                

        setter0 = ConditionsToolSetterFastReduction('dummyChainName')
        helper0 =  trigJetHypoToolHelperFromDict_(self.chain_label,
                                                  self.chain_name,
                                                  setter0)
                
        # helper0 = AlwaysSameHelperTool()
        # helper0.passflag = False

        # helper1 = AlwaysSameHelperTool()
        # helper1.passflag = True

        return (helper0, helper1)


def JetHypoExerciserCompareCfg(label,
                               fn_frag,
                               mult_string,
                               event_generator):


    test_conditions = FastReductionLabeledCompTests(label,
                                                    fn_frag,
                                                    mult_string)

    print(test_conditions.__dict__)

    ht0, ht1 = test_conditions.make_helper_tools()
    
    # print('ht0 = ', ht0)
    # print('ht1 = ', ht1)
    
    jetHypoExerciserAlg=JetHypoExerciserCompareAlg("JetHypoCompareExerciser")
    jetHypoExerciserAlg.JetHypoHelperTool0 = ht0  # run ufn first
    # jetHypoExerciserAlg.JetHypoHelperTool1 = ht0
    jetHypoExerciserAlg.JetHypoHelperTool1 = ht1  # then tree

    jetHypoExerciserAlg.event_generator = event_generator
    
    jetHypoExerciserAlg.visit_debug = True  #  more evebt debug if True
    
    jetHypoExerciserAlg.logname = test_conditions.logname
    
    
    print(jetHypoExerciserAlg)
    
    result=ComponentAccumulator()
    result.addEventAlgo(jetHypoExerciserAlg)
    return result


if __name__ == "__main__":
    doSimpleGen = True
    useEtNotE = True
    if doSimpleGen:
        n_signal = 6
        n_background = 10
        bkgdEmax = 50000.
        useEtNotE = True
        event_generator = make_simple_event_generator(
                n_signal,
                n_background,
                bkgdEmax,
                useEtNotE)
        mult_string = '_s' + str(n_signal).zfill(2) + '_b' + \
                      str(n_background).zfill(2)
    else:
        n_sig_lo = 10
        n_sig_hi = 15
        e_sig_lo = 80
        e_sig_hi = 90
        eta_sig_hi = 5
        n_bkgd = 2
        bkgd_etmax = 50000.  # MeV
        useEtaEtNotEtaE = False
        event_generator = RandomSignalHypoJetVectorGenerator()
        event_generator.n_sig_lo = n_sig_lo
        event_generator.n_sig_hi = n_sig_hi
        event_generator.e_sig_lo = e_sig_lo * 1000.
        event_generator.e_sig_hi = e_sig_hi * 1000.
        event_generator.eta_sig_hi = eta_sig_hi
        event_generator.n_bkgd = n_bkgd
        event_generator.bkgd_etmax = bkgd_etmax
        event_generator.useEtaEtNotEtaE = useEtaEtNotEtaE

        s_params = [str(p).zfill(2) for p in (
                n_sig_lo,
                n_sig_hi,
                e_sig_lo,
                e_sig_hi,
                eta_sig_hi,
                n_bkgd)]
    
        mult_string = '_s%s_%s_%s_%s_%s_b%s' % tuple(s_params)
        
    # label_ind = 22  # simplegen, s=5,6
    # label_ind = 25 # debug label
    # label_ind =  26 # and
    # label_ind = 21 # quadjet
    label_ind = 12   # multithreshold
    label = test_strings[label_ind]
    lfn = 'test_case.txt'
    if(not os.path.exists(lfn)):
        with open(lfn, 'w') as ofile:
            ofile.write(label+'\n')
       
       
       
    fn_frag = {
        0: 'simple1condComp',
        1: 'simple2condComp',
        12: 'NoRootNeeded',
        21: 'QFBComp',
        22: 'QFBCompLow',
        23: 'dijet',
        24: 'dijeteasy',
        25: 'debug',
        26: 'and',
    }[label_ind]
    
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Exec.MaxEvents=10
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(JetHypoExerciserCompareCfg(label,
                                         fn_frag,
                                         mult_string,
                                         event_generator)
    )

    cfg.run()

    #f=open("HelloWorld.pkl","wb")
    #cfg.store(f)
    #f.close()

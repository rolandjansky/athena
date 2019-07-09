from __future__ import print_function

from TrigHLTJetHypoUnitTests.TrigHLTJetHypoUnitTestsConf import (
    JetHypoExerciserAlg,
    SimpleHypoJetVectorGenerator,)

from TriggerMenuMT.HLTMenuConfig.Menu import DictFromChainName

from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetHypoToolHelperFromDict

from AthenaCommon.JobProperties import jobproperties
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def JetHypoExerciserCfg():
    result=ComponentAccumulator()
    

    chainNameDecoder = DictFromChainName.DictFromChainName()
    # chain_name = 'HLT_j85'
    # chain_name = 'HLT_j80_0eta240_2j60_320eta490_j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20'

    n_conds = 6
    chain_name = 'HLT'
    for i in range(n_conds):
        chain_name += '_j80'

    chain_dict = chainNameDecoder.getChainDict(chain_name)
    assert len(chain_dict['chainParts']) == n_conds

    run_with_partitions = True
    if (run_with_partitions):
        for cp in chain_dict['chainParts']:
            cp['hypoScenario'] = 'simple_partition'

    print(chain_dict['chainParts'][0])

    
    # j0_dijetSEP80j1etSEP0j1eta240SEP80j2etSEP0j2eta240SEP700djmass_L1J20'

    ht=trigJetHypoToolHelperFromDict(chain_dict)

    generator = SimpleHypoJetVectorGenerator()
    
    generator.ets = [80000. + 1000.*i for i in range(n_conds)]
    generator.etas = [0.5] * n_conds
    generator.n_bkgd = 4
    generator.bkgd_etmax = 50.
    
    
    jetHypoExerciserAlg=JetHypoExerciserAlg("JetHypoExerciser")
    jetHypoExerciserAlg.JetHypoHelperTool = ht
    jetHypoExerciserAlg.event_generator = generator
    jetHypoExerciserAlg.visit_debug = True


    if (run_with_partitions):
        lfn = chain_name + '_b' + str(generator.n_bkgd) + '_part.log'
    else:
        lfn = chain_name + '_b' + str(generator.n_bkgd) + '.log'

    print(chain_dict['chainParts'][0])
    jetHypoExerciserAlg.logname = lfn

    
    print(jetHypoExerciserAlg)

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
    cfg.setAppProperty("EvtMax", 1000)
    cfg.run()

    #f=open("HelloWorld.pkl","w")
    #cfg.store(f)
    #f.close()


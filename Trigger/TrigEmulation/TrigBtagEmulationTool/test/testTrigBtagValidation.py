#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def main():
    EvtMax = 20
    inputFiles = ['/global/homes/c/cvarni/Athena/TrigBtagEmulationToolLayout/data/TrigAnalysisTest.2022-05-09T2101.test_trigAna_RDOtoADO_v1Dev_grid.AOD.pool.root']
    
    # for validation compare the decisions of a single chain, or all the chains in the menu 
    jetcoll_name_mapping = {
        "a10sd_cssk_pf_jes_ftf": "HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_jes_ftf", # 0
        "a10sd_cssk_pf_nojcalib_ftf": "HLT_AntiKt10EMPFlowCSSKSoftDropBeta100Zcut10Jets_nojcalib_ftf:", # 1
        "a10sd_pf_nojcalib_ftf": "HLT_AntiKt10EMPFlowSoftDropBeta100Zcut10Jets_nojcalib_ftf", # 2
        "a10r_subjesIS_ftf": "HLT_AntiKt10EMTopoRCJets_subjesIS", # 3
        "a10_lcw_subjes": "HLT_AntiKt10LCTopoJets_subjes", # 4
        "a10sd_lcw_nojcalib": "HLT_AntiKt10LCTopoSoftDropBeta100Zcut10Jets_nojcalib", # 5
        "a10t_lcw_jes": "HLT_AntiKt10LCTopoTrimmedPtFrac4SmallR20Jets_jes", # 6
        "a4_cssk_pf_nojcalib_ftf": "HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf", # 7
        "a4_pf_nojcalib_ftf": "HLT_AntiKt4EMPFlowJets_nojcalib_ftf", # 8
        "a4_pf_subjesgscIS_ftf": "HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf", # 9
        "a4_pf_subresjesgscIS_ftf": "HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf", # 10
        "a4_subjesIS": "HLT_AntiKt4EMTopoJets_subjesIS", # 11
        "a4_subjesgscIS_ftf": "HLT_AntiKt4EMTopoJets_subjesgscIS_ftf", # 12
    }

    trigger_slice = ['Jet', 'Bjet'][1]
    jetcontainer_EMTopo = ''
    jetcontainer_PFlow = ''
    validation_singlechain = ''
    jetcoll_emul = list(jetcoll_name_mapping.keys())[0] # only used for Jet slice, bjet slice uses a4_pf_subresjesgscIS_ftf [10]

    from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import setupMenu
    chains_phys_pp_run3_v1 = setupMenu()
    if validation_singlechain:
        emulatedChains = [cp for cp in chains_phys_pp_run3_v1[trigger_slice] if cp.name == validation_singlechain]
    else:
        if trigger_slice == 'Jet':
            jetcontainer_EMTopo = jetcoll_name_mapping[jetcoll_emul]
            emulatedChains = [cp for cp in chains_phys_pp_run3_v1[trigger_slice] if jetcoll_emul in cp.name]
        elif trigger_slice == 'Bjet':
            emulatedChains = [cp for cp in chains_phys_pp_run3_v1[trigger_slice] if '_pf_' in cp.name and '_HT' not in cp.name and 'dl1d85bb' not in cp.name]

        # retagging example, DL1d from BTagging/20210519r22/dl1d/antikt4empflow/network.json (needs to be coded in TrigBtagEmulationJet.cxx as well)
        if trigger_slice[0] == 'B':
            from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
            from TriggerMenuMT.HLT.Menu.Physics_pp_run3_v1 import SingleBjetGroup, PrimaryLegGroup
            emulatedChains += [ChainProp(name='HLT_j45_0eta290_020jvt_pf_ftf_newTagger_L1J20', l1SeedThresholds=['FSNOSEED'], groups=PrimaryLegGroup+SingleBjetGroup)]

            from TriggerMenuMT.HLT.Menu.SignatureDicts import JetChainParts
            JetChainParts['bTag'] += ['newTagger']


    from AthenaCommon.Configurable import ConfigurableRun3Behavior
    with ConfigurableRun3Behavior():
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Scheduler.ShowDataDeps = True
        ConfigFlags.Scheduler.ShowDataFlow = True
        ConfigFlags.Scheduler.ShowControlFlow = True
        ConfigFlags.Input.Files = inputFiles 
        ConfigFlags.Exec.MaxEvents = EvtMax

        ConfigFlags.lock()
        ConfigFlags.dump()

        from AthenaConfiguration.MainServicesConfig import MainServicesCfg
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        acc = MainServicesCfg( ConfigFlags )
        acc.merge( PoolReadCfg( ConfigFlags ) )
        acc.getService("MessageSvc").Format = "% F%80W%S%7W%R%T %0W%M"
        acc.getService("MessageSvc").defaultLimit = 2147483647
    
        from TrigBtagEmulationTool.TrigBtagEmulationToolConfig import TrigBtagValidationTestCfg
        acc.merge(TrigBtagValidationTestCfg(ConfigFlags,
                                            toBeEmulatedTriggers = emulatedChains,
                                            InputChain_EMTopo = '',
                                            InputJetContainer_EMTopo = jetcontainer_EMTopo,
                                            InputJetContainer_EMTopoPresel = '',
                                            InputChain_PFlow = validation_singlechain,
                                            InputJetContainer_PFlow = jetcontainer_PFlow,
                                            InputJetContainer_PFlowPresel = '' ))
        
        acc.printConfig(withDetails = True, summariseProps = True)
        acc.store( open('TrigBtagValidationConfig.pkl','wb') )

        acc.run()

if __name__ == "__main__":
    main()

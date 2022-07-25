# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from TriggerMenuMT.HLT.Config.Utility.DictFromChainName import dictFromChainName
from TriggerMenuMT.HLT.Config.Utility.ChainDefInMenu import ChainProp
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg

# Jet Manager Tools
def jetManagerToolCfg(flags,
                      name: str,
                      **kwargs) -> ComponentAccumulator:
    assert isinstance(name, str), "JetManagerTool name must be a string"
    acc = ComponentAccumulator()
    kwargs.setdefault('BTaggingLink', 'btaggingLink')
    acc.setPrivateTools(CompFactory.Trig.JetManagerTool(name, **kwargs))
    return acc

# CNT
def jetPFlowCNTManagerToolCfg(flags,
                              **kwargs) -> ComponentAccumulator:
    kwargs.setdefault('JetContainerName', 'HLT_AntiKt4EMPFlowJets_subjesIS')
    return jetManagerToolCfg(flags,
                             name = "JM_PFlow_CNT",
                             **kwargs)

# PRESEL
def jetEMTopoPRESELManagerToolCfg(flags,
                                  **kwargs) -> ComponentAccumulator:
    kwargs.setdefault('JetContainerName', 'HLT_AntiKt4EMTopoJets_subjesIS')
    return jetManagerToolCfg(flags, 
                             name = "JM_EMTopo_PRESEL",
                             **kwargs)

# Emulation Tool                            
def TrigBtagEmulationToolCfg(flags, 
                             toBeEmulatedTriggers: list, 
                             InputChain_EMTopo: str,
                             InputJetContainer_EMTopo: str,
                             InputJetContainer_EMTopoPresel: str,
                             InputChain_PFlow: str,
                             InputJetContainer_PFlow: str,
                             InputJetContainer_PFlowPresel: str,
                             **kwargs) -> ComponentAccumulator:
    assert isinstance(toBeEmulatedTriggers, list)
    assert isinstance(InputChain_EMTopo, str)
    assert isinstance(InputJetContainer_EMTopo, str)
    assert isinstance(InputJetContainer_EMTopoPresel, str)
    assert isinstance(InputChain_PFlow, str)
    assert isinstance(InputJetContainer_PFlow, str)
    assert isinstance(InputJetContainer_PFlowPresel, str)

    if len(InputChain_EMTopo) == 0:
        InputChain_EMTopo = 'HLT_j45_subjesgsc_ftf_L1J15'
    if len(InputJetContainer_EMTopo) == 0:
        InputJetContainer_EMTopo = 'HLT_AntiKt4EMTopoJets_subjesIS'
    if len(InputJetContainer_EMTopoPresel) == 0:
        InputJetContainer_EMTopoPresel = 'HLT_AntiKt4EMTopoJets_subjesIS'
    if len(InputChain_PFlow) == 0:
        InputChain_PFlow = 'HLT_j[234][05]_.*pf_ftf_[0b].*[ft]_L1.*'
    if len(InputJetContainer_PFlow) == 0:
        InputJetContainer_PFlow = 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf'
    if len(InputJetContainer_PFlowPresel) == 0:
        InputJetContainer_PFlowPresel = 'HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf'

    ### determine trigger thresholds from to be emulated chain names
    chainDefinitions = {}
    for chain in toBeEmulatedTriggers:
        chainDict = dictFromChainName(chain)
        chainParts = ['L1item:' + chainDict['L1item']]
        for chainPart in chainDict['chainParts']:
            # Gaudi::Property has limited support for nested structures, put everything in a long string and parse in C++
            # and update TrigBtagEmulationChain::parseChainDefinition(...) accordingly
            partDefinition = ''
            partDefinition += 'L1threshold:' + chainPart['L1threshold']
            partDefinition += '|name:' + chainPart['chainPartName']
            partDefinition += '|multiplicity:' + chainPart['multiplicity']
            partDefinition += '|threshold:' + chainPart['threshold']
            partDefinition += '|etaRange:' + chainPart['etaRange']
            partDefinition += '|jvt:' + (chainPart['jvt'] if chainPart['jvt'] else '-99999')
            partDefinition += '|tagger:' + chainPart['bTag']
            partDefinition += '|jetpresel:' + chainPart['trkpresel']
            partDefinition += '|dijetmass:' + (chainPart['hypoScenario'][len('DJMASS'):] if ('DJMASS' in chainPart['hypoScenario']) else 'None')
            partDefinition += '|isPFlow:' + ('True' if (chainPart['constitType'] == 'pf') else 'False')
            partDefinition += '|isShared:' + ('True' if ('SHARED' in chainPart['chainPartName']) else 'False')
            chainParts.append(partDefinition)
        chainName = chain.name if isinstance(chain, ChainProp) else chain
        chainDefinitions[chainName] = chainParts
    ### all trigger thresholds parsed

    # Component Accumulator
    acc = ComponentAccumulator()

    ### add TriggerDecisionTool
    tdt = acc.getPrimaryAndMerge(TrigDecisionToolCfg(flags))

    ### set tool options
    kwargs.setdefault('EmulatedChainDefinitions', chainDefinitions)
    kwargs.setdefault('TrigDecisionTool', tdt)
    kwargs.setdefault('InputChain', InputChain_PFlow)

    kwargs.setdefault('JM_PFlow_CNT' , acc.popToolsAndMerge(jetPFlowCNTManagerToolCfg(flags,
                                                                                      JetContainerName=InputJetContainer_PFlow)))

    kwargs.setdefault('JM_EMTopo_PRESEL', acc.popToolsAndMerge(jetEMTopoPRESELManagerToolCfg(flags,
                                                                                             JetContainerName=InputJetContainer_EMTopoPresel)))

    from TrigBjetHypo.TrigBjetBtagHypoTool import bTaggingWP
    from TrigBjetHypo.TrigBjetBtagHypoTool import bbTaggingWP

    working_points = { "newTagger" : 1.234 }
    working_points.update(bTaggingWP)
    working_points.update(bbTaggingWP)

    kwargs.setdefault('WorkingPoints', working_points)
    kwargs.setdefault('FTD_Remapping', 
                      {"DL1d20210519r22_pu" : "DL1dEMUL_pu",
                       "DL1d20210519r22_pc" : "DL1dEMUL_pc",
                       "DL1d20210519r22_pb" : "DL1dEMUL_pb"})

    acc.setPrivateTools(CompFactory.Trig.TrigBtagEmulationTool('TrigBtagEmulationTool', **kwargs))
    return acc

def TrigBtagValidationTestCfg(flags,
                              **kwargs) -> ComponentAccumulator:
    acc = ComponentAccumulator()
    histSvc = CompFactory.THistSvc()
    histSvc.Output += ["VALIDATION DATAFILE='validation.root' OPT='RECREATE'"]
    acc.addService(histSvc)

    options = {}
    options['EmulatedChains'] = [c.name for c in kwargs['toBeEmulatedTriggers']]

    options['TrigBtagEmulationTool'] = acc.popToolsAndMerge( TrigBtagEmulationToolCfg(flags,
                                                                                      **kwargs))

    from TrigDecisionTool.TrigDecisionToolConfig import TrigDecisionToolCfg
    tdt = acc.getPrimaryAndMerge(TrigDecisionToolCfg(flags))
    options['TrigDecisionTool'] = tdt

    acc.addEventAlgo( CompFactory.Trig.TrigBtagValidationTest('TrigBtagValidation', **options) )
    return acc

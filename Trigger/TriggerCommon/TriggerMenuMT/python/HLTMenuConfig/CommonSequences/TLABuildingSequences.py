#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, MenuSequence
from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTLAPhotonHypoAlg
from TrigMuonHypo.TrigMuonHypoConf import TrigMuonTLAHypoAlg
from TrigHLTJetHypo.TrigHLTJetHypoConf import TrigJetTLAHypoAlg
from AthenaCommon.CFElements import seqAND, findAlgorithm
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def addTLAStep(chain, chainDict):
    '''
    Add one extra chain step for TLA Activities
    '''

    signatures = chainDict['signatures'] # EG: ['Photon'] or ['Photon', 'Jet']
    tlaSequencesList = []
    print("addTLAStep: processing chain: ", chainDict['chainName'])
    for tlaSignature in signatures:
        
        print("addTLAStep: processing signature: ", tlaSignature)
        # call the sequence from their respective signatures
        tlaSequencesList.append( getTLASignatureSequence(tlaSignature) )

        # # the tlaImputMaker does not need to be RoI-based as we access features through previousDecisions
        # # which live in the full eventContext
        # tlaInputMaker = CompFactory.InputMakerForRoI("IMTLA_"+tlaSignature, RoIsLink="initialRoI")
        # tlaInputMaker.mergeUsingFeature = True

        # # the HypoAlg is where the object-copying is performed
        # # for the moment, each signature has a distinct hypo implementation
        # tlaHypoAlg = getTLAHypoAlg(tlaSignature)
        # tlaHypoTool = getTLAHypoTool(tlaSignature)

        # seq = MenuSequence(
        # 	Sequence    = tlaSequence(tlaSignature, inputMaker),								
        # 	Maker       = tlaInputMaker,
        # 	Hypo        = tlaHypoAlg, #not sure if this works
        # 	HypoToolGen = tlaHypoTool
        # 	)
        # tlaSequencesList.append(seq)	


            
    print("addTLAStep: About to add a step with: ", len(tlaSequencesList), "parallel sequences.")            
    # we add one step per TLA chain, with sequences matching the list of signatures
    # and multiplicities matching those of the previous step of the chain (already merged if combined)
    prevStep = chain.steps[-1]
    stepName = 'Step{:d}_merged{:d}_TLAStep_{:s}'.format(len(chain.steps)+1,len(prevStep.legIds), prevStep.name)
    step = ChainStep(name         = stepName,
                    Sequences     = tlaSequencesList,
                    multiplicity = prevStep.multiplicity,
                    chainDicts   = prevStep.stepDicts)	

    chain.steps.append(step)


def getTLASignatureSequence(tlaSignature):
    # Here we simply retrieve the TLA sequence from the existing signature code f
    
    if "Photon" in tlaSignature:
        from ..Photon.TLAPhotonMenuSequences import TLAPhotonMenuSequence
        photonOutCollectionName = "HLT_egamma_Photons"
        return TLAPhotonMenuSequence(ConfigFlags, photonOutCollectionName)
    
    elif "Muon" in tlaSignature:
        from ..Muon.TLAMuonSequence import TLAMuonMenuSequence
        return TLAMuonMenuSequence(ConfigFlags)
    
    elif "Jet" in tlaSignature:
        from ..Jet.JetTLASequences import TLAJetMenuSequence
        jetOutCollectionName = "HLT_AntiKt4EMTopoJets_subjesIS"
        return TLAJetMenuSequence(ConfigFlags, jetOutCollectionName)

def getTLAHypoAlg(tlaSignature):

    if "Photon" in tlaSignature:
        hypo = TrigEgammaTLAPhotonHypoAlg("TLAPhotonHypoAlg")
        hypo.TLAOutputName = "HLT_Egamma_Photons_TLA"
        return hypo

    elif "Muon" in tlaSignature:
        hypo = TrigMuonTLAHypoAlg("TLAMuonHypoAlg")
        hypo.TLAOutputName = "HLT_Muon_RoI_TLA"
        return hypo

    elif "Jet" in tlaSignature:
        hypo = TrigJetTLAHypoAlg("TLAJetHypoAlg")
        # how do we handle the case where the standard jet collection changes and we want this to be reflected in the TLA collectionName?
        jetCollectionName = "HLT_AntiKt4EMTopoJets_subjesIS"
        hypo.TLAOutputName = jetCollectionName+"_TLA" 
        return hypo

def getTLAHypoTool(tlaSignature):

    if "Photon" in tlaSignature:
        from TrigEgammaHypo.TrigEgammaTLAPhotonHypoTool import TrigEgammaTLAPhotonHypoToolFromDict
        hypoToolGen = TrigEgammaTLAPhotonHypoToolFromDict
        return hypoToolGen

    elif "Muon" in tlaSignature:
        from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFMSonlyHypoToolFromDict
        hypoToolGen = TrigMuonEFMSonlyHypoToolFromDict
        return hypoToolGen

    elif "Jet" in tlaSignature:
        from TrigHLTJetHypo.TrigJetHypoToolConfig import trigJetTLAHypoToolFromDict
        hypoToolGen = trigJetTLAHypoToolFromDict
        return hypoToolGen


def findTLAStep(chainConfig):
    tlaSteps = [s for s in chainConfig.steps if 'TLAStep' in s.name and 'EmptyPEBAlign' not in s.name and 'EmptyTLAAlign' not in s.name]
    if len(tlaSteps) == 0:
        return None
    elif len(tlaSteps) > 1:
        raise RuntimeError('Multiple TLA steps in one chain are not supported. ', len(tlaSteps), ' were found in chain ' + chainConfig.name)
    return tlaSteps[0]


def alignTLASteps(chain_configs, chain_dicts):

    def is_tla_dict(chainNameAndDict):
        return len([ chainNameAndDict[1]['eventBuildType'] == 'physicsTLA' ]) > 0

    all_tla_chain_dicts = dict(filter(is_tla_dict, chain_dicts.items()))
    all_tla_chain_names = list(all_tla_chain_dicts.keys())

    
    def is_tla_config(chainNameAndConfig):
        return chainNameAndConfig[0] in all_tla_chain_names

    all_tla_chain_configs = dict(filter(is_tla_config, chain_configs.items()))

    for chainName, chainConfig in all_tla_chain_configs.items():
        print("MARCO: Before alignment steps are: ", chainConfig.steps)

    maxTLAStepPosition = {} # {eventBuildType: N}

    def getTLAStepPosition(chainConfig):
        tlaStep = findTLAStep(chainConfig)
        return chainConfig.steps.index(tlaStep) + 1

    # First loop to find the maximal TLA step positions to which we need to align
    for chainName, chainConfig in all_tla_chain_configs.items():
        tlaStepPosition = getTLAStepPosition(chainConfig)
        ebt = all_tla_chain_dicts[chainName]['eventBuildType']
        if ebt not in maxTLAStepPosition or tlaStepPosition > maxTLAStepPosition[ebt]:
            maxTLAStepPosition[ebt] = tlaStepPosition

    # Second loop to insert empty steps before the TLA steps where needed
    for chainName, chainConfig in all_tla_chain_configs.items():
        tlaStepPosition = getTLAStepPosition(chainConfig)
        ebt = all_tla_chain_dicts[chainName]['eventBuildType']
        if tlaStepPosition < maxTLAStepPosition[ebt]:
            numStepsNeeded = maxTLAStepPosition[ebt] - tlaStepPosition
            log.debug('Aligning TLA step for chain %s by adding %d empty steps', chainName, numStepsNeeded)
            chainConfig.insertEmptySteps('EmptyTLAAlign', numStepsNeeded, tlaStepPosition-1)

    #post alignment printout
    for chainName, chainConfig in all_tla_chain_configs.items():
        print("MARCO: AFTER alignment steps are: ", chainConfig.steps)

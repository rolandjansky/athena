#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)


def addTLAStep(chain, chainDict):
    '''
    Add one extra chain step for TLA Activities
    '''

    signatures = chainDict['signatures'] # EG: ['Photon'] or ['Photon', 'Jet']
    tlaSequencesList = []
    log.debug("addTLAStep: processing chain: ", chainDict['chainName'])
    print("addTLAStep: processing chain: ", chainDict['chainName'])
    for tlaSignature in signatures:
        
        log.debug("addTLAStep: processing signature: ", tlaSignature)
        # call the sequence from their respective signatures
        tlaSequencesList.append( RecoFragmentsPool.retrieve(getTLASignatureSequence, ConfigFlags, tlaSignature=tlaSignature) )
        #tlaSequencesList.append(getTLASignatureSequence(ConfigFlags, tlaSignature=tlaSignature)),
            
    log.debug("addTLAStep: About to add a step with: ", len(tlaSequencesList), "parallel sequences.")            
    
    # we add one step per TLA chain, with sequences matching the list of signatures
    # and multiplicities matching those of the previous step of the chain (already merged if combined)
    prevStep = chain.steps[-1]
    stepName = 'Step{:d}_merged{:d}_TLAStep_{:s}'.format(len(chain.steps)+1,len(prevStep.legIds), prevStep.name)
    step = ChainStep(name         = stepName,
                    Sequences     = tlaSequencesList,
                    multiplicity = prevStep.multiplicity,
                    chainDicts   = prevStep.stepDicts)	

    chain.steps.append(step)


def getTLASignatureSequence(ConfigFlags, tlaSignature):
    # Here we simply retrieve the TLA sequence from the existing signature code f
    
    if "Photon" in tlaSignature:
        from ..Photon.TLAPhotonMenuSequences import TLAPhotonMenuSequence
        photonOutCollectionName = "HLT_egamma_Photons"
        return RecoFragmentsPool.retrieve(TLAPhotonMenuSequence, ConfigFlags, photonsIn=photonOutCollectionName)
       # return TLAPhotonMenuSequence(ConfigFlags, photonOutCollectionName)
        #return sequence
    
    elif "Muon" in tlaSignature:
        from ..Muon.TLAMuonSequence import TLAMuonMenuSequence
        muonOutCollectionName = "HLT_RoI_Muons"
        return RecoFragmentsPool.retrieve(TLAMuonMenuSequence, ConfigFlags, muonsIn=muonOutCollectionName)
        #return sequence
        #return TLAMuonMenuSequence(ConfigFlags), muonOutCollectionName
    
    elif "Jet" in tlaSignature:
        from ..Jet.JetTLASequences import TLAJetMenuSequence
        jetOutCollectionName = "HLT_AntiKt4EMTopoJets_subjesIS"
        return RecoFragmentsPool.retrieve(TLAJetMenuSequence, ConfigFlags, jetsIn=jetOutCollectionName)
        #return sequence
       # return TLAJetMenuSequence(ConfigFlags, jetOutCollectionName)


def findTLAStep(chainConfig):
    tlaSteps = [s for s in chainConfig.steps if 'TLAStep' in s.name and 'EmptyPEBAlign' not in s.name and 'EmptyTLAAlign' not in s.name and 'PEBInfoWriter' not in s.name]
    if len(tlaSteps) == 0:
        return None
    elif len(tlaSteps) > 1:
        print("Found multiple step in chain: ", chainConfig.name, "steps: ")
        print(chainConfig.steps)
        raise RuntimeError('Multiple TLA steps in one chain are not supported. ', len(tlaSteps), ' were found in chain ' + chainConfig.name)
    return tlaSteps[0]


def alignTLASteps(chain_configs, chain_dicts):

    def is_tla_dict(chainNameAndDict):
        return  chainNameAndDict[1]['eventBuildType'] == 'PhysicsTLA' 

    all_tla_chain_dicts = dict(filter(is_tla_dict, chain_dicts.items()))
    all_tla_chain_names = list(all_tla_chain_dicts.keys())

    
    def is_tla_config(chainNameAndConfig):
        return chainNameAndConfig[0] in all_tla_chain_names

    all_tla_chain_configs = dict(filter(is_tla_config, chain_configs.items()))

    # for chainName, chainConfig in all_tla_chain_configs.items():
    #     print("MARCO: Before alignment steps are: ", chainConfig.steps)

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
    # for chainName, chainConfig in all_tla_chain_configs.items():
    #     print("MARCO: AFTER alignment steps are: ", chainConfig.steps)

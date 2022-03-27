#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from TriggerMenuMT.HLT.Config.MenuComponents import ChainStep, RecoFragmentsPool
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.Logging import logging
from ..Jet.JetChainConfiguration import JetChainConfiguration
log = logging.getLogger(__name__)


def addTLAStep(chain, chainDict):
    '''
    Add one extra chain step for TLA Activities
    '''

    tlaSequencesList = []
    log.debug("addTLAStep: processing chain: %s", chainDict['chainName'])
    

    for cPart in chainDict['chainParts']:
        
        log.debug("addTLAStep: processing signature: %s", cPart['signature'] )
        # call the sequence from their respective signatures
        tlaSequencesList.append(getTLASignatureSequence(ConfigFlags, chainDict=chainDict, chainPart=cPart)), #signature=cPart['signature'])),
            
    log.debug("addTLAStep: About to add a step with: %d, parallel sequences.", len(tlaSequencesList))            
    
    # we add one step per TLA chain, with sequences matching the list of signatures
    # and multiplicities matching those of the previous step of the chain (already merged if combined)
    prevStep = chain.steps[-1]
    stepName = 'Step{:d}_merged{:d}_TLAStep_{:s}'.format(len(chain.steps)+1,len(prevStep.legIds), prevStep.name)
    step = ChainStep(name         = stepName,
                    Sequences     = tlaSequencesList,
                    multiplicity = prevStep.multiplicity,
                    chainDicts   = prevStep.stepDicts)	

    chain.steps.append(step)



def getTLASignatureSequence(ConfigFlags, chainDict, chainPart):
    # Here we simply retrieve the TLA sequence from the existing signature code 
    signature= chainPart['signature']
    
    if signature == 'Photon':    
        from ..Photon.PrecisionPhotonTLAMenuSequences import TLAPhotonMenuSequence
        photonOutCollectionName = "HLT_egamma_Photons"
        return RecoFragmentsPool.retrieve(TLAPhotonMenuSequence, ConfigFlags, photonsIn=photonOutCollectionName)
    
    elif signature == 'Muon':    
        from ..Muon.TLAMuonSequence import TLAMuonMenuSequence        
        return TLAMuonMenuSequence (ConfigFlags, muChainPart=chainPart)       
    
    elif signature  == 'Jet' or signature  == 'Bjet':   
        from ..Jet.JetTLASequences import TLAJetMenuSequence
        jetDef = JetChainConfiguration(chainDict)
        jetInputCollectionName = jetDef.jetName
        log.debug(f"TLA jet input collection = {jetInputCollectionName}")
        # Turn off b-tagging for jets that have no tracks anyway - we want to avoid 
        # adding a TLA AntiKt4EMTopoJets_subjetsIS BTagging container in the EDM.
        # We do not switch off BTag recording for Jet signatures as both Jet and Bjet signature
        # will use the same hypo alg, so it needs to be configured the same!
        # Thus, BTag recording will always run for PFlow jets, creating an empty container if no btagging exists. 
        attachBtag = True
        if jetDef.recoDict["trkopt"] == "notrk": attachBtag = False
        return RecoFragmentsPool.retrieve(TLAJetMenuSequence, ConfigFlags, jetsIn=jetInputCollectionName, attachBtag=attachBtag)


def findTLAStep(chainConfig):
    tlaSteps = [s for s in chainConfig.steps if 'TLAStep' in s.name and 'EmptyPEBAlign' not in s.name and 'EmptyTLAAlign' not in s.name and 'PEBInfoWriter' not in s.name]
    if len(tlaSteps) == 0:
        return None
    elif len(tlaSteps) > 1:
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


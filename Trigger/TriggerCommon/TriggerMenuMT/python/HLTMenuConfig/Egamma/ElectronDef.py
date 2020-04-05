# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import fastElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronSequenceSetup import precisionElectronMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( doRinger=True ):
    return fastCaloMenuSequence("Electron", doRinger)

def fastElectronSequenceCfg( flags ):
    return fastElectronMenuSequence()

def precisionCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Electron')

def precisionElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence()

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class ElectronChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------
        etcut1step          = [ self.getFastCalo() ]
        etcut_sequence      = [ self.getFastCalo(), self.getFastElectron(), self.getPrecisionCaloElectron()]
        electron_sequence   = [ self.getFastCalo(), self.getFastElectron(), self.getPrecisionCaloElectron(), self.getPrecisionElectron()]
        etcut_noringer_sequence      = [ self.getFastCalo(doRinger=False), self.getFastElectron(), self.getPrecisionCaloElectron()]
        electron_noringer_sequence   = [ self.getFastCalo(doRinger=False), self.getFastElectron(), self.getPrecisionCaloElectron(), self.getPrecisionElectron()]

        stepDictionary = {
                'etcut1step': etcut1step,
                'etcut'     : etcut_sequence,
                'lhloose'   : electron_sequence,
                'lhvloose'  : electron_sequence,
                'lhmedium'  : electron_sequence,
                'lhtight'   : electron_sequence,
                'etcutnoringer'     : etcut_noringer_sequence,
                'lhloosenoringer'   : electron_noringer_sequence,
                'lhvloosenoringer'  : electron_noringer_sequence,
                'lhmediumnoringer'  : electron_noringer_sequence,
                'lhtightnoringer'   : electron_noringer_sequence,
                }


        log.debug('electron chain part = ' + str(self.chainPart))
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['L2IDAlg'] + self.chainPart['isoInfo']
        for addInfo in self.chainPart['addInfo']:
            key+=addInfo
            
        log.debug('electron key = ' + key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for electron chain with key: " + key )

        chainSteps = []

        for step in steps:
            log.debug('Adding electron trigger step ' + str(step))
            chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain

    # --------------------
    # Configuration of electron steps
    # --------------------

    def getFastCalo(self, doRinger=True):
        stepName = "Step1_FastCalo_electron"
        log.debug("Configuring step " + stepName)
        fastCalo = RecoFragmentsPool.retrieve( electronFastCaloCfg, doRinger ) # the None will be used for flags in future
        return ChainStep(stepName, [fastCalo], [self.mult], [self.dict])

    def getFastElectron(self):
        stepName = "Step2_fast_electron"
        log.debug("Configuring step " + stepName)
        electronReco = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, None )
        return ChainStep(stepName, [electronReco], [self.mult], [self.dict])


    def getPrecisionCaloElectron(self):
        stepName = "Step3_precisionCalo_electron"
        log.debug("Configuring step " + stepName)
        precisionReco = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )
        return ChainStep(stepName, [precisionReco], [self.mult], [self.dict]) 


    def getPrecisionElectron(self):
        stepName = "Step4_precision_electron"
        log.debug("Configuring step " + stepName)
        precisionElectron = RecoFragmentsPool.retrieve( precisionElectronSequenceCfg, None )
        return ChainStep(stepName, [precisionElectron], [self.mult], [self.dict])

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import fastElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronSequenceSetup import precisionElectronMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------


def electronFastCaloCfg( flags ):
    return fastCaloMenuSequence("Electron", doRinger=False)

def electronFastCaloRingerCfg( flags ):
    return fastCaloMenuSequence("Electron", doRinger=True)

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
        etcut1step          = [ self.getFastCalo(doRinger=False) ]
        etcut_sequence      = [ self.getFastCalo(doRinger=False), self.getFastElectron(), self.getPrecisionCaloElectron()]
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
        if doRinger:
            stepName       = "FastCaloRinger_electron"
            fastCaloCfg    = electronFastCaloRingerCfg
        else:
            stepName       = "FastCalo_electron"
            fastCaloCfg = electronFastCaloCfg
        return self.getStep(1,stepName,[ fastCaloCfg])

    def getFastElectron(self):
        stepName = "fast_electron"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg])

    def getPrecisionCaloElectron(self):
        stepName = "precisionCalo_electron"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg])

    def getPrecisionElectron(self):
        stepName = "precision_electron"
        return self.getStep(4,stepName,[ precisionElectronSequenceCfg])


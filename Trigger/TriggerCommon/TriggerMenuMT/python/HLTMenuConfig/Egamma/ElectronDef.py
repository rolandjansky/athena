# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

# this must be moved to the HypoTool file:
def diElectronMassComboHypoToolFromDict(chainDict):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaDielectronMassHypoTool
    name = chainDict['chainName']
    tool= TrigEgammaDielectronMassHypoTool(name)
    tool.LowerMassElectronClusterCut = 50000
    tool.UpperMassElectronClusterCut = 130000
    return tool

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

        stepDictionary = {
                'etcut1step': ['getFastCaloNoRinger'],
                'etcut'     : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron'],
                'lhloose'   : ['getFastCaloRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhvloose'  : ['getFastCaloRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhmedium'  : ['getFastCaloRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtight'   : ['getFastCaloRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'etcutnoringer'     : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron'],
                'lhloosenoringer'   : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhvloosenoringer'  : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhmediumnoringer'  : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtightnoringer'   : ['getFastCaloNoRinger', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
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
        
        for step in steps:
            log.debug('Adding electron trigger step ' + str(step))
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps) 
        
        return myChain

    # --------------------
    # Configuration of electron steps
    # --------------------

    def getFastCaloRinger(self):
        stepName       = "FastCaloRinger_electron"
        fastCaloRingerCfg    = electronFastCaloRingerCfg
        return self.getStep(1,stepName,[ fastCaloRingerCfg ])

    def getFastCaloNoRinger(self):
        stepName       = "FastCalo_electron"
        fastCaloCfg    = electronFastCaloCfg
        return self.getStep(1,stepName,[ fastCaloCfg])

    def getFastElectron(self):
        stepName = "fast_electron"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg])

    def getPrecisionCaloElectron(self):
        stepName = "precisionCalo_electron"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg])

    def getPrecisionElectron(self):
        if "Zee" in self.chainName:
            stepName = "precision_topoelectron"
            return self.getStep(4,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron"
            return self.getStep(4,stepName,[ precisionElectronSequenceCfg])


# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import fastElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronSequenceSetup import precisionElectronMenuSequence

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( flags ):
    return fastCaloMenuSequence("Electron", doRinger=True)

def fastElectronSequenceCfg( flags ):
    return fastElectronMenuSequence()

def precisionCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Electron')

def precisionElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence()


# this must be moved to the HypoTool file:
def diElectronMassComboHypoToolFromDict(chainDict):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaMassHypoTool
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('MassOfAccepted', type='TH1F', path='EXPERT', title="Mass in accepted combinations [MeV]", xbins=75, xmin=0, xmax=150000)]
    tool= TrigEgammaMassHypoTool(name)
    tool.LowerMassElectronClusterCut = 50000
    tool.UpperMassElectronClusterCut = 130000
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
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
                'etcut1step': ['getFastCalo'],
                'etcut'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron'],
                'lhloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhvloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'etcutnoringer'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron'],
                'lhloosenoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhvloosenoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhmediumnoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtightnoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlooseivarloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlooseivarmedium' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlooseivartight'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlmediumivarloose' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlmediumivarmedium': ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhlmediumivartight' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtightivarloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtightivarmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
                'lhtightivartight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionElectron'],
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

    def getFastCalo(self):
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

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))

        if "Zee" in self.chainName:
            stepName = "precision_topoelectron"+isocut
            return self.getStep(4,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron"+isocut
            return self.getStep(4,stepName,[ precisionElectronSequenceCfg])


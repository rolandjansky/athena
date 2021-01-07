# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import fastElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronSequenceSetup import precisionElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionElectronSequenceSetup_GSF import precisionElectronMenuSequence_GSF
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionTrackingSequenceSetup import precisionTrackingMenuSequence

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( flags ):
    return fastCaloMenuSequence("Electron", doRinger=True)

def fastElectronSequenceCfg( flags ):
    return fastElectronMenuSequence(do_idperf=False)

def fastElectronSequenceCfg_idperf( flags ):
    return fastElectronMenuSequence(do_idperf=True)

def precisionCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Electron')

def precisionTrackingSequenceCfg( flags ):
    return precisionTrackingMenuSequence('Electron')

def precisionElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence()

def precisionGSFElectronSequenceCfg( flags ):
    return precisionElectronMenuSequence_GSF()


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
        log.debug("Assembling chain for %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------

        stepDictionary = {
                'etcut1step': ['getFastCalo'],
                'idperf'    : ['getFastCalo', 'getFastElectron_idperf', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'etcut'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'lhloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhvloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'etcutnoringer'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                'lhloosenoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhvloosenoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumnoringer'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightnoringer'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivarloose'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivarmedium' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhlooseivartight'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivarloose' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivarmedium': ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhmediumivartight' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivarloose'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivarmedium'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                'lhtightivartight'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
        # gsf sequences. For now just settin gup as normal non-gsf chains
                'lhloosegsf'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhvloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightgsf'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivarloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivarmediumgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhlooseivartightgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivarloosegsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivarmediumgsf': ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhmediumivartightgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivarloosegsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivarmediumgsf' : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                'lhtightivartightgsf'  : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],

                }

        log.debug('electron chain part = %s', self.chainPart)
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['L2IDAlg'] + self.chainPart['isoInfo'] + self.chainPart['trkInfo']


        for addInfo in self.chainPart['addInfo']:
            key+=addInfo

        log.debug('electron key = %s', key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for electron chain with key: " + key )
        
        for step in steps:
            log.debug('Adding electron trigger step %s', step)
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
   
    def getFastElectron_idperf(self):
        stepName = "fast_electron_idperf"
        return self.getStep(2,stepName,[ fastElectronSequenceCfg_idperf])

    def getPrecisionCaloElectron(self):
        stepName = "precisionCalo_electron"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg])

    def getPrecisionTracking(self):
        stepName = "precisionTracking_electron"
        return self.getStep(4,stepName,[ precisionTrackingSequenceCfg])

    def getPrecisionElectron(self):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = %s', isocut)

        if "Zee" in self.chainName:
            stepName = "precision_topoelectron"+isocut
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron"+isocut
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg])

    def getPrecisionGSFElectron(self):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))

        if "Zee" in self.chainName:
            stepName = "precision_topoelectron_GSF"+isocut
            return self.getStep(5,stepName,sequenceCfgArray=[precisionGSFElectronSequenceCfg], comboTools=[diElectronMassComboHypoToolFromDict])
        else:
            stepName = "precision_electron_GSF"+isocut
            return self.getStep(5,stepName,[ precisionGSFElectronSequenceCfg])

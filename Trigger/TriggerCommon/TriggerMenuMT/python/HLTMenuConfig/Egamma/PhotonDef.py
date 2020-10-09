#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonSequenceSetup import fastPhotonMenuSequence, precisionPhotonMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------
def fastPhotonCaloSequenceCfg( flags ):
    return fastCaloMenuSequence('Photon', doRinger=False)
    
def fastPhotonSequenceCfg( flags ):    
    return fastPhotonMenuSequence()

def precisionPhotonCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Photon')

def precisionPhotonSequenceCfg( flags ):
    return precisionPhotonMenuSequence('Photon')

def diphotonDPhiHypoToolFromDict(chainDict):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaDPhiHypoTool
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [defineHistogram('DphiOfAccepted', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2)]
    tool= TrigEgammaDPhiHypoTool(name)
    tool.ThresholdDPhiCut = 1.5
    monTool.HistPath = 'EgammaDphiHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool

#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class PhotonChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        log.debug("Assembling chain for " + self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration 
        # --------------------
        stepDictionary = {
            "etcut": ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton'],
            "etcutetcut": ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton'],
            "loose":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "medium":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "tight":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "looseicaloloose":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "mediumicaloloose":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "tighticaloloose":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "looseicalomedium":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "mediumicalomedium":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "tighticalomedium":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "looseicalotight":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "mediumicalotight":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
            "tighticalotight":  ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'], 
        }
        
        ## This needs to be configured by the Egamma Developer!!
        log.debug('photon chain part = ' + str(self.chainPart))
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['isoInfo']
        for addInfo in self.chainPart['addInfo']:
            key+=addInfo
            
        log.debug('photon key = ' + key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for photon chain with key: " + key )

        chainSteps = []

        for step in steps:
            log.debug('Adding photon trigger step ' + str(step))
            chainstep = getattr(self, step)()
            chainSteps+=[chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getFastCalo(self):
        stepName = "PhotonFastCalo"
        return self.getStep(1,stepName,[ fastPhotonCaloSequenceCfg])
        
    def getFastPhoton(self):
        stepName = "FastPhoton"
        return self.getStep(2,stepName,[ fastPhotonSequenceCfg])

    def getPrecisionCaloPhoton(self):
        stepName = "PhotonPrecisionCalo"
        return self.getStep(3,stepName,[ precisionPhotonCaloSequenceCfg])
            
    def getPrecisionPhoton(self):
        if "dPhi15" in self.chainName:
            stepName = "precision_topophoton"
            return self.getStep(4,stepName,sequenceCfgArray=[precisionPhotonSequenceCfg], comboTools=[diphotonDPhiHypoToolFromDict])
        else:
            stepName = "precision_photon"
            return self.getStep(4,stepName,[ precisionPhotonSequenceCfg])

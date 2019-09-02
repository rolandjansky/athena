# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonSequenceSetup import fastPhotonMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------
def fastPhotonCaloSequenceCfg( flags ):
    return fastCaloMenuSequence('Photon')
    
def fastPhotonSequenceCfg( flags ):    
    return fastPhotonMenuSequence()

def precisionPhotonCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Photon')

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
            "": [self.getFastCalo(), self.getFastPhoton(), self.getPrecisionCaloPhoton()]
        }
        
        ## This needs to be configured by the Egamma Developer!!
        log.debug('photon chain part = ' + str(self.chainPart))
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['isoInfo']
        log.debug('photon key = ' + key)
        steps=stepDictionary[key]

        chainSteps = []
        for step in steps:
            log.debug('Adding photon trigger step ' + str(step))
            chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getFastCalo(self):
        stepName = "Step1_PhotonFastCalo"
        log.debug("Configuring step " + stepName)
        fastCalo = RecoFragmentsPool.retrieve( fastPhotonCaloSequenceCfg, None ) # the None will be used for flags in future
        return ChainStep(stepName, [fastCalo])
        
    def getFastPhoton(self):
        stepName = "Step2_L2Photon"
        log.debug("Configuring step " + stepName)
        photonReco = RecoFragmentsPool.retrieve( fastPhotonSequenceCfg, None )
        return ChainStep(stepName, [photonReco])

    def getPrecisionCaloPhoton(self):
        stepName = "Step3_PhotonPrecisionCalo"
        log.debug("Configuring step " + stepName)
        precisionCaloPhoton = RecoFragmentsPool.retrieve( precisionPhotonCaloSequenceCfg, None )
        return ChainStep(stepName, [precisionCaloPhoton])

            
            

        
                

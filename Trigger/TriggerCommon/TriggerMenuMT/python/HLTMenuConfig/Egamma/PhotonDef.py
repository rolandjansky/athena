# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonSequenceSetup import photonMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO, 
# so let's make them functions already now
#----------------------------------------------------------------
def gammaFastCaloCfg( flags ):
    return fastCaloMenuSequence("GammaFastCalo")
    
def photonSequenceCfg( flags ):    
    return photonMenuSequence()

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
            "": [self.getFastCalo(), self.getPhoton()]
        }
        
        ## This needs to be configured by the Egamma Developer!!
        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['isoInfo'] 
        steps=stepDictionary[key]

        chainSteps = []
        for step in steps:
            chainSteps+=[step]
    
        myChain = self.buildChain(chainSteps)
        return myChain
        

    # --------------------
    # Configuration of steps
    # --------------------
    def getFastCalo(self):
        stepName = "Step1_g5_etcut"
        log.debug("Configuring step " + stepName)
        fastCalo = RecoFragmentsPool.retrieve( gammaFastCaloCfg, None ) # the None will be used for flags in future
        return ChainStep(stepName, [fastCalo])
        
    def getPhoton(self):
        stepName = "Step2_g5_etcut"
        log.debug("Configuring step " + stepName)
        photonReco = RecoFragmentsPool.retrieve( photonSequenceCfg, None )
        return ChainStep(stepName, [photonReco])

            
            

        
                

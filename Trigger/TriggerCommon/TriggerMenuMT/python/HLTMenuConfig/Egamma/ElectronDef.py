# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef")


from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep, RecoFragmentsPool

from TriggerMenuMT.HLTMenuConfig.CommonSequences.CaloSequenceSetup import fastCaloMenuSequence

from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronSequenceSetup import fastElectronMenuSequence
from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuSequence

#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( flags ):
    return fastCaloMenuSequence("Electron")

def fastElectronSequenceCfg( flags ):
    return fastElectronMenuSequence()

def precisionCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Electron')

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
        myStepNames = []
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)
        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------
        if 'etcut1step' in self.chainPart['addInfo']:
            myStepNames += ["Step1_electron"]
            for step in myStepNames:
                chainSteps += [self.getElectronStep(step)]
        elif 'etcut' in self.chainPart['addInfo']:
            myStepNames += ["Step1_electron"]
            myStepNames += ["Step2_electron"]
            myStepNames += ["Step3_electron"]
            for step in myStepNames:
                chainSteps += [self.getElectronStep(step)]
        else:
            raise RuntimeError("Chain configuration unknown for chain: " + self.chainName )

        myChain = self.buildChain(chainSteps)
        return myChain

    # --------------------
    # Configuration of electron chain
    # --------------------
    def getElectronStep(self, stepName):
        if stepName == "Step1_electron":
          log.debug("Configuring step " + stepName)
          fastCalo = RecoFragmentsPool.retrieve( electronFastCaloCfg, None ) # the None will be used for flags in future
          chainStep =ChainStep(stepName, [fastCalo], [self.mult])
        elif stepName == "Step2_electron":
          log.debug("Configuring step " + stepName)
          electronReco = RecoFragmentsPool.retrieve( fastElectronSequenceCfg, None )
          chainStep=ChainStep(stepName, [electronReco], [self.mult])
        elif stepName == "Step3_electron":
          log.debug("Configuring step " + stepName)
          precisionReco = RecoFragmentsPool.retrieve( precisionCaloSequenceCfg, None )
          chainStep=ChainStep(stepName, [precisionReco], [self.mult])
        else:
          raise RuntimeError("chainStepName unknown: " + stepName )

        log.debug("Returning chainStep from getEtCutStep function: " + stepName)
        return chainStep

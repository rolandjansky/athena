#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)
from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from ..CommonSequences.CaloSequences import fastCaloMenuSequence
from ..Photon.FastPhotonMenuSequences import fastPhotonMenuSequence
from ..Photon.PrecisionPhotonMenuSequences import precisionPhotonMenuSequence
from ..Photon.PrecisionCaloMenuSequences import precisionCaloMenuSequence
from ..Photon.HipTRTMenuSequences import hipTRTMenuSequence
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTopoHypoTool



from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
def fastPhotonCaloSequenceCfg( flags ):
    return fastCaloMenuSequence(flags, 'Photon', doRinger=False)
    
def fastPhotonSequenceCfg( flags ):    
    return fastPhotonMenuSequence( flags )

def precisionPhotonCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Photon')

def precisionPhotonCaloSequenceCfg_ion( flags ):
    return precisionCaloMenuSequence('Photon', ion=True)

def precisionPhotonSequenceCfg( flags ):
    return precisionPhotonMenuSequence('Photon')

def precisionPhotonSequenceCfg_ion( flags ):
    return precisionPhotonMenuSequence('Photon', ion=True)

def hipTRTMenuSequenceCfg( flags ):
    return hipTRTMenuSequence()


def _diPhotonComboHypoToolFromDict(chainDict, lowermass=80000,uppermass=-999,dphi=1.5,applymass=False,applydphi=False): 
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [
        defineHistogram('DphiOfAccepted', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
        defineHistogram('MassOfAccepted', type='TH1F', path='EXPERT', title="Mass in accepted combinations [MeV]", xbins=75, xmin=0, xmax=150000)
    ]
    tool= TrigEgammaTopoHypoTool(name)
    tool.AcceptAll = False
    tool.ApplyMassCut = applymass
    tool.LowerMassEgammaClusterCut = lowermass
    tool.UpperMassEgammaClusterCut = uppermass
    tool.ApplyDPhiCut = applydphi
    tool.ThresholdDPhiCut = dphi
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool

def diphotonDPhiHypoToolFromDict(chainDict):
    return _diPhotonComboHypoToolFromDict(chainDict,lowermass=80000,uppermass=-999,dphi=1.5,applymass=False,applydphi=True)

def diphotonDPhiMassHypoToolFromDict(chainDict):
    return _diPhotonComboHypoToolFromDict(chainDict,lowermass=80000,uppermass=-999,dphi=1.5,applymass=True,applydphi=True)


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class PhotonChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        self.chainDict = chainDict
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChainImpl(self):
        log.debug("Assembling chain for %s", self.chainName)

        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # --------------------
        stepDictionary = {
            "etcut"  : ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton'],
            "hiptrt" : ['getFastCalo', 'getHipTRT'], # hipTRT sequence 
            "nominal": ['getFastCalo', 'getFastPhoton', 'getPrecisionCaloPhoton', 'getPrecisionPhoton'],
        }

        ## This needs to be configured by the Egamma Developer!!
        log.debug('photon chain part = %s', self.chainPart)
        
        key = "nominal"
        if "etcut" in self.chainPart['IDinfo']:
            key = "etcut"
        if self.chainPart['extra']=="hiptrt":
            key	= "hiptrt"

        log.debug('photon key = %s', key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for photon chain with key: " + key )

        chainSteps = []

        for step in steps:
            log.debug('Adding photon trigger step %s', step)
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
        if self.chainPart['extra'] == 'ion':
            stepName = "PhotonPrecisionHICalo"
            return self.getStep(3,stepName, [precisionPhotonCaloSequenceCfg_ion])

        stepName = "PhotonPrecisionCalo"
        return self.getStep(3,stepName,[ precisionPhotonCaloSequenceCfg])
    
    def getHipTRT(self):
        stepName = "hipTRT"
        return self.getStep(2,stepName,[ hipTRTMenuSequenceCfg])

    def getPrecisionPhoton(self):
        
        if "dPhi15" in self.chainDict["topo"]:
            if "m80" in self.chainDict["topo"]:
                stepName = "precision_photon_dPhi15_m80"
                return self.getStep(4,stepName,sequenceCfgArray=[precisionPhotonSequenceCfg], comboTools=[diphotonDPhiMassHypoToolFromDict])
            else:
                stepName = "precision_photon_dPhi15"
                return self.getStep(4,stepName,sequenceCfgArray=[precisionPhotonSequenceCfg], comboTools=[diphotonDPhiHypoToolFromDict])
        else:
            if self.chainPart['extra'] == 'ion':
                stepName = "precision_photon_ion"
                return self.getStep(4,stepName, [precisionPhotonSequenceCfg_ion])

            stepName = "precision_photon"
            return self.getStep(4,stepName,[ precisionPhotonSequenceCfg])
    

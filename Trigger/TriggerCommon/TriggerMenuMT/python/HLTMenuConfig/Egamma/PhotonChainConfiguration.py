#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


from ..Menu.ChainConfigurationBase import ChainConfigurationBase

from ..CommonSequences.CaloSequences import fastCaloMenuSequence
from .FastPhotonMenuSequences import fastPhotonMenuSequence
from .PrecisionPhotonMenuSequences import precisionPhotonMenuSequence
from .PrecisionCaloMenuSequences import precisionCaloMenuSequence
from .TLAPhotonMenuSequences import TLAPhotonMenuSequence
from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTopoHypoTool


from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------
def fastPhotonCaloSequenceCfg( flags ):
    return fastCaloMenuSequence('Photon', doRinger=False)
    
def fastPhotonSequenceCfg( flags ):    
    return fastPhotonMenuSequence()

def TLAPhotonSequenceCfg(flags,  HLT_threshold ):
    photonsIn = "HLT_egamma_Photons"
    return TLAPhotonMenuSequence(flags, photonsIn, HLT_threshold=HLT_threshold)

def precisionPhotonCaloSequenceCfg( flags ):
    return precisionCaloMenuSequence('Photon')

def precisionPhotonSequenceCfg( flags ):
    return precisionPhotonMenuSequence('Photon')

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

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):
        log.debug("Assembling chain for %s", self.chainName)

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
        log.debug('photon chain part = %s', self.chainPart)
        addInfo = 'etcut'

        key = self.chainPart['extra'] + self.chainPart['IDinfo'] + self.chainPart['isoInfo']
        for addInfo in self.chainPart['addInfo']:
            key+=addInfo

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


 
        if self.dict["eventBuildType"] == "PhotonDS" :
            log.debug('Adding photon trigger step getTLAPhoton')
            TLAStep = self.getTLAPhoton()
            chainSteps+= [TLAStep]




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

    def getTLAPhoton(self):
        stepName = "TLAPhoton"
        HLT_threshold = 0
       
        for cPart in self.dict['chainParts']:
            if 'Photon' in cPart['signature']:
                HLT_threshold = float(cPart['threshold'])
            
        #print("MARCOLOG ", HLT_threshold)    
        return self.getStep(5, stepName, [TLAPhotonSequenceCfg],  HLT_threshold=HLT_threshold)

    def getPrecisionCaloPhoton(self):
        stepName = "PhotonPrecisionCalo"
        return self.getStep(3,stepName,[ precisionPhotonCaloSequenceCfg])

    def getPrecisionPhoton(self):
   
        if "dPhi15" in self.chainName and "m80" not in self.chainName:
            stepName = "precision_photon_dPhi15"
            return self.getStep(4,stepName,sequenceCfgArray=[precisionPhotonSequenceCfg], comboTools=[diphotonDPhiHypoToolFromDict])
        elif "m80" in self.chainName and "dPhi15" in self.chainName:
            stepName = "precision_photon_dPhi15_m80"
            return self.getStep(4,stepName,sequenceCfgArray=[precisionPhotonSequenceCfg], comboTools=[diphotonDPhiMassHypoToolFromDict])
        else:
            stepName = "precision_photon"
            return self.getStep(4,stepName,[ precisionPhotonSequenceCfg])
    

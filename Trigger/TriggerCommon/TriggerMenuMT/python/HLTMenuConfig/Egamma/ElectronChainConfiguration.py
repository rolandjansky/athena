
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)


from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from ..CommonSequences.CaloSequences import fastCaloMenuSequence
from ..CommonSequences.CaloSequences_FWD import fastCaloMenuSequence_FWD


from .FastElectronMenuSequences import fastElectronMenuSequence
from .FastElectronMenuSequences_LRT import fastElectronMenuSequence_LRT
from .PrecisionCaloMenuSequences import precisionCaloMenuSequence
from .PrecisionCaloMenuSequences_LRT import precisionCaloMenuSequence_LRT
from .PrecisionCaloMenuSequences_FWD import precisionCaloMenuSequence_FWD
from .PrecisionElectronMenuSequences import precisionElectronMenuSequence
from .PrecisionElectronMenuSequences_GSF import precisionElectronMenuSequence_GSF
from .PrecisionElectronMenuSequences_LRT import precisionElectronMenuSequence_LRT
from .PrecisionTrackingMenuSequences import precisionTrackingMenuSequence
from .PrecisionTrackingMenuSequences_LRT import precisionTrackingMenuSequence_LRT

from TrigBphysHypo.TrigMultiTrkComboHypoConfig import StreamerDiElecFastComboHypoCfg, DiElecPrecisionComboHypoCfg, TrigMultiTrkComboHypoToolFromDict

from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
#----------------------------------------------------------------
# fragments generating configuration will be functions in New JO,
# so let's make them functions already now
#----------------------------------------------------------------

def electronFastCaloCfg( flags, is_probe_leg=False ):
    return fastCaloMenuSequence("Electron", is_probe_leg=is_probe_leg)

def fastElectronSequenceCfg( flags, is_probe_leg=False ):
    return fastElectronMenuSequence(do_idperf=False, is_probe_leg=is_probe_leg)

def fastElectronSequenceCfg_idperf( flags, is_probe_leg=False ):
    return fastElectronMenuSequence(do_idperf=True, is_probe_leg=is_probe_leg)

def fastElectronSequenceCfg_lrt( flags, is_probe_leg=False ):
    return fastElectronMenuSequence_LRT(do_idperf=False, is_probe_leg=is_probe_leg)

def fastElectronSequenceCfg_lrt_idperf( flags, is_probe_leg=False ):
    return fastElectronMenuSequence_LRT(do_idperf=True, is_probe_leg=is_probe_leg)

def precisionCaloSequenceCfg( flags, is_probe_leg=False ):
    return precisionCaloMenuSequence('Electron', is_probe_leg=is_probe_leg)

def precisionCaloSequenceCfg_lrt( flags, is_probe_leg=False ):
    return precisionCaloMenuSequence_LRT('Electron', is_probe_leg=is_probe_leg)

def precisionCaloSequenceCfg_ion( flags, is_probe_leg=False ):
    return precisionCaloMenuSequence('Electron', is_probe_leg=is_probe_leg, ion=True)

def precisionTrackingSequenceCfg( flags, is_probe_leg=False ):
    return precisionTrackingMenuSequence('Electron', is_probe_leg=is_probe_leg)

def precisionTrackingSequenceCfg_ion( flags, is_probe_leg=False ):
    return precisionTrackingMenuSequence('Electron', is_probe_leg=is_probe_leg, ion=True)

def precisionTrackingSequenceCfg_lrt( flags, is_probe_leg=False ):
    return precisionTrackingMenuSequence_LRT('Electron', is_probe_leg=is_probe_leg)

def precisionElectronSequenceCfg( flags, is_probe_leg=False ):
    return precisionElectronMenuSequence(is_probe_leg=is_probe_leg, do_idperf = False)

def precisionElectronSequenceCfg_idperf( flags, is_probe_leg=False ):
    return precisionElectronMenuSequence(is_probe_leg=is_probe_leg, do_idperf = True)

def precisionElectronSequenceCfg_ion( flags, is_probe_leg=False ):
    return precisionElectronMenuSequence(is_probe_leg=is_probe_leg, ion=True, do_idperf = False)

def precisionGSFElectronSequenceCfg( flags, is_probe_leg=False):
    return precisionElectronMenuSequence_GSF(is_probe_leg=is_probe_leg, do_idperf = False)

def precisionGSFElectronSequenceCfg_idperf( flags, is_probe_leg=False):
    return precisionElectronMenuSequence_GSF(is_probe_leg=is_probe_leg, do_idperf = True)

def precisionElectronSequenceCfg_lrt( flags, is_probe_leg=False):
    return precisionElectronMenuSequence_LRT(is_probe_leg=is_probe_leg, do_idperf = False)

def precisionElectronSequenceCfg_lrt_idperf( flags, is_probe_leg=False ):
    return precisionElectronMenuSequence_LRT(is_probe_leg=is_probe_leg, do_idperf = True)

# this must be moved to the HypoTool file:

from TrigEgammaHypo.TrigEgammaHypoConf import TrigEgammaTopoHypoTool


def _diElectronMassComboHypoToolFromDict(chainDict, mass_range): 
    name = chainDict['chainName']
    monTool = GenericMonitoringTool("MonTool_"+name)
    monTool.Histograms = [
        defineHistogram('DphiOfAccepted', type='TH1F', path='EXPERT', title="PrecisionCalo Hypo entries per Phi;Phi", xbins=128, xmin=-3.2, xmax=3.2),
        defineHistogram('MassOfAccepted', type='TH1F', path='EXPERT', title="Mass in accepted combinations [MeV]", xbins=75, xmin=0, xmax=150000)
    ]
    tool= TrigEgammaTopoHypoTool(name)
    tool.AcceptAll = False
    tool.ApplyMassCut = True
    tool.LowerMassEgammaClusterCut = mass_range[0]
    tool.UpperMassEgammaClusterCut = mass_range[1]
    monTool.HistPath = 'EgammaMassHypo/'+tool.getName()
    tool.MonTool = monTool
    return tool


def diElectronZeeMassComboHypoToolFromDict(chainDict):
    return _diElectronMassComboHypoToolFromDict(chainDict, (50000, 130000))

def diElectronJpsieeMassComboHypoToolFromDict(chainDict):
    return _diElectronMassComboHypoToolFromDict(chainDict, (1000, 5000))

def diEgammaHegMassComboHypoToolFromDict(chainDict):
    return _diElectronMassComboHypoToolFromDict(chainDict, (90000, 1400000))

def electronFastCaloCfg_fwd( flags, is_probe_leg=False ):
    return fastCaloMenuSequence_FWD("Electron", is_probe_leg=is_probe_leg)

def precisionCaloSequenceCfg_fwd( flags, is_probe_leg=False ):
    return precisionCaloMenuSequence_FWD('Electron', is_probe_leg=is_probe_leg)


#----------------------------------------------------------------
# Class to configure chain
#----------------------------------------------------------------
class ElectronChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
        self.chainDict = chainDict
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
          
                'etcut'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking'],
                
                # nominal and nominal-idperf
                'nominal'     : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionElectron'],
                
                # gsf and gsf-idperf
                'nominalgsf'   : ['getFastCalo', 'getFastElectron', 'getPrecisionCaloElectron', 'getPrecisionTracking', 'getPrecisionGSFElectron'],
                
                # lrt and lrt-idperf
                'nominallrt'  : ['getFastCalo', 'getFastElectron_lrt', 'getPrecisionCaloElectron_lrt', 'getPrecisionTracking_lrt', 'getPrecisionElectron_lrt'],  

                # fwd sequences
                'etcutfwd' : ['getFastCalo_fwd', 'getPrecisionCaloElectron_fwd']
                }

        log.debug('electron chain part = %s', self.chainPart)
        key = "nominal"

        
        if self.chainPart['addInfo']:
            if "etcut1step" in self.chainPart['addInfo']:
                key = "etcut1step"
            elif "etcut" in self.chainPart['addInfo'] and "fwd" in self.chainPart['addInfo']:
                key = "etcutfwd"
            else:
                key = "etcut"
        
        if "gsf" in self.chainPart['gsfInfo']:
            key = "nominalgsf"

        if self.chainPart['lrtInfo']:  
            key = "nominallrt"

        log.debug('electron key = %s', key)
        if key in stepDictionary:
            steps=stepDictionary[key]
        else:
            raise RuntimeError("Chain configuration unknown for electron chain with key: " + key )
        
        for step in steps:
            log.debug('Adding electron trigger step %s', step)
            is_probe_leg = self.chainPart['extra']=='probe'
            chainstep = getattr(self, step)(is_probe_leg=is_probe_leg)
            chainSteps+=[chainstep]

        myChain = self.buildChain(chainSteps) 
        
        return myChain

    # -------------------------------
    # Configuration of electron steps
    # -------------------------------

    def getFastCalo(self,is_probe_leg=False):
        stepName       = "FastCalo_electron"
        fastCaloCfg    = electronFastCaloCfg
        return self.getStep(1,stepName,[ fastCaloCfg], is_probe_leg=is_probe_leg)

    def getFastElectron(self,is_probe_leg=False):
        if "bBeeM6000" in self.chainDict['topo']:
            stepName = "fast_electron_bBee"
            return self.getStep(2,stepName,sequenceCfgArray=[fastElectronSequenceCfg], comboHypoCfg=StreamerDiElecFastComboHypoCfg)
        elif 'idperf' in self.chainPart['idperfInfo']:
            stepName = "fast_electron_idperf"
            return self.getStep(2,stepName,[ fastElectronSequenceCfg_idperf], is_probe_leg=is_probe_leg)
        else:
            stepName = "fast_electron"
            return self.getStep(2,stepName,[ fastElectronSequenceCfg],is_probe_leg=is_probe_leg)

    def getFastElectron_lrt(self,is_probe_leg=False):
        if 'idperf' in self.chainPart['idperfInfo']:
            stepName = "fast_electron_lrt_idperf"
            return self.getStep(2,stepName,[ fastElectronSequenceCfg_lrt_idperf],is_probe_leg=is_probe_leg)
        else:
            stepName = "fast_electron_lrt"
            return self.getStep(2,stepName,[ fastElectronSequenceCfg_lrt],is_probe_leg=is_probe_leg)

    def getPrecisionCaloElectron(self,is_probe_leg=False):
        if self.chainPart['extra'] == 'ion':
            stepName = 'precisionCalo_ion_electron'
            return self.getStep(3, stepName, [precisionCaloSequenceCfg_ion], is_probe_leg=is_probe_leg)

        stepName = "precisionCalo_electron"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg], is_probe_leg=is_probe_leg)
    
    def getPrecisionCaloElectron_lrt(self,is_probe_leg=False):
        stepName = "precisionCalo_electron_lrt"
        return self.getStep(3,stepName,[ precisionCaloSequenceCfg_lrt],is_probe_leg=False)

    def getPrecisionTracking(self,is_probe_leg=False):
        if self.chainPart['extra'] == 'ion':
            stepName = 'precisionTracking_ion_electron'
            return self.getStep(4, stepName, [precisionTrackingSequenceCfg_ion], is_probe_leg=is_probe_leg)

        stepName = "precisionTracking_electron"
        return self.getStep(4,stepName,[ precisionTrackingSequenceCfg], is_probe_leg=is_probe_leg)

    def getPrecisionTracking_lrt(self,is_probe_leg=False):
        stepName = "precisionTracking_electron_lrt"
        return self.getStep(4,stepName,[ precisionTrackingSequenceCfg_lrt],is_probe_leg=is_probe_leg)

    def getPrecisionElectron(self,is_probe_leg=False):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = %s', isocut)
         
        if "Zee" in self.chainDict['topo']:
            stepName = "precision_electron_Zee"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronZeeMassComboHypoToolFromDict]) # Needs probe leg option too?
        elif "Jpsiee" in self.chainDict['topo']:
            stepName = "precision_topoelectron_Jpsiee"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diElectronJpsieeMassComboHypoToolFromDict]) # Needs probe leg option too?
        elif "Heg" in  self.chainDict['topo']:
            stepName = "precision_electron_Heg"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboTools=[diEgammaHegMassComboHypoToolFromDict])
        elif "bBeeM6000" in  self.chainDict['topo']:
            stepName = "precision_electron_bBee"+isocut
            return self.getStep(5,stepName,sequenceCfgArray=[precisionElectronSequenceCfg], comboHypoCfg=DiElecPrecisionComboHypoCfg, comboTools=[TrigMultiTrkComboHypoToolFromDict])
        elif self.chainPart['extra'] == 'ion':
            stepName = "precision_ion_electron" + str(isocut)
            return self.getStep(5,stepName,[precisionElectronSequenceCfg_ion], is_probe_leg=is_probe_leg)
        elif "idperf" in self.chainPart['idperfInfo']:
            stepName = "precision_electron_idperf"+str(isocut)
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg_idperf], is_probe_leg=is_probe_leg)
        else:
            stepName = "precision_electron_nominal"+str(isocut)
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg ], is_probe_leg=is_probe_leg)     

    def getPrecisionGSFElectron(self,is_probe_leg=False):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))
       
        if "Zee" in  self.chainDict['topo']:
            stepName = "precision_topoelectron_Zee_GSF"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionGSFElectronSequenceCfg], comboTools=[diElectronZeeMassComboHypoToolFromDict]) # Needs probe leg option too?
        elif "Jpsiee" in  self.chainDict['topo']:
            stepName = "precision_topoelectron_Jpsiee_GSF"+str(isocut)
            return self.getStep(5,stepName,sequenceCfgArray=[precisionGSFElectronSequenceCfg], comboTools=[diElectronJpsieeMassComboHypoToolFromDict]) # Needs probe leg option too?
        elif "idperf" in self.chainPart['idperfInfo']:
            stepName = "precision_electron_GSF_idperf"+str(isocut)
            return self.getStep(5,stepName,[ precisionGSFElectronSequenceCfg_idperf], is_probe_leg=is_probe_leg)
        else:
            stepName = "precision_electron_GSF"+str(isocut)
            return self.getStep(5,stepName,[ precisionGSFElectronSequenceCfg], is_probe_leg=is_probe_leg)

    def getPrecisionElectron_lrt(self,is_probe_leg=False):

        isocut = self.chainPart['isoInfo']
        log.debug(' isolation cut = ' + str(isocut))

        if 'idperf' in self.chainPart['idperfInfo']:
            stepName = "precision_electron_lrt_idperf"+str(isocut)
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg_lrt_idperf],is_probe_leg=is_probe_leg)
        else:
            stepName = "precision_electron_lrt"+str(isocut)
            return self.getStep(5,stepName,[ precisionElectronSequenceCfg_lrt],is_probe_leg=is_probe_leg)

    def getFastCalo_fwd(self,is_probe_leg=False):
        stepName       = "FastCalo_FWD_electron"
        return self.getStep(1, stepName, [electronFastCaloCfg_fwd], is_probe_leg=is_probe_leg)

    def getPrecisionCaloElectron_fwd(self,is_probe_leg=False):
        stepName = "precisionCalo_FWD_electron"
        return self.getStep(2,stepName,[ precisionCaloSequenceCfg_fwd], is_probe_leg=is_probe_leg)

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Muon.MuonDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase

from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muFastOvlpRmSequence, muCombSequence, muCombOvlpRmSequence, muEFMSSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence, muEFIsoSequence, efLateMuRoISequence, efLateMuSequence
from TrigMuonHypoMT.TrigMuonHypoMTConfig import TrigMuonEFInvMassHypoToolFromDict

# this must be moved to the HypoTool file:
def dimuDrComboHypoToolFromDict(chainDict):
    from DecisionHandling.DecisionHandlingConf import DeltaRRoIComboHypoTool
    name = chainDict['chainName']
    tool= DeltaRRoIComboHypoTool(name)
    tool.DRcut=3.
    return tool


#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def muFastSequenceCfg(flags):
    return muFastSequence()

def muFastOvlpRmSequenceCfg(flags):
    return muFastOvlpRmSequence()

def muCombSequenceCfg(flags):
    return muCombSequence()

def muCombOvlpRmSequenceCfg(flags):
    return muCombOvlpRmSequence()

def muEFMSSequenceCfg(flags):
    return muEFMSSequence()

def muEFSASequenceCfg(flags):
    return muEFSASequence()

def muIsoSequenceCfg(flags):
    return muIsoSequence()

def muEFCBSequenceCfg(flags):
    return muEFCBSequence()

def FSmuEFSASequenceCfg(flags):
    return muEFSAFSSequence()

def FSmuEFCBSequenceCfg(flags):
    return muEFCBFSSequence()

def muEFIsoSequenceCfg(flags):
    return muEFIsoSequence()

def muEFLateRoISequenceCfg(flags):
    return efLateMuRoISequence()

def muEFLateSequenceCfg(flags):
    return efLateMuSequence()


############################################# 
###  Class/function to configure muon chains 
#############################################

class MuonChainConfiguration(ChainConfigurationBase):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)

    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleChain(self):                            
        chainSteps = []

        stepDictionary = self.getStepDictionary()

        key = self.chainPart['extra']+self.chainPart['isoInfo']

        steps=stepDictionary[key]

        for step_level in steps:
            for step in step_level:
                chainstep = getattr(self, step)()
                chainSteps+=[chainstep]
    
        myChain = self.buildChain(chainSteps)
        return myChain

    def getStepDictionary(self):


        # --------------------
        # define here the names of the steps and obtain the chainStep configuration
        # each value is a list [ L2, EF ] where L2 = [list of L2 steps] and EF = [ EF steps]
        # this way, Bphys (or others) can insert steps at the end of L2 and end of EF after
        # the muon steps are defined
        # note that bphys chains are by default noL2Comb, even though this is not in the name
        # --------------------

        stepDictionary = {
            "":[['getmuFast', 'getmuComb'], ['getmuEFSA', 'getmuEFCB']],
            "fast":[['getmuFast']],
            "Comb":[['getmuFast', 'getmuComb']],
            "noL2Comb" : [['getmuFast'], ['getmuEFSA', 'getmuEFCB']],
            "ivar":[['getmuFast', 'getmuComb', 'getmuIso']],
            "noL1":[[],['getFSmuEFSA', 'getFSmuEFCB']],
            "msonly":[['getmuFast', 'getmuMSEmpty'], ['getmuEFMS']],
            "ivarmedium":[['getmuFast', 'getmuComb'], ['getmuEFSA', 'getmuEFCB', 'getmuEFIso']],
            "lateMu":[[],['getLateMuRoI','getLateMu']],
            "Dr": [['getmuFastDr', 'getmuCombDr']],
            "muoncalib":[['getmuFast']]
        }

        return stepDictionary
  
        
    # --------------------
    def getmuFast(self):
        doOvlpRm = False
        if "bTau" in self.chainName or "bJpsi" in self.chainName or "bUpsi" in self.chainName or "bDimu" in self.chainName or "bBmu" in self.chainName:
           doOvlpRm = False
        elif self.mult>1:
           doOvlpRm = True
        elif len( self.dict['signatures'] )>1 and not self.chainPart['extra']:
           doOvlpRm = True
        else:
           doOvlpRm = False

        if doOvlpRm:
           return self.getStep(1,"mufast", [muFastOvlpRmSequenceCfg] )
        else:
           return self.getStep(1,"mufast", [muFastSequenceCfg] )
         
    # --------------------
    def getmuComb(self):
        doOvlpRm = False
        if "bTau" in self.chainName or "bJpsi" in self.chainName or "bUpsi" in self.chainName or "bDimu" in self.chainName or "bBmu" in self.chainName:
           doOvlpRm = False
        elif self.mult>1:
           doOvlpRm = True
        elif len( self.dict['signatures'] )>1 and not self.chainPart['extra']:
           doOvlpRm = True
        else:
           doOvlpRm = False

        if doOvlpRm:
           return self.getStep(2, 'muComb', [muCombOvlpRmSequenceCfg] )
        else:
           return self.getStep(2, 'muComb', [muCombSequenceCfg] )

    # --------------------
    def getmuEFSA(self):
        return self.getStep(3,'muEFSA',[ muEFSASequenceCfg])

    # --------------------
    def getmuEFMS(self):
        return self.getStep(3,'muEFMS', [muEFMSSequenceCfg])

    # --------------------
    def getmuIso(self):
        return self.getStep(3,'muIso', [muIsoSequenceCfg])

    # --------------------
    def getmuEFCB(self):

        if 'invm' in self.chainPart['invMassInfo']:
            return self.getStep(4,'EFCB', [muEFCBSequenceCfg], comboTools=[TrigMuonEFInvMassHypoToolFromDict])
        else:
            return self.getStep(4,'EFCB', [muEFCBSequenceCfg])
 
    # --------------------
    def getFSmuEFSA(self):
        return self.getStep(5,'FSmuEFSA', [FSmuEFSASequenceCfg])

    # --------------------
    def getFSmuEFCB(self):
        return self.getStep(6,'FSmuEFCB', [FSmuEFCBSequenceCfg])

    #---------------------
    def getmuEFIso(self):
        return self.getStep(5,'muEFIso',[ muEFIsoSequenceCfg])

    #--------------------
    def getmuMSEmptyAll(self, stepID):
        return self.getEmptyStep(stepID,'muMS_empty')

    #--------------------
    def getmuMSEmpty(self):
        return self.getmuMSEmptyAll(2)

    #--------------------
    def getmuFastEmpty(self):
        return self.getEmptyStep(1,'muFast_empty')

    #--------------------
    def getEFCBEmpty(self):
        return self.getEmptyStep(6,'EFCBEmpty')
    
    #--------------------
    def getLateMuRoI(self):
        return self.getStep(1,'muEFLateRoI',[muEFLateRoISequenceCfg])

    #--------------------
    def getLateMu(self):
        return self.getStep(2,'muEFLate',[muEFLateSequenceCfg])

    #--------------------
    def getmuCombDr(self):     
        step=self.getStep(2, 'muComb', sequenceCfgArray=[muCombSequenceCfg], comboTools=[dimuDrComboHypoToolFromDict])
        return step

    def getmuFastDr(self):
        step=self.getStep(1,"mufast", [muFastSequenceCfg], comboTools=[dimuDrComboHypoToolFromDict]  )
        return step

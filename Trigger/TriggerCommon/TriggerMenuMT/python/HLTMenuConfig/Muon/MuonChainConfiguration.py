# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from ..Menu.ChainConfigurationBase import ChainConfigurationBase

from .MuonMenuSequences import muFastSequence, muFastOvlpRmSequence, mul2mtSAOvlpRmSequence, muCombSequence, muCombLRTSequence, muCombOvlpRmSequence, mul2mtCBOvlpRmSequence, mul2IOOvlpRmSequence, muEFSASequence, muEFCBSequence, muEFCBLRTSequence, muEFSAFSSequence, muEFCBFSSequence, muEFIsoSequence, muEFMSIsoSequence, efLateMuRoISequence, efLateMuSequence
from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFInvMassHypoToolFromDict


#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def muFastSequenceCfg(flags):
    return muFastSequence()

def muFastOvlpRmSequenceCfg(flags):
    return muFastOvlpRmSequence()

def mul2mtSAOvlpRmSequenceCfg(flags):
    return mul2mtSAOvlpRmSequence()

def muCombSequenceCfg(flags):
    return muCombSequence()

def muCombLRTSequenceCfg(flags):
    return muCombLRTSequence()

def muCombOvlpRmSequenceCfg(flags):
    return muCombOvlpRmSequence()

def mul2IOOvlpRmSequenceCfg(flags):
    return mul2IOOvlpRmSequence()

def mul2mtCBOvlpRmSequenceCfg(flags):
    return mul2mtCBOvlpRmSequence()

def muEFSASequenceCfg(flags):
    return muEFSASequence()

def muEFCBSequenceCfg(flags):
    return muEFCBSequence()

def muEFCBLRTSequenceCfg(flags):
    return muEFCBLRTSequence()

def FSmuEFSASequenceCfg(flags):
    return muEFSAFSSequence()

def FSmuEFCBSequenceCfg(flags):
    return muEFCBFSSequence()

def muEFIsoSequenceCfg(flags):
    return muEFIsoSequence()

def muEFMSIsoSequenceCfg(flags):
    return muEFMSIsoSequence()

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

        iso = ""
        if self.chainPart['isoInfo']:
            iso = 'iso'
        key = self.chainPart['extra']+iso


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
        doMSonly = 'msonly' in self.chainPart['msonlyInfo']

        stepDictionary = {
            "":[['getmuFast', 'getmuMSEmpty' if doMSonly else 'getmuComb'], ['getmuEFSA'] if doMSonly else ['getmuEFSA', 'getmuEFCB']],
            "l2io":[['getmuFast', 'getmuCombIO'], ['getmuEFSA', 'getmuEFCB']],
            "l2mt":[['getmuFastl2mt', 'getmuCombl2mt'], ['getmuEFSA', 'getmuEFCB']],
            "noL2Comb" : [['getmuFast', 'getmuMSEmpty'], ['getmuEFSA', 'getmuEFCB']],
            "noL1":[[],['getFSmuEFSA'] if doMSonly else ['getFSmuEFSA', 'getFSmuEFCB']],
            "iso":[['getmuFast', 'getmuMSEmpty' if doMSonly else 'getmuComb'], ['getmuEFSA', 'getEFCBEmpty', 'getmuEFMSIso'] if doMSonly else ['getmuEFSA', 'getmuEFCB', 'getmuEFIso']],
            "lateMu":[[],['getLateMuRoI','getLateMu']],
            "muoncalib":[['getmuFast']],
            "l2lrt":[['getmuFast', 'getmuComb']],

        }

        return stepDictionary
  
        
    # --------------------
    def getmuFast(self):
        doOvlpRm = False
        if any(x in self.chainName for x in ['bJpsi', 'bUpsi', 'bDimu', 'bBmu', 'bPhi', 'bTau', 'l2io']):
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
    def getmuFastl2mt(self):
        return self.getStep(1,"mufastl2mt", [mul2mtSAOvlpRmSequenceCfg] )
         
    # --------------------
    def getmuComb(self):

        doOvlpRm = False
        if any(x in self.chainName for x in ['bJpsi', 'bUpsi', 'bDimu', 'bBmu', 'bPhi', 'bTau']):
           doOvlpRm = False
        elif self.mult>1:
           doOvlpRm = True
        elif len( self.dict['signatures'] )>1 and not self.chainPart['extra']:
           doOvlpRm = True
        else:
           doOvlpRm = False


        if doOvlpRm:
           return self.getStep(2, 'muComb', [muCombOvlpRmSequenceCfg] )
        elif "LRT" in self.chainPart['addInfo']:
           return self.getStep(2, 'muCombLRT', [muCombLRTSequenceCfg] )
        else:
           return self.getStep(2, 'muComb', [muCombSequenceCfg] )

    # --------------------
    def getmuCombIO(self):
        return self.getStep(2, 'muCombIO', [mul2IOOvlpRmSequenceCfg] )

    # --------------------
    def getmuCombl2mt(self):
          return self.getStep(2,"muCombl2mt", [mul2mtCBOvlpRmSequenceCfg] )

    # --------------------
    def getmuEFSA(self):
        return self.getStep(3,'muEFSA',[ muEFSASequenceCfg])

    # --------------------
    def getmuEFCB(self):

        if 'invm' in self.chainPart['invMassInfo']:
            return self.getStep(4,'EFCB', [muEFCBSequenceCfg], comboTools=[TrigMuonEFInvMassHypoToolFromDict])
        elif "LRT" in self.chainPart['addInfo']:
            return self.getStep(4,'EFCBLRT', [muEFCBLRTSequenceCfg])
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

    #---------------------
    def getmuEFMSIso(self):
        return self.getStep(5,'muEFMSIso',[ muEFMSIsoSequenceCfg])

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
        return self.getEmptyStep(4,'muefCB_Empty')
    
    #--------------------
    def getLateMuRoI(self):
        return self.getStep(1,'muEFLateRoI',[muEFLateRoISequenceCfg])

    #--------------------
    def getLateMu(self):
        return self.getStep(2,'muEFLate',[muEFLateSequenceCfg])


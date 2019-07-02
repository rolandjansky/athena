# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Muon.MuonDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep

from TriggerMenuMT.HLTMenuConfig.Muon.MuonSequenceSetup import muFastSequence, muCombSequence, muEFMSSequence, muEFSASequence, muIsoSequence, muEFCBSequence, muEFSAFSSequence, muEFCBFSSequence, muEFIsoSequence



#--------------------------------------------------------
# fragments generating config will be functions in new JO
#--------------------------------------------------------
def muFastSequenceCfg(flags):
    return muFastSequence()

def muCombSequenceCfg(flags):
    return muCombSequence()

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
        log.debug("Assembling chain for " + self.chainName)

        stepDictionary = self.getStepDictionary()

        key = self.chainPart['extra']+self.chainPart['isoInfo']

        steps=stepDictionary[key]

        for step_level in steps:
            for step in step_level:
                chainSteps+=[step]
    
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
            "":[[self.getmuFast(), self.getmuComb()], [self.getmuEFSA(), self.getmuEFCB()]],
            "fast":[[self.getmuFast()]],
            "Comb":[[self.getmuFast(), self.getmuComb()]],
            "noL2Comb" : [[self.getmuFast()], [self.getmuEFSA(), self.getmuEFCB()]],
            "ivar":[[self.getmuFast(), self.getmuComb(), self.getmuIso()]],
            "noL1":[[],[self.getFSmuEFSA(), self.getFSmuEFCB()]],
            "msonly":[[self.getmuFast(), self.getmuMSEmpty(1)], [self.getmuEFMS()]],
            "ivarmedium":[[self.getmuFast(), self.getmuComb()], [self.getmuEFSA(), self.getmuEFCB(), self.getmuEFIso()]],
        }
       
        return stepDictionary

    def getStep(self, stepID, stepPartName, sequenceCfgArray):
        stepName = 'Step%d'%stepID + '_%d'%self.mult + stepPartName
        log.debug("Configuring step " + stepName)
        seqArray = []
        for sequenceCfg in sequenceCfgArray:
            seqArray.append( RecoFragmentsPool.retrieve( sequenceCfg, None))
        return ChainStep(stepName, seqArray, self.mult)
        
    # --------------------
    def getmuFast(self):
        return self.getStep(1,"mufast", [muFastSequenceCfg] )
         
    # --------------------
    def getmuComb(self):
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
        return self.getStep(4,'EFCB', [muEFCBSequenceCfg])
 
    # --------------------
    def getFSmuEFSA(self):
        return self.getStep(1,'FSmuEFSA', [FSmuEFSASequenceCfg])

    # --------------------
    def getFSmuEFCB(self):
        return self.getStep(2,'FSmuEFCB', [FSmuEFCBSequenceCfg])

    #---------------------
    def getmuEFIso(self):
        return self.getStep(5,'muEFIso',[ muEFIsoSequenceCfg])

    #--------------------
    def getmuMSEmpty(self, stepID):
        return self.getStep(stepID,'muMS_empty',[])



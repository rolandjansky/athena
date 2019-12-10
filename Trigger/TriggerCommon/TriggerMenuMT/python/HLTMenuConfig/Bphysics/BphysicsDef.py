# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for bphysics chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase, RecoFragmentsPool
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import ChainStep
from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import MuonChainConfiguration as MuonChainConfiguration

from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsSequenceSetup import dimuL2Sequence,dimuEFSequence


#--------------------------------------------------------
# fragments generating config will be functions in new JO
# I have no idea what the above sentence means - copy/paste from muons...
#--------------------------------------------------------

def dimuL2SequenceCfg(flags):
    return dimuL2Sequence()
    
def dimuEFSequenceCfg(flags):
    return dimuEFSequence()

############################################# 
###  Class/function to configure muon chains 
#############################################

class BphysicsChainConfiguration(MuonChainConfiguration):

    def __init__(self, chainDict):
        ChainConfigurationBase.__init__(self,chainDict)
       
    # ----------------------
    # Assemble the chain depending on information from chainName
    # ----------------------
    def assembleBphysChain(self):                            
        chainSteps = []
        log.debug("Assembling chain for " + self.chainName)

        muonStepDictionary = self.getStepDictionary()
        bphysStepDictionary = self.getBphysStepDictionary()
         
        #infrastructure cannot deal with having more than one "key" here
        #if we have noL2Comb+extra, this needs to be defined in the muonStepDictionary
        mu_key = 'noL2Comb'+self.chainPart['extra']+self.chainPart['isoInfo']
        
        bphys_key = self.getBphysKey()
        
        muon_steps = muonStepDictionary[mu_key]
        bphys_steps = bphysStepDictionary[bphys_key]

        for mu_step_level, bphys_step_level in zip(muon_steps, bphys_steps):
            for step in mu_step_level:
                chainSteps += [step]
            for step in bphys_step_level:
                chainSteps += [step]


        myChain = self.buildChain(chainSteps)
        return myChain

    def getBphysStepDictionary(self):
        
        stepDictionary = {
            "dimu":[[self.getdimuL2()], [self.getdimuEF()]],
        }
        
        return stepDictionary
        
    def getBphysKey(self):
    
        if len(self.dict['topo']) > 1:
            log.warning("BphysicsChainConfiguration.getBphysKey is not setup for > 1 topo! will use the first one.")

        the_topo = self.dict['topo'][0]
        
        topo_dict = {
            'bJpsimumu' : 'dimu',
            'bUpsimumu' : 'dimu',
            'bBmumu'    : 'dimu',
            'bDimu'     : 'dimu'
        }   
        
        return topo_dict[the_topo] 
    
    # --------------------
    def getdimuL2(self):
        stepName = 'Step2_l2Dimu'
        log.debug("Configuring step " + stepName)
        bphySeq = RecoFragmentsPool.retrieve( dimuL2SequenceCfg, None)
        return ChainStep(stepName, [bphySeq], multiplicity=[1])

    def getdimuEF(self):
        stepName = 'Step5_efDimu'
        log.debug("Configuring step " + stepName)
        bphySeq = RecoFragmentsPool.retrieve( dimuEFSequenceCfg, None)
        return ChainStep(stepName, [bphySeq], multiplicity=[1])


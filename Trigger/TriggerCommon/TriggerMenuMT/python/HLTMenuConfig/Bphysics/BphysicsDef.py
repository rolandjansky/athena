# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for bphysics chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsDef")

from TriggerMenuMT.HLTMenuConfig.Menu.ChainConfigurationBase import ChainConfigurationBase
from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import MuonChainConfiguration as MuonChainConfiguration

from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import muCombSequenceCfg, muEFCBSequenceCfg
from TrigBphysHypo.TrigMultiTrkComboHypoConfig import DimuL2ComboHypoCfg, DimuEFComboHypoCfg, TrigMultiTrkComboHypoToolFromDict

#--------------------------------------------------------
# fragments generating config will be functions in new JO
# I have no idea what the above sentence means - copy/paste from muons...
#--------------------------------------------------------


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

        log.debug("Assembling chain for " + self.chainName)

        stepDictionary = self.getBphysStepDictionary()
        key = self.getBphysKey()
        steps=stepDictionary[key]

        chainSteps = []
        for step_level in steps:
            for step in step_level:
                chainStep = getattr(self, step)()
                chainSteps+=[chainStep]

        chain = self.buildChain(chainSteps)
        return chain

    def getBphysStepDictionary(self):

        stepDictionary = {
            'dimu' : [['getmuFast', 'getDimuComb'], ['getmuEFSA', 'getDimuEFCB']],
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
            'bDimu'     : 'dimu',
            'bDimu2700' : 'dimu',
            'bPhi'      : 'dimu',
            'bTau'      : 'dimu'
        }

        return topo_dict[the_topo]

    def getDimuComb(self):
        return self.getStep(2, 'dimuComb', [muCombSequenceCfg], comboHypoCfg=DimuL2ComboHypoCfg)

    def getDimuEFCB(self):
        return self.getStep(4, 'dimuEFCB', [muEFCBSequenceCfg], comboHypoCfg=DimuEFComboHypoCfg, comboTools=[TrigMultiTrkComboHypoToolFromDict])

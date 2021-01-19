# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from TriggerMenuMT.HLTMenuConfig.Muon.MuonDef import muCombSequenceCfg
from TriggerMenuMT.HLTMenuConfig.Bphysics.BphysicsSequenceSetup import bmumuxSequence, dimuSequence

from TrigBphysHypo.TrigMultiTrkComboHypoConfig import DimuL2ComboHypoCfg, DimuEFComboHypoCfg, TrigMultiTrkComboHypoToolFromDict
from TrigBphysHypo.TrigBmumuxComboHypoConfig import BmumuxComboHypoCfg, TrigBmumuxComboHypoToolFromDict

#--------------------------------------------------------
# fragments generating config will be functions in new JO
# I have no idea what the above sentence means - copy/paste from muons...
#--------------------------------------------------------

def dimuSequenceCfg(flags):
    return dimuSequence()

def bmumuxSequenceCfg(flags):
    return bmumuxSequence()

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

        log.debug("Assembling chain for %s", self.chainName)

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
            'dimu'   : [['getmuFast', 'getDimuComb'], ['getmuEFSA', 'getmuEFCB', 'getDimu']],
            'bl2io'  : [['getmuFast', 'getmuCombIO'], ['getmuEFSA', 'getmuEFCB', 'getDimu']],
            'bmumux' : [['getmuFast', 'getDimuComb'], ['getmuEFSA', 'getmuEFCB', 'getBmumux']],
        }
        return stepDictionary

    def getBphysKey(self):

        the_topo = self.dict['topo'][0]

        topo_dict = {
            'bJpsimumu' : 'dimu',
            'bJpsi'     : 'dimu',
            'bUpsimumu' : 'dimu',
            'bUpsi'     : 'dimu',
            'bBmumu'    : 'dimu',
            'bDimu'     : 'dimu',
            'bDimu2700' : 'dimu',
            'bDimu6000' : 'dimu',
            'bPhi'      : 'dimu',
            'bTau'      : 'dimu',
            'bJpsimumul2io' : 'bl2io',
            'bBmumux'   : 'bmumux'
        }

        return topo_dict[the_topo]

    def getDimuComb(self):
        return self.getStep(2, 'dimuComb', [muCombSequenceCfg], comboHypoCfg=DimuL2ComboHypoCfg)

    def getDimu(self):
        return self.getStep(5, 'dimu', [dimuSequenceCfg], comboHypoCfg=DimuEFComboHypoCfg, comboTools=[TrigMultiTrkComboHypoToolFromDict])

    def getBmumux(self):
        return self.getStep(5, 'bmumux', [bmumuxSequenceCfg], comboHypoCfg=BmumuxComboHypoCfg, comboTools=[TrigBmumuxComboHypoToolFromDict])

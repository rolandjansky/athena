# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for bphysics chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger(__name__)

from ..Menu.ChainConfigurationBase import ChainConfigurationBase
from ..Muon.MuonChainConfiguration import MuonChainConfiguration
from ..Muon.MuonChainConfiguration import mul2IOOvlpRmSequenceCfg, muEFCBSequenceCfg

from .BphysicsMenuSequences import dimuL2Sequence, dimuEFSequence, bmumuxSequence

from TrigBphysHypo.TrigMultiTrkComboHypoConfig import StreamerDimuL2ComboHypoCfg, StreamerDimuL2IOComboHypoCfg, DimuEFComboHypoCfg, StreamerDimuEFComboHypoCfg, TrigMultiTrkComboHypoToolFromDict
from TrigBphysHypo.TrigBmumuxComboHypoConfig import BmumuxComboHypoCfg, TrigBmumuxComboHypoToolFromDict

#--------------------------------------------------------
# fragments generating config will be functions in new JO
# I have no idea what the above sentence means - copy/paste from muons...
#--------------------------------------------------------

def dimuL2SequenceCfg(flags):
    return dimuL2Sequence()

def dimuEFSequenceCfg(flags):
    return dimuEFSequence()

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
            'dimu'   : [['getmuFast', 'getDimuL2'], ['getmuEFSA', 'getmuEFCB', 'getDimuEF']],
            'bmumux' : [['getmuFast', 'getDimuL2'], ['getmuEFSA', 'getDimuEFCB', 'getBmumux']],
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
            'bBmumux'   : 'bmumux'
        }

        return topo_dict[the_topo]

    def getDimuL2(self):
        if 'noL2Comb' in self.chainPart['extra']:
            return self.getStep(2, 'dimuL2', [dimuL2SequenceCfg], comboHypoCfg=StreamerDimuL2ComboHypoCfg)
        else:
            return self.getStep(2, 'dimuL2IO', [mul2IOOvlpRmSequenceCfg], comboHypoCfg=StreamerDimuL2IOComboHypoCfg)

    def getDimuEF(self):
        return self.getStep(5, 'dimuEF', [dimuEFSequenceCfg], comboHypoCfg=DimuEFComboHypoCfg, comboTools=[TrigMultiTrkComboHypoToolFromDict])

    def getDimuEFCB(self):
        return self.getStep(4, 'dimuEFCB', [muEFCBSequenceCfg], comboHypoCfg=StreamerDimuEFComboHypoCfg)

    def getBmumux(self):
        return self.getStep(5, 'bmumux', [bmumuxSequenceCfg], comboHypoCfg=BmumuxComboHypoCfg, comboTools=[TrigBmumuxComboHypoToolFromDict])

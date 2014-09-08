# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Module of factory functions to control the instantiation of Algorithms.

In some cases instantiation is done by a remote module.
In thsi case the factory function imports that module and retrieves the
instance."""


from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigGenericAlgs.TrigGenericAlgsConf import \
    PESA__DummyUnseededAllTEAlgo as DummyAlgo

from TrigGenericAlgs.TrigGenericAlgsConf import \
    PESA__SeededSuperRoiAllTEAlgo as SeededAlgo

from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX

from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo,
                                           TrigCaloCellMaker_jet_super,
                                           TrigCaloTowerMaker_jet,
                                           TrigCaloTopoTowerMaker_jet,
                                           TrigCaloClusterMaker_topo,
                                           TrigLArNoisyROAlgConfig)

from TrigHLTJetRec.TrigHLTJetRecConf import TrigHLTRoIDiagnostics

from TrigHLTJetRec.TrigHLTJetRecConfig import (TrigHLTJetDiagnostics_named,
                                               TrigHLTClusterDiagnostics_named,
                                               TrigHLTCellDiagnostics_named,
                                               TrigHLTHypoDiagnostics_named,
                                               TrigHLTJetRec_AntiKt04,
                                               TrigHLTJetRec_AntiKt10)

from TrigJetHypo.TrigJetHypoConfig import (EFJetHypo,
                                           # EFCentJetHypo,
                                           EFCentFullScanMultiJetHypo,
                                           # EFFwdJetHypo,
                                           EFJetHypoNoiseConfig,
                                           EFFwdJetHypo_doBasicCleaning,
                                           EFJetHypoNoiseConfig,
                                           EFJetHypo_doBasicCleaning)

# from TrigJetHypo.TrigEFJetMassDEtaConfig import (EFJetMassDEta,
#                                                  EFJetMassDEta2J7,
#                                                  EFJetMassDEta2J10,
#                                                  EFJetMassDEta2J25,
#                                                  EFJetMassDEta2J30,
#                                                  EFJetMassDEta2J35,
#                                                  EFJetMassDEta2J40)

# from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Jet

# from TrigEFLongLivedParticles.TrigEFLongLivedParticlesConfig import \
#     TrigLoFRemovalConfig



# from TrigJetHypo.TrigEFHTHypoConfig import EFHT_HAD
# from TrigJetHypo.TrigEFJetMassYConfig import EFJetMassY

# from TrigJetHypo.TrigJetDEtaFexConfig import EFDEtaFex

# from TrigJetHypo.TrigJetDEtaHypoConfig import EFDEtaHypo

abomination_to_keep_config_weakvalue_dict_intact = []


class Instantiator(object):

    def __init__(self):
        self.cache = {}

    def __call__(self, a):
        """__call__ takes the string returned by alg.asString for
        an object alg, and evals it to return an Athena Algorithm instance"""

        s = a.asString()  # convert alg to a string to be eval'd
        alg = self.cache.get(s)
        if alg:
            return alg

        try:
            alg = eval(s)
        except Exception, e:
            m = '%s() Error instantiating  Algorithm: eval(%s) %s'
            m = m % (self.__class__.__name__, s, str(e))
            raise RuntimeError(m)

        self.cache[s] = alg
        # in 2012 it was required to maintain a reference to the
        # configuration objects due to the use of a weak referece
        # dictionary in a module far far away. Who knows what 2015 will bring..
        abomination_to_keep_config_weakvalue_dict_intact.append(alg)

        return alg

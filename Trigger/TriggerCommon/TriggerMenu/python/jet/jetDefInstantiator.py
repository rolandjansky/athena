# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Module of factory functions to control the instantiation of Algorithms.

In some cases instantiation is done by a remote module.
In thsi case the factory function imports that module and retrieves the
instance."""

from exc2string import exc2string2

#from TriggerJobOpts.TriggerFlags import TriggerFlags

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo as DummyAlgo   # noqa: F401
from TrigGenericAlgs.TrigGenericAlgsConf import PESA__SeededSuperRoiAllTEAlgo as SeededAlgo # noqa: F401

# from TrigGenericAlgs.TrigGenericAlgsConf import DummyFEX

# from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo,
#                                           TrigCaloCellMaker_jet_super,
#                                           TrigCaloTowerMaker_jet,
#                                           TrigCaloTopoTowerMaker_jet,
#                                           TrigCaloClusterMaker_topo,
#                                           TrigLArNoisyROAlgConfig)

from TrigCaloRec.TrigCaloRecConfig import (TrigCaloCellMaker_jet_fullcalo, # noqa: F401
                                           TrigCaloCellMaker_jet_super,    # noqa: F401
                                           TrigCaloClusterMaker_topo)      # noqa: F401

from TrigCaloRec.TrigCaloRecConf import (TrigL1BSTowerMaker,)     # noqa: F401

from TrigHLTJetRec.TrigHLTJetRecConf import TrigHLTRoIDiagnostics # noqa: F401

from TrigHLTJetRec.TrigHLTJetRecConfig import (TrigHLTJetDiagnostics_named,     # noqa: F401
                                               TrigHLTJetRecFromCluster,        # noqa: F401
                                               TrigHLTJetRecFromJet,            # noqa: F401
                                               TrigHLTJetRecGroomer,            # noqa: F401
                                               TrigHLTJetRecFromTriggerTower,   # noqa: F401
                                               TrigHLTClusterDiagnostics_named, # noqa: F401
                                               TrigHLTCellDiagnostics_named,    # noqa: F401
                                               TrigHLTHypoDiagnostics_named,    # noqa: F401
                                               TrigHLTJetDebug,                 # noqa: F401
                                               TrigHLTEnergyDensity,            # noqa: F401
                                               TrigHLTSoftKiller,               # noqa: F401
                                               TrigHLTJetDSSelector,            # noqa: F401
                                               TrigHLTTrackMomentHelpers,)      # noqa: F401

from TrigHLTJetHypo.TrigHLTJetHypoConfig import (TrigHLTJetHypo2,              # noqa: F401
                                                 TrigHLTJetHypo_DijetMassDEta, # noqa: F401
                                                 TrigHLTJetHypo_DijetMassDEtaDPhi,
                                                 TrigHLTJetHypo_EtaEt,         # noqa: F401
                                                 TrigHLTJetHypo_HT,            # noqa: F401
                                                 TrigHLTJetHypo_TLA,           # noqa: F401
                                                 TrigHLTJetHypo_SMC,           # noqa: F401
                                                 TrigHLTJetHypo_Dijet,         
                                                 TrigHLTJetHypo_JetAttrs,         
                                                 )

from TrigDetCalib.TrigDetCalibConf import ScoutingStreamWriter    # noqa: F401

from TrigHIRec.TrigHICaloRec import (TrigCaloTowerMaker_hijet,    # noqa: F401
                                     TrigHIClusterMaker_hijet,    # noqa: F401
                                     TrigHIEventShapeMaker_hijet, # noqa: F401
                                    )
                                    
from TrigHIRec.TrigHLTHIJetRecConfig import TrigHLTHIJetRecFromHICluster  # noqa: F401

abomination_to_keep_config_weakvalue_dict_intact = []


class Instantiator(object):

    def __init__(self):
        self.cache = {}
        self.err_hdr = '%s()' % self.__class__.__name__

    def __call__(self, a):
        """__call__ takes the string returned by alg.asString for
        an object alg, and evals it to return an Athena Algorithm instance"""

        try:
            s = a.asString()  # convert alg to a string to be eval'd
        except Exception, e:
            m = '%s() call to asString failed for object %s\n%s ' % (
                self.err_hdr, str(a), str(e))
            raise RuntimeError(m)


        alg = self.cache.get(s)
        if alg:
            return alg

        try:
            alg = eval(s)
        except Exception, e:
            tb = exc2string2()
            m = '%s() Error instantiating  Algorithm: eval(%s) '\
                '%s\nTraceback: \n%s'
            m = m % (self.__class__.__name__, s, str(e), tb)
            try:
                alg = a.alg
            except:
                m += '\nAttempt to retrieve pre-instantiated Algorithm failed'

                raise RuntimeError(m)

        def manual_attr_add(k, v):

            try:
                val = eval(v)
            except Exception, e:
                m = '%s() Error running  eval: '\
                    'name %s value: eval(%s) \n%s' % (err_hdr, k, v, str(e))
                raise RuntimeError(m)

            try:
                alg.__setattr__(k, val)
            except Exception, e:
                m = '%s() Error inserting a new Algorithm attribute: '\
                    'name %s value: eval(%s) \n%s' % (err_hdr, k, v, str(e))
                raise RuntimeError(m)

        err_hdr = self.err_hdr
        [manual_attr_add(k, v) for k, v in a.manual_attrs.items()]

        if a.__class__.__name__ != 'AlgStringProxy': self.cache[s] = alg
        # in 2012 it was required to maintain a reference to the
        # configuration objects due to the use of a weak referece
        # dictionary in a module far far away. Who knows what 2015 will bring..
        abomination_to_keep_config_weakvalue_dict_intact.append(alg)

        return alg

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" Helper functions for configuring MET chains
"""

from AthenaCommon.CFElements import seqAND
from ..Menu.SignatureDicts import METChainParts_Default
from ..Menu.MenuComponents import RecoFragmentsPool, ChainStep, MenuSequence
from copy import copy
from ..CommonSequences.FullScanDefs import caloFSRoI, trkFSRoI
from AthenaCommon.Logging import logging
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool
from abc import ABC, abstractmethod
from string import ascii_uppercase

log = logging.getLogger(__name__)

# The keys from the MET chain dict that directly affect reconstruction
# The order here is important as it also controls the dict -> string conversion
recoKeys = ["EFrecoAlg", "calib", "constitType", "constitmod", "jetCalib", "addInfo"]
metFSRoIs = [caloFSRoI, trkFSRoI]

def metRecoDictToString(recoDict, skipDefaults=True):
    """ Convert a dictionary containing reconstruction keys to a string

    Any key (from recoKeys) missing will just be skipped.
    If skipDefaults is True then any key whose value is the default one will
    also be skipped.
    """
    return "_".join(
        recoDict[k]
        for k in recoKeys
        if k in recoDict
        and (not skipDefaults or recoDict[k] != METChainParts_Default[k])
    )


class AlgConfig(ABC):
    """ Base class to describe algorithm configurations

    Each individual 'EFrecoAlg' should be described by *one* AlgConfig subclass.
    It must provide its list of required inputs to the constructor and override
    the make_fex method

    The name of fexAlg *must* be self.fexName and the METContainerKey property
    *must* be set to self.outputKey (but this class usually should take care of
    this).

    The subclass must also implement the @classmethod 'algType' which returns
    the EFrecoAlg string that it describes.
    """

    @classmethod
    def algType(cls):
        """ The algorithm that this object configures - this corresponds to the
        EFrecoAlg in the METChainParts dictionary

        Note that no two subclasses of AlgConfig can describe the same algorithm
        (as identified by this string).
        """
        raise NotImplementedError("algType not implemented by subclass!")

    def __init__(self, inputs=[], inputRegistry=None, **recoDict):
        """ Initialise the base class

        =========
        Arguments
        =========
        inputs: The nicknames of the inputs that this FEX uses
        inputRegistry:
            The InputConfigRegistry instance to use. Usually this can be left
            as None and then METRecoSequences.default_inputs will be used.
            However, this parameter is provided in case a FEX requires a vastly
            different set of input objects
        recoDict: Pass *all* the keys required for the recoDict
        """
                   
        # Make sure that we got *all* the keys (i.e. the subclass didn't
        # inadvertently steal one of them from us)
        alg_type = self.algType()
        assert all(k in recoDict for k in recoKeys), (
            f"AlgConfig.__init__ for {alg_type} did not receive all the recoKeys"
            " - this suggests a problem in the subclass __init__ method!"
        )
        self.recoDict = copy(recoDict)
        self._suffix = metRecoDictToString(recoDict)
        if inputRegistry is None:
            from .METRecoSequences import default_inputs

            inputRegistry = default_inputs
        self._registry = inputRegistry
        self._inputs = inputs

    @abstractmethod
    def make_fex(self, name, inputs):
        """ Create the fex from its name and the inputs dict """
        pass

    @property
    def inputRegistry(self):
        """ The InputConfigRegistry object used to build the input sequences """
        return self._registry

    @property
    def outputKey(self):
        """ The MET container object produced by this algorithm """
        from TrigEDMConfig.TriggerEDMRun3 import recordable

        return recordable("HLT_MET_{}".format(self._suffix))

    @property
    def fexName(self):
        """ The name of the algorithm made by this configuration """
        return "EFMET_{}".format(self._suffix)

    def getMonTool(self):
        """ Create the monitoring tool """
        return getMETMonTool()

    def athSequences(self):
        """ Get the reco sequences (split by step) """
        if hasattr(self, "_athSequences"):
            return self._athSequences

        inputMakers = self.inputMakers()
        # Retrieve the inputss
        log.verbose("Create inputs for %s", self._suffix)
        steps, inputs = self.inputRegistry.build_steps(
            self._inputs, metFSRoIs, self.recoDict
        )
        fex = self.make_fex(self.fexName, inputs)
        fex.MonTool = self.getMonTool()
        fex.METContainerKey = self.outputKey
        sequences = []

        for idx, algList in enumerate(steps):
            # Put the input makers at the start
            algList.insert(0, inputMakers[idx])
            if idx == len(steps) - 1:
                algList += [fex]
            sequences.append(seqAND(f"METAthSeq_step{idx}_{self._suffix}", algList))
        self._athSequences = sequences
        return self._athSequences

    def menuSequences(self):
        """ Get the menu sequences (split by step) """
        if hasattr(self, "_menuSequences"):
            return self._menuSequences

        from TrigMissingETHypo.TrigMissingETHypoConfigMT import (
            TrigMETCellHypoToolFromDict,
        )
        from TrigStreamerHypo.TrigStreamerHypoConfigMT import (
            StreamerHypoToolMTgenerator,
        )

        sequences = []
        inputMakers = self.inputMakers()
        ath_sequences = self.athSequences()
        for idx, seq in enumerate(ath_sequences):
            if idx == len(ath_sequences) - 1:
                hypo = self.make_hypo_alg()
                hypo_tool = TrigMETCellHypoToolFromDict
            else:
                hypo = self.make_passthrough_hypo_alg(idx)
                hypo_tool = StreamerHypoToolMTgenerator
            sequences.append(
                MenuSequence(
                    Sequence=seq,
                    Maker=inputMakers[idx],
                    Hypo=hypo,
                    HypoToolGen=hypo_tool,
                )
            )
        self._menuSequences = sequences
        return self._menuSequences

    def make_steps(self, chainDict):
        """ Create the actual chain steps """
        # NB - we index the steps using uppercase letters 'A', 'B', etc
        # This technically means that there is an upper limit of 26 on the
        # number of different steps that can be provided this way, but it seems
        # unlikely that we'll actually run into this limit. If we do, it
        # shouldn't be a problem to change it
        return [
            ChainStep(
                f"step{ascii_uppercase[idx]}_{self._suffix}",
                [seq],
                multiplicity=[1],
                chainDicts=[chainDict],
            )
            for idx, seq in enumerate(self.menuSequences())
        ]

    def make_hypo_alg(self):
        """ The hypo alg used for this configuration """

        from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT

        return TrigMissingETHypoAlgMT(
            name="METHypoAlg_{}".format(self._suffix), METContainerKey=self.outputKey
        )

    def make_passthrough_hypo_alg(self, step):
        from TrigStreamerHypo.TrigStreamerHypoConf import TrigStreamerHypoAlgMT

        return TrigStreamerHypoAlgMT(
            f"METPassThroughHypo_{self._suffix}_step{step}", SetInitialRoIAsFeature=True
        )

    def inputMakers(self):
        """ The input makers for each step """
        if hasattr(self, "_inputMakers"):
            return self._inputMakers
        from ..Jet.JetMenuSequences import getInitialInputMaker, getTrackingInputMaker

        self._inputMakers = [getInitialInputMaker(), getTrackingInputMaker()]
        return self._inputMakers

    @classmethod
    def _get_subclasses(cls):
        """ Provides a way to iterate over all subclasses of this class """
        for subcls in cls.__subclasses__():
            for subsubcls in subcls.__subclasses__():
                yield subsubcls
            yield subcls

    @classmethod
    def _makeCls(cls, dummyFlags, **kwargs):
        """ This is a rather horrible work-around.

        The RecoFragmentsPool approach wants a function that takes a set of
        dummy flags. However our class constructors don't do this (and passing
        in a class doesn't *quite* fit what the RecoFragmentsPool is expecting.
        So instead we pass this function...
        """
        return cls(**kwargs)

    @classmethod
    def fromRecoDict(cls, EFrecoAlg, **recoDict):
        for subcls in cls._get_subclasses():
            if subcls.algType() == EFrecoAlg:
                return RecoFragmentsPool.retrieve(
                    subcls._makeCls, None, EFrecoAlg=EFrecoAlg, **recoDict
                )

        raise ValueError("Unknown EFrecoAlg '{}' requested".format(EFrecoAlg))


# Load all the defined configurations
from . import AlgConfigs

# Make sure that there is an AlgConfig for every EFrecoAlg
AlgConfigs.test_configs()

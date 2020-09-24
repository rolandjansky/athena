# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Helper functions for configuring MET chains
"""

from AthenaCommon.CFElements import seqAND
from ..Menu.SignatureDicts import METChainParts_Default, JetChainParts_Default
from ..Menu.MenuComponents import RecoFragmentsPool
from copy import copy
import six

# The keys from the MET chain dict that directly affect reconstruction
# The order here is important as it also controls the dict -> string conversion
recoKeys = ["EFrecoAlg", "calib", "jetDataType", "jetCalib", "addInfo"]

def extractMETRecoDict(chainDict, fillDefaults=True):
    """ Extract the keys relevant to reconstruction from a provided dictionary

    If fillDefaults is True then any missing keys will be taken from the
    METChainParts_Default dictionary.
    """
    if fillDefaults:
        return {k: chainDict.get(k, METChainParts_Default[k])
                for k in recoKeys}
    else:
        return {k: chainDict[k] for k in recoKeys if k in chainDict}

def metRecoDictToString(recoDict, skipDefaults=True):
    """ Convert a dictionary containing reconstruction keys to a string

    Any key (from recoKeys) missing will just be skipped.
    If skipDefaults is True then any key whose value is the default one will
    also be skipped.
    """
    return "_".join(
            recoDict[k] for k in recoKeys if 
            k in recoDict and 
            (not skipDefaults or recoDict[k] != METChainParts_Default[k]) )

def jetRecoDictForMET(**recoDict):
    """ Get a jet reco dict that's usable for the MET slice """
    keys = ["recoAlg", "dataType", "calib", "jetCalib", "trkopt", "cleaning"]
    jrd = {k: recoDict.get(k, JetChainParts_Default[k]) for k in keys}
    if "jetDataType" in recoDict:
        # Allow for the renaming dataType -> jetDataType
        jrd["dataType"] = recoDict["jetDataType"]
    return jrd



class AlgConfig(object):
    """ Base class to describe algorithm configurations

    Each individual 'EFrecoAlg' should be described by *one* AlgConfig subclass.
    That subclass must set two data members from its __init__ method:

    - self.inputs should be a *list* of all sequences required to make inputs
      for the algorithm that calculates the MET value
    - self.fexAlg should the FEX that calculates the MET value

    Alternatively the recoSequence property can be overwritten to not require
    these.

    The name of fexAlg *must* be self.fexName and the METContainerKey property
    *must* be set to self.outputKey

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

    def __init__(self, **recoDict):
        # Make sure that we got *all* the keys (i.e. the subclass didn't
        # inadvertently steal one of them from us)
        assert set(recoKeys) == set(six.iterkeys(recoDict) ), (
                "AlgConfig.__init__ did not receive all the recoKeys - this "
                "suggests a problem in the subclass __init__ method!")
        self.recoDict = copy(recoDict)
        self._suffix = metRecoDictToString(recoDict)

    @property
    def outputKey(self):
        """ The MET container object produced by this algorithm """
        from TrigEDMConfig.TriggerEDMRun3 import recordable
        return recordable("HLT_MET_{}".format(self._suffix) )

    @property
    def fexName(self):
        """ The name of the algorithm made by this configuration """
        return "EFMET_{}".format(self._suffix)

    @property
    def hypoAlg(self):
        """ The hypo alg used for this configuration """
        if hasattr(self, "_hypoAlg"):
            return self._hypoAlg

        from TrigMissingETHypo.TrigMissingETHypoConf import TrigMissingETHypoAlgMT
        self._hypoAlg = TrigMissingETHypoAlgMT(
                name = "METHypoAlg_{}".format(self._suffix),
                METContainerKey = self.outputKey)
        return self._hypoAlg

    @property
    def menuSequence(self):
        """ The menu sequence for this configuration """
        if hasattr(self, "_menuSequence"):
            return self._menuSequence
        
        from ..Menu.MenuComponents import MenuSequence
        from TrigMissingETHypo.TrigMissingETHypoConfigMT import TrigMETCellHypoToolFromDict
        self._menuSequence = MenuSequence(
                Sequence    = self.athSequence,
                Maker       = self.inputMaker,
                Hypo        = self.hypoAlg,
                # TODO - this shouldn't be called 'cell' something
                HypoToolGen = TrigMETCellHypoToolFromDict)
        return self._menuSequence
    @property
    def inputMaker(self):
        """ The input maker for this configuration """
        if hasattr(self, "_inputMaker"):
            return self._inputMaker
        from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
        self._inputMaker = clusterFSInputMaker()
        return self._inputMaker

    @property
    def athSequence(self):
        """ Create the 'ath' sequence
        
        The return value is cached so only one instance is ever created
        """
        if hasattr(self, "_athSequence"):
            return self._athSequence
        self._athSequence = seqAND("METAthSeq_{}".format(self._suffix),
                                   [self.inputMaker, self.recoSequence])
        return self._athSequence

    @property
    def recoSequence(self):
        """ Create the full reco sequence
        
        The return value is cached so only one instance is ever created
        """
        if hasattr(self, "_recoSequence"):
            # Cache the result so we only ever create this once
            return self._recoSequence
        # First, verify that everything was done right
        assert self.fexAlg.name() == self.fexName, (
            "Incorrect algorithm name '{}' set (should be '{}')".format(
                self.fexAlg.name(), self.fexName) )
        assert self.fexAlg.METContainerKey == self.outputKey, (
            "Incorrect output key '{}' set (should be '{}')".format(
                self.fexAlg.METContainerKey, self.outputKey) )

        self._recoSequence = seqAND("METRecoSeq_{}".format(self._suffix),
                     self.inputs + [self.fexAlg])
        return self._recoSequence

    def _make_fex_alg(self, alg_cls, name=None, METContainerKey=None,
                  MonTool=None, **kwargs):
        """ Helper function to make Fex algs

        Ensures that (unless overridden) the default name, output key and
        monitoring tool are used.
        """
        from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool
        # Set the defaults
        if name is None:
            name = self.fexName
        if METContainerKey is None:
            METContainerKey = self.outputKey
        if MonTool is None:
            MonTool = getMETMonTool()
        return alg_cls(
                name            = name,
                METContainerKey = METContainerKey,
                MonTool         = MonTool,
                **kwargs)


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
                        subcls._makeCls, None, EFrecoAlg=EFrecoAlg, **recoDict)

        raise ValueError("Unknown EFrecoAlg '{}' requested".format(EFrecoAlg) )

# Load all the defined configurations
from . import AlgConfigs
# Make sure that there is an AlgConfig for every EFrecoAlg
AlgConfigs.test_configs()

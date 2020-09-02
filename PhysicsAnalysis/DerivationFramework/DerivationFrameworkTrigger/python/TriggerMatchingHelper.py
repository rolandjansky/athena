# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from DerivationFrameworkTrigger.DerivationFrameworkTriggerConf import DerivationFramework__TriggerMatchingTool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkJob # noqa: F401

import AthenaCommon.AppMgr as AppMgr
from AthenaCommon.Configurable import Configurable
import copy
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TriggerMatchingHelper(object):
    """ Helper class for setting up shared trigger matching.
    
        'Shared' trigger matching uses a algorithm+tool that are shared across
        several different derivation formats that can be run in the same trains
        together. The tool can only be run once per event and sharing an algorithm
        ensures that this happens correctly.

        This class can technically support multiple different tool
        configurations (set by the kwargs in the __init__ function). However
        right now we only really expect this to be used with its default
        settings.
    """
    # Register of tool and algorithm pairs
    _register = {}

    @classmethod
    def get_alg_and_tool(cls, **kwargs):
        properties = copy.deepcopy(
                DerivationFramework__TriggerMatchingTool.getDefaultProperties())
        if "name" not in kwargs or kwargs["name"] == Configurable.DefaultName:
            kwargs["name"] = "DFTriggerMatchingTool"
        properties.update(kwargs)
        # We don't want to hash the ChainNames as they don't affect the physics
        keys = sorted(k for k in properties if k != "ChainNames")
        hashval = hash(tuple(str(properties[k]) for k in keys) )
        if hashval not in cls._register:
            # We need to make a new alg and tool, but first we have to check
            # that two arguments do not collide - the name and the output prefix.
            # The name mustn't collide because otherwise athena will just return
            # us the same object, the output prefix mustn't collide because
            # otherwise we'll get errors trying to write containers
            if any(t.name() == properties["name"]
                   for _, t in cls._register.itervalues() ):
                raise ValueError("Attempting to create a new trigger matching tool with the same name as an existing one")
            tool = DerivationFramework__TriggerMatchingTool(**properties)
            AppMgr.ToolSvc += tool
            alg = DerivationFramework__CommonAugmentation(
                    name=properties["name"]+"_Alg",
                    AugmentationTools = [tool])
            cls._register[hashval] = (alg, tool)
        return cls._register[hashval]

    def __init__(self, trigger_list, add_to_df_job=False, **kwargs):
        """ Create the helper

            The trigger_list is the list of triggers that *this* derivation
            needs matching results for.

            kwargs can be used to pass extra properties to the underlying
            DerivationFramework::TriggerMatchingTool. If you change *any*
            properties you *must* also set the 'name' and
            'OutputContainerPrefix' keys.

            If this is a 'private' configuration (i.e. one that is to be used
            *only* in your format) then these values should include your
            format's name and you can just add self.tool to your kernel
            algorithm. However, if you're using a shared algorithm and tool (one
            that doesn't include your format's name) then you *must* either
            include self.alg in your derivation's sequence, or set
            'add_to_df_job' to True, which will add the algorithm directly into
            the main DF job. You should *never* set 'add_to_df_job' to True for
            a private configuration!

            Doing anything else will likely cause the derivation framework to
            fail when it runs multiple formats in a train. Unless you run a
            train in your local tests, this will not show up in those tests and
            will just break a nightly instead!
        """
        self.alg, self.tool = self.get_alg_and_tool(**kwargs)
        if add_to_df_job:
            global DerivationFrameworkJob
            if self.alg not in DerivationFrameworkJob:
                DerivationFrameworkJob += self.alg
        self.tool.ChainNames += trigger_list
        self.trigger_list = trigger_list[:]
        self.container_prefix = self.tool.OutputContainerPrefix

    def output_containers(self):
        """ Get the full list of containers to be written out """
        return ["{0}{1}".format(self.container_prefix, chain).replace(".", "_")
                for chain in self.trigger_list]
    def add_to_slimming(self, slimming_helper):
        """ Add the created containers to the output """
        slimming_helper.AllVariables += self.output_containers()
        for cont in self.output_containers():
            slimming_helper.AppendToDictionary.update({
                cont : "xAOD::TrigCompositeContainer",
                cont+"Aux" : "AOD::AuxContainerBase!"})

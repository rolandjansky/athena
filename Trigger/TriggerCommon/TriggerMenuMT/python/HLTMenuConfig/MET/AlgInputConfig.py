#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

""" Helpers for creating input reco sequences

Classes:
    AlgInputConfig - base class for individual input configs
    InputConfigRegistry - class to gather input configurations together

Together, these classes implement a rudimentary dependency resolution method.
The main unit in this are the inputs being created. These are referred to by
nicknames rather than the container names directly as these can vary depending
on the reconstruction parameters provided. For example, the clusters input
config creates the "Clusters" input. This corresponds to a StoreGate key of
'HLT_TopoCaloClustersFS' if the cluster calibration is 'em' and
'HLT_TopoCaloClustersLCFS' if the cluster calibration is 'lcw'.

This file just provides the underlying mechanisms. For the concrete example see
METRecoSequences, which will be referenced in the following explanation.

Each config declares the inputs it will produce and on what inputs it depends.
The dependencies are allowed to be different for different reco configurations
but for simplicity's sake the inputs produced are always the same. For example
the jet input config in METRecoSequences produces ["Jets", "JetDef"] and its
dependencies are ["Clusters"] if no tracks are needed and ["Clusters", "Tracks"
"Vertices"] otherwise.

Each config also has a create_sequence method which builds the actual reco
sequence. This method returns a two-tuple. The first element is the reco
sequence which makes the inputs, the second is a dictionary mapping the produced
input's nicknames to their values. These values are usually storegate keys but
can be any object. For example, the jet input config (with the correct reco
configuration) would produce
{
    "Jets" : "HLT_AntiKt4EMTopoJets_subjesIS",
    "JetDef" : jetdef
}
for this mapping, where jetdef is a python object produced by the jet reco
sequence.

The dependency resolution is done by the InputConfigRegistry class. This
maintains a list of known input configs where no two can claim to produce the
same input. The main one used here is the default_inputs object created in
METRecoSequences. The 'build_steps' method is then used to create the necessary
sequences in the correct order, split into their steps and also to collate and 
return the mapping of input nicknames to values.

The same system is also used for component-accumulator based configuration.
"""

from abc import ABC, abstractmethod
from collections import defaultdict
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from functools import partial

log = logging.getLogger(__name__)


class AlgInputConfig(ABC):
    """ Base class for building up inputs for the FEXes """

    def __init__(self, produces, step=None):
        """Initialise the class

        =========
        Arguments
        =========
        produces: The nicknames of the inputs this config produces
        step: The step in which the input sequences should run

        step can be None, in which case it will be calculated as the maximum
        of all the steps of the input's dependencies. This means that an input
        with no dependencies must have its step set explicitly
        """
        self._produces = produces
        self._step = step

    @property
    def step(self):
        """ The reconstruction step this input runs in """
        return self._step

    @property
    def produces(self):
        """ Which (nicknamed) inputs does this config produce """
        return self._produces

    @abstractmethod
    def dependencies(self, recoDict):
        """ Given the reco parameters, which inputs does this config depend on """
        pass

    @abstractmethod
    def create_sequence(self, inputs, RoIs, recoDict):
        """ Create the sequence and return it along with a dictionary of the objects that it produces """
        pass

    # TODO: When we have CAs for all inputs, make this an abstract method
    def create_accumulator(self, flags, inputs, RoIs, recoDict):
        """ Create a component accumulator for this input and return it along with a dictionary of the objects that it produces """
        raise NotImplementedError(
            f"{self.produces} does not provide a CA implementation!"
        )


class InputConfigRegistry:
    def __init__(self):
        self._configs = {}

    def add_input(self, config):
        overlap = [x for x in config.produces if x in self._configs]
        if overlap:
            raise ValueError(
                f"Attempting to add an input config that produces {overlap} but these are already present"
            )
        for x in config.produces:
            self._configs[x] = config

    def build_steps(self, requested, RoIs, recoDict, return_ca=False, flags=None):
        """Build the necessary input sequence, separated by steps

        =========
        Arguments
        =========
        requested: The nicknames of the requested inputs
        RoIs: The input RoIs, one must be provided for each step
        recoDict: The recoDict extracted from the chain
        return_ca: If true, return a component accumulator
        flags: The input flags for the CA configuration. Must be provided if
               return_ca is true.

        =======
        Returns
        =======
        (steps, inputs)
        where steps is a list of input sequences or CAs, one for each step and
        inputs is a dictionary mapping input nickname to storegate key
        """
        if return_ca and flags is None:
            raise ValueError(
                "Must provide flags if a component accumulator is requested"
            )
        # The input sequences, keyed by step
        steps = defaultdict(ComponentAccumulator) if return_ca else defaultdict(list)
        # The mapping of input nickname to storegate key
        inputs = {}
        # Internal mapping of input nickname to step
        input_steps = {}
        log.debug("Producing steps for requested inputs: %s", requested)
        for name in requested:
            this_steps = self._create_input(
                name,
                RoIs,
                recoDict,
                input_steps=input_steps,
                inputs=inputs,
                return_ca=return_ca,
                flags=flags,
            )
            for step, reco in this_steps.items():
                if return_ca:
                    steps[step].merge(reco)
                else:
                    steps[step] += reco
        # Now convert the steps into a list, filling in empty steps with empty
        # lists/CA
        steps = [steps[idx] for idx in range(max(steps.keys()) + 1)]
        log.debug("Built steps for inputs: %s", inputs)
        log.debug("Steps are:\n%s", steps)
        return steps, inputs

    def _create_input(
        self,
        name,
        RoIs,
        recoDict,
        input_steps,
        inputs,
        return_ca=False,
        flags=None,
        _seen=None,
    ):
        """Create an input and its dependencies

        =========
        Arguments
        =========
        name: The name of the input to create
        RoIs: The input RoIs, one for each step
        recoDict: The recoDict extracted from the chain
        input_steps: Mapping of encountered inputs to the steps they happen in
        inputs: The names of any inputs already created
        return_ca: If true, return a component accumulator
        flags: The input flags for the CA configuration. Must be provided if
               return_ca is true.
        _seen: internal parameter for catching circular dependencies

        =======
        Returns
        =======
        steps
        where steps is a list of input sequences, one for each step
        The provided input_steps and inputs parameters are also updated with
        the new inputs that have been produced
        """
        log.debug("Creating inputs for %s", name)
        if _seen is None:
            _seen = []
        elif name in _seen:
            raise RuntimeError(
                "Circular dependency: {}".format(" -> ".join(_seen + [name]))
            )
        if name in input_steps:
            log.debug("Input already created")
            # We've already seen this step so return dummies
            return {}

        steps = defaultdict(ComponentAccumulator) if return_ca else defaultdict(list)

        try:
            config = self._configs[name]
        except KeyError:
            raise KeyError(f"Requested input {name} not defined")

        dependencies = config.dependencies(recoDict)
        log.debug("Dependencies are %s", dependencies)
        for dep_name in dependencies:
            # The reco steps for the inputs
            dep_steps = self._create_input(
                dep_name,
                RoIs,
                recoDict,
                input_steps,
                inputs,
                return_ca,
                flags,
                _seen + [name],
            )
            # The step number for this dependencey
            dep_step = input_steps[dep_name]
            if config.step is not None and dep_step > config.step:
                raise ValueError(
                    f"Dependency {dep_name} is in a later step '{dep_step}' than {name} which requires it (step = {config.step})"
                )
            # Add these reco sequences to our output lists
            for step, reco in dep_steps.items():
                if return_ca:
                    steps[step].merge(reco)
                else:
                    steps[step] += reco

        if config.step is None:
            if len(dependencies) == 0:
                raise ValueError(f"Unable to work out step for input config {name}!")
            # If the config doesn't specify a step then we run this as early as possible - i.e. in the latest step of all its dependencies
            this_step = max(input_steps[dep] for dep in dependencies)
        else:
            this_step = config.step

        log.debug("%s step is %i", name, this_step)
        # Finally, add *our* info
        if this_step > len(RoIs):
            raise ValueError(
                f"Step {this_step} is greater than the number of RoIs ({RoIs})"
            )
        if return_ca:
            builder = partial(config.create_accumulator, flags)
        else:
            builder = config.create_sequence

        reco, produced_inputs = builder(inputs, RoIs[this_step], recoDict)
        if return_ca:
            steps[this_step].merge(reco)
        else:
            steps[this_step] += reco
        inputs.update(produced_inputs)
        # Add this to the list of things we've already seen, along with everything else it's made
        for made in produced_inputs.keys():
            input_steps[made] = this_step
        return steps
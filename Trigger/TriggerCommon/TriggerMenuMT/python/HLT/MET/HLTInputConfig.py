#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

"""Input configurations for algorithms depending on other HLT FEXs

The new NN FEX uses the outputs of other FEX algorithms as its inputs so needs a specialised 
input config. In order to comply with the other interface all it needs is a suitable build_steps
method.
"""

from collections import defaultdict
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from .ConfigHelpers import AlgConfig, stringToMETRecoDict

log = logging.getLogger(__name__)

class HLTInputConfigRegistry:
    """Configure HLT outputs as inputs"""

    def build_steps(self, requested, RoIs, recoDict, return_ca=False, flags = None):
        """Build the necessary input sequence, separated by steps"""

        if return_ca and flags is None:
            raise ValueError(
                "Must provide flags if a component accumulator is requested"
            )
        # The input sequences, keyed by step
        steps = defaultdict(ComponentAccumulator) if return_ca else defaultdict(list)
        # The mapping of input nickname to storegate key
        inputs = {}
        log.debug("Producing steps for requested inputs: %s", requested)
        for name in requested:
            # TODO, maybe pass the inputs and input_steps dictionaries to the algorithms?
            dct = stringToMETRecoDict(name)
            cfg = AlgConfig.fromRecoDict(**dct)
            if return_ca:
                this_steps = cfg.make_reco_ca(flags)
            else:
                this_steps = cfg.recoAlgorithms()
            for step, reco in enumerate(this_steps):
                if return_ca:
                    steps[step].merge(reco)
                else:
                    steps[step] += [alg for alg in reco if alg not in steps[step]]
            inputs[name] = cfg.outputKey
        # Now convert the steps into a list, filling in empty steps with empty
        # lists/CA
        steps = [steps[idx] for idx in range(max(steps.keys()) + 1)]
        log.debug("Built steps for inputs: %s", inputs)
        log.debug("Steps are:\n%s", steps)
        return steps, inputs


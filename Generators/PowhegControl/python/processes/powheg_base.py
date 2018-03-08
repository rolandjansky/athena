# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from configurable import Configurable
import math
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class PowhegBase(Configurable):
    """! Base class for PowhegBox processes.

    All process types inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, version, executable_name, powheg_executable="pwhg_main", is_reweightable=True, **kwargs):
        """! Constructor.

        @param base_directory  path to PowhegBox code.
        @param version         PowhegBox version.
        @param executable_name folder containing appropriate PowhegBox executable.
        """
        super(PowhegBase, self).__init__()

        ## Powheg executable that will be used
        self.executable = os.path.join(base_directory, version, executable_name, powheg_executable)

        ## List of additional algorithms to schedule
        self.algorithms = []

        ## List of external processes to schedule
        self.externals = {}

        ## List of validation functions to run before preparing runcard
        self.validation_functions = []

        # Universal keywords that are set from the run arguments
        self.add_keyword("ebeam1", kwargs.get("beam_energy", None))
        self.add_keyword("ebeam2", kwargs.get("beam_energy", None))
        self.add_keyword("iseed", int(kwargs.get("random_seed", None)))
        self.add_keyword("numevts", kwargs.get("nEvents", None))

        # Add parameter validation functions
        self.validation_functions.append("validate_integration_parameters")

        ## Switch to determine whether reweighting is allowed
        self.is_reweightable = is_reweightable

    def add_algorithm(self, alg_or_process):
        """! Add an algorithm or external process to the sequence.

        @param process  Algorithm or external process to add.
        """
        # Either add to the list of algorithms to schedule
        if isinstance(alg_or_process, str):
            self.algorithms.append(alg_or_process)
        # ... or add as an external process
        else:
            self.externals[alg_or_process.name] = alg_or_process

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        raise AttributeError("Powheg version is not known!")

    def prepare_to_parallelise(self, n_cores):
        """! Scale calculation parameters by n_cores."""
        __nEvents_unscaled = self.parameters_by_keyword("numevts")[0].value
        for keyword in ["ncall1", "ncall1rm", "ncall2", "ncall2rm", "nubound", "numevts"]:
            for parameter in self.parameters_by_keyword(keyword):
                if parameter.value > 0:
                    parameter.value = int(math.ceil(float(parameter.value) / n_cores))
        __nEvents_scaled = self.parameters_by_keyword("numevts")[0].value
        logger.info("Scaling number of events per job from {} down to {}".format(__nEvents_unscaled, __nEvents_scaled))
        # Freeze parallelstage parameters before printing list for user
        [parameter.freeze() for parameter in self.parameters_by_name("parallelstage")]

    def validate_parameters(self):
        """! Validate any parameters which need it before preparing runcard."""
        for function_name in self.validation_functions:
            getattr(self, function_name)()

    def validate_integration_parameters(self):
        """! Validate integration keywords by forcing to integer values."""
        self.expose()  # convenience call to simplify syntax
        for name in ("foldcsi", "foldphi", "foldy", "itmx1", "itmx2", "ncall1", "ncall2", "nEvents"):
            for parameter in self.parameters_by_name(name):
                try:
                    parameter.value = int(parameter.value)
                except TypeError:
                    logger.fatal("Failed to validate {} with value {}".format(name, parameter.value))
                    raise

    def check_decay_mode(self, decay_mode, allowed_decay_modes=None):
        """! Check whether a decay mode is allowed an raise an exception if it is not."""
        if allowed_decay_modes is None:
            allowed_decay_modes = self.allowed_decay_modes
        if decay_mode not in allowed_decay_modes:
            logger.warning("Decay mode {} not recognised!".format(decay_mode))
            logger.info("Allowed decay modes are:")
            for allowed_decay_mode in allowed_decay_modes:
                logger.info("... {}".format(allowed_decay_mode))
            raise ValueError("Decay mode {} not recognised!".format(decay_mode))

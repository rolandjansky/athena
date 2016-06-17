# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PowhegConfig_base
#  PowhegControl base class for all Powheg processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
import collections
import glob
import math
import os
import subprocess
import time
import strategies
from AthenaCommon import Logging
from DecoratorFactory import decorate
from utility import FileParser, IntegrationGridTester, LHEUtils, RepeatingTimer

# Base class for configurable objects in the jobOptions
#
#  All subprocesses inherit from this class
class PowhegConfig_base(object):
    # Setup athena-compatible logger
    __logger = Logging.logging.getLogger("PowhegControl")

    # Switch to determine whether new attributes can be added
    __attributes_frozen = False

    # Constructor
    #  @param runArgs Generate_tf run arguments
    #  @param opts athena run options
    #  @param indicator that this is a leading-order process
    def __init__(self, runArgs=None, opts=None, LO_process=False):
        # Current directory
        self.__base_directory = os.environ["PWD"]

        # Path to Powheg installation
        self.__powheg_directory = os.environ["POWHEGPATH"]

        # This must be defined by each derived class - don't change it in the jobOptions!
        self._powheg_executable = self.__powheg_directory

        # Switch to determine which version of Powheg this process belongs to
        self._powheg_version_type = 1

        # Allow processes to indicate that they are leading order
        self.__is_leading_order = LO_process

        # Number of subjobs to run in manyseeds mode -- reset by ATHENA_PROC_NUMBER if present
        self.__n_cores = 1

        # Dictionary of named groups of event weights
        self.__event_weight_groups = collections.OrderedDict()

        # Switch to enable/disable reweighting
        self.__enable_reweighting = False
        self.__enable_NNLO_reweighting = False

        # Name of Powheg output LHE events.
        self.__powheg_LHE_output = "pwgevents.lhe"
        # Name of output LHE events. Needed by Generate_tf for showering.
        self.__output_events_file_name = "PowhegOTF._1.events"
        # Name of output LHE events after tar-ing. Needed by Generate_tf for showering.
        self.__output_tarball_name = None

        # List of parameters which are fixed (not user-configurable)
        self.__fixed_parameters = []
        # List of parameters which are user-configurable
        self.__configurable_parameters = {}
        # List of parameters which are user-configurable but are not written directly to the runcard
        self.__phantom_parameters = {}
        # List of decorators attached to this process
        self.__run_card_decorators = []

        # Reverse look-up table from user-exposed name to Powheg parameter name
        self.configurable_to_parameters = collections.defaultdict(list)

        # Reverse look-up table from weight description to weight ID
        self.weight_description_to_ID = collections.defaultdict(list)

        # Add universal functionality
        self.add_parameter_set("base")

        # Initialise values from runArgs
        if runArgs is None:
            self.logger.warning("No run arguments found! Using defaults.")
        else:
            # Read values from runArgs
            if hasattr(runArgs, "ecmEnergy"):
                self.beam_energy = 0.5 * runArgs.ecmEnergy
            if hasattr(runArgs, "maxEvents") and runArgs.maxEvents > 0:
                self.nEvents = int(1.1 * runArgs.maxEvents + 0.5)
            if hasattr(runArgs, "randomSeed"):
                self.random_seed = runArgs.randomSeed
            if hasattr(runArgs, "outputTXTFile"):
                for tarball_suffix in [x for x in [".tar.gz", ".tgz"] if x in runArgs.outputTXTFile]:
                    self.__output_tarball_name = runArgs.outputTXTFile
                    self.__output_events_file_name = self.output_tarball_name.split(tarball_suffix)[0] + ".events"
            # Set inputGeneratorFile to match output events file; otherwise Generate_tf check will fail
            runArgs.inputGeneratorFile = self.output_events_file_name

        # Enable parallel mode if AthenaMP mode is enabled
        self.__n_cores = int(os.environ.pop("ATHENA_PROC_NUMBER", 1))
        if self.cores > 1:
            self.logger.info("This job is running with an athenaMP-like whole-node setup, requesting {} cores".format(self.cores))
            self.manyseeds, self.nEvents_unscaled = 1, 0
            # Try to modify the transform opts to suppress athenaMP mode
            if hasattr(opts, "nprocs"):
                self.logger.info("Re-configuring to keep athena running serially while parallelising Powheg generation.")
                opts.nprocs = 0
            else:
                self.logger.warning("No 'nprocs' option provided!")

    # Override default attribute setting to stop users setting non-existent attributes
    #  @param key Attribute name
    #  @param value Value to set this to
    def __setattr__(self, key, value):
        if self.__attributes_frozen and not hasattr(self, key):
                    # raise AttributeError( "This Powheg process has no option '{}'".format( key ) )
            self.logger.warning("This Powheg process has no option '{}'".format(key))
            time.sleep(300)
        object.__setattr__(self, key, value)

    # Run normal event generation
    #  @param external_run_card Use a user-provided run card
    #  @param run_card_only Only generate the run card
    def generate(self, external_run_card=False, run_card_only=False):
        if not external_run_card:
            self.__generate_run_card()
        if not run_card_only:
            self.__generate_events()
        return

    # Initialise runcard with generic options
    def __generate_run_card(self):
        # Check that event generation is correctly set up
        if (self.bornsuppfact > 0.0) and (self.bornktmin <= 0.0):
            self.logger.warning("These settings: bornsuppfact = {} and bornktmin = {} cannot be used to generate events!".format(self.bornsuppfact, self.bornktmin))
            self.logger.warning("Only fixed-order distributions can be produced with these settings!")

        # Scale-down number of events produced in each run if running in multicore mode
        if self.cores > 1:
            self.logger.info("Preparing to parallelise: running with {} jobs".format(self.cores))
            self.nEvents_unscaled = int(self.nEvents)
            self.nEvents = int(math.ceil(float(self.nEvents_unscaled) / self.cores))
            self.ncall1 = int(math.ceil(float(self.ncall1) / self.cores))
            self.ncall2 = int(math.ceil(float(self.ncall2) / self.cores))
            self.nubound = int(math.ceil(float(self.nubound) / self.cores))
            self.logger.info("Scaling number of events per job from {} down to {}".format(self.nEvents_unscaled, self.nEvents))

        # Finalise registered decorators
        for run_card_decorator in self.run_card_decorators:
            if hasattr(run_card_decorator, "finalise"):
                run_card_decorator.finalise()

        # Print list of configurable parameters for users
        self.logger.info("** User configurable parameters for this process **")
        self.logger.info(":   Option name   :  ATLAS default  :     Current     : Description")
        for (powheg_parameter, configurable_list) in sorted(self.configurable_parameters.items() + self.phantom_parameters.items(), key=lambda x: x[1][0].lower()):
            self.logger.info(": {:<15} : {:>15} : {:>15} : {}".format(configurable_list[0], configurable_list[1], getattr(self, configurable_list[0]), configurable_list[2]))
            self.configurable_to_parameters[configurable_list[0]].append(powheg_parameter)

        # Add configurable parameters to fixed list
        for configurable_list in self.configurable_parameters.values():
            self.fix_parameter(non_configurable_name=configurable_list[0], default=configurable_list[1], desc=configurable_list[2])

        # Write out final runcard
        self.logger.info("Writing Powheg runcard to {}".format(self.run_card_path))
        with open(self.run_card_path, "w") as f:
            for non_configurable_list in sorted(self.fixed_parameters, key=lambda x: x[0].lower()):
                name, value, default, desc = non_configurable_list
                if "lhrwgt" in name:
                    value = "'{}'".format(value)  # lhrwgt parameters need to be in quotes
                # Set starting value to first in list when multiple values are provided
                if isinstance(value, list):
                    if name in sum([self.configurable_to_parameters[x] for x in ["PDF", "mu_R", "mu_F"]], []):
                        self.__enable_reweighting = True
                        self.logger.debug("Reweighting enabled")
                    value = value[0]
                f.write("{:<30}! [ATLAS default: {}] {}\n".format("{} {}".format(name, value), default, desc))
                # Print warnings for specific parameters
                if name == "bornsuppfact" and value > 0:
                    self.logger.warning("Born-level suppression is enabled: using this in conjunction with J-slicing may give problems.")

        # Check for NNLO reweighting
        if hasattr(self, "NNLO_reweighting_inputs") and len(self.NNLO_reweighting_inputs) > 0:
            self.__enable_reweighting = True
            self.__enable_NNLO_reweighting = True
            self.logger.debug("NNLO reweighting enabled")

        # Print final preparation message
        self.logger.info("Using executable: {}".format(self._powheg_executable))
        return

    # Run normal event generation
    def __generate_events(self):
        # Initialise timer
        time_start = time.time()
        self.logger.info("Starting Powheg LHEF event generation at {}".format(time.ctime(time_start)))

        # Setup heartbeat thread
        heartbeat = RepeatingTimer(600., lambda: self.__emit_heartbeat(time.time() - time_start))
        heartbeat.setName("heartbeat thread")
        heartbeat.daemon = True  # Allow program to exit if this is the only live thread
        heartbeat.start()

        # Remove any existing .lhe files to avoid repeated events
        for LHE_file in glob.glob("*.lhe") + glob.glob("*.ev*ts"):
            self.logger.warning("Removing existing LHE file: {}".format(LHE_file))
            os.remove(LHE_file)

        # Run appropriate Powheg process and display generation output until finished
        if not os.path.isfile(self._powheg_executable):
            raise OSError("Powheg executable {} not found!".format(self._powheg_executable))
        self.__run_generation_strategy()

        # Print timing information
        generation_end = time.time()
        elapsed_time = generation_end - time_start
        self.logger.info("Running nominal Powheg took {} for {} events => {:6.4f} Hz".format(RepeatingTimer.human_readable_time_interval(elapsed_time), self.nEvents, self.nEvents / elapsed_time))

        # Concatenate output events if running in multicore mode
        if self.cores > 1:
            self.logger.info("Concatenating output LHE files: expecting {}".format(self.cores))
            LHEUtils.merge("pwgevents*.lhe", self.powheg_LHE_output)
            subprocess.call("rm pwgevents-*.lhe 2> /dev/null", shell=True)
            # Unscale nEvents in case this is needed by afterburners
            FileParser("powheg*.input").text_replace("numevts .*", "numevts {}".format(self.nEvents_unscaled))

        # Check for required output file
        if not os.path.isfile(self.powheg_LHE_output):
            self.logger.warning("No output LHEF file found! Probably because the Powheg process was killed before finishing.")
            raise RuntimeError("No output LHEF file produced by Powheg. Terminating job.")

        # Run Powheg afterburners
        self.__run_afterburners()
        elapsed_time = time.time() - generation_end
        self.logger.info("Running Powheg afterburners took {}".format(RepeatingTimer.human_readable_time_interval(elapsed_time)))

        # Move output to correctly named file
        try:
            os.rename(self.powheg_LHE_output, self.output_events_file_name)
            self.logger.info("Moved {} to {}".format(self.powheg_LHE_output, self.output_events_file_name))
        except OSError:
            self.logger.warning("No output LHEF file found! Probably because the Powheg process was killed before finishing.")
            raise RuntimeError("No output LHEF file produced by Powheg. Terminating job.")

        # Tar events if LHE output is requested
        if self.output_tarball_name is not None:
            self.logger.info("Tar-ing output events into {}".format(self.output_tarball_name))
            for line in subprocess.check_output(["tar", "cvzf", self.output_tarball_name, self.output_events_file_name], stderr=subprocess.STDOUT).splitlines():
                self.logger.info(line)

        # Print finalisation message
        IntegrationGridTester.output_results(self.logger)
        self.logger.info("Finished at {}".format(time.asctime()))

        # Kill heartbeat thread
        heartbeat.cancel()
        return

    # Run external Powheg process
    def __run_generation_strategy(self):
        # Initialise reweighting
        if self.enable_reweighting:
            strategies.initialise_reweighting(self)

        # Run single core
        if self.cores == 1:
            strategies.generate_single_core(self)

        # Run multicore
        else:
            # Run v1-style multicore (only needs one step)
            if self._powheg_version_type == 1:
                strategies.generate_v1_multi_core(self)
            # Run v2-style multicore (needs four steps)
            else:
                strategies.generate_v2_multi_core(self)
        return

    # Run external Powheg process
    def __run_afterburners(self):
        # Run event weight calculator if bornsupfact was enabled
        if self.bornsuppfact > 0.0 and self.bornktmin > 0.0:
            strategies.afterburner_mean_event_weight_calculator(self.powheg_LHE_output)

        # Run MadSpin afterburner if requested
        if hasattr(self, "_MadSpin_executable"):
            strategies.afterburner_MadSpin(self)

        # Run PHOTOS afterburner if requested
        if hasattr(self, "_PHOTOS_executable") and self.use_photos == 1:
            strategies.afterburner_PHOTOS(self)

        # Run scale/PDF/arbitrary reweighting if requested
        if self.enable_reweighting:
            strategies.afterburner_reweighting(self)

        # Run NNLO reweighting if requested
        if self.enable_NNLO_reweighting:
            strategies.afterburner_NNLO_reweighting(self)

        return

    # Register configurable parameter: exposed to the user and written to the run card
    #  @param configurable_name Configurable parameter name exposed to the user
    #  @param value Value of the parameter
    #  @param desc Description for the run card
    #  @param parameter Name used in the run card if different
    def add_parameter(self, configurable_name, value, desc="", parameter=None):
        setattr(self, configurable_name, value)  # add new attribute
        powheg_parameter = parameter if parameter is not None else configurable_name
        self.configurable_parameters[powheg_parameter] = [configurable_name, "-", desc]
        return

    # Register configurable parameter: exposed to the user but not written to the run card
    #  @param configurable_name Configurable parameter name exposed to the user
    #  @param value Value of the parameter
    #  @param desc Description for the run card
    def add_phantom(self, configurable_name, value, desc=""):
        setattr(self, configurable_name, value)  # add new attribute
        self.phantom_parameters[configurable_name] = [configurable_name, "-", desc]
        return

    # Register a non-configurable parameter: not exposed to the user but written to the run card
    #  @param non_configurable_name Parameter name
    #  @param value Value of the parameter
    #  @param desc Description for the run card
    def fix_parameter(self, non_configurable_name, value=None, desc="", default="-"):
        # Get previously set value if not overwriting
        if value is None:
            value = getattr(self, non_configurable_name)
        # Remove it from the configurable list if it was there
        for powheg_parameter, configurable_list in self.configurable_parameters.items():
            # Retrieve Powheg parameter name and description if there is a match
            if non_configurable_name == configurable_list[0]:
                non_configurable_name, default, desc = powheg_parameter, configurable_list[1], configurable_list[2]
                self.configurable_parameters.pop(powheg_parameter)
                break
        self.fixed_parameters.append([non_configurable_name, value, default, desc])
        return

    # Alias to PowhegDecorators.decorate
    #  @param parameter_set Name of a set of parameter to add to this configurable
    def add_parameter_set(self, parameter_set, **kwargs):
        return decorate(self, parameter_set, **kwargs)

    # Add a new named group of event weights
    #  @param group_name         Name of the group of weights
    #  @param parameters_to_vary Names of the parameters to vary
    def define_event_weight_group(self, group_name, parameters_to_vary):
        self.logger.info("Defining new weight group '{}' which alters {} parameters".format(group_name, len(parameters_to_vary)))
        for parameter_name in parameters_to_vary:
            self.logger.info("... {}".format(parameter_name))
        self.event_weight_groups[group_name] = collections.OrderedDict()
        self.event_weight_groups[group_name]["parameter_names"] = parameters_to_vary
        self.__enable_reweighting = True
        self.logger.debug("Reweighting enabled")
        return

    # Add a new event weight
    #  @param group_name       Name of the group of weights that this weight belongs to
    #  @param weight_name      Name of this event weight
    #  @param parameter_values Values of the parameters
    def add_weight_to_group(self, group_name, weight_name, parameter_values):
        assert(len(parameter_values) == len(self.event_weight_groups[group_name]["parameter_names"])), "Expected {} parameter values but only {} were provided!".format(len(self.event_weight_groups[group_name]["parameter_names"]), len(parameter_values))
        self.event_weight_groups[group_name][weight_name] = []
        for parameter_name, value in zip(self.event_weight_groups[group_name]["parameter_names"], parameter_values):
            self.event_weight_groups[group_name][weight_name].append((parameter_name, value))
        return

    # Output a heartbeat message
    #  @param duration Time interval in seconds between output messages
    def __emit_heartbeat(self, duration):
        message = "Heartbeat: Powheg generation has been running for {} in total".format(RepeatingTimer.human_readable_time_interval(duration))
        self.logger.info(message)
        with open("{}/eventLoopHeartBeat.txt".format(self.__base_directory), "w") as f:
            f.write(message)
        return

    # Store default values before allowing user-input so that changes can be detected
    def populate_default_strings(self):
        # Configurable parameters: [configurable_name, default, desc]
        for configurable_list in self.configurable_parameters.values():
            configurable_list[1] = getattr(self, configurable_list[0])
        # Phantom parameters: [configurable_name, default, desc]
        for configurable_list in self.phantom_parameters.values():
            configurable_list[1] = getattr(self, configurable_list[0])
        # Fixed parameters: [non_configurable_name, value, default, desc]
        for non_configurable_list in self.fixed_parameters:
            non_configurable_list[2] = non_configurable_list[1]
        # Freeze attributes at this point
        self.__attributes_frozen = True
        return

    # Get base directory
    @property
    def base_directory(self):
        return self.__base_directory

    # Get dictionary of configurable parameters: visible to user and written to runcard
    @property
    def configurable_parameters(self):
        return self.__configurable_parameters

    # Get number of cores
    @property
    def cores(self):
        return self.__n_cores

    # Get whether reweighting needs to be enabled
    @property
    def enable_reweighting(self):
        return self.__enable_reweighting

    # Get whether NNLO reweighting needs to be enabled
    @property
    def enable_NNLO_reweighting(self):
        return self.__enable_NNLO_reweighting

    # Get ordered dictionary of event weight groups
    @property
    def event_weight_groups(self):
        return self.__event_weight_groups

    # Get dictionary of non-configurable parameters: not visible to user but written to runcard
    @property
    def fixed_parameters(self):
        return self.__fixed_parameters

    # Get whether this is a leading-order process
    @property
    def is_LO(self):
        return self.__is_leading_order

    # Get handle to logger
    @property
    def logger(self):
        return self.__logger

    # Get output file name
    @property
    def output_events_file_name(self):
        return self.__output_events_file_name

    # Get output tarball name
    @property
    def output_tarball_name(self):
        return self.__output_tarball_name

    # Get dictionary of phantom parameters: visible to user but not written to runcard
    @property
    def phantom_parameters(self):
        return self.__phantom_parameters

    # Get Powheg directory
    @property
    def powheg_directory(self):
        return self.__powheg_directory

    # Get output file name
    @property
    def powheg_LHE_output(self):
        return self.__powheg_LHE_output

    # Get list of enabled run card decorators
    @property
    def run_card_decorators(self):
        return self.__run_card_decorators

    # Get full path to runcard
    @property
    def run_card_path(self):
        return "{}/powheg.input".format(self.base_directory)

    # Deprecated function - use generate() instead
    def generateRunCard(self):
        self.logger.warning("The function generateRunCard() is deprecated as of 2015-02-19")
        self.logger.warning("Please use generate() instead of generateRunCard() and generateEvents()")
        time.sleep(300)
        return self.__generate_run_card()

    # Deprecated function - use generate() instead
    def generateEvents(self):
        self.logger.warning("The function generateEvents() is deprecated as of 2015-02-19")
        self.logger.warning("Please use generate() instead of generateRunCard() and generateEvents()")
        time.sleep(300)
        return self.__generate_events()

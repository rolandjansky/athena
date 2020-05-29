# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import collections
import os
import processes
from AthenaCommon import Logging
from decorators import timed
from algorithms import Scheduler
from utility import HeartbeatTimer

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")



def _format_QCD_scale_text(factor):
    '''
    Helper function to format QCD scale value
    to conform to the ATLAS weight variation
    naming scheme if possible.
    Given a scale factor as a float, it returns
    a string that is:
    - '0.5'
    - '1'
    - '2'
    - or the float rounded to two digits after the decimal point if it is not equal to one of the above
    '''
    try:
        return {0.5 : '0.5', 1.0 : '1', 2.0 : '2'}[factor]
    except KeyError:
        return '{0:.2f}'.format(factor)



class PowhegControl(object):
    """! Provides PowhegConfig objects which are user-configurable in the jobOptions.

    All subprocesses inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, process_name, run_args=None, run_opts=None):
        """! Constructor.

        @param run_args  Generate_tf run arguments
        @param run_opts  athena run options
        """
        ## Current directory
        self.__run_directory = os.environ["PWD"]

        ## Name of output LHE file used by Generate_tf for showering
        self.__output_LHE_file = "PowhegOTF._1.events"

        ## Dictionary of named groups of event weights
        self.__event_weight_groups = collections.OrderedDict()

        ## Scheduler to determine algorithm ordering
        self.scheduler = Scheduler()

        # Load run arguments
        process_kwargs = {"cores": int(os.environ.pop("ATHENA_PROC_NUMBER", 1))}
        if run_args is None:
            logger.warning("No run arguments found! Using defaults.")
        else:
            # Read values from run_args
            if hasattr(run_args, "ecmEnergy"):
                process_kwargs["beam_energy"] = 0.5 * run_args.ecmEnergy
            if hasattr(run_args, "maxEvents") and run_args.maxEvents > 0:
                if hasattr(run_args, "outputEVNTFile") or hasattr(run_args, "outputYODAFile"):
                    process_kwargs["nEvents"] = int(1.1 * run_args.maxEvents + 0.5)
                else:# default nEvents value is maxEvents for lhe-only production
                    process_kwargs["nEvents"] = run_args.maxEvents
            else:#default is 10k events if no --maxEvents was set
                if hasattr(run_args, "outputEVNTFile") or hasattr(run_args, "outputYODAFile"):
                    process_kwargs["nEvents"] = 11000 # 10% safety factor if we shower the events
                else:
                    process_kwargs["nEvents"] = 10000 # no safety factor for lhe-only production
            if hasattr(run_args, "randomSeed"):
                process_kwargs["random_seed"] = run_args.randomSeed
            if hasattr(run_args, "outputTXTFile"):
                for tarball_suffix in [x for x in [".tar.gz", ".tgz"] if x in run_args.outputTXTFile]:
                    ## Name of output LHE file
                    self.__output_LHE_file = run_args.outputTXTFile.split(tarball_suffix)[0] + ".events"
                    self.scheduler.add("output tarball preparer", self.__output_LHE_file, run_args.outputTXTFile)
            # Set inputGeneratorFile to match output events file - otherwise Generate_tf check will fail
            run_args.inputGeneratorFile = self.__output_LHE_file

        # Load correct process
        self.process = getattr(processes.powheg, process_name)(os.environ["POWHEGPATH"].replace("POWHEG-BOX", ""), **process_kwargs)

        # check if pre-made integration grids will be used or not
        self.process.check_using_integration_files()

        # Expose all keyword parameters as attributes of this config object
        for parameter in self.process.parameters:
            if parameter.is_visible:
                setattr(self, parameter.name, parameter.value)

        # Expose all external parameters as attributes of this config object
        for external in self.process.externals.values():
            for parameter in external.parameters:
                if parameter.is_visible:
                    setattr(self, parameter.name, parameter.value)

        # Add appropriate directory cleanup for this process
        self.scheduler.add("directory cleaner", self.process)

        # Schedule correct output file naming
        self.scheduler.add("output file renamer", self.__output_LHE_file)

        # Enable parallel mode if AthenaMP mode is enabled. Also force multistage mode for RES.
        if self.process.cores == 1 and self.process.powheg_version != "RES":
            self.scheduler.add("singlecore", self.process)
        else:
            if self.process.cores == 1:
                logger.info("Configuring this POWHEG-BOX-RES process to run in multistage mode")
            else:
                # Try to modify the transform opts to suppress athenaMP mode
                logger.info("This job is running with an athenaMP-like whole-node setup, requesting {} cores".format(self.process.cores))
                if hasattr(run_opts, "nprocs"):
                    logger.info("Re-configuring to keep athena running serially while parallelising POWHEG-BOX generation.")
                    run_opts.nprocs = 0
                else:
                    logger.warning("Running in multicore mode but no 'nprocs' option was provided!")
            self.scheduler.add("multicore", self.process)
            self.scheduler.add("merge output", self.process.cores, self.nEvents)
            self.process.parameters_by_name("manyseeds")[0].value = 1

        # Freeze the interface so that no new attributes can be added
        self.interface_frozen = True

        # Print executable being used
        logger.info("Configured for event generation with: {}".format(self.process.executable))

    def generate(self, create_run_card_only=False, save_integration_grids=True, use_external_run_card=False, use_XML_reweighting=True):
        """! Run normal event generation.

        @param create_run_card_only    Only generate the run card.
        @param save_integration_grids  Save the integration grids for future reuse.
        @param use_external_run_card   Use a user-provided Powheg run card (powheg.input).
        @param use_XML_reweighting     Use XML-based reweighting.
        """
        self.process.use_XML_reweighting = use_XML_reweighting

        # Schedule integration gridpack creator if requested
        if save_integration_grids:
            self.scheduler.add("integration gridpack creator", self.process)

        # Run appropriate generation functions
        if not use_external_run_card:
            self._generate_run_card()
        else:
            logger.warning("Using native Powheg run card (must be located at './powheg.input' in order for Powheg to find it!) to configure event generation, instead of PowhegControl configuration interface")
        if not create_run_card_only:
            self._generate_events()

    def _generate_run_card(self):
        """! Initialise runcard with appropriate options."""
        # Check that event generation is correctly set up
        if (hasattr(self, "bornsuppfact") and self.bornsuppfact > 0.0) and (hasattr(self, "bornktmin") and self.bornktmin <= 0.0):
            logger.warning("These settings: bornsuppfact = {} and bornktmin = {} cannot be used to generate events!".format(self.bornsuppfact, self.bornktmin))
            logger.warning("Only fixed-order distributions can be produced with these settings!")

        # Scale-down number of events produced in each run if running in multicore mode
        if self.process.cores > 1:
            logger.info("Preparing to parallelise: running with {} jobs".format(self.process.cores))
            self.process.prepare_to_parallelise(self.process.cores)

        # Validate any parameters which need validation/processing
        self.process.validate_parameters()

        # Construct sorted list of configurable parameters for users - including those from external processes
        parameters_unsorted = list(self.process.parameters)
        for external in self.process.externals.values():
            parameters_unsorted.extend(external.parameters)
        parameters_sorted = [x[1] for x in sorted(dict((p.name.lower(), p) for p in parameters_unsorted).items(), key=lambda x: x[0])]

        # Print sorted list of configurable parameters
        logger.info("=========================================================================================================")
        logger.info("|                             User configurable parameters for this process                             |")
        logger.info("=========================================================================================================")
        logger.info("|        Option name        |    ATLAS default    |                     Description                     |")
        logger.info("=========================================================================================================")
        for parameter in [p for p in parameters_sorted if p.is_visible]:
            _default_value = "default" if (parameter.default_value is None or parameter.default_value == "") else str(parameter.default_value)
            logger.info("| {:<25} | {:>19} | {}".format(parameter.name, _default_value, parameter.description))
        logger.info("========================================================================================================")

        # Print list of parameters that have been changed by the user
        parameters_changed = [p for p in parameters_sorted if p.value != p.default_value]
        logger.info("In these jobOptions {} parameter(s) have been changed from their default value:".format(len(parameters_changed)))
        for idx, parameter in enumerate(parameters_changed):
            logger.info("  {:<3} {:<19} {:>15} => {}".format("{})".format(idx + 1), "{}:".format(parameter.name), str(parameter.default_value), parameter.value))

        # Check for parameters which can result in non-equal event weights being used
        event_weight_options = []

        # Write out final runcard
        run_card_path = "{}/powheg.input".format(self.__run_directory)
        logger.info("Writing POWHEG-BOX runcard to {}".format(run_card_path))
        with open(run_card_path, "w") as f_runcard:
            for parameter in sorted(self.process.parameters, key=lambda p: p.keyword.lower()):
                if parameter.name == "bornsuppfact" and parameter.value > 0:
                    event_weight_options.append(("Born-level suppression", "magnitude"))
                if parameter.name == "withnegweights" and parameter.value > 0:
                    event_weight_options.append(("negative event weights", "sign"))
                # PDF variations
                if parameter.name == "PDF" and isinstance(parameter.value, collections.Iterable):
                    if "PDF_variation" not in self.__event_weight_groups.keys(): # skip if this group already exists
                        if len(parameter.value) < 2:
                            logger.error("Use 'PowhegConfig.PDF = {0}' rather than 'PowhegConfig.PDF = [{0}]'".format(parameter.value[0] if len(parameter.value) > 0 else "<value>"))
                            raise TypeError("Use 'PowhegConfig.PDF = {0}' rather than 'PowhegConfig.PDF = [{0}]'".format(parameter.value[0] if len(parameter.value) > 0 else "<value>"))
                        self.define_event_weight_group("PDF_variation", ["PDF"], combination_method="hessian")
                        for PDF in map(int, parameter.value[1:]):
                            self.add_weight_to_group("PDF_variation", "MUR1_MUF1_PDF{:d}".format(PDF), [PDF])
                # Scale variations
                if parameter.name in ["mu_F", "mu_R"] and isinstance(parameter.value, collections.Iterable):
                    pdfs = self.process.parameters_by_name("PDF")[0].value
                    nominal_pdf = pdfs if isinstance(pdfs, int) or isinstance(pdfs, str) else pdfs[0]
                    if "scale_variation" not in self.__event_weight_groups.keys(): # skip if this group already exists
                        mu_Rs = self.process.parameters_by_name("mu_R")[0].value
                        mu_Fs = self.process.parameters_by_name("mu_F")[0].value
                        if not isinstance(mu_Rs, collections.Iterable) or not isinstance(mu_Fs, collections.Iterable) or len(mu_Rs) != len(mu_Fs):
                            logger.error("Number of mu_R and mu_F variations must be the same.")
                            raise ValueError("Number of mu_R and mu_F variations must be the same.")
                        self.define_event_weight_group("scale_variation", ["mu_R", "mu_F"], combination_method="envelope")
                        for mu_R, mu_F in zip(map(float, mu_Rs[1:]), map(float, mu_Fs[1:])):
                            mu_R_text = _format_QCD_scale_text(mu_R)
                            mu_F_text = _format_QCD_scale_text(mu_F)
                            self.add_weight_to_group("scale_variation", "MUR{mur}_MUF{muf}_PDF{nominal_pdf}".format(mur=mu_R_text, muf=mu_F_text, nominal_pdf=nominal_pdf), [mu_R, mu_F])
                f_runcard.write("{}\n".format(parameter))

        # Schedule cross-section_calculator
        if len(event_weight_options) > 0:
            self.scheduler.add("cross section calculator")
            logger.warning("POWHEG-BOX has been configured to run with {}".format(" and ".join([x[0] for x in event_weight_options])))
            logger.warning("This means that event weights will vary in {}.".format(" and ".join([x[1] for x in event_weight_options])))
            logger.warning("The cross-section passed to the parton shower will be inaccurate.")
            logger.warning("Please use the cross-section printed in the log file before showering begins.")

        # Schedule reweighting if more than the nominal weight is requested
        if len(self.__event_weight_groups) > 0:
            # Change the order so that scale comes first and user-defined is last
            __ordered_event_weight_groups_list = []
            for __key in ["scale_variation", "PDF_variation"]:
                if __key in self.__event_weight_groups.keys():
                    __ordered_event_weight_groups_list.append((__key, self.__event_weight_groups.pop(__key)))
            for __item in self.__event_weight_groups.items():
                __ordered_event_weight_groups_list.append(__item)
            self.__event_weight_groups = collections.OrderedDict(__ordered_event_weight_groups_list)
            for group_name, event_weight_group in self.__event_weight_groups.items():
                _n_weights = len(event_weight_group) - 3 # there are always three entries: parameter_names, combination_method and keywords
                # Sanitise weight groups, removing any with no entries
                if _n_weights <= 0:
                    logger.warning("Ignoring weight group '{}' as it does not have any variations defined. Check your jobOptions!".format(group_name))
                    del self.__event_weight_groups[group_name] # this is allowed because items() makes a temporary copy of the dictionary
                # Otherwise print weight group information for the user
                else:
                    logger.info("Adding new weight group '{}' which contains {} weights defined by varying {} parameters".format(group_name, _n_weights, len(event_weight_group["parameter_names"])))
                    for parameter_name in event_weight_group["parameter_names"]:
                        logger.info("... {}".format(parameter_name))
                        if not self.process.has_parameter(parameter_name):
                            logger.warning("Parameter '{}' does not exist for this process!".format(parameter_name))
                            raise ValueError("Parameter '{}' does not exist for this process!".format(parameter_name))
            # Add reweighting to scheduler
            self.scheduler.add("reweighter", self.process, self.__event_weight_groups)

    @timed("Powheg LHE event generation")
    def _generate_events(self):
        """! Generate events according to the scheduler."""
        # Setup heartbeat thread
        heartbeat = HeartbeatTimer(600., "{}/eventLoopHeartBeat.txt".format(self.__run_directory))
        heartbeat.setName("heartbeat thread")
        heartbeat.daemon = True  # Allow program to exit if this is the only live thread
        heartbeat.start()

        # Print executable being used
        logger.info("Using executable: {}".format(self.process.executable))

        # Additional arguments needed by some algorithms
        extra_args = {"quark colour fixer": [self.process]}

        # Schedule external processes (eg. MadSpin, PHOTOS, NNLO reweighting)
        for algorithm, external in self.process.externals.items():
            if external.needs_scheduling(self.process):
                self.scheduler.add(algorithm, external, *extra_args.get(algorithm, []))

        # Schedule additional algorithms (eg. quark colour fixer)
        for algorithm in self.process.algorithms:
            self.scheduler.add(algorithm, *extra_args.get(algorithm, []))

        if len(self.process.parameters_by_keyword("for_reweighting")) == 1:
            if self.process.parameters_by_keyword("for_reweighting")[0].value == 1:
                self.scheduler.add("LHE file nominal weight updater", *extra_args.get(algorithm, []))
                logger.info ("Since parameter for_reweighting was set to 1, virtual corrections are added at the reweighting stage only.")
                logger.info ("Will run LHE file nominal weight updater so that XWGTUP value is updated with value of reweighted nominal weight.")

        # Output the schedule
        self.scheduler.print_structure()

        # Run pre-processing
        self.scheduler.run_preprocessors()

        # Run event generation
        self.scheduler.run_generators()

        # Run post-processing
        self.scheduler.run_postprocessors()

        # Kill heartbeat thread
        heartbeat.cancel()

    def define_event_weight_group(self, group_name, parameters_to_vary, combination_method="none"):
        """! Add a new named group of event weights.

        @exceptions ValueError  Raise a ValueError if reweighting is not supported.

        @param group_name          Name of the group of weights.
        @param parameters_to_vary  Names of the parameters to vary.
        @param combination_method  Method for combining the weights.
        """
        if not self.process.is_reweightable:
            logger.warning("Additional event weights cannot be added by this process! Remove reweighting lines from the jobOptions.")
            raise ValueError("Additional event weights cannot be added by this process! Remove reweighting lines from the jobOptions.")
        self.__event_weight_groups[group_name] = collections.OrderedDict()
        self.__event_weight_groups[group_name]["parameter_names"] = parameters_to_vary
        self.__event_weight_groups[group_name]["combination_method"] = combination_method
        self.__event_weight_groups[group_name]["keywords"] = [[p.keyword for p in self.process.parameters_by_name(parameter)] for parameter in parameters_to_vary]

    def add_weight_to_group(self, group_name, weight_name, parameter_values):
        """! Add a new event weight to an existing group.

        @param group_name        Name of the group of weights that this weight belongs to.
        @param weight_name       Name of this event weight.
        @param parameter_values  Values of the parameters.
        """
        if group_name not in self.__event_weight_groups.keys():
            raise ValueError("Weight group '{}' has not been defined.".format(group_name))
        n_expected = len(self.__event_weight_groups[group_name]["parameter_names"])
        if len(parameter_values) != n_expected:
            raise ValueError("Expected {} parameter values but only got {}".format(n_expected, len(parameter_values)))
        self.__event_weight_groups[group_name][weight_name] = []
        for parameter_name, value in zip(self.__event_weight_groups[group_name]["parameter_names"], parameter_values):
            self.__event_weight_groups[group_name][weight_name].append((parameter_name, value))

    def __setattr__(self, key, value):
        """! Override default attribute setting to stop users setting non-existent attributes.

        @exceptions AttributeError  Raise an AttributeError if the interface is frozen

        @param key    Attribute name.
        @param value  Value to set the attribute to.
        """
        # If this is a known parameter then keep the values in sync
        if hasattr(self, "process"):
            # ... for parameters of the process
            for parameter in self.process.parameters_by_name(key):
                parameter.ensure_default()
                parameter.value = value
            # ... and for parameters of any external processes
            for external in self.process.externals.values():
                for parameter in external.parameters_by_name(key):
                    parameter.ensure_default()
                    parameter.value = value
        # If the interface is frozen then raise an attribute error
        if hasattr(self, "interface_frozen") and self.interface_frozen:
            if not hasattr(self, key):
                raise AttributeError("This POWHEG-BOX process has no option '{}'".format(key))
        object.__setattr__(self, key, value)

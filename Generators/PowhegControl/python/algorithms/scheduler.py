# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from functools import partial
from . import generators
from . import postprocessors
from . import preprocessors


## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class Scheduler(object):
    """! Schedule algorithms in appropriate order.

    @author James Robinson  <james.robinson@cern.ch>
    """

    ## Name of PowhegBox output LHE events
    powheg_LHE_output = "pwgevents.lhe"

    ## Preprocessing evaluation order
    ordered_preprocessors = ["directory cleaner", "reweighter"]

    ## Event generation evaluation order
    ordered_event_generators = ["singlecore", "multicore", "merge output", "output validation"]

    ## Postprocessing evaluation order
    ordered_postprocessors = [
        "quark colour fixer",
        "PHOTOS", "reweighter",
        "NNLO reweighter",
        "LHE file nominal weight updater",
        "MadSpin",
        "integration grid tester",
        "cross section calculator",
        "LHE file cleaner",
        "output file renamer",
        "output tarball preparer",
        "integration gridpack creator",
        "directory cleaner",
    ]

    ## Map preprocessing names to functions
    preprocessor_fn_dict = {
        "directory cleaner": preprocessors.directory_cleaner,
        "reweighter": preprocessors.reweighter
    }

    ## Map event generation names to functions
    generator_fn_dict = {
        "merge output": partial(generators.merge_output, powheg_LHE_output=powheg_LHE_output),
        "multicore": generators.multicore,
        "output validator": partial(generators.output_validator, powheg_LHE_output=powheg_LHE_output),
        "singlecore": generators.singlecore
    }

    ## Map postprocessing names to functions
    postprocessor_fn_dict = {
        "cross section calculator": partial(postprocessors.cross_section_calculator, powheg_LHE_output=powheg_LHE_output),
        "directory cleaner": postprocessors.directory_cleaner,
        "integration gridpack creator": postprocessors.integration_gridpack_creator,
        "integration grid tester": postprocessors.integration_grid_tester,
        "MadSpin": partial(postprocessors.MadSpin, powheg_LHE_output=powheg_LHE_output),
        "NNLO reweighter": partial(postprocessors.NNLO_reweighter, powheg_LHE_output=powheg_LHE_output),
        "output file renamer": partial(postprocessors.output_file_renamer, powheg_LHE_output=powheg_LHE_output),
        "output tarball preparer": postprocessors.output_tarball_preparer,
        "PHOTOS": partial(postprocessors.PHOTOS, powheg_LHE_output=powheg_LHE_output),
        "reweighter": partial(postprocessors.reweighter, powheg_LHE_output=powheg_LHE_output),
        "quark colour fixer": partial(postprocessors.quark_colour_fixer, powheg_LHE_output=powheg_LHE_output),
        "LHE file cleaner": partial(postprocessors.lhe_cleaner, powheg_LHE_output=powheg_LHE_output),
        "LHE file nominal weight updater": partial(postprocessors.lhe_nominal_weight_updater, powheg_LHE_output=powheg_LHE_output),
    }

    def __init__(self):
        """! Constructor."""
        ## Sequence of functions to run
        self.sequence = {"preprocessors": {}, "generators": {}, "postprocessors": {}}

        # Add universal components
        self.add("output validator")
        self.add("integration grid tester")
        self.add("LHE file cleaner")

    def add(self, name, *args):
        """! Add a component to the sequence.

        @param name name of component to add.
        @param args arguments to pass to this component when it runs.
        """
        if name in self.ordered_preprocessors:
            if name not in self.sequence["preprocessors"].keys():
                self.sequence["preprocessors"][name] = args

        if name in self.ordered_event_generators:
            if name not in self.sequence["generators"].keys():
                self.sequence["generators"][name] = args

        if name in self.ordered_postprocessors:
            if name not in self.sequence["postprocessors"].keys():
                self.sequence["postprocessors"][name] = args

    def print_structure(self):
        """! Print the sequence to the logger."""
        logger.info("Scheduling the following PowhegControl sequence:")
        logger.info("|---+ Preprocessing:")
        for name in sorted(self.sequence["preprocessors"].keys(), key=lambda x: self.ordered_preprocessors.index(x)):
            logger.info("    |--- {}".format(name))
        logger.info("|---+ Event generation:")
        for name in sorted(self.sequence["generators"].keys(), key=lambda x: self.ordered_event_generators.index(x)):
            logger.info("    |--- {}".format(name))
        logger.info("|---+ Postprocessing:")
        for name in sorted(self.sequence["postprocessors"].keys(), key=lambda x: self.ordered_postprocessors.index(x)):
            logger.info("    |--- {}".format(name))

    def run_preprocessors(self):
        """! Run all preprocessors in order."""
        for name in filter(lambda x: x in self.sequence["preprocessors"].keys(), self.ordered_preprocessors):
            self.preprocessor_fn_dict[name](*self.sequence["preprocessors"][name])

    def run_generators(self):
        """! Run all event generators in order."""
        for name in filter(lambda x: x in self.sequence["generators"].keys(), self.ordered_event_generators):
            self.generator_fn_dict[name](*self.sequence["generators"][name])

    def run_postprocessors(self):
        """! Run all postprocessors in order."""
        for name in filter(lambda x: x in self.sequence["postprocessors"].keys(), self.ordered_postprocessors):
            self.postprocessor_fn_dict[name](*self.sequence["postprocessors"][name])

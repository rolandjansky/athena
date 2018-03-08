# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl AfterburnerNNLOReweighting
#  NNLOPS afterburner to reweight Powheg events
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AthenaCommon.Logging import logging
import glob
import os
import re
import shutil
from xml.etree import ElementTree
from ..utility import LHEUtils, ProcessHandling

# Initialise logging handler
logger = logging.getLogger("PowhegControl")

# Convenience function to set up an instance of the reweighting class


def afterburner_NNLO_reweighting(configurator):
    reweighter = AfterburnerNNLOReweighting(configurator)
    reweighter.run_NNLO_executable()
    reweighter.postprocess_NNLO_events()


# Handler for reweighting functions
class AfterburnerNNLOReweighting(object):

    def __init__(self, configurator):
        logger.info("Initialising NNLO reweighting")

        # Read-in inputs and outputs
        self._LHEF_name = "pwgevents.lhe"
        self._LHEF_nnlo_name = "pwgevents.lhe.NNLO"
        self._NNLO_reweighting_inputs = configurator.NNLO_reweighting_inputs
        self._NNLO_output_weights = configurator.NNLO_output_weights
        self._nominal_group_name = [x[1] for x in configurator.fixed_parameters if x[0] == "lhrwgt_group_name"][0]

        # Strip comment strings from input LHEF file - reweighter will crash otherwise
        logger.info("Removing comments from input LHE file")
        os.system(r"sed -i 's/\!.*$//g' {}".format(self._LHEF_name))

        # Construct weight descriptor sets for consistent ordering
        logger.info("Constructing list of weights")
        self._NNLO_weight_descriptors, existing_weights = [], configurator.weight_description_to_ID.items()[:]
        for idx, (NNLO_weight_ID, NNLO_weight_description) in enumerate(self._NNLO_output_weights.items(), start=4001):
            weight_ID, weight_description = NNLO_weight_ID, NNLO_weight_description.replace("muR = 1.0, muF = 1.0", "nominal")
            # Check whether any of the NNLO descriptions refer to existing weights
            for existing_weight_description, existing_weight_ID in existing_weights:
                # Check for both '$description' and "$description"
                re_match = re.search( r"""["']{}["']""".format(existing_weight_description), weight_description )
                if re_match is not None:
                    weight_description = weight_description.replace(re_match.group(0), re_match.group(0).replace(existing_weight_description, str(existing_weight_ID)))
                    logger.info("... identified '{}' as weight ID '{}'".format(existing_weight_description, existing_weight_ID))
            # If a match is found then replace description -> ID in the string and store in the map
            configurator.weight_description_to_ID[weight_description] = weight_ID
            self._NNLO_weight_descriptors.append((idx, weight_ID, weight_description))

        # Get NNLO executable
        self._NNLO_executable = self.get_executable(configurator._powheg_executable)
        logger.info("Using NNLO reweighter at {}".format(self._NNLO_executable))

        # Get reweighting file
        for NNLO_reweighting_file_name in self._NNLO_reweighting_inputs.values():
            if os.path.isfile(NNLO_reweighting_file_name):
                logger.info("Using local configuration file {}".format(NNLO_reweighting_file_name))
            else:
                logger.info("Trying to acquire configuration file {}".format(NNLO_reweighting_file_name))
                try:
                    shutil.copy(glob.glob("{}/../AuxFiles/*/{}".format(configurator.powheg_directory, NNLO_reweighting_file_name))[0], NNLO_reweighting_file_name)
                except (OSError, IndexError):
                    logger.warning("{} does not exist!".format(NNLO_reweighting_file_name))
                    logger.warning("(1) if you generated this file then please ensure it is visible to Powheg on-the-fly.")
                    logger.warning("(2) if you expected Powheg on-the-fly to know about this file, please submit a bug report.")

    # Run the NNLO executable
    def run_NNLO_executable(self):
        # Run NNLOPS
        if "nnlopsreweighter" in self._NNLO_executable:
            self.construct_NNLOPS_runcard()
            processes = [ProcessHandling.SingleProcessThread(self._NNLO_executable)]
            manager = ProcessHandling.ProcessManager(processes)
            while manager.monitor():
                pass
            # Rename output
            shutil.move("pwgevents.lhe.nnlo", self._LHEF_nnlo_name)

        # Run DYNNLO
        elif "minnlo" in self._NNLO_executable:
            # Stage 1 - produce MINLO-W*-denom.top files
            stage_1_command = self.construct_DYNNLO_argument_list()
            logger.info("Running reweighting stage 1: denominator calculation")
            processes = [ProcessHandling.SingleProcessThread(stage_1_command)]
            manager = ProcessHandling.ProcessManager(processes)
            while manager.monitor():
                pass

            # Stage 2 - produce MINLO-W*-denom.top files
            stage_2_command = stage_1_command + ["MINLO-W{}-denom.top".format(idx) for idx in range(1, len(self._NNLO_reweighting_inputs) + 1)]
            logger.info("Running reweighting stage 2: reweighting with pre-calculated denominators")
            processes = [ProcessHandling.SingleProcessThread(stage_2_command)]
            manager = ProcessHandling.ProcessManager(processes)
            while manager.monitor():
                pass

            # Rename output
            shutil.move("pwgevents.lhe-nnlo", self._LHEF_nnlo_name)

    # Postprocessing for NNLOPS and DYNNLO events
    def postprocess_NNLO_events(self):
        shutil.move(self._LHEF_name, "{}.NLO".format(self._LHEF_name))
        logger.info("Reformatting NNLO reweighting output")
        # Run NNLOPS
        if "nnlopsreweighter" in self._NNLO_executable:
            self.postprocess_NNLOPS_events()
        # Run DYNNLO
        elif "minnlo" in self._NNLO_executable:
            self.postprocess_DYNNLO_events()

    # Determine the path to the appropriate executable
    def get_executable(self, powheg_executable):
        process = powheg_executable.split("/")[-2]
        if process == "HJ":
            relative_path = "nnlopsreweighter"
        elif process == "Wj":
            relative_path = "DYNNLOPS/WNNLOPS/Reweighter/minnlo"
        elif process == "Zj":
            relative_path = "DYNNLOPS/ZNNLOPS/Reweighter/minnlo"
        return powheg_executable.replace("pwhg_main", relative_path)

    # Construct the argument list needed by DYNNLOPS reweighter
    def construct_DYNNLO_argument_list(self):
        DYNNLO_top_files = self._NNLO_reweighting_inputs.values()
        return [self._NNLO_executable, "pwgevents.lhe", len(DYNNLO_top_files)] + DYNNLO_top_files

    # Construct the run card needed by NNLOPSreweighter
    def construct_NNLOPS_runcard(self):
        logger.info("Constructing NNLOPS run card")
        # Prepare the nnlopsreweighter runcard
        with open("nnlopsreweighter.input", "wb") as f_input_card:
            # Name of the input LHE file
            f_input_card.write("lhfile {}\n\n".format(self._LHEF_name))

            # Header of "nnlofiles" followed by a quoted label and the name of a HNNLO output file.
            f_input_card.write("nnlofiles\n")
            for label, NNLO_reweighting_file_name in self._NNLO_reweighting_inputs.items():
                f_input_card.write("'{}' {}\n".format(label, NNLO_reweighting_file_name))
            f_input_card.write("\n")

            # NNLOPS weights, in LHEv3 format
            # Description line tells reweighter how to calculate weights:
            #  *) loops through the weight IDs in the LHEF file and through the labels of the nnlofiles.
            #  *) if description contains a weight-label and an nnlofile-label:
            #     - compute new weight by reweighting the corresponding weights in the
            #       input file with the result from the nnlofile
            f_input_card.write("<initrwgt>\n")
            f_input_card.write("<weightgroup name='NNLOPS'>\n")
            for _, weight_ID, weight_description in self._NNLO_weight_descriptors:
                f_input_card.write("<weight id='{}'> {} </weight>\n".format(weight_ID, weight_description))
            f_input_card.write("</weightgroup>\n")
            f_input_card.write("</initrwgt>\n")

    def postprocess_NNLOPS_events(self):
        logger.info("Renaming NNLO weights")
        # Rename NNLO weights to fit ATLAS conventions - could be rewritten to use XML parsing, but this may be overkill
        with open(self._LHEF_name, "wb") as f_output:
            with open(self._LHEF_nnlo_name, "rb") as f_input:
                for input_line in f_input:
                    output_line = input_line
                    for idx, weight_ID, weight_description in self._NNLO_weight_descriptors:
                        # Switch weight ID/description naming
                        if "id='{}'".format(weight_ID) in output_line:
                            output_line = output_line.replace(weight_ID, str(idx)).replace(weight_description, weight_ID)
                    # Convert exponential delimiter
                    if "wgt id=" in output_line:
                        output_line = output_line.replace("D+", "E+").replace("D-", "E-")
                    if len(output_line) > 1:  # strip empty lines
                        f_output.write(output_line)

    def postprocess_DYNNLO_events(self):
        logger.info("Converting output to LHEv3 format")

        # Extract intro, header and footer
        opening_string = LHEUtils.get_opening_string(self._LHEF_nnlo_name)
        closing_string = LHEUtils.get_closing_string(self._LHEF_nnlo_name)
        if opening_string.find("<header>") != -1:
            intro = opening_string[: opening_string.find("<header>")]
            header = opening_string[opening_string.find("<header>"): opening_string.find("</header>") + 9]
        else:
            intro = opening_string[: opening_string.find("<init>")]
            header = "<header></header>"
        init = opening_string[opening_string.find("<init>"): opening_string.find("</init>") + 8]
        if closing_string.find("<LesHouchesEvents>") != -1:
            footer = closing_string[closing_string.find("</LesHouchesEvents>"):]
        else:
            footer = "</LesHouchesEvents>"

        # Add nominal weight to header
        logger.info("Adding LHEv3 weight: nominal")
        header_elem = LHEUtils.add_weight_to_header(header, self._nominal_group_name, "nominal", 0)

        # Get list of any weights already used in the range 4000-5000
        weight_name_to_id = {}
        try:
            w_elems = header_elem.iter(tag="weight")  # needs python >= 2.7
        except AttributeError:
            w_elems = header_elem.getiterator(tag="weight")
        weight_number_offset = max(filter(lambda w: 4000 <= w < 5000, [int(w_elem.attrib["id"]) for w_elem in w_elems]) + [4000])

        # Add DYNNLO weights to header
        dynnlo_weight_names = [x[0] for x in LHEUtils.string_to_weight(LHEUtils.get_first_event(self._LHEF_nnlo_name))]
        for idx, weight_name in enumerate(dynnlo_weight_names, start=1):
            logger.info("Adding LHEv3 weight: {}".format(weight_name))
            weight_name_to_id[weight_name] = weight_number_offset + idx
            header_elem = LHEUtils.add_weight_to_header(header_elem, "dynnlo", weight_name, weight_name_to_id[weight_name])

        # Convert Powheg input events to LHEv3 output ones
        logger.info("Converting Powheg output into LHEv3 format")
        with open(self._LHEF_name, "wb") as f_output:
            f_output.write(intro)
            ElementTree.ElementTree(header_elem).write(f_output)
            f_output.write(init)
            for event in LHEUtils.event_iterator(self._LHEF_nnlo_name):
                f_output.write(LHEUtils.Powheg2LHEv3(event, weight_name_to_id))
            f_output.write(footer)

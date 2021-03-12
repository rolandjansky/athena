# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import glob
import os
import re
import shutil
from AthenaCommon import Logging
from ...decorators import timed
from ...utility import LHE, ProcessManager, SingleProcessThread
from xml.etree import ElementTree


## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("NNLO reweighter")
def NNLO_reweighter(process, powheg_LHE_output):
    """! Move output to correctly named file.

    @param process            External NNLOPS process.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    process.expose()
    logger.info("Initialising NNLO reweighting")
    output_LHE_name = "{}.NNLO".format(powheg_LHE_output)
    aux_directory = os.path.join(*(process.executable.split("POWHEG")[:-1] + ["AuxFiles"]))

    # Get NNLO executable
    logger.info("Using NNLO reweighter at {}".format(process.executable))

    # Strip comment strings from input LHEF file - reweighter will crash otherwise
    logger.info("Removing comments from input LHE file")
    os.system(r"sed -i 's/\!.*$//g' {}".format(powheg_LHE_output))

    acquire_NNLO_inputs(aux_directory, process.NNLO_reweighting_inputs)
    run_NNLO_executable(process, powheg_LHE_output, output_LHE_name)
    reformat_NNLO_events(process, powheg_LHE_output, output_LHE_name)


def acquire_NNLO_inputs(aux_directory, NNLO_reweighting_inputs):
    """! Get reweighting files from AuxFiles directory if not provided with job."""
    for NNLO_reweighting_file_name in NNLO_reweighting_inputs.values():
        logger.info("Looking for configuration file {}...".format(NNLO_reweighting_file_name))
        if os.path.isfile(NNLO_reweighting_file_name):
            logger.info("... found locally")
        else:
            try:
                shutil.copy(glob.glob("{}/*/{}".format(aux_directory, NNLO_reweighting_file_name))[0], NNLO_reweighting_file_name)
                logger.info("... found in Powheg OTF auxiliary directory")
            except (OSError, IndexError):
                logger.warning("... NOT found!")
                logger.warning("(1) if you generated this file then please ensure it is visible to Powheg on-the-fly.")
                logger.warning("(2) if you expected Powheg on-the-fly to know about this file, please submit a bug report.")
                raise IOError("NNLO reweighting inputs not found.")


def run_NNLO_executable(process, powheg_LHE_output, output_LHE_name):
    """! Run the NNLO executable."""
    # Run NNLOPS
    NNLO_executable = process.executable
    if "nnlopsreweighter" in NNLO_executable:
        process.NNLO_weight_list = construct_NNLOPS_weight_list(process.NNLO_output_weights, powheg_LHE_output)
        run_NNLOPS_executable(NNLO_executable, process.NNLO_reweighting_inputs, process.NNLO_weight_list, powheg_LHE_output, output_LHE_name)
    # Run DYNNLO
    elif "minnlo" in NNLO_executable:
        run_DYNNLO_executable(NNLO_executable, process.NNLO_reweighting_inputs, powheg_LHE_output, output_LHE_name)


def run_NNLOPS_executable(NNLO_executable, NNLO_reweighting_inputs, NNLO_weight_list, powheg_LHE_output, output_LHE_name):
    """! Run NNLOPSreweighter with appropriate run card."""
    # Prepare the nnlopsreweighter runcard
    logger.info("Constructing NNLOPS run card")
    with open("nnlopsreweighter.input", "w") as f_input_card:
        # Name of the input LHE file
        f_input_card.write("lhfile {}\n\n".format(powheg_LHE_output))

        # Header of "nnlofiles" followed by a quoted label and the name of a HNNLO output file.
        f_input_card.write("nnlofiles\n")
        for label, NNLO_reweighting_file_name in NNLO_reweighting_inputs.items():
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
        for _, weight_ID, weight_name in NNLO_weight_list:
            f_input_card.write("<weight id='{}'> {} </weight>\n".format(weight_ID, weight_name))
        f_input_card.write("</weightgroup>\n")
        f_input_card.write("</initrwgt>\n")

    # Run the executable until finished
    if not os.path.isfile(NNLO_executable):
        raise OSError("NNLO reweighting executable {} not found!".format(NNLO_executable))
    manager = ProcessManager([SingleProcessThread(NNLO_executable)])
    while manager.monitor():
        pass

    # Rename output
    shutil.move("pwgevents.lhe.nnlo", output_LHE_name)


def run_DYNNLO_executable(NNLO_executable, NNLO_reweighting_inputs, powheg_LHE_output, output_LHE_name):
    """! Run DYNNLOPS reweighter with appropriate arguments."""
    # Stage 1 - produce MINLO-W*-denom.top files
    stage_1_command = [NNLO_executable, powheg_LHE_output, len(NNLO_reweighting_inputs)] + NNLO_reweighting_inputs.values()
    logger.info("Running reweighting stage 1: denominator calculation")
    manager = ProcessManager([SingleProcessThread(stage_1_command)])
    while manager.monitor():
        pass

    # Stage 2 - produce MINLO-W*-denom.top files
    stage_2_command = stage_1_command + ["MINLO-W{}-denom.top".format(idx) for idx in range(1, len(NNLO_reweighting_inputs) + 1)]
    logger.info("Running reweighting stage 2: reweighting with pre-calculated denominators")
    manager = ProcessManager([SingleProcessThread(stage_2_command)])
    while manager.monitor():
        pass

    # Rename output
    shutil.move("pwgevents.lhe-nnlo", output_LHE_name)


def reformat_NNLO_events(process, powheg_LHE_output, output_LHE_name):
    """! Reformat NNLOPS and DYNNLO events."""
    shutil.move(powheg_LHE_output, "{}.NLO".format(powheg_LHE_output))
    logger.info("Reformatting NNLO reweighting output")
    NNLO_executable = process.executable
    # Run NNLOPS
    if "nnlopsreweighter" in NNLO_executable:
        reformat_NNLOPS_events(process.NNLO_weight_list, powheg_LHE_output, output_LHE_name)
    # Run DYNNLO
    elif "minnlo" in NNLO_executable:
        reformat_DYNNLO_events(powheg_LHE_output, output_LHE_name)


def reformat_NNLOPS_events(NNLO_weight_list, powheg_LHE_output, output_LHE_name):
    """! Reformat NNLOPS events to fit ATLAS conventions."""
    logger.info("Renaming NNLO weights")
    # Rename NNLO weights to fit ATLAS conventions - could be rewritten to use XML parsing, but this may be overkill
    with open(powheg_LHE_output, "w") as f_output:
        with open(output_LHE_name, "r") as f_input:
            for input_line in f_input:
                output_line = input_line
                for idx, weight_ID, weight_name in NNLO_weight_list:
                    # Switch weight ID/description naming
                    if "id='{}'".format(weight_ID) in output_line:
                        output_line = output_line.replace(weight_ID, str(idx)).replace(weight_name, weight_ID)
                # Convert exponential delimiter
                if "wgt id=" in output_line:
                    output_line = output_line.replace("D+", "E+").replace("D-", "E-")
                if len(output_line) > 1:  # strip empty lines
                    f_output.write(output_line)


def reformat_DYNNLO_events(powheg_LHE_output, input_LHE_name):
    """! Reformat DYNNLO events to fit ATLAS conventions."""
    logger.info("Converting output to LHEv3 format")

    # Extract intro, header and footer
    intro = "\n".join([elem for elem in [LHE.opening_tag(input_LHE_name), LHE.comment_block(input_LHE_name)] if elem])
#    header = LHE.header_block(input_LHE_events)
#    init = LHE.init_block(input_LHE_events)
    header = LHE.header_block(input_LHE_name)
    init = LHE.init_block(input_LHE_name)
    closing_string = LHE.postamble(input_LHE_name)
    if closing_string.find("<LesHouchesEvents>") != -1:
        footer = closing_string[closing_string.find("</LesHouchesEvents>"):]
    else:
        footer = "</LesHouchesEvents>"

    # Add nominal weight to header
    logger.info("Adding LHEv3 weight: nominal")
    header_elem = LHE.add_weight_to_header(header, "nominal", "nominal", 0)

    # Get list of any weights already used in the range 4000-5000
    weight_name_to_id = {}
    try:
        w_elems = header_elem.iter(tag="weight")  # needs python >= 2.7
    except AttributeError:
        w_elems = header_elem.getiterator(tag="weight")
    weight_number_offset = max(filter(lambda w: 4000 <= w < 5000, [int(w_elem.attrib["id"]) for w_elem in w_elems]) + [4000])

    # Add DYNNLO weights to header
    dynnlo_weight_names = [x[0] for x in LHE.string_to_weight(LHE.get_first_event(input_LHE_name))]
    for idx, weight_name in enumerate(dynnlo_weight_names, start=1):
        logger.info("Adding LHEv3 weight: {}".format(weight_name))
        weight_name_to_id[weight_name] = weight_number_offset + idx
        header_elem = LHE.add_weight_to_header(header_elem, "dynnlo", weight_name, weight_name_to_id[weight_name])

    # Convert Powheg input events to LHEv3 output ones
    logger.info("Converting Powheg output into LHEv3 format")
    with open(powheg_LHE_output, "w") as f_output:
        f_output.write("{}\n".format(intro))
        ElementTree.ElementTree(header_elem).write(f_output)
        f_output.write("{}\n".format(init))
        for event in LHE.event_iterator(input_LHE_name):
            f_output.write(LHE.Powheg2LHEv3(event, weight_name_to_id))
        f_output.write(footer)


def construct_NNLOPS_weight_list(NNLO_output_weights, powheg_LHE_output):
    """! Construct list of NNLO weights."""
    # Construct list of tuples (weight_ID, weight_name) for existing weights
    existing_weights, NNLO_weights = [], []
    with open(powheg_LHE_output, "r") as f_input:
        for line in f_input:
            if "weight id=" in line:
                existing_weights.append((str(int(line.split("'")[1])), line.split(">")[1].split("<")[0].strip()))

    # Construct weight descriptor sets for consistent ordering
    logger.info("Constructing list of weights")
    for idx, (NNLO_weight_name, NNLOPS_command) in enumerate(NNLO_output_weights.items(), start=4001):
        NNLOPS_command = NNLOPS_command.replace("muR = 1.0, muF = 1.0", "nominal")
        # Check whether any of the NNLO descriptions refer to existing weights
        for weight_ID, weight_name in existing_weights:
            # Check for both '$description' and "$description"
            re_match = re.search(r"""["']{}["']""".format(weight_name), NNLOPS_command)
            if re_match is not None:
                NNLOPS_command = NNLOPS_command.replace(re_match.group(0), re_match.group(0).replace(weight_name, weight_ID))
                logger.info("... identified '{}' as weight ID '{}'".format(weight_name, weight_ID))
        # Add this to the list of known weights
        existing_weights.append((idx, NNLO_weight_name))
        NNLO_weights.append((idx, NNLO_weight_name, NNLOPS_command))
    return NNLO_weights

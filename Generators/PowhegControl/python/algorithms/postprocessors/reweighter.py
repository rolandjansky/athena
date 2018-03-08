# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser
from ..generators import singlecore_untimed
import collections
import os
import shutil

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

## Tuple to contain arbitrary grouped weights
WeightTuple = collections.namedtuple("WeightTuple", ["parameter_settings", "keywords", "ID", "name", "combine", "group"])


@timed("reweighting")
def reweighter(process, weight_groups, powheg_LHE_output):
    """! Add a series of additional weights to an LHE file.

    @param process            PowhegBox process.
    @param weight_groups      OrderedDict containing groups of weights to add.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.
    """
    # List of tuples holding reweighting information
    weight_list = []

    # Initial values for scale, PDF and other weight groups
    _idx_scale_start, _idx_PDF_start, _idx_other_start = 1001, 2001, 3001

    # Construct ordered list of weights
    for group_name, weight_group in weight_groups.items():
        keyword_dict = dict((n, k) for n, k in zip(weight_group["parameter_names"], weight_group["keywords"]))
        method = weight_group["combination_method"]
        logger.info("Preparing weight group: {:<19} with {} weights".format(group_name, len(weight_group)))

        # Nominal variation: ID=0
        if group_name == "nominal":
            weight_list.append(WeightTuple(parameter_settings=weight_group["nominal"], keywords=keyword_dict, ID=0, name="nominal", combine=method, group=group_name))

        # Scale variations: ID=1001+
        elif group_name == "scale_variation":
            for idx, name in enumerate([k for k in weight_group.keys() if k not in ["parameter_names", "combination_method", "keywords"]], start=_idx_scale_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], keywords=keyword_dict, ID=idx, name=name, combine=method, group=group_name))
            _idx_scale_start = idx + 1

        # PDF variations: ID=2001+
        elif group_name == "PDF_variation":
            for idx, name in enumerate([k for k in weight_group.keys() if k not in ["parameter_names", "combination_method", "keywords"]], start=_idx_PDF_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], keywords=keyword_dict, ID=idx, name=name, combine=method, group=group_name))
            _idx_PDF_start = idx + 1

        # Other variations: ID=3001+
        else:
            for idx, name in enumerate([k for k in weight_group.keys() if k not in ["parameter_names", "combination_method", "keywords"]], start=_idx_other_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], keywords=keyword_dict, ID=idx, name=name, combine=method, group=group_name))
            _idx_other_start = idx + 1

    # Construct xml output
    xml_lines, weightgroup = [], ""
    for weight in weight_list:
        # Group elements
        if weight.group != weightgroup:
            xml_lines.append("</weightgroup>")
            weightgroup = weight.group
            xml_lines.append("<weightgroup name='{}'>".format(weightgroup))
        # Weight elements
        keyword_pairs = ["{}={}".format(k, v) for p, v in weight.parameter_settings for k in weight.keywords[p]]
        xml_lines.append("<weight id='{}'> {} </weight>".format(weight.ID, " ".join(keyword_pairs)))
    xml_lines.append(xml_lines.pop(0))

    # Write xml output
    with open("reweighting_input.xml", "wb") as f_rwgt:
        f_rwgt.write("<initrwgt>\n")
        for xml_line in xml_lines:
            f_rwgt.write("{}\n".format(xml_line))
        f_rwgt.write("</initrwgt>")

    # Make backup of generation statistics
    if os.path.isfile("pwgcounters.dat"):
        shutil.copy("pwgcounters.dat", "pwgcounters.dat.bak")

    # # Iterate over variations
    # for idx_weight, weight in enumerate(weight_list, start=1):
    #     add_single_weight(process, weight, idx_weight, len(weight_list))

    # Add reweighting lines to runcard
    FileParser("powheg.input").text_replace("rwl_file .*", "rwl_file 'reweighting_input.xml'")
    FileParser("powheg.input").text_replace("rwl_add .*", "rwl_add 1")
    FileParser("powheg.input").text_replace("clobberlhe .*", "clobberlhe 1")

    # Copy the old events and then run the reweighter
    shutil.copy(powheg_LHE_output, "{}.unweighted".format(powheg_LHE_output))
    logger.info("Preparing to add {} weights to generated events.".format(len(weight_list)))
    singlecore_untimed(process)

    # Move the reweighted file back
    shutil.move(powheg_LHE_output.replace(".lhe", "-rwgt.lhe"), powheg_LHE_output)

    # Remove rwgt and pdf lines, which crash Pythia
    FileParser(powheg_LHE_output).text_remove("^#pdf")
    FileParser(powheg_LHE_output).text_remove("^#rwgt")
    FileParser(powheg_LHE_output).text_remove("^#new weight")

    # Rename all weights
    for weight in weight_list:
        FileParser(powheg_LHE_output).text_replace(".* id='{}' .*".format(weight.ID), "<weight id='{}'> {} </weight>".format(weight.ID, weight.name))

    # Correct LHE version identification; otherwise Pythia will treat all files as v1
    FileParser(powheg_LHE_output).text_replace('LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"')

    # Restore generation statistics and initial runcard
    shutil.move("powheg_nominal.input", "powheg.input")
    if os.path.isfile("pwgcounters.dat.bak"):
        shutil.move("pwgcounters.dat.bak", "pwgcounters.dat")


# def add_single_weight(process, weight, idx_weight, n_total):
#     """! Add a single additional weight to an LHE file.
#
#     @param process       PowhegBox process.
#     @param weight     Tuple with details of the weight to be added.
#     @param idx_weight Which number this weight is.
#     @param n_total    Total number of weights to add.
#     """
#     @timed("weight variation {}/{}".format(idx_weight, n_total))
#     def __timed_inner_fn():
#         logger.info("... weight name is:     {}".format(weight.name))
#         logger.info("... weight index ID is: {}".format(weight.ID))
#         shutil.copy("powheg_nominal.input", "powheg.input")
#
#         # As the nominal process has already been run, turn on compute_rwgt
#         FileParser("powheg.input").text_replace("compute_rwgt 0", "compute_rwgt 1")
#
#         # Ensure that manyseeds is turned off,  as this would cause the reweighting to crash
#         FileParser("powheg.input").text_replace("manyseeds .*", "manyseeds 0")
#         FileParser("powheg.input").text_replace("parallelstage .*", "parallelstage -1")
#
#         # Apply weight settings
#         for (parameter, value) in weight.parameter_settings:
#             try:
#                 for keyword in weight.keywords[parameter]:
#                     FileParser("powheg.input").text_replace("^{}.*".format(keyword), "{} {}".format(keyword, value))
#                 logger.info("... setting {} to {}".format(parameter, value))
#             except KeyError:
#                 logger.warning("Parameter '{}' not recognised. Cannot reweight!".format(parameter))
#
#         # Set reweighting metadata
#         FileParser("powheg.input").text_replace("lhrwgt_descr .*", "lhrwgt_descr '{}'".format(weight.name))
#         FileParser("powheg.input").text_replace("lhrwgt_id .*", "lhrwgt_id '{}'".format(weight.ID))
#         FileParser("powheg.input").text_replace("lhrwgt_group_combine .*", "lhrwgt_group_combine '{}'".format(weight.combine))
#         FileParser("powheg.input").text_replace("lhrwgt_group_name .*", "lhrwgt_group_name '{}'".format(weight.group))
#
#         # Run the process until termination
#         untimed_generation_singlecore(process)
#         shutil.move("pwgevents-rwgt.lhe", "pwgevents.lhe")
#
#     # Run single weight variation
#     __timed_inner_fn()

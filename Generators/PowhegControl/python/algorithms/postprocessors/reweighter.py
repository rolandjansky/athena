# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser
from ..generators import multicore_untimed, singlecore_untimed
import collections
import os
import shutil

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

## Tuple to contain arbitrary grouped weights
WeightTuple = collections.namedtuple("WeightTuple", ["parameter_settings", "keywords", "ID", "name", "combine", "group", "parallel_xml_compatible"])


@timed("reweighting")
def reweighter(process, weight_groups, powheg_LHE_output):
    """! Add a series of additional weights to an LHE file.

    @param process            PowhegBox process.
    @param weight_groups      OrderedDict containing groups of weights to add.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.
    """
    ## List of tuples holding reweighting information
    weight_list = []

    ## List of attributes of a weight_group that are not themselves a weight
    non_weight_attributes = ["parameter_names", "combination_method", "keywords"]

    ## Dictionary of available keywords for faster XML-reweighting: from rwl_setup_param_weights.f
    xml_kwds = {"renscfact": "renscfact", "facscfact": "facscfact", "lhans1": "lhapdf", "lhans2": "lhapdf"}

    # Initial values for scale, PDF and other weight groups
    _idx_scale_start, _idx_PDF_start, _idx_other_start = 1001, 2001, 3001

    if not process.use_XML_reweighting:
        logger.warning("... XML-style reweighting is not enabled for this process. Will use old (multiple-pass) method.")

    # Construct ordered list of weights
    for group_name, weight_group in weight_groups.items():
        logger.info("Preparing weight group: {:<19} with {} weights".format(group_name, len(weight_group) - len(non_weight_attributes)))

        # Name -> keyword dictionary is different if this is an XML reweighting
        is_parallel_xml_compatible = process.use_XML_reweighting and all([k in xml_kwds.keys() for _kw_set in weight_group["keywords"] for k in _kw_set])
        if is_parallel_xml_compatible:
            _keywords = [list(set([xml_kwds[k] for k in _kw_set])) for _kw_set in weight_group["keywords"]]
        else:
            if process.use_XML_reweighting:
                logger.warning("... this weight group is incompatible with XML-style reweighting. Will use old (multiple-pass) method.")
            _keywords = weight_group["keywords"]
        keyword_dict = dict((n, k) for n, k in zip(weight_group["parameter_names"], _keywords))

        # Common arguments for all weights
        tuple_kwargs = {"keywords": keyword_dict, "combine": weight_group["combination_method"], "group": group_name, "parallel_xml_compatible": is_parallel_xml_compatible}

        # Nominal variation: ID=0
        if group_name == "nominal":
            weight_list.append(WeightTuple(parameter_settings=weight_group["nominal"], ID=0, name="nominal", **tuple_kwargs))

        # Scale variations: ID=1001+
        elif group_name == "scale_variation":
            for idx, name in enumerate([k for k in weight_group.keys() if k not in non_weight_attributes], start=_idx_scale_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], ID=idx, name=name, **tuple_kwargs))
            _idx_scale_start = idx + 1

        # PDF variations: ID=2001+
        elif group_name == "PDF_variation":
            for idx, name in enumerate([k for k in weight_group.keys() if k not in non_weight_attributes], start=_idx_PDF_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], ID=idx, name=name, **tuple_kwargs))
            _idx_PDF_start = idx + 1

        # Other variations: ID=3001+
        else:
            for idx, name in enumerate([k for k in weight_group.keys() if k not in non_weight_attributes], start=_idx_other_start):
                weight_list.append(WeightTuple(parameter_settings=weight_group[name], ID=idx, name=name, **tuple_kwargs))
            _idx_other_start = idx + 1

    # Make backup of generation statistics
    if os.path.isfile("pwgcounters.dat"):
        shutil.copy("pwgcounters.dat", "pwgcounters.dat.bak")

    # .. and also backup unweighted events
    try:
        shutil.copy(powheg_LHE_output, "{}.unweighted".format(powheg_LHE_output))
    except IOError:
        raise IOError("Nominal LHE file could not be found. Probably POWHEG-BOX crashed during event generation.")

    # Do XML-reweighting
    if process.use_XML_reweighting:
        # Add nominal weight if not already present
        if not any([weight.group == "nominal" for weight in weight_list]):
            weight_list = [WeightTuple(ID=0, name="nominal", group="nominal", parallel_xml_compatible=True, parameter_settings=[], keywords=None, combine=None)] + weight_list

        # Construct xml output
        xml_lines, serial_xml_weight_list, current_weightgroup = [], [], None
        for weight in weight_list:
            # Group elements
            if weight.parallel_xml_compatible and weight.group != current_weightgroup:
                xml_lines.append("</weightgroup>")
                current_weightgroup = weight.group
                xml_lines.append("<weightgroup name='{}' combine='{}'>".format(weight.group, weight.combine))
            # Weight elements
            if weight.parallel_xml_compatible:
                keyword_pairs_str = " ".join(["{}={}".format(k, v) for p, v in weight.parameter_settings for k in weight.keywords[p]])
                xml_lines.append("<weight id='{}'> {} </weight>".format(weight.ID, keyword_pairs_str))
            else:
                serial_xml_weight_list.append(weight)
        xml_lines.append(xml_lines.pop(0)) # move the closing tag to the end
        n_parallel_xml_weights = len(weight_list) - len(serial_xml_weight_list)

        # First iterate over the simultaneous XML weights
        if n_parallel_xml_weights > 0:
            # Write xml output
            with open("reweighting_input.xml", "wb") as f_rwgt:
                f_rwgt.write("<initrwgt>\n")
                [f_rwgt.write("{}\n".format(xml_line)) for xml_line in xml_lines]
                f_rwgt.write("</initrwgt>")

            # Add reweighting lines to runcard
            FileParser("powheg.input").text_replace("rwl_file .*", "rwl_file 'reweighting_input.xml'")
            FileParser("powheg.input").text_replace("rwl_add .*", "rwl_add 1")
            FileParser("powheg.input").text_replace("clobberlhe .*", "clobberlhe 1")

            logger.info("Preparing simultaneous calculation of {} additional weights for generated events.".format(n_parallel_xml_weights))

            # Allow RES processes to do their reweighting with manyseeds enabled, or they will look for the wrong files
            if process.parameters_by_name("manyseeds")[0].value == 1:
                if process.powheg_version == "RES":
                    multicore_untimed(process)
                else:
                    FileParser("powheg.input").text_replace("manyseeds .*", "manyseeds 0")
                    FileParser("powheg.input").text_replace("parallelstage .*", "parallelstage -1")
                    singlecore_untimed(process)
            else:
                singlecore_untimed(process)

            # Move the reweighted file back
            rename_LHE_output(powheg_LHE_output)

        # Iterate over any variations which require non-simultaneous reweighting
        if len(serial_xml_weight_list) > 0:
            logger.info("Preparing individual calculation of {} additional weights for generated events.".format(len(serial_xml_weight_list)))
            shutil.move("reweighting_input.xml", "reweighting_input.nominal")
            for idx_weight, weight in enumerate(serial_xml_weight_list, start=1):
                add_single_weight(process, weight, idx_weight, len(serial_xml_weight_list), use_XML=True)
                rename_LHE_output(powheg_LHE_output)
            shutil.move("reweighting_input.nominal", "reweighting_input.xml")

    # Do non-XML-reweighting
    else:
        # Iterate over any variations which require old-style reweighting
        logger.info("Preparing individual calculation of {} additional weights for generated events.".format(len(weight_list)))
        for idx_weight, weight in enumerate(weight_list, start=1):
            add_single_weight(process, weight, idx_weight, len(weight_list), use_XML=False)
            rename_LHE_output(powheg_LHE_output)

    # Remove rwgt and pdf lines, which crash Pythia
    FileParser(powheg_LHE_output).text_remove("^#pdf")
    FileParser(powheg_LHE_output).text_remove("^#rwgt")
    FileParser(powheg_LHE_output).text_remove("^#new weight")

    # # Strip NaN values from rwgt and pdf lines, which crash Pythia (even versions which accept the comment lines)
    # FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#pdf")
    # FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#rwgt")
    # FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#new weight")

    # Rename all weights
    for weight in weight_list:
        FileParser(powheg_LHE_output).text_replace(".* id='{}' .*".format(weight.ID), "<weight id='{}'> {} </weight>".format(weight.ID, weight.name))

    # Correct LHE version identification; otherwise Pythia will treat all files as v1
    FileParser(powheg_LHE_output).text_replace('LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"')

    # Restore generation statistics and initial runcard
    shutil.move("powheg_nominal.input", "powheg.input")
    if os.path.isfile("pwgcounters.dat.bak"):
        shutil.move("pwgcounters.dat.bak", "pwgcounters.dat")

def add_single_weight(process, weight, idx_weight, n_total, use_XML):
    """! Add a single additional weight to an LHE file.

    @param process    PowhegBox process.
    @param weight     Tuple with details of the weight to be added.
    @param idx_weight Which number this weight is.
    @param n_total    Total number of weights being added.
    @param use_XML    Whether to use XML or old-style reweighting.
    """
    @timed("weight variation {}/{}".format(idx_weight, n_total))
    def __timed_inner_fn():
        logger.info("... weight name is:     {}".format(weight.name))
        logger.info("... weight index ID is: {}".format(weight.ID))
        shutil.copy("powheg_nominal.input", "powheg.input")

        # Make appropriate runcard changes
        if use_XML:
            FileParser("powheg.input").text_replace("rwl_file .*", "rwl_file 'reweighting_input.xml'")
            FileParser("powheg.input").text_replace("rwl_add .*", "rwl_add 1")
            FileParser("powheg.input").text_replace("clobberlhe .*", "clobberlhe 1")
        else:
            # As the nominal process has already been run, turn on compute_rwgt
            FileParser("powheg.input").text_replace("compute_rwgt 0", "compute_rwgt 1")
            # Ensure that manyseeds/parallelstage are turned off, as these cause the reweighting to crash
            FileParser("powheg.input").text_replace("manyseeds .*", "manyseeds 0")
            FileParser("powheg.input").text_replace("parallelstage .*", "parallelstage -1")

        # Apply weight settings
        for (parameter, value) in weight.parameter_settings:
            try:
                for keyword in weight.keywords[parameter]:
                    FileParser("powheg.input").text_replace("^{} .*".format(keyword), "{} {}".format(keyword, value))
                logger.info("... setting {} to {}".format(parameter, value))
            except KeyError:
                logger.warning("Parameter '{}' not recognised. Cannot reweight!".format(parameter))

        if use_XML:
            # Create XML reweighting file
            with open("reweighting_input.xml", "wb") as f_rwgt:
                f_rwgt.write("<initrwgt>\n")
                f_rwgt.write("<weightgroup name='{}' combine='{}'>\n".format(weight.group, weight.combine))
                f_rwgt.write("<weight id='{}'> </weight>\n".format(weight.ID))
                f_rwgt.write("</weightgroup>\n")
                f_rwgt.write("</initrwgt>")
        else:
            # Change reweighting metadata in runcard
            FileParser("powheg.input").text_replace("lhrwgt_descr .*", "lhrwgt_descr '{}'".format(weight.name))
            FileParser("powheg.input").text_replace("lhrwgt_id .*", "lhrwgt_id '{}'".format(weight.ID))
            FileParser("powheg.input").text_replace("lhrwgt_group_combine .*", "lhrwgt_group_combine '{}'".format(weight.combine))
            FileParser("powheg.input").text_replace("lhrwgt_group_name .*", "lhrwgt_group_name '{}'".format(weight.group))

        # Run the process until termination
        if process.parameters_by_name("manyseeds")[0].value == 1:
            if process.powheg_version == "RES":
                multicore_untimed(process)
            else:
                FileParser("powheg.input").text_replace("manyseeds .*", "manyseeds 0")
                FileParser("powheg.input").text_replace("parallelstage .*", "parallelstage -1")
                singlecore_untimed(process)
        else:
            singlecore_untimed(process)

    # Run single weight variation
    __timed_inner_fn()


def rename_LHE_output(powheg_LHE_output):
    reweighted_events_file_name = powheg_LHE_output.replace(".lhe", "-rwgt.lhe")
    try:
        shutil.move(reweighted_events_file_name, powheg_LHE_output)
    except IOError:
        raise IOError("Reweighted LHE file '{filename}' could not be found. Probably POWHEG-BOX crashed during reweighting.".format(filename=reweighted_events_file_name))


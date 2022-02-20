# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser
from ..generators import multicore_untimed, singlecore_untimed
import collections
import os
import shutil
import re

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

## Tuple to contain arbitrary grouped weights
WeightTuple = collections.namedtuple("WeightTuple", ["parameter_settings", "keywords", "ID", "name", "combine", "group", "parallel_xml_compatible"])

def default_weight_exists_already(powheg_LHE_output):
    """! Returns True if the LHE file contains a weight called "nominal".

    The weight must be present before the beginning of the first <event> block,
    otherwise the function returns False.

    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.
    """
    search_string = "<weight id='0' >default</weight>"
    with open(powheg_LHE_output, 'r') as lhe_file:
        for line in lhe_file:
            if search_string in line:
                return True
            if '<event>' in line:
                # Important to break the loop here with a return, otherwise the
                # entire (often very large) file is scanned!
                return False


@timed("reweighting")
def reweighter(process, weight_groups, powheg_LHE_output):
    """! Add a series of additional weights to an LHE file.

    @param process            PowhegBox process.
    @param weight_groups      OrderedDict containing groups of weights to add.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.
    """
    logger.info("Starting to run PowhegControl PDF and QCD scale reweighter")
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
        if group_name == "nominal" and not default_weight_exists_already(powheg_LHE_output):
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

    # ... the Powheg input card
    try:
        shutil.copy("powheg.input", "powheg.input.before_reweighting")
    except IOError:
        raise IOError("Powheg input card ('powheg.input') cannot be found at the start of reweighting. In a normal setup, PowhegControl generates this input card. Its absence is probably a sign of problems --- please investigate or contact the PowhegControl experts.")

    # ... and also backup unweighted events
    try:
        shutil.copy(powheg_LHE_output, "{}.before_reweighting".format(powheg_LHE_output))
    except IOError:
        raise IOError("Nominal LHE file could not be found. Probably POWHEG-BOX crashed during event generation.")

    # Do XML-reweighting
    if process.use_XML_reweighting:
        # Add nominal weight if not already present
        if not default_weight_exists_already(powheg_LHE_output) and not any([weight.group == "nominal" for weight in weight_list]):
            weight_list = [WeightTuple(ID=0, name="nominal", group="nominal", parallel_xml_compatible=True, parameter_settings=[], keywords=None, combine=None)] + weight_list

        FileParser("powheg.input").text_replace("pdfreweight .*", "pdfreweight 0")
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
            with open("reweighting_input.xml", "w") as f_rwgt:
                f_rwgt.write("<initrwgt>\n")
                [f_rwgt.write("{}\n".format(xml_line)) for xml_line in xml_lines]
                f_rwgt.write("</initrwgt>")

            # Add reweighting lines to runcard
            # TODO: add check that the file contains these keywords, otherwise raise an error about
            #       what is probably a faulty process interface!
            FileParser("powheg.input").text_replace("rwl_file .*", "rwl_file 'reweighting_input.xml'")
            FileParser("powheg.input").text_replace("rwl_add .*", "rwl_add 1")
            FileParser("powheg.input").text_replace("clobberlhe .*", "clobberlhe 1")

            logger.info("Preparing simultaneous calculation of {} additional weights for generated events.".format(n_parallel_xml_weights))

            # Allow RES processes to do their reweighting with manyseeds enabled, or they will look for the wrong files
            if list(process.parameters_by_name("manyseeds"))[0].value == 1:
                if process.powheg_version == "RES":
                    os.system('cp reweighting_input.xml backup_of_reweighting_input.xml')
                    os.system('cp powheg.input powheg.input.for_reweighting')
                    multicore_untimed(process)
                else:
                    FileParser("powheg.input").text_replace("manyseeds .*", "manyseeds 0")
                    FileParser("powheg.input").text_replace("parallelstage .*", "parallelstage -1")
                    os.system('cp reweighting_input.xml backup_of_reweighting_input.xml')
                    os.system('cp powheg.input powheg.input.for_reweighting')
                    singlecore_untimed(process)
            else:
                os.system('cp reweighting_input.xml backup_of_reweighting_input.xml')
                os.system('cp powheg.input powheg.input.for_reweighting')
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

    if process.use_XML_reweighting:
        comment_patterns = ["#pdf", "#rwgt", "#new weight", "#matching", " #Random" ]       
        if process.remove_oldStyle_rwt_comments:
                       
            logger.info("Removing comment lines from lhe file - these lines can be added back using the 'remove_oldStyle_rwt_comments=False' argument in generate()")
            # Remove comment lines, which may cause a crash in Pythia
            for pattern in comment_patterns:
                removed = FileParser(powheg_LHE_output).text_remove("^"+pattern)
                logger.info("{} line(s) starting with '{}' were removed from {}".format(removed, pattern, powheg_LHE_output))
        else:
            logger.info("Fixing comment lines from lhe file - these lines can be simply removed using the 'remove_oldStyle_rwt_comments=True' argument in generate()")
            for pattern in comment_patterns: # no whitespace needed in patterns here
                repair_comment_lines(powheg_LHE_output, pattern) # the last pattern starts with a space

    replacelist = []    
    # Rename all weights
    for weight in weight_list:
        replacelist += [[".* id='{}' .*".format(weight.ID), "<weight id='{weight_id}'>{weight_name}</weight>".format(weight_id=weight.ID, weight_name=weight.name), 1]]

    # Correct LHE version identification; otherwise Pythia will treat all files as v1
    replacelist += [['LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"', 1]]
    FileParser(powheg_LHE_output).text_replace_multi(replacelist)

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
            FileParser("powheg.input").text_replace("pdfreweight .*", "pdfreweight 0")
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
            with open("reweighting_input.xml", "w") as f_rwgt:
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
        if list(process.parameters_by_name("manyseeds"))[0].value == 1:
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


def repair_comment_lines(lheFile, pattern):
    if pattern is None:
        return
    if not os.path.isfile("{}.before_reweighting".format(lheFile)):
        logger.error("Impossible to find file {}.before_reweighting".format(lheFile))
        raise IOError

    # create backup file
    shutil.move(lheFile, "{}.text_replace_backup".format(lheFile))

    n_found = 0
    n_events = 0
    with open("{}.text_replace_backup".format(lheFile), "rb") as f_input:
        line_in = f_input.readline()
        while line_in:
            if re.search("^"+pattern.lstrip(), line_in.decode().lstrip()):
                n_found += 1
            elif re.search("^</event>", line_in.decode().lstrip()):
                n_events += 1
            line_in = f_input.readline()

    n_found_noWeights = 0
    n_events_noWeights = 0
    with open("{}.before_reweighting".format(lheFile), "rb") as f_input:
        line_in = f_input.readline()
        while line_in:
            if re.search("^"+pattern.lstrip(), line_in.decode().lstrip()):
                n_found_noWeights += 1
            elif re.search("^</event>", line_in.decode().lstrip()):
                n_events_noWeights += 1
            line_in = f_input.readline()

    # in case anything turns bad, will give up fixing
    impossible_to_fix = False

    # initialise counters to 0
    n_replaced = 0
    n_added_back = 0
    with open(lheFile, "w") as f_output:
        # first strategy: loop over the file with weights, and replace the relevant lines from the files without weights
        if n_found == n_found_noWeights:
            # loop in parallel on the lhe file with weights that we want to fix, and on the lhe file without weights from which we'll take the correct comment lines
            with open("{}.text_replace_backup".format(lheFile), "rb") as f_input, open("{}.before_reweighting".format(lheFile), "rb") as f_input_noWeights:
                line_in = f_input.readline()
                line_in_noWeights = f_input_noWeights.readline()
                while line_in: # loop on the lines of the output file with weights
                    if re.search("^"+pattern.lstrip(), line_in.decode().lstrip()): # found pattern, this line may need to be replaced (ignoring leading whitespaces)
                        processed = False
                        while line_in_noWeights: # loop on the next lines in the other files
                            if re.search("^"+pattern, line_in_noWeights.decode().lstrip()): # found pattern, using this line as replacement (ignoring leading whitespaces)
                                if (line_in.decode().rstrip().lstrip() != line_in_noWeights.decode().rstrip().lstrip()):
                                    f_output.write(line_in_noWeights.decode())
                                    n_replaced += 1
                                else:
                                    f_output.write(line_in.decode())
                                line_in_noWeights = f_input_noWeights.readline()
                                processed = True
                                break # end loop over the other file for the moment
                            else: # pattern not found in the other file, go to next line
                                line_in_noWeights = f_input_noWeights.readline() # keep trying
                        if processed: # if this line has been processed, no need to do the next line
                            line_in = f_input.readline() # next line in output file
                        else : # line hasn't been processed, it means the other file doesn't have enough appropriate lines
                            impossible_to_fix = True
                            break # end loop over patterns, giving up fixing
                    if impossible_to_fix: # it's pointless to continue
                        break
                    # no pattern not found, we keep the line as-is and go to the next
                    f_output.write(line_in.decode())
                    line_in = f_input.readline()

                # this is a cross-check - both file should have the same number of comment lines
                while line_in_noWeights: # finish processing the other file - we shouldn't find any more lines with pattern
                    if re.search("^"+pattern, line_in_noWeights.decode()): # found pattern, something is wrong
                        impossible_to_fix = True
                        break
                    if impossible_to_fix: # it's pointless to continue
                        break
                    line_in_noWeights = f_input_noWeights.readline()
        # alternative strategy: loop over file without weights, and add the comments from it just before the end of event
        elif n_found == 0 and n_found_noWeights == n_events_noWeights and n_events == n_events_noWeights:
            # loop in parallel on the lhe file with weights that we want to fix, and on the lhe file without weights from which we'll take the correct comment lines
            with open("{}.text_replace_backup".format(lheFile), "rb") as f_input, open("{}.before_reweighting".format(lheFile), "rb") as f_input_noWeights:
                line_in = f_input.readline()
                line_in_noWeights = f_input_noWeights.readline()
                while line_in_noWeights:
                    if re.search("^"+pattern, line_in_noWeights.decode().lstrip()):
                        while line_in:
                            if re.search("^</event>", line_in.decode().lstrip()):
                                f_output.write(line_in_noWeights.decode())
                                f_output.write(line_in.decode())
                                n_added_back += 1
                                line_in = f_input.readline()
                                break
                            else:
                                f_output.write(line_in.decode())
                                line_in = f_input.readline()
                    line_in_noWeights = f_input_noWeights.readline()
                while line_in:
                    f_output.write(line_in.decode())
                    line_in = f_input.readline()

    # processing of files ended, now handling the outcome
    if n_found == 0 and n_found_noWeights == 0: # no line found with this pattern in the file to be fixed
        shutil.move("{}.text_replace_backup".format(lheFile), lheFile)
        logger.info("No line with pattern '{}' was found in lhe file before or after reweighting, so need to fix it".format(pattern))
    elif impossible_to_fix: # we couldn't fix it, so we get the backup copy back, and remove the problematic lines
        shutil.move("{}.text_replace_backup".format(lheFile), lheFile)
        logger.info("Impossible to fix the possibly buggy comment lines with pattern {} in {} using the corresponding lines from {}.before_reweighting".format(pattern, lheFile, lheFile))
        logger.info("Keeping those lines as they are in '{}".format(lheFile))
    else:
        os.remove("{}.text_replace_backup".format(lheFile))
        if n_replaced != 0:
            logger.info("{} line(s) starting with '{}' replaced in {} using the corresponding line(s) from {}.before_reweighting".format(n_replaced, pattern, lheFile, lheFile))
        elif n_found != 0 and n_replaced == 0:
            logger.info("No line starting with '{}' was replaced in {} ({} were found) since none seems buggy".format(pattern, lheFile, n_found))
        elif n_added_back !=0:
            logger.info("{} line(s) starting with '{}' added back in {} using the corresponding line(s) from {}.before_reweighting".format(n_added_back, pattern, lheFile, lheFile))

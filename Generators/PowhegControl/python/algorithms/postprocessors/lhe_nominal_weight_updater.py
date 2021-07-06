# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import LHE
from xml.etree import ElementTree
import shutil

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

@timed("LHE file updating nominal weight")
def lhe_nominal_weight_updater(powheg_LHE_output):
    """Post-process LHE file to update the nominal weight.
    In some cases (namely when for_reweighting == 1), virtual corrections
    are added only in the reweighting stage. In this case, the nominal weight
    in the <event> block, i.e. in the XWGTUP field, is unfortunately not updated.
    This can be a problem later on because Pythia8 uses this weight as nominal,
    while the 0th weight in the <rwgt> block is the 'correct' nominal, i.e.
    which includes these virtual corrections.
    This post-processor will replace the value in the XWGTUP field
    by this 0th weight. In addition, the original XWGTUP value is saved as an additional
    weight in the <rwgt> block, in a weight with name 'XWGTUP_original',
    with an id equal to 9001 and in a group or weights
    called 'supplemental'.

    #@param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author Timothee Theveneaux-Pelzer <tpelzer@cern.ch>
    #"""

    logger.info("Updating XWGTUP value in lhe file with weight of index 0")

    wgtid_for_old_XWGTUP_value = 9001 # if not set to None, the old value of XWGTUP will be saved
    weightgroup_name_for_old_XWGTUP_value = "supplemental"
    weight_name_for_old_XWGTUP_value = "XWGTUP_original"

    # Get opening and closing strings
    preamble = LHE.preamble(powheg_LHE_output)
    if (wgtid_for_old_XWGTUP_value is not None and
        weightgroup_name_for_old_XWGTUP_value is not None and
        weight_name_for_old_XWGTUP_value is not None):
        # first modify the header, to add the additional weight
        header_old_str = LHE.header_block(powheg_LHE_output)
        header_new = LHE.add_weight_to_header(header_old_str, weightgroup_name_for_old_XWGTUP_value, weight_name_for_old_XWGTUP_value, wgtid_for_old_XWGTUP_value)
        header_new_str = ElementTree.tostring(header_new).replace("\"", "\'").strip() + "\n"
        # some complicated loop to restore order of original lines
        header_lines = []
        for l in header_new_str.splitlines():
            if l.find("<weightgroup") >= 0:
                ll = l.strip(">").split()
                combine = ""
                name = ""
                for word in ll:
                    if word.find("combine") >= 0:
                        combine = word.split("combine='")[1].split("'")[0]
                    elif word.find("name") >= 0:
                        name = word.split("name='")[1].split("'")[0]
                l_formated = "<weightgroup name='%s' combine='%s' >"%(name, combine)
                header_lines.append(l_formated)
            else:
                header_lines.append(l)
        header_new_str = "\n".join(header_lines)+"\n"
        # then loop on the preamble lines and write the new preamble
        preamble_new = ""
        inHeader = False
        for input_line in preamble.splitlines():
            if inHeader: # in header
                if input_line.find("</header>") >=0:
                    preamble_new += header_new_str # write the new header
                    inHeader = False # next line is out of header
                else:
                    continue # nothing to do if outside the header
            else: # not in header
                if input_line.find("<header>") >=0:
                    inHeader = True # header starts
                else:
                    preamble_new += input_line + "\n" # not in header, writing the line unmodified
        preamble = preamble_new # update preamble
    postamble = LHE.postamble(powheg_LHE_output) # postamble is unmodified

    powheg_LHE_updated = "{}.updated".format(powheg_LHE_output)
    with open(powheg_LHE_updated, "w") as f_output:
        f_output.write("{}".format(preamble))
        for input_event in LHE.event_iterator(powheg_LHE_output):
            output_event = LHE.update_XWGTUP_with_reweighted_nominal(input_event, wgtid_for_old_XWGTUP_value)
            f_output.write(output_event)
        f_output.write(postamble)

    # Make a backup of the original events
    shutil.move(powheg_LHE_output, "{}.lhe_nominal_weight_updater_backup".format(powheg_LHE_output))
    shutil.move(powheg_LHE_updated, powheg_LHE_output)

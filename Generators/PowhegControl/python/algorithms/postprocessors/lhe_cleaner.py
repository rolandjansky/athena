# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")

@timed("LHE file cleaning")
def lhe_cleaner(powheg_LHE_output):
    """Post-process LHE file to conform to what the parton-shower generator expects.

    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.
    """
    ## Remove rwgt and pdf lines, which crash Pythia
    #FileParser(powheg_LHE_output).text_remove("^#pdf")
    #FileParser(powheg_LHE_output).text_remove("^#rwgt")
    #FileParser(powheg_LHE_output).text_remove("^#new weight")
    ## Strip NaN values from rwgt and pdf lines, which crash Pythia (even versions which accept the comment lines)
    #FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#pdf")
    #FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#rwgt")
    #FileParser(powheg_LHE_output).text_replace("NaN", "0", regex_line_match="^#new weight")

    # Removing rwgt, pdf, and new weight comment lines if one such line looks buggy, i.e. if one NaN values is found
    # Otherwise it would probably make Pythia crash (even versions which accept such comment lines)
    pdf_removed = FileParser(powheg_LHE_output).text_remove_all_if_one_line_matches("^#pdf", "NaN")
    rwg_removed = FileParser(powheg_LHE_output).text_remove_all_if_one_line_matches("^#rwgt", "NaN")
    new_removed = FileParser(powheg_LHE_output).text_remove_all_if_one_line_matches("^#new weight", "NaN")
    rdm_removed = FileParser(powheg_LHE_output).text_remove_all_if_one_line_matches("^ #Random", "NaN") # this one starts with a space

    if pdf_removed != 0:
        logger.info("{} line(s) starting with '#pdf' were removed in {} since at least one looked buggy".format(pdf_removed, powheg_LHE_output))
    if rwg_removed != 0:
        logger.info("{} line(s) starting with '#rwgt' were removed in {} since at least one looked buggy".format(rwg_removed, powheg_LHE_output))
    if new_removed != 0:
        logger.info("{} line(s) starting with '#new weight' were removed in {} since at least one looked buggy".format(new_removed, powheg_LHE_output))
    if rdm_removed != 0:
        logger.info("{} line(s) starting with ' #Random' were removed in {} since at least one looked buggy".format(new_removed, powheg_LHE_output)) # this one starts with a space

    # Correct LHE version identification; otherwise Pythia will treat all files as v1
    FileParser(powheg_LHE_output).text_replace('LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"')

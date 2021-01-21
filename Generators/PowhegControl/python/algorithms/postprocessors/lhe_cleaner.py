# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
    logger.info("Starting to run PowhegControl LHE file cleaner")
    import os
    os.system('cp {0} unmodified_{0}'.format(powheg_LHE_output))
    # Remove rwgt and pdf lines, which crash Pythia
    FileParser(powheg_LHE_output).text_remove("^#pdf")
    FileParser(powheg_LHE_output).text_remove("^#rwgt")
    FileParser(powheg_LHE_output).text_remove("^#new weight")
    # Correct LHE version identification; otherwise Pythia will treat all files as v1
    FileParser(powheg_LHE_output).text_replace('LesHouchesEvents version="1.0"', 'LesHouchesEvents version="3.0"')

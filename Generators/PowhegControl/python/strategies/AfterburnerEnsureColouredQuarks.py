# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl AfterburnerEnsureColouredQuarks
#  Ensure that all final-state quarks have a defined colour
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AthenaCommon.Logging import logging
from ..utility import LHEUtils
import shutil

# Initialise logging handler
logger = logging.getLogger("PowhegControl")

# Post-process existing events to ensure that quarks are coloured
def afterburner_ensure_coloured_quarks(LHE_file_name):
    logger.info("Checking for presence of colourless quarks!")

    # Move file to backup location
    LHE_file_name_backup = "{}.colourless_quarks_backup".format(LHE_file_name)
    shutil.move(LHE_file_name, LHE_file_name_backup)

    # Get opening and closing strings
    opening_string = LHEUtils.get_opening_string(LHE_file_name_backup)
    closing_string = LHEUtils.get_closing_string(LHE_file_name_backup)

    # Checking events for colourless final-state quarks
    n_events = 0
    logger.info("Checking events for colourless final-state quarks")
    with open(LHE_file_name, "wb") as f_output:
        f_output.write(opening_string)
        for input_event in LHEUtils.event_iterator(LHE_file_name_backup):
            is_event_changed, output_event = LHEUtils.ensure_coloured_quarks(input_event)
            f_output.write(output_event)
            if is_event_changed : n_events += 1
        f_output.write(closing_string)
    logger.info("Re-coloured final-state quarks in {0} events!".format(n_events))

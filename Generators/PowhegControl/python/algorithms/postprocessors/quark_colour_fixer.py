# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import LHE
import shutil

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("quark colour fixer")
def quark_colour_fixer(process, powheg_LHE_output):
    """! Post-process existing events to ensure that all quarks are coloured.

    @param process            PowhegBox process.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    # Check whether vdecaymode is one of the hadronic modes
    if all(p.value in [0, 10] for p in process.parameters_by_keyword("vdecaymode")):
        logger.info("Checking for presence of colourless quarks!")

        # Get opening and closing strings
        preamble = LHE.preamble(powheg_LHE_output)
        postamble = LHE.postamble(powheg_LHE_output)

        # Check events for colourless final-state quarks and colour them
        n_events = 0
        logger.info("Checking events for colourless final-state quarks")
        powheg_LHE_recoloured = "{}.recoloured".format(powheg_LHE_output)
        with open(powheg_LHE_recoloured, "w") as f_output:
            f_output.write("{}\n".format(preamble))
            for input_event in LHE.event_iterator(powheg_LHE_output):
                is_event_changed, output_event = LHE.ensure_coloured_quarks(input_event)
                f_output.write(output_event)
                n_events += [0, 1][is_event_changed]
            f_output.write(postamble)
        logger.info("Re-coloured final-state quarks in {} events!".format(n_events))

        # Make a backup of the original events
        shutil.move(powheg_LHE_output, "{}.quark_colour_fixer_backup".format(powheg_LHE_output))
        shutil.move(powheg_LHE_recoloured, powheg_LHE_output)

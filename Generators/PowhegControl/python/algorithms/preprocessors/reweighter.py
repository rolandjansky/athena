# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ...decorators import timed
from ...utility import FileParser
import shutil


@timed("reweighting setup")
def reweighter(process, *args):
    """! Prepare for reweighting by enabling the runcard option.

    @author James Robinson  <james.robinson@cern.ch>
    """
    # Alter/remove runcard options depending on which weighting scheme is being used
    if process.use_XML_reweighting:
        FileParser("powheg.input").text_remove("lhrwgt_descr")
        FileParser("powheg.input").text_remove("lhrwgt_group_combine")
        FileParser("powheg.input").text_remove("lhrwgt_group_name")
        FileParser("powheg.input").text_remove("lhrwgt_id")
        FileParser("powheg.input").text_remove("storeinfo_rwgt")
    else:
        FileParser("powheg.input").text_remove("rwl_add")
        FileParser("powheg.input").text_remove("rwl_file")
        FileParser("powheg.input").text_remove("rwl_format_rwgt")
        FileParser("powheg.input").text_remove("rwl_group_events")
        # Enable #rwgt information lines - these cause Pythia to crash if present in the final output
        FileParser("powheg.input").text_replace("storeinfo_rwgt 0", "storeinfo_rwgt 1")

    # Make a copy of the nominal input file
    shutil.copy("powheg.input", "powheg_nominal.input")


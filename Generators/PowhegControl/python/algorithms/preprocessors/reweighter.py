# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ...decorators import timed
from ...utility import FileParser
import shutil


@timed("reweighting setup")
def reweighter(*args):
    """! Prepare for reweighting by enabling the runcard option.

    @author James Robinson  <james.robinson@cern.ch>
    """
    # Make a copy of the nominal input file
    shutil.copy("powheg.input", "powheg_nominal.input")

    # Enable #rwgt information lines - these cause Pythia to crash if present in the final output
    FileParser("powheg.input").text_replace("storeinfo_rwgt 0", "storeinfo_rwgt 1")

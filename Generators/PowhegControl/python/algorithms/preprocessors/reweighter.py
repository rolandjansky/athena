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

    # # Write nominal weight xml output
    # with open("nominal_weight.xml", "wb") as f_rwgt:
    #     f_rwgt.write("<initrwgt>\n")
    #     f_rwgt.write("<weightgroup name='nominal'>\n")
    #     f_rwgt.write("<weight id='1'>  </weight>\n")
    #     f_rwgt.write("</weightgroup>\n")
    #     f_rwgt.write("</initrwgt>")
    #
    # # Enable nominal weight in XML format - Powheg reweighting will crash if this is not present
    # FileParser("powheg.input").text_replace("rwl_file .*", "rwl_file 'nominal_weight.xml'")

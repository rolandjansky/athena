# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Authors: Peter Waller <peter.waller@cern.ch> and "Peter Onyisi" <peter.onyisi@cern.ch>

Please see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DQDefects

The main way to use all of the code found in this module can be found in the
"db" module.
"""

from DQUtils import define_iov_type

@define_iov_type
def DEFECT_IOV(channel, present, recoverable, user, comment):
    pass

DEFAULT_CONNECTION_STRING = "COOLOFL_GLOBAL/CONDBR2"

PARENT_FOLDERSET = "/GLOBAL/DETSTATUS"
DEFECTS_FOLDER = "/GLOBAL/DETSTATUS/DEFECTS"
DEFECT_FOLDER_DESC = "Data Quality defects folder"

DEFECT_LOGIC_FOLDER = "/GLOBAL/DETSTATUS/DEFECTLOGIC"
DEFECT_LOGIC_FOLDER_DESC = "Data Quality virtual defects logic"
DEFECT_LOGIC_TAG_FORMAT = "DetStatusDEFECTLOGIC-%04i"

from .db import DefectsDB

# Add a NullHandler to the DQDefects logger so that we don't get complaints
# if there are no log handlers defined in client applications.

# Logging is the responsibility of the client application!
import logging
class NullHandler(logging.Handler):
    def emit(self, record):
        pass
logging.getLogger("DQDefects").addHandler(NullHandler())

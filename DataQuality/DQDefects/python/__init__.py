# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""
Authors: Peter Waller <peter.waller@cern.ch> and "Peter Onyisi" <peter.onyisi@cern.ch>

Please see https://twiki.cern.ch/twiki/bin/viewauth/Atlas/DQDefects

The main way to use all of the code found in this module can be found in the
"db" module.
"""

from .constants import (DEFECT_IOV, DEFAULT_CONNECTION_STRING,
                        PARENT_FOLDERSET,
                        DEFECTS_FOLDER, DEFECT_FOLDER_DESC,
                        DEFECT_LOGIC_FOLDER, DEFECT_LOGIC_FOLDER_DESC,
                        DEFECT_LOGIC_TAG_FORMAT)
from .db import DefectsDB
import logging

# Add a NullHandler to the DQDefects logger so that we don't get complaints
# if there are no log handlers defined in client applications.

# Logging is the responsibility of the client application!
class NullHandler(logging.Handler):
    def emit(self, record):
        pass
logging.getLogger("DQDefects").addHandler(NullHandler())

__all__ = ['DefectsDB', 'DEFECT_IOV',
           'DEFAULT_CONNECTION_STRING', 'PARENT_FOLDERSET',
           'DEFECTS_FOLDER', 'DEFECT_FOLDER_DESC',
           'DEFECT_LOGIC_FOLDER', 'DEFECT_LOGIC_FOLDER_DESC',
           'DEFECT_LOGIC_TAG_FORMAT' ]

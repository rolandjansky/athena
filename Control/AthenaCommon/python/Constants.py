# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# File: AthenaCommon/python/Constants.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

"""Useful/conventional constants."""

__all__ = [ 'ALL', 'VERBOSE', 'DEBUG', 'INFO', 'WARNING', 'ERROR', 'FATAL',
            'TRUE', 'FALSE' ]


### message levels -----------------------------------------------------------
ALL     = 0
VERBOSE = 1
DEBUG   = 2
INFO    = 3
WARNING = 4
ERROR   = 5
FATAL   = 6

### for job options legacy (TODO: get rid of these!) -------------------------
TRUE    = True
FALSE   = False

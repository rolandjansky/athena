# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @author: Sebastien Binet
## @file :  Logging.py
## @purpose: try to import Logging from AthenaCommon.
##           falls back on stdlib's one

__version__ = "$Revision$"
__author__  = "Sebastien Binet"

__all__ = ['msg', 'logging']

from PyCmt.Logging import msg, logging

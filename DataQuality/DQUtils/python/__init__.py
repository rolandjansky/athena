# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
DQUtils - the Data Quality utility package

This package is focussed at providing utility functions for quickly retrieving 
and dealing with DQ data from COOL.
"""

import logging

from .channel_mapping import channel_mapping, channel_names, convert_channel
from .db import fetch_iovs, write_iovs, Databases
from .events import process_iovs, process_iovs_mc, process_iovs_changed
from .general import send_error_email, daemonize
from .grl import grl_from_dir, load_grl_iovs_any
from .sugar import IOVSet, define_iov_type
from .logger import init_logger


# Below code ensures no warning is given if client doesn't set up logging.
try:
    from logging.handlers import NullHandler
except ImportError:
    class NullHandler(logging.Handler):
        def emit(self, record):
            pass
            
logging.getLogger("DQUtils").addHandler(NullHandler())

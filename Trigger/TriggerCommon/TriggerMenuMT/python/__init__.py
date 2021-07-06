# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging, log

class TriggerMenuMTFormatter(logging.Formatter):
   """Custom formatter for loggers within TriggerMenuMT package.

   Logger names of type 'TriggerMenuMT.M1.M2.M3' will be shortened to
   'TriggerMenuMT:M3' to avoid long logger names. Since the logging module
   automatically applies log formats of parents to children this applies to
   all loggers that are created via `logging.getLogger(__name__)` within
   the TriggerMenuMT package.
   """
   def format(self, record):
      fields = record.name.split('.')
      if len(fields)>1:
         record.name = ':'.join([fields[0], fields[-1]])
      return super().format(record)


def _setupLogger(logger):
   """Configure TriggerMenuMT logger"""
   import copy
   # Create custom handler/formatter but relying on athena defaults:
   athena_hdlr = log.handlers[0]
   hdlr = copy.copy(athena_hdlr)
   hdlr.setFormatter(TriggerMenuMTFormatter(athena_hdlr.formatter._fmt))

   logger.propagate = False    # avoid message duplication
   logger.addHandler(hdlr)


# Create TriggerMenuMT root logger:
_log = logging.getLogger(__name__)
_setupLogger(_log)
_log.debug("Configuring TriggerMenuMT logger")

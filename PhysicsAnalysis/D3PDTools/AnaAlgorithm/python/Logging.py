# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: AnaAlgorithm/python/Logging.py
# Author: TJ Khoo (khoo@cern.ch) -- borrowed from AthenaCommon/python/Logging.py

"""AnalysisBase Python-side logging."""

# Try to import AthenaCommon first -- if this fails, we'll set up
# the standalone logger

try:
    from AthenaCommon.Logging import logging as logging

except Exception:

    from AnaAlgorithm import Constants

    __all__ = [ 'log', 'logging', 'LogError' ]



    ### basic logging and messages -----------------------------------------------
    import logging


    ### configure logger module --------------------------------------------------
    logging.VERBOSE = logging.DEBUG - 1
    logging.ALL     = logging.DEBUG - 2
    logging.addLevelName( logging.VERBOSE, 'VERBOSE' )
    logging.addLevelName( logging.ALL, 'ALL' )


    ### specialized logger to allow setting with Gaudi-like message levels =====
    class AnaLogger( logging.getLoggerClass(), object ):
       __levelmap = { Constants.ALL      : logging.ALL,
                      Constants.VERBOSE  : logging.VERBOSE,
                      Constants.DEBUG    : logging.DEBUG,
                      Constants.INFO     : logging.INFO,
                      Constants.WARNING  : logging.WARNING,
                      Constants.ERROR    : logging.ERROR,
                      Constants.FATAL    : logging.FATAL }

       def setLevel( self, level ):
          try:
             level = self.__levelmap[ level ]
          except KeyError:
             pass

          super( AnaLogger, self ).setLevel( level )

       def setFormat( self, fmt = None, datefmt = None ):
          for h in log.handlers :
             if not fmt:
                fmt = h.formatter.format
             if not datefmt:
                datefmt = h.formatter.datefmt

             h.setFormatter( logging.Formatter( fmt = fmt, datefmt = datefmt ) )

       @classmethod
       def mapLoggingToMsgLevel( self, level ):
          for k,v in self.__levelmap.items():
             if v == level:
                return k
          return level

       @classmethod
       def mapMsgLevelToLogging( self, level ):
          try:
             return self.__levelmap[ level ]
          except KeyError:
             return level

       def all( self, msg, *args, **kwargs):
          if self.manager.disable >= logging.ALL:
             return
          if logging.ALL >= self.getEffectiveLevel():
             self._log(logging.ALL, msg, args, **kwargs)

       def verbose( self, msg, *args, **kwargs):
          if self.manager.disable >= logging.VERBOSE:
             return
          if logging.VERBOSE >= self.getEffectiveLevel():
             self._log(logging.VERBOSE, msg, args, **kwargs)


    # now change the Logger class and make sure to reset the 'root' logger (which
    # is created on logging import and will otherwise have the old class)
    logging.setLoggerClass( AnaLogger )
    logging.root = AnaLogger( 'root', logging.INFO )
    logging.Logger.root = logging.root
    logging.Logger.manager = logging.Manager( logging.Logger.root )


    ### configure logger ---------------------------------------------------------
    log = logging.getLogger()
    def _setupLogger( log ):
       import sys
       log.name = "Analysis"
       hdlr = logging.StreamHandler( sys.stdout )
       frmt = logging.Formatter( "Py:%(name)-14s%(levelname)8s %(message)s" )
       hdlr.setFormatter( frmt )
       for h in log.handlers:                 # basicConfig handler already installed?
          if h.formatter._fmt == logging.BASIC_FORMAT:
             log.removeHandler( h )
             break
       log.addHandler( hdlr )
    _setupLogger( log )
    del _setupLogger

    def LogError( descr ):
       raise RuntimeError( descr )               # <== Note exception!

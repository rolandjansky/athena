# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon.Logging import logging

__all__ = [ 'TransformLogger', 'logging' ]

class TransformLogger(object):
    """Add-in (base) class to provide easy logger access"""
    # Mapping between strings and logging levels
    messageLevels = { 'ALL'     : logging.ALL,
                      'VERBOSE' : logging.VERBOSE,
                      'DEBUG'   : logging.DEBUG,
                      'INFO'    : logging.INFO,
                      'WARNING' : logging.WARNING,
                      'ERROR'   : logging.ERROR,
                      'FATAL'   : logging.FATAL }

    def __init__(self,loggerName=None,parentName=None):
        if not loggerName: loggerName = self.__class__.__name__
        self.__loggerLocalName = loggerName
        self.__loggerParentName = parentName
        self.__logger = logging.getLogger( self.loggerName() )
        self.setLoggerLevel( self.messageLevels[ 'INFO' ] )


    def loggerLocalName(self):
        return self.__loggerLocalName


    def loggerParentName(self):
        return self.__loggerParentName


    def loggerName(self):
        if self.__loggerParentName:
            return self.__loggerParentName + '.' + self.__loggerLocalName
        else:
            return self.__loggerLocalName
        

    def setLoggerName(self,name):
        self.__loggerLocalName = name
        self.__loggerParentName = None
        self.__logger = logging.getLogger( self.loggerName() )
        

    def setLoggerLocalName(self,name):
        self.__loggerLocalName = name
        self.__logger = logging.getLogger( self.loggerName() )
        

    def setLoggerParentName(self,name):
        self.__loggerParentName = name
        self.__logger = logging.getLogger( self.loggerName() )


    def setLoggerLevel(self,level):
        """Accepts a number or a string (as given in TransformLogger.messageLevels)"""
        if type(level).__name__ == 'str':
            level = TransformLogger.messageLevels[level]
        self.__logger.setLevel(level)


    def loggerLevel(self):
        return self.__logger.getEffectiveLevel()


    def logger(self):
        return self.__logger


    def printFatal(self,message):
        for m in message.split(os.linesep):
            self.__logger.fatal(m)
    

    def printError(self,message):
        for m in message.split(os.linesep):
            self.__logger.error(m)


    def printWarning(self,message):
        for m in message.split(os.linesep):
            self.__logger.warning(m)


    def printInfo(self,message):
        for m in message.split(os.linesep):
            self.__logger.info(m)


    def printDebug(self,message):
        for m in message.split(os.linesep):
            self.__logger.debug(m)
    

    def printVerbose(self,message):
        for m in message.split(os.linesep):
            self.__logger.verbose(m)
    
            
    def printAll(self,message):
        for m in message.split(os.linesep):
            self.__logger.all(m)
    


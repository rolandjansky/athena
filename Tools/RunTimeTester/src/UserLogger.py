# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import logging

from LoggerSetup import *

class UserLogger:
    # def __init__(self):
    #    print 'instatiated UserLogger'
        
    def makeLogger(self, runPath, identifiedName):
        logFileDir = runPath
        
        formatter = createFormatter()
        
        errorLogName   = createLogFile(logFileDir,
                                       'postprocessing.error.log',
                                       'postprocessing.error.log\n') 

        warningLogName = createLogFile(logFileDir,
                                       'postprocessing.warning.log',
                                       'postprocessing.warning.log\n') 

        infoLogName    = createLogFile(logFileDir,
                                       'postprocessing.info.log',
                                       'postprocessing.info.log\n') 

        debugLogName   = createLogFile(logFileDir,
                                       'postprocessing.debug.log',
                                       'postprocessing.debug.log\n') 

        handlers = []
        handlers.append(createFileHandler(debugLogName,
                                          logging.DEBUG,
                                          formatter))
        handlers.append(createFileHandler(errorLogName,
                                          logging.ERROR,
                                          formatter))
        handlers.append(createFileHandler(warningLogName,
                                          logging.WARNING,
                                          formatter))
        handlers.append(createFileHandler(infoLogName,
                                          logging.INFO,
                                          formatter))

        myLogger = logging.getLogger('UserLogger.%s' % identifiedName)
        [myLogger.removeHandler(h) for h in myLogger.handlers]
        [myLogger.addHandler(h) for h in handlers] 

        myLogger.setLevel(logging.DEBUG)
        return myLogger

if __name__ == '__main__':
    ul = UserLogger()
    ul.makeLogger()

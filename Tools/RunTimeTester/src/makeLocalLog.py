# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from LoggerSetup     import createFormatter, createLogFile, createFileHandler
import logging

def makeLocalLog(logFileDir, errorlog, warninglog, infolog, debuglog, name):

    formatter = createFormatter()
    
    errorLogName   = createLogFile(logFileDir,
                                   errorlog,
                                   errorlog+'\n') 
    
    warningLogName = createLogFile(logFileDir,
                                   warninglog,
                                   warninglog+'\n') 
    
    infoLogName    = createLogFile(logFileDir,
                                   infolog,
                                   infolog+'\n')
    
    debugLogName   = createLogFile(logFileDir,
                                   debuglog,
                                   debuglog+'\n')
    

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
    
    logger = logging.getLogger(name)
    [logger.removeHandler(h) for h in logger.handlers] 
    [logger.addHandler(h) for h in handlers] 
    
    logger.setLevel(logging.DEBUG)
    return logger

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ----------------------------------------------------------------
# ----- IMPORTS --------------------------------------------------
# ----------------------------------------------------------------

import sys
import os, os.path
import logging

from Tools import now, mkdirIfNew, rmdirMkdir, changePerm
from ToVariableLoggingHandler import ToVariableLoggingHandler
from exc2string import exc2string

# ----------------------------------------------------------------

# pseudo private, not copied out on imports of this module
dirToWhichLogsOutput = ''

# ----------------------------------------------------------------

def getDirToWhichLogsOutput():
    return dirToWhichLogsOutput

# ----------------------------------------------------------------
# ----- SETUP FUNCTIONS ------------------------------------------
# ----------------------------------------------------------------

def setupLoggers(parentDir):
    'Create the loggers for the RTT.'
    
    # ------------------------------
    # Create the output dirs
    # ------------------------------    
    global dirToWhichLogsOutput
    dirToWhichLogsOutput = createLogDirectories(parentDir)
    
    # ------------------------------
    # Setup the format
    # ------------------------------
    formatter = createFormatter()

    # -----------------------------------
    # Create the full name of the logs
    # -----------------------------------
    mainLogName    = createLogFile(dirToWhichLogsOutput,'RTT.log','RTT.log\n')
    errorLogName   = createLogFile(dirToWhichLogsOutput,'RTTerror.log','RTTerror.log\n') 
    warningLogName = createLogFile(dirToWhichLogsOutput,'RTTwarning.log','RTTwarning.log\n') 
    infoLogName    = createLogFile(dirToWhichLogsOutput,'RTTinfo.log','RTTinfo.log\n')

    dirSnapsName   = createLogFile(dirToWhichLogsOutput,'dirSnapShots.log','dirSnapShots.log\n')
    # ----------------------------------
    # create the file handler loggers
    # ----------------------------------
    logFileHandler     = createFileHandler(mainLogName,logging.DEBUG,formatter)
    errorFileHandler   = createFileHandler(errorLogName,logging.ERROR,formatter)
    warningFileHandler = createFileHandler(warningLogName,logging.WARNING,formatter)
    infoFileHandler    = createFileHandler(infoLogName,logging.INFO,formatter)

    dirSnapsHandler    = createFileHandler(dirSnapsName,logging.DEBUG,formatter)
    # ----------------------------------
    # create the stream handler logger
    # ----------------------------------
    logStreamHandler = createStreamHandler(logging.INFO,formatter)

    # ----------------------------------
    # create the variable logging handler
    # ----------------------------------
    variableLoggingHandler = createVariableLoggingHandler(logging.INFO, formatter)

    # -------------------------------------------
    # add the handlers and set the logger level
    # -------------------------------------------    
    logger = addHandlers('rtt',
                         logFileHandler,
                         infoFileHandler,
                         warningFileHandler,
                         errorFileHandler,
                         logStreamHandler,
                         variableLoggingHandler)
    logger.setLevel(logging.DEBUG) # Do Not change output level here

    dirSnapsLogger = addHandlers('dirSnaps', dirSnapsHandler)
    dirSnapsLogger.setLevel(logging.DEBUG) # Do Not change output level here

    # ---------------------------------------------------------
    # Now do all the above for a separate special case logger
    # ---------------------------------------------------------

    smallLogName     = createLogFile(dirToWhichLogsOutput,'RTTstandalone.log','RTTstandalone.log\n')
    smallFileHandler = createFileHandler(smallLogName,logging.DEBUG,formatter)

    smallLogger = addHandlers('rtt.standalone',smallFileHandler)
    smallLogger.setLevel(logging.DEBUG) # Do Not change output level here

    return dirToWhichLogsOutput
# ----------------------------------------------------------------

def setupLoggerForDebug(level=None):
    formatter = createFormatter()

    level = loggerLevelFactory(level)

    logStreamHandler = createStreamHandler(level,formatter)
    logger = addHandlers(None,logStreamHandler)
    logger.setLevel(logging.DEBUG) # Do Not change output level here

# ----------------------------------------------------------------

def setupLoggerForTestSuite(logDir, level=None):
    formatter = createFormatter()    

    # convert to logging variable. Sets default if necessary
    level = loggerLevelFactory(level) 

    fullPathToLog = createLogFile(logDir,
                                  'testSuite.log',
                                  'testSuite.log\n')
    fileHandler = createFileHandler(fullPathToLog,level,formatter)
        
    #    logger = addHandlers(None,fileHandler)
    logger = addHandlers('rtt',fileHandler)
    logger.setLevel(logging.DEBUG) # Do Not change output level here

    print logger.handlers
    return fileHandler # so we can remove it

# ----------------------------------------------------------------
def resetLoggerForTestSuite(logger, handler, directory):
    
    level     = handler.level
    formatter = handler.formatter
    logger.removeHandler(handler)
    fullPathToLog = createLogFile(directory,
                                  'testSuite.log',
                                  'testSuite.log\n')

    fileHandler = createFileHandler(fullPathToLog, level, formatter)
        
    #    logger = addHandlers(None,fileHandler)
    logger.addHandler(fileHandler)
    logger.setLevel(logging.DEBUG) # Do Not change output level here

    print logger.handlers
    return fileHandler # so we can reset it later
# ----------------------------------------------------------------
# ----- HELPER FUNCTIONS -----------------------------------------
# ----------------------------------------------------------------

def createLogDirectories(parentDir):
    if not os.path.exists(parentDir) or not os.path.isabs(parentDir):
        print 'Unable to setup the logger because the path: ' + str(parentDir)
        print 'either does not exist, or is not absolute.'
        sys.exit()
            
    try:
        mkdirIfNew(os.path.join(parentDir,'logs'))
    except Exception, e:        
        print 'Unable to create the path: ' + str(os.path.join(parentDir,'logs'))
        print str(e)
        print exc2string(sys.exc_info())
        sys.exit()

    logsDirName = 'logs/logfiles_'+now()
    fullLogsDirName = os.path.join(parentDir, logsDirName)

    try:
        if os.path.exists(fullLogsDirName):
            rmdirMkdir(fullLogsDirName)
        else:
            mkdirIfNew(fullLogsDirName)
    except Exception, e:
        print 'Unable to create path: ' + str(fullLogsDirName)
        print str(e)
        print exc2string(sys.exc_info())
        sys.exit()                         

    return fullLogsDirName

# ----------------------------------------------------------------

def createFileHandler(name,level,formatter):
    theHandler = logging.FileHandler(name)
    theHandler.setFormatter(formatter)
    theHandler.setLevel(level)

    return theHandler

# ----------------------------------------------------------------

def createStreamHandler(level,formatter):
    theHandler = logging.StreamHandler()
    theHandler.setFormatter(formatter)
    theHandler.setLevel(level)

    return theHandler

# ----------------------------------------------------------------

def createVariableLoggingHandler(level,formatter):
    theHandler = ToVariableLoggingHandler()
    theHandler.setFormatter(formatter)
    theHandler.setLevel(level)

    return theHandler
    
# ----------------------------------------------------------------

def createFormatter():    
    msgFmt    = '%(asctime)s %(filename)s, %(lineno)s, %(levelname)s %(message)s'
    tmeFmt    = '%d%b%y %H:%M:%S'
    formatter = logging.Formatter(msgFmt, tmeFmt)
    return formatter

# ----------------------------------------------------------------

def createLogFile(parentDir,fileName,startText='A log file',permission=0775):
    if not os.path.exists(parentDir) or not os.path.isabs(parentDir):
        print 'The parent directory: ' + str(parentDir)
        print 'in which to create the logger file: ' + str(fileName)
        print 'is either inexistant, or is not absolute. It must be both.'
        logging.shutdown()
        sys.exit()

    try:
        fullPath = os.path.join(parentDir,fileName)
        open(fullPath,'w').write(startText)
    except Exception, e:
        print 'Unable to create and write to the log file: '
        print str(fullPath)
        print str(e)
        print exc2string(sys.exc_info())
        logging.shutdown()
        sys.exit()

    try:
        changePerm(fullPath,permission)
    except Exception, e:
        print 'Unable to set permission: ' + str(permission)
        print 'on the log file: ' + str(fullPath)
        print str(e)
        print exc2string(sys.exc_info())
        logging.shutdown()
        sys.exit()
    
    return fullPath
    
# ----------------------------------------------------------------

def addHandlers(theLoggerName=None,*handlers):
    'Creates a logger instance and adds handlers to it. Returns the instance.'

    logger = logging.getLogger(theLoggerName)
    [logger.addHandler(handler) for handler in handlers]

    return logger

# ----------------------------------------------------------------

def loggerLevelFactory(level=None):    
    levels = {
        'DEBUG':     logging.DEBUG,
        'INFO':      logging.INFO,
        'WARNING':   logging.WARNING,
        'ERROR':     logging.ERROR,
        'CRITICAL':  logging.CRITICAL
        }
    if level in levels:
        level=levels[level]
    else:
        print 'Unknown level %s\n' %level
        print 'Possible levels: ', str(levels.keys())
        print 'setting level to DEBUG'
        
        level = logging.DEBUG

    return level

# ----------------------------------------------------------------

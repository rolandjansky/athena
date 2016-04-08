# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
class MinderBase. Top of the Minder Heirarchy.

- Takes care of the state machine. This is a mixture of
a status object and a dispatcher.

The state machine  has states: queued, running, success, error,
it nows how to proceed from one state to the next, and species the actions to be
performed when the state changes. The way jobs are submited, and the names of
log files are examples of items that change with mode.

This class also has the following methods, which are common to all Minders,

poll
fromQueued
fromRunning
fromSuccess
fromError
setStatus
setStatusList
forceCloseDown
setDone

Many of these are overidden in derived classes.
"""

from Factory_MinderStateEngine   import MinderStateEngineFactory
from Factory_XMLConverter        import XMLConverterFactory
from exc2string2                 import exc2string2
from RTTSException               import RTTCodingError
from Tools                       import nowSecs
import RTTFileLogger

import logging
logger = logging.getLogger('rtt')

import shelve

import os.path
#import cPickle



from MethodTimer import MethodTimer
#from Tracer      import Tracer

class MinderBase(MethodTimer):
    """
    A robust (ie simple) base class for all minders.
    Some, such as ErrorMinder and Container minders,
    inherit only form this class. Because Error minders
    are one of the notification mechanisms for problems,
    this base class should always be instantiable (no exceptions
    are to thrown)

    The "done" flag is handled in this class as this is a fundemental
    control flag for all minders. is minder.done = True, the minder
    is dicarded form active minder lists.

    The class provides:
    - a logger
    - a state engine
    - an xmlconverter
    - the done flag and simple methods for handling it
    - identified name. This is a unique string for the minder. Used, e.g. for XML reporting
    """

    def __init__(self, logDir, package, identifiedName, serialNumber, elementCreator, textNodeCreator):
        MethodTimer.__init__(self)

        self.done            = False
        self.logDir          = logDir
        self.package         = package
        self.identifiedName  = identifiedName
        self.jobSerialNumber = serialNumber
        self.elementCreator  = elementCreator
        self.textNodeCreator = textNodeCreator
        self.jobGroup        = None
        self.rttPilotJob     = False
        self.rttATNJob       = False

        self.logger          = None
        self.makeLog()
        self.logger.debug('MinderBase init')
        self.stateEngine     = None
        try:
            self.stateEngine = MinderStateEngineFactory(self.logger).create(self)
        except Exception, e:
            msg = 'Error instantiating  state engine:\n%s, Traceback:\n%s' %  (str(e), exc2string2())
            self.logger.error(msg)
            raise RTTCodingError(msg)
            
        msg =  'MinderBase instantiated a state engine of type %s' % self.stateEngine.__class__.__name__
        self.logger.info(msg)

        self.xmlConverter        = XMLConverterFactory(self.logger).create(self)
        msg =  'MinderBase instantiated an xml converter of type %s' % self.xmlConverter.__class__.__name__
        self.logger.info(msg)
        

        self.removeMe            = False
        # self.monActions          = {} # key = run position, value = instances
        self.actions             = {} # key = run position, value = instances
        self.tests               = {} # key = run position, value = instances
        self.checks              = []




    # --------------------------------------------------------------------
    def numberOfActions(self):
        tot = 0
        for k,v in self.actions.items():
            tot += len(v)
        return tot
    
    def numberOfTests(self):
        tot = 0
        for k,v in self.tests.items():
            tot += len(v)
        return tot
        
    def setSummaryCallBack(self, newCallBack):
        self.xmlConverter.setSummaryCallBack(newCallBack)
        self.logger.debug('Set summary call back%s' % str(newCallBack))
        

    def isDone(self): return self.done
    
    def setDone(self):

        self.done = True
        self.xmlConverter.makeSummaryReport()

        # self.xmlConverter.makeSummaryReport() 
        
        m = 'done set in minder %s' % self.identifiedName
        self.logger.info(m)
        return 'terminate'

    def setOperatorCloseDown(self):
        self.stateEngine.setOperatorCloseDown()
        self.poll()

    def status(self):
        return self.stateEngine.state.state

    #def dataForXMLNode(self):
    #    return self.dataForXMLNode

    def dataForMonitoring(self):
        "Added to by subclasses"

        dict =   {
            'name':              self.identifiedName,
            'nTests':            0,
            'done':              self.isDone(),
            'operatorCloseDown': self.closedDownByOperator(),
            'nTestsSuccess':     0,
            'nTestsFailure':     0,
            'nRetries':          0,
            'jobSuccess':        False,
            'jobFailure':        False,
            'ppSuccess':         False,
            'ppFailure':         False
            }
    
    
        return dict

    

# =============================================================================

    def makeLog(self, loggerNameExt=''):
        # set up a log file for this TestRun instance
        # all logs for a pacakge go in a sub dir

        logfile = os.path.join(self.logDir, self.package, self.identifiedName)

        self.logger = RTTFileLogger.getLogger(self.identifiedName, logfile)

    def __del__(self):
        self.logger.info('Destructor for Minder %s' % self.identifiedName)

    def unlink(self): pass

    def poll(self):
        """
        polls the state engine. If the state engine
        changes state, report and re- poll
        """
        m = '%s Start (Base) %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        self.logger.debug(m)
        
        transition = self.stateEngine.poll()
        
        if transition:

            m = 'State transition detected (%s, %s): %s -> %s, calling back' % (self.__class__.__name__,
                                                                                self.identifiedName,
                                                                                self.stateEngine.pollEntryState,
                                                                                self.stateEngine.pollExitState)
            self.logger.info(m)

            try:
                self.xmlConverter.makeSummaryReport()
            except:
                m='Error on calling back to TestRun:\n%s' % exc2string2()
                raise RTTCodingError(m)
                
            # self.poll()
        m = '%s End (Base) %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        self.logger.debug(m)

    def fromOperatorCloseDown(self):
        self.setDone()
        return 'done'

    def closedDownByOperator(self): return self.stateEngine.closedDownByOperator()
    
    def fromDone(self):
        self.setDone()
        assert(self.done)
        return 'done'

    def currentState(self):
        return self.stateEngine.state.state

    def __str__(self):
        s = '----------- %s ---------------\n' % self.__class__.__name__
        for k in self.__dict__.keys():
            s += ' 40%s:      %s\n' % (str(k), str(self.__dict__[k]))
            return s


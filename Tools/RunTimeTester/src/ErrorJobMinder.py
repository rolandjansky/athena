# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MinderBase       import MinderBase
from ErrorMinderToXML import ErrorMinderToXML
# =============================================================================
class ErrorJobMinder(MinderBase):
    def __init__(self, argBag, jDescriptor):

        MinderBase.__init__(self, argBag.logDir,
                            argBag.package,
                            jDescriptor.identifiedName,
                            jDescriptor.jobSerialNumber,
                            argBag.elementCreator,
                            argBag.textNodeCreator
                            )

        self.elementCreator   = argBag.elementCreator
        self.textNodeCreator  = argBag.textNodeCreator
        self.xmlConverter     = ErrorMinderToXML(self)
        self.errorMessages    = jDescriptor.errorMessages
        self.name             = jDescriptor.name
        self.jobSerialNumber  = jDescriptor.jobSerialNumber
        self.jobGroup         = 'ErrorGroup'
        self.resPath          = jDescriptor.resPath
        self.runPath          = jDescriptor.runPath
        self.neverCopyAndNeverDelete = []
        
        self.done             = False
        self.logger.info('ErrorMinder is now in charge of job %s\nError messages: %s' % (self.identifiedName,
                                                                                         self.errorMessages))
        
    def unlink(self):
        'break circular references to allow garbage collection'

        self.stateEngine.unlink()
        del self.stateEngine
        self.xmlConverter.unlink()
        del self.xmlConverter
        MinderBase.unlink(self)


        
    def setDone(self):
        self.logger.info('ErrorJobMinder: setDone: error messages: %s' % str(self.errorMessages))
        self.done = True
        m = 'done set in Error Minder %s' % self.identifiedName
        self.logger.info(m)
        self.xmlConverter.update()


    def status(self): return 'error'

    def dataForMonitoring(self):
        dict = MinderBase.dataForMonitoring(self)
        dict['jobSuccess'] = False
        dict['jobFailure'] = True
        dict['done']       = True
        return dict

    def isSuccess(self): return False
    def isDone(self):    return self.done

    # --------------------------------------------------------------------
    
    def dump(self):
        self.logger.debug('|-------------------------------------------|')
        self.logger.debug('|                                           |')
        self.logger.debug('|            ErrorJobMinder  dump           |')
        self.logger.debug('|                                           |')
        self.logger.debug('|-------------------------------------------|')
        self.logger.debug('\n'+self.__str__())
        
    def fromQueued(self):
        self.setDone()
        return 'done'
    
    def fromOperatorCloseDown(self):
        self.setDone()
        return 'done'

    def fromDone(self):
        self.setDone()
        return 'done'
        

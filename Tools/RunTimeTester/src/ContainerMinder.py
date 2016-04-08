# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MinderBase                 import MinderBase
from simpleDataForMonitoring    import simpleDataForMonitoring
from RTTSException              import RTTCodingError
from exc2string2                import exc2string2
from Tools                      import nowSecs

import os.path
# note there are more imports at the end of the module
# placed there to avoid circular import problems.

"""
Recursive minder creation: if te cointainerDescriptor contains container
descriptors, a containerMinder will be constructed tat n turn will build its
contained minders. These need to be connected to the outer minders by providing
setSummaryCallBack and poll metods.
"""
#from Tracer import Tracer
#class ContainerMinder(MinderBase, Tracer):
class ContainerMinder(MinderBase):
    def __init__(self, argBag, containerDescriptor):


        MinderBase.__init__(self, argBag.logDir,
                            argBag.package,
                            containerDescriptor.identifiedName,
                            containerDescriptor.jobSerialNumber,
                            argBag.elementCreator,
                            argBag.textNodeCreator
                            )


        self.summaryCallBack = None
        self.finishedTests   = [] # added purely while I (PS: Who is I) was testing the querier code, needs to be thought out
        try:                      # maybe there arn't any such files
            self.outChainFiles   = argBag.outChainFiles
        except:
            self.outChainFiles   = []
        self.jobGroup        = containerDescriptor.jobGroup
        self.name            = containerDescriptor.name
        self.errorMessages   = []
        self.resPath         = containerDescriptor.resPath

        self.minders = [argBag.minderFactory.create(argBag, d) for d in containerDescriptor.descriptors()]
        self.noOfMinders = len(self.minders)

    def cullMinders(self):
        doneMinders = [m for m in self.minders if m.done]
        for m in doneMinders: self.minders.remove(m)


    def dump(self): self.logger.debug(self.__str__())

    def isSuccess(self): return True
    

    def setSummaryCallBack(self, newCallBack):
        'Chain the xml converters'
        
        self.xmlConverter.setSummaryCallBack(newCallBack)
        [m.setSummaryCallBack(self.xmlConverter) for m in self.minders]
        #[m.setSummaryCallBack(newCallBack) for m in self.minders]
        self.logger.debug('Set summary call back in self.minders %s'% self.xmlConverter.__class__.__name__)

    def dataForMonitoring(self):
        return simpleDataForMonitoring(self)

    def fromQueued(self): return 'running'
    
    def fromRunning(self):
        self.fromRunningActions()
        if self.fromRunningTest():
            self.logger.debug('ContainerMinder::fromRunning setting done for %s:%s' % (self.__class__.__name__,
                                                                                       self.identifiedName))
            return 'done'
        return 'running'
    
    def fromRunningActions(self):
        self.logger.debug('ContainerMinder::fromRunningActions: no of active Minders %d' % self.noOfActiveMinders())
        self.cullMinders()
        
    def fromRunningTest(self):
        self.logger.debug('ContainerMinderStateEngine::fromRunningTest: no of active Minders %d' % self.noOfActiveMinders())
        return self.noOfActiveMinders() == 0
        
   
        
class SequentialMinder(ContainerMinder):
    def __init__(self, argBag, containerDescriptor):
        ContainerMinder.__init__(self, argBag, containerDescriptor)
        self.currentMinder = None
        self.setCurrentMinder()
        
    def setCurrentMinder(self):
        self.currentMinder = None
        if self.minders: 
            self.currentMinder = self.minders.pop(0)

    def cullMinders(self):
        if not self.currentMinder: return
            
        # current minder is set and running - leave as is
        if not self.currentMinder.isDone(): return

        # current minder is set and done. Set new current and return 
        if self.minders:
            self.currentMinder = self.minders.pop(0)
        else:
            self.currentMinder = None
        return
                           
    def poll(self):
        if self.currentMinder:
            try:
                self.currentMinder.poll()
            except Exception, e:
                msg = 'Minder: %s: Contained minder %s threw an exception during poll\nTraceBack\n%s' % (
                    self.identifiedName,
                    self.currentMinder.identifiedName,
                    exc2string2())
                
                self.logger.error(msg)
                raise RTTCodingError(msg)
                
        ContainerMinder.poll(self)
        # m = '%s End %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        
    def noOfActiveMinders(self):
        if not self.currentMinder: return len(self.minders)
        return len(self.minders)+1


    def setSummaryCallBack(self, newCallBack):
        'Chain the xml converters'
        if self.currentMinder:
            ContainerMinder.setSummaryCallBack(self, newCallBack)
            self.currentMinder.setSummaryCallBack(self.xmlConverter)
            self.logger.debug('Set summary call back in self.currentMinder %s'% self.xmlConverter.__class__.__name__)



    def __str__(self):
        s = '\n'
        s+='SequentialMinder start\n'
        
        s+= '   Minder name: %s\n' % self.identifiedName
        s+= '   %d minders in list\n' % len(self.minders)
        s+= '   Dump of Current Minder: \n %s\n' % (str(self.currentMinder))

        s+='SequentialMinder end\n'
        
        return s
    
        
class ParallelMinder(ContainerMinder):
    def __init__(self, argBag, containerDescriptor):
        ContainerMinder.__init__(self, argBag, containerDescriptor)

    def poll(self):
        # m = '%s Start %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        # print m

        try:
            [m.poll() for m in self.minders]
        except Exception, e:
            msg = 'Minder %s: a contained minder threw an exception during poll\nTraceBack"\n%s' % (self.identifiedName,
                                                                                                    exc2string2())
            self.logger.error(msg)
            raise RTTCodingError(msg)
        
        ContainerMinder.poll(self)
        # m = '%s End %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        # print m

    def noOfActiveMinders(self):
        return len(self.minders)

    def cullMinders(self):
        self.minders = [m for m in self.minders if not m.isDone()]

    def __str__(self):
        s='\n'
        s+='ParallelMinder start\n'

        
        s+= '   Minder name: %s\n' % self.identifiedName
        s+= '   Minder list:\n %s\n' % str(self.minders)

        s+='ParallelMinder end\n'

        return s
     
#class ChainJobMinder(Tracer):
from ChainFileCopier import ChainFileCopier

class ChainJobMinder(MinderBase):
    def __init__(self, argBag, chainJobDescriptor):
        MinderBase.__init__(self,
                            argBag.logDir,
                            argBag.package,
                            chainJobDescriptor.identifiedName,
                            chainJobDescriptor.jobSerialNumber,
                            argBag.elementCreator,
                            argBag.textNodeCreator
                            )
        paths = chainJobDescriptor.descriptor.paths
        # minder instantiation deferred to first poll - bacause before it is unknown of the chain
        # files are present in the store.
        self.minder          = None # minder instantiation deferred to first poll - bacause before 
        self.fileCopier      = None # instantiated when minder is instantiated
        self.innerDescriptor = chainJobDescriptor.descriptor
        self.argsForMinder   = argBag
        self.name            = chainJobDescriptor.name
        self.jobGroup        = chainJobDescriptor.jobGroup
        self.errorMessages   = []
        self.resPath         = chainJobDescriptor.resPath
        self.chainStoreArea  = chainJobDescriptor.chainStoreArea
        self.noOfMinders     = 0
        # setup links to be placed in the chain store area.
        try:                      # maybe there arn't any
            self.outChainFiles   = argBag.outChainFiles
        except:
            self.outChainFiles   = []

    def makeMinder(self):
        factory         = self.argsForMinder.minderFactory
        # route the logged messages to the files for this minder (descriptor gets the TestRun logger)
        self.innerDescriptor.logger = self.logger
        # decide which input files to use depending on what is available in the chain store
        self.innerDescriptor.deferredMinderUpdate()
        
        self.minder     = factory.create(self.argsForMinder, self.innerDescriptor)
        # grab the inner minders cleanSpace method, so it an be called by this outer Minder
        # after the out minder has finished with the work directory. Done in this manner,
        # the cleanSpace method still accesses the inner minders attributes.
        # self.cleanSpace    = self.minder.cleanSpace
        self.fileCopier = ChainFileCopier( self.outChainFiles,
                                           self.minder.runPath,
                                           self.chainStoreArea,
                                           self.logger)
        def makeFn(obj):
            def fn():
                obj.logger.info('%s dummy fn: no action is beinbg taken', obj.name)

            return fn

        # set the inner minder cleanup function to a do nothing function so it will
        # not delete files in work directory
        # self.minder.cleanSpace = makeFn(self.minder)

        # copy the list of chainFiles to the inner minder to prevent those chainFiles
        # which are also keep files to be "kept"
        self.minder.neverCopyAndNeverDelete.extend([os.path.join(self.minder.runPath, ocf) for ocf in self.outChainFiles])
        
        self.noOfMinders = 1
        self.logger.debug('%s:%s created inner minder %s:%s' % (self.__class__.__name__,
                                                                self.identifiedName,
                                                                self.minder.__class__.__name__,
                                                                self.minder.identifiedName))
    def poll(self):
        # m = '%s Start %s::poll() for %s' % (nowSecs(), self.__class__.__name__, self.identifiedName)
        # print m

        # minders instantiated on first poll
        if not self.minder:
            try:
                self.makeMinder()
            except:
                msg = '%s error making inner minder Traceback:\n%s' % (self.__class__.__name__,  exc2string2())
                raise RTTCodingError(msg)

            self.minder.setSummaryCallBack(self.xmlConverter)
                
            
        try:
            self.minder.poll()
        except Exception, e:
            msg = '%s %s: inner minder  %s threw an exception during poll\nTraceBack"\n%s' % (self.__class__.__name__,
                                                                                              self.identifiedName,
                                                                                              self.minder.identifiedName,
                                                                                              exc2string2())
            self.logger.error(msg)
            raise RTTCodingError(msg)

        MinderBase.poll(self)

    def noOfActiveMinders(self):
        if self.minder:
            if self.minder.isDone():
                return 0
            return 1
        
        return 0

    def cleanSpace(self):
        for oc in self.outChainFiles:
            try:
                os.remove(os.path.join(self.minder.runPath, oc))
            except:
                self.logger.error('Unable to delete chain file (%s) from run directory.' % oc)
                self.logger.error(exc2string2())
            else:
                self.logger.debug('Successfully deleted chain file (%s) from run directory.' % oc)
                
    def setSummaryCallBack(self, newCallBack):
        'Chain the xml converters'
        
        self.xmlConverter.setSummaryCallBack(newCallBack)
        ## self.minder.setSummaryCallBack(newCallBack)
        self.logger.debug('Set summary call back in self.xmlConverter %s'% str(newCallBack.__class__.__name__))
        #self.logger.debug('Set summary call back in self.minder %s'% str(newCallBack))
        
    def __str__(self):
        s = '\n'
        s+='ChainJobMinder  start\n'
        s+='   Contained Minder: %s:%s\n '%(self.minder.__class__.__name__,
                                            self.minder.identifiedName)
        s+='   Chain Files:\n '
        s+='       '+str(self.fileCopier)
        s+='ChainJobMinder  end\n'
        return s

    def dataForMonitoring(self):
        return simpleDataForMonitoring(self)

    def fromQueued(self): return 'running'
    
    def fromRunning(self):
        self.fromRunningActions()
        if self.fromRunningTest():
            self.logger.debug('ContainerMinder::fromRunning setting finishing for %s:%s' % (self.__class__.__name__,
                                                                                            self.identifiedName))
            return 'finishing'
        return 'running'

    def fromFinishing(self):
        self.cleanSpace()
        return 'done'
    
    def fromRunningActions(self):
        self.logger.debug('ContainerMinder::fromRunningActions: no of active Minders %d' % self.noOfActiveMinders())
        #self.cullMinders()
        if (self.minder.isSuccess() and self.minder.isDone()): self.fileCopier.copyFiles()

    def fromRunningTest(self):
        self.logger.debug('ContainerMinderStateEngine::fromRunningTest: no of active Minders %d' % self.noOfActiveMinders())
        return self.noOfActiveMinders() == 0

        
   

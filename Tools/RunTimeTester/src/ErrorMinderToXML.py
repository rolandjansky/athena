# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an aml file summary of an RTT run
"""
import sys, os, fnmatch
from StringIO      import StringIO
from makeRTTDOM    import makeRTTDOM
from xml.dom.ext   import PrettyPrint
from Defs          import RTTdefs

from MethodTimer import MethodTimer

class ErrorMinderToXML(MethodTimer):
    
    def __init__(self, minder):

        MethodTimer.__init__(self)
        
        self.minder          = minder
        self.logger          = minder.logger
        self.elementCreator  = minder.elementCreator
        self.textNodeCreator = minder.textNodeCreator
        self.callBack        = None

    def unlink(self):
        'break circular references to allow garbage collection'
        del self.minder
    def update(self):
        self.logger.debug('%s: XMLconversion starts' % self.__class__.__name__)
        minder = self.minder # for nested methods
        # nested scopes are used to pass parameters to these nested functions
        def findErrorMessages():

            element = self.elementCreator('errors')
            
            text = reduce(lambda x, y: x+y+'\n',
                          minder.errorMessages,
                          '')
            if text:
                tn = self.textNodeCreator(text)
                element.appendChild(tn)

            return element

        def findDataSets():
            return self.elementCreator('datasets')

        def findNtestSuccess():
            return self.elementCreator('sUserTests')

        def makeKeepFileNode():
            return self.elementCreator('deliveredkeepfiles')
            
        def makeTextNode((tagName, text)):
            tn = self.textNodeCreator(str(text).strip())
            el = self.elementCreator(str(tagName))
            el.appendChild(tn)                                   
            return el

        # logger.debug('Creating minder node for doc: ' + str(document))
        
        mElement = self.elementCreator('minder')
        textInfo = [
            ('identifiedName',  self.minder.identifiedName),
            ('jobName',         self.minder.name),
            ('jobID',           self.minder.jobSerialNumber),
            ('state',           self.minder.stateEngine.state.state),
            ('status',          self.minder.status()),
            ('jobGroup',        self.minder.jobGroup),
            ('resultsPath',     self.minder.resPath),
            ('stateHistory',    str(self.minder.stateEngine.state.history))
            ]

        [mElement.appendChild(makeTextNode(ti)) for ti in textInfo]
        
        mElement.appendChild(findDataSets())
        mElement.appendChild(findNtestSuccess() )
        mElement.appendChild(makeKeepFileNode() )
        mElement.appendChild(findErrorMessages() )
          
        
        def makeFlagNode((tagName, flag)):
            if flag: return self.elementCreator(str(tagName))
            return None

        flagInfo = [
            ('rttPilotJob',   self.minder.rttPilotJob),
            ('rttATNJob',     self.minder.rttATNJob)
            ]

        for f in flagInfo:
            node = makeFlagNode(f)
            if node: mElement.appendChild(node)

        if self.minder.isDone():
            mElement.appendChild(self.elementCreator('done'))
        
        m = 'Created, Error minder node: %s' % str(mElement)
        self.logger.debug(m)
        dict = self.minder.dataForMonitoring()
        if self.callBack:
            self.callBack.update(dict, mElement)
            self.logger.debug('%s: call back performed' % self.__class__.__name__)

        self.logger.debug('%s: XMLconversion ends' % self.__class__.__name__)
        self.minderXMLNode =  mElement

    def makeSummaryReport(self):
        if not self.callBack:
            self.logger.error('No object for Minder to call back to, abandoning update')
            return 
        self.update()
        assert (self.minderXMLNode != None)
        dict = self.minder.dataForMonitoring()
        self.callBack.update(dict, self.minderXMLNode)

    def __str__(self):
        temp = StringIO() 
        PrettyPrint(self.update(), temp)
        str = temp.getvalue()
        temp.close()
        return str


    def setSummaryCallBack(self, newCallBack):
        self.logger.debug('setting a callback of type %s' % (
            newCallBack.__class__.__name__))
        self.callBack = newCallBack

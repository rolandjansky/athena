# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an xml file summary of an RTT run
"""
import sys, os, fnmatch, os.path
from StringIO      import StringIO
from makeRTTDOM    import makeRTTDOM
# from xml.dom.ext   import PrettyPrint
from Defs          import RTTdefs
from exc2string2   import exc2string2
from MethodTimer   import MethodTimer
from sortValues    import sortValues

class ContainerXMLConverter(MethodTimer):
    
    def __init__(self, minder):

        MethodTimer.__init__(self)
        
        self.minder          = minder
        self.logger          = minder.logger
        self.elementCreator  = minder.elementCreator
        self.textNodeCreator = minder.textNodeCreator
        self.callBack        = None
        self.minderXMLNode   = None
        self.containedMinderNodeDict = {}
        
    def unlink(self):
        del self.minder
        if self.callBack: del self.callBack
        
    def update(self, dict, node):

        # if node:
        #    print '---------ContainerXMLNode converter for %s received node:' % self.minder.identifiedName
        #    temp = StringIO() 
        #    PrettyPrint(node, temp)
        #    print temp.getvalue()
        # else:
        #    print '---------ContainerXMLNode converter for  received None as node'
        
        if node:
            oldNode = self.containedMinderNodeDict.pop(dict['name'], None)
            # if oldNode: oldNode.unlink() # not sure if this will work - is this node in the deocument?

            self.containedMinderNodeDict[dict['name']] = node

    def makeSummaryReport(self):
        if not self.callBack:
            self.logger.error('No object for Minder to call back to, abandoning update')
            return
                                      
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

        def makeTextNode((tagName, text)):
            tn = self.textNodeCreator(str(text).strip())
            el = self.elementCreator(str(tagName))
            el.appendChild(tn)                                   
            return el

        # logger.debug('Creating minder node for doc: ' + str(document))
        
        mElement = self.elementCreator('%s' % self.minder.__class__.__name__)
        textInfo = {
            'identifiedName':    self.minder.identifiedName,
            'jobID':             self.minder.jobSerialNumber,
            'state':             self.minder.stateEngine.state.state,
            'status':            self.minder.status(),
            'jobGroup':          self.minder.jobGroup,
            'stateHistory':      str(self.minder.stateEngine.state.history),
            'nContainedMinders': str(self.minder.noOfMinders),
            'nActiveMinders':    str(self.minder.noOfActiveMinders())
            }


        
        for key,val in textInfo.items():
            if val.__class__.__name__ in ['list', 'tuple']: # 1-to-many tag/value mapping
                [mElement.appendChild(makeTextNode((key,v))) for v in val]
            else: #1-to-1 tag/value mapping 
                mElement.appendChild(makeTextNode((key,val)))

        # keep fixed ordering of nodes. Needed by RTT self tests
        sortedNodes = sortValues(self.containedMinderNodeDict)
        #[mElement.appendChild(n) for n in self.containedMinderNodeDict.values()]
        [mElement.appendChild(n) for n in sortedNodes]
        self.outputToFile(mElement)

        m = 'Created, minder node for %s' % self.minder.identifiedName
        dict = self.minder.dataForMonitoring()
        self.callBack.update(dict, mElement)
        self.logger.debug(m)
        

    def outputToFile(self, minderNode):
        space = ' '
        f = os.path.join(self.minder.resPath, 'rttjobinfo.xml')
        h = None
        try:
            h = open(f, 'w')
            h.write(minderNode.toprettyxml(3*space, '\n'))
            h.close()
        except:
            self.logger.error('Unable to output minder info XML file. Traceback: \n%s' % exc2string2())
        if h: h.close()    

    def setSummaryCallBack(self, newCallBack):
        self.logger.debug('received a callback of type %s' % (
            newCallBack.__class__.__name__))
        self.callBack = newCallBack

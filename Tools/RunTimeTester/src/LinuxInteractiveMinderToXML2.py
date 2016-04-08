# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to provide an xml file summary of an RTT run
"""
from StringIO      import StringIO
from makeRTTDOM    import makeRTTDOM
from xml.dom.ext   import PrettyPrint
from Defs          import RTTdefs
from exc2string2   import exc2string2
from MethodTimer   import MethodTimer

import sys
import os
import fnmatch

class LinuxInteractiveMinderToXML(MethodTimer):
    
    def __init__(self, minder):

        MethodTimer.__init__(self)

        
        self.minder          = minder
        self.logger          = minder.logger
        self.elementCreator  = minder.elementCreator
        self.textNodeCreator = minder.textNodeCreator
        self.callBack        = None
        self.minderXMLNode   = None
        
    def unlink(self):
        del self.minder
        if self.callBack: del self.callBack
        
    def update(self):

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

        def findDatasets():

            dsElement = self.elementCreator('datasets')
            
            try:
                dataSets = minder.datasets
            except:
                dataSets = {}

            datasetInstances = []
            [datasetInstances.extend(dataSets[k]) for k in dataSets]
            #for ds in datasetInstances:
            #    tn = self.textNodeCreator(os.path.basename(ds.physical.strip()))
            #    el = self.elementCreator('dataset')
            #    el.appendChild(tn)
            #    dsElement.appendChild(el)

            for ds in datasetInstances:
                dsInfoEl = self.elementCreator('datasetinfo')

                head, tail = os.path.split(ds.physical)
                
                tn = self.textNodeCreator(tail.strip())
                el = self.elementCreator('file')
                el.appendChild(tn)
                dsInfoEl.appendChild(el)

                tn = self.textNodeCreator(head.strip())
                el = self.elementCreator('directory')
                el.appendChild(tn)
                dsInfoEl.appendChild(el)

                tn = self.textNodeCreator(str(ds.mtime).strip())
                el = self.elementCreator('mtime')
                el.appendChild(tn)
                dsInfoEl.appendChild(el)

                tn = self.textNodeCreator(str(ds.size).strip())
                el = self.elementCreator('size')
                el.appendChild(tn)
                dsInfoEl.appendChild(el)

                tn = self.textNodeCreator(str(ds.origin).strip())
                el = self.elementCreator('origin')
                el.appendChild(tn)
                dsInfoEl.appendChild(el)
                                
                dsElement.appendChild(dsInfoEl)

            return dsElement

        def findMissingDatasets():

            dsElement = self.elementCreator('missingDatasets')
            
            try:
                datasetInstances = minder.missingDatasets
            except:
                datasetInstances = []

            for ds in datasetInstances:
                tn = self.textNodeCreator(os.path.basename(ds.physical.strip()))
                el = self.elementCreator('missingDataset')
                el.appendChild(tn)
                dsElement.appendChild(el)

            return dsElement

        def findNtestSuccess():
            l = len([s for s in minder.finishedTests if s.result == 0])
            tn = self.textNodeCreator(str(l))
            el = self.elementCreator('sUserTests')
            el.appendChild(tn)
            return el

        def findKeepFiles():

            resPath = minder.resPath
            
            # minder.keepFilesPatterns is a list of dictionaries.
            # kFilePatterns = [k['keepFileString']
            #                 for k in minder.keepFilePatterns]
            # minder.keepFiles is a dictionary of dictionaries.

            results = []

            for k,v in minder.keepFiles.items():
                default = ''
                name  = v.get('keepFileString', default)
                info  = v.get('infoString', default)
                color = v.get('displayColor', default)
                md5   = v.get('md5sum', default)

                lookIn  = os.path.dirname(name)
                lookFor = os.path.basename(name)
                if not os.path.exists(lookIn): continue

                matches = fnmatch.filter(os.listdir(lookIn), lookFor)

                result = [{'keepFileName' : os.path.join(lookIn, m), 'infoString' : info, 'displayColor' : color, 'md5sum' : md5} for m in matches]
                results.extend(result)

            return results
                
        def makeKeepFileNode():
            # logger.debug('Going to make element deliveredkeepfiles, document: ' +str( document))
            el = self.elementCreator('deliveredkeepfiles')

            knownKeepFiles = findKeepFiles()
            if knownKeepFiles:
                baseEl = self.elementCreator('baseKeepFileDir')
                dirRelToResBase = minder.resPath.split(minder.resultsBasePath)[1]
                if dirRelToResBase.startswith('/'): dirRelToResBase = dirRelToResBase[1:]
                if not dirRelToResBase.endswith('/'): dirRelToResBase + '/'
                baseEl.appendChild(self.textNodeCreator(dirRelToResBase))
                el.appendChild(baseEl)
                
            for k in knownKeepFiles:
                name  = k['keepFileName']
                info  = k['infoString']
                color = k['displayColor']
                md5   = k['md5sum']
                self.logger.debug('Keepfile dict: %s' % str(k))

                def keepFileNode():
                    # def keepFileNode(style):
                    # if style=='new_style':
                    el2 = self.elementCreator('keepfile_newstyle')
                    resBase = minder.resultsBasePath                    
                    keepFileRelativeToResBase = name.strip().split(resBase)[1]
                    if keepFileRelativeToResBase.startswith('/'): keepFileRelativeToResBase = keepFileRelativeToResBase[1:]
                    tn = self.textNodeCreator(keepFileRelativeToResBase)
                    el2.setAttribute('info', info)
                    el2.setAttribute('displayColor', color)
                    el2.setAttribute('md5sum', md5)
                    el2.appendChild(tn)

                    # if style=='old_style':
                    #   el2 = self.elementCreator('keepfile')
                    # tn = self.textNodeCreator(os.path.basename(str(name).strip()))
                    #    el2.setAttribute('info',info)
                    #   el2.setAttribute('displayColor',color)
                    #    el2.appendChild(tn)
                    return el2

                el.appendChild(keepFileNode())
                # el.appendChild(keepFileNode('old_style'))

            return el
            
        def makeTextNode((tagName, text)):
            tn = self.textNodeCreator(str(text).strip())
            el = self.elementCreator(str(tagName))
            el.appendChild(tn)                                   
            return el

        # logger.debug('Creating minder node for doc: ' + str(document))
        
        mElement = self.elementCreator('minder')
        textInfo = {
            'identifiedName':  self.minder.identifiedName,
            'jobName':         self.minder.name,
            'jobDisplayName':  str(self.minder.jobDisplayName),
            'jobID':           self.minder.jobSerialNumber,
            'state':           self.minder.stateEngine.state.state,
            'status':          self.minder.status(),
            'jobGroup':        self.minder.jobGroup,
            'nUserTests':      self.minder.numberOfTests(),
            'nUserActions':    self.minder.numberOfActions(),
            'resultsPath':     self.minder.resPath,
            'exitStatus':      self.minder.exitStatus,
            'cpuTime':         self.minder.cpuTime,
            'cpuTime2000':     self.minder.cpuTime2000,
            'batchStatus':     self.minder.batchStatus,
            'mem':             self.minder.mem,
            'vmem':            self.minder.vmem,
            'wallTime':        self.minder.wallTime,
            'nRetries':        self.minder.errorStateCurRetry,
            # 'nUpdates':        self.minder.nUpdates,
            # 'sumTimeToUpdate': self.minder.sumTimeToUpdate,
            'stateHistory':         str(self.minder.stateEngine.state.history),
            'postProcessingResult': self.minder.postProcessingResult,
            'processingResult':     self.minder.processingResult,
            'chainLength':          str(self.minder.chainLength),
            'chainSuccesses':       str(self.minder.chainSuccesses),
            'jobDocString':    str(self.minder.jobDocString),
            'jobDocURL':    str(self.minder.jobDocURL)
            }

            
        textInfo.update(self.minder.descDataForXMLNode)

        
        for key,val in textInfo.items():
            if val.__class__.__name__ in ['list', 'tuple']: # 1-to-many tag/value mapping
                [mElement.appendChild(makeTextNode((key,v))) for v in val]
            else: #1-to-1 tag/value mapping 
                mElement.appendChild(makeTextNode((key,val)))
            
        #[mElement.appendChild(makeTextNode((key,val))) for key,val in textInfo.items()]
        
        mElement.appendChild(findDatasets())
        mElement.appendChild(findMissingDatasets())
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

        if self.minder.closedDownByOperator():
            mElement.appendChild(self.elementCreator('operatorCloseDown'))


        el = self.elementCreator('checkResults')
        [el.appendChild(makeTextNode( ('checkResult', r) ))
         for r in self.minder.checkResults]
        mElement.appendChild(el)

        el = self.elementCreator('testResults')
        for r in self.minder.finishedTests:

            inner = self.elementCreator('testResult')
            n = makeTextNode( ('testName', r.testId) )
            inner.appendChild(n)
            if r.error:
                n = makeTextNode( ('testStatus', 'error') )
            else:
                n = makeTextNode( ('testStatus', str(r.result) ) )
            inner.appendChild(n)

            el.appendChild(inner)

        mElement.appendChild(el)
        
        self.outputToFile(mElement)

        self.minderXMLNode = mElement


        m = 'Created, minder node for %s' % self.minder.identifiedName
        self.logger.debug(m)

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
        self.logger.debug('setting a callback of type %s' % (
            newCallBack.__class__.__name__))
        self.callBack = newCallBack

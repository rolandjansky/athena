# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
A class to manage the closeDown keys for the RTT.
a closedownKey os is a dictionary that has been tunrd into an xml file.
The dictionay must contain an entry 'startTime'.

Tester appends such a key to a file closeDownKeyFile. Ongoing RTT TestRuns
periodiocaly check to see if the they should shutdown. They should do so if
there is a key that is more uptodate (ie its startTime is later than the time
associated with the tester, and all entries in the closeDownKey are equal
to a corresponding key in the TestRun key. This mismatching allows selective
shutdowns of test tuns.

Note that the closeDownKey need not have all the entries in the TestRunKey.
This, providing a closeDown key with just the time will close down all ongoing
TestRuns (no selectivity).

To orevent the closeDownKeyFile growing, when a new key needs to be written
the old keys are removed.


The entrance methods are
laterMatch()     -  to trigger shutdowns
writ((dict=None) - to write out new keys 
"""


import os, xml.dom, sys, copy
from StringIO          import StringIO
from makeRTTDOM        import makeRTTDOM
from exc2string2       import exc2string2
from Evaluate          import Evaluate, getText
from xml.dom.minidom   import parseString
from xml.dom.ext       import PrettyPrint
# -------------------------------------------------------------------------

def mismatch(dict, refdict):    
    keys = [ k for k in dict.keys() if k in refdict.keys()]

    if 'startTime' in keys: keys.remove('startTime')
    if not keys: return False # no mismatch if no more keys
    # look for a mismatch (allows no entries to shutdown all testers)
    for k in keys:
        if dict[k] != refdict[k]:
            return True
    return False
        
# ------------------------------------------------------------------------


# obsolete code kept while we still use the (almost) obsolete
# module Minder.py
def dictToDocument(dict, document):
    keyNode = document.createElement('key')        


    def addTextNode(parentNode, item):
        try:
            tn = document.createTextNode(str(item[1]).strip("'").strip())
        except xml.dom.InvalidCharacterErr:
            tn = document.createTextNode('invalid text')
            
        n  = document.createElement(str(item[0]).strip("'").strip())
        
        n.appendChild(tn)
        parentNode.appendChild(n)


    [addTextNode(keyNode, item) for item in dict.items()]
    document.documentElement.appendChild(keyNode)

    return document


# ------------------------------------------------------------------------

class RTTCloseDownKeyMaker:
    def __init__(self, argBag):

        self.logger        = argBag.pop('logger')
        self.closeDownFile = argBag.pop('closeDownFile')
        self.isNightly     = argBag.pop('isNightly')
        self.testerStart   = argBag.pop('testerStart')
        
        self.__dict__.update(argBag.__dict__)
        self.document         = None
        self.docElement       = None
    # ------------------------------------------------------------------------

    def laterMatch(self, dict):

        if 'startTime' not in dict.keys(): return False

        # select keys that occur chronologically after the current key
        dictsFromFile = self.read()
        dictsFromFile = [d for (d,e) in  dictsFromFile]
        dictsFromFile = [d for d in dictsFromFile if 'startTime' in d.keys()]
        dictsFromFile = [d for d in dictsFromFile
                         if float(d['startTime'])>float(dict['startTime'])]

        if not dictsFromFile: return False

        # there are some later keys.
        # check to see the numbers of keys do not match
        toRemove = [d for d in dictsFromFile if mismatch(d, dict)]

        # print 'dictsFromFile %d toRemove %d' % (len(dictsFromFile),
        #                                      len(toRemove))
        # if no keys match, there was no later match
        if len(dictsFromFile) > len(toRemove): return True
        return False

    # ------------------------------------------------------------------------

    def read(self):
        """
        returns the information in the close down keys file in the form
        of a  dictionary and as a DOM node
        """
        
        self.document = self.fileToDocument()
        self.docElement = self.document.documentElement

        self.docElement = self.document.documentElement

        keyFromFileTags = self.document.getElementsByTagName('key')
        
        dicts = []
        for el in keyFromFileTags:
            dict = {}
            childTags = el.childNodes
            childTags = [e for e in childTags if e.nodeType == e.ELEMENT_NODE]
            def fillDict(e, dict):
                dict[e.nodeName] = getText(e)
            [fillDict(e, dict) for e in childTags]

            if 'startTime' in dict.keys():
                dict['startTime'] = float(dict['startTime'])
            dicts.append( (dict, el) )


        return dicts

    # ------------------------------------------------------------------------

    def write(self, dictToAdd=None):                

        if dictToAdd is None:
            dictToAdd = self.makeDict()
        dictsRead = self.read()
        dicts = self.removeOldKeys(dictsRead, dictToAdd)
            
        try:
            newKeys = [self.dictToNode(d) for (d,n) in dicts]
        except:
            self.logger.error('Error adding info to the RTTClosedown key')
            self.logger.error(exc2string2())
            return

        oldKeys = self.document.getElementsByTagName('key')
        [self.document.documentElement.removeChild(n) for n in oldKeys]
        [self.document.documentElement.appendChild(n) for n in newKeys]
        [n.unlink() for n in oldKeys]
        try:
            self.xmlToFile()
        except:
            
            m = 'Unable to write RTTCloseDownKey to file (%s)' % (
                str(self.closeDownFile))
            m += 'Traceback:\n%s' % (exc2string2())
            self.logger.error(m)

        m = 'Wrote an  RTTCloseDownKey to file (%s)' % (
            str(self.closeDownFile))
        self.logger.info(m)

        
    # ------------------------------------------------------------------------
    
    def fileToDocument(self):
        'Parses close down key file, returns a DOM document '
        
        if os.path.exists(self.closeDownFile):
            ifile = open(self.closeDownFile, 'r')
            temp = ifile.read()
            ifile.close()
        else:
            return  makeRTTDOM('RTTCloseDownKey')
            
        try:
            return parseString(temp)
        except:
            m = 'Could not parse  %s' % str(self.closeDownFile)
            m += 'traceback\n%s' % exc2string2()
            self.logger.error(m)

        return ''

    # ------------------------------------------------------------------------

    def removeOldKeys(self, dictsIn, newDict):
        # prevent pile-up of old keys

        dicts = []

        # keep dicts which mismatch - they are from RTT runs
        # with different parameters
        [dicts.append( (d, node) ) for (d, node) in dictsIn
                       if mismatch(d, newDict)]

        what = 'dictionary'
        number = len(dictsIn)-len(dicts)
        if number != 1: what = 'dictionaries'
        self.logger.info('removed %d %s' % (number, what))

        dicts.append( (newDict, self.dictToNode(newDict)) )
        
        return dicts
    # ------------------------------------------------------------------------

    def makeDict(self):

        els = ['cmtConfig', 'originalBranch', 'release',
               'releaseType', 'runType', 'topProject', 'otherProject']

        dict = {}
        for el in els:
            dict[el] = self.__dict__[el]

        if self.isNightly: dict['release'] = 'nightly'

        # subtract 0.5 seconds from start time. The time is written to a
        # file and, due to rounding errors, the it may be in advance of
        # the same time passed to TestRun, which will cause a shutdown.
        startTime = self.testerStart-0.5
        dict['startTime'] = startTime

        return dict
    
    # ------------------------------------------------------------------------

    def docToPrettyString(self):
        buffer = StringIO()
        PrettyPrint(self.document, buffer)
        return buffer.getvalue()
        
    # ------------------------------------------------------------------------

    def xmlToFile(self):

        of = open(str(self.closeDownFile), 'w')
        # print '------------'
        # print 'xmlToFile ', of 
        # print 'xmlToFile ', self.docToPrettyString()
        # print '------------'
        of.write(self.docToPrettyString())
        of.close

    # ------------------------------------------------------------------------

    def dictToNode(self, dict):

        document = self.document # nested scopes
        keyNode  = document.createElement('key')        

        
        def addTextNode(parentNode, item):
            try:
                tn = document.createTextNode(str(item[1]).strip("'").strip())
            except xml.dom.InvalidCharacterErr:
                tn = document.createTextNode('invalid text')
            
            n  = document.createElement(str(item[0]).strip("'").strip())
            n.appendChild(tn)
            
            parentNode.appendChild(n)


        [addTextNode(keyNode, item) for item in dict.items()]

        return keyNode
    # ------------------------------------------------------------------------

    def __del__(self):
        if self.document: self.document.unlink()
            
if __name__ == '__main__':
    import time
    from RTTpath import RTTpath
    class JLogger:
        def debug(self, s): print 'debug',s
        def info(self, s): print 'info', s
        def warning(self, s): print 'warning',s
        def error(self, s): print 'error',s
        def crtical(self, s): print 'crtitical',s

    class JPaths:
        def __init__(self):

            self.cmtConfig      = 'x86_64-slc4-gcc34-opt'
            self.originalBranch = 'N.X.0'
            self.release        = '12.0.2'
            # self.release        = 'atlrel_4'
            self.releaseType    = 'project'
            self.runType        = 'release'
            self.isNightly      = False
            # self.isNightly      = True
            self.closeDownFile  = RTTpath('./closeDownFile.xml')

    logger = JLogger()
    a = RTTCloseDownKeyMaker(JPaths(), time.time(), logger)
    a.write()
    # a.write({'startTime': 1159868860.})

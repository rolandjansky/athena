# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import string,getopt,sys,re,glob
import os.path, shutil
import xml.dom.minidom as dom
# ---- Defined functions ----
def help_message():
      ''' help_message function '''
      print '''atnXML.py :
      
                XML processor of ATN tool (atnXML.py)

            Options: -h      --> displays this help message
                     -f file --> XML file name

            Usage:
                  atnXML.py [-h] -f file name '''
      sys.exit(0)

def getData(nodes):
    rc = ''
    for node in nodes:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

#def handleClient(client):
#    clientName = client.getElementsByTagName("clientname")[0]
#    print 'Client:', getdata(clientname.childNodes)
#    opts = client.getElementsByTagName("account"

def findTextNodes(nodeList):
    for subnode in nodeList:
        if subnode.nodeType == subnode.ELEMENT_NODE:
             if subnode.attributes: 
                print "subnode.attributes", repr (subnode.attributes.keys())   
             print "element node: " + subnode.tagName
             # call function again to get children
             findTextNodes(subnode.childNodes)
        elif subnode.nodeType == subnode.TEXT_NODE:
             print "text node: ",
             print "parent:", subnode.parentNode.tagName
             print subnode.data

class testData:
    ''' A parsed XML testData '''
    def __init__(self):
        # Record tagname and attributes dictionary
        self.testAttr = { 'name' : "unknown", 'type' : "", 'suite' : "general", 'reference' : "" }  
        self.options = [] 
        self.kvSpecs = {}
        self.rttSpecs = {}  
        self.properties = { 'timelimit' : "8", 'author' : "", 'mailto' : "" , 'prescript' : "" , 'postscript' : "" , 'package_atn' : "" , 'rundir' : "" }
        self.expectations = { 'errorMessage' : [""], 'warningMessage' : [""], 'successMessage' : [" "], 'returnValue' : [""] }  
    def addError(self, pattern):
        self.expectations['errorMessage'].append(pattern)
    def addWarning(self, pattern):
        self.expectations['warningMessage'].append(pattern)
    def addSuccess(self, pattern):
        self.expectations['successMessage'].append(pattern)
    def setReturnValue(self, value):
        self.expectations['returnValue'] = value 
    def setTestAttributes(self, dict):                                   
        self.testAttr = dict.copy()
    def setTestAttr(self, key, value):
        self.testAttr[key] = value 
    def setTestProperties(self, dict):
        self.properties[key] = dict.copy()
    def setTestProp(self, key, value):
        self.properties[key] = value
    def setTestExpectations(self, dict):
        self.expectations[key] = dict.copy()
    def addTestExpect(self, key, value):
        self.expectations[key].append(value)
    def addOption(self, pattern):
        self.options.append(pattern)
    def setKvSpecs(self, dict): 
        self.kvSpecs = dict.copy() 
    def setRttSpecs(self, dict):
        self.rttSpecs = dict.copy()
    def setTimeLimit(self, value):
        self.properties['timelimit'] = value
    def setAuthor(self, value):
        self.properties['author'] = value
    def setMail(self, value):
        self.properties['mailto'] = value
    def setPrescript(self, value):
        self.properties['prescript'] = value
    def setPostscript(self, value):
        self.properties['postscript'] = value
    def setAtnPackage(self, value):
        self.properties['package_atn'] = value
    def setRundir(self, value):
        self.properties['rundir'] = value

def fillTestData(nodeList, testDataObj):
    propList = [ 'timelimit', 'author', 'mailto' , 'prescript' , 'postscript' , 'package_atn' , 'rundir' ]
    expectList = [ 'errorMessage', 'warningMessage', 'successMessage', 'returnValue' ]
    for subnode in nodeList:
        if subnode.nodeType == subnode.ELEMENT_NODE:
##             print "DDDD", subnode.tagName
             if subnode.tagName == 'TEST':
                 if subnode.attributes:
                    atts=subnode.attributes.items()
                    for n, v in atts:
                        testDataObj.setTestAttr(n, v.encode())
             else:
                 for pname in expectList:
                     if subnode.tagName == pname:
                         child = subnode.firstChild
                         while child is not None:
                               next = child.nextSibling
                               if child.nodeType == subnode.TEXT_NODE:
##                                  print "XXXF",pname, child.data
                                  testDataObj.addTestExpect(pname, child.data.encode())
                               child=next

             fillTestData(subnode.childNodes, testDataObj)
#
        elif subnode.nodeType == subnode.TEXT_NODE:   
                 if subnode.parentNode.tagName == 'options' or subnode.parentNode.tagName == 'options_atn':
                    testDataObj.addOption(subnode.data.encode())
                    child = subnode.firstChild
                 for pname in propList:
                      if subnode.parentNode.tagName == pname:
                           testDataObj.setTestProp(pname, subnode.data.encode())
#
def findNodes(nodeList):
    for subnode in nodeList:
        if subnode.nodeType == subnode.ELEMENT_NODE:
             if subnode.attributes:
                print "subnode.attributes", repr (subnode.attributes.keys())
             print "element node: " + subnode.tagName
             # call function again to get children
             findNodes(subnode.childNodes)
        elif subnode.nodeType == subnode.TEXT_NODE:
             print "text node: ",
             print subnode.data

########################################################################
#    MAIN SECTION                                                      #
########################################################################
# ---- Global definitions ----

# ---- Process command line arguments ----

if __name__ == '__main__':

    try:
        optionslist, args = getopt.getopt(sys.argv[1:],'hf:')
    except getopt.error:
        print '''Error: You tried to use an unknown option or the
                 argument for an option that requires it was missing
                 Try "atnXML.py -h" for more information.'''
        sys.exit(0)

    for a in optionslist[:]:
        if a[0] == '-h':
            help_message()
        if a[0] == '-f' and a[1] != '':
            fileName=a[1]
        
    xmlFile = open(fileName)
#    xmlFile = open('TestRelease/TestRelease-00-00-18/test/TestHelloWorld.xml')
    xmlDoc = dom.parse(xmlFile) 
#    findTextNodes(xmlDoc.childNodes)                                      
#    findTextNodes(xmlDoc.getElementsByTagName('atn'))
    TDO = atnXML.testData()
    atnXML.fillTestData(xmlDoc.getElementsByTagName('atn'), TDO)
    

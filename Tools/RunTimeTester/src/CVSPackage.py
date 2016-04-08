# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from formatCollection import formatCollection
from RTTpath          import RTTpath
from exc2string       import exc2string
from os.path          import join, exists
import sys, xml.dom.minidom
from PackageXMLFile   import PackageXMLFile
import time


"""
 Examples:
 ---------
 package.name = JiveXML
 package.pathToXMLfile = /some/base/path/JiveXML_TestConfiguration.xml
 package.fullName = graphics/JiveXML
 
 Example of dictionary of parameters rececovered from cmt
 {'cmtPath':
   '/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasCore/rel_0',

 'name': 'AthExHelloWorld',
 'local': False,
 'errorMessage': '',
 'version': 'AthExHelloWorld-00-01-15',
  'pathToXMLfile' : '/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasCore/
      rel_0/Control/AthenaExamples/AthExHelloWorld/test/
      AthExHelloWorld_TestConfiguration.xml',
 'offset': 'Control/AthenaExamples',
 'root':
      '/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasCore/
       rel_0/Control/AthenaExamples/AthExHelloWorld',
"""

class CVSPackage:
    def __init__(self, packageName, pathToTestConfig, packageOffset, packageVersion,
                 packageCmtPath, packageRoot, packageSharePath, errorMsg, local=False):
        
        self.name              = packageName
        self.pathToTestConfig  = pathToTestConfig
        self.errorMessage      = errorMsg
        self.offset            = packageOffset
        self.version           = packageVersion
        self.cmtPath           = packageCmtPath
        self.root              = packageRoot
        self.packageSharePath  = packageSharePath
        self.local             = local

        # packages are made for all Test conf files, but not all test
        # conf files ask for the RTT.
        self.wantsRTT         = True

        # Some packages are needed even though its jobs are not run.
        self.runJobs = True

        self.packageXMLfile = PackageXMLFile(self.pathToTestConfig)
        
        if not self.packageXMLfile.exists():
            self.wantsRTT = False
            return

        # Do not set wantsRTT to False, if XML file is invalid
        if self.packageXMLfile.isValid:
            rtt = self.packageXMLfile['rtt']
            rttjobs = self.packageXMLfile['rtt/jobList']
            if not rtt or not rttjobs:
                self.wantsRTT = False
        else:
            self.errorMessage = self.packageXMLfile.errMsg
            
    # ---------------------------------------------------
            
    def getPathToXMLfile(self):
        return self.pathToTestConfig

    # ---------------------------------------------------
    
    def getFullName(self):
        return self.root

    # ---------------------------------------------------

    def getOffset(self):
        return self.offset

    # ---------------------------------------------------

    def getVersion(self):
        return self.version

    # ---------------------------------------------------
    
    def isLocal(self):
        return self.local

    # for project builds, CMTpath is the directory in which
    # the cmt directory is found
    def getPackageCmtPath(self):
        return str(RTTpath(self.cmtPath))

    def setErrorMessage(self,msg):
        self.errorMessage += msg+'\n'

    def noError(self):
        return len(self.errorMessage) == 0
        
    def setNoRun(self): self.runJobs = False

    def runMyJobs(self): return self.runJobs

    # def getRttCmtHomePath(self): return self.rttCmtHomePath
    # def getRttCmtProjPath(self): return self.rttCmtProjPath

    def __str__(self):
        return 'CVSPackage:\n'+formatCollection(self.__dict__)+'\n'

    def __del__(self):
        if self.packageXMLfile: del self.packageXMLfile

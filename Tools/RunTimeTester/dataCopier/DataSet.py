# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Dataset is a collection of information about datasets
import os.path


class DataSet:
    def __init__(self, logicalName, destDir, pkgCfg):
        self.logicalName  = logicalName
        self.physicalName = ''
        self.filesToCopy  = []
        self.isInDestDir  = True
        self.destDir      = destDir
        self.packageCfg   = pkgCfg
        self.dsSource     = ''
        self.isToCopy     = False

        self.copyStatus   = 'unknown'
        self.copyInfo     = 'not set'

        self.tobeCopied(self.logicalName, self.destDir)
        self.setPhysicalName(self.logicalName, self.destDir)
        self.setdsSource()
        self.setIsToCopy()
        
    def tobeCopied(self, name, destDir):
          bname = os.path.basename(name)
          name = os.path.join(destDir, bname)
          if not os.path.exists(name):
              self.isInDestDir = False

    def setPhysicalName(self, logicalName, destDir):
        name = os.path.basename(logicalName)
        self.physicalName = os.path.join(destDir, name)

    def setdsSource(self):
        if os.path.isabs(self.logicalName):
            pieces = self.logicalName.split('/')
            self.dsSource = pieces[1]  # eg : dsSource = castor
                                       # if /castor/
    def setIsToCopy(self):
        if not os.path.exists(self.physicalName):
            print 'file to be copied '+str(self.physicalName)
            self.isToCopy     = True
            self.filesToCopy.append(self.logicalName)
            
        
        
    
class DataSetToBeCopied(DataSet):
    def __init__(self, logicalName, destDir, pkgCfg):
        DataSet.__init__(self, logicalName, destDir, pkgCfg)

        #self.copyStatus =''
        #self.copyInfo   ='Unknown'
        self.copyStatusInit()
        
    def copyStatusInit(self):
        self.copyStatus = 'unknown'
        self.copyInfo   = 'No info yet'
    

class DQ2DataSet(DataSet):

    def __init__(self, logicalName, destDir, pkgCfg, files, site ):
        DataSet.__init__(self, logicalName, destDir, pkgCfg)
        self.physicalFiles = []
        self.files         = files
        self.filesToCopy   = []
        self.site          = site
        self.setdsSource()
        self.setPhysicalFiles()
        self.setFilesToCopy()
        self.setIsToCopy()
        #print str(self.__dict__)
        
    def setdsSource(self):
        self.dsSource = 'dq2'

    def setPhysicalFiles(self):
        for file in self.files:
            fn = os.path.join(self.destDir, self.logicalName)
            fn = os.path.join(fn, file)
            self.physicalFiles.append(fn)

    def setFilesToCopy(self):
        for file in self.physicalFiles:
            if not os.path.exists(file):
                self.filesToCopy.append(file)

    def setIsToCopy(self):
        if self.filesToCopy:
            self.isToCopy    = True
            self.isInDestDir = False
    

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Dataset is a collection of information about datasets
import os.path


class DataSet:
    def __init__(self, logicalName, destDir, pkgCfg, catalog, stageArea=''):
        self.logicalName  = logicalName
        self.physicalName = ''
        self.filesToCopy  = []
        self.isInDestDir  = True
        self.destDir      = destDir
        self.packageCfg   = pkgCfg
        self.catalog      = catalog 
        self.dsSource     = 'unknown'
        self.isToCopy     = False
        self.stageArea    = stageArea
        self.copyStatus   = 'unknown'
        self.copyInfo     = 'not set'

        self.setPhysicalName(self.logicalName, self.destDir)
        self.setdsSource()
        self.setIsToCopy()

    def setPhysicalName(self, logicalName, destDir):
        if self.logicalName in self.catalog.keys():
            self.physicalName = self.catalog[self.logicalName]
            return ''
        name = os.path.basename(logicalName)
        self.physicalName = os.path.join(destDir, name)

    def setdsSource(self):
        dict = {'/afs' : 'afs',
                '/castor': 'castor'}
        
        for key in dict.keys():
            if self.logicalName.startswith(key):
                self.dsSource = dict[key]

    def setIsToCopy(self):
        if not self.logicalName in self.catalog.keys():
            self.isToCopy    = True
            self.isInDestDir = False


class DataSetCERN(DataSet):
    def __init__(self, logicalName, destDir, pkgCfg, catalog, stageArea=''):
        DataSet.__init__(self, logicalName, destDir, pkgCfg, catalog, stageArea)
        self.setPhysicalName(logicalName)
        self.setIsToCopy()
        
    def setPhysicalName(self, logicalName):
        self.physicalName = logicalName

    def setIsToCopy(self):
        if not os.path.isabs(self.physicalName):
            print 'file to be copied '+str(self.physicalName)
            self.isToCopy     = True
            self.filesToCopy.append(self.logicalName)
    
class DataSetToBeCopied(DataSet):
    def __init__(self, logicalName, destDir, pkgCfg, catalog, stageArea=''):
        DataSet.__init__(self,
                         logicalName,
                         destDir,
                         pkgCfg,
                         catalog,
                         stageArea)

        #self.copyStatus =''
        #self.copyInfo   ='Unknown'
        self.copyStatusInit()
        
    def copyStatusInit(self):
        self.copyStatus = 'unknown'
        self.copyInfo   = 'No info yet'
    

class DQ2DataSet(DataSet):

    def __init__(self, logicalName, destDir, pkgCfg, files, site, catalog, stageArea=''):
        DataSet.__init__(self, logicalName, destDir, pkgCfg, catalog, stageArea)
        self.physicalFiles = []
        self.files         = files
        self.filesToCopy   = []
        self.site          = site
        self.setLogicalFiles()
        self.setdsSource()
        #self.setPhysicalFiles()
        self.setFilesToCopy()
        self.setIsToCopy()
        #print str(self.__dict__)
        
    def setdsSource(self):
        self.dsSource = 'dq2'

    def setPhysicalFiles(self):
        self.physicalFiles=[os.path.join(self.destDir, file)
                            for file in self.logicalFiles]

    def setFilesToCopy(self):
        copyCandidates   = [file for file in self.logicalFiles
                            if not file in self.catalog.keys()]
        copyCandidates   = [os.path.join(self.destDir, file)
                            for file in copyCandidates]
        self.filesToCopy = [file for file in copyCandidates
                            if not os.path.exists(file)]
        self.physicalFiles = self.filesToCopy                    
        
    def setIsToCopy(self):
        if len(self.filesToCopy) > 0:
            self.isToCopy    = True
            self.isInDestDir = False
            
    def setLogicalFiles(self):
        self.logicalFiles = [os.path.join(self.logicalName, file)
                             for file in self.files]

















"""
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
    
"""

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from os.path import join, isdir, exists
from Tools2 import getFileOrDirDateStamp
from Tools import emptyDirMkDir, mkdirIfNew

# Here's how it works:
#    We have a base volume location /some/base/path
#    Below which we have some volumes, say 2, called X and Y
#    For a dev run, rel_2, SLC4 opt, AFS build, AtlasProduction...
#    Procedure:
#       1. Path creation
#            For this example, the path, relative to base volume+X or Y, is:
#               nightly/dev/build/i686-slc4-gcc34-opt/offline
#            If this relative path exists:    
#            a) below neither X or Y ==> pick either X or Y, create path below it
#            b) below one of X or Y  ==> create path below the other
#            c) below both X and Y   ==> compare time stamps on the time_stamp file
#                                        that exists in relative path dir and pick
#                                        the older of the two.
# 

bigFileIO = None

def createBigFileIO(site, bigFilesArea, workDirs, isNightly):
    dict = {'CERNslc4':  CERNBigFileIO}
    big = dict.get(site, BigFileIO)
    global bigFileIO
    if not bigFileIO:
        bigFileIO = big(bigFilesArea, workDirs, isNightly)
    return bigFileIO

class BigFileIO:
    """Responsible for creating and yielding the path to the volume
    where big files will be dumped."""

    def __init__(self, bigFilesArea, workDirs, isNightly ):

        self.bigFilesArea = bigFilesArea
        self.workDirs     = workDirs
        self.isNightly    = isNightly
        
        self.setRelativeBaseDumpLocation()
        self.chooseDumpLocation()
        if hasattr(self, 'pathToDump'):
            self.createDumpLocation()
            self.deleteDumpLocationContents()
            self.createTimeStamp()

                    
    def createDumpLocation(self):
        mkdirIfNew(self.pathToDump)

    def deleteDumpLocationContents(self):
        emptyDirMkDir(self.pathToDump)

    def createTimeStamp(self):
        stampFile = join(self.pathToDump, 'time_stamp')
        if not exists(stampFile):
            os.system('touch %s' % stampFile)
        
    def cmpTimeStamps(self, a, b):
        aStamp = getFileOrDirDateStamp(a)
        bStamp = getFileOrDirDateStamp(b)
        cmp = 0
        if aStamp < bStamp: cmp = -1
        if aStamp > bStamp: cmp = 1
        return cmp
        
    def chooseDumpLocation(self):
        if not self.relLocation: return
        area = self.bigFilesArea
        tgts = [join(area, d, self.relLocation) for d in os.listdir(area) if isdir(join(area, d))]
        
        notExists = [t for t in tgts if not exists(t)]
        if notExists:
            self.pathToDump = notExists[0]
        else:
            tgts = [join(t, 'time_stamp') for t in tgts]
            notExists = [t for t in tgts if not exists(t)]
            if notExists:
                self.pathToDump = os.path.dirname(notExists[0])
            else:
                tgts.sort(self.cmpTimeStamps)
                self.pathToDump = os.path.dirname(tgts[0])
            
    def setRelativeBaseDumpLocation(self):
        if not self.bigFilesArea or not exists(self.bigFilesArea):
            self.relLocation = None
            return

        self.relLocation = self.workDirs['packages'].split(self.workDirs['base'])[1]
        if self.relLocation.startswith('/'): self.relLocation = self.relLocation[1:]

        # replace rel_X with 'nightly'
        if self.isNightly:
            self.relLocation = 'nightly/' + self.relLocation[self.relLocation.find('/')+1:]

    def getJobDumpLocation(self, minder):
        relPath = minder.runPath.split(minder.workDirs['packages'])[1]
        if relPath.startswith('/'): relPath = relPath[1:]

        try:
            descDumpLoc = join(self.pathToDump, relPath)
        except:
            descDumpLoc = None
        
        if descDumpLoc and not exists(descDumpLoc):
            mkdirIfNew(descDumpLoc)

        return descDumpLoc

class CERNBigFileIO(BigFileIO):
    def __init__(self, bigFilesArea, workDirs, isNightly):
        BigFileIO.__init__(self, bigFilesArea, workDirs, isNightly)
        
    # Do not make it for CERN as volume hangs
    # below the path, not above it
    def createDumpLocation(self):
        if not exists(self.pathToDump):
            # this will stop big file moving
            self.pathToDump = None

    def deleteDumpLocationContents(self):
        if not self.pathToDump or not exists(self.pathToDump): return
        BigFileIO.deleteDumpLocationContents(self)
        
    def createTimeStamp(self):
        if not self.pathToDump: return
        BigFileIO.createTimeStamp(self)

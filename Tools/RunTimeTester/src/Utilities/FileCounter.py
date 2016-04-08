# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from visitor import FileVisitor
import os


class FileCounter(FileVisitor):

    def __init__(self, fnFrag, grandParFrag, data=None, listonly=0):

        FileVisitor.__init__(self, data, listonly)
        
        self.nFiles       = {}
        self.fnFrag       = fnFrag
        self.grandParFrag = grandParFrag
        print 'Looking for filenames containing ', self.fnFrag
        print '... with grandparent directory containing', self.grandParFrag
        
    def visitor(self, data, dirPath, filesInDir):

        parentDir, temp = os.path.split(dirPath)
        parentDir, temp = os.path.split(parentDir)
        temp, parentDir = os.path.split(parentDir)
        # print parentDir
        if parentDir.find(self.grandParFrag) != -1:
            # print '........',parentDir
            # print '...........',dirPath

            fileCount = 0
            for file in filesInDir:
                if file.find(self.fnFrag) != -1: fileCount += 1

            # print ' has psFiles: ',fileCount
            self.nFiles[dirPath]=fileCount

def printit(fileCounter):

    items   = fileCounter.nFiles.items()

    nzero = 0
    for item in items:

        if int(item[1]) == 0:
            print item
            nzero+=1

    print
    

    nsome=0
    for item in items:

        if int(item[1]) != 0:
            print item
            nsome+=1

    print 'Looked for files containing ', fileCounter.fnFrag, 'in the filename, if these had'
    print fileCounter.grandParFrag,' in the grandparent directory'
    print 'total:         ',nsome+nzero
    print 'directories with   no such files:' ,nzero
    print 'directories with some such files:' ,nsome
    


def afCountFiles(fnFrag):
    fc = FileCounter(fnFrag, 'AthenaAtlfast')
    fc.run('.')
    printit(fc)

def rcCountFiles(fnFrag):
    fc = FileCounter(fnFrag, 'RuleChecker')
    fc.run('.')
    printit(fc)

if __name__ == '__main__':

    afCountFiles('.ps')

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from RTTSException import RTTSException
from types         import IntType

import os, shutil, logging

logger=logging.getLogger('rtt')

def isInt(a):
    try:
        i = int(a)
        return True
    except:
        return False

def stripVersion(fn, sep):
    (dir, file) = os.path.split(fn)
    file = file.split(sep)[0]
    return file
    
def ctimeCmp(a, b):
    return cmp(os.path.getctime(a), os.path.getctime(b))
        
class VersionedFileSaver:
    def __init__(self, maxVersion, destDir=None):
        if not type(maxVersion) == IntType:
            msg = 'VersionedFileSaver contructor arg not an int: '+maxVersion
            #raise RTTSException(msg)
            #raise Exception, e:
            print msg
        self.maxVersion = maxVersion;
        if destDir:
            self.destDir = destDir
        else:
            self.destDir = os.getcwd()

    def getVersionedFiles(self, fn, sep):

        vFiles = []
        if not os.path.exists(fn): return vFiles

        if not os.path.isfile(fn): return vFiles

        # remove version if any from file name
        noVerFile = stripVersion(fn, sep)

        # look for versioned files: these have the form xxx;n
        # where n is in integer
        (dir, vfile)= os.path.split(noVerFile)
        vfile += sep

        vFiles = os.listdir(self.destDir)
        vFiles = [f for f in vFiles if f.find(vfile) >=0]
        vFiles = [f for f in vFiles if f.count(sep) == 1]


        vFiles = [f for f in vFiles if isInt(f.split(sep)[1])]
        vFiles = [os.path.join(self.destDir, f) for f in vFiles]

        return vFiles

    

    def saveFile(self, fn):
        sep = '-'
        self.vFiles = self.getVersionedFiles(fn, sep)
        nextVersion = -1

        # find the last (in time) created file
        self.vFiles.sort(ctimeCmp)

        if self.vFiles:
            lastFile = os.path.basename(self.vFiles[-1])
            nextVersion = (int(lastFile.split(sep)[1])+1) % self.maxVersion
        else:
            nextVersion = 1


        noVerFile = stripVersion(fn, sep)
        toFile = noVerFile+sep+str(nextVersion)
        if self.destDir:
            toFile =  os.path.join(self.destDir, os.path.basename(toFile))


        try:
            shutil.copyfile(fn, toFile)
        except (IOError, os.error), why:
            logger.error("Cannot copy  %s to %s: %s" % (fn, toFile, str(why)))
        
if __name__ == '__main__':


    fn = './temp'
    file = open(fn, 'w')

    file.write('junk\n')
    file.close()

    try:
        vfs = VersionedFileSaver('7')
    except:
        print 'caught bad VSF construction'


    destDir = os.environ['HOME']
    # vfs = VersionedFileSaver(8,destDir)
    vfs = VersionedFileSaver(8)
    print vfs.destDir
    nvers = 1
    while nvers > 0:
        nvers -= 1
        vfs.saveFile(fn)
            
    for f in vfs.vFiles:
        print f,os.path.getctime(f)
        

        
        

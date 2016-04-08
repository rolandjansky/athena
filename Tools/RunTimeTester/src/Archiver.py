# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path
import os
import fnmatch
import shutil

from RTTSException     import RTTInputError, RTTTimeOut
from Factory_BaseClass import Factory
from Tools2            import getFileOrDirDateStamp
from Factory_IOUtils   import IOUtilsFactory
from Tools             import timed_cmd

# -----------------------------------------------------------------
# -----------------------------------------------------------------
# -----------------------------------------------------------------

class ArchivePathSelectorFactory(Factory):
    def __init__(self, logger):
        Factory.__init__(self, logger)        

    def create(self, type_, base_path, narchives):
        dict = {'afs': AFSArchivePathSelector, 'castor':CastorArchivePathSelector}
        try:
            return dict[type_](self.logger, base_path, narchives)
        except KeyError:
            m = 'Unknown archive <type> %s, must be one of %s' % (type_, dict.keys())
            raise RTTInputError(m)
        

# -----------------------------------------------------------------

class ArchivePathSelector:
    def __init__(self, logger, base_path, narchives):
        self.logger      = logger
        self.io          = self.createUtils()
        shouldExist      = [os.path.join(base_path, str(i)) for i in range(int(narchives))]
        dontExist        = [s for s in shouldExist if not self.io.exists(s)]
        doExist          = [s for s in shouldExist if s not in dontExist]
        doExistButNotDir = [s for s in shouldExist if s in doExist and not self.io.isdir(s)]
        if doExistButNotDir:
            m = ''
            for thing in doExistButNotDir:
                m  += "%s: exists but is not a directory. Cannot archive there.\n" % thing
            raise RTTInputError(m)

        self.base_path = base_path
        self.available = doExist
        self.narchives = narchives
        if self.makeDirs(dontExist): self.available.extend(dontExist)
        
    def cmpTimeStamps(self, a, b):
        aStamp = self.io.lastmodtime(a, asSecsSinceEpoch=True)
        bStamp = self.io.lastmodtime(b, asSecsSinceEpoch=True)
        cmp = 0
        if aStamp < bStamp: cmp = -1
        if aStamp > bStamp: cmp = 1
        return cmp

    def makeDirs(self, dirsToMake):
        return False not in [self.io.mkdir(d) for d in dirsToMake]

    def select_archive(self, run_frag): # run_frag = nightly/dev/build/i686-slc4-gcc34-opt/offline
        self.available.sort()
        timestamps = [os.path.join(a, run_frag, 'time_stamp') for a in self.available]
        notUsedYet = [o for o in timestamps if not self.io.exists(o)]
        if notUsedYet:
            path_ = os.path.dirname(notUsedYet[0])
            m = 'Choosing unused archive: %s' % path_
            self.logger.debug(m)
            self.makeTimeStamp(path_)
            return path_ # just take the first one
        
        # Have to compare timestamps
        timestamps.sort(self.cmpTimeStamps)
        path_ = os.path.dirname(timestamps[0])
        # Delete everything in that dir
        self.logger.debug('Choosing previously used archive: %s' % path_)
        self.logger.info('Preparing to delete everything inside: %s' % path_)
        self.io.deleteStuffIn(path_)
        self.logger.info('Have deleted everything inside: %s' % path_)
        self.makeTimeStamp(path_)
        return path_
    
# -----------------------------------------------------------------

class AFSArchivePathSelector(ArchivePathSelector):
    def __init__(self, logger, base_path, narchives):      
        ArchivePathSelector.__init__(self, logger, base_path, narchives) 

    def createUtils(self):
        return IOUtilsFactory(self.logger).create('afs')

    def makeTimeStamp(self, path_):
        ts = os.path.join(path_, 'time_stamp')
        timed_cmd('mkdir -p %s; touch %s' % (path_, ts))

# -----------------------------------------------------------------
    
class CastorArchivePathSelector(ArchivePathSelector):
    def __init__(self, logger, base_path, narchives):
        ArchivePathSelector.__init__(self, logger, base_path, narchives)

    def createUtils(self):
        return IOUtilsFactory(self.logger).create('castor')

    def makeTimeStamp(self, path_):
        ts = os.path.join(path_, 'time_stamp')
        m = 'Making time stamp: %s' % ts
        self.logger.debug(m)
        cmd = 'touch time_stamp;rfmkdir -p %s;rfcp time_stamp %s;nsls -l %s;rm -f time_stamp;' % (path_, path_, ts)
        self.logger.debug("Exec command: %s" % cmd)
        m = 'Reply from cmd was:\n%s' % str(timed_cmd(cmd, 30))
        self.logger.debug(m)
        
# -----------------------------------------------------------------
# -----------------------------------------------------------------
# -----------------------------------------------------------------

class ArchivingCondition:
    class ParentDir:
        def __init__(self, dirDict):
            self.is_    = dirDict.get('is_', [])
            self.isnot_ = dirDict.get('isnot_', [])
            
        def ismatch(self, filepath):
            dirs     = [f.strip() for f in os.path.dirname(filepath).split('/') if f.strip()]
            if self.isnot_ and True in [len(fnmatch.filter(dirs, isnot_))>0 for isnot_ in self.isnot_]: return False
            if not self.is_: return True

            return [len(fnmatch.filter(dirs, is_))>0 for is_ in self.is_].count(True)==len(self.is_)

    class Size:
        def __init__(self, sizeDict):
            self.gt = sizeDict.get('gt', 0)
            self.lt = sizeDict.get('lt', 0)

        def ismatch(self,filepath):
            if self.gt and not self.lt:
                return os.path.getsize(filepath)>self.gt
            elif self.lt and not self.gt:
                return os.path.getsize(filepath)<self.lt
            else:
                return os.path.getsize(filepath)<self.lt and os.path.getsize(filepath)>self.gt
                
    class Name:
        def __init__(self, nameDict):
            self.is_    = nameDict.get('is_', [])
            self.isnot_ = nameDict.get('isnot_', [])
                
        def ismatch(self,filepath):
            filename = os.path.basename(filepath)
            m_   = True in [fnmatch.fnmatch(filename, is_) for is_ in self.is_]
            nm_  = True in [fnmatch.fnmatch(filename, isnot_) for isnot_ in self.isnot_]
            if self.isnot_:
                if self.is_:
                    value = m_ and not nm_
                    return value
                value = not nm_
                return value
            return m_

    def __init__(self, conditionsList):
        self.conditions = conditionsList
    def ismatch(self, f):
        # All these conditions must match
        # This is an A _and_ B _and_ C type condition
        return False not in [c.ismatch(f) for c in self.conditions]

# -----------------------------------------------------------------
# -----------------------------------------------------------------
# -----------------------------------------------------------------

class ArchiverFactory(Factory):
    def __init__(self, logger):
        Factory.__init__(self, logger)

    def create(self, type_, base_path, run_frag, conditions):
        return {'afs': AFSArchiver, 'castor':CastorArchiver}[type_](self.logger, base_path, run_frag, conditions)

# -----------------------------------------------------------------

class Archiver:
    def __init__(self, logger, base_path, run_frag, conditions=[]):
        self.logger      = logger
        self.conditions  = conditions
        self.basepath    = base_path
        self.copiedOK    = []
        self.notCopiedOK = []
        self.run_frag    = run_frag # e.g. nightly/dev/build/i686-slc4-gcc34-opt/offline
        self.archivepath = os.path.join(base_path, run_frag) # full path down to the heirarchy
        self.io = self.createUtils()
        
    def get(self, runpath):
        candidateFiles = []
        for root, dirs, files in os.walk(runpath):
            candidateFiles.extend([os.path.join(root,f) for f in files])
        return candidateFiles

    def archive(self, runpath, dontArchive=[]):
        try:
            matches = [m for m in self.findmatches(runpath) if m not in dontArchive]
            self.copy(matches, runpath)
        except Exception, e:
            self.logger.error('Exception thrown while archiving files from run dir: %s' % runpath)
            self.logger.error('Traceback: \n%s' % str(e))
            
    def copy(self, matchingfiles, runpath):
        if not matchingfiles: return

        statii = []
        for mf in matchingfiles:
            status = self.copyfile(mf)
            if status[0] is None: continue
            statii.append(status)
            
        self.copiedOK    = [s for s in statii if s[0]]
        self.notCopiedOK = [s for s in statii if not s[0]]
        self.logger.info("Copied %d files ok, %d files not ok" % (len(self.copiedOK),
                                                                  len(self.notCopiedOK)))

    def copiedFiles(self):
        return [s[1] for s in self.copiedOK]
        
    def copyfile(self, filepath):
        if not os.path.isfile(filepath): return (None, None, None) # no links or dirs
        try:
            fileRelToHierarchy = filepath.split(self.run_frag[self.run_frag.find('/'):])[1]
        except:
            raise
        if fileRelToHierarchy.startswith('/'): fileRelToHierarchy = fileRelToHierarchy[1:]
        
        src     = filepath
        dirname = os.path.dirname(fileRelToHierarchy)
        dst     = os.path.join(self.archivepath, dirname)

        self.io.mkdir(dst)
        self.io.copyfile(src,dst)
        return (self.io.exists(os.path.join(dst,os.path.basename(filepath))), src, dst)

    def findmatches(self, runpath):        
        matching = []
        for f in self.get(runpath):
            if [c for c in self.conditions if c.ismatch(f)]: matching.append(f)
        return matching

# -----------------------------------------------------------------

class AFSArchiver(Archiver):
    def __init__(self, logger, base_path, run_frag, conditions=[]):
        Archiver.__init__(self, logger, base_path, run_frag, conditions)
        self.type = 'afs'
        
    def createUtils(self):
        return IOUtilsFactory(self.logger).create('afs')

    def duplicate(self):
        return AFSArchiver(self.logger, self.basepath, self.run_frag, self.conditions[:])

# -----------------------------------------------------------------
        
class CastorArchiver(Archiver):
    def __init__(self, logger, base_path, run_frag, conditions=[]):
        Archiver.__init__(self, logger, base_path, run_frag, conditions)
        self.type = 'castor'

    def createUtils(self):
        return IOUtilsFactory(self.logger).create('castor')

    def duplicate(self):
        return CastorArchiver(self.logger, self.basepath, self.run_frag, self.conditions[:])

# -----------------------------------------------------------------
# -----------------------------------------------------------------
# -----------------------------------------------------------------

if __name__ == '__main__':
    class Logger:
        def debug(self, s): pass
        def info(self, s): pass
        def warning(self, s): pass
        def error(self, s): pass

    import sys
    # basepath = '/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/tct/archive/15.2.X.Y/'
    basepath = '/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/brinick/archive'
    run_frag = 'nightly/devval/build/i686-slc4-gcc34-opt/offline'
    aps = CastorArchivePathSelector(Logger(), basepath, 2)
    where =  aps.select_archive(run_frag)
    print where
    sys.exit(1)
    
    # apsf = ArchivePathSelectorFactory(Logger())
    # sel = apsf.create('afs', '/afs/cern.ch/atlas/project/RTT/data/brinick/archive',2)
    # run_frag = 'nightly/dev/build/i686-slc4-gcc34-opt/offline'
    # where = sel.select_archive(run_frag)
    archive_base = where.split(run_frag)[0].strip()
    af = ArchiverFactory(Logger())
    ac = ArchivingCondition(
        [
        # ArchivingCondition.Size({'lt':500.0}),
        ArchivingCondition.ParentDir({'isnot_':['DBR*ase']}),
        # ArchivingCondition.Name({'is_':['*.root', '*.data']})
        ]
        )
    
    archiver = af.create('castor', archive_base, run_frag, [ac])
    rp = '/afs/cern.ch/atlas/offline/external/FullChainTest/tier0/rtt/FCT.Results/rel_0/15.2.X.Y/build/i686-slc4-gcc34-opt/offline/FullChainTests/FCTTransform/Ztautau_csc_recoAOD_trf/46'
    matches =  archiver.findmatches(rp)
    print len(matches)
    for m in matches: print m
    # archiver.archive(rp)

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import os.path
import time
import shutil
from stat import ST_MTIME
from exc2string2 import exc2string2

from Factory_BaseClass import Factory
from RTTSException     import RTTCodingError, RTTInputError, RTTTimeOut
from Tools             import timed_cmd

class IOUtilsFactory(Factory):
    def __init__(self, logger):
        Factory.__init__(self,  logger, self.__class__.__name__)
            
    def create(self, type_):
        dict = {'afs':AFSIOUtils, 'castor':CastorIOUtils}
        try:
            return dict[type_](self.logger)
        except KeyError:
            m  = 'Unknown IO utils type %s, should be one of %s\n' % (type_, dict.keys())
            m += 'This illegal type was likely input through your top level cfg file.'
            raise RTTInputError(m)
        except Exception, e:
            raise RTTCodingError(str(e))
        
# -----------------------------------------------------------------
# -----------------------------------------------------------------
# -----------------------------------------------------------------

class AFSIOUtils:
    def __init__(self, logger):
        self.logger = logger
        
    def mkdir(self, dirToMake):
        try:
            timed_cmd('mkdir -p %s' % dirToMake)
            return self.exists(dirToMake)
        except:
            return False
    def exists(self, d):
        return os.path.exists(d)
    def isdir(self, d):
        return os.path.isdir(d)
    def isfile(self, f):
        return os.path.isfile(f)
    def filesize(self, fpath):
        if not self.isfile(fpath): raise RTTInputError('%s: not a file' % fpath)
        return os.path.getsize(fpath)
    def lastmodtime(self, fpath, asSecsSinceEpoch=False):
        if not self.isfile(fpath): raise RTTInputError('%s: not a file' % fpath)
        stats = time.localtime(os.lstat(fpath)[ST_MTIME])
        if asSecsSinceEpoch: return time.mktime(stats)
        return time.strftime('%y/%m/%d@%H:%M', stats)
    
    def deleteStuffIn(self, path):
        # Deletes everything inside directory path
        if not self.exists(path) or not self.isdir(path): return
        timed_cmd('rm -rf %s/*' % path)
    def copyfile(self,src,dst):
        dst = os.path.join(dst, os.path.basename(src))
        try:
            shutil.copyfile(src,dst)
            return True
        except:
            self.logger.error(exc2string2())
            return False
    
# -----------------------------------------------------------------

class CastorIOUtils:
    def __init__(self, logger):
        self.logger = logger

    def mkdir(self, dirToMake):
        try:
            timed_cmd('rfmkdir -p %s' % dirToMake, 30)
            return True
        except:
            return False

    def path_info(self, fpath):
        try:
            return timed_cmd('nsls -ld %s' % fpath, 30)
        except:
            return None

    def isfile(self, fpath):
        answer = self.path_info(fpath)
        if not answer: return False
        return len(answer)==1 and (not answer[0].strip().startswith('d') and not answer[0].strip().startswith('l'))
        
    def exists(self, fpath):
        answer = self.path_info(fpath)
        if not answer: return False
        return len(answer)>1 or (len(answer)==1 and "No such file or directory" not in answer[0])

    def isdir(self, fpath):
        answer = self.path_info(fpath)
        if not answer: return False
        return len(answer)==1 and answer[0].strip().startswith('d')

    def filesize(self, fpath):
        if not self.isfile(fpath): raise RTTInputError('%s: not a file' % fpath)
        info = self.path_info(fpath)
        return int(info[0].strip().split()[4])

    def lastmodtime(self, fpath, asSecsSinceEpoch=False):
        """Return the last modifed time of fpath, either (default) as a string YY/MM/DD@HH:MM,
        or as seconds since the Unix epoch."""
        if not self.isfile(fpath): raise RTTInputError('%s: not a file' % fpath)
        info = self.path_info(fpath)
        toks = info[0].strip().split()

        fmonth = toks[5]
        fday   = toks[6]
        
        if len(toks[7].split(':'))==1: # year, not time
            fyear = toks[7]
            ftime = '?'
        else:
            fyear = '?'
            ftime = toks[7]

        if asSecsSinceEpoch:
            from datetime import datetime
            if fyear == '?': fyear = time.localtime()[0]
            if ftime == '?': ftime = '00:00'
            fmonth = {'Jan':1,'Feb':2,'Mar':3,'Apr':4,'May':5,'Jun':6,'Jul':7,'Aug':8,'Sep':9,'Oct':10,'Nov':11,'Dec':12}[fmonth]
            dt = datetime(int(fyear),int(fmonth),int(fday),int(ftime.split(':')[0]), int(ftime.split(':')[1]))
            return time.mktime(dt.timetuple())
            
        return '%s/%s/%s@%s' % (fyear, fmonth, fday, ftime)
        
    def copyfile(self, src, dst):
        timed_cmd('rfcp %s %s;' % (src, dst), 120)
        if not self.exists(os.path.join(dst,os.path.basename(src))):
            m = 'Unable to copy file %s to %s' % (src, dst)
            self.logger.error(m)
            return False
        return True
    
    def deleteStuffIn(self, path):
        # Deletes everything inside directory path
        if not self.exists(path) or not self.isdir(path): return            
        timed_cmd('yes | rfrm -r %s;rfmkdir %s;' % (path, os.path.basename(path))) # rfrm -r asks if we are sure, hence pipe yes
        print 'Deleted everything inside %s' % path

if __name__ == '__main__':
    path = '/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/moniFile.db'
    path = '/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/rtt/brinick/archive/0/nightly/15.0.X.Y/build/i686-slc4-gcc34-opt/p1hlt'
    # path = '/afs/cern.ch/atlas/project/RTT/prod/time_stamp'
    cu = CastorIOUtils(None)
    #cu = AFSIOUtils(None)
    
    # cu.path_info(path)
    print cu.exists(path)
    cu.deleteStuffIn(path)
    # print cu.lastmodtime(path, True)

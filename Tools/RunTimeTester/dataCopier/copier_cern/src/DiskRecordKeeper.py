# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os.path, time, os, anydbm
from popen2 import Popen4

class DiskRecordKeeper:
    def __init__(self, dDir):
        self.dir      = dDir
        self.aTimes   = {}
        self.fileInfo = {}

        self.getDatasetNames()
        self.fillLastAccessTimesDict()
        self.storeAccessTimes()

    def getDatasetNames(self):
        #returns a list of datasets on disk
        ls_Dir = Popen4('ls '+self.dir)
        data   = ls_Dir.fromchild.readlines()
        self.datasets=[]
        for ds in data:
            ds = os.path.join(self.dir, ds)
            self.datasets.append(ds[0:-1])
        #return  self.datasets

    def fillLastAccessTimesDict(self):
        for d in self.datasets:
            #self.aTimes[d]=time.ctime(os.path.getatime(d))
            #print os.stat(d)
            self.fileInfo[d] = os.stat(d)
            #self.aTimes[d]   = (os.stat(d)[6], time.ctime(os.stat(d)[7]))
            self.aTimes[d]   = (os.stat(d)[7])

    def storeAccessTimes(self):
        dbH = anydbm.open('FILEACCESSRECORD', 'n')
        for k in self.aTimes.keys():
            dbH[k]= str(self.aTimes[k])

    
    def sortedDictValues(self, adict):
        items = adict.items()
        for item in items:
            backitems=[ [v[1],v[0]] for v in items]
        backitems.sort()
        #print str(items)
        return backitems
        #return [value for key, value in items]



if __name__ =='__main__':
    #drk = DiskRecordKeeper('/unix/atlas/RTTdev/eric')
    drk = DiskRecordKeeper('/unix/atlas1/RTTdata')

    items = drk.sortedDictValues(drk.aTimes)
    #print str(a)

    print 'Last Access Time               size             File Name'
    print '================               ====             =========\n\n'
    for item in items:
        size      = str(os.stat(item[1])[6]/1000000000.0)
        accesstime = str( time.ctime(item[0]))
        filename  = str(item[1])
        
        print  accesstime +'     '+size+' GB        '+filename
        #print str( time.ctime(item[0])) +'    '+ size+' GB          '+str(item[1])

    #print '\n\n\n\n'
    #print 'Last Access Time             File Name'
    #print '================             =========\n\n'
    #for k in drk.aTimes.keys():
    #    #print  k
    #    print str(drk.aTimes[k])+'    '+ k
    #    print '\n'
    

#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys,os,shutil,commands,getFileLists
from PROCTools.getFileLists import *


def getFileSize(pfn):
    "Get size of a file through os.stat (regular file) or rfstat (file on castor)"
    size=None
    pos=pfn.rfind(':')
    if pos!=-1:
        pfn=pfn[1+pos:]
        #print "New pfn:",pfn
    if pfn.startswith("/castor/"):
        cmd= "rfstat "+pfn
        (stat,out)=commands.getstatusoutput(cmd)
        if stat!=0:
            print "ERROR: ",cmd,"failed."
            return None
        for l in out.split(os.linesep):
            if l.startswith("Size"):
                colon=l.index(':')
                size=long(l[1+colon:].strip())
                return size
        print "ERROR: Failed to interpret output of rfstat"
        return None
    else: #Assume regular file
        try:
            statinfo=os.stat(pfn)
            size=statinfo[6]
        except:
            print "Can't acess regular file: ",pfn
        return size


def freeSpace(p):  #wont' work on afs ..
    s = os.statvfs(p)
    return s.f_bsize * s.f_bavail




if __name__=="__main__":
    def usage():
        print "Copy a full TCT to a (local) disk"
        print sys.argv[0],"tctpath destpath"


    if len(sys.argv)!=3:
        usage()
        sys.exit(-1)

    sDir=os.path.normpath(sys.argv[1])
    dDir=os.path.normpath(sys.argv[2])
    
    patterns=(".*myESD.*pool.root$",".*myAOD.*pool.root$","myTAGCOMM.*root$","TAG.root",)
    os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"
    os.environ['STAGE_HOST']="castoratlast3"

    if not os.access(sDir,os.R_OK):
        print "Can't read from",sDir
        sys.exit(-1)

    if not os.access(dDir,os.W_OK):
        try:
            os.mkdir(dDir)
        except OSError,why:
            print "Can't write to",dDir
            print why
            sys.exit(-1)


    ff=findTCTFiles(sDir,"")
    chains=ff.getChains()

    allFilesToCopy=dict()

    print "Searching for files to copy..."
    for name,tci in ff._commonDirs.iteritems():
        filesToCopy=[tci[0].logfile,]
        for p in patterns:
            filesToCopy+=ff.findFilesInDir(tci[0].directory,p)
            
        allFilesToCopy[name]=filesToCopy

    totalSize=0
    nFiles=0
    for n,fs in allFilesToCopy.iteritems():
        #print n
        nFiles+=len(fs)
        for f in fs:
            #print "  ",f
            totalSize+=getFileSize(f)

    print "Found %i files with a total Size of %.2f MB" % (nFiles, totalSize/(1024.0*1024.0))
    print "Start Copying:"


    for n,fs in allFilesToCopy.iteritems():
        if len(fs)>1:
            print "Working on ",n
            logpath=fs[0]
            if not logpath.startswith(sDir):
                print "ERROR, log file path",logpath,"does not start with",sDir
                sys.exit(-1)
            locDir="/".join(logpath[len(sDir):].split("/")[:-1])
            #print locDir
            destdir=dDir+locDir
            try:
                os.makedirs(destdir)
            except OSError,why:
                print "Failed to create directory",destdir
                print why
                sys.exit(-1)

            try:
                shutil.copy2(logpath,destdir+"/"+os.path.basename(logpath))
            except Exception, why:
                print "Can't copy log-file to",os.path.basename(logpath)
                print why
                sys.exit(-1)

            for f in fs[1:]:
                destfile=destdir+"/"+os.path.basename(f)
                if f. startswith("/castor/"):
                    #print "Castor copying",f,"to",destfile
                    cmd="rfcp "+f+" "+destfile
                    #print cmd
                    (stat,out)=commands.getstatusoutput(cmd)
                    if stat!=0:
                        print out
                        print "ERROR: ",cmd,"failed."
                        sys.exit(-1)
                else: # Regular file
                    #print "Copying",f,"to",destfile
                    try:
                        shutil.copy2(f,destfile)
                    except Exception, why:
                        print "Can't copy file to",os.path.basename(logpath)
                        print why
                        sys.exit(-1) 
            
    print "Finished."
    
            

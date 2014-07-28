# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
from xml.dom.minidom import parse
from time import time
import re
from string import *

def tctPath(build,rel):
    if not rel.startswith("rel_"):
        rel="rel_"+rel
    arch = "x86_64-slc6-gcc47-opt"
    if build.startswith("17.") or build.startswith("18."):
        arch = "i686-slc5-gcc43-opt"
    return "/afs/cern.ch/atlas/project/RTT/prod/Results/tct/"+rel+"/"+build+"/build/"+arch+"/offline/Tier0ChainTests/"




class TCTChainInfo:
    def __init__(self,dir,log,runEventIn=()):
        self.directory=dir
        self.logfile=log
        self.eventlist=runEventIn;
        self.loglines=0
        self.cpulist=()
        self.memlist=()

    def addRunEvent(self,run,event):
        self.eventlist+=((run,event),)



class findTCTFiles:
    def __init__(self,rDir,vDir,checkAge=False):
        self._checkAge=checkAge
        self._rDir=rDir
        self._vDir=vDir
        self._commonDirs=dict()
        self._vFiles=[]
        self._rFiles=[]

        
    def checkFileAge(self,path):
        try:
            fileTime=os.stat(path)[8]
        except:
            return
        age=time()-fileTime
        if age>86400: #More than 24h
            print "WARNING! File",path
            print "is more than",int(age/86400.),"day(s) old"
        return


    def hasLogfile(self,ref,p,fl):
        #print "Checking",p
        for f in fl:
            logpath=p+'/'+f
            #print "\t",f
            if os.path.isfile(logpath) and f.endswith("_log") and not f.endswith("_script_log"):
                #name=f[:-4].rstrip(digits) #Was needed for old rtt version, job index number appended
                name=f[:-4]
                tci=TCTChainInfo(p,logpath)
                if ref: #Reference TCT (the first one)
                    if self._commonDirs.has_key(name):
                        print "ERROR: Duplicate directory name",name
                    else:
                        self._commonDirs[name]=(tci,)
                else: #validation (the second one)
                    if self._commonDirs.has_key(name): #exits also in reference set
                        self._commonDirs[name]+=(tci,)
                return
                  
            
    def addNew(self,fileList,file):
        newSplit=file.split("/")
        newStream=newSplit[-3]
        newFile=newSplit[-1]
        for old in fileList:
            oldSplit=old.split("/")
            if oldSplit[-1]==newFile and oldSplit[-3]==newStream:
                return
        #print "Adding",file
        fileList+=[file,]
        return

    def getTCTChainInfo(self,tci):
        logLine="AthenaEventLoopMgr                                   INFO   ===>>>  start processing event #"
        cpuline="Py:PerfMonSvc        INFO <cpu>:    ("
        memline="Py:PerfMonSvc        INFO <vmem>:   ("
        
        try:
            lf=open(tci.logfile,"r")
        except:
            return None

        for l in lf:
            tci.loglines+=1
            if l.startswith(logLine):
                p1=len(logLine)
                p2=l.find(",",p1)
                event=int(l[p1:p2])
                p1=l.find("#",p2)
                p2=l.find(" ",p1)
                run=int(l[p1+1:p2])
                tci.addRunEvent(run,event)

            elif l.startswith(cpuline):
                p1=len(cpuline)
                p2=l.find("+/-",p1)
                cpu=float(l[p1:p2])
                if l.endswith(" s"):
                    cpu*=1000
                tci.cpulist+=(cpu,)

            elif l.startswith(memline):
                p1=len(memline)
                p2=l.find("+/-",p1)
                mem=float(l[p1:p2])
                if l.endswith(" GB"):
                    mem*=1000
                tci.memlist+=(mem,)
                
        lf.close()
        return tci.eventlist


    def getChains(self):
        print "Input directory:",self._rDir
        print "Searching for TCT sub-directories"
        os.path.walk(self._rDir,self.hasLogfile,True)
        print "Found ",len(self._commonDirs),"directories"
        return self._commonDirs
              
        
    def getCommonChains(self):
        print "Seaching for compatible TCT directories ..." 
        allEvents=set()

        os.path.walk(self._rDir,self.hasLogfile,True)   # Reference directory
        os.path.walk(self._vDir,self.hasLogfile,False)  # Validation directory
        
        names=self._commonDirs.keys()
        for tctname in names:
            tcis=self._commonDirs[tctname]

            if len(tcis)!=2:
                self._commonDirs.pop(tctname)
                continue

            ref=tcis[0]
            val=tcis[1]
            
            refEvents=self.getTCTChainInfo(ref)
            if refEvents is None or len(refEvents)==0:
                print "No events found in",ref.logfile
                self._commonDirs.pop(tctname)
                continue
            
            valEvents=self.getTCTChainInfo(val)
            if valEvents is None or len(valEvents)==0:
                print "No events found in",val.logfile
                continue


            sValEvents=set(valEvents)
            sRefEvents=set(refEvents)
            
            if (sValEvents == sRefEvents):
                allEvents|=sRefEvents
                print "TCT",tctname,"is compatible.",len(sRefEvents),"Events."
            else:
                #print "TCT",tctname,"is NOT compatible.",len(refEvents),len(valEvents)
                print "TCT %s is NOT compatible. %i/%i Events." % (tctname,len(sRefEvents),len(sValEvents))
                
                self._commonDirs.pop(tctname)
                
        print "Found %i compatible TCT chains with at total of %i events" % (len(self._commonDirs),len(allEvents))
        #rint "Done"
        #sys.exit(0)
        return self._commonDirs
            

    def findFilesInDir(self,dir,filename):
        #print "Searching dir",dir
        pattern=re.compile(filename)
        result=[]
        ls=os.listdir(dir)
        for f in ls:
            if len(pattern.findall(f)):
                #print "Found",f
                self.addNew(result,dir+"/"+f)
        if "rttjobinfo.xml" in ls:
            castorfiles=self.extractCastorPath(dir+"/rttjobinfo.xml",pattern)
            for f in castorfiles:
                self.addNew(result,f)
        return result
            
    def findFiles(self,pattern):
        result=dict()
        if len(self._commonDirs)==0:
            self.getCommonChains()
        #print self._commonDirs
        
        for (name,(ref,val)) in self._commonDirs.iteritems():
            reffiles=self.findFilesInDir(ref.directory,pattern)
            valfiles=self.findFilesInDir(val.directory,pattern)

            result[name]=()
            #Find pairs:
            for rf in reffiles:
                rfN=rf.split("/")[-1]
                #print "Checking:",rfN
                for vf in valfiles:
                    vfN=vf.split("/")[-1]
                    if (vfN == rfN):
                        result[name]+=((rf,vf),)
                        del vf
                        break
        return result



    def extractCastorPath(self,rttxmlfile,pattern):
        res=[]
        dom=parse(rttxmlfile)
        archfiles=dom.getElementsByTagName("archivefile")
        for af in archfiles:
            cpEle=af.getElementsByTagName("destination")
            castorpath=cpEle[0].childNodes[0].data.strip()
            #print castorpath
            if len(pattern.findall(castorpath)):
                res+=[castorpath,]
            
        del dom
        return res



if __name__=="__main__":
    def usage():
        print "Find compatible pairs of TCT files"
        print "Usage example"
        print sys.argv[0],"15.6.X.Y rel_1 15.6.X.Y-VAL rel_2  'myTAGCOMM.*root'"

    if len(sys.argv) != 6:
        usage()
        sys.exit(-1)

    rPath=tctPath(sys.argv[1],sys.argv[2])
    vPath=tctPath(sys.argv[3],sys.argv[4])

    if not os.access(rPath,os.R_OK):
        print "Can't access output of reference TCT at",rPath
        sys.exit(-1)

    if not os.access(vPath,os.R_OK):
        print "Can't access output of reference TCT at",vPath
        sys.exit(-1)

    pattern=re.compile(sys.argv[5])
    ff=findTCTFiles(rPath,vPath)

    tctlist=ff.getCommonChains()
    
    res=ff.findFiles(pattern)
    for name,fl in res.iteritems():
        print name
        for (r,v) in fl:
            print "Ref:",r
            print "Val:",v
        
    
        

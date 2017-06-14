# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,sys
from xml.dom.minidom import parse
from time import time
import re
from string import *
import json

def tctPath(build,rel):
    if not rel.startswith("rel_"):
        rel="rel_"+rel
    arch = os.environ['CMTCONFIG']
    if build.startswith("17.") or build.startswith("18."):
        arch = "i686-slc5-gcc43-opt"
    return "/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/"+rel+"/"+build+"/build/"+arch+"/Athena/Tier0ChainTests/"

class TCTChainInfo:
    def __init__(self,dir,log,runEventIn=()):
        self.directory=dir
        self.logfile=log
        self.eventlist=runEventIn;
        self.loglines=0
        self.cpulist=()
        self.memlist=()
        self.outputDict={}

    def addRunEvent(self,run,event):
        self.eventlist+=((run,event),)

class findTCTFiles:
    def __init__(self, rDir, vDir, checkAge = False):
        self._checkAge = checkAge
        self._rDir = rDir
        self._vDir = vDir
        self._commonDirs = dict()
        self._vFiles = []
        self._rFiles = []
        
    def checkFileAge(self,path):
        try:
            fileTime = os.stat(path)[8]
        except:
            return
        age = time() - fileTime

        if age > 86400: #More than 24h
            print "WARNING! File %s is more than %d day(s) old" % (path, int(age/86400.))
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

    def findBetween(self, s, first, last ):
        try:
            start = s.index( first ) + len( first )
            end = s.index( last, start )
            return s[start:end]
        except ValueError:
            return ""

    def getTCTChainInfo(self,tci):
        validationStartLine = "INFO Validating output files"
        
        try:
            lf = open(tci.logfile,"r")
        except:
            return None

        # loop through the log file and find all the output files being validated
        foundFileValidationStart = False
        nextLineHasEventCount = False
        lastOutputFileName = ""
        for l in lf:
            tci.loglines += 1
            if not foundFileValidationStart:
                if validationStartLine in l:
                    foundFileValidationStart = True
            elif "Testing event count..." in l:                
                format = self.findBetween(l, "INFO", "Testing event count...").strip()
                tci.outputDict[format] = -1
                nextLineHasEventCount = True
                lastOutputFileName = format
            elif nextLineHasEventCount:
                tci.outputDict[lastOutputFileName] = int(self.findBetween(l, "Event counting test passed (", " events)."))
                nextLineHasEventCount = False
        #print "Done parsing %s, found the following output files:" % tci.logfile
        #for file in tci.outputDict:
        #    print "  %s : %d events" % (file, tci.outputDict[format])
        lf.close()
        return tci.outputDict

    def getChains(self):
        print "Input directory:",self._rDir
        print "Searching for TCT sub-directories"
        os.path.walk(self._rDir,self.hasLogfile,True)
        print "Found ",len(self._commonDirs),"directories"
        return self._commonDirs
              
        
    def getCommonChains(self):
        print "Searching for compatible TCT directories ..." 
        allEvents=0
        
        os.path.walk(self._rDir, self.hasLogfile, True)   # Reference directory
        os.path.walk(self._vDir, self.hasLogfile, False)  # Validation directory
        
        names = self._commonDirs.keys()
        for tctname in names:
            if (tctname.startswith("LatestRun") or tctname.endswith("_MP") or tctname.endswith("IDCosmic0") or tctname.endswith("_PHYSVAL") or tctname.endswith("Derived_Outputs")):
                print "skipping "+tctname
                self._commonDirs.pop(tctname)
                continue

            tcis = self._commonDirs[tctname]

            if len(tcis) != 2:
                self._commonDirs.pop(tctname)
                continue

            ref = tcis[0]
            val = tcis[1]

            formats = ["RDO", "ESD", "AOD", "TAG"] # or anything matching them, though more than one will cause problems! /CO
            
            refEvents = self.getTCTChainInfo(ref)
            if refEvents is None or len(refEvents) == 0:
                print "No events found in",ref.logfile
                self._commonDirs.pop(tctname)
                continue
            
            valEvents = self.getTCTChainInfo(val)
            if valEvents is None or len(valEvents) == 0:
                print "No events found in", val.logfile
                continue

            if (valEvents == refEvents):
                for file in refEvents:
                    allEvents += refEvents[file]
                print "TCT %s output seems compatible for ref and chk:" % (tctname)
                for format in refEvents:
                    print "%-70s: ref: %d events, val: %d events" % (format, refEvents[format], valEvents[format])
            else:
                # workaround for when names of test output files change between two rels 
                print "The (names of the) output files differ in some way:"
                print refEvents
                print valEvents
                print "Will now attempt to match the files by type"
                matchFound = False
                for refFormat in refEvents:
                    if matchFound:
                        break # exit loop if a match was found
                    valFormat = "MOCK"
                    for vFormat in valEvents:
                        #print vFormat
                        for f in formats:
                            if matchFound:
                                break
                            if f in refFormat and f in vFormat:
                                valFormat = vFormat
                                print "Both are %s: %s, %s" % (f, refFormat, valFormat)
                                matchFound = True
                    print "  %s, ref: %d, val: %d" % (format, refEvents[refFormat], valEvents[valFormat])
                if not matchFound:
                    # don't compare the files for this then!
                    self._commonDirs.pop(tctname)
                    print "TCT %s is NOT compatible, outputs different number of events for at least one format:" % tctname
                    print 
                
        print "Found %i compatible TCT chains with at total of %i events" % (len(self._commonDirs), allEvents)
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
            ## temporary fix until RTT API is ready
            if not cpEle:
                s = af.firstChild.nodeValue
                json_acceptable_string = s.replace("'", "\"")
                d = json.loads(json_acceptable_string)
                castorpath=d['src']
            else:
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
        
    
        

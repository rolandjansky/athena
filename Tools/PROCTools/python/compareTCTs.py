#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys
from PROCTools.getFileLists import *
sys.argv += [ '-b' ] # tell ROOT to not use graphics
from ROOT import TFile,TTree
from PROCTools.diffTAGTree import diffTTree

os.environ['STAGE_SVCCLASS']="atlascerngroupdisk"
os.environ['STAGE_HOST']="castoratlast3"

ignoreTrees=set(("CollectionMetadata",))

def compareTreeFiles(rName,vName,details):
    if rName.startswith("/castor"):
        rFile=TFile.Open("root://castoratlas/"+rName)
    elif rName.startswith("/eos"):
        rFile=TFile.Open("root://eosatlas/"+rName)
    else:
        rFile=TFile.Open(rName)

    if vName.startswith("/castor"):
        vFile=TFile.Open("root://castoratlas/"+vName)
    elif vName.startswith("/eos"):
        vFile=TFile.Open("root://eosatlas/"+vName)
    else:
        vFile=TFile.Open(vName)

    if rFile is None:
        print "Failed to open reference file",rName
        return (0,1)
    
    if vFile is None:
        print "Failed to open validation file",vName
        return (0,1)

    rKeys=set()
    for k in rFile.GetListOfKeys():
        rKeys.add(k.GetName())
    vKeys=set()
    for k in vFile.GetListOfKeys():
        vKeys.add(k.GetName())
    #print rKeys
    #print vKeys
    keys=rKeys & vKeys
    keys -= ignoreTrees
        
    if len(keys)==0:
        print "ERROR no common trees names found in files",rName,vName
        return 0

    nGood=0
    nBad=0
    for k in keys:
        rTree=rFile.Get(k)
        vTree=vFile.Get(k)
        if not isinstance(rTree,TTree):
            continue
        if not isinstance(vTree,TTree):
            continue
        print "Comparing TTree",k
        (good,bad)=diffTTree(rTree,vTree,details)
        nGood+=good
        nBad+=bad
    return (nGood,nBad)

def diffPoolFiles(ref,chk,details,toIgnore = ['RecoTimingObj_p1_RAWtoESD_timings', 'RecoTimingObj_p1_ESDtoAOD_timings']):
    import PyUtils.PoolFile as PF
    try:
        df = PF.DiffFiles( refFileName = ref, chkFileName = chk, ignoreList = toIgnore)
        if details is None:
            df.printSummary()
        else:
            df.printSummary(details)
        stat=df.status()
        del df
    except:
        print "Exception caught while diff'ing POOL files"
        stat=True
    return stat 

def diffPickleFiles(ref,chk,details):
    try:
        ref_nlines = open(ref).readlines()
        chk_nlines = open(chk).readlines()
        if len(ref_nlines) == len(chk_nlines):
            stat=False
            print "same number of lines!"
        else :
            print ref," has ",len(ref_nlines)," lines." 
            print chk," has ",len(chk_nlines)," lines."
            stat=True
        #print ref," has ",len(ref_nlines)," lines." 
        #print chk," has ",len(chk_nlines)," lines."
        #for refer, check in zip(ref_nlines,chk_nlines):
        #    if refer != check:
        #        print "Expected %r; got %r " % (refer,check)
        #stat=False
    except:
        stat=True
        print "Exception caught while comparinging jobReport(_RecoTrf)?.gpickle files"
    return stat 


if __name__ == "__main__":
    if len(sys.argv)<3 or sys.argv[1]=="-h" or sys.argv[1]=="--help":
        print "Usage: compareTCTs.py --nRef=<refernce nightly number> --nVal=<validation nightly number> --rRef=<reference nightly> --rVal=<validation nightly> --details=<text file> --file=<pattern> --sum=<summary file> --refPath=<path to ref-nightly> --valPath=<path to val-nightly>"
        print "  Example: compareTCTs.py --nRef=15.6.X.Y --nVal=15.6.X.Y-VAL --rel=rel_4"
        sys.exit(-1)
    
    import getopt
    nRef=None
    nVal=None
    rRef=None
    rVal=None
    detailsFN=None
    filePattern=[]
    sumFileName=None
    refPath=None
    valPath=None
    diffroot=False
    opts,args=getopt.getopt(sys.argv[1:],"b",["nRef=","nVal=","rRef=","rVal=","rel=","nightly=","details=","file=","sum=","refPath=","valPath=","diff-root="])
    for o,a in opts:
        if o=="--nRef": nRef=a
        if o=="--nVal": nVal=a
        if o=="--rRef": rRef=a
        if o=="--rVal": rVal=a
        if o=="--nightly": nRef=nVal=a
        if o=="--rel": rVal=rRef=a
        if o=="--details": detailsFN=a
        if o=="--file": filePattern+=(a,)
        if o=="--sum": sumFileName=a
        if o=="--refPath": refPath=a
        if o=="--valPath": valPath=a
	if o=="--diff-root":
	    if a=="True": diffroot=True
        
    if refPath is None:
        if nRef is None:
            print "Reference nightly not defined! Please use --nRef parameter!"
            sys.exit(-1)

        if rRef is None:
            print "Reference nightly number not defined! Please use --rRef parameter!"
            sys.exit(-1)

    if valPath is None:
        if nVal is None:
            print "Validation nightly not defined! Please use --nVal parameter!"
            sys.exit(-1)
        if rVal is None:
            print "Validation nightly number not defined! Please use --rVal parameter!"
            sys.exit(-1)

    if len(filePattern)==0:
        filePattern+=(".*myESD.*pool.root$",".*myRDO.*pool.root$",".*myAOD.*pool.root$","myTAG.*.root$","jobReport(_RecoTrf)?.gpickle$")
    else:
        for i in range(len(filePattern)):
        #for fP in filePattern:
            if filePattern[i].upper()=="ROOT":
                filePattern[i]="*root$"
            elif filePattern[i].upper()=="POOL":
                filePattern[i]="*.pool.root$"
            elif filePattern[i].upper()=="ESD":
                filePattern[i]=".*myESD.*pool.root$"
            elif filePattern[i].upper()=="RDO":
                filePattern[i]=".*myRDO.*pool.root$"
            elif filePattern[i].upper()=="AOD":
                filePattern[i]=".*myAOD.*pool.root$"
            elif filePattern[i].upper()=="TAGCOMM":
                filePattern[i]="myTAGCOMM.*root$"
            elif filePattern[i]=="TAG":
                filePattern[i]="myTAG.*.root$"
            elif filePattern[i].upper()=="NONE":
                filePattern=[]
            else:
                pass
            
    print "Comparing files matching:" 
    print filePattern

    #Hack to process POOL files first (otherwise inifite loop)
    allPatterns = []
    for fP in filePattern:
        if fP.find("pool") == -1:
            allPatterns.append(fP)
        else:
            allPatterns.insert(0, fP)

    if refPath is None:
        refPath = tctPath(nRef, rRef);

    if valPath is None:
        valPath = tctPath(nVal, rVal);

    if not os.access(refPath, os.R_OK):
        print "Can't access output of reference TCT at",refPath
        sys.exit(-1)

    if not os.access(valPath, os.R_OK):
        print "Can't access output of validation TCT at",valPath
        sys.exit(-1)
   
    msg = "Reference TCT:\n"
    msg += refPath + "\n"
    msg += "Validation TCT:\n"
    msg += valPath + "\n"

    print msg
    if detailsFN is not None:
        details = open(detailsFN, "w")
        details.write(msg)
    else:
        details=None

    ff = findTCTFiles(refPath,valPath)
    tctlist=ff.getCommonChains()
    print "Output from findTCTFile.getCommonChains():"
    print tctlist
        
    statPerChain=dict()
    
    Summary=""
    for pattern in allPatterns:
        nIdenticalFiles = 0
        nDifferentFiles = 0
        filesToCompare = ff.findFiles(pattern)
        print "Will now look for files matching pattern:", pattern
        print "The found files to compare:", filesToCompare
        print "Comparing files matching [%s]" % pattern
        Summary += "Comparing files matching [%s]\n" % pattern
        #for (tctName,r,v) in filesToCompare:
        for name,rv in filesToCompare.iteritems():
            #print "TCT:",name,":",len(rv)
            print "Chain %s: Found %i files matching [%s]" % (name,len(rv),pattern)
            for (r,v) in rv:
                fileName=r.split('/')[-1]
                print "Comparing files",fileName,"of TCT",name
                identical=False
                if (fileName.endswith(".pool.root") and not fileName.startswith("myTAG")):
                    if(r.startswith("/eos")): r = "root://eosatlas/"+r
                    if(v.startswith("/eos")): v = "root://eosatlas/"+v
                    if not diffroot: stat=diffPoolFiles(r,v,details)
                    else: 
		        stat=os.system("acmd.py diff-root "+r+" "+v+" --error-mode resilient --ignore-leaves HITStoRDO_timings RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings --entries 10 > tmp.txt")
                        os.system("cat tmp.txt|grep -v sync")
                        os.system("rm -f tmp.txt")
                    identical=not stat
                elif fileName.endswith(".root"):
                    #(eq,dif)=compareTreeFiles(r,v,details)
                    #if eq==0 and dif==0: continue
                    #identical=(dif==0)
                    stat=os.system("diffTAGTree.py "+r+" "+v+" > tmp.txt")
                    os.system("cat tmp.txt|grep -v sync")
                    os.system("rm -f tmp.txt")
                    identical=not stat
                elif fileName.endswith(".pickle"):
                    stat=diffPickleFiles(r,v,details)
                    identical=not stat
                else:
                    print "ERROR: Don't know how to compare",fileName
                
                if (identical): 
                    nIdenticalFiles+=1 
                    if not statPerChain.has_key(name):
                        statPerChain[name]=False
                    else:
                        statPerChain[name]|=False
                    print "Files are identical"
                else:
                    statPerChain[name]=True
                    nDifferentFiles+=1
                    print "Files are not identical"
            
        Summary+="Found %i identical files and %i different files\n" % (nIdenticalFiles, nDifferentFiles)
        print "Found %i identical files and %i different files\n" % (nIdenticalFiles, nDifferentFiles)
        
    if details is not None:
        details.write(Summary)
        details.close()

    #print Summary

    #print tctlist
    #Check log,mem & cpu,
    complain=""
    for (name,info) in tctlist.iteritems():
        #print name,info,len(info)
        if len(info)<2: continue
        print "\n"+name+":"
        if (info[0].loglines>0 and info[1].loglines>0):
            ratio=100.0*(info[1].loglines-info[0].loglines)/info[0].loglines
            ln="\tLoglines: %i -> %i (%.2f)" %  (info[0].loglines,info[1].loglines,ratio)
            if abs(ratio)>5:
                print ln+"***"
                complain+=name+" "+ln+"***\n"
            else:
                print ln
        if len(info[0].cpulist)>4 and len(info[1].cpulist)>4:
            #ESD
            cpu_r=info[0].cpulist[1]
            cpu_v=info[1].cpulist[1]
            if (cpu_r>0 and cpu_v>0):
                ratio=100.0*(cpu_v-cpu_r)/cpu_r;
                ln="\tESD CPU: %i -> %i (%.2f%%)" %  (cpu_r,cpu_v,ratio)
                if abs(ratio)>15:
                    print ln+"***"
                    complain+=name+" "+ln+"***\n"
                else:
                    print ln

            #AOD
            cpu_r=info[0].cpulist[4]
            cpu_v=info[1].cpulist[4]
            if (cpu_r>0 and cpu_v>0):
                ratio=100.0*(cpu_v-cpu_r)/cpu_r;
                ln="\tAOD CPU: %i -> %i (%.2f%%)" %  (cpu_r,cpu_v,ratio)
                if abs(ratio)>15:
                    print ln+"***"
                    complain+=name+" "+ln+"***\n"
                else:
                    print ln
                    
        if len(info[0].memlist)>4 and len(info[1].memlist)>4:
            mem_r=info[0].memlist[1]
            mem_v=info[1].memlist[1]
            if (mem_r>0 and mem_v>0):
                ratio=100.0*(mem_v-mem_r)/mem_r;
                ln="\tESD MEM: %i -> %i (%.2f%%)" %  (mem_r,mem_v,ratio)
                if abs(ratio)>15:
                    print ln+"***"
                    complain+=name+" "+ln+"***\n"
                else:
                    print ln

            mem_r=info[0].memlist[4]
            mem_v=info[1].memlist[4]
            if (mem_r>0 and mem_v>0):
                ratio=100.0*(mem_v-mem_r)/mem_r;
                ln="\tAOD MEM: %i -> %i (%.2f%%)" %  (mem_r,mem_v,ratio)
                if abs(ratio)>15:
                    print ln+"***"
                    complain+=name+" "+ln+"***\n"
                else:
                    print ln
    
    isok=True
    for f,s in statPerChain.iteritems():
        if s:
            print "%-70s CHANGED" % f
            isok=False
        else:
            print "%-70s IDENTICAL" % f

    if sumFileName is not None:
        sumFile=open(sumFileName,"w")
        for f,s in statPerChain.iteritems():
            line = "%-70s" % f
            if s:
                line += "CHANGED\n"
            else:
                line += "IDENTICAL\n"
            sumFile.write(line)

        sumFile.write("\n\n")
        sumFile.write(complain)
        sumFile.close()
        
    #print statPerChain
    del diffTTree
    if not isok:
        sys.exit(-1)        

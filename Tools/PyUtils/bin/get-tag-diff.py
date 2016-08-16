#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    get-tag-diff.py
# @purpose: Get the list of tag differences between 2 releases (CERN centric)
# @author:  Walter Lampl <walter.lampL@cern.ch> (based on earlier code by Sebastien)
# @date:    April 2016
#
# @example:
#
# getTagDiff rel_3,12.0.X 12.0.3
#


from optparse import OptionParser

import sys
import os,stat
import subprocess
import tempfile
import shutil

#standard os.walk is too slow on our directory strucutre 
def walkCMTPath(root):
    subdirs=os.listdir(root)
    if 'cmt' in subdirs and os.access(root+"/cmt/version.cmt",os.F_OK):
        yield root+"/cmt/version.cmt"
    else: #Loop to find suitable sub-dirs
        for f in subdirs:
            if f in ("InstallArea",".svn",".git"): continue #Ignore those sub-dirs
            nextLevel=root+"/"+f
            if os.path.isdir(nextLevel) and not os.path.islink(nextLevel):
                #print "Diving into ",nextLevel
                for path in walkCMTPath(nextLevel):
                    yield path
                    pass
                pass
            pass
        pass
    return



def parseCMTPath(dumpfilename):
    if not os.access(dumpfilename,os.R_OK) :
        return None

    dumpfile=open(dumpfilename)
    allProjects=dumpfile.readline().strip().split(":") 
    dumpfile.close()
    if len(allProjects)==0 or allProjects[0]=="": 
        return None 

    cmtPackages=dict()
    for projPath in reversed(allProjects): #We want to scan projects in reverse order (so that packages patch overwrite others)
        if len(projPath.split("/")) < 3:
            print "Encountered unexpected path name in CMTPATH", projPath
            return None
        thisProject=projPath.split("/")[-2]
        for cmtversfile in walkCMTPath(projPath):
            versfile=open(cmtversfile)
            fullPackVersion=versfile.readline().strip()
            versfile.close()
            if fullPackVersion.count("-")<3 or fullPackVersion.count("-")>4:
                #print "WARNING, encountered unexpteced version.cmt format", fullPackVersion,fullPackVersion.count("-")
                continue
            if not (fullPackVersion.split("-")[1]).isdigit(): #ignoring things like "DetCommonRelease-DetCommon-00-02-11"
                #print "Ignoring ",fullPackVersion
                continue
                
            packageName=fullPackVersion.split("-")[0]
            packageVersion="-".join(fullPackVersion.split("-")[1:])
            packagePath=cmtversfile[len(projPath)+1:-16]
            cmtPackages[packageName]=(thisProject,packagePath,packageVersion)
            pass
        pass
    print "Found [%i] cmt packages" % len(cmtPackages)
    return cmtPackages


def parseCMAKEPath(dumpfilename):
    if not os.access(dumpfilename,os.R_OK) :
        return None

    dumpfile=open(dumpfilename)
    allProjects=dumpfile.readline().strip().split(":") 
    dumpfile.close()
    if len(allProjects)==0 or allProjects[0]=="": 
        return None 

    #print "allProjects:",allProjects

    cmakePackages=dict()
    for projPath in reversed(allProjects): #We want to scan projects in reverse order (so that packages patch overwrite others)
        if not os.access(projPath+"/packages.txt",os.R_OK):
            print "WARNING, no packages.txt file found in",projPath
        else:
            #Guess project name:
            thisProject=projPath.split("/")[-4]
            #print "Current project is ",thisProject
            packagesFile=open(projPath+"/packages.txt")
            for l in packagesFile:
                ls=l.strip()
                #print "Working on",ls
                if not ls.startswith("#") and len(ls.split(" "))>1:
                    packagePath=ls.split(" ")[0]
                    fullPackVersion=ls.split(" ")[1]
                    packageName=fullPackVersion.split("-")[0]
                    packageVersion="-".join(fullPackVersion.split("-")[1:])
                    cmakePackages[packageName]=(thisProject,packagePath,packageVersion)
                    pass
                pass
            packagesFile.close()
        pass

    print "Found [%i] cmake packages" % len(cmakePackages)    
    return cmakePackages
    


def comparePackageDicts(dictRef,dictChk,header):

    fmt = "%-15s %-15s | %-15s %-15s | %-45s"
    print fmt % (header[0], "ref-project", header[1], "chk-project", "pkg-name")
    print "-" * 100
    allPackages=set(dictRef.keys())
    allPackages.update(dictChk.keys())
    nDiffs=0
    for pack in allPackages:
        if pack in dictRef.keys():
            refEntry=dictRef[pack]
        else:
            refEntry=("None",None,"None")
        
        if pack in dictChk.keys():
            chkEntry=dictChk[pack]
        else:
            chkEntry=("None",None,"None")
        if (chkEntry[2]!=refEntry[2]): #Compare version numbers
            nDiffs+=1
            print fmt % (refEntry[2],refEntry[0].replace("Atlas",""),chkEntry[2],chkEntry[0].replace("Atlas",""),
                         chkEntry[1] or refEntry[1])
            

    print "-"*100
    print "::: found [%i] tags which are different" % nDiffs
    return nDiffs

def getPackageDict(setupcmd):
    tempdir=tempfile.mkdtemp()
    if not os.access(tempdir,os.W_OK):
        print "ERROR, failed to get writeable temp directory!"
        return None

    print "Setting up release %s in temporary directory %s" % (setupcmd,tempdir)
    sys.stdout.flush()

    asetupscriptname=tempdir+"/setupAndPath.sh"
    asetupscriptfile=open(asetupscriptname,"w")
    asetupscriptfile.write("#!/bin/bash\n");
    asetupscriptfile.write("cd %s\n" % tempdir)
    asetupscriptfile.write("set CMTPATH=""\n")
    asetupscriptfile.write("set CMAKE_PREFIX_PATH=""\n")
    asetupscriptfile.write("source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh %s,notest\n" %setupcmd)
    #asetupscriptfile.write("echo $CMTPATH\n")
    #asetupscriptfile.write("echo $CMAKE_PREFIX_PATH\n")
    asetupscriptfile.write("if [ $? -ne 0 ]; then \n   exit -1\nfi\n")
    asetupscriptfile.write("echo $CMTPATH > cmtpath.txt\n")
    asetupscriptfile.write("echo $CMAKE_PREFIX_PATH > cmakepath.txt\n")
    asetupscriptfile.close()

    sc=subprocess.call(["/bin/bash",asetupscriptname],env=dict())
    
    if sc!=0:
        print "ERROR asetup %s failed!" % setupcmd
        shutil.rmtree(tempdir)
        return None

    #Try scanning cmt-like directory structure
    packageVersions=parseCMTPath(tempdir+"/cmtpath.txt")
    if packageVersions is not None:
        #print "Found cmt-like directories"
        pass
    else:
        packageVersions=parseCMAKEPath(tempdir+"/cmakepath.txt")
        if packageVersions is not None:
            #print "Found cmake-like directories"
            pass
        pass

    if packageVersions is None:
        print "ERROR, Neither $CMTPATH nor $CMAKE_PREFIX_PATH found!"
        
    
    shutil.rmtree(tempdir)        
    return packageVersions

if __name__=="__main__":

    parser = OptionParser( usage = "usage: %prog [-R|--ref] 12.0.X,rel_3 [-r|--rel] 12.0.3")
    parser.add_option( "-R",
                       "--ref",
                       dest = "ref",
                       help = "The description string of the reference release (eg: 12.0.X,rel_3)" )

    parser.add_option( "-r",
                       "--chk",
                       dest = "chk",
                       help = "The description string of the to-be-compared release (eg: 12.0.X,rel_3 or 12.0.3)" )


    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.ref = args[0]
        pass

    if len(args) > 1 and args[1][0] != "-":
        options.chk = args[1]
        pass

    if options.ref == None or options.chk == None:
        parser.print_help()
        sys.exit(1)
        pass
    

    print "## Comparing [%s] VS [%s]" % ( options.ref, options.chk )

    sys.stdout.flush()
    refPackages=getPackageDict(options.ref)
    if refPackages is None: sys.exit(-1)

    chkPackages=getPackageDict(options.chk)
    if chkPackages is None: sys.exit(-1)

    comparePackageDicts(refPackages,chkPackages,(options.ref,options.chk))



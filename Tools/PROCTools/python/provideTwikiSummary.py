#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os

def readReleaseList():
    release_list = []
    f = open('releaseList.txt', 'r')
    for line in f:
        try:
            release_list.append(line.split()[0])
        except:
            pass
    return release_list



def cleanDiffRoot(ref, val,q, ftype, path):
    logfile = "%s/run_%s/diff-root-%s.%s.log" %(path,q,q,ftype)
    outfile = "%s_vs_%s_diff-root-%s.%s.txt" %(ref,val,q,ftype)
    if path:
        outfile = "%s/%s_vs_%s/%s" %(path,ref,val,outfile)
    command = 'cat %s  | grep -v "diff= \[" | grep -v "::sync-" > %s' %(logfile,outfile)
    #print command
    os.system(command)


def cleanLog(ref, val, path, log):
#logfile = "/storage/users/mb1063/proc/checks_%s_vs_%s.log" %(ref,val)
    logfile = "%s/%s" %(path,log)
    outfile = "%s_vs_%s_RunTier0Test.txt" %(ref,val)
    if path:
        outfile = "%s/%s_vs_%s/%s" %(path,ref,val,outfile)
    command = 'cat  %s  > %s' %(logfile,outfile)
    #print command
    os.system(command)


def execDiffs(ref, val, path=None):
    filename = "%s_vs_%s_tag-diff.txt" %(ref,val)
    command1 = "get-tag-diff.py --ref=%s --chk=%s > %s/%s_vs_%s/%s" %(ref,val,path,ref,val,filename)
    print command1
    os.system(command1)


"""
  interprets log ATTENTION not all results with breaks T0 is really braking it, please double check
"""
def readLogFile(path,log):
    q221 = "ok"
    q431 = "ok"
    f = open("%s/%s"%(path,log), 'r')
    for line in f:
        if "q221" in line and 'break' in line:
            q221 = "breaks T0"
        if "q431" in line and 'break' in line:
            q431 = "breaks T0"
    return q221, q431

"""
  prints line for twiki table
"""
def PrintTwiki(ref, chk, q221, q431):
    status = "%Y%"
    if "break" in q221 or "break" in q431:
        status = "%X%"
    path = "https://twiki.cern.ch/twiki/pub/Atlas/Tier0CacheReleases"

    twiki = "| [[%s/%s_vs_%s_tag-diff.txt][%s vs %s]] | [[%s/%s_vs_%s_diff-root-q221.ESD.txt][ESD diff]] [[%s/%s_vs_%s_diff-root-q221.AOD.txt][AOD diff]] | [[%s/%s_vs_%s_diff-root-q431.ESD.txt][ESD diff]] [[%s/%s_vs_%s_diff-root-q431.AOD.txt][AOD diff]] | [[%s/%s_vs_%s_RunTier0Test.txt][%s/%s]] | %s |  |" %(path,ref,chk,ref,chk,path,ref,chk,path,ref,chk,path,ref,chk,path,ref,chk,path,ref,chk,q221,q431,status)
    print twiki
    return 0




"""
    
"""
def execute_all_steps(ref,val,path,log):
    #path = "%s_vs_%s" %(ref,val)

    os.system("mkdir %s/%s_vs_%s" %(path,ref,val)) 
    print "INFO getting Tag diff of %s and %s" %(ref,val)
    execDiffs(ref, val, path)
    print "INFO providing RunTier0Test log summary"
    cleanLog(ref,val, path,log)
    print "INFO providing short q221 ESD diff-root summary"
    cleanDiffRoot(ref,val,"q221","ESD", path)
    print "INFO providing short q221 AOD diff-root summary"
    cleanDiffRoot(ref,val,"q221","AOD", path)
    print "INFO providing short q431 ESD diff-root summary"
    cleanDiffRoot(ref,val,"q431","ESD", path)
    print "INFO providing short q431 AOD diff-root summary"
    cleanDiffRoot(ref,val,"q431","AOD", path)

    q221, q431 = readLogFile(path,log)
    PrintTwiki(ref, val, q221, q431)


"""
  execute diff for a list of releases to be compared
"""
def writeManyFiles(release_list):
    for idx,ref in enumerate(release_list):
        try:
            val = release_list[idx+1]
        except:
            return 0
        #print rel, val
        execute_all_steps(ref,val)

from optparse import OptionParser

parser=OptionParser(usage="\n ./provideTwikiSummary.py -r 20.7.5.3 -v 20.7.5.4 -p <path  with RunTier0Tests results>  \n")
parser.add_option("-r","--ref" ,type="string"        ,dest="ref"   ,default=None    ,help="ref release")
parser.add_option("-v","--val" ,type="string"        ,dest="val"   ,default=None    ,help="val release")
parser.add_option("-p","--path" ,type="string"     ,dest="path"  ,default=None    ,help="path where RunTier0Tests was executed")
parser.add_option("-l","--log" ,type="string"     ,dest="log"  ,default="log"    ,help="RunTier0Tests log")
parser.add_option("-f","--file" ,type="string"     ,dest="file"  ,default=None    ,help="file with list of releases")
(options,args)=parser.parse_args()


if options.file:
    release_list = readReleaseList()
    #print release_list
    writeManyFiles(release_list)

if options.ref and options.val:
    execute_all_steps(options.ref,options.val,options.path,options.log)



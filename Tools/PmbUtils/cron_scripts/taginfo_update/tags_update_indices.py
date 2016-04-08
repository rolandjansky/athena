#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Script for running in cron and updating indices of the historical tag
#information saved by nicos. The output is in small and easily
#parsable ascii files, readily usable from for instance python and
#php:

import glob
import datetime
import os

base='/afs/cern.ch/atlas/software/dist/nightlies/nicos_work/tags'
outputbase='/afs/cern.ch/atlas/project/pmb/tags'

def getFirstLines(filename,nlines,nbytesguess=64):
    #attempt to read just the first nlines in a file in an efficient way
    fh=open(filename)
    l=''
    while True:
        l+=fh.read(nbytesguess)
        if l.count('\n')>=nlines:
            break
    fh.close()
    del fh
    return l.split('\n')[0:nlines]

def fileNameToDateTime(fn):
    return datetime.datetime.strptime(os.path.basename(fn),'tags_%Y_%m_%d_H%H')

def getCorrectedDate(approx_date,release_str,dbginfo):
    #Corrects date for nightlies with a cycle of rel_0...rel6 corresponding to weekdays
    if not release_str.startswith('rel_'):
        return approx_date
    target_weekday=(int(release_str[4:])-1)%7#maps atlas rel_X convention to monday=0..sunday=6 convention
    if approx_date.weekday()==target_weekday:
        return approx_date
    deltadays=target_weekday-approx_date.weekday()
    if deltadays>3: deltadays-=7
    if deltadays<-3: deltadays+=7
    if deltadays<-2 or deltadays > 2:
        print "WARNING: Date correction of %i days too high! Not applying! (%s)"%(deltadays,dbginfo)
        return approx_date
    return approx_date+datetime.timedelta(deltadays)

def getRelease(tagfile):
    #returns None in case of errors
    l=getFirstLines(tagfile,2)
    if len(l)!=2: return None
    if not l[0].startswith('#List of tags for'): return None
    if not l[1].startswith('#release'): return None
    return l[1][len('#release'):].strip()

def getTagFiles(dir):
    #Only way to determine if this build has a weekly cycle (and if we
    #thus can apply date corrections) is to look for any
    #'rel_3',...,'rel_6' releases in the same dir
    weeklycyclepatterns=['rel_3','rel_4','rel_5','rel_6']
    weeklycycle=False
    l0=[]
    for f in glob.glob(os.path.join(dir,'tags_*_*_*_H*')):
        d=fileNameToDateTime(f).date()
        rel=getRelease(f)
        if not weeklycycle and rel in weeklycyclepatterns:
            weeklycycle=True
        l0+=[(d,rel,f)]
    if 'mig' in dir.lower() or 'x.y.z-' in dir.lower():
        weeklycycle=False
    out={}
    for d,rel,f in l0:
        if weeklycycle:
            d=getCorrectedDate(d,rel,dbginfo=dir)
        if not d in out.keys():
            out[d]=(rel,f)
        elif fileNameToDateTime(f)>fileNameToDateTime(out[d][1]):
                #Was probably a restarted build
                out[d]=(rel,f)
    dates=out.keys()
    dates.sort()
    l=[]
    for d in dates:
        l+=[(d,out[d][0],out[d][1])]
    return l

#class used to sort cmtconfigs by importance
class Cmtconfig:
    def __init__(self,cmtconfig):
        cpu,platform,compiler,mode=cmtconfig.split('-')
        self.cmtconfig=cmtconfig
        self.platform_value=0
        warn='WARNING: Problems decoding cmtconfig %s'%cmtconfig
        try:
            if 'slc' in platform: self.platform_value=100000000.0+(float(platform[3:]))
            elif 'mac' in platform: self.platform_value=100000.0+(float(platform[3:]))
        except:
            print warn; self.platform_value=0; pass
        self.mode_value=0
        if mode=='dbg':self.mode_value=1
        elif mode=='opt':self.mode_value=2
        self.cpu_value=0
        cpus=['i686','x86_64','i586','i486','i386']
        if cpu in cpus: self.cpu_value=100-cpus.index(cpu)
        self.compiler_value=0
        if compiler.startswith('gcc'):
            try: self.compiler_value=float(compiler[3:])
            except: print warn; self.compiler_value=0; pass

    def __str__(self):
        return self.cmtconfig
    def __ismoreimportantthan(self,other):
        if self.mode_value!=other.mode_value: return self.mode_value>other.mode_value
        if self.platform_value!=other.platform_value: return self.platform_value>other.platform_value
        if self.cpu_value!=other.cpu_value: return self.cpu_value>other.cpu_value
        if self.compiler_value!=other.compiler_value: return self.compiler_value>other.compiler_value
        print "WARNING: Fallback to string comparison of",self.cmtconfig,'and',other.cmtconfig
        return self.cmtconfig>other.cmtconfig
    def __cmp__(self,other):
        if self.cmtconfig==other.cmtconfig: return 0
        if self.__ismoreimportantthan(other): return -1
        return 1

def getTagsIndex(base):
    index={}
    #allcmtconfigs=[]
    for dir in glob.glob(os.path.join(base,'*','*')):
        for d,rel,f in getTagFiles(dir):
            #strip off base part:
            f=f[len(base):]
            if f.startswith('/'): f=f[1:]
            (build,cmtconfig,file)=f.split('/')
            #if not cmtconfig in allcmtconfigs:allcmtconfigs+=[cmtconfig]
            if not build in index.keys():index[build]={}
            if not d in index[build].keys(): index[build][d]=[]
            cf=(Cmtconfig(cmtconfig),os.path.join(os.path.join(cmtconfig,file)),rel)
            index[build][d]+=[cf]

    #sort by Cmtconfig's, and remove the temporary Cmtconfig object:
    for build,dates2infolist in index.items():
        for d,infolist in dates2infolist.items():
            infolist.sort()
            for i in range(len(infolist)):
                infolist[i]=infolist[i][1:]
    return index

def classifyBuilds(index,daysBeforeOldNightly=7,daysBeforeOldRelease=30):
    builds=[]
    today=datetime.date.today()
    for build,dates2files in index.items():
        dates=dates2files.keys()
        if not dates: continue
        dates.sort()
        mostrecentdate=dates[-1]
        rel=dates2files[mostrecentdate][0][1]
        isNightly=rel.startswith('rel_')
        age=(today-mostrecentdate).days
        if isNightly:
            if age>daysBeforeOldNightly: key='oldnightly'
            else: key='nightly'
        else:
            #if len(dates)>1:
            #    print "WARNING: Release has more than one date!",str(dates)
            if age>daysBeforeOldRelease: key='oldrelease'
            else: key='release'
        builds+=[(key,build)]
    builds.sort()
    return builds

def outputBuildsIndexAscii(index,filename):
    fh=open(filename,'w')
    for key,build in classifyBuilds(index):
        dates=index[build].keys()
        dates.sort()
        l=[]
        for d in dates:
            l+=[str(d)]
        fh.write(';'.join([key,build,','.join(l)])+'\n')

def outputBuildInfoIndexAscii(buildinfo,filename):
    fh=open(filename,'w')
    dates=buildinfo.keys()
    dates.sort()
    for d in dates:
        for f,r in buildinfo[d]:
            fh.write(';'.join([str(d)]+f.split('/'))+'\n')

if __name__=='__main__':
    index=getTagsIndex(base)
    outputBuildsIndexAscii(index,os.path.join(outputbase,'index.txt'))
    for build,buildinfo in index.items():
        if '/' in build:
            build=build.replace('/','__')
        outputBuildInfoIndexAscii(buildinfo,os.path.join(outputbase,'build_%s.txt'%build))

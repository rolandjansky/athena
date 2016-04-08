#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Configuration:
rttbase = '/afs/cern.ch/atlas/project/RTT/prod/Results/rtt/'
archivebase = '/afs/cern.ch/atlas/project/pmb/archive/rtt/'
customarchivebase='/afs/cern.ch/atlas/project/pmb/archive/custom/'
archivepackages = ['RecPerfTests','RecExRecoTest','RecExTrigTest','Digitization','G4AtlasTests']#'TrigAnalysisTest/^MC_pp_v2_top.*$'

file_everywhere='rttjobinfo.xml'

#start file patterns with '!' to force compression (compressed log-file is automatically added)

archivefiles = [#'ntuple_*.pmon.gz',
                '!ntuple_*.perfmon.summary.txt','!TransformTimer*.pickle','!AthenaSummary*.txt',
                '!runScript.sh','!postProcessorScript.sh','!postProcessor.log','!rtt.job.status','!rttjobinfo.xml','!dmsg*.txt','!*.pool.root.checkFile*.txt']

archiveprojects = ['offline','tier0']
archivebuilds_alwayskeep=['dev','devval','bugfix','val']
archivebuilds_ignorepatterns=['mig','MIG','old','OLD','val','VAL']

#Imports:

import os.path
from os import makedirs
import glob
import time
import shutil
import sys
import re
import subprocess

def shouldIgnoreBuild(build):
    if build in archivebuilds_alwayskeep: return False
    for s in archivebuilds_ignorepatterns:
        if s in build: return True
    return False

def getJobDirs():
    dirs=[]
    for package in archivepackages:
        jobname_pattern=None
        if '/' in package:
            package,jobname_pattern = package.split('/',2)
        for project in archiveprojects:
            for f in glob.glob(os.path.join(rttbase,'rel_*','*','build','*',project,package,'*')):
                #f should be a job dir!
                parts=f[len(rttbase):].split('/')
                assert parts[2]=='build'
                if jobname_pattern and not re.match(jobname_pattern,parts[-1]):
                    continue
                if shouldIgnoreBuild(parts[1]): continue
                jobinfo = os.path.join(f,file_everywhere)
                if os.path.exists(jobinfo):
                    dirs+=[os.path.dirname(jobinfo)]
    return dirs

def actualCopyOfJobFiles(jobdir,targetdir,logfile):
    mb=1000000
    warnlimit=5*mb
    nevercopy_limit_uncompressed=150*mb
    nevercopy_limit_compressed=40*mb
    archfilepatterns=['!'+logfile]+archivefiles[:]
    for a in archfilepatterns:
        compress=a.startswith('!')
        if compress: a = a[1:]
        for f in glob.glob(os.path.join(jobdir,a)):
            if os.path.basename(f).startswith('<'): continue#work around rtt bug
            lim=nevercopy_limit_uncompressed
            if not compress:
                lim=nevercopy_limit_compressed
            if os.path.getsize(f)>lim:
                print "WARNING: Ignoring very large file [%s] %s"%(os.path.getsize(f),f)
                continue
            shutil.copy2(f,targetdir)
            final=os.path.join(targetdir,os.path.basename(f))
            if compress:
                os.system('/usr/bin/gzip '+final)
                final+='.gz'
                if os.path.getsize(f)>nevercopy_limit_compressed:
                    print "WARNING: Removing very large file [%s] %s"%(os.path.getsize(final),final)
                    os.remove(final)
                    continue
            if 'pmon.gz' in final:
                warnlimit=10*mb
                if 'details_all' in final: warnlimit=25*mb
            if os.path.getsize(final)>warnlimit:
                print "WARNING: Archived file rather big [%s]:"%os.path.getsize(final),final

def jobGroup(jobdir):
    f=os.path.join(jobdir,file_everywhere)
    if not os.path.exists(f):
        return
    jg=False
    for l in open(f):
        if jg:
            return l.strip()
        if '<jobGroup>' in l:
            if '</jobGroup>' in l:
                return l.split('<jobGroup>')[1].split('</jobGroup>')[0]
            jg=True
        

def performArchiveAction(jobdir):
    #todo: sleep a few seconds here - to not hammer too much

    #1) Decode various info from the path:
    assert jobdir.startswith(rttbase)
    parts=jobdir[len(rttbase):].split('/')
    assert len(parts)==7
    logfile=parts[-1]+'_log'
    abslogfile=os.path.join(jobdir,logfile)
    assert parts[0].startswith('rel_')
    weekday=int(parts[0][4:])
    weekday=(weekday-1)%7#to get monday=0, sunday=6
    assert weekday>=0 and weekday<=6

    #2) Look at modification time of the log file to make sure the job is truly done:
    if not os.path.exists(abslogfile): return
    tmod=os.path.getmtime(abslogfile)
    timelim=60*60*3#Should not have been touched for 3 hours
    if (not tmod) or (time.time()-tmod)<timelim:
        return

    #2a) However, for assigning a job-date we will instead look at the
    #    timestamp of runScript.sh:
    runscript=os.path.join(jobdir,'runScript.sh')
    if not os.path.exists(runscript): return
    tmod=os.path.getmtime(runscript)
    if (not tmod) or (time.time()-tmod)<timelim:
        return
    from datetime import date,timedelta
    jobdate=date.fromtimestamp(tmod)

    #3) Figure out the date under which to archive the job. Note that
    #   this should be the date which matches with the "rel_X" number,
    #   and as such might differ by +-1 from the timestamp of the
    #   logfile, and it can even be in the future.

    if jobdate.weekday()!=weekday:
        #print "WARNING: Correcting date!!!"
        deltadays=weekday-jobdate.weekday()
        if deltadays>=5: deltadays-=7
        if deltadays<=-5: deltadays+=7
        assert deltadays>=-2 and deltadays <= 2
        jobdate=jobdate+timedelta(deltadays)

        #FIXME: CoolDozer does not apply this correction, and rel5
        #might actually get the date of a thursday or saturday,
        #depending on the date when something finishes... (update: it does now supposedly)
    assert jobdate.weekday()==weekday

    #4) Ignore if jobdate is older than 2 weeks.
    if abs((date.today()-jobdate).days)>14:
        #print "Ignoring old directory...",jobdir
        return

    #5) Figure out job group (because once upon a time the jobgroup was part of the jobid we want to keep it that way):
    jg=jobGroup(jobdir)
    if not jg:
        print "WARNING: Could not determine jobGroup from",jobdir
        return

    #6) Figure out target directory - and abort if it is already there. Otherwise copy.
    jobbuildid=os.path.join(parts[1],parts[3],parts[4],parts[5],jg,parts[6])
    targetdir=os.path.join(archivebase,str(jobdate.day).rjust(2,'0'),str(jobdate.month).rjust(2,'0'),str(jobdate.year),jobbuildid)

    if not os.path.exists(targetdir):
        makedirs(targetdir)
        actualCopyOfJobFiles(jobdir,targetdir,logfile)
        return True

def updateAllMonthIndices():

    indexbase=os.path.join(archivebase,'index')

    def dirsize(dir):
        assert os.path.isdir(dir)
        dir_size = 0
        for (path, dirs, files) in os.walk(dir):
            for file in files:
                dir_size += os.path.getsize(os.path.join(path, file))
        return dir_size

    def getsubdirs(dir):
        assert os.path.isdir(dir)
        l=[]
        for d in os.listdir(dir):
            if os.path.isdir(os.path.join(dir,d)):
                l+=[d]
        return l

    def indexFile(month,year):
        return os.path.join(indexbase,str(year)+'-'+str(month).rjust(2,'0'))

    def updateIndex(month,year):
        indexfile=indexFile(month,year)

        #collect info from directory structure:
        packages=[]
        info={}
        for day in range(1,1+31):
            basedir=os.path.join(archivebase,str(day).rjust(2,'0'),str(month).rjust(2,'0'),str(year))
            if not os.path.isdir(basedir): continue
            info[day]={}
            for build in getsubdirs(basedir):
                for cmtconfig in getsubdirs(os.path.join(basedir,build)):
                    for cache in getsubdirs(os.path.join(basedir,build,cmtconfig)):
                        for package in getsubdirs(os.path.join(basedir,build,cmtconfig,cache)):
                            pkgdir=os.path.join(basedir,build,cmtconfig,cache,package)
                            if not package in packages:
                                packages += [package]
                            if not package in info[day].keys():
                                info[day][package]={'size':0,'count':0}
                            info[day][package]['size']+=dirsize(pkgdir)
                            for jobgroup in getsubdirs(pkgdir):
                                info[day][package]['count'] += len(getsubdirs(os.path.join(pkgdir,jobgroup)))

        #collect also info from custom jobs:
        package='custom'
        for day in range(1,1+31):
            basedir=os.path.join(customarchivebase,str(day).rjust(2,'0'),str(month).rjust(2,'0'),str(year))
            if not os.path.isdir(basedir): continue
            if not package in packages: packages+=[package]
            if not day in info.keys(): info[day]={}
            assert not package in info[day].keys()
            info[day][package]={'size':0,'count':0}
            for build in getsubdirs(basedir):
                for cmtconfig in getsubdirs(os.path.join(basedir,build)):
                    for job in getsubdirs(os.path.join(basedir,build,cmtconfig)):
                        #fixme: no need to dive into subdirs here actually... we just need the job count
                        jobdir=os.path.join(basedir,build,cmtconfig,job)
                        info[day][package]['size']+=dirsize(jobdir)
                        info[day][package]['count']+=1
        
        #print html table
        style="font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;white-space:nowrap;"
        l=['<table style="%s">'%style]
        fields=['Date','nightly']+packages[:]
        style="background-color:rgb(215,215,255);"
        l+=['  <tr style="%s"><th>'%style+'</th><th>'.join(fields)+'</th></tr>']
        import calendar
        (weekday,lastday)=calendar.monthrange(year,month)
        for day in range(1,1+lastday):
            weekday+=1
            fields=[str(day).rjust(2,'0'),'rel_%i'%(weekday%7)]
            for package in packages:
                if (not day in info.keys()) or (not package in info[day].keys()):
                    fields+=['-']
                else:
                    url='?package=%s&amp;date=%s-%s-%s'%(package,str(day).rjust(2,'0'),str(month).rjust(2,'0'),year)
                    fields+=['<a href="%s">'%url+str(info[day][package]['count'])+' (%.1f mb)</a>'%(info[day][package]['size']/(1024.0*1024.0))]
            l+=['  <tr><td>'+'</td><td>'.join(fields)+'</td></tr>']
        l+=['</table>']

        #write to file:
        file = open(indexfile, 'w')
        for outline in l:
            file.write(outline+'\n')
        file.close()


    #loop over all months from the beginning, and update those that either
    #1) misses an index file or 2) are in the current or previous month.
    import datetime
    firstyear=2010
    firstmonth=1
    thismonth=datetime.date.today().month
    thisyear=datetime.date.today().year

    prevmonth=thismonth-1
    prevmonth_year=thisyear
    if prevmonth==0:
        prevmonth=12
        prevmonth_year=thisyear-1

    for year in range(firstyear,thisyear+1):
        for month in range((firstmonth if year==firstyear else 1),1+(thismonth if year==thisyear else 12)):
            doupdate=(not os.path.exists(indexFile(month,year))) or \
                      ((month==thismonth and year==thisyear) or (month==prevmonth and year==prevmonth_year))
            if doupdate:
                updateIndex(month,year)

if __name__ == '__main__':
    #Acquire lock:
    lock=os.path.join(archivebase,'lock')
    if os.path.exists(lock):
        print "Aborting due to presence of lock file",lock
        sys.exit(1)
    os.system('/bin/touch '+lock)
    assert os.path.exists(lock)

    t0 = time.time()
    timelim_hours=3

    jobdirs = getJobDirs()
    njobdirs=len(jobdirs)
    #print "Found %s job run directories to investigate"%len(jobdirs)
    copied=0
    for jobdir in jobdirs:
        if performArchiveAction(jobdir): copied+=1
        if (time.time()-t0)>timelim_hours*60*60:
            print "WARNING: Running for more than %i hours. Aborting."%timelim_hours
            break

    #print "Performed archival for %s jobs"%str(copied)

    if not copied:
        print "WARNING: Did not archive any jobs at all. Investigated %i job directories"%njobdirs

    updateAllMonthIndices()

    #print "Updated indices"

    #todo: quota warning

    #Release lock:
    assert os.path.exists(lock)
    os.remove(lock)

    #possibly warn about quota:

    maxused=0
    for day in range(1,31+1):
        daypath=os.path.join(archivebase,str(day).rjust(2,'0'))
        cmd='/usr/bin/fs quota %s'%daypath
        p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE)
        output = p.communicate()[0]
        if not '% of quota used' in output:
            print "WARNING: Problems determining quota for %s"%daypath
            continue
        quota_percentage=int(output.split()[0].replace('%',''))
        maxused=max(maxused,quota_percentage)
    if maxused>84:
        print "WARNING: Quota usage of up to %i%%"%maxused

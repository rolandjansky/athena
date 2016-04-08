# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Utilities for extracting parsed information from the archive of
# custom nightly test jobs, including intelligent caching between runs.
#
# Author: Thomas Kittelmann
# First version: April 2011

import PmbUtils.LogFileParsingUtils
import os,sys,datetime,glob,cPickle,gzip
import PerfMonComps.PMonSD

custom_archive='/afs/cern.ch/atlas/project/pmb/archive/custom'
cache_dir='/afs/cern.ch/atlas/project/pmb/archive/custom/cache'

def days_back(n,as_dir_str=False):
    for daydiff in range(-n,1):
        dt=datetime.datetime.now().date()+datetime.timedelta(days=daydiff)
        if as_dir_str: yield dt.strftime('%d/%m/%Y')
        else: yield dt

def getActiveBuilds(joblist,daysback=30):
    builds=set()
    for date_dir in days_back(daysback,as_dir_str=True):
        for j in joblist:
            for f in glob.glob(os.path.join(custom_archive,date_dir,'*','*',j,'__done')):
                b='/'.join(f.split('/')[-4:-2])
                if not b in builds: builds.add(b)
    builds=list(builds)
    builds.sort()
    return builds

def gather(gatherfunction,job,build,daysback=30,verbose=False):
    out={}
    if daysback==None:
        daystr='all'
        daysback=(datetime.datetime.now().date()-datetime.date(2011, 4, 1)).days#days since custom jobs started
    else:
        daystr='last%idays'%daysback
    #Load cache:
    cachefile=os.path.join(cache_dir,'%s_%s_%s_%s.pkl.gz'%(gatherfunction.__name__,daystr,job,build.replace('/','_')))
    cache={}
    if os.path.exists(cachefile):
        try:
            cache=cPickle.load(PerfMonComps.PMonSD.gzip_fastopen(cachefile))
        except Exception,err:
            print "WARNING could not load cache %s:"%cachefile,err
            pass
    cachekeys=set(cache.keys())
    cache_verified=False
    #Loop over required dates and either gather anew or take from cache:
    for dt in days_back(daysback):
        d=os.path.join(custom_archive,dt.strftime('%d/%m/%Y'),build)
        if not os.path.exists(os.path.join(d,job,'__done')): continue
        if dt in cachekeys:
            data=cache[dt]
            if not cache_verified:
                #always load at least one of the cached ones the hard
                #way to detect when something changed in the parsing
                #function and the cache is invalid:
                cache_verified=True
                if verbose: print "Gathering data from",os.path.join(d,job)
                data_check=gatherfunction(os.path.join(d,job))
                if data!=data_check:
                    print "WARNING: Detected changed parsing. Invalidating cache."
                    data=data_check
                    cache,cachekeys={},set()
        else:
            if verbose: print "Gathering data from",os.path.join(d,job)
            data=gatherfunction(os.path.join(d,job))
        out[dt]=data
    PerfMonComps.PMonSD.force_share(out)
    del cache
    if out:
        cPickle.dump(out,gzip.open(cachefile,'w'))
    return out

def pmbparse(d):
    fn=os.path.join(d,'__log.txt.gz')
    return PmbUtils.LogFileParsingUtils.parseall(fn,quiet=True,extra_files=['__command.sh'],extra_msg_parsing=True) if os.path.exists(fn) else None

def log_to_href(filename):
    if not filename.startswith(custom_archive) or not filename.endswith('.gz'): return 'invalid'
    if filename.endswith('__log.txt.gz'): filename=os.path.dirname(filename)
    return 'http://atlas-pmb.web.cern.ch/atlas-pmb/archive/?showlog=%s'%filename[len(custom_archive)+1:]

if __name__=='__main__':

    #define jobs (stream->(long_job,short_job)):
    jobs={}
    #jobs['JetTauEtmiss']=('reco_jettauetmiss','reco_jettauetmiss')
    jobs['Muons']=('reco_muons_250evts','reco_muons_25evts')
    jobs_all=reduce(lambda x,y:x+y,jobs.values())
    jobs_long=map(lambda x:x[0],jobs.values())
    jobs_short=map(lambda x:x[1],jobs.values())

    builds=getActiveBuilds(jobs_all)
    for b in builds:
        for j in jobs_all:
            print len(gather(pmbparse,j,b,daysback=None))

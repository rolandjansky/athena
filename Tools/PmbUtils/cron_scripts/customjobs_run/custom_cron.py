#!/usr/bin/

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script for running a few select jobs on a few select branches on
# specific build machine via cron. This mainly to be able to monitor
# developments in cpu usage.
#
# Several efforts are made to ensure that results are as reliable as
# possible: Job is only started if machine is not too busy, input files
# are copied locally before start, and each job is preceded by a small
# nevts=1 test job, to ensure all afs caches are set up.

import os,glob,time,datetime,sys,subprocess,shutil

##############################################################################
############################# DEFINE DIRS ####################################
##############################################################################

archive_dir      = '/afs/cern.ch/atlas/project/pmb/archive/custom'
amicache_dir     = '/afs/cern.ch/atlas/project/pmb/archive/custom/ami_cache/'
nightlies_dir    = '/afs/cern.ch/atlas/software/builds/nightlies'
username=os.getenv('USER')
tmpdir_base      = '/build/%s/custom_nightly_tests/rundirs'%username
tmp_infiles_base = '/build/%s/custom_nightly_tests/infiles'%username
free_space_checks_gb=[('/build',30.0),('/tmp',2.0)]#todo: also check quota of target archive dir on afs
asetup_dir='/afs/cern.ch/atlas/software/dist/AtlasSetup'

##############################################################################
############################# DEFINE JOBS ####################################
##############################################################################

#
#A job function should return None if it doesn't want to run.
#
#Otherwise it should return (infiles,cmd) where infiles is a list of
#  tuple pairs. The former being the input file location on afs or
#  castor and the latter being the local expected location of the input file (should start with tmp_infiles_base).
#  (todo: CASTOR not supported yet)
#
#It is assumed that all output files starts with "my*"
#

def infile_to_local(f):
    assert f.startswith('/afs')#for now...
    return os.path.join(tmp_infiles_base,os.path.basename(f))

def which(exename):
    for path in os.environ["PATH"].split(os.pathsep):
        fp=os.path.join(path,exename)
        if os.path.exists(fp) and os.access(fp,os.X_OK):
            return fp

def easy_trf_cmd():
    e=which('easy_trf.py')
    if e: return e
    return 'easy_trf.py'
    #Workaround missing PmbUtils with easy_trf.py:
    #afsloc='/afs/cern.ch/atlas/project/pmb/rtt-mon'
    #s='export PYTHONPATH=$PWD:$PYTHONPATH && mkdir PmbUtils && touch PmbUtils/__init__.py && cp /afs/cern.ch/atlas/project/pmb/rtt-mon/PmbUtils/python/AMIUtils.py PmbUtils/ && \\\n'
    #return s+'%s/PmbUtils/share/easy_trf.py'%afsloc

def digi_trf_cmd():
    e=which('Digi_trf.py')
    if e: return e
    return 'Digi_trf.py'

def __digi_cmd(builddate,infile,nevts,short):
    if short: nevts=1
    f=infile_to_local(infile)
    return [(infile,f)],'ATHENA_PYCINTEX_MINVMEM=180 %s inputHitsFile=%s outputRDOFile=myRDO.pool.root maxEvents=%i geometryVersion=ATLAS-GEO-10-00-00 conditionsTag=OFLCOND-SIM-BS14T-00 digiSeedOffset1=11 digiSeedOffset2=22 preExec="from@PerfMonComps.PerfMonFlags@import@jobproperties@as@pmjp,,pmjp.PerfMonFlags.doSemiDetailedMonitoringFullPrint=True" triggerConfig=default preInclude=Digitization/MC09PileUpLumi3.5E33_25ns.py --ignoreerrors=True'%(digi_trf_cmd(),f,nevts)

def __reco_easytrf(tag,infile,nevts,short):
    if short: nevts=1
    f=infile_to_local(infile)
    amicache=os.path.join(amicache_dir,'ami_%s.pkl'%tag)
    tmpfix='--extrapostexec=topSequence.JobInfo.PrintFATAL=False'
    return [(infile,f)],'%s run %s:pmb amicache:%s %s %i %s'%(easy_trf_cmd(),tag,amicache,f,nevts,tmpfix)

def __ftag_by_date(builddate):
    #for consistent operation, add new tag starting the following day only.
    if builddate>=datetime.date(2011, 10, 13): return 'f411'
    if builddate>=datetime.date(2011,  9, 24): return 'f405'
    if builddate>=datetime.date(2011,  8, 18): return 'f395'
    if builddate>=datetime.date(2011,  7, 30): return 'f392'
    if builddate>=datetime.date(2011,  5, 25): return 'f380'
    if builddate>=datetime.date(2011,  5,  6): return 'f371'
    if builddate>=datetime.date(2011,  4, 15): return 'f361'
    return 'f354'

def __nevts(nevts,builddate):
    if nevts=='many':
        if builddate>=datetime.date(2011, 7, 27): return 500
        return 250
    return nevts
    
def __reco_jettauetmiss(branch,cmtcfg,builddate,short,nevts,extra_tags=None):
    #return None if shouldn't run on branch/cmtcfg,weekday
    infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00178044.physics_JetTauEtmiss.merge.RAW._lb0092._SFO-11._0001.1_250evts'
    if builddate>=datetime.date(2011, 6, 19):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00182726.physics_JetTauEtmiss.merge.RAW._lb0215._SFO-5._0001.1_1000evts'
    if builddate>=datetime.date(2011, 10, 8):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00189822.physics_JetTauEtmiss.merge.RAW._lb0120._SFO-11._0001.1_1000evts'
    ftag=__ftag_by_date(builddate)
    if extra_tags:
        ftag=':'.join([ftag]+extra_tags)
    return __reco_easytrf(ftag,infile,__nevts(nevts,builddate),short)
def reco_jettauetmiss(**kw): return __reco_jettauetmiss(nevts='many',**kw)

def __reco_muons(branch,cmtcfg,builddate,short,nevts):
    #return None if shouldn't run on branch/cmtcfg,weekday
    infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00178044.physics_Muons.merge.RAW._lb0092._SFO-ALL._0001.1_250evts'
    if builddate>=datetime.date(2011, 6, 19):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00182726.physics_Muons.merge.RAW._lb0215._SFO-5._0001.1_500evts'
    if builddate>=datetime.date(2011, 10, 8):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00189822.physics_Muons.merge.RAW._lb0120._SFO-11._0001.1_500evts'
    return __reco_easytrf(__ftag_by_date(builddate),infile,__nevts(nevts,builddate),short)
def reco_muons(**kw): return __reco_muons(nevts='many',**kw)

def __reco_egamma(branch,cmtcfg,builddate,short,nevts):
    #return None if shouldn't run on branch/cmtcfg,weekday
    infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00178044.physics_Egamma.merge.RAW._lb0092._SFO-ALL._0001.1_250evts'
    if builddate>=datetime.date(2011, 6, 19):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00182726.physics_Egamma.merge.RAW._lb0215._SFO-5._0001.1_500evts'
    if builddate>=datetime.date(2011, 10, 8):
        infile='/afs/cern.ch/atlas/project/pmb/data/data11_7TeV.00189822.physics_Egamma.merge.RAW._lb0120._SFO-11._0001.1_500evts'
    return __reco_easytrf(__ftag_by_date(builddate),infile,__nevts(nevts,builddate),short)
def reco_egamma(**kw): return __reco_egamma(nevts='many',**kw)

def reco_jettauetmiss_msgabuse_5evts(**kw): return __reco_jettauetmiss(nevts=5,extra_tags=['msgdbg'],**kw)
def reco_jettauetmiss_msgabuse_25evts(**kw): return __reco_jettauetmiss(nevts=25,extra_tags=['msgdbg'],**kw)

def digi(branch,cmtcfg,builddate,short):
    f='/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/pileupInputs/mc09_14TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e564_s851_tid149330_00/HITS.149330._000232.pool.root.1'
    return __digi_cmd(builddate,f,25,short)

##############################################################################
################### DEFINE BRANCHES AND WHICH JOBS TO RUN ####################
##############################################################################

joblist_allstreams=[reco_jettauetmiss,reco_muons,reco_egamma]
joblist_jetonly=[reco_jettauetmiss]
joblist_msgabuse=[reco_jettauetmiss_msgabuse_5evts,reco_jettauetmiss_msgabuse_25evts]
joblist_digi=[digi]

runlist=[]
#digi:
runlist+=[('dev','x86_64-slc5-gcc43-opt',joblist_digi)]
#main interest is dev+tier0 non-val builds. Run all streams:
runlist+=[('dev','i686-slc5-gcc43-opt',joblist_allstreams)]
runlist+=[('17.2.X.Y','i686-slc5-gcc43-opt',joblist_allstreams)]
#For specialised setups we run just one stream:
runlist+=[('17.2.X.Y-VAL','i686-slc5-gcc43-opt',joblist_jetonly)]
runlist+=[('17.2.X.Y','x86_64-slc5-gcc43-opt',joblist_jetonly)]
runlist+=[('devval','x86_64-slc5-gcc43-opt',joblist_jetonly)]
runlist+=[('dev','x86_64-slc5-gcc43-opt',joblist_jetonly)]
runlist+=[('17.1.X.Y','i686-slc5-gcc43-opt',joblist_jetonly)]
#runlist+=[('16.6.X.Y','i686-slc5-gcc43-opt',joblist_jetonly)]
#runlist+=[('mig6','i686-slc5-gcc43-opt',joblist_jetonly)]#doesn't work due to 2-day cycle
#Msg abuse monitoring needs two special jobs in a debug build:
runlist+=[('dev','x86_64-slc5-gcc43-dbg',joblist_msgabuse)]
runlist+=[('devval','i686-slc5-gcc43-dbg',joblist_msgabuse)]
runlist+=[('devval','x86_64-slc5-gcc43-dbg',joblist_msgabuse)]

##############################################################################
####################### ABORT IF MACHINE NOT QUIET ###########################
##############################################################################

def get_load():
    #duplicated in Misc.py
    p=subprocess.Popen('/bin/ps -eo pcpu --no-headers'.split(), stdout=subprocess.PIPE).communicate()[0]
    return 0.01*sum(map(lambda x:float(x),p.split()))

def get_n_cores():
    #duplicated in Misc.py
    n=0
    for l in open('/proc/cpuinfo'):
        if l.startswith('processor') and l.split()[0:2]==['processor',':']:
            n+=1
    return n

def ok_to_run():
    l=get_load()
    n=get_n_cores()
    if n==1: return l<0.05
    return l<n*0.33

if not ok_to_run():
    sys.exit(0)#not an error, will relaunch later

##############################################################################
######################### ABORT IF LIMITED SPACE #############################
##############################################################################

def get_free_space(mountpoint):
    if not os.path.exists(mountpoint):
        return None
    for l in subprocess.Popen(['df','-k',mountpoint], stdout=subprocess.PIPE).communicate()[0].split('\n'):
        l=l.split()
        if l and l[-1]==mountpoint: return float(l[-3])/(1024.0*1024.0)

space_ok=True
for mountpoint,limit in free_space_checks_gb:
    fs=get_free_space(mountpoint)
    if fs==None:
        print "Could not determine space left on %s"%mountpoint
        space_ok=False
    elif fs<limit:
        print "Too little space left on %s : %.2f gb (required %.2f gb)"%(mountpoint,fs,limit)
        space_ok=False
if not space_ok:
    print "Aborting."
    sys.exit(1)

##############################################################################
#################### FIGURE OUT WHICH JOBS TO RUN ############################
##############################################################################

def touch(filename):
    #only creates if doesn't exist. Won't update timestamp of existing file.
    if not os.path.exists(filename):
        open(filename,'wa').close()

def decode_build_date(relNstr,modtime):
    assert relNstr.startswith('rel_') and len(relNstr)==5 and relNstr[-1] in '0123456'
    weekday=int(relNstr[4:])
    weekday=(weekday-1)%7#to get monday=0, sunday=6
    assert weekday>=0 and weekday<=6
    builddate=datetime.date.fromtimestamp(modtime)
    #Adjust builddate so it matches the rel_X number exactly (rel_1=monday, etc.):
    if builddate.weekday()!=weekday:
        #print "WARNING: Correcting date!!!"
        deltadays=weekday-builddate.weekday()
        if deltadays>=5: deltadays-=7
        if deltadays<=-5: deltadays+=7
        assert deltadays>=-2 and deltadays<=2
        builddate=builddate+datetime.timedelta(deltadays)
    return builddate

asetup='export AtlasSetup=%s && source %s/scripts/asetup.sh'%((asetup_dir,)*2)

for b,cmtcfg,joblist in runlist:
    pattern='%s/%s/AtlasProduction/rel_*/InstallArea/%s/lib'%(nightlies_dir,b,cmtcfg)
    dirs=glob.glob(pattern)
    for d in dirs:
        if not os.path.isdir(d): continue#protect against afs issues
        dtime=os.path.getmtime(d)
        hours_ago=(time.time()-dtime)/(3600.0)
        #To stay away from builds in progress the timestamp should have an age of at least 3 hours
        if hours_ago<3: continue
        #To stay away from builds about to be restarted, the timestamp should have an age of at most 5 days.
        if hours_ago>24*5: continue
        #Get build date:
        relN=d.split('/')[-4]
        builddate=decode_build_date(relN,dtime)
        build_archive_dir = os.path.join(archive_dir,builddate.strftime('%d/%m/%Y'),b,cmtcfg)
        #Figure out atlas setup command:
        asetup_args=cmtcfg.split('-')+[b,relN]
        if asetup_args[0]=='x86_64': asetup_args[0]='64'
        else: asetup_args[0]='32'
        asetup_cmd=asetup+' --notest --buildsbase --runtime --nosave --input None '+','.join(asetup_args)

        #Figure out which jobs to run:
        for job in joblist:
            #Get command + short command:
            infiles,cmd=job(branch=b,cmtcfg=cmtcfg,builddate=builddate,short=False)
            if not cmd: continue#not on this build apparently
            for srcfile,localfile in infiles:
                if os.path.exists(localfile): continue
                if not os.path.isdir(os.path.dirname(localfile)):
                    os.makedirs(os.path.dirname(localfile))
                shutil.copy2(srcfile,localfile)
                assert os.path.exists(localfile)

            infiles_short,cmd_short=job(branch=b,cmtcfg=cmtcfg,builddate=builddate,short=True)
            assert infiles==infiles_short
            assert cmd_short

            #Check if done already done, and if not acquire "lock" by making target output dir:
            target_dir=os.path.join(build_archive_dir,job.__name__)
            file_done=os.path.join(target_dir,'__done')
            file_start=os.path.join(target_dir,'__start')
            #file_exitcode=os.path.join(target_dir,'__exitcode')
            #file_logfile=os.path.join(target_dir,'__log.txt.gz')
            if os.path.isdir(target_dir):
                if os.path.exists(file_done):
                    continue
                #already running or done (todo: check here that the job actually finished in 24 hours... otherwise delete and relaunch)
                if not os.path.exists(file_start): time.sleep(60)
                if not os.path.exists(file_start) or (time.time()-os.path.getmtime(file_start))>24*3600:
                    print "Warning: Having to remove %s"%target_dir
                    shutil.rmtree(target_dir)
                else:
                    continue
            try:
                os.makedirs(target_dir)#Now we are in charge...
                assert os.path.isdir(target_dir)
                touch(file_start)
            except Exception,err:
                print "ERROR: Problems encountered while making %s"%target_dir
                print "Aborting."
                if os.path.isdir(target_dir): shutil.rmtree(target_dir)
                sys.exit(1)
                pass

            #Temporary run-dir:
            tmpdir=os.path.join(tmpdir_base,'_'.join([b,cmtcfg,relN,job.__name__]))
            tmpdir_start=os.path.join(tmpdir,'__start')
            if os.path.exists(tmpdir):
                canremove=False
                if not os.path.exists(tmpdir_start): time.sleep(20)
                if not os.path.exists(tmpdir_start): canremove=True
                elif (time.time()-os.path.getmtime(tmpdir_start))>24*3600: canremove=True
                if not canremove:
                    print "WARNING: Found unexpected recent tmp rundir %s"%tmpdir
                    continue
                shutil.rmtree(tmpdir)
            os.makedirs(tmpdir)
            touch(tmpdir_start)

            #NB: Stop after one job has run! Cron will relaunch us each hour and will then run the next job!

            def cmd_gen(_cmd,_asetup_cmd,rundir,infiles):
                file_command=os.path.join(rundir,'__command.sh')
                cmds=['#!/bin/bash',
                      '','#Input files are copied locally before running. Sources are:']
                i=0
                for src,local in infiles:
                    i+=1
                    cmds+=['# %i) %s => %s'%(i,src,local)]
                cmds+=['','touch __start_asetup',_asetup_cmd+' && touch __start && \\',_cmd+' >__log.txt 2>&1','echo $? > __exitcode']
                cmds+=['for f in my*.pool.root; do',
                       '    if [ -f "$f" ]; then checkFile.py $f > $f.checkfile.txt 2>/dev/null; fi',
                       'done']
                cmds+=['BAD=0',
                       'for f in ntuple*.pmon.gz; do',
                       '    if [ -f "$f" ]; then tar xf $f "*.pmonsd.*" || BAD=1; fi',
                       'done',
                       'if [ $BAD != 0 ]; then echo "ERROR: tar problems in $PWD"; fi']
                cmds+=["ls -oqAQ1S --block-size=1 --ignore='__*' > __dirlist.txt"]
                cmds+=['gzip *.txt','touch __done','']
                fh=open(file_command,'w')
                fh.write('\n'.join(cmds))
                fh.close()
                return file_command
            tmpdir_shortrun=os.path.join(tmpdir,'prerun')
            os.makedirs(tmpdir_shortrun)
            tmpdir_run=os.path.join(tmpdir,'run')
            os.makedirs(tmpdir_run)
            cmdfile_short=cmd_gen(cmd_short,asetup_cmd,tmpdir_shortrun,infiles_short)
            cmdfile=cmd_gen(cmd,asetup_cmd,tmpdir_run,infiles)
            ec1=os.system('cd %s && source %s'%(tmpdir_shortrun,cmdfile_short))
            ec2=os.system('cd %s && source %s'%(tmpdir_run,cmdfile))
            if ec1 or ec2:
                print "NB: Problems in job",target_dir
            collect=[]
            for f in os.listdir(tmpdir_run):
                if os.path.isdir(f): continue
                bn=os.path.basename(f)
                if bn.startswith('__') or '.checkfile.txt.gz' in bn or 'pmonsd' in bn:
                    collect+=[bn]
            for c in collect:
                shutil.copy2(os.path.join(tmpdir_run,c),target_dir)
            shutil.rmtree(tmpdir)
            touch(file_done)
            sys.exit(0)#On purpose we stop after one job has run...

#colour weekends differently in plots? Vertical red lines when input file changed

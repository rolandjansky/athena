#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PmbUtils.ArchiveUtils
import PmbUtils.LogFileParsingUtils
import PmbUtils.TransformTimerUtils
import os

def getval(adict,keys):
    if not adict or not keys: return None
    if len(keys)==1:
        if not keys[0] in adict.keys(): return None
        return adict[keys[0]]
    if isinstance(adict,dict):
        if not keys[0] in adict.keys(): return None
    else:
        if keys[0]>=len(adict): return None
    return getval(adict[keys[0]],keys[1:])

def getvals(listofdicts,keys):
    l=[]
    for d in listofdicts:
        val=getval(d,keys)
        if val!=None: l+=[val]
    return l

def sortAndUniqify(alist):
    mylist=alist[:]
    mylist.sort()
    first=True
    last=None
    out=[]
    for e in mylist:
        if first or last!=e:
            last=e
            first=False
            out+=[e]
    return out

import numpy


#def locate(pattern, root=None):
#    if root==None: root=os.curdir
#    import os,fnmatch
#    for path, dirs, files in os.walk(os.path.abspath(root)):
#        for filename in fnmatch.filter(files, pattern):
#            yield os.path.join(path, filename)
                            

###############################################################################
def getNumbers(builddir,jobnamebase,pmonsumfilenames,normcomp,normcompval):
    import glob
    stages=['evt','ini','1st','fin']

    #extract the detailed perfmon summaries of up to five identical jobs:
    detailedsummaries=[]
    miscinfos=[]
    for i in range(5):
        jobname=jobnamebase
        if i>0: jobname='%s_%i'%(jobname,i+1)
        (jobdir,logfile)=PmbUtils.ArchiveUtils.getJobDirAndLog(builddir,'RecPerfTests',jobname,'TrfRecPerfBS')
        if not jobdir: continue
        lfnfo=PmbUtils.LogFileParsingUtils.parseAthenaLogFileForPerfMonSummary(logfile,addTransformTimerInfo=True,extractMiscInfo=True,quiet=True)
        if not lfnfo['misc']['statusok']: continue
        #The key in the transform timer has evolved a bit (some old
        #jobs had "TOTAL" instead as key, but I am not really sure
        #those numbers can be trusted):
        trfkey=None
        for key in ['BaseOfBasicTrf','RAWtoESD','ESDtoAOD']:
            if key in lfnfo['transform_timings'].keys(): trfkey=key
        if not trfkey: continue
        if not '__total__' in lfnfo['transform_timings'][trfkey].keys(): continue
        tt=lfnfo['transform_timings'][trfkey]['__total__']
        if tt['wall-time']/float(tt['cpu-time'])>2.0: continue

        #if lfnfo['misc']['si2k']<2900.0: continue
        for pmonsumfilename in pmonsumfilenames:
            candidates=glob.glob(os.path.join(jobdir,pmonsumfilename))
            if len(candidates)==1:
                pmonsumfile=candidates[0]
                break
            else:
                pmonsumfile=None
        if pmonsumfile==None: continue

        #if not os.path.exists(pmonsumfile):
        #    #try as a pattern:
        #    candidates=glob.glob(pmonsumfile)
        #    if len(candidates)==1:
        #        pmonsumfile=candidates[0]
        #    else:
        #        continue
        (misc,nfo)=PmbUtils.LogFileParsingUtils.parseDetailedPerfmonSummaryTextFile(pmonsumfile)
        if not nfo: continue
        nfo['misc']={'si2k':lfnfo['misc']['si2k'],'vmem_max':lfnfo['misc']['vmem_max']}
        #print "MAX VMEM:",lfnfo['misc']['vmem_max']
        detailedsummaries+=[nfo]
        miscinfos+=[misc]

    if not detailedsummaries: return

    #Find all components
    allstages=['evt','ini','1st','fin']
    allcomps=[]
    for summary in detailedsummaries:
        for stage in allstages:
            allcomps+=summary[stage].keys()
    allcomps=sortAndUniqify(allcomps)
    #    #obsolete:
    #    (jobdir,logfile)=PmbUtils.ArchiveUtils.getJobDirAndLog(builddir,'RecPerfTests','details_all_bs','TrfRecPerfBS')
    #    if not jobdir: return
    #    pmonsumfile=os.path.join(jobdir,'ntuple_bstoesddpd.perfmon.summary.txt.gz')
    #    if not os.path.exists(pmonsumfile): return
    #    (misc,nfo)=PmbUtils.LogFileParsingUtils.parseDetailedPerfmonSummaryTextFile(pmonsumfile)
    #    if not nfo: return

    #Extract numbers
    out={}
    #First cpu numbers:
    comp2overhead={}

    normfact=1.0
    normvals=[]
    #(normcomp,normcompfact)=(None,None)
    if normcomp:
        #(normcomp,normcompfact)=normcompfct(thedate)
        assert normcomp in allcomps
        normvals=getvals(detailedsummaries,['evt',normcomp,'cpu'])
        if normvals:
            normfact=normcompval/min(normvals)
        else: normfact=1.0

    for stage in allstages:
        evtstage=(stage=='evt')
        outstage={}
        si2kfacts=getvals(detailedsummaries,['misc','si2k'])#fixme: if si2k factors are not needed, no need to scan the log file
        #normvals=getvals(detailedsummaries,[stage,normcomp,'cpu'])
        if normvals:
            ibestjob=normvals.index(min(normvals))
        else:
            ibestjob=None
        #if not normvals: continue
        for comp in allcomps:
            vals=getvals(detailedsummaries,[stage,comp,'cpu'])
            #if vals and len(vals)==len(si2kfacts):
            if vals:# and len(vals)==len(normvals):
                #for i in range(len(vals)): vals[i]*=(2500.0/si2kfacts[i])
                #for i in range(len(normvals)):
                #    vals[i]*=(400.0/max(1,normvals[i]))
                #val=min(vals)*normfact
                if ibestjob!=None:
                    val=vals[ibestjob]
                else:
                    val=min(vals)
                val=val*normfact
                if evtstage:
                    outstage[comp]=val
                    if comp==normcomp:
                        outstage['normfluct']=val/normfact-normcompval
                    val=-val
                if not comp in comp2overhead.keys(): comp2overhead[comp]=val*0.001
                else: comp2overhead[comp]+=val*0.001

        if evtstage: out['cpu_evt']=outstage

    out['cpu_overhead']=comp2overhead

    #Then vmem and malloc/nalloc/nfree (malloc/nalloc/nfree for evt and vmem for ini+1st+evt):
    comp2vmem={}
    comp2malloc={}
    comp2nalloc={}
    comp2nfree={}
    comp2nallocdiff={}
    vmem_max=numpy.mean(getvals(detailedsummaries,['misc','vmem_max']))
    for stage in allstages:
        if stage=='fin': continue#adding finalisation numbers would probably not give the results we are interested in.
        for comp in allcomps:
            if stage=='evt':
                vals_malloc=getvals(detailedsummaries,[stage,comp,'malloc'])
                if vals_malloc: comp2malloc[comp]=numpy.mean(vals_malloc)/1024.0
                nalloc=None
                nfree=None
                vals_nalloc=getvals(detailedsummaries,[stage,comp,'nalloc'])
                if vals_nalloc:
                    nalloc=numpy.mean(vals_nalloc)/1000000.0
                    comp2nalloc[comp]=nalloc
                vals_nfree=getvals(detailedsummaries,[stage,comp,'nfree'])
                if vals_nfree:
                    nfree=numpy.mean(vals_nfree)/1000000.0
                    comp2nfree[comp]=nfree
                if nfree!=None and nalloc!=None:
                    comp2nallocdiff[comp]=nalloc-nfree
            vals_vmem=getvals(detailedsummaries,[stage,comp,'vmem'])
            if vals_vmem:
                val=numpy.mean(vals_vmem)
                if comp in comp2vmem.keys(): comp2vmem[comp]+=val
                else: comp2vmem[comp]=val
#        for comp,data in stagedata.items():
#            if stage=='evt' and 'malloc' in data.keys():
#                comp2malloc[comp]=data['malloc']/1024.0
#            if not 'vmem' in data.keys():
#                continue
#            val=data['vmem']
#            if not val: continue
#            if comp in comp2vmem.keys(): comp2vmem[comp]+=val
#            else: comp2vmem[comp]=val
#
    outvmem={}
    othervmem=0.0
    for c,v in comp2vmem.items(): outvmem[c]=v/1024.0
    core_vals=getvals(miscinfos,['vmem_core'])
    if core_vals:
        outvmem['core']=numpy.mean(core_vals)
    tot=0.0
    for c,v in outvmem.items(): tot+=v
    outvmem['unaccounted']=vmem_max-tot
    out['vmem']=outvmem
    out['malloc']=comp2malloc
    if comp2nalloc: out['nalloc']=comp2nalloc
    if comp2nallocdiff: out['nallocdiff']=comp2nallocdiff
    if comp2nfree: out['nfree']=comp2nfree
    out['leak_per_evt']=numpy.mean(getvals(miscinfos,['vmem_fit_growth']))
    return out

def analyseBuildForTransformTimerInfo(builddir):
    badval=(None,None)
    (jobdir,logfile)=PmbUtils.ArchiveUtils.getJobDirAndLog(builddir,'RecPerfTests','fullchain_bs','TrfRecPerfBS')
    if not (jobdir and logfile):
        return badval
    (jobdir_short,logfile_short)=PmbUtils.ArchiveUtils.getJobDirAndLog(builddir,'RecPerfTests','fullchain_short_bs','TrfRecPerfBS')
    if not (jobdir_short and logfile_short):
        return badval
    lfnfo=PmbUtils.LogFileParsingUtils.parseAthenaLogFileForPerfMonSummary(logfile_short,addTransformTimerInfo=False,extractMiscInfo=True,quiet=True)
    if not lfnfo or not lfnfo['misc']['statusok']:
        return badval
    tt_nshort=lfnfo['misc']['inputevts']
    lfnfo=PmbUtils.LogFileParsingUtils.parseAthenaLogFileForPerfMonSummary(logfile,addTransformTimerInfo=False,extractMiscInfo=True,quiet=True)
    if not lfnfo or not lfnfo['misc']['statusok']:
        return badval
    tt_n=lfnfo['misc']['inputevts']
    ttfile=os.path.join(jobdir,'TransformTimer_Reco_trf.pickle.gz')
    ttfile_short=os.path.join(jobdir_short,'TransformTimer_Reco_trf.pickle.gz')
    if (not os.path.exists(ttfile)) or (not os.path.exists(ttfile_short)):
        return badval
    tt_short={}
    tt_long={}
    tt_short=PmbUtils.TransformTimerUtils.getExclusiveInfoFromPickleFile(ttfile_short)
    tt_long=PmbUtils.TransformTimerUtils.getExclusiveInfoFromPickleFile(ttfile)
    outtt_evt={}
    outtt_overhead={}

    step2val_long={}
    step2val_short={}
    def pretty(step):
        if step.startswith('Reco_trf/'): return step[len('Reco_trf/'):]
        else: return step

    for step,stepdata in tt_long['stepinfo'].items():
        if not step in tt_short['stepinfo'].keys():
            if not 'outputDRAW' in step: 
                print "WARNING: Step %s only found in long job. Ignoring."%step
            continue
        s=tt_short['stepinfo'][step]
        (c_long,w_long)=(stepdata['cpuTime'],stepdata['wallTime'])
        (c_short,w_short)=(s['cpuTime'],s['wallTime'])
        step=pretty(step)
        step2val_long[step]=c_long
        step2val_long[step+'/io']=w_long-c_long
        step2val_short[step]=c_short
        step2val_short[step+'/io']=w_short-c_short

    for step,val_long in step2val_long.items():
        val_short=step2val_short[step]
        val_evt=(val_long-val_short)/float(tt_n-tt_nshort)
        val_overhead=val_short-tt_nshort*val_evt
        step=pretty(step)
        outtt_evt[step]=val_evt*1000.0
        outtt_overhead[step]=val_overhead

#    for step,stepdata in tt_long['stepinfo'].items():
#        if not step in tt_short['stepinfo'].keys(): continue
#        val_short=tt_short['stepinfo'][step]['cpuTime']
#        val_long=stepdata['cpuTime']
#        val_evt=(val_long-val_short)/float(tt_n-tt_nshort)
#        val_overhead=val_short-tt_nshort*val_evt
#        outtt_evt[step]=val_evt*1000.0
#        outtt_overhead[step]=val_overhead
    return outtt_evt,outtt_overhead

def analyseBuild(builddir,thedate):
    out={}
    (trf_evt,trf_overhead)=analyseBuildForTransformTimerInfo(builddir)
    if trf_evt and trf_overhead:
        out['trf_cpu_overhead']=trf_overhead
        out['trf_cpu_evt']=trf_evt
    info_rawtoesd=getNumbers(builddir,'details_all_bs',['ntuple_RAWtoESD.perfmon.summary.txt.gz','ntuple_bstoesd*.perfmon.summary.txt.gz'],'CaloCellMaker',300.0)

    info_esdtoaod=getNumbers(builddir,'details_esdtoaod_bs',['ntuple_ESDtoAOD.perfmon.summary.txt.gz','ntuple_esdtoaod*.perfmon.summary.txt.gz'],'CaloCellMakerFromCluster',50.0)
    out['vmem_leak_evt']={}
    if info_rawtoesd:
        #transfer raw->esd numbers directly to
        out['cpu_overhead']=info_rawtoesd['cpu_overhead']
        out['cpu_evt']=info_rawtoesd['cpu_evt']
        out['malloc']=info_rawtoesd['malloc']
        if 'nalloc' in info_rawtoesd.keys(): out['nalloc']=info_rawtoesd['nalloc']
        if 'nallocdiff' in info_rawtoesd.keys(): out['nallocdiff']=info_rawtoesd['nallocdiff']
        if 'nfree' in info_rawtoesd.keys(): out['nfree']=info_rawtoesd['nfree']
        out['vmem']=info_rawtoesd['vmem']
        out['vmem_leak_evt']['rawtoesd_leak']=info_rawtoesd['leak_per_evt']
    if info_esdtoaod:
        for k in ['cpu_overhead','cpu_evt','malloc','nallocdiff','nalloc','nfree']:
            if k in info_esdtoaod.keys() and k in out.keys():#info_rawtoesd
                for comp,val in info_esdtoaod[k].items():
                    out[k]['AOD:'+comp]=val#out->info_rawtoesd
        out['vmem_aod']=info_esdtoaod['vmem']
        out['vmem_leak_evt']['esdtoaod_leak']=info_esdtoaod['leak_per_evt']

    def addTotal(d,key,divfact=1):
        if not key in d.keys(): return
        c2v=d[key]
        tot=0.0
        for c,v in c2v.items():
            tot+=v
        tot/=divfact
        n='total'
        if divfact!=1:
            n+='/%i'%divfact
        c2v[n]=tot
    addTotal(out,'cpu_overhead',10)
    addTotal(out,'cpu_evt',10)
    addTotal(out,'malloc',10)
    addTotal(out,'nalloc',10)
    addTotal(out,'nallocdiff',10)
    addTotal(out,'nfree',10)
    addTotal(out,'vmem',10)
    addTotal(out,'vmem_aod',10)
    addTotal(out,'trf_cpu_overhead',10)
    addTotal(out,'trf_cpu_evt',10)

    #assert 'total/10' in out['cpu_overhead'].keys()

    return out

def setupPlots():
    d=[]
    d+=[ ['trf_cpu_evt','CPU/evt breakdown for full Reco_trf','milliseconds',None] ]
    d+=[ ['cpu_evt','CPU event loop contributions in RAW->ESD->AOD','milliseconds',100] ]
    d+=[ ['vmem','VMem contribution in RAW->ESD jobs','megabytes',20] ]
    d+=[ ['vmem_aod','VMem contribution in ESD->AOD jobs','megabytes',20] ]
    d+=[ ['trf_cpu_overhead','CPU overhead breakdown for full Reco_trf','seconds',None] ]
    d+=[ ['cpu_overhead','CPU overhead contributions in RAW->ESD->AOD','seconds',3] ]
    d+=[ ['malloc','Malloc traffic in event loop of RAW->ESD->AOD','megabytes',10] ]
    d+=[ ['vmem_leak_evt','VMEM growth per evt in RAW->ESD->AOD','megabytes/evt',None] ]
    #d+=[ ['nalloc','Number of allocs per event in RAW->ESD->AOD','millions',0.05] ]
    d+=[ ['nfree','Number freed allocs/evt in RAW->ESD->AOD','millions',0.05] ]
    d+=[ ['nallocdiff','Number unfreed allocs/evt in RAW->ESD->AOD','millions',0.01] ]

    name='recperftests_detpmon'
    version=211
    #version=None
    return (name,version,d)


def createAllPlots(basedir,firstdate=None,lastdate=None,firstdate_zoom=None,daysbackforactivebuild=30):
    def ensuredir(d):
        if not os.path.exists(d):
            os.mkdir(d)

    ensuredir(basedir)
    alltests=[]
    for cfg in PmbUtils.ArchiveUtils.getActiveBuilds(packages=['RecPerfTests'],
                                                     daysback=daysbackforactivebuild,
                                                     firstdate=firstdate,
                                                     lastdate=lastdate):
        #if not 'dev' in cfg: continue#FIXME
        #if not '16.2.X' in cfg: continue#FIXME
        #if not '16.6.X' in cfg: continue#FIXME
        bkey=cfg.replace('/','__')
        outdir=os.path.join(basedir,bkey)
        alltests+=[(cfg,bkey,outdir)]

    for cfg,bkey,outdir in alltests:
        ensuredir(outdir)#fixme: not needed?
        PmbUtils.ArchiveUtils.archivePlots(cfg,setupPlots,analyseBuild,
                                           firstdate=firstdate,lastdate=lastdate,firstdate_zoom=firstdate_zoom,
                                           toStdOut=False,outdir=outdir)

    def reldir(basedir,extendeddir):
        assert extendeddir.startswith(basedir)
        d=extendeddir[len(basedir):]
        while d.startswith('/'): d=d[1:]
        return d

    html=['<html>']
    html+=['<head>','<title>','Archive-based performance trends','</title>','</head>','<body>']
    html+=['<ul>']
    for b,bkey,outdir in alltests:
        html+=['  <li><a href="%s">%s</a></li>'%(reldir(basedir,outdir),b)]
    html+=['</ul>']
    html+=['</body>']
    html+=['</html>']

    file = open(os.path.join(basedir,'index.html'), 'w')
    for outline in html:
        file.write(outline+'\n')
    file.close()

if __name__=='__main__':
    #hardcode options for now:
    targetdir='/afs/cern.ch/atlas/project/pmb/www/arch-mon/perftests'
    daysbackforshorttermplots=31
    daysbackforactivebuild=daysbackforshorttermplots-3
    import datetime
    firstdate=datetime.date(year=2010,month=1,day=1)#the archive started early 2010, so this is "-infinity"
    #FIXME:
    #firstdate=datetime.date(year=2011,month=4,day=1)
    #firstdate=datetime.date(year=2010,month=1,day=15)#the archive started early 2010, so this is "-infinity"
    lastdate=datetime.date(year=2999,month=1,day=1)#the far future, "+infinity"
    firstdate_zoom=datetime.datetime.now().date()+datetime.timedelta(days=-daysbackforshorttermplots)

    import random
    import string
    import shutil

    randstr=''.join(random.choice(string.letters) for i in xrange(10))
    tmpdir=targetdir+'__tmp__'+randstr
    createAllPlots(tmpdir,
                   firstdate=firstdate,
                   lastdate=lastdate,
                   firstdate_zoom=firstdate_zoom,
                   daysbackforactivebuild=daysbackforactivebuild)

    if os.path.exists(targetdir):
        shutil.rmtree(targetdir)
    shutil.move(tmpdir,targetdir)

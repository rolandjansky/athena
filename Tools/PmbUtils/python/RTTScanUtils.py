# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import glob
import datetime
import pickle
import copy
__rttbase='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt'
#__rttbasealt='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt_j_r'
__rtturlbase='http://atlas-pmb.web.cern.ch/atlas-pmb/rtt-mon/rttjob.php'
__lastweek=None

def lastweek():
    #NB: __lastweek instead of lastweek for var name, to avoid
    #troubles when clashing with function name:
    global __lastweek
    if not __lastweek:
        __lastweek=[]
        for deltadays in range(-6,1):
            __lastweek+=[datetime.date.today()+datetime.timedelta(deltadays)]
    return __lastweek

def getCurrentRundirs(packages):
    global __rttbase
    #global __rttbasealt
    bases=[__rttbase+'*']#added wildcard for now
    #if os.path.exists(__rttbasealt):
    #    bases+=[__rttbasealt]
    dirs=[]
    for base in bases:
        for p in packages:
            for rs in glob.glob(os.path.join(base,'rel_*/*/build/*/*/',p,'*/runScript.sh')):
                d=os.path.dirname(rs)
                if os.path.isdir(d) and not d in dirs and not '/mig' in d and not '/hlt' in d and not '/p1hlt' in d and not '-opt/TrigMC' in d and not '-dbg/TrigMC' in d:
                    dirs+=[d]
    dirs_and_tmod=[]
    for d in dirs:
        runscriptfile=os.path.join(d,'runScript.sh')#take from runScript.sh rather than dir, in case of manual manipulations of dir.
        if not os.path.exists(runscriptfile):
            continue
        tmod=os.path.getmtime(runscriptfile)
        dirs_and_tmod += [(d,tmod)]
    return dirs_and_tmod

def getJobDate(approx_date,nightly,info=None):
    #approx_date: A first guess at the date of the nightly (for
    #example from the timestamp of the logfile)
    #nightly: either a number 0,...6 or a string 'rel0',...,'rel6' or 'rel_0',...,'rel_6'
    #
    #If the weekday of the approx_date does not correspond to the
    #nightly, the resulting date is adjusted by as much as 2 days in
    #either direction so that it does.

    #1) Get weekday (monday=0, sunday=6) of nightly:
    if type('')==type(nightly):
        if nightly.startswith('rel_'): nightly=nightly[4:]
        if nightly.startswith('rel'): nightly=nightly[3:]
    nightly=int(nightly)
    assert nightly>=0 and nightly<=6
    weekday=(nightly-1)%7

    #2) If already good, just return:
    if approx_date.weekday()==weekday:
        return approx_date

    #3) A small adjustment is necessary:
    deltadays=weekday-approx_date.weekday()
    if deltadays<-3: deltadays+=7
    if deltadays>3: deltadays-=7
    #print "WARNING: Correcting date with",deltadays
    if info and (not (deltadays>=-2 and deltadays <= 2)):
        print "ERROR: Dates need adjustment with more than 2 days. Info: %s"%info
        return None
    assert deltadays>=-2 and deltadays <= 2
    return approx_date+datetime.timedelta(deltadays)

def analyseRunDir(dir,modtime):
    global __rttbase
    #global __rttbasealt
    #actualbase= __rttbasealt if dir.startswith(__rttbasealt+'/') else __rttbase
    actualbase=__rttbase
    info={}
    #1) Decode job and build info from path:
    
    parts=dir.split('/')[len(actualbase.split('/')):]
    #parts=dir[len(actualbase)+1:].split('/')

    assert parts[0].startswith('rel_')
    info['nightly'] = int(parts[0][4:])

    info['build']=parts[1]
    info['cmtconfig']=parts[3]
    info['cache']=parts[4]
    info['package']=parts[5]
    info['jobname']=parts[6]
    info['logfile']=os.path.join(dir,info['jobname']+'_log')
    info['runscript']=os.path.join(dir,'runScript.sh')

    #2) Look in rttjobinfo_xml for status:
    rttjobinfo_xml=os.path.join(dir,'rttjobinfo.xml')

    isFailure=None#None means unknown

    status='unextracted'
    if not os.path.exists(rttjobinfo_xml):
        status='rttjobinfo.xml missing'
    else:
        def getFieldVals(fields,iter):
            info={}
            for f in fields: info[f]=None
            for l in iter:
                for field in fields:
                    if '<%s>'%field in l:
                        if '</%s>'%field in l:
                            info[field]=l.split('<%s>'%field)[1].split('</%s>'%field)[0].strip()
                        else:
                            info[field]=iter.next().strip()
                        break
            return info
        def xml_iter(fn):
            for l in open(fn):
                for e in l.replace('><','>\n<').split('\n'):
                    yield e
        fv=getFieldVals(['state','timedOut','jobExitCode','postProcessingSuccess'],xml_iter(rttjobinfo_xml))
        if fv['timedOut']=='True': status='timedout'
        elif fv['state']!='done':
            if fv['state']=='queuedInBatch':status='queued'
            elif fv['state']=='runningInBatch':status='running'
            else: status='state: %s'%fv['state']
        else:
            #if fv['jobSuccess']=='True' and fv['jobExitCode']=='0': status='success'
            if fv['jobExitCode']=='0':
                status='success'
                if fv['postProcessingSuccess']!='success': status='post-processing problem'
            else: status='error'

    info['status'] = status

    #Append warnings to status in case of missing logfile or runscript:

    if not os.path.exists(info['logfile']): status = 'Logfile missing (RTT status is "%s")'%status
    elif not os.path.exists(info['runscript']): status = 'runScript.sh missing (RTT status is "%s")'%status

    #3) Find proper date
    info['jobdate']=getJobDate(datetime.date.fromtimestamp(modtime),info['nightly'],info=dir)
    if info['jobdate']==None:
        print "Problems getting job date for",info
        return None

    return info

def loadDictFromFile(filename,silent_on_problems=True):
    d={}
    try:
        f=open(filename,'r')
        d=pickle.load(f)
        f.close()
    except:
        d={}
        if silent_on_problems:
            pass
        else:
            raise
    return d

def saveDictToFile(thedict,filename):
    f=open(filename,'w')
    pickle.dump(thedict,f)
    f.close()

def getRunDirInfo(packages,cachefile=None,timelimit_days=10,silent=False):

    cache={}
    if cachefile:
        cache=loadDictFromFile(cachefile)
        if not silent: print "Loaded cache with %i entries from %s"%(len(cache),cachefile)

    rundirinfo={}
    today=datetime.date.today()

    for key in getCurrentRundirs(packages):
        jobdate=datetime.date.fromtimestamp(key[1])
        #print "GETTING JOBDATE ",key[0],jobdate
        if abs((today-jobdate).days)>timelimit_days+3: continue#will always be too old even with correction for rel_X
        if key in cache.keys():
            info=copy.deepcopy(cache[key])
        else:
            info=analyseRunDir(key[0],key[1])
        if info==None: continue
        if abs((today-info['jobdate']).days)>timelimit_days: continue
        rundirinfo[key]=info

    if cachefile:
        saveDictToFile(rundirinfo,cachefile)

    return rundirinfo

#A few utilities for dealing with the data:
def getURLRunDir(jobinfo):
    return 'rttjob.php?cmtconfig=%s&cache=%s&build=%s&nightly=rel_%i&package=%s&jobname=%s' %\
    (jobinfo['cmtconfig'],jobinfo['cache'],jobinfo['build'],jobinfo['nightly'],jobinfo['package'],jobinfo['jobname'])

def getRundir(jobinfo):
    return os.path.dirname(jobinfo['logfile'])

def indexRunDirInfoByBuildPackageJob(rundirinfo):
    info={}
    for key,jobinfo in rundirinfo.items():
        #Construct keys we want to order by:
        build=os.path.join(jobinfo['build'],jobinfo['cache'],jobinfo['cmtconfig'])#as a string since lists and dicts are unhashable
        package=jobinfo['package']
        job=jobinfo['jobname']
        jobdate=jobinfo['jobdate']
        #build up new ordering:
        if not build in info.keys(): info[build]={}
        if not package in info[build].keys(): info[build][package]={}
        if not job in info[build][package].keys(): info[build][package][job]={}
        assert not jobdate in info[build][package][job].keys()
        info[build][package][job][jobdate]=copy.deepcopy(jobinfo)
    return info


def __getChildData(elem,childname):
    t=elem.findtext(childname)
    return str(t).strip() if t else None

def __parseTestXml(filename):

    #FIXME: Need to make sure we don't parse more than once!!
    def addResult(res,userjobid,rttgroup,type,rttdisplayname,rttdoc):
        if rttgroup==None or rttgroup=='':
            print "ERROR: Could not get RTT group for userJobId",userjobid
            rttgroup='unknown'
        if rttdisplayname==None or rttdisplayname=='':
            rttdisplayname=userjobid
        if userjobid in results.keys():
            print "ERROR: userJobId appears multiple times in file:",userjobid
        res[userjobid]={'rttgroup':rttgroup,'type':type,
                   'rttdisplayname':rttdisplayname,'userjobid':userjobid,'rttdoc':rttdoc,
                   'userjobid_lineno':None,'rttdisplayname_full':rttdisplayname}
        return userjobid

    def addUserJobIdLineNumbers(results,filename):
        f=open(filename,'r')
        lines=f.readlines()
        for dict in results.values():
            userjobid=dict['userjobid']
            if not userjobid: continue
            needle='userJobId="'+userjobid+'"'#could be fooled by spaces
            i=0
            found=False
            for line in lines:
                i+=1
                if needle in line:
                    found=True
                    break
            if found:
                dict['userjobid_lineno']= i
        f.close()

    #Go
    if not filename or not os.path.exists(filename):
        print "WARNING: Test XML file not found:",filename
        return (None,{})

    import xml.etree.cElementTree

    results={}
    userjobid2name={}
    try:
        et=xml.etree.cElementTree.parse(filename)
        NS='{http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt}'
        for e_rtt in et.getiterator(tag=NS+'rtt'):
            for e in e_rtt.getiterator():
                if e.tag==NS+'athena':
                    userjobid=e.get('userJobId')
                    if userjobid==None or userjobid == '':
                        continue
                    rttdisplayname=__getChildData(e,NS+'displayName')
                    rttgroup=__getChildData(e,NS+'group')
                    rttdoc=__getChildData(e,NS+'doc')
                    if rttgroup and rttgroup!='':
                        rttgroup='Athena'+rttgroup
                    name=addResult(results,userjobid,rttgroup,'athena',rttdisplayname,rttdoc)
                    userjobid2name[userjobid]=name
                elif e.tag==NS+'jobTransform':
                    userjobid=e.get('userJobId')
                    if userjobid: userjobid.strip()
                    if userjobid==None or userjobid == '':
                        continue
                    jtname=__getChildData(e,NS+'jobTransformJobName')
                    rttgroup=__getChildData(e,NS+'group')
                    rttdoc=__getChildData(e,NS+'doc')
                    name=addResult(results,userjobid,rttgroup,'jobTransform',jtname,rttdoc)
                    userjobid2name[userjobid]=name
        if results:
            addUserJobIdLineNumbers(results,filename)
    except:
        results=None
        raise
    return (results,userjobid2name)

def getTestXMLInfo(rundirinfo_bybuildpackagejob):
    out={}
    for buildkey,buildinfo in rundirinfo_bybuildpackagejob.items():
        out[buildkey]={}
        for package,pkginfo in buildinfo.items():
            chosen_date=None
            chosen_file=None
            for jobkey,jobinfos in pkginfo.items():
                dates=jobinfos.keys()[:]
                dates.sort()
                dates.reverse()
                for d in dates:
                    if chosen_date and chosen_date>=d: continue
                    jobinfo=jobinfos[d]
                    testxmlfile=os.path.join(getRundir(jobinfo),'%s_TestConfiguration.xml'%package)
                    #Make sure it is not an empty symlink by opening and veryfying the existance of at least one line::
                    ok=False
                    try:
                        open(testxmlfile).readline()
                        ok=True
                    except:
                        pass
                    if ok:
                        chosen_date=d
                        chosen_file=testxmlfile
            out[buildkey][package]=__parseTestXml(chosen_file)
    return out

#def getStandardRunDirInfoWithCache():
#    #Always use this method to access the common pmb cache
#    packages=['RecPerfTests']
#    cachefile='/afs/cern.ch/atlas/project/pmb/rtt-mon/PmbUtils/testcache.dat'
#    timelimit_days=8
#    rundirinfo = getRunDirInfo(packages=packages,
#                               cachefile=cachefile,
#                               timelimit_days=timelimit_days)
#    return indexRunDirInfoByBuildPackageJob(rundirinfo)

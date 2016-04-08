# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)
#  Initial version: August 2009
#  Rewritten to take just the perfmon summary March 2010 (this leaves out the option to get the evt0 contributions).

import PerfMonComps.PMonSD
import PmbUtils.MsgSvcUtils
import datetime

def parseTransformTimerPrintouts(line_iter):
    measurements=['wall-time','cpu-time']
    def strToSeconds(s):
        #Assumes strings like '00 hrs 12 mins 57 secs'
        s=s.strip().split()
        return int(s[0])*3600+int(s[2])*60+int(s[4])
    def getPartName(s):
        return s.split('[')[1].split(']')[0].strip()
    def parseReport(s):
        assert 'report [' in s
        infos=s.split('report [%s]'%getPartName(s))[1]
        d={}
        for info in infos.split(','):
            (info_name,val)=info.split(':')
            info_name=info_name.strip()
            val=val.strip()
            if info_name in measurements:
                v=strToSeconds(val)
                d[info_name]=v
        return d
    def insertResultInTree(tree,path,result):
        d=tree
        for p in path:
            assert not '/' in p
            if not p in d.keys():
                d[p]={}
            d=d[p]
        d['__total__']=result

    hier=[]
    last_stop=None
    tree={}
    for l in line_iter:
        if not 'wall-time' in l: continue
        l=' '.join(l.strip().split())
        if 'start [' in l:
            hier+=[getPartName(l)]
        elif 'stop [' in l:
            last_stop=tuple(hier[:])
            if hier[-1]!=getPartName(l): return None
            hier=hier[0:len(hier)-1]
        elif 'report [' in l:
            assert last_stop and (last_stop[-1]==getPartName(l))
            insertResultInTree(tree,last_stop,parseReport(l))
    return tree

def decode_raw_data_file(fn):
    #todo: support esd input
    import os
    fn=os.path.basename(fn)
    if not fn.startswith('data') or not 'RAW' in fn: return None
    if 'physics_' in fn:
        stream=fn.split('physics_')[1].split('.')[0]
        streamstr='physics_%s'%stream
    else:
        stream,streamstr='express','express_express'
    assert streamstr in fn
    run=int(fn.split('.')[fn.split('.').index(streamstr)-1])
    lb=int(filter(lambda x:x.startswith('_lb'),fn.split('.'))[0][3:])
    return {'stream':stream,'run':run,'lb':lb}

def parseall(filename,quiet=False,extra_files=None,extra_msg_parsing=False):
    return parseAthenaLogFileForPerfMonSummary(filename=filename,addTransformTimerInfo=True,
                                               extractMiscInfo=True,addPMonSD=True,quiet=quiet,
                                               extra_files=extra_files,
                                               extra_msg_parsing=extra_msg_parsing)

def _decode_build_date(relNstr,builddate):
    #TODO: This method almost similar to the one in custom_cron.py. custom_cron.py should use it as well.
    assert relNstr.startswith('rel_') and len(relNstr)==5 and relNstr[-1] in '0123456'
    weekday=int(relNstr[4:])
    weekday=(weekday-1)%7#to get monday=0, sunday=6
    assert weekday>=0 and weekday<=6
    if isinstance(builddate,datetime.datetime):
        builddate=builddate.date()
    elif isinstance(builddate,datetime.date):
        builddate=builddate
    else:
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

__vectorspy_prefix='libvectorspy INFO '
def parse_vectorspy(lines):
    reports=[]
    report=None
    in_report=False
    lprefix=len(__vectorspy_prefix)
    for l in lines:
        if not l.startswith(__vectorspy_prefix): continue
        l=l[lprefix:]
        if not in_report:
            if l.startswith('Report begin'):
                in_report=True
                report={'files':{},'symbs':{},'vects':[]}
        else:
            if l.startswith('Report end'):
                in_report=False
                reports+=[report]
            else:
                if l.startswith('Symb '):
                    symbid,symb=l[5:].split(' : ')
                    report['symbs'][int(symbid)]=symb.strip()
                elif l.startswith('File '):
                    fileid,file=l[5:].split(' : ')
                    report['files'][int(fileid)]=file.strip()
                elif ' vectors holding ' in l:
                    nv,rest=l.split(' vectors holding ')
                    bytes,rest=rest.split(' bytes (')
                    used,rest=rest.split(' used) created in file ')
                    fileid,symbid=rest.split(' symb ')
                    used = int(used) if used!='?' else None
                    report['vects']+=[(int(nv),int(bytes),used,int(fileid),int(symbid))]
    return reports

__stringspy_prefix='libstringspy INFO '
def parse_stringspy(lines):
    reports=[]
    report=None
    in_report=False
    lprefix=len(__stringspy_prefix)
    for l in lines:
        if not l.startswith(__stringspy_prefix): continue
        l=l[lprefix:]
        if not in_report:
            if l.startswith('Report begin'):
                in_report=True
                report={'files':{},'symbs':{},'strings':[]}
        else:
            if l.startswith('Report end'):
                in_report=False
                reports+=[report]
            else:
                if l.startswith('Symb '):
                    symbid,symb=l[5:].split(' : ')
                    report['symbs'][int(symbid)]=symb.strip()
                elif l.startswith('File '):
                    fileid,file=l[5:].split(' : ')
                    report['files'][int(fileid)]=file.strip()
                elif ' string reps holding ' in l:
                    nv,rest=l.split(' string reps holding ')
                    bytes,rest=rest.split(' bytes (')
                    wasted,rest=rest.split(' wasted) in file ')
                    fileid,symbid=rest.split(' symb ')
                    report['strings']+=[(int(nv),int(bytes),float(wasted),int(fileid),int(symbid))]
    return reports

def parseAthenaLogFileForPerfMonSummary(filename,addTransformTimerInfo=False,extractMiscInfo=False,addPMonSD=False,extra_files=None,quiet=False,
                                        extra_msg_parsing=False):
    if not quiet:
        print "parsing",filename

    #Parses the perfmon summary and other info from an athena job

    import os

    if filename.startswith('/castor/'):
        filename='rfio:'+filename
    if filename.startswith('rfio:'):
        filename=filename[5:]
        import PyUtils.rfio
        PyUtils.rfio
        #need to read it all into memory to avoid changing code below...
        fh=PyUtils.rfio.File(filename).readall().split('\n')
        for i in range(len(fh)): fh[i]+='\n'
    else:
        filename=os.path.expanduser(filename)
        if filename.endswith('.gz'): fh=PerfMonComps.PMonSD.gzip_fastopen(filename)
        else: fh=open(filename)

    stage=None
    pmonsummaries=[{}]
    pmonsummaries_index=0
    miscinfo={}

    transformtimerlines=[]
    pmonsd_lines=[]
    ninputevts=-1
    nlines=0
    statusOK=False
    statusBadNoMatterWhat=False
    nwarnings=0
    host=None
    tcmalloc=False
    cpu_model=None
    cpu_cache=None
    atlas_release=None
    vmem_max=0.0
    si2k=None
    platform=None
    architecture=None
    ntracks_per_evt=None
    ntracks_targetcol=('ResolvedTracksTrackCollection','"ResolvedTracks"')
    ntracks_collection_ok=False
    ntracks_nextlineok=False
    files_output={}
    command=None
    command_takenext=False
    build_info_date=None
    build_info_id=None
    msgsvc_abuses_lines=[]
    msgsvc_abuses_enabled=False
    vectorspy_lines=[]
    stringspy_lines=[]
    cmtconfig=None
    extra_msg_info=None
    if extra_msg_parsing:
        extra_msg_info={}
    for line in fh:
        nlines+=1
        if extra_msg_info!=None:
            msgparts=line.split()
            if len(msgparts)>2 and msgparts[1] in ['ALWAYS','VERBOSE','DEBUG','INFO','WARNING','ERROR','FATAL'] and not msgparts[0].endswith('|'):
                msgsrc=(msgparts[0],msgparts[1])
            else:
                msgsrc=(None,None)
            if msgsrc in extra_msg_info: extra_msg_info[msgsrc]+=1
            else: extra_msg_info[msgsrc]=1
        if addTransformTimerInfo and 'wall-time' in line:#tk: fixme, seems a bit open pattern
            transformtimerlines+=[line[:]]
            continue
        if addPMonSD and PerfMonComps.PMonSD.need_line(line):
            pmonsd_lines+=[line[:]]
            continue
        if extractMiscInfo:
            if line.startswith(__vectorspy_prefix):
                vectorspy_lines+=[line]
                continue
            if line.startswith(__stringspy_prefix):
                stringspy_lines+=[line]
                continue
            if not msgsvc_abuses_enabled and 'List' in line and 'Listing sources of Unprotected and Unseen messages' in line:
                msgsvc_abuses_enabled=True
            if msgsvc_abuses_enabled and '|' in line or 'Listing sources' in line or '========' in line:
                msgsvc_abuses_lines+=[line]
            if not build_info_id and line.startswith('Py:Athena') and 'INFO using release' in line:
                build_info_date=line.split('built on [')[1].replace(']','').strip()
                build_info_date=datetime.datetime.strptime(build_info_date,'%Y %m/%d %H:%M').date()
                tmp=[e for e in line.split() if e.startswith('[') and 'rel_' in e]
                tmp=tmp[-1].replace('[','').replace(']','').split('/')
                build_info_id,relnstr=tmp
                build_info_date=_decode_build_date(relnstr,build_info_date)
                continue
            if not command:
                if command_takenext:
                    command=line.strip()
                    if command.startswith('/'):
                        abstrf=command.split()[0]
                        command=command.replace(abstrf,os.path.basename(abstrf))
                    command_takenext=False
                    continue
                if line.startswith('easy_trf.py Launching command:'):
                    command=line.split('Launching command:')[1].strip()
                    continue
                elif line.startswith('Original job transform command:'):
                    command_takenext=True
                    continue

            if line.startswith('Py:') and 'INFO Counted' in line and 'events (' in line and 'MB)' in line:
                line=line.split()
                files_output[line[0][3:]]={'nevts':int(line[line.index('Counted')+1]),
                                'filesize_mb':float(line[line.index('MB)')-1].replace('(',''))}
                continue

            if ntracks_per_evt==None:
                if ntracks_collection_ok:
                    if ntracks_nextlineok:
                        ntracks_nextlineok=False
                        tmp=line.split()
                        if tmp[0]=='total':
                            ntracks_per_evt=float(tmp[1])
                        continue
                    ntracks_nextlineok= 'n/event' in line
                    if ntracks_nextlineok: continue
                if 'Printing Statistics for' in line and ntracks_targetcol[0] in line and ntracks_targetcol[1] in line:
                    tmp=line.split('Printing Statistics for')[1].split()
                    ntracks_collection_ok=(ntracks_targetcol==(tmp[0],tmp[1]))
                    continue

            if 'platform' in line:
                if line.strip().startswith('platform='):
                    platform=line.strip().split('platform=')[1]
                    continue
            if 'architecture' in line:
                if line.strip().startswith('architecture='):
                    architecture=line.strip().split('architecture=')[1]
                    continue
            if 'PerfMonSvc' in line:
                if 'dvmem' in line and 'vmem=' in line:
                    v=float(line.split('vmem=')[1].strip().split()[0])
                    if v and v>vmem_max: vmem_max=v
                if 'VmPeak:' in line:
                    v=float(line.split('VmPeak:')[1].split()[0].strip())/1024.0
                    if v and v>vmem_max: vmem_max=v
                if not si2k and 'Si2k:' in line:
                    si2k=float(line.split('Si2k:')[1].strip().split()[0])
            if line.startswith('ByteStreamInputSvc') and 'total number of Event =' in line:
                n=int(line.split('total number of Event =')[1].strip().split()[0])
                if n>ninputevts: ninputevts=n
                continue
            if line.startswith('AthenaEventLoopMgr') and 'events processed so far' in line:
                n=int(line.split('events processed so far')[0].strip().split()[-1])
                if n>ninputevts: ninputevts=n
                continue
            if not statusOK:
                if line.startswith('ExitCode=0'): statusOK=True
                if line.startswith('athCode=0'): statusOK=True
                if line.startswith('Py:Athena') and 'leaving with code 0' in line:statusOK=True
            if not statusBadNoMatterWhat:
                if line.startswith('stack trace:'): statusBadNoMatterWhat=True
                if line.startswith('Traceback (most recent call last):'): statusBadNoMatterWhat=True
                if line.startswith('Signal handler: Killing'): statusBadNoMatterWhat=True
                if line.startswith('It was not possible to return the standard output of your job'): statusBadNoMatterWhat=True
                if line.startswith('ExitCode=') and not line.startswith('ExitCode=0'): statusBadNoMatterWhat=True
                if line.startswith('ErrorCode=') and not line.startswith('ErrorCode=0'): statusBadNoMatterWhat=True
                if line.startswith('Fatals=') and not line.startswith('Fatals=0'): statusBadNoMatterWhat=True
                if line.startswith('athCode=') and not line.startswith('athCode=0'): statusBadNoMatterWhat=True
                #if line.startswith('athAcronym=') and not line.startswith('athAcronym=successful run'): statusBadNoMatterWhat=True
            if line.startswith('Warnings='): nwarnings+=int(line.split('Warnings=')[1].strip())
            if not host and 'running on' in line:
                host=line.split('running on')[1].strip().split()[0]
            if not tcmalloc and 'Preloading tcmalloc' in line:
                tcmalloc=True
            if not cpu_model and 'Found CPU model:' in line:
                cpu_model=' '.join(line.split('Found CPU model:')[1].strip().split())
            if not cpu_cache and 'Found CPU cache size:' in line:
                cpu_cache=' '.join(line.split('Found CPU cache size:')[1].strip().split())
            if not atlas_release and line.startswith('AtlasRelease='):
                atlas_release=line[13:].strip()


        #the rest is for perfmon summaries:
        if line[0]!='P':#Ignore most lines with minimal overhead (safe since empty lines still have '\n')
            continue
        if not line.startswith("Py:PerfMonSvc"):
            continue

        #The following is a bit fragile between perfmon versions unfortunately...

        # remove line header
        line=line.strip("Py:PerfMonSvc").strip().strip("INFO").strip()

        if line.startswith('Statistics for '):
            stage=line.split('Statistics for ')[1].strip("'").split("'")[0]#fragile...
            if stage in pmonsummaries[pmonsummaries_index].keys():
                #This must be the next perfmon summary in the file
                pmonsummaries_index+=1
                pmonsummaries+=[{}]
            pmonsummaries[pmonsummaries_index][stage]={};
            continue
        if not stage or not line.startswith('<'): continue
        line=line.split()
        if not (line[1]=='(' and line[3]=='+/-' and line[5]==')'):
            continue#avoid for instance PerfMon postprocessing problems
        var=line[0].strip('<>:')
        val=float(line[2])
        err=float(line[4])
        unit=line[6]
        assert not var in pmonsummaries[pmonsummaries_index][stage].keys()
        pmonsummaries[pmonsummaries_index][stage][var]={'unit':unit,'val':val,'err':err}
        continue

    output={'perfmon_summaries':pmonsummaries}
    if addPMonSD:
        output['pmonsd']=PerfMonComps.PMonSD.parse(pmonsd_lines)
    if not cmtconfig and addPMonSD and output['pmonsd']:
        cmtconfig=output['pmonsd'][0]['special']['values']['release'].split('/')
        cmtconfig=cmtconfig[1] if len(cmtconfig)==2 else None
    if addTransformTimerInfo:
        output['transform_timings']=parseTransformTimerPrintouts(transformtimerlines)
    if extractMiscInfo:
        for pmonsum in pmonsummaries:
            for stage,stageinfo in pmonsum.items():
                if 'vmem' in stageinfo.keys():
                    if stageinfo['vmem']['val']>vmem_max: vmem_max=stageinfo['vmem']['val']
        if statusBadNoMatterWhat: statusOK=False
        cpunormid=None
        if platform and architecture and cpu_model and cpu_cache:
            cpunormid='%s:%s:%s:%s'%(platform,architecture,cpu_model,cpu_cache)
            for char in ' ()@':
                cpunormid=cpunormid.replace(char,'_')

        infile=None
        amitag=None
        if command:
            if 'AMITag=' in command: amitag=command.split('AMITag=')[1].split()[0]
            for field in command.split():
                if field.startswith('input') and 'File=' in field:
                    infile=field.split('File=')[1]
                    break
        infile_info=decode_raw_data_file(infile) if infile else None

        if vectorspy_lines: vectorspy=parse_vectorspy(vectorspy_lines)
        else: vectorspy=None
        if stringspy_lines: stringspy=parse_stringspy(stringspy_lines)
        else: stringspy=None

        if msgsvc_abuses_lines: msgsvc_abuses=PmbUtils.MsgSvcUtils.parse(msgsvc_abuses_lines,collapse_multijobs=True)
        else: msgsvc_abuses=None
        if extra_msg_parsing=='info10':
            other_info=0
            for s in extra_msg_info.keys():
                if s[1]=='INFO':
                    c=extra_msg_info[s]
                    if c<10:
                        other_info+=c
                        del extra_msg_info[s]
            if other_info:
                assert not ('other','INFO') in extra_msg_info
                extra_msg_info[('<others>','INFO')]=other_info
        misc={'inputevts':ninputevts,'log_length':nlines,'statusok':statusOK,'warnings':nwarnings,'host':
              host,'tcmalloc':tcmalloc,'cpu_model':cpu_model,'cpu_cache':cpu_cache,
              'atlas_release':atlas_release,'vmem_max':vmem_max,'si2k':si2k,'platform':platform,'arch':architecture,'cpunormid':cpunormid,
              'cmtconfig':cmtconfig,
              'ntracks_per_evt':ntracks_per_evt,
              'ntracks_per_evt_collection':'/'.join(ntracks_targetcol).replace('"',''),
              'outputfiles':files_output,
              'parsed_file':filename,
              'command':command,
              'infile':infile,
              'infile_info':infile_info,
              'amitag':amitag,
              'build_date':build_info_date,
              'build_branch':build_info_id,
              'msgsvc_abuses':msgsvc_abuses,
              'vectorspy':vectorspy,
              'stringspy':stringspy,
              'extra_msg_info':extra_msg_info
              }
        output['misc']=misc
    if extra_files:
        output['extra_files_parsed']={}
        for ef in extra_files:
            fn=os.path.join(os.path.dirname(filename),ef) if not '/' in ef else ef
            if os.path.exists(fn):
                if fn.endswith('.gz'): fh=PerfMonComps.PMonSD.gzip_fastopen(fn)
                else: fh = open(filename)
                content=[line.strip() for line in open(fn)]
            else:
                content=None
            output['extra_files_parsed'][ef]=content

    PerfMonComps.PMonSD.force_share(output)
    return output


def parseAthenaLogFilesForPerfMonSummaries(file2id,addTransformTimerInfo=False,extractMiscInfo=False):
    #We make sure to sort by the id, so as to get similar ordered dictionaries (for diffing, checksums, etc.)
    id2file=dict([(v, k) for (k, v) in file2id.iteritems()])
    assert len(id2file)==len(file2id)
    id2perfmonsummary={}
    for theid,filename in id2file.items():
        print " --> Parsing file:",filename,"("+str(theid)+")"
        id2perfmonsummary[theid]=parseAthenaLogFileForPerfMonSummary(filename,addTransformTimerInfo=addTransformTimerInfo,extractMiscInfo=extractMiscInfo)
    return id2perfmonsummary

def outputId2PerfMonSummaryToFile(id2perfmonsummary,datatype,filename):
    f=open(filename,'w')
    import pickle
    #Header:
    pickle.dump("PMBUtilsParsedLogFilesBegin",f)
    pickle.dump(int(4),f)#version
    pickle.dump(datatype,f)
    #Info:
    pickle.dump(id2perfmonsummary,f)
    #Footer
    pickle.dump("PMBUtilsParsedLogFilesEnd",f)
    f.close()
    print " --> Dumped output in",filename

def createJobsFromFileWithId2PerfMonSummary(filename,label,sortvalue,stage=None):
    specialcpu=(stage=='specialcpu')
    specialvmem=(stage=='specialvmem')
    if stage=='all' or specialcpu or specialvmem:
        stage=None
    #stage==None will result in data from all stages being added, each at separate values.
    #stage=='specialcpu' will scale timings and fin and ini with
    #1/nevts and show other+io domains based on the transform
    #timer. 'specialvmem' will show max vmem.

    #returns (datatype,listofjobs) or None in case of errors.
    f=open(filename,'r')
    import pickle
    if pickle.load(f)!="PMBUtilsParsedLogFilesBegin":
        print "ERROR:",filename,"not in correct format!"
        return None
    v=pickle.load(f)
    if v>=0 and v<=3:
        print "ERROR:",filename,"Obsolete version!"
        return None
    if v!=int(4):
        print "ERROR:",filename,"not in correct format or wrong version!"
        return None
    datatype = pickle.load(f)
    if type(datatype)!=type("") or len(datatype)==0:
        print "ERROR:",filename,"not in correct format!"
        return None

    id2perfmonsummary = pickle.load(f)
    if type(id2perfmonsummary)!=type({}):
        print "ERROR:",filename," not in correct format!"
        return None
    if pickle.load(f)!="PMBUtilsParsedLogFilesEnd":
        print "ERROR:",filename," not in correct format!"
        return None
    f.close()

    from ValuePerDomainCalcUtils import Job,TimeLabel

    #in case we are just looking at one stage:
    timelabel = TimeLabel(label=label,sortvalue=sortvalue+0.0000001*3)
    #for multiple stages:
    stage2timelabel={}
    perfmonsum_index=0#raw->esd
    transformtimer='/BaseOfBasicTrf'
    replaceEvtVmemWithMax=False

    jobs=[]
    for jobid,perfmonsummary in id2perfmonsummary.items():
        transform_timings=None
        nevts=None
        vmem_max=None
        pmonsum=perfmonsummary['perfmon_summaries'][0]#take numbers from first perfmon summary in log
        if not pmonsum: continue
        #print pmonsum
        #print 'test',pmonsum['evt']['vmem']['val']
        if specialvmem:
            vmem_max=perfmonsummary['misc']['vmem_max']
            pmonsum['max']={'vmem':{'val':vmem_max,'unit':'MB'}}
        if specialcpu:
            for tt,v in flattenTransformTimerTree(perfmonsummary['transform_timings']):
                if tt==transformtimer: transform_timings=v
            nevts=perfmonsummary['misc']['inputevts']
            for s in ['ini','fin']:
                if not s in pmonsum.keys():continue
                for var in pmonsum[s].keys():
                    if pmonsum[s][var]['unit'] in ['s','ms','ns']:
                        pmonsum[s][var]['val']*=(1.0/nevts)
            #other_cpu_key='cpu-time'
            if pmonsum:
                other_cpu=transform_timings['cpu-time']*1000.0/nevts-pmonsum['ini']['cpu']['val']-pmonsum['fin']['cpu']['val']-pmonsum['evt']['cpu']['val']
                io=(transform_timings['wall-time']-transform_timings['cpu-time'])*1000.0/nevts
                pmonsum['other']={'cpu':{'val':other_cpu,'unit':'ms'}}
                pmonsum['io']={'cpu':{'val':io,'unit':'ms'}}
            #print pmonsum['ini']['cpu']['val'],pmonsum['fin']['cpu']['val'],pmonsum['evt']['cpu']['val'],transform_timings['cpu-time']*1000.0/nevts
        if replaceEvtVmemWithMax and 'evt' in pmonsum.keys():
            #print "REPLACING",pmonsum['evt']['vmem']['val'],"with",perfmonsummary['misc']['vmem_max']
            pmonsum['evt']['vmem']['val']=perfmonsummary['misc']['vmem_max']
        if stage:
            #only one stage
            meas_name2val={}
            if not stage in pmonsum.keys():
                print "Warning: Did not find perfmon summary data for job '%s' and stage '%s' in file %s"%(jobid,stage,filename)
                continue
            for var,data in pmonsum[stage].items():
                unit = data['unit']
                val = data['val']
                meas_name2val[''+var+':'+unit]=val
            jobs+=[Job(jobid, meas_name2val, time=timelabel)]
        else:
            #all available stages:
            for a_stage in pmonsum.keys():
                t='%s_%s'%(label,a_stage)
                if specialcpu and a_stage!='evt':t+='/nevt'
                if not t in stage2timelabel.keys():
                    nsubsortval=len(stage2timelabel.keys())+3
                    if a_stage=='ini': nsubsortval=0
                    if a_stage=='evt': nsubsortval=1
                    if a_stage=='other': nsubsortval=2
                    if a_stage=='io': nsubsortval=3
                    if a_stage=='max': nsubsortval=4
                    if a_stage=='fin': nsubsortval=5
                    stage2timelabel[t]=TimeLabel(label=t,sortvalue=sortvalue+1000*nsubsortval)
                meas_name2val={}
                for (var,data) in pmonsum[a_stage].items():
                    unit = data['unit']
                    val = data['val']
                    #if a_stage!='evt' and scaleNonEvtTimings!=1.0 and unit in ['ms','ns','s']:
                    #    val=scaleNonEvtTimings*val
                    meas_name2val[''+var+':'+unit]=val
                #print "Adding job",jobid,stage2timelabel[t].getLabel(),stage2timelabel[t].sortval,meas_name2val.keys()
                jobs+=[Job(jobid, meas_name2val, time=stage2timelabel[t])]

    return (datatype,jobs)

def parseDetailedPerfmonSummaryTextFile(fn):
    if fn.endswith('.gz'):
        fh=PerfMonComps.PMonSD.gzip_fastopen(fn)
    else:
        fh=open(fn)
    pslice='- slice] ==='
    pcomps='- comps] ==='
    pvmemfit='vmem fit:'
    stage=None
    stage2data={}
    inIniSlice=False
    vmem_core=None
    vmem_fitgrowth=None
    for l in fh:
        if vmem_fitgrowth==None and l.startswith(pvmemfit):
            l=l.split()
            assert l[-1]=='kB'
            vmem_fitgrowth=float(l[-2])/1024.0
            continue
        if pslice in l:
            inIniSlice=('ini - slice' in l)
            continue
        if inIniSlice:
            if l.startswith('VMem'):
                vmem_core=float(l.split()[2])
            continue
        if pcomps in l:
            stage=l.split(pcomps)[0].split('[')[1].strip()
            stage2data[stage]={}
            continue
        if stage==None:
            continue
        if l.startswith('[cpu/sys]'):
            l=l.split()
            assert len(l)==6 and l[3]=='(ms)' and l[4]=='|'
            compname=l[5]
            if not compname in stage2data[stage].keys():
                stage2data[stage][compname]={}
            stage2data[stage][compname]['cpu']=float(l[1])
        elif l.startswith('dVmem|dMalloc'):
            l=l.split()
            assert len(l)==6 and l[3]=='kB' and l[4]=='|'
            compname=l[5]
            if not compname in stage2data[stage].keys():
                stage2data[stage][compname]={}
            stage2data[stage][compname]['vmem']=float(l[1])
            stage2data[stage][compname]['malloc']=float(l[2])
        elif l.startswith('alloc|free'):
            l=l.split()
            assert len(l)==5 and l[3]=='|'
            compname=l[4]
            if not compname in stage2data[stage].keys():
                stage2data[stage][compname]={}
            stage2data[stage][compname]['nalloc']=int(l[1])
            stage2data[stage][compname]['nfree']=int(l[2])

    #print "TMP extracted",vmem_fitgrowth
    misc={'vmem_core':vmem_core,'vmem_fit_growth':vmem_fitgrowth}
    return (misc,stage2data)

def formatDetailedPerfmonSummaryStage2DataForSpreadSheet(stage2data,stage='evt',onlyalgs=[]):
    first=True
    columns=None
    for comp,data in stage2data[stage].items():
        if first:
            first=False
            columns=data.keys()
            columns.sort()
            print 'Component at %s stage;%s'%(stage,';'.join(columns))
        if onlyalgs and not comp in onlyalgs: continue
        row=[comp]
        for col in columns:
            row+=[str(data[col])]
        print ';'.join(row)


def findMostInterestingAlgsInDetailedPerfmonSummaryStage2Data(stage2data,stages=[],ncut=10):
    keepalgs=[]
    if not stages:
        stages=stage2data.keys()
    for stage in stages:
        var2sortlist={}
        for comp,data in stage2data[stage].items():
            for var,val in data.items():
                if not var in var2sortlist.keys():
                    var2sortlist[var]=[]
                var2sortlist[var]+=[(abs(val),comp)]
        for var,sortlist in var2sortlist.items():
            sortlist.sort()
            first=max(0,len(sortlist)-ncut)
            for val,comp in sortlist[first:]:
                if val and not comp in keepalgs:
                    keepalgs+=[comp]
    return keepalgs

def pruneDetailedPerfmonSummaryStage2Data(stage2data,keepalgs):
    #Only keep the algs in the passed list. Modifies the passed stage2data
    print 'keeping',keepalgs
    stages=stage2data.keys()
    for stage in stages:
        comps=stage2data[stage].keys()
        for comp in comps:
            if not comp in keepalgs:
                del stage2data[stage][comp]

def summariseTimings(tree,
                     separateLog=False,
                     separateHistMerge=False,
                     separateVal=True,
                     separateESDVal=True):
    l=flattenTransformTimerTree(tree)
    measurements=l[0][1].keys()
    summary={}
    parts=['athena_rawtoesd','athena_esdtoaod','athena_esdtodpd','other','total']
    if separateLog: parts+=['log']
    if separateVal: parts+=['val']
    if separateESDVal: parts+=['esdval']
    if separateHistMerge: parts+=['dqhistmerge']
    for n in parts:
        summary[n]={}
        for m in measurements:
            summary[n][m]=0.0
    for n,v in l:
        last=n.split('/')[-1]
        key='other'
        if last.endswith(' validation'):
            if last != 'outputESDFile validation':
                if separateVal:
                    key='val'
            else:
                if separateESDVal: key='esdval'
                elif separateVal: key='val'
        elif separateLog and last.endswith(' log'): key='log'
        elif last=='RAWtoESD': key='athena_rawtoesd'
        elif last=='ESDtoAOD': key='athena_esdtoaod'
        elif last=='ESDtoDPD': key='athena_esdtodpd'
        elif separateHistMerge and last=='DQHistogramMerge': key='dqhistmerge'
        for m in measurements:
            summary[key][m]+=v[m]
            summary['total'][m]+=v[m]
    return summary

def flattenTransformTimerTree(tree):
    #put the output of parseTransformTimerPrintouts in a (maybe) more useful form
    out=[]
    def flatten(a_tree,path,out):
        total=a_tree['__total__']
        measurements=total.keys()
        total_children={}
        for m in measurements: total_children[m]=0.0
        for k,v in a_tree.items():
            if k!='__total__':
                for m in measurements: total_children[m]+=v['__total__'][m]
                flatten(v,'%s/%s'%(path,k),out)
        selfinfo={}
        for m in measurements:
            selfinfo[m]=total[m]-total_children[m]
        out+=[(path,selfinfo)]
    for k,v in tree.items():
        flatten(v,'/%s'%k,out)
    return out

if __name__=='__main__':
    f='mylog.txt'
    p=parseall(f)
    for k,v in p['misc'].items():
        print k,v

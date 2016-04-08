# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import pipes, re, os, sys

def __ami(tag):

    if '_' in tag:
        tag,preferred_trf= tag.split('_',1)
    else:
        preferred_trf=None

    #silence stdout (todo: add quiet=True arg to ArgDicTools)
    dummy=open('/dev/null','w')    
    oldstdout=sys.stdout
    sys.stdout=dummy
    #do stuff
    import PATJobTransforms.ArgDicTools
    tmp=PATJobTransforms.ArgDicTools.BuildDicFromCommandLine(['./GetCommand.py', 'AMI=%s'%tag],returnList=True)
    #reenable stdout
    sys.stdout=oldstdout
    del dummy
    #return as dict or list of dicts (if tag implies consequtive trfs)
    if len(tmp)==1: return tmp[0]['info']
    d=[t['info'] for t in tmp]
    if len(d)!=1:
        if preferred_trf:
            d=[t for t in d if t['amiTransform'].lower().startswith(preferred_trf.lower())]
        if not preferred_trf or len(d)!=1:
            print "ERROR: Tag implies multiple transform steps. Please postfix tag with either '_digi', '_reco' or something else to select e.g. Digi_trf.py or Reco_rtf.py" 
            return None
    assert len(d)==1
    return d[0]

def save_amiinfo_to_pickle(tag,outfile):
    d=__ami(tag)
    import cPickle
    if outfile.endswith('.gz'):
        import gzip
        fh=gzip.open(outfile,'w')
    else:
        fh=open(outfile,'w')
    cPickle.dump(d,fh)
    return d

def get_amiinfo_with_cache(tag,cachefile):
    amiinfo=None
    if os.path.exists(cachefile):
        try:
            if cachefile.endswith('.gz'):
                import PmbUtils.Misc
                fh=PmbUtils.Misc.gzip_fastopen(cachefile)
            else:
                fh=open(cachefile,'r')
            import cPickle
            amiinfo=cPickle.load(fh)
        except Exception,err:
            print "WARNING: Problems while trying to load ami info from %s:"%cachefile,err
            amiinfo=None
            pass
    if not amiinfo:
        try:
            amiinfo=save_amiinfo_to_pickle(tag,cachefile)
        except Exception,err:
            print "WARNING: Could not save ami info in %s:"%cachefile,err
            pass
    return amiinfo

def __guess_output_name(dstype):
    dl=dstype.lower()
    if 'ntup' in dl or 'hist' in dl or 'mon' in dl: return 'my%s.root'%dstype
    if 'draw' in dl: return 'my%s.data'%dstype
    if 'esd' in dl or 'aod' in dl or 'rdo' in dl or 'tag' in dl: return 'my%s.pool.root'%dstype
    assert False

def print_diff(tag1,tag2):
    print "Differences between %s and %s:"%(tag1,tag2)
    d1=__ami(tag1)
    assert d1
    d2=__ami(tag2)
    assert d2

    if 'amiOuputDic' in d1.keys(): outputkey='amiOuputDic' 
    else: outputkey='amiOutputDic'#spelling error

    if not 'amiRelease' in d1: d1['amiRelease']='<not set>'
    if not 'amiRelease' in d2: d2['amiRelease']='<not set>'

    if d1.keys()!=d2.keys() or len(d1.keys())!=5: print "Unexpected difference in keys!"
    if set(d1.keys())!=set(['amiInputDic', 'amiTransform', 'amiPhysDic', 'amiRelease', outputkey]): print "Unexpected keys!"

    def diff(s,v1,v2):
        if v1!=v2: print "Found different %s:\n  %s: %s\n  %s: %s"%(s,tag1,v1,tag2,v2)

    #trf:
    diff("executable",d1['amiTransform'],d2['amiTransform'])

    #input:
    if len(d1['amiInputDic'].keys())!=1: print "Unexpected amiInputDic!"
    diff("input argument",d1['amiInputDic'].keys()[0],d2['amiInputDic'].keys()[0])
    diff("associated release",d1['amiRelease'],d2['amiRelease'])
    #misc:
    def diff_dict(s,dict1,dict2,format=lambda x,y:'%s=%s'%(x,y),ignore=None):
        class Missing: pass
        missing=Missing()
        allkeys=set(dict1.keys()+dict2.keys())
        for k in allkeys:
            if ignore and k in ignore: continue
            v1=dict1.get(k,missing)
            v2=dict2.get(k,missing)
            if v1!=v2:
                if v1!=missing: v1=format(k,v1)
                else: v1=''
                if v2!=missing: v2=format(k,v2)
                else: v2=''
                print "Found different %s:\n  %s: %s\n  %s: %s"%(s,tag1,v1,tag2,v2)

    diff_dict('argument',d1['amiPhysDic'],d2['amiPhysDic'],ignore=['AMITag'])
    diff_dict('output determination',d1[outputkey],d2[outputkey])

def get_info(tag,infile,max_evts=None,amiinfo=None,ami_cachefile=None,extra_preExec=None,extra_postExec=None):

    flags=[]
    extra_preExec=extra_preExec[:] if extra_preExec else []
    extra_postExec=extra_postExec[:] if extra_postExec else []

    if ':' in tag:
        flags=tag.split(':')
        tag=flags.pop(0)
        for f in flags: assert f in ['pmb','pmonsd','pmondet','msgdbg']
        #pmb implies pmonsd + adds ATHENA_PYCINTEX_MINVMEM=180 in front
        if 'pmb' in flags and not 'pmonsd' in flags: flags+=['pmonsd']
        if 'pmonsd' in flags or 'pmondet' in flags:
            extra_preExec+=['from PerfMonComps.PerfMonFlags import jobproperties as pmjp']
            if 'pmondet' in flags:
                extra_preExec+=['rec.doDetailedPerfMon=True',
                                'pmjp.PerfMonFlags.doPostProcessing=True']
            if 'pmonsd' in flags:
                extra_preExec+=['pmjp.PerfMonFlags.doSemiDetailedMonitoringFullPrint=True']
        if 'msgdbg' in flags:
            extra_preExec+=['svcMgr.MessageSvc.countInactive=True']
        special_import_pmon=True

    if not amiinfo and ami_cachefile:
        amiinfo=get_amiinfo_with_cache(tag,ami_cachefile)

    if amiinfo: d=amiinfo
    else: d=__ami(tag)

    assert len(d['amiInputDic'].keys())==1

    l=[]

    for k,v in d['amiPhysDic'].items():
        if max_evts!=None and k=='maxEvents':
            v=str(max_evts)
        if extra_preExec and k=='preExec':
            v=',,'.join([v]+extra_preExec)
            extra_preExec=[]
        if extra_postExec and k=='postExec':
            v=',,'.join([v]+extra_postExec)
            extra_postExec=[]
        l+=[(k,v)]
    if extra_preExec:
        l+=[('preExec',',,'.join(extra_preExec))]
    if extra_postExec:
        l+=[('postExec',',,'.join(extra_postExec))]

    #outputs:
    if 'amiOuputDic' in d.keys(): outputkey='amiOuputDic' 
    else: outputkey='amiOutputDic'#spelling error

    bn=os.path.basename(infile)
    usednames=[]
    outputs=d[outputkey].keys()
    outputs.sort()
    for k in outputs:
        v=d[outputkey][k]
        if 'ifMatch' in v and not re.match(v['ifMatch'],bn): continue
        outname,i=__guess_output_name(v['dstype']),0
        #simple protection against duplicate output names:
        while outname in usednames:
            i+=1
            outname=__guess_output_name(v['dstype']).replace('.','_%i.'%i)
        l+=[(k,outname)]

    #Add input file last:
    l+=[(d['amiInputDic'].keys()[0],infile)]

    #Format:
    l=[d['amiTransform']]+map(lambda x:'%s=%s'%(x[0],pipes.quote(str(x[1]))),l)

    if 'pmb' in flags: l=['ATHENA_PYCINTEX_MINVMEM=180']+l

    return l

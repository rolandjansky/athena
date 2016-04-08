# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Utilities for analysing PMonSD output (from PerfMonComps.PMonSD.parse(..)).
#
# Author: Thomas Kittelmann
# First version: April 2011

def get_step_name(i):
    f=i['fulloutput_file']
    if f in ['ntuple_rdotoesd.pmon.gz','ntuple_bstoesd.pmon.gz']: return 'rawtoesd'
    if f in ['ntuple_esdtoaoddpd.pmon.gz']: return 'esdtodpd'
    if f=='ntuple.pmon.gz': return 'unknown step'
    if f.startswith('ntuple_'): f=f[7:]
    if f.endswith('.pmon.gz'): f=f[:-8]
    return f.lower()

def evtlooptime_breakdown_singlestep_raw(pmonsd_dict,include_wall_time=False,maxvals=False):
    """Take single pmonsd dictionary and breaks down how the time is
    spent in the evt loop. This includes stuff outside alg::execute
    (nonalg) and collapsed entries (other%i), and optionally also
    wall-time. It correctly weighs algs which are not run exactly once
    every event."""
    name_other='other'#ARG
    name_nonalg='nonalg'#ARG
    name_wall='wall'#ARG
    #
    nevt=float(pmonsd_dict['special']['nevts']-1)#-1 since 1st event is in separate step
    if nevt<0.5: return []
    total_alg_cpu=0.0
    out=[]
    steps_comps_evt=pmonsd_dict['steps_comps']['evt']
    for comp,compdata in steps_comps_evt.items():
        nalgs=1
        cpu_mean=compdata['cpu']*(compdata['n']/nevt)
        cpu=cpu_mean#default unless maxvals set
        total_alg_cpu+=cpu
        if maxvals:
            cpu_max=pmonsd_dict['evt_max_info'][comp]['cpu'][0]
            cpu=cpu_max
            if maxvals=='rel':
                if cpu_max<100: cpu=0.0#ignore if <100ms
                else: cpu=cpu_max/cpu
            elif maxvals=='diff':
                cpu=cpu_max-cpu_mean
        assert not comp in [name_other,name_nonalg,name_wall]
        if comp.startswith('[collapsed_'):
            if maxvals: continue
            nalgs=int(comp.split('[collapsed_')[1].split('_')[0])
            assert nalgs>=1
            comp=name_other
        out+=[(cpu,nalgs,comp)]
    if not maxvals:
        overall_evtloop=pmonsd_dict['special']['timings']['evtloop_time']
        out+=[(overall_evtloop['cpu']-total_alg_cpu,1,name_nonalg)]
        if include_wall_time:
            out+=[(overall_evtloop['wall']-overall_evtloop['cpu'],1,name_wall)]
    return out

def evtlooptime_breakdown(pmonsd,name_filters=None,total_format=' [sum of %i algs]',**kw):
    """Combines evtloop breakdown from multiple steps."""
    #if total_format contains %i, it will be replaced with correct count
    #name_filters can be None (=>default filter), a single function
    #(same for all steps), or a list of filters, one for each step.
    #Let the filter return False to ignore entries
    if type(pmonsd)==dict:
        pmonsd=[pmonsd]
    if name_filters==None:
        def default_filter(comp,stepname):
            return '%s:%s'%(comp,stepname)
        name_filters=default_filter
    if type(name_filters)!=list:
        name_filters=[name_filters]*len(pmonsd)
    name_filters=list(name_filters)#to make pylint shut up
    assert len(pmonsd)==len(name_filters)
    #collect and collapse:
    d={}
    for p in pmonsd:
        stepname=get_step_name(p)
        name_filter=name_filters.pop(0)
        for cpu,n,name in evtlooptime_breakdown_singlestep_raw(p,**kw):
            name=name_filter(name,stepname)
            assert name!=None#should return False to ignore, None is an error
            if type(name)==tuple:
                name=name[0]
                n=0
            if name==False: continue#requested to ignore
            if name in d.keys():
                d[name][0]+=cpu
                d[name][1]+=n
            else:
                d[name]=[cpu,n]
    #format for output:
    out=[]
    for name,data in d.items():
        cpu,n=data
        if n>1:
            if '%i' in total_format: t=total_format%n
            else: t=total_format
            name+=t
        out+=[(cpu,name)]
    out.sort()
    return out

def get_total_walltime(pmonsd):
    if isinstance(pmonsd,list):
        return sum(get_total_walltime(p) for p in pmonsd)
    return pmonsd['special']['snapshots']['snapshot_post_fin']['wall']+pmonsd['special']['values']['jobcfg_walltime']

def memcontrib_breakdown(pmonsd,name_filter=None,var='vmem',total_format=' [sum of %i]',other_name='other'):
    assert isinstance(pmonsd,dict)
    if name_filter==None: name_filter=lambda comp,step: '%s/%s'%(comp,step)
    name2vals={}
    ignored_any=False
    for step,comps in pmonsd['steps_comps'].items():
        for name,value in ((name_filter(c,step),vals[var]) for c,vals in comps.items() if vals['n']==1):
            if value==None or name==None:
                ignored_any=True
                continue
            name2vals.setdefault(name,[]).append(value)
    d=dict((n if len(v)==1 else n+(total_format%len(v) if '%i' in total_format else total_format),
            sum(v)) for n,v in name2vals.items())
    if not ignored_any and other_name and var=='vmem':
        assert not other_name in d
        d[other_name]=pmonsd['special']['values']['vmem_peak']-sum(d.values())
    return d

if __name__=='__main__':
    import cPickle
    import AlgDomains
    pmonsd_info=cPickle.load(open('test.psd'))

    def stupid_filter(name,stepname):
        return name[0]+','+stepname[0]

    def pmbdomain_filter(name,stepname):
        d=AlgDomains.filter_alg_to_pmbdomains_rawtoesd(name)
        if d==None: d=name
        return d#+':'+stepname

    multistep=evtlooptime_breakdown(pmonsd_info,pmbdomain_filter)
    for c,n in multistep:
        print '%8.1f %s'%(c,n)

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import PmbUtils.PMonSDUtils as pmonsd_utils
import PmbUtils.MsgSvcUtils as msg_utils
import PmbUtils.AlgDomains as algdomain_utils
import PmbUtils.Misc as misc_utils
import os

quick_test=False

def is_multijobs(info):
    return isinstance(info,dict) and 'multijobs' in info

def job_ok(jobinfo):
    if not jobinfo: return False
    if is_multijobs(jobinfo):
        n_ok=len([1 for l,i in jobinfo['labels_and_jobs'] if job_ok(i)])
        return n_ok and (jobinfo.get('ok_if_any_ok',True) or n_ok==len(jobinfo['labels_and_jobs']))
    if not 'misc' in jobinfo.keys() or not jobinfo['misc']['amitag'] or not jobinfo['misc']['infile_info']: return False
    if not 'pmonsd' in jobinfo.keys() or not jobinfo['pmonsd']: return False
    return jobinfo['misc']['statusok']

def info_by_step(info,gather_fct):
    n2v=[]
    for p in info['pmonsd']:
        sn=pmonsd_utils.get_step_name(p)
        for name,val in gather_fct(p):
            n2v+=[(os.path.join(name,sn),val)]
    return n2v

def iter_jobs(label2info,require_ok=False,reverse=False,return_labels=False):
    for lbl,info in misc_utils.dictiter_bykeys(label2info,reverse=reverse):
        if is_multijobs(info):
            for l,i in info['labels_and_jobs']:
                if not require_ok or job_ok(i): yield ('%s/%s'%(lbl,l),i) if return_labels else i
        elif not require_ok or job_ok(info): yield (lbl,info) if return_labels else info

def get_steps(label2info):
    l=[]
    for i in iter_jobs(label2info,require_ok=True):
        for isn,sn in ((ip,pmonsd_utils.get_step_name(p)) for ip,p in enumerate(i['pmonsd'])):
            if not (isn,sn) in l: l+=[(isn,sn)]
    return l

def get_pmonsd_steps(label2info,istep):
    pmonsd_steps=set()
    for info in iter_jobs(label2info,require_ok=True):
        if istep<len(info['pmonsd']):
            for s in info['pmonsd'][istep]['steps_comps'].keys(): pmonsd_steps.add(s)
    #a bit of sorting in more intuitive order:
    return [e for _,e in sorted(({'ini':1,'cbk':2,'1st':3,'evt':4,'fin':100}.get(s,99),s) for s in pmonsd_steps)]

###################################################
###################################################
###################################################

def define_page_msgdbg(label2info):
    def calc_msg_abuses_shortjob(info):
        t=dict((lbl,job['misc']['msgsvc_abuses']) for lbl,job in info['labels_and_jobs'])
        if 'reco_jettauetmiss_msgabuse_25evts' in t:
            return [(comp+'/'+lvl,count) for count,comp,lvl in t['reco_jettauetmiss_msgabuse_25evts']]
    def calc_msg_abuses_perevt(info):
        t=dict((lbl,job['misc']) for lbl,job in info['labels_and_jobs'])
        info_short=t.get('reco_jettauetmiss_msgabuse_5evts',None)
        info_long=t.get('reco_jettauetmiss_msgabuse_25evts',None)
        if not info_short or not info_long: return None
        l_short,l_long=info_short['msgsvc_abuses'],info_long['msgsvc_abuses']
        n_short,n_long=info_short['inputevts'],info_long['inputevts']
        l_evtloop,l_overhead=msg_utils.split_in_evtloop_and_overhead(n_short,l_short,n_long,l_long,ignore_negative_counts=False)
        ret = [(comp+'/'+lvl,count) for count,comp,lvl in l_evtloop]
        return ret
        if True:#TEMPORARY HACK
            scaled=['ClassIDSvc/VERBOSE']
            return [(('1%% of %s'%n,0.01*c) if n in scaled else (n,c)) for n,c in ret]
        return ret

    plots=[]
    plots+=[{'title' : 'Total MsgSvc abuses during a short job',
             'varcalc' : calc_msg_abuses_shortjob,
             'ylabel' : 'Number of unprotected printouts',
             'total' : ('10% of total',lambda x:x*0.10),
             'nshown':40,
             'doc' : ['The absolute number of unprotected MsgSvc printouts in a 25 evts reco job.',
                      'Given that the job is running over very few events, most contributions shown are likely to be outside the event loop.']
             }]
    plots+=[{'title' : 'Average MsgSvc abuses each event',
             'varcalc' : calc_msg_abuses_perevt,
             'ylabel' : 'Number of unprotected printouts per event',
             'total' : ('10% of total',lambda x:x*0.10),
             'nshown':40,
             'doc' : ['Average number of unprotected MsgSvc printouts per event.',
                      'This is estimated by running two similar jobs with different number of events, subtracting the numbers for each component, and dividing by the difference in number of events.']
             }]

    return {'plots':plots,
            'doc':['Unconditional output of DEBUG or VERBOSE messages represents a drain of CPU resources. More info <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ImprovingSoftware#Avoid_MsgStream_string_operation">here</a>.',
                   'The plots on this page monitors the main offenders in reco jobs, and the idea is of course that anything showing up here should be fixed.',
                   'The most straight-forward way to protect your code is by using the <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/AthenaBaseComps">ATH_MSG_xxx macros</a>.']
            }

###################################################
###################################################
###################################################

def define_page_loglines(label2info):

    #TODO: The next three could go in some utility module:
    def _msginfo_overall_breakdown(parsed_info):
        counts={}
        for s,c in parsed_info['misc']['extra_msg_info'].items():
            counts[s[1]] = counts.get(s[1],0) + c
        if None in counts and 'other' not in counts:
            counts['other']=counts[None]
            del counts[None]
        return counts
    def _msginfo_breakdown_type(parsed_info,msgtype,namefilter=None):
        counts={}
        for s,c in parsed_info['misc']['extra_msg_info'].items():
            if msgtype==None or s[1]==msgtype:
                name=s[0]
                if namefilter:
                    name=namefilter(name)
                counts[name] = counts.get(name,0) + c
        if None in counts and 'other' not in counts:
            counts['other']=counts[None]
            del counts[None]
        return counts
    def _msginfo_namefilter(name):
        if name==None: return 'unknown'
        i=name.find('_')
        if i>0 and i+3<len(name):
            return name[0:i+1]+'...'
        if len(name)<=28: return name
        return name[0:25]+'...'
    def calc_loglines_overall(info):
        return [(name,count) for name,count in _msginfo_overall_breakdown(info).items()]
    def gen_calc_loglines_by_type(msgtype):
        def calc_loglines_overall(info):
            return [(name,count) for name,count in _msginfo_breakdown_type(info,msgtype=msgtype,namefilter=_msginfo_namefilter).items()]
        return calc_loglines_overall
    plots=[]
    plots+=[{'title' : 'Overview of lines printed in logfile',
             'varcalc' : calc_loglines_overall,
             'ylabel' : 'Number of lines',
             'total' : ('total',lambda x:x),
             'nshown':40,
             'doc' : ['Count of the different types of lines printed in the logfile.',
                      'Not all lines are printed in the standard msg svc format and are simply classified as Other.']
             }]

    for msgtype in ['INFO','WARNING']:
        plots+=[{'title' : 'Top sources of %ss printed in logfile'%msgtype,
                 'varcalc' : gen_calc_loglines_by_type(msgtype),
                 'ylabel' : 'Number of %ss'%msgtype,
                 'total' : ('25% of total',lambda x:x*0.25),
                 'nshown':40,
                 'doc' : ['Top sources of %ss printed in the logfile.'%msgtype,
                          'Note that only lines printed in the standard msg svc format are guaranteed to be included.']
                 }]


    plots+=[{'title' : 'Top sources of lines printed in logfile',
             'varcalc' : gen_calc_loglines_by_type(None),
             'ylabel' : 'Number of lines',
             'total' : ('25% of total',lambda x:x*0.25),
             'nshown':40,
             'doc' : ['Top contributions to lines printed in the logfile, irrespective of source.',
                      'Note that only lines printed in the standard msg svc format are guaranteed to be included.']
             }]

    return {'plots':plots,
             'doc':['The plots on this page monitors the various lines printed in the log-files.',
                    'In general one should avoid 100s of INFO messages, and any VERBOSE, DEBUG, WARNING, ERROR and FATAL is in principle something to address (some WARNINGs can be tolerated in special circumstances).']
             }

###################################################
###################################################
###################################################

def define_page_evtcpu(label2info):
    def raw_alg_filter(comp,step): return comp+{'rawtoesd':'','esdtoaod':'/e2a','esdtodpd':'/e2d'}.get(step,'/'+step)
    def overall_domain_filter(comp,step):
        if comp=='nonalg': return ('Outside alg execute',)#tuple means don't count
        return (algdomain_utils.filter_alg_to_public_plot_domain(comp),)
    def gen_varcalc(the_filter,maxvals=False):
        def the_varcalc(info):
            return [(v,k) for k,v in pmonsd_utils.evtlooptime_breakdown(info['pmonsd'],the_filter,maxvals=maxvals)]
        return the_varcalc

    doc_common='CPU consumption of algorithm execute methods (including contributions from algs in all Athena steps).'
    ylabel='milliseconds per event'
    plots=[]
    plots+=[{'title' : 'Domain breakdown',
             'varcalc' : gen_varcalc(overall_domain_filter),
             'ylabel' : ylabel,
             'total' : ('25% of total',lambda x:x*0.25),
             'doc' : [doc_common,'Here summed up by domain for a broad overview.']
             }]
    for dom in algdomain_utils.domains_for_plots:
        plots+=[{'title' : dom+' algs',
                 'varcalc' : gen_varcalc(algdomain_utils.generate_public_plot_filter(dom)),
                 'ylabel' : ylabel,
                 'ycut' : 5.0,
                 'nshown' : 15,
                 'otherstr':'Other %i algs',
                 'doc' : [doc_common,'Here just showing the algorithms in the %s domain.'%dom]
                 }]
        if quick_test: break
    plots+=[{'title':'Top algs',
             'varcalc': gen_varcalc(raw_alg_filter),
             'ylabel' : ylabel,
             'nshown' : 20,
             'otherstr' : None,
             'doc' : [doc_common,'Here just showing the algs with highest contributions, irrespective of domain.']
             }]
    #plots+=[{'title':'Top algs in single events',
    #         'varcalc': gen_varcalc(raw_alg_filter,maxvals=True),
    #         'ylabel' : ylabel,
    #         'nshown' : 15,
    #         'otherstr' : None,
    #         'doc' : [doc_common,'Here we are showing not the average consumption, but the highest consumption of each alg in any given event.'+
    #                  ' This should help pinpoint algorithms whose high average cpu consumption is due to bad scaling behaviour on a few busy events.']
    #         }]
    #plots+=[{'title':'Top algs in single/mean',
    #         'varcalc': gen_varcalc(raw_alg_filter,maxvals='rel'),
    #         'ylabel' : 'Max contribution / mean contribution',
    #         'nshown' : 15,
    #         'otherstr' : None,
    #         'doc' : [doc_common,'Here we are showing not the average consumption, but the highest consumption of each alg in any given event divided by the average.'+
    #                  ' This should help pinpoint algorithms whose high average cpu consumption is due to bad scaling behaviour on a few busy events.']
    #         }]
    plots+=[{'title':'Top algs outlier excess',
             'varcalc': gen_varcalc(raw_alg_filter,maxvals='diff'),
             'ylabel' : 'Max contribution minus mean contribution [ms]',
             'nshown' : 20,
             'otherstr' : None,
             'doc' : [doc_common,'Here we are showing not the average consumption, but the highest consumption of each alg in any given event minus the mean contribution.'+
                      ' This should help pinpoint algorithms whose high average cpu consumption is due to bad scaling behaviour on a few busy events.']
             }]
    return {'plots':plots,
            'doc':['This page has plots detailing where the time is spent in the Athena event loops.'
            +' By roughly assigning the algorithms into a few domains, results are shown both overall by domain as well as '
            +' a more detailed view within a given domain.',
                   'For a look at wall-time and the time spent outside the event loop, have a look at the <em>timings</em> pages.'],
            'title':'Event Loop CPU'
            }


def __gen_page_mem_or_malloc_detail(thevar):
    def define_page_detail(label2info):
        def gen_calc_details(istep,pmonsd_step):
            def detail_name_filter(comp,step):
                if step!=pmonsd_step: return None
                if '[/' in comp and comp.endswith(']'):
                    #CondAttrListVec[/SCT/DAQ/Configuration/Module]
                    a,b=comp[0:-1].split('[/')
                    comp='%s[/%s/..]'%(a,b.split('/')[0])
                elif '[0x' in comp and '+' in comp:
                    comp=comp.split('[0x')[0]
                if comp.startswith('/afs'):
                    comp=os.path.basename(comp)
                return comp
            def calc_details(info):
                return [(k,v/1024.0) for k,v in pmonsd_utils.memcontrib_breakdown(info['pmonsd'][istep],
                                                                                  name_filter=detail_name_filter,
                                                                                  var=thevar).items()]
            return calc_details
        plots=[]
        for istep,step in get_steps(label2info):
            for pmonsd_step in get_pmonsd_steps(label2info,istep):
                if pmonsd_step=='evt':continue
                cap=thevar.upper() if thevar=='vmem' else thevar.capitalize()
                plots+=[{'title' : '%s in %s/%s'%(cap,step,pmonsd_step),
                         'varcalc' : gen_calc_details(istep,pmonsd_step),
                         'ylabel' : 'megabytes',
                         'nshown':15,
                         'doc' : ['%s contributions during %s in the %s Athena job'%(cap,{'dso':'library loading','cbk':'callbacks',
                                                                                          '1st':'the first event'}.get(step,'the %s stage'%step),
                                                                                     pmonsd_step)]
                         }]
                if quick_test: break
        return {'plots':plots,
                'doc':['This page contains a large number of plots of memory consumption within the various stages of the various Athena jobs',
                       'You probably want to look first at the <em>memory</em> pages and then refer back here for more details if needed.']}
    return define_page_detail

def define_page_memdetail(label2info):
    return __gen_page_mem_or_malloc_detail('vmem')(label2info)

def define_page_mallocdetail(label2info):
    return __gen_page_mem_or_malloc_detail('malloc')(label2info)

def define_page_memory(label2info):
    plots=[]
    plots+=[{'title':'Memory consumption',
             'name':'stepmem',
             'varcalc_bystep':lambda p:((var,p['special']['values'][var]/(1024.*1024)) for var in ['vmem_peak','vmem_mean','rss_mean']),
             'ylabel':'gigabytes',
             'doc' : ['An overall view of memory consumption in the various Athena steps based on vmem_peak, vmem_mean, and rss_mean numbers reported in the special section of the PerfMonSD output.',
                      'vmem_peak is the maximal vmem value attained at any point during the job, while vmem_mean and rss_mean are the average of values sampled once per event throughout the event loop of the jobs (rss is physical memory used and vmem is virtual address space consumed).']
             }]
    plots+=[{'title':'Memory leaks',
             'name':'memleaks',
             'varcalc_bystep':lambda p:((var,p['special']['leaks']['leakperevt_evt51plus'][var]) for var in ['vmem','malloc']),
             'ylabel':'kilobytes per event',
             'doc' : ['Memory growth/evt estimation based on the leakperevt_evt51plus numbers reported in the special section of the PerfMonSD output, either based on vmem or malloc (malloc is more stable and reproducible but also less inclusive).',
                      'In an ideal world, all values would here be zero. When they are not it represents leaks in one sense of the other: Either someone is new\'ing some memory each event without subsequent deletion, or some caches grow and grow throughout the job.']

             }]
    for istep,step in get_steps(label2info):
        def mem_breakdown(p):
            n2v=[]
            for name,totals in p['steps_totals'].items():
                for totalname,totaldata in totals.items():
                    if totaldata['n']==1:
                        v=totaldata['vmem']
                        n2v+=[(name,v)]
                        break
            snaps=p['special']['snapshots']
            n2v+=[('core',snaps['snapshot_pre_ini']['vmem'])]
            n2v+=[('evt',snaps['snapshot_post_lastevt']['vmem']-snaps['snapshot_post_1stevt']['vmem'])]
            n2v+=[('other',p['special']['values']['vmem_peak']-sum(v for n,v in n2v))]
            return [(n,v/1024.0) for n,v in n2v]
        def gen_membreakdown_calc(istep): return lambda info:mem_breakdown(info['pmonsd'][istep]) if istep<len(info['pmonsd']) else None
        plots+=[{'title':'Memory in %s'%step,
                 'name':'membreakdown_%i_%s'%(istep,step),
                 'varcalc': gen_membreakdown_calc(istep),
                 'ylabel':'megabytes',
                 'total' : ('25% of total',lambda x:x*0.25),
                 'doc' : ['VMEM consumption in the %s Athena step of the transform, based solely on PerfMonSD output.'%step,
                          'Specifically shown are the total value of each step (dso, ini, 1st, cbk, ...) for components with n=1 (i.e. happening once in a job such as MyAlg::initialize but not MyAlg::execute to take an example). To get the rest of the contributions, the snapshot_pre_ini, snapshot_post_1stevt and snapshot_post_lastevt vmem numbers from the special section are used to add core and evt entries. Finally, the vmem_peak value is used to catch everything left out in an entry named other.']
             }]
    return {'plots':plots,
            'doc':['This page starts with an overview of both physical and virtual memory consumption in the various Athena steps of reco jobs.',
                   'Next are plots showing the level of memory leaks and overall breakdown in the steps.',
                   'For further breakdown of the memory consumption, refer to the <em>memdetail</em> pages.']}

def define_page_timings(label2info):
    plots=[]
    def pmonsd_overheads(p):
        s=p['special']
        l=[('jobcfg_wall',s['values']['jobcfg_walltime'])]
        l+=[('overhead_cpu',s['timings']['overhead_time']['cpu'])]
        l+=[('overhead_wall',s['timings']['overhead_time']['wall']-s['timings']['overhead_time']['cpu'])]
        return ((var,val*0.001) for var,val in l)
    def pmonsd_evtloop(p):
        s=p['special']
        l=[('evtloop_cpu',s['timings']['evtloop_time']['cpu'])]
        l+=[('evtloop_wall',s['timings']['evtloop_time']['wall']-s['timings']['evtloop_time']['cpu'])]
        return ((var,val*0.001) for var,val in l)
    def overheads_with_outside_pmonsd(info):
        outside=info['transform_timings'].values()[0]['__total__']['wall-time']-0.001*pmonsd_utils.get_total_walltime(info['pmonsd'])
        athena_overheads=info_by_step(info,pmonsd_overheads)
        return athena_overheads+[('outside_pmonsd',outside)]
    def transform_timer_wall_breakdown(info):
        import PmbUtils.LogFileParsingUtils as lfp
        return [('trf timer/'+os.path.basename(k).lower(),v['wall-time']) for k,v in lfp.flattenTransformTimerTree(info['transform_timings'])]
    def pmonsd_vs_trftimer(info):
        pmonsd_totals=info_by_step(info,lambda p:[('pmonsd',0.001*pmonsd_utils.get_total_walltime(p))])
        trf_totals=transform_timer_wall_breakdown(info)
        return trf_totals+pmonsd_totals+[('trf timer - pmonsd',sum(v for k,v in trf_totals)-sum(v for k,v in pmonsd_totals))]
    plots+=[{'title':'Event loop time',
             'varcalc_bystep':pmonsd_evtloop,
             'ylabel':'seconds',
             'total' : ('total',lambda x:x),
             'doc' : ['The CPU and Wall time consumption of the event loops of the Athena steps in the transform.',
                      'The numbers are based on the cpu and wall columns of the evtloop_time reported in the special section of the PerfMonSD output.']
             }]
    plots+=[{'title':'Overhead timings',
             'varcalc':overheads_with_outside_pmonsd,
             'ylabel':'seconds',
             'total' : ('10% of total',lambda x:x*0.1),
             'doc' : ['The CPU and Wall time consumption outside the event loops of the Athena steps in the transform.',
                      'The numbers are based on the cpu and wall columns of the overhead_time as well as the jobcfg_walltime number, all reported in the special section of the PerfMonSD output.']
             }]
    #should be on debug page?:
    plots+=[{'title':'PMonSD vs TransformTimer',
             'varcalc':pmonsd_vs_trftimer,
             'ylabel':'seconds',
             'doc' : ['Comparison of timing numbers from PerfMonSD and the TransformTimer which should be more inclusive but not always available.',
                      'This is mostly an internal plot for PMB tool developers used to see how much of the whole picture is left out by only considering info from PerfMonSD running inside the Athena steps rather than also the TransformTimer.']
             }]
    return {'plots':plots,
            'doc':['This page shows plots with cpu and wall-time inside and outside the Athena event loops.',
                   'For a more detailed view of algorithms inside the event loops refer to the <em>evtcpu</em> pages.']}

def define_page_overview(label2info):
    plots=[]
    def gen_overview_timings(is_overhead):
        def overview_timings(info):
            l=[]
            for lbl,jobinfo in info['labels_and_jobs']:
                if not job_ok(jobinfo): continue
                if is_overhead:
                    #remove the worst spurious outliers
                    overheadcpu=sum(p['special']['timings']['overhead_time']['cpu'] for p in jobinfo['pmonsd'])
                    if not overheadcpu: continue
                    overheadwall=sum(p['special']['timings']['overhead_time']['wall'] for p in jobinfo['pmonsd'])
                    if overheadwall>overheadcpu*1.2: continue
                evtcpu_s=0.001*sum(p['special']['timings']['evtloop_time']['cpu'] for p in jobinfo['pmonsd'])
                if not is_overhead:
                    l+=[(lbl,evtcpu_s)]
                else:
                    nevts=set(p['special']['nevts'] for p in jobinfo['pmonsd'])
                    if not len(nevts)==1:
                        print "WARNING: Supposedly OK job had different nevts in different stages."
                        continue
                    nevts=nevts.pop()
                    totalwall_s=jobinfo['transform_timings'].values()[0]['__total__']['wall-time']
                    l+=[(lbl,(totalwall_s-nevts*evtcpu_s)/60.0)]
            return l
        return overview_timings
    plots+=[{'title':'Overview of event-loop CPU consumption',
             'varcalc':gen_overview_timings(is_overhead=False),
             'ylabel':'seconds',
             'doc' : ['Global overview of reco cpu time consumption in the Athena event loops in selected streams and builds.']
             }]
    for memvar,descr in [('vmem_peak','virtual'),('rss_mean','physical')]:
        def gen_varcalc(thevar):
            def overview_mem(info):
                l=[]
                for lbl,jobinfo in info['labels_and_jobs']:
                    if not job_ok(jobinfo): continue
                    for p in jobinfo['pmonsd']:
                        sn=pmonsd_utils.get_step_name(p)
                        l+=[('%s_%s'%(sn,lbl),p['special']['values'][thevar]/1024.0/1024.0)]
                return l
            return overview_mem
        plots+=[{'title':'Overview of %s memory usage'%descr,
                 'varcalc':gen_varcalc(memvar),
                 'ylabel':'gigabytes',
                 'doc' : ['Global overview of reco %s memory consumption in selected streams and builds.'%descr,
                          'Source is the %s value from the special section of PerfMonSD output'%memvar]
                 }]
    if False:#too many fluctuations
        plots+=[{'title':'Overview of job overhead',
                 'varcalc':gen_overview_timings(is_overhead=True),
                 'ylabel':'minutes',
                 'doc' : ['Global overview of reco time consumption outside the Athena event loops in selected streams and builds.',
                          'Note that this includes wall-time, and the graphs can be expected to fluctuate somewhat. '
                          +'For this reason, jobs where the PMonSD reported overhead numbers have wall time more than 20% '
                          +'larger than CPU time are ignored.']
             }]
    max_wall2cpu_percent=20
    athenacpu_offset=5
    def calc_overview_overhead(info):
        l=[]
        for lbl,jobinfo in info['labels_and_jobs']:
            if not job_ok(jobinfo): continue
            overheadcpu_s=0.001*sum(p['special']['timings']['overhead_time']['cpu'] for p in jobinfo['pmonsd'])
            overheadwall_s=0.001*sum(p['special']['timings']['overhead_time']['wall'] for p in jobinfo['pmonsd'])
            totevtwall_s=0.001*sum(p['special']['nevts']*p['special']['timings']['evtloop_time']['wall'] for p in jobinfo['pmonsd'])
            jobcfg_s=0.001*sum(p['special']['values']['jobcfg_walltime']  for p in jobinfo['pmonsd'])
            totalwall_s=jobinfo['transform_timings'].values()[0]['__total__']['wall-time']
            l+=[('%i+%s_AthenaCPU'%(athenacpu_offset,lbl),athenacpu_offset+overheadcpu_s/60.0)]
            if overheadcpu_s*(1.0+max_wall2cpu_percent*0.01)>overheadwall_s:
                l+=[(lbl+'_AthenaWall',(overheadwall_s-overheadcpu_s)/60.0)]
                l+=[(lbl+'_AthenaPyCfg',jobcfg_s/60.0)]
                l+=[(lbl+'_TrfOther',(totalwall_s-overheadwall_s-jobcfg_s-totevtwall_s)/60.0)]
        return l
    plots+=[{'title':'Job overhead',
             'varcalc':calc_overview_overhead,
             'ylabel':'minutes',
             'doc' : ['Time spent outside the the Athena event loops:',
                      'AthenaCPU: Means the CPU time spent in Athena after the python configuration stage (mainly during ini, fin and delayed ini in the first event). Note that it is shifted by +%i minutes.'%athenacpu_offset,
                      'AthenaWall: The additional wall-time spent for the same tasks. It is a rough measure of I/O wait such as during database lookups.',
                      'AthenaPyCfg: The (wall) time spent during the python configuration stages of the Athena jobs.',
                      'TrfOther: All other time spent by the transform, including sub-tasks such as log-file parsing and DQ histogram merging.',
                      'NB: To reduce spurious spikes, only AthenaCPU is shown when AthenaWall is more than %i%% of AthenaCPU.'%max_wall2cpu_percent]
             }]
    return {'plots':plots,
            'doc':['Welcome to the PMB performance monitoring pages where various reconstruction computing performance metrics'
                   +' are plotted.',
                   'The present page contains a few plots providing a very high-level overview of the current CPU and memory'
                   +' performance in both the Tier0 and the main development branch of ATLAS offline software.'
                   +' Please use the menu at the top of the page to navigate to more detailed information.',
                   'For maximum stability and reproducibility the relevant jobs are not run in RTT, but rather via custom'
                   +' scripts in cron on the same machines each day. Most data is collected via <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD">PerfMonSD</a>, the documentation of which it is recommended to read. Note that only data from successful jobs are used for the plots, and that not all builds are displayed on this frontpage. For information about other builds (such as validation builds) or failed jobs, you must navigate to other builds via the menus or you must investigate job information at the bottom of each page, which includes log-files of failed jobs.'],
            'title':''}

def define_page_filesizes(label2info):
    def filesizes(info):
        fact=1024.0/info['misc']['inputevts']
        return [(s.split('.output')[1].replace('File',''),d['filesize_mb']*fact) for s,d in info['misc']['outputfiles'].items() if '.output' in s]
    plots=[]
    plots+=[{'title':'Output file sizes per input event',
             'varcalc':filesizes,
             'ylabel':'kilobytes / input event',
             'doc' : ['Size of output files divided by number of input events (thus properly taking into account the fact that not all events contribute to all output files)']
             }]
    return {'plots':plots,
            'doc':'Page showing size of output files normalised to number of events coming in.'}

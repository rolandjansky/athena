# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Script for running over the custom archive and producing various plots.
#
# This is still very much in development.
#
# Author: Thomas Kittelmann
# First version: April 2011

#########################################################################################

import sys,time

daysback=25
daysback_longterm=100
nsubmax='auto'
do_longterm=True

quick_test=False
if 'quick' in sys.argv:
    sys.argv.remove('quick')
    quick_test=True#global switch to make development a bit less painful
    do_longterm=False

in_cron=False
if 'incron' in sys.argv:
    sys.argv.remove('incron')
    in_cron=True#global switch to make development a bit less painful

verbose=False
if 'verbose' in sys.argv:
    sys.argv.remove('verbose')
    verbose=True

if 'nomp' in sys.argv:
    sys.argv.remove('nomp')
    nsubmax=1

nsubmax=2#FIXME
img_format='png'
alt_formats=[('svg','SVG'),('pdf','PDF')]#,('transp:png','transparent PNG')]#fixme: can't do transparent version now due to weird bug...
#,('eps','EPS'),
streams=[]
streams+=[('JetTauEtmiss','Jet')]
streams+=[('Muons','Mu')]
streams+=[('Egamma','Eg')]

comparison_builds={'17.2.X.Y/i686-slc5-gcc43-opt':('T32','32 bit 17.2.X.Y'),
                   '17.2.X.Y/x86_64-slc5-gcc43-opt':('T64','64 bit 17.2.X.Y'),
                   '17.1.X.Y/i686-slc5-gcc43-opt':('O32','32 bit 17.1.X.Y'),
                   'dev/i686-slc5-gcc43-opt':('dev','32 bit dev branch')}

msg_builds=['dev/i686-slc5-gcc43-dbg','devval/x86_64-slc5-gcc43-dbg',
            'devval/i686-slc5-gcc43-dbg','dev/x86_64-slc5-gcc43-dbg']
msg_jobs=['reco_jettauetmiss_msgabuse_5evts','reco_jettauetmiss_msgabuse_25evts']

if quick_test:
    daysback=7
    alt_formats=[]
    streams=streams[0:1]
    del comparison_builds['17.2.X.Y/x86_64-slc5-gcc43-opt']
stream_to_short=dict(e for e in streams)

#########################################################################################

import PmbUtils.CustomJobsArchiveUtils as arch_utils
import PmbUtils.PMonSDUtils as pmonsd_utils
import PmbUtils.Misc as misc_utils
import PmbUtils.Plot as plot_utils
import PmbUtils.WebPageDefinitions as pagedefs
import PmbUtils.TagInfo as taginfo
import sys,os,datetime,copy
pagedefs.quick_test=quick_test

#not really sure where the best location for the these job functions would be...
job_ok=pagedefs.job_ok
is_multijobs=pagedefs.is_multijobs
info_by_step=pagedefs.info_by_step
iter_jobs=pagedefs.iter_jobs

assert len(sys.argv)==2,"please supply empty dir as argument"
outdirbase=sys.argv[1]
assert os.path.isdir(outdirbase),"please supply empty dir as argument"
assert not os.listdir(outdirbase),"dir must be empty"

__buildidx=None
def _actual_get_tagdiff(date1,buildid1,date2,buildid2):
    out=['Finding tag differences for %s:%s and %s:%s:'%(buildid1,date1,buildid2,date2)]
    global __buildidx
    if __buildidx==None: __buildidx=taginfo.getOverallBuildIndex(os.path.join(taginfo.indexbase,'index.txt'))
    tags1=taginfo.getTags(__buildidx,buildid1,date1,silent_if_unknown_build=True)
    if tags1==None:
        out+=['ERROR: Could not find tags for build %s on date %s'%(buildid1,date1)]
        return (-1,out)
    tags2=taginfo.getTags(__buildidx,buildid2,date2,silent_if_unknown_build=True)
    if tags2==None:
        out+=['ERROR: Could not find tags for build %s on date %s'%(buildid2,date2)]
        return (-1,out)
    diffs,onlyin1,onlyin2=taginfo.diffTags(tags1,tags2)
    if onlyin1:
        out+=["  Packages only in %s:%s:"%(buildid1,date1)]
        for p,t in onlyin1: out+=["    %s"%taginfo.formatPkgTag(p,t)]
    if onlyin2:
        out+=["  Packages only in %s:%s:"%(buildid2,date2)]
        for p,t in onlyin2: out+=["    %s"%taginfo.formatPkgTag(p,t)]
    if diffs:
        out+=["  Packages with different tags:"]
        for p,t1,t2 in diffs: out+=["    %s"%taginfo.formatPkgTag(p,t1,t2)]
    return (len(onlyin1)+len(onlyin2)+len(diffs),out)

__tagcache={}
def get_tagdiff(date1,buildid1,date2,buildid2):
    global __tagcache
    callsig=(date1,buildid1,date2,buildid2)
    if not callsig in __tagcache:
        __tagcache[callsig]=_actual_get_tagdiff(date1=date1,buildid1=buildid1,date2=date2,buildid2=buildid2)
    return __tagcache[callsig]

def sample_job(jobinfo):
    #takes first ok job (or one of the failed ones if there are no
    #good ones) in a multi job, or just the unchanged argument for a
    #single job.
    assert type(jobinfo)!=list
    if not jobinfo or not 'multijobs' in jobinfo: return jobinfo
    last=None
    for l,i in jobinfo['labels_and_jobs']:
        last=i
        if job_ok(i):
            break
    return last

def logs_to_html_list(label2info,dirname):
    l=[]
    l+=['  <p style="padding-top:100px"/><a name="jobinfo"><strong>Details of concerned jobs:</strong></a>']
    table_header1='  <table style="font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:left;white-space:nowrap;">'
    table_header2='    <tr><th style="text-align:center;background-color:rgb(200,200,255);" colspan="%i">%s</th></tr>'
    l+=[table_header1]
    l+=[table_header2%(6,'Job information')]
    l+=['    <tr style="background-color:rgb(215,215,255);"><th>Job</th><th>status</th><th>archive</th><th>logfile</th><th>PerfMonSD output</th><th>Command to reproduce</th></tr>']
    cmdfiles=set()
    setup_str='source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh'
    build_ids=[]
    build_ids_used=set()
    for lbl,info in iter_jobs(label2info,return_labels=True,reverse=True):
        if not info or not 'misc' in info: continue
        cmd=info['misc'].get('command',None)
        if not cmd: cmd='<command could not be extracted>\n'
        command_sh=info.get('extra_files_parsed',{}).get('__command.sh',None)
        build_id=(info['misc']['build_date'],info['misc']['build_branch'])
        if not None in build_id and not build_id in build_ids_used:
            build_ids_used.add(build_id)
            build_ids+=[build_id]
        setup_line=None
        infile_fixed=False
        if command_sh:
            for e in command_sh:
                if setup_str in e:
                    setup_line=e[:e.index(setup_str)]+e[e.index(setup_str):].split('&&')[0]
                if ') ' in e and ' => ' in e:
                    e=e.split()
                    afsloc,local=e[e.index('=>')-1],e[e.index('=>')+1]
                    if 'File='+local in cmd:
                        cmd=cmd.replace(local,afsloc)
                        infile_fixed=True

        ami=info['misc'].get('amitag','')

        if not setup_line or not infile_fixed:
            cmd='<command could not be extracted (2)>\n'
        else:
            cmd='\n'.join(['#!/bin/bash','#Command based on AMI tag %s'%ami if ami else '',setup_line,cmd,''])

        cmdid=hex(hash(cmd))[2:]
        if ami: cmdid='ami%s_%s'%(ami,cmdid)
        cmdfile='command_%s.txt'%cmdid
        if not cmdfile in cmdfiles:
            cmdfiles.add(cmdfile)
            open(os.path.join(dirname,cmdfile),'w').write(cmd)
            cmdlink=misc_utils.html_link('command',cmdfile)
        log=info['misc']['parsed_file'] if info else 'missing'
        log=misc_utils.html_link('log',arch_utils.log_to_href(log),style='color:black;')
        afsdir=os.path.dirname(info['misc']['parsed_file'])
        afslink=misc_utils.html_link('afs','!file://%s'%afsdir,style='color:black;')
        status='<span style="color:%s">%s</span>'%(('green','ok') if job_ok(info) else ('red','fail'))
        pmonsd=[]
        for p in info.get('pmonsd',[]):
            pfile=p['fulloutput_file'].replace('.pmon.gz','.pmon.pmonsd.txt.gz')
            pmonsd+=[misc_utils.html_link(pmonsd_utils.get_step_name(p),
                                          arch_utils.log_to_href(os.path.join(afsdir,pfile)),
                                          style='color:black;')]
        format='    <tr style="font-size:80%%"><th>%s</th><th style="text-align:center">%s</th><th style="text-align:center">%s</th>'
        format+='<th style="text-align:center">%s</th><th>%s</th><th style="text-align:center">%s</th></tr>'
        l+=[format%(lbl,status,afslink,log,' / '.join(pmonsd),cmdlink)]
    l+=['  </table>']
    l+=['  <p/><a name="tagdiffs"><strong>Package tag differences:</strong></a>']
    l+=['  <div style="width:900px;padding:10px;color:gray;font-size:80%">The following table contains sample tag differences for builds where jobs used on this page were running.']
    l+=['  If you want other differences (i.e. for different dates or across different builds), you can use the pmb-taginfo command on lxplus. Click on any tag difference link in the table below for examples of exact commands to run.</div><p/>']
    branches=set(e[1] for e in build_ids)
    single_branch = branches.pop() if len(branches)==1 else None
    l+=[table_header1]
    l+=[table_header2%(3,'Sample tag differences (%s)'%single_branch if single_branch else 'Sample tag differences')]
    colnames =('First date','Second date') if single_branch else ('Build 1','Build 2')
    colstyles=(' style="text-align:center"',' style="text-align:center"') if single_branch else ('','')
    l+=['    <tr style="background-color:rgb(215,215,255);"><th>%s</th><th>%s</th><th style="text-align:center">Tag differences</th></tr>'%colnames]
    for ibldid in range(len(build_ids)-1,0,-1):
        b2,b1=build_ids[ibldid],build_ids[ibldid-1]
        ntagschanged,tagdifflines=get_tagdiff(date1=b2[0],buildid1=b2[1],date2=b1[0],buildid2=b1[1])
        tagdifflines=['# --- Tag differences of %s:%s to %s:%s " ---'%(b2[1],b2[0],b1[1],b1[0]),'','#','# To reproduce run:','#',
                      '#   /afs/cern.ch/atlas/project/pmb/bin/pmb-taginfo --diff=%s:%s:%s:%s'%(b2[1],b2[0],b1[1],b1[0]),'#','']+tagdifflines
        tagdifflines='\n'.join(tagdifflines)+'\n'
        tagdiff_filename='tagdiff_%s_%s_%s_%s_%s.txt'%(b2[1],b2[0],b1[1],b1[0],hex(hash(tagdifflines))[2:])

        tf='%i tag%s changed'%(ntagschanged,'&nbsp;' if ntagschanged==1 else 's')
        if ntagschanged:
            open(os.path.join(dirname,tagdiff_filename),'w').write(tagdifflines)
            tf=misc_utils.html_link(tf,tagdiff_filename)
        #tf=max(0,4-len(str(ntagschanged)))*'&nbsp;'+tf
        format='    <tr style="font-size:80%%%%"><th%s>%%s</th><th%s>%%s</th><th style="text-align:right">%%s&nbsp;</th></tr>'%colstyles
        d1=misc_utils.date_format(b1[0],show_year=False)
        d2=misc_utils.date_format(b2[0],show_year=False)
        l+=[format%(d2 if single_branch else '%s:%s'%(b2[1],d2),
                    d1 if single_branch else '%s:%s'%(b1[1],d1),tf)]
    l+=['  </table>']
    l+=['  <div style="height:100px"></div>']
    return l

def finish_up_page(html,label2info):
    update='    <span style="font-size:70%%">Page automatically updated LASTUPDATE[%s +0000].</span><p/>'%datetime.datetime.utcnow().isoformat(' ')
    #html.insert(0,update)
    htmlout='\n'.join(html+[''])
    outdir=plot_utils.get_plot_dir()
    htmlout+='\n'.join(logs_to_html_list(label2info,outdir)+[''])
    htmlout+='\n'+update+'\n'
    if htmlout:
        open(os.path.join(outdir,'html.snippet'),'w').write(htmlout)

def gen_annotations(labels2info):
    #returns title,comments,label2comment (label2comment for the case where points have different comments)
    global stream_to_short
    extra_text=[]
    #figure out if all points have the same stream:
    pts_streams,nevts,trfs=set(),set(),set()
    labels_have_stream=True

    for l,i in labels2info.items():
        if is_multijobs(i):
            goodjobs=[j for _,j in i['labels_and_jobs'] if job_ok(j)]
        else: goodjobs=[i] if job_ok(i) else []
        if not goodjobs: continue
        for j in goodjobs:
            s=j['misc']['infile_info']['stream']
            if labels_have_stream:
                if not isinstance(l,str) or (stream_to_short[s].lower() not in l.lower() and s.lower() not in l.lower()):
                    labels_have_stream=False
            pts_streams.add(s)
            trfs.add(filter(lambda x:x.endswith('_trf.py'),j['misc']['command'].split())[0])
            nevts.add(j['misc']['inputevts'])
    if not nevts: return None,None
    if not labels_have_stream and len(pts_streams)==1: stream=pts_streams.pop()
    else: stream=None
    assert len(trfs)==1 and len(nevts)>=1
    nevts = None if len(nevts)!=1 else nevts.pop()
    trftype=trfs.pop().split('_trf.py')[0]

    label2comment={}
    amitags,file_ids=set(),set()
    for l,i in labels2info.items():
        i=sample_job(i)
        if not job_ok(i): continue
        label2comment[l]='ami %s, run %i, lb %i'%(i['misc']['amitag'],i['misc']['infile_info']['run'],i['misc']['infile_info']['lb'])
        if nevts==None:
            label2comment[l]+=', %ievts'%i['misc']['inputevts']
            #label2comment[l]=label2comment[l].replace(' ','').replace(',',', ').replace(':',': ')

    nevts_str= ' of %i events'%nevts if nevts else ''
    title='%s %s%s'%(stream,trftype.lower(),nevts_str) if stream else '%s%s'%(trftype.lower(),nevts_str)
    return title,label2comment

def make_page(label2info,page_fct,file_prefix,label_order=None,label2descr=None,mark_transitions=False,extra_doc=None,label2info_longterm=None,label2descr_longterm=None):
    #todo: file_prefix should come from page_fct. Should get dirname as argument.
    if verbose: print "Making page",file_prefix
    page=page_fct(label2info)
    page_descr=page.get('doc',['Page description is missing.','This is being fixed presently.'])
    if not isinstance(page_descr,list): page_descr=[page_descr]
    if extra_doc: page_descr += (extra_doc if isinstance(extra_doc,list) else [extra_doc])
    page_descr+=['At the end of the page you can find <a href="!#jobinfo">job info</a> (including log files and how to rerun) as well as <a href="!#tagdiffs">tag differences</a> for the concerned builds.']
    page_title=page.get('title','title missing')
    plots=page['plots']
    html=[]
    jobtype_title,label2comment=gen_annotations(label2info)#fixme: gen_annotations configurable? Different for each plot?
    if label2info_longterm: jobtype_title_lt,label2comment_lt=gen_annotations(label2info_longterm)
    else: jobtype_title_lt,label2comment_lt=None,None

    if not jobtype_title:
        finish_up_page(html,label2info)#no plots, just the job list...
        if verbose: print "Done making",file_prefix
        return
    def plot_name(plot):
        return plot.get('name',plot['title'].lower().replace(' ','').replace('+','').replace('-','').replace('_','').replace('/',''))
    def anchor_name(plot):
        return 'plot_%s'%plot_name(plot)

    listhtml=[]
    #listhtml+=['    <strong>%s</strong>'%page_title]
    listhtml+=['    <div style="width:900px;font-size:80%;color:gray;padding:10px">']
    listhtml+=['      '+('<p/>'.join(page_descr) if isinstance(page_descr,list) else page_descr)]
    listhtml+=['    </div>']
    listhtml+=['    <p/><strong>Plots on this page:</strong>']
    ncols=min(3,1+(len(plots)-1)/4)
    if ncols==1: listhtml+=['    <div>']
    else: listhtml+=['    <div style="width:1000px;column-count:%i;-moz-column-count:%i;-webkit-column-count:%i">'%(ncols,ncols,ncols)]
    listhtml+=['      <ul style="font-size:80%">']
    for iplot,plot in enumerate(plots):
        listhtml+=['        <li><a href="!#%s">%s</a></li>'%(anchor_name(plot),plot['title'])]
    listhtml+=['      </ul>']
    listhtml+=['    </div>']
    html+=['\n'.join(listhtml)]

    def prepare_data(plot,label2info,label_order,jobtype_title,label2comment,label2descr):
        comments=[]
        okdata=((label,info) for label,info in misc_utils.dictiter_bykeys(label2info) if job_ok(info))
        if 'varcalc_bystep' in plot.keys(): data=[(label,info_by_step(info,plot['varcalc_bystep'])) for label,info in okdata]
        else: data=[(label,plot['varcalc'](info)) for label,info in okdata]
        data = [(x,y) for x,y in data if y]
        nshown=plot.get('nshown',None)
        ycut=plot.get('ycut',None)
        if nshown or ycut: data=misc_utils.remove_least_significant_comps(data,nshown=nshown,ycut=ycut,otherstr=plot.get('otherstr','Other %i'))
        if label_order:
            data.sort(lambda x,y:cmp(label_order.index(x[0]),label_order.index(y[0])) or cmp(x,y))
        label_descriptions=[]
        transitions=[]
        if label2descr:
            last_descr=[]
            for ilbl,(lbl,_) in enumerate(data):
                descr=label2descr.get(lbl,[])
                if descr!=last_descr:
                    if not descr in label_descriptions: label_descriptions+=[descr]
                    if mark_transitions and ilbl: transitions+=[(ilbl-1,ilbl)]
                last_descr=descr
        if label_descriptions:
            extralines=[]
            for i in range(max(len(e) for e in label_descriptions)):
                for el in ['%s: %s'%e[i] for e in label_descriptions if i<len(e)]:
                    if not el in extralines: extralines+=[el]
            comments+=['','Label%s:'%('' if len(extralines)==1 else 's')]
            comments+=extralines

        if 'total' in plot.keys():
            for i in range(len(data)):
                if data[i][1]:
                    data[i][1].insert(0,(plot['total'][0],plot['total'][1](sum(v for k,v in data[i][1]))))
        p=plot_utils.create_plot_from_label2compandvals(data,xlabel_rotation=90,
                                                        comments=comments,label2comment=label2comment,
                                                        mark_transitions=transitions,
                                                        transition_opts={'lw':7,'color':(0.8,0.8,0.99)})
        title=plot['title']+' in '+jobtype_title
        p.set_title(title)
        ylbl=plot.get('ylabel',None)
        if ylbl: p.set_yaxis_label(ylbl)
        titles=[title]
        if ylbl: titles+=[ylbl]
        def align_fct(x,i,ie,w): return str(x).ljust(w) if ie==-1 else str(x).rjust(w)
        return p,misc_utils.data_table(data,align_fct=align_fct,title=' / '.join(titles)),title

    def write_all(filename,plot,tablelines):
        outfile=plot.write(filename,quiet=True)
        plot.write_script(filename,['Autogenerated python script for plot generation.','Contact person: Thomas Kittelmann.','<std>'])
        outfilebn,_=os.path.splitext(outfile)
        open(outfilebn+'.'+'txt','w').write('\n'.join(tablelines)+'\n')
        return outfilebn

    for iplot,plot in enumerate(plots):
        fn='plot_%i_%s_%s'%(iplot,file_prefix,plot_name(plot))
        #Recent plots:
        p,tablelines,title=prepare_data(plot,label2info,label_order,jobtype_title,label2comment,label2descr=label2descr)
        outfilebn=write_all(fn,p,tablelines)

        if label2info_longterm:
            assert label_order==None
            p,tablelines,title=prepare_data(plot,label2info_longterm,label_order=None,
                                            jobtype_title=jobtype_title_lt,label2comment=label2comment_lt,
                                            label2descr=label2descr_longterm)
            outfilebn_longterm=write_all(fn+'_longterm',p,tablelines)
        else:
            outfilebn_longterm=None

        links,links_lt=[],[]
        for format,text in [('png','PNG')]+alt_formats+[('txt','table'),('py','python')]:
            if format!='png':
                altfile=outfilebn+'.'+format#fixme wont work with 'transp:png'
                assert os.path.exists(altfile)
                links+=[misc_utils.html_link(text,os.path.basename(altfile))]
            if outfilebn_longterm:
                altfile=outfilebn_longterm+'.'+format#fixme wont work with 'transp:png'
                assert os.path.exists(altfile)
                links_lt+=[misc_utils.html_link(text,os.path.basename(altfile))]

        #Create html for linking image, and create thumbnail in the process:
        imgtag=misc_utils.html_img_tag(outfilebn+'.png',th_rel=0.5)
        #Not so pretty, but dig out the thumbnail dimensions from this piece of html:
        th_w=int(imgtag.split('width="')[1].split('"')[0])
        th_h=int(imgtag.split('height="')[1].split('"')[0])
        totwidth=max(1000,400+th_w)
        totheight=max(400,th_h+50)
        plothtml=['\n  <div style="padding:10px;width:%ipx;height:%ipx">'%(totwidth,totheight)]
        #plothtml+=['    [<a href="!#%s">link</a>] <a name="plot_%s"><strong>%s:</strong></a><br/>'%(anchor_name(plot),plot_name(plot),title)]
        plothtml+=['    <a name="%s"><strong>Plot: %s</strong></a><br/>'%(anchor_name(plot),title)]
        plothtml+=['    <div style="float:left;width:%ipx;padding:10px">'%th_w]
        plothtml+=['      '+imgtag]
        plothtml+=['    </div>']
        plothtml+=['    <div style="float:right;width:%ipx;font-size:80%%;color:gray;padding:10px">'%(totwidth-th_w-50)]
        doc=plot.get('doc','(description missing)')
        plothtml+=['      <strong>Direct links:</strong> <a href="!">this page</a> / <a href="!#%s">this plot</a><p/>'%anchor_name(plot)]
        plothtml+=['      <strong>Other formats:</strong> '+' / '.join(links)+'<p/>']
        if links_lt:
            plothtml+=['      <strong>Long term:</strong> '+' / '.join(links_lt)+'<p/>']
        plothtml+=['      <strong>Description:</strong><p/>']
        plothtml+=['      '+('<p/>'.join(doc) if isinstance(doc,list) else doc)]
        plothtml+=['    </div>']
        plothtml+=['  </div><br/>\n\n']
        html+=['\n'.join(plothtml)]

    finish_up_page(html,label2info)
    if verbose: print "Done making",file_prefix

plot_utils.set_default_extension([img_format]+[f for f,n in alt_formats])

stream_to_job=lambda s: 'reco_%s'%s.lower()
jobs=[stream_to_job(s) for s,_ in streams]

pages=[(pagedefs.define_page_timings,'timings'),
       (pagedefs.define_page_memory,'memory'),
       (pagedefs.define_page_evtcpu,'evtcpu'),
       (pagedefs.define_page_memdetail,'memdetail'),
       (pagedefs.define_page_mallocdetail,'maldetail'),
       (pagedefs.define_page_filesizes,'filesizes'),
       (pagedefs.define_page_loglines,'printouts')]

builds=arch_utils.getActiveBuilds(jobs,daysback=daysback)

if quick_test:
    builds.remove('17.2.X.Y-VAL/i686-slc5-gcc43-opt')
    builds.remove('17.2.X.Y/x86_64-slc5-gcc43-opt')

if nsubmax=='auto':
    freecores=misc_utils.get_n_cores()-misc_utils.get_load()
    nsubmax=max(1,int(0.5+(0.4 if in_cron else 0.9)*freecores))
    if quick_test: nsubmax=max(misc_utils.get_n_cores(),nsubmax)

import multiprocessing
__processes=[]
__mp_max=0
def end_process(proc):
    global __processes
    proc.join()
    ec=proc.exitcode
    if ec!=0:
        print "ERROR: Process ended badly!"
        for p in __processes:
            print "Terminating a child process"
            p.terminate()
        time.sleep(1)
        sys.exit(ec)

def make_page_mp(**kw):
    global __processes
    global __mp_max
    if nsubmax<=1:
        make_page(**kw)
        return
    finished=[p for p in __processes if not p.is_alive()]
    if finished:
        for p in finished:
            end_process(p)
            __processes.remove(p)
    while len(__processes)>=nsubmax:
        p=__processes.pop(0)
        end_process(p)
    p=multiprocessing.Process(target=make_page,kwargs=kw)
    p.start()
    __processes+=[p]
    __mp_max=max(__mp_max,len(__processes))

def finish_all_processes():
    global __processes
    global __mp_max
    for p in __processes: end_process(p)
    __processes=[]
    if verbose and __mp_max: print "Ended all subprocesses. Had at most %i running."%(__mp_max)


#msg abuse plots:

for b in msg_builds:
    date2msgjobs={}
    date2msgjobs_lt={}
    for job in msg_jobs:
        gathered_info=arch_utils.gather(arch_utils.pmbparse,job,b,daysback=daysback,verbose=verbose)
        for dt,info in misc_utils.dictiter_bykeys(gathered_info):
            date2msgjobs.setdefault(dt,{'multijobs':True,'ok_if_any_ok':False,'labels_and_jobs':[]})['labels_and_jobs']+=[(job,info)]
        if do_longterm:
            gathered_info_lt=arch_utils.gather(arch_utils.pmbparse,job,b,daysback=daysback_longterm,verbose=verbose)
            for dt,info in misc_utils.dictiter_bykeys(gathered_info_lt):
                date2msgjobs_lt.setdefault(dt,{'multijobs':True,'ok_if_any_ok':False,'labels_and_jobs':[]})['labels_and_jobs']+=[(job,info)]

    plot_utils.set_plot_dir(os.path.join(outdirbase,'msg/%s'%b.replace('/','__')),create=True)
    if date2msgjobs:
        make_page_mp(label2info=date2msgjobs,page_fct=pagedefs.define_page_msgdbg,file_prefix='msgsvcabuse',
                     label2info_longterm=date2msgjobs_lt)

compare_info={}
compare_info_label_order=[]
compare_info_label2descriptions={}

ncompare=3

date2multistreams={}
multistreams_label2descriptions={}
date2multistreams_lt={}
multistreams_label2descriptions_lt={}

for stream,stream_shortname in streams:
    for b in builds:
        job=stream_to_job(stream)
        gathered_info=arch_utils.gather(arch_utils.pmbparse,job,b,daysback=daysback,verbose=verbose)
        if not gathered_info: continue#perhaps deliberately no such jobs on this nightly
        if do_longterm:
            gathered_info_longterm=arch_utils.gather(arch_utils.pmbparse,job,b,daysback=daysback_longterm,verbose=verbose)
        else:
            gathered_info_longterm=None
        build_label,build_descr=comparison_builds.get(b,(None,None))
        if build_label:
            #need the last ncompare good jobs for comparison plots + misc streams/builds for overview
            added_order=[]
            for dt,info in misc_utils.dictiter_bykeys(gathered_info,reverse=True):
                if len(added_order)<ncompare and job_ok(info):
                    label='%s-%s-%s'%(stream_to_short[stream],build_label,str(dt)[5:])
                    assert not label in compare_info
                    compare_info[label]=info
                    added_order+=[label]
                    labeldescrip=[(stream_to_short[stream],stream+' stream'),(build_label,build_descr)]
                    compare_info_label2descriptions[label]=labeldescrip
                date2multistreams.setdefault(dt,{'multijobs':True,'ok_if_any_ok':True,'labels_and_jobs':[]})['labels_and_jobs']+=[(stream+'_'+build_label,info)]
                msl2d=multistreams_label2descriptions.setdefault(dt,[])
                msl2d+=[(build_label,build_descr)]
            if do_longterm:
                for dt,info in misc_utils.dictiter_bykeys(gathered_info_longterm,reverse=True):
                    date2multistreams_lt.setdefault(dt,{'multijobs':True,'ok_if_any_ok':True,'labels_and_jobs':[]})['labels_and_jobs']+=[(stream+'_'+build_label,info)]
                    msl2d=multistreams_label2descriptions_lt.setdefault(dt,[])
                    msl2d+=[(build_label,build_descr)]

            for a in added_order[::-1]: compare_info_label_order+=[a]

        if quick_test and (b,stream)!=('17.2.X.Y/i686-slc5-gcc43-opt','JetTauEtmiss'): continue
        b=b.replace('/','__')
        for page_def,page_name in pages:
            plot_utils.set_plot_dir(os.path.join(outdirbase,page_name,'%s__%s'%(stream,b)),create=True)
            make_page_mp(label2info=gathered_info,label2info_longterm=gathered_info_longterm,
                         page_fct=page_def,file_prefix='%s_%s'%(page_name,b))

#One global overview page to serve as front page:
plot_utils.set_plot_dir(os.path.join(outdirbase,'all/overview'),create=True)#FIXME put inside make_page!!
make_page_mp(label2info=date2multistreams,page_fct=pagedefs.define_page_overview,file_prefix='overview',
             label2descr=multistreams_label2descriptions,
             label2info_longterm=date2multistreams_lt,
             label2descr_longterm=multistreams_label2descriptions_lt)

#One domain-callgrind page:
import PmbUtils.RTTDomainCallgrindUtil
PmbUtils.RTTDomainCallgrindUtil.append_html_table_to_file(filename=os.path.join(outdirbase,'profiling/callgrind/html.snippet'),domain_prettifier=lambda x: x.capitalize())

#One plot per page type comparing builds!
for page_def,page_name in pages:
    plot_utils.set_plot_dir(os.path.join(outdirbase,page_name,'build_and_stream_comparisons'),create=True)#FIXME put inside make_page!!
    make_page_mp(label2info=compare_info,page_fct=page_def,file_prefix='%s_buildstreamcomp'%page_name,
                 label_order=compare_info_label_order,label2descr=compare_info_label2descriptions,
                 mark_transitions=True,
                 extra_doc='This is a special page comparing quantities across streams and builds by taking the last '
                 'few good jobs in each stream/build configuration and plotting all the resulting values next to each other.')

finish_all_processes()

if verbose: print 'Ending happily'
#todo:
#  - Table in html pre tags for safari.
#  - table output in csv format for oocalc/excel
#  - comments about how to reproduce
#  - links to related pages.
#  - links to nicos?
#  - long-term plots
#  - transparent plots (perhaps use plot replay ability to work around matplotlib bug?)
#  - links to f-tag definitions (and differences)
#  - make sure plots look good even with just 1 days data (1 day should not be 2?)
#  - put page defs in different module and add cmdline interface
#  - taginfo module.
#  - info for how to add a plot based on a custom job to a page (if not multijob)
#  - Something better than the many [1] [1] [1] [1] [1] [1] [1] [2] [2] [2] [2] [2] [2] [2]
#  - plot titles should perhaps be left adjusted rather than centered
#  - can a job under strace tell us which packages are used in the jobs? to have smaller tag diffs?
#  - msg abuse plots.

#todo for custom_cron.py:
#  - update to f376 (or newer)
#  - remove local PmbUtils hack
#  - no need to export AtlasSetup env var.
#  - T32 and dev builds first.

#menustyle.css in SVN!!

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import glob
import os

def decode(callgrindfile_path):
    parts=callgrindfile_path.split('/')[-8:]
    reln,branch,_,cmtconfig,project,rttpkg,rttjob,filename=parts
    project={'offline':'AtlasProduction'}[project]#FIXME: Add more as needed
    step,domain=filename[len('callgrind.'):-len('.out.bz2')].split('_')
    webbase='https://atlas-rtt.cern.ch/prod/rtt/'+'/'.join(parts[:-1])
    webrttjob='https://atlas-rtt.cern.ch/index.php?q=%%28release=%s;branch=%s;cmtconfig=%s;project=%s;packagename=%s;jobname=%s%%29&amp;page=job'%(reln,branch,cmtconfig,project,rttpkg,rttjob)

    return {'domain':domain,'step':step,'reln':reln,'branch':branch,'cmtconfig':cmtconfig,'rttpkg':rttpkg,'rttjob':rttjob,'filename':filename,
            'afs_file':callgrindfile_path,'www_file':'%s/%s'%(webbase,filename),'www_log':'%s/%s_log'%(webbase,rttjob),'www_rttjob':webrttjob,
            'log_creation_time':os.path.getmtime(os.path.join(os.path.dirname(callgrindfile_path),'%s_log'%rttjob))}

def find_all():
    pattern='/afs/cern.ch/atlas/project/RTT/prod/Results/rtt*/rel_*/*/build/*/*/RecPerfTests/domaincallgrind_*/callgrind.*_*.out.bz2'
    l=[]
    for p in glob.glob(pattern):
        dec=decode(p)
        l+=[(dec['log_creation_time'],dec['step'],dec)]
    l.sort(reverse=True)
    return [dec for _,_,dec in l]

def produce_html_table(decoded_list=None,domain_prettifier=None):

    if domain_prettifier==None: domain_prettifier=lambda x:x
    if decoded_list==None: decoded_list=find_all()

    __tablestyle='font-size:80%;border:thin solid gray;background-color:rgb(240,240,240);text-align:center;white-space:nowrap;'
    __headelementstyle='text-align:left;background-color:rgb(200,200,255);'
    __elementstyle='text-align:left;font-size:80%'

    def format(entries,head=False):
        st=__elementstyle
        if head: st=__headelementstyle
        st=' style="%s"'%st
        return '<tr><td%s>%s</td></tr>'%(st,('</td><td %s>'%st).join(entries))

    out=[]

    out+=['<div style="width:900px;font-size:80%;color:gray;padding:10px">']
    out+=['Domain specific callgrind files which can be investigated with kcachegrind. The files have been patched to have correct AFS paths to the sourcecode. This means that if you run on a machine with CERN AFS access, source code information should be integrated.']
    out+=['<p/>']
    out+=['While some never versions of kcachegrind can load .bz2\'ed files directly, this is unfortunately not normally the case. So to open you must download or copy the file somewhere and run bunzip2 on it before it can be opened with kcachegrind (you can use e.g. /tmp/$USER to avoid big files in your AFS homedir.']
    out+=['<p/>']
    out+=['If planning to ssh to lxplus and launch kcachegrind there, remember to supply -X (or -Y from osx) so you can actually launch X applications.']

    out+=['<p/>']
    out+=['If your domain is missing, perhaps the RTT job is not running. Check for red flags <a href="https://atlas-rtt.cern.ch/index.php?q=%28packagename=RecPerfTests;verbosity=vvv;jobname=domaincallgrind_*%29">here</a> (note that to preserve resources each domain job does not actually run each night, so a green flag does not mean that output should be there).']
    out+=['<p/>']
    out+=['Should you with to reproduce these results by hand, look at the beginning of the log-files for the commands']
    out+=['<p/>']
    out+=['Note, you can find callgrind jobs with global profiling <a href="https://atlas-rtt.cern.ch/index.php?q=%28verbosity=vvv;jobname=CALLGRIND*%29">here</a>']
    out+=['</div>']

#download, bunzip2, open with kcachegrind. AFS access to really look inside 
#link to rolfs 


    out+=['<table style="%s">'%__tablestyle]
    out+=[format(['domain','step','nightly','build','callgrind file','other info'],head=True)]
    for dec in decoded_list:
        l=[domain_prettifier(dec['domain']),
           dec['step'],
           dec['reln'],
           '%s/%s'%(dec['branch'],dec['cmtconfig']),
           '<a href="%s">%s</a>'%(dec['www_file'],dec['filename']),
           '<a href="!file://%s">AFS location</a> / '%dec['afs_file']+
           '<a href="%s">RTT job page</a> / '%dec['www_rttjob']+
           '<a href="%s">job logfile</a>'%dec['www_log']
           ]
        out+=[format(l)]
    out+=['</table>']
    return out

def append_html_table_to_file(filename,**kw):
    dirname=os.path.dirname(filename)
    if not os.path.exists(dirname):
        os.makedirs(dirname)
    fh=open(filename,'a')
    fh.write('\n'.join(produce_html_table(**kw))+'\n')
    fh.close()

if __name__=='__main__':
    append_html_table_to_file(filename=os.getcwd()+'/test.html',domain_prettifier=lambda x: x.capitalize())

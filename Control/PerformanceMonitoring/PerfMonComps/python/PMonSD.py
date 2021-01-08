# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__   ='Module for parsing and basic analysis of Semi-Detailed PerfMon (PMonSD) output. More info at https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD'
__author__='Thomas Kittelmann <thomas.kittelmann@cern.ch>'


_appname='PMonSD'

def pmonsd_version():
    """Version of PMonSD (tuple of major and minor version numbers)"""
    return (1,0)#Must always be the same as the version in src/SemiDetHelper.cxx
                #Also remember to update the supported versions in __actual_parse(..)

def parse(infile,outfile=None):
    """Parse PMonSD output and return list of dictionaries. Optionally save output in pickle file."""
    p=__smart_parse(infile)
    if p is None:
        return None
    if outfile:
        __save_output(p,outfile,'.psd',infile)
    return p

def deparse(infile):
    """Turn parsed PMonSD info back into a list of strings
    identical to the ones it was parsed from"""
    out=[]
    p=__smart_parse(infile)
    if p is None:
        return []
    for e in p:
        out+=__deparse_single(e)
    return out

def need_line(z):
    """To identify lines which PMonSD needs for parsing"""
    return z.startswith(_appname)

def print_ascii(infile):
    """Print parsed PMonSD info to stdout"""
    for line in deparse(infile):
        print(line)

def diff(infile1,infile2):
    """Look for main performance differences between two PMonSD outputs (this function not finalised yet!)"""
    return __actual_diff(infile1,infile2)

def timestamp_to_datetime(s):
    """For correctly converting timestamps printed by pmonsd to datetime objects"""
    import datetime
    return datetime.datetime.strptime(s,'%Y-%m-%dT%H:%M:%S+0000')

def _validate_identical(infile1,infile2):
    #For validation
    p1=__smart_parse(infile1)
    p2=__smart_parse(infile2)
    if p1 is None or p2 is None:
        return False
    return p1==p2

def force_share(obj):
    """Dive into lists and dictionaries and make sure strings with similar content gets shared"""
    if type(obj)==list:
        __fs_list(obj)
    elif type(obj)==dict:
        __fs_dict(obj)

def get_shared_string(s): return __get_shared_string(s)

def gzip_fastopen(fn):
    """Alternative to gzip.open which is extremely slow in python 2.6"""
    #os.popen is deprecated after python 2.6, but if we move to 2.7 we
    #cah hopefully use gzip.open again
    import os
    return os.popen("gzip -d -c %s"%fn)


##############################################################################
##############################################################################
##############################################################################
##           The rest of the file is pure internal implementation           ##
##############################################################################
##############################################################################
##############################################################################







def __save_output(data,outfile,prefix,infile=None):
    if not outfile.endswith(prefix) and not outfile.endswith(prefix+'.gz'):
        if outfile.endswith('.gz'):
            outfile=outfile[:-3]
        outfile+=prefix+'.gz'
    if outfile.endswith('.gz'):
        import gzip
        fh=gzip.open(outfile,'w')
    else:
        fh=open(outfile,'w')
    if infile is not None and outfile==infile:
        print("%s.parse WARNING: output file %s equals input file. Won't dump."%(_appname,outfile))
    else:
        import cPickle
        force_share(data)#make sure we register shared strings
        cPickle.dump(data,fh)
        print("%s.parse INFO: Placed info in %s"%(_appname,outfile))

def __smart_parse(infile):
    #Input can be in a wide range of formats.
    if type(infile)==str:
        #must be a filename
        if infile.endswith('.pmon.gz'):
            #perfmon tar-ball
            import tarfile
            tf=tarfile.open(infile)
            for n in tf.getnames():
                if n.endswith('.pmonsd.txt'):
                    return __actual_parse(tf.extractfile(n))
            return None
        if infile.endswith('.gz'):
            #gzipped txt output or pickle
            #import gzip
            pkl=infile.endswith('.psd.gz')
            try:
                #fh=gzip.open(infile)
                fh=gzip_fastopen(infile)
            except IOError as e:
                print("Could not open and decompress file %s: %s"%(infile,e))
                return None
        else:
            #unzipped txt output or pickle
            fh,pkl=open(infile),infile.endswith('.psd')
        if pkl:
            #a pickle file with previously parsed info:
            import cPickle
            try:
                ds=cPickle.load(fh)
            except cPickle.UnpicklingError as e:
                print("ERROR: Could not load pickled info from file %s: %s"%(infile,e))
                return None
            #basic validation of the format:
            for d in ds:
                keys=d.keys()
                for k in ['version','special','steps_comps','steps_totals']:
                    assert k in keys
            return ds
        else:
            return __actual_parse(fh)
    else:
        #in case this is already parsed info, make sure we just return it as it is:
        if type(infile)==list:
            if len(infile)==0:
                return infile
            if type(infile[0])==dict and 'steps_comps' in infile[0].keys():
                return infile
        #Hopefully this is something we can iterate through (like a list of strings or a file-handle):
        return __actual_parse(infile)

def __actual_parse(filehandle):
    _prefix=_appname+' '
    intro_version='semi-detailed perfmon info v'
    end_marker='semi-detailed perfmon info / end'
    supported_versions=[(0,1),(1,0)]#keep in synch with version info above
    assert pmonsd_version() in supported_versions
    assert max(supported_versions)==pmonsd_version()
    version=None
    output=[]
    def new_dict():
        return {'special':{'leaks':{},'timings':{},'snapshots':{},'values':{},'nevts':None},
                'steps_comps':{},'steps_totals':{},'evt_max_info':{}}
    def decode_max(s):
        v,i=s.split('@')
        return (float(v),int(i))
    d=new_dict()
    stepcount={}#for keeping track of in what order within each step a component is listed
    for z in filehandle:
        if not z.startswith(_prefix):
            continue
        #ensure the first thing we pick up is the version:
        if version is None:
            if intro_version in z:
                vstr=z.split(intro_version)[1].split()[0]
                full_info=vstr[-1]=='f'
                v_major,v_minor=vstr[:-1].split('.')
                version=(int(v_major),int(v_minor))
                assert version in supported_versions,"Unknown version of %s output: %s"%(_appname,version)
                d['version']=version
                d['full_info']=full_info
                if version>pmonsd_version():
                    print("WARNING: Using PMonSD of version %f to parse output made with version %f"%(pmonsd_version(),version))
            continue
        #remove prefix:
        z=z[len(_prefix):].strip()
        if z.startswith('WARNING'):
            continue
        if z.startswith('=='):
            #This is a comment/separator. Look for end marker:
            if end_marker in z:
                #found. Grab parsed info and make room for more (in case of concatenated logs)
                output+=[d]
                d=new_dict()
                version=None#reset
            elif 'Full output inside:' in z:
                filename=z.split('Full output inside:')[1].split('===')[0].strip()
                d['fulloutput_file']=filename
            continue
        if not z.startswith('['):
            continue#ignore column headers
        f=z.split()
        if f[0]=='[---]' and '=' in f[1]:
            for valfield in f[1:]:
                n,vstr=valfield.split('=',1)
                try:
                    v=float(vstr)
                except ValueError:
                    v=vstr
                    pass
                #special value
                d['special']['values'][n]=v
            continue
        step,comp,n,f=f[0][1:-1],f[-1],int(f[1]),f[2:-1]
        is_total=comp.startswith('[total')
        is_collapsed=comp.startswith('[collapsed')
        if step=='---':
            if comp.startswith('leak'):
                d['special']['leaks'][comp]={'n':n,'vmem':float(f[2]),'malloc':float(f[3])}
            elif comp.endswith('_time'):
                d['special']['timings'][comp]={'n':n,'cpu':float(f[0]),'wall':float(f[1])}
                if comp=='evtloop_time':
                    d['special']['nevts']=n
            else:
                d['special']['snapshots'][comp]={'n':n,'cpu':float(f[0]),'wall':float(f[1]),
                                                 'vmem':float(f[2]),'malloc':float(f[3])}
        else:
            if step not in d['steps_comps'].keys():
                d['steps_comps'][step]={}
                d['steps_totals'][step]={}
                stepcount[step]=0
            iorder=stepcount[step]
            stepcount[step]+=1
                #workaround situation where two collapsed or total lines have same form (nentries is always different):
            if is_collapsed and comp in d['steps_comps'][step].keys():
                comp+=':n=%i'%n
            if is_total and comp in d['steps_totals'][step].keys():
                comp+=':n=%i'%n
            if len(f)==6:
                #has max@evt info
                d['steps_comps'][step][comp]={'order':iorder,'n':n,'cpu':float(f[0]),'vmem':float(f[2]),'malloc':float(f[4])}
                d['evt_max_info'][comp]={'cpu':decode_max(f[1]),'vmem':decode_max(f[3]),'malloc':decode_max(f[5])}
            else:
                #doesn't have max@evt info (step!='evt' or 'evt' but collapsed or total)
                nfo={'order':iorder,'n':n,'cpu':float(f[0]),'vmem':float(f[1]),'malloc':float(f[2])}
                if is_total:
                    d['steps_totals'][step][comp]=nfo
                else:
                    d['steps_comps'][step][comp]=nfo
    force_share(output)#make sure we register shared strings
    return output

def __deparse_single(d):
    _prefix=_appname+' '
    out=[]
    assert type(d)==dict
    def header(z,s,center=True):
        if center:
            s=(' %s '%s).center(82,'=')
        else:
            s=(' %s '%s).ljust(82,'=')
        z+=[ _prefix+'==='+s+'===']
        
    full_info=d['full_info']
    if full_info:
        fullstr='f'
    else:
        fullstr='c'
    header(out,'semi-detailed perfmon info v%i.%i%s / start'%(d['version'][0],d['version'][1],fullstr))
    header(out,'Documentation: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD',center=False)
    header(out,'Note that documentation includes recipe for easy parsing from python.  ',center=False)
    header(out,'Units are milliseconds (cpu/wall) and kilobytes (vmem/malloc).         ',center=False)
    if 'fulloutput_file' in d.keys():
        header(out,('Full output inside: %s'%d['fulloutput_file']).ljust(71),center=False)
    stdsteps=['ini','1st','cbk','evt','fin']
    steps=[]
    for step in d['steps_comps'].keys():
        if step not in stdsteps and step not in steps:
            steps+=[step]
    steps.sort()
    steps=stdsteps+steps
    colheader_std='      n    cpu    vmem  malloc component'
    colheader_evt='      n    cpu    max@evt    vmem    max@evt  malloc    max@evt  component'
    def format_max(m):
        return '%6i@%-4i'%m

    format_evt_withmax='%4i %6i %s %6i %s %6i %s %s'
    format_evt_nomax='%4i %6i            %7i            %7i             %s'
    format_notevt='%4i %6i %7i %7i %s'
    for step in steps:
        is_evt=step=='evt'
        header(out,'step %s'%step)
        entries=[]
        if step not in d['steps_comps'].keys():
            continue
        for comp,compdata in d['steps_comps'][step].items():
            if '_comps]:n=' in comp:
                comp=comp.split('_comps]:n=')[0]+'_comps]'
            if is_evt and comp in d['evt_max_info'].keys():
                s=format_evt_withmax%(compdata['n'],compdata['cpu'],format_max(d['evt_max_info'][comp]['cpu']),
                                                 compdata['vmem'],format_max(d['evt_max_info'][comp]['vmem']),
                                                 compdata['malloc'],format_max(d['evt_max_info'][comp]['malloc']),comp)
            else:
                if is_evt:
                    format=format_evt_nomax
                else:
                    format=format_notevt
                s=format%(compdata['n'],compdata['cpu'],compdata['vmem'],compdata['malloc'],comp)
            entries+=[(compdata['order'],comp,s)]
        for comp,compdata in d['steps_totals'][step].items():
            if '_comps]:n=' in comp:
                comp=comp.split('_comps]:n=')[0]+'_comps]'
            format='%4i %6i %7i %7i %s'
            if is_evt:
                format='%4i %6i            %7i            %7i             %s'
            s=format%(compdata['n'],compdata['cpu'],compdata['vmem'],compdata['malloc'],comp)
            entries+=[(compdata['order'],comp,s)]
        if entries:
            entries.sort()
            if is_evt:
                out+=[ _prefix+' '*len(step)+colheader_evt]
            else:
                out+=[ _prefix+' '*len(step)+colheader_std]
            for _,_,s in entries:
                out+=[ '%s[%s] %s'%(_prefix,step,s)]
    header(out,'special info')
    out+=[ _prefix+'         n      cpu     wall     vmem   malloc component']
    for t in ['evtloop_time','overhead_time']:
        dt=d['special']['timings'][t]
        out+=[ '%s[---] %4i %8i %8i        -        - %s'%(_prefix,dt['n'],dt['cpu'],dt['wall'],t)]
    for snapshot in ['pre_ini','post_ini','post_1stevt','post_lastevt','post_fin']:
        snapshot='snapshot_'+snapshot
        ds=d['special']['snapshots'][snapshot]
        out+=[ '%s[---] %4i %8i %8i %8i %8i %s'%(_prefix,ds['n'],ds['cpu'],ds['wall'],ds['vmem'],ds['malloc'],snapshot)]
    leaks=d['special']['leaks'].keys()
    leaks.sort()
    for leak in leaks:
        dl=d['special']['leaks'][leak]
        out+=[ '%s[---] %4i        -        - %8i %8i %s'%(_prefix,dl['n'],dl['vmem'],dl['malloc'],leak)]
    order=[['vmem_peak','vmem_mean','rss_mean'],
           ['jobcfg_walltime','jobstart'],
           ['cpu_bmips','cpu_res','release'],
           ['cpu_model'],
           ['malloc','pycintex_vmemfix'],
           ['pmonsd_cost_onceperjob','pmonsd_cost_perevt']]
    for lineorder in order:
        lineformat=[]
        for sv in lineorder:
            v=d['special']['values'][sv]
            if type(v)==float:
                v_str='%i'%v
            else:
                v_str=v
            lineformat+=['%s=%s'%(sv,v_str)]
        out+=['%s[---] %s'%(_prefix,' '.join(lineformat))]
    header(out,'semi-detailed perfmon info / end')
    return out

def _validate_deparsing(f):
    #parse + deparse and test that we end up with the same.
    _prefix=_appname+' '
    lines=[]
    if f.endswith('.gz'):
        #import gzip
        #fh=gzip.open(f)
        fh=gzip_fastopen(f)
    else:
        fh=open(f)
    for z in fh:
        if z.startswith(_prefix):
            if z.startswith(_prefix+'WARNING'):
                continue
            if z.endswith('\n'):
                z=z[0:-1]
            lines+=[z]
    if len(lines)==0:
        print("File does not have %s lines!"%_appname)
        return False
    d=__smart_parse(lines)
    if d is None:
        return False
    lines2=deparse(d)
    if len(lines)!=len(lines2):
        print("Validation failed! Linecount mismatch. %i vs. %i"%(len(lines),len(lines2)))
        found=False
        for i in range(min(len(lines),len(lines2))):
            if lines[i]!=lines2[i]:
                found=True
                print("< ",lines[i])
                print("> ",lines2[i])
            if not found:
                print("Difference must be in final lines")
                return False
        return False
    for i in range(len(lines)):
        if lines[i]!=lines2[i]:
            print("Validation failed in following lines:")
            print("parsed  :",lines[i])
            print("deparsed:",lines2[i])
            return False
    return True

def __actual_diff(infile1,infile2):
    d1=__smart_parse(infile1)
    d2=__smart_parse(infile2)
    if d1 is None or d2 is None:
        return False
    #Gymnastics to accept separate types:
    if type(d1)==list and type(d2)==list:
        if len(d1)!=len(d2):
            print("Difference detected: Different number of %s sections in inputs: %i vs. %i"%(_appname,len(d1),len(d2)))
            return False
        for i in range(len(d1)):
            if not diff(d1[i],d2[i]):
                print("Difference detected in %s sections with index %i"%(_appname,i))
                return False
        return True
    #Finally start direct diff of two dicts:
    keys1=set(d1.keys())
    keys2=set(d2.keys())
    if keys1!=keys2:
        print("Different keys detected!")
        return False
    if d1['version']!=d2['version']:
        print("WARNING: Comparing pickles with different versions. Results might not be trustworthy!")
    #diff components:
    steps1=set(d1['steps_comps'].keys())
    steps2=set(d2['steps_comps'].keys())
    if steps1!=steps2:
        print("Different steps detected!")
        return False
    #tricky part when comparing components is that a component might
    #have been collapsed in just one, without a significant difference
    #in performance.
    #for nentries and total#algs, it should be possible though to do something fairly robust.
    def _ncomps_and_entries(compdata):
        nentries2ncomps={}
        anycollapsed=False
        for comp,data in compdata.items():
            n=data['n']
            if n not in nentries2ncomps.keys():
                nentries2ncomps[n]=0
            if comp.startswith('[collapsed_'):
                anycollapsed=True
                #nc=int(comp.split('_')[1])
            else:
                pass
                #nc=1
            nentries2ncomps[n]+=1 # check if this should be +=nc
        return nentries2ncomps,anycollapsed


    def compat(comp,step,data1,data2):
        n=data1['n']
        if not n==data2['n']:
            print("Different number of entries for %s in step %s: %i vs. %i"%(comp,step,n,data2['n']))
            return False,True
        valchange=False
        for var,cut in [('cpu',100),('vmem',100),('malloc',10)]:
            if abs(data1[var]-data2[var])>cut:
                valchange=True
                print("Change in %s %s in step %s: %i to %i"%(comp,var,step,data1[var],data2[var]))
                #don't abort since job config didn't change, only performance (want other changes as well)
        return True,valchange
    valchange=False
    for step in steps1:
        ne1,anycollapsed1=_ncomps_and_entries(d1['steps_comps'][step])
        ne2,anycollapsed2=_ncomps_and_entries(d2['steps_comps'][step])
        if ne1!=ne2:
            print("Difference in number of components and/or number of entries in step %s!"%step)
            return False
        check=[]
        if not anycollapsed1 and not anycollapsed2:
            #awesome, we can check all comps completely before vs. after
            for comp,compdata in d1['steps_comps'][step].items():
                if comp not in d2['steps_comps'][step].keys():
                    print("Difference: Component %s only present in one input in step %s"%(comp,step))
                    return False
                check+=[(comp,compdata,d2['steps_comps'][step][comp])]
        else:
            #only check those explicitedly listed (i.e. not collapsed) in both jobs
            for comp,compdata in d1['steps_comps'][step].items():
                if comp in d2['steps_comps'][step].keys():
                    check+=[(comp,compdata,d2['steps_comps'][step][comp])]
        for comp,data1,data2 in check:
            ok,vc=compat(comp,step,data1,data2)
            if not ok:
                return False
            valchange=valchange or vc
    #todo: use wall-time data to guess significance!
    return not valchange

__allstrings={}
def __get_shared_string(s):
    global __allstrings
    return __allstrings.setdefault(s,s)

def __fs_list(z):
    i=len(z)
    while i:
        i-=1
        t=type(z[i])
        if t==str:
            z[i]=__get_shared_string(z[i])
        elif t==list:
            __fs_list(z[i])
        elif t==dict:
            __fs_dict(z[i])

def __fs_dict(d):
    keys=d.keys()
    for k in keys:
        o=d[k]
        del d[k]
        t=type(o)
        if t==str:
            o=__get_shared_string(o)
        elif t==list:
            __fs_list(o)
        elif t==dict:
            __fs_dict(o)
        d[__get_shared_string(k)]=o

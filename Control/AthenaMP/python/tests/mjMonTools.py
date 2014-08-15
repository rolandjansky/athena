# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    mpMonTools.py
# @purpose: Library for mp performance monitoring of AthenaMP
# @author:  Mous Tatarkhanov <tmmous@cern.ch>
# @date:    December 2009

__version__ = "$Revision: 276791 $"
__author__  = "Mous Tatarkhanov <tmmous@cern.ch>"

import sys
import os
import subprocess
import commands
import signal
import time
import array
import copy
    

TIME_STEP = 10
KB = (1 << 10)
MB = (1 << 20)
GB = (1 << 30)


PAGESIZE=os.sysconf("SC_PAGE_SIZE")/1024 #KiB
have_pss=0
 
mp_stat = dict() #dictionary which will hold all statistics: ProcDict objects, io-,mem-,cpu-dicts 
pid_list = list() #book-keeping of  all spawned and launched  processes for later clean-up
init_mem = list() #get_memstat()
init_numa = list() #get_numastat()

T0 = time.time()
numa_T0 = T0

def init_mp_stat():
    global mp_stat
    global pid_list
    global T0
    global init_mem
    global init_numa
    global numa_T0

    del mp_stat  #delete data from previous np
    del pid_list #delete data from previous np
    
    #brand new mp_stat
    mp_stat = dict()
    pid_list = list()
    mp_stat['io'] = dict()
    mp_stat['mem'] = dict()
    mp_stat['cpu'] = dict()
    #mp_stat['cpid'] = dict()
    mp_stat['pid'] = dict()
    mp_stat['numa'] = dict()
    mp_stat['numa']['Time'] = list()
    mp_stat['numa']['numa_hit'] = list()
    mp_stat['numa']['numa_miss'] = list()
    mp_stat['numa']['numa_foreign'] = list() 
    mp_stat['numa']['interleave_hit'] = list()
    mp_stat['numa']['local_node'] = list()
    mp_stat['numa']['other_node'] = list()
    
    T0 = time.time()
    init_mem = get_memstat()
    init_numa = list(get_numastat())
    init_numa[0] = numa_T0
    
    print "initial_mem=%s" % init_mem
    print "initial_numa=%s" % list(init_numa)
    
    
class ProcDict(dict):
    """this is to store the process information"""
    key_list = ['state', 'vmem', 'rss', 'sz', 'cpu_time', 'elap_time', 'private', 'shared']
    pid = -1
    start_time = -1
    init_time = -1
    out=""
    err=""
    elap_time = -1
    cpu_time = -1
    vmem = -1
    rss  = -1
    nbr_rows = -1 # the number of rows

    def __init__(self, pid=-1, start_time=-1, out="", err="", child = False):        
        self.pid = pid
        self.start_time = start_time
        #self['np']=np
        self.out = out
        self.err = err

        for key in self.key_list:
            self[key] = list()
        self.proc_ps_stat()
        if child:
            mp_stat["cpid"][self.pid] = self
        else:
            mp_stat["pid"][self.pid] = self
        
    def add_ps_line(self, line):
        ps_str = line.split()
        if self.start_time is -1:
            self.start_time = _seconds(ps_str[5])
        self['state'].append( ps_str[1] )
        self['vmem'].append( int(ps_str[2])/1024 )
        self['rss'].append( int(ps_str[3])/1024 )
        self['sz'].append( int(ps_str[4])/1024 )
        self['cpu_time'].append( _seconds(ps_str[6]) )
        self['elap_time'].append( _seconds(ps_str[7]) )
        
        if self.init_time==-1:
            global T0
            if grepExist(self.out, "'start processing'"):
                self.init_time = time.time()- T0
                print "pid-%i: init_time=%s"% (self.pid, self.init_time)

        private = shared = -1
        try:
            pass
            #private, shared = _get_shared_private_mem(self.pid)
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:", e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
        self["private"].append(private)
        self["shared"].append(shared)  
    
    def proc_ps_stat(self):
        """ ps statistics for this process of pid """
        out = commands.getoutput("ps --pid %i -o pid,state,vsize,rss,sz,start,cputime,etime" % self.pid)
        lines = out.splitlines()
        if len(lines) > 1:
            self.add_ps_line(lines[1])
        else:
            print "there is no process with pid: [%i]", self.pid
            return False
        return True  
    
    def children_exist(self):
        """ figures out weather the np kids were spawned for mother mpid""" 
        sc, out = commands.getstatusoutput("ps --ppid %i -o pid,start" % self.pid)
        if sc is not 0:
            #print "   children_exist: Error, sc=%i" % sc
            return False
 
        ps_lines = out.splitlines()
        nc = len(ps_lines)-1
        print "  children_exist().nbr of children = %i" % nc
        if nc > 0 :
            print "%i children workers exist. Creating ProcDicts..." % nc
            ps_lines.pop(0)
            for line in ps_lines:
                ps_str = line.split()
                cpid = int(ps_str[0])
                ProcDict(cpid, start_time = _seconds(ps_str[1]))
                print "..... child [%i] added" %  cpid
            return nc
        else:
            #print "no children exist for parent: %s " % self.pid
            return False


    def trim_stat(self): 
        """trim finished process information at the end of statistics """
        while True:
            if (self['state'][-1] in ['S', 'Z', 'T', 'D', 'X']) or (self['vmem'][-1] is 0) or (self['rss'][-1] is 0): #STOPPED, ZOMBIE, STOPPED2, Dddd, Xxxx
                for key in self.key_list:
                    self[key].pop()
            else:
                break

    def summarize(self):
        """finalize summary information."""
        self.elap_time = self['elap_time'][-1]
        self.cpu_time = self['cpu_time'][-1]
        self.vmem = self['vmem'][-1]
        self.rss = self['rss'][-1]
        self.nbr_rows = len(self['elap_time'])

class SPSummary(dict): 
    """ Sub Processes Summary - convenient way of presenting to ROOT or other output methods"""
    cpid_list=list()
    np = 1
    total_cpu_time = 0
    total_cpu_time_x = 0
    total_elap_time = 0
    total_elap_time_x = 0
    
    def __init__(self, np):
        #self['np']=list()
        self.np = np
        self['pid']=list()
        #self['elap_time']=list()
        self['elap_time_x']=list()
        #self['cpu_time']=list()
        #self['user_time']=list()
        #self['system_time']=list()
        #self['bootstrap_time']=list()
        self['cpu_time_x']=list()
        self['vmem']=list()
        self['rss']=list()
        self['free_mem_spike']=list()
        self['init_time_x'] = list()

    def extract_summary(self, dir):
        self.spid_list = mp_stat["pid"].keys()
        for pid in  mp_stat["pid"].keys():
            print "extract_summary: pid %i" % pid
            self['pid'].append(pid)
            self['init_time_x'].append(mp_stat['pid'][pid].init_time)
            self['elap_time_x'].append(mp_stat['pid'][pid].elap_time)
            self['cpu_time_x'].append(mp_stat['pid'][pid].cpu_time)
            self['vmem'].append(mp_stat['pid'][pid].vmem)
            self['rss'].append(mp_stat['pid'][pid].rss)
            self["free_mem_spike"].append( get_spike(sumList(mp_stat['mem']['kbmemfree'],
                                                            sumList(mp_stat['mem']['kbbuffers'],
                                                                mp_stat['mem']['kbcached']) )))
        for pid in self.cpid_list:
            print "  %s/%s exists ->" % (dir,pid), os.path.exists(os.path.join(dir,"%s" % pid)) #FIX: add the extraction from cpid's logs.
            out_path = os.path.join(dir,  'stdout')
            err_path = os.path.join(dir,  'stderr')

            #e_time = int(grepValue(out_path, "WORKER_ELAP_TIME"));            
            #u_time = int(grepValue(out_path, "WORKER_USER_TIME"));
            #s_time = int(grepValue(out_path, "WORKER_SYSTEM_TIME"));
            #c_time = u_time + s_time

            #self['elap_time'].append(e_time); self.total_elap_time += e_time;
            #self['user_time'].append(u_time)
            #self['system_time'].append(s_time)
            #self['cpu_time'].append(c_time); 
            #self.total_cpu_time += c_time;    
            #b_time = int(grepValue(out_path, "BOOTSTRAP_ELAP_TIME"));
            #self['bootstrap_time'].append(b_time); 
                                      
            
class MPSummary(dict):
    """ Mother Process Summary - convenient way of presenting data to ROOT..."""
    mpid = dict()
    np = 1
    def __init__(self, np):
        mpid = mp_stat['pid']
        self.np = np
        #self['np'] = list()
        #self['np'] = [ np, ]
        self["m_par_time"] = list()
        self["m_firstevent_time"]= list()
        self["m_cpu_time"]= list()
        self["m_elap_time"]= list()
        self["x_init_time"]= list()#externally observed time
        self["x_par_time"]= list() #externally observed time
        self["x_fin_time"]= list()  #externally observed time
        self["free_mem_spike"]= list()  #externally observed value
        self["swap_change"] = list()
        
    def extract_summary(self, log):
        self["m_par_time"] = [0,]# int(grepValue(log, "PARALLEL_ELAP_TIME")),]
        self['m_merging_time'] = [0,]# int(grepValue(log, "MERGING_ELAP_TIME")),]
        self["m_firstevent_time"] = [0,]# int(grepValue(log, "FIRSTEVENT_ELAP_TIME")),]
        self["m_cpu_time"] =  [ mp_stat["pid"].cpu_time, ] #mp_stat["mpid"]["cpu_time"][-1]
        self["m_elap_time"]=  [ mp_stat["pid"].elap_time, ] #mp_stat["mpid"]["elap_time"][-1]
        self["free_mem_spike"]= [ get_spike(sumList(mp_stat['mem']['kbmemfree'],  
                                            sumList(mp_stat['mem']['kbbuffers'], mp_stat['mem']['kbcached']))
                                            ), ]
        self["swap_change"] = [ get_spike(mp_stat['mem']['kbswpfree']), ]




####### useful auxillary functions ##########################
def _seconds(time_str): #handles time in "H:M:S" and "M:S" format 
    time_nums = time_str.split(":")
    if (len(time_nums)==3):
        return 3600*int(time_nums[0])+60*int(time_nums[1]) + int(time_nums[2])
    elif (len(time_nums)==2):
        return 60*int(time_nums[0]) + int(time_nums[1])
    print "ERROR: _seconds() returning - 0"
    return 0

def show_numactl():
    sc,out=commands.getstatusoutput("numactl --show")
    if sc==256:
        print "mjMonTools.show_numactl: numastat is not working! zeroes will be returned"
        return False
    else:
        print "mjMonTools.show_numactl: \n %s" % out
        return True

def get_numastat():
    sc,out=commands.getstatusoutput("numastat")
    if sc==256:
        print "mjMonTools.get_numastat: numastat is not working! zeroes will be returned"
        return (0,0,0,0,0,0,0)
    else:
        lines = out.splitlines()
        return (time.time(),
                int(lines[1].split()[1]),
                int(lines[2].split()[1]),
                int(lines[3].split()[1]),
                int(lines[4].split()[1]),
                int(lines[5].split()[1]),
                int(lines[6].split()[1])
                )
        
def save_numastat():
    current_numa = get_numastat()
    #print "current_numa=%s" % list(current_numa)

    _numa_stat = (
        mp_stat['numa']['Time'],
        mp_stat['numa']['numa_hit'],
        mp_stat['numa']['numa_miss'],
        mp_stat['numa']['numa_foreign'],
        mp_stat['numa']['interleave_hit'],
        mp_stat['numa']['local_node'],
        mp_stat['numa']['other_node']
        )
    
    change_numa = subList(current_numa,init_numa)
    print "NUMA_CHANGE=%s" % change_numa
    return [_numa_stat[i].append(change_numa[i]) for i in range(len(change_numa))]
    
def print_memstat(msg =""):
    mem = get_memstat()
    t = time.time() - T0;
    save_numastat()
    print msg + " [T=%i sec]" % t + " USED[%i Mb][change: %i Mb] - FREE[%i Mb][change: %i Mb]" % ( 
        mem["USED"], mem["USED"]-init_mem["USED"], mem["FREE"], mem["FREE"]-init_mem["FREE"])

def get_memstat():
    out=commands.getoutput("free -m")
    mem = dict()
    lines = out.splitlines()
    mem_strs = lines[1].split()
    mem['used'] = int(mem_strs[2])
    mem['free'] = int(mem_strs[3])
    mem['cached'] = int(mem_strs[5])
    mem['buffers'] = int(mem_strs[6])
    mem_strs = lines[2].split()
    mem['USED'] = int(mem_strs[2])
    mem['FREE'] = int(mem_strs[3])
    #print "mem: [%s Mbs]" %  mem
    return mem

init_mem = get_memstat()

def meanList(num_list):
    """finds average value of the number list"""
    if len(num_list) == 0:
        print "meanList: WARNING - empty list, returning 0.0"
        return 0.0
    return float(sum(num_list)) / len(num_list)
    
def sumList(l1, l2):
    """sum up values of two lists l1 + l2"""
    if len(l1) is not len(l2):
        print "sumList: WARNING: len(l1) not equals len(l2)"
        n = len(l1) if len(l2) > len(l1) else len(l2)
    else:
        n = len(l1)

    sum = list()
    for i  in xrange(n):
        sum.append(l1[i] + l2[i])
    return sum

def subList(l1, l2): 
    """subtract values of two lists: l1 - l2"""
    if len(l1) is not len(l2):
        print "subList: WARNING: len(l1) not equals len(l2)"
        n = len(l1) if len(l2) > len(l1) else len(l2)
    else:
        n = len(l1)

    sub = list()
    for i  in xrange(n):
        sub.append(l1[i] - l2[i])
    return sub
        
def get_spike(l):
    #print " get_spike:",
    #print " e0 = ",  l[0]/1024, "Mb", 
    #print " eN = ",  l[-1]/1024, "Mb",
    #print " max = ", max(l)/1024, "Mb", 
    #print " min = ", min(l)/1024, "Mb", 
    #print " e0 - eN = ",  (l[0] - l[-1])/1024, "Mb",
    #print " e0 - min = ", (l[0] - min(l))/1024, "Mb",
    #print " eN - min = ", (l[-1] - min(l))/1024, "Mb",
    #print " return  max - min =", (max(l) - min(l))/1024, "Mb"
    return max(l) - min(l)   

def prepare_mp_stat():
    """ this function construct "cp" dict from "cpid" dict for ROOT formation convenience
    converting: cpid:pid:vmem,rss,cpu-time,... -> cp:pid,vmem,rss,cputime,...  """
    mp_stat['sp'] = mp_stat['pid']
    del mp_stat['pid']
    mp_stat['pid']=dict()
    mp_stat['pid']['pid'] = list()
        
    for key in ProcDict.key_list:
        mp_stat['pid'][key]=list()
            
    for pid in mp_stat['sp'].keys():
        mp_stat['pid']['pid'] += [pid,] * mp_stat['sp'][pid].nbr_rows
        for key in ProcDict.key_list:
            mp_stat['pid'][key] += mp_stat['sp'][pid][key]
    del mp_stat['sp']


def print_summary():
    print "===== SUB PROCESS SUMMARY ====="
    for (k, v) in mp_stat['sp_summary'].items():
        print "sp_summary['%s']=%s " % (k, v)
        
################## children tools ######################
def launched_processes_working(ppid):
    """ ps statistics for children of ppid. returns False if no children exist """
    out = commands.getoutput("ps --ppid %i -o pid,state,vsize,rss,sz,start,cputime,etime" % ppid)
    ps_lines = out.splitlines()
    ps_lines.pop(0)
    
    exist = False # switch for existance of launched processes (not any processes)
    
    if len(ps_lines) > 0:
        print "Subprocesses exist:"
        for line in ps_lines:
            ps_str = line.split()
            pid = int(ps_str[0])
            #print "subprocess pid=%i" % pid
            if pid in mp_stat["pid"].keys():
                exist = True
                mp_stat["pid"][pid].add_ps_line(line)
                print "pid-%i: ps-stat appended" % pid
            else:
                print "pid-%i: secondary proc" % pid
        return exist 
    else:
        print "No subprocesses exist for parent: %i" % ppid
        return  exist #False
    return exist #False


def summarize_children_stat(): 
    """trim finished worker information at the end of statistics """
    for pid in mp_stat["cpid"].keys():
        mp_stat['cpid'][pid].trim_stat()
        mp_stat['cpid'][pid].summarize()

def summarize_proc_stat(): 
    """summarize proc finished worker information at the end of statistics """
    #mp_stat['mpid'].summarize()
    for pid in mp_stat['pid'].keys():
        #mp_stat['pid'][pid].trim_stat()
        mp_stat['pid'][pid].summarize()

def children_born(log, mpid, np):
    """ figures out weather the np kids were spawned for mother mpid""" 
    sc,out = commands.getstatusoutput("ps --ppid %i -o pid,start" % mpid)
    if sc is not 0:
        print "   mpMonTools.children_born: no kids yet... Error, sc=%i" % sc
        return False

    ps_lines = out.splitlines()
    #print "ps_lines=", ps_lines
    nc = len(ps_lines)-1
    
    print " children_exist: nbr of children = [%i]" % nc
    if grepValue(log, "FIRSTEVENT_ELAP_TIME") is None:
        return False        
    
    if nc==np : #nbr of children is equal to nbr of procs required 
        print "%i children workers forked! Registering them (creating ProcDicts) ..." % np
        ps_lines.pop(0)
        for line in ps_lines:
            ps_str = line.split()
            pid = int(ps_str[0])
            ProcDict(pid, start_time = _seconds(ps_str[1]))
            print "..... child [%i] added" %  pid
        return True
    else:
        print "no children exist for parent: %s " % mpid
    return False


################ LOG VARIABLES PROCESSING: Grepping ##################
################ LOG VARIABLES PROCESSING: Grepping ##################
def grepExist(log, field):
    """grep check for the existance of the unique field in the log 
    """
    print "grep %s %s" % (field, log),
    sc,out = commands.getstatusoutput( "grep %s %s" % (field, log))
    if sc==256:
        print " FALSE:  sc=%i" % sc
        return False
    line = out.splitlines()[0]
    print " TRUE: sc=%i \n  grepped-line=%s" % (sc,line)
    return True

def grepExist2(log, field):
    """grep check for the existance of the unique field in the log 
    """
    print "grep %s %s" % (field, log)
    sc,out = commands.getstatusoutput( "grep %s %s" % (field, log))
    if sc!=0:
        print "grepping %s in %s failed with sc=%i" % (field, log, sc) 
        return False
    line = out.splitlines()[0]
    print "grepped-line=%s" % line
    print "sc=", sc

    return True
                                                
def grepValue(log, field, sep='='):
    """grep unique field in the log and find corresponding value by regexp 
       Example: out = 'Py:EventLoopMgr      INFO EvtMax  =  123456  something'      
       grepValue(log, "EvtMax", sep="=") = '123456' 
    """
    sc,out = commands.getstatusoutput( "grep %s %s" % (field, log))
    if sc!=0:
        #print "grepping %s in %s failed" % (field, log)
        return None
    line = out.splitlines()[0]
    import re 
    vexpr = '\s*'+ sep+ '\s*(\d+)'
    m = re.search( field + vexpr, line)
    value = m.group(1)
    return value

def grepPath(log, field, sep=':'):
    """grep unique field in the log and find corresponding value by regexp 
       Example: out = 'Py:EventLoopMgr      INFO master workdir: /tmp/athena-mp-tmp-tmmous/22590-1261097934  smthng'      
       grepPath(log, "workdir", sep=":") = '/tmp/athena-mp-tmp-tmmous/22590-1261097934' 
    """
    sc,out = commands.getstatusoutput( "grep %s %s" % (field, log))
    if sc!=0:
        print "grepping %s in %s failed" % (field, log)
        return None
    line = out.splitlines()[0]
    import re 
    vexpr = '\s*'+ sep+ '\s*([^\s]+)'
    m = re.search( field + vexpr, line)
    path = m.group(1)
    return path


############# related to  athena-mp #########################
def launch_athena(jobo, ne, se, np, output_dir, numa_set=None):
    """"launching cmd: athena.py -c EvtMax=$ne $jobo  1> mp.output/stdout_$jobo.$np.$ne   2> mp.output/stderr_$jobo.$np.$ne""" 
    
    if not os.path.isdir(output_dir):
        os.mkdir(output_dir)
        
    numa_args = list()
    
    print "job command and options as template: %s" % jobo
    from string import Template
    arg_template= Template(jobo)
    arg_str = arg_template.substitute(MAXEVT=ne, SKIPEVT=se)
    proc_args = arg_str.split();
    job = proc_args[0]

    stdout_name = os.path.join(output_dir, "stdout.%s.%i.%i"  % (job,np,ne))
    stderr_name = os.path.join(output_dir, "stderr.%s.%i.%i"  % (job,np,ne))
    STDOUT_FILE = open(stdout_name , "w")
    STDERR_FILE = open(stderr_name,  "w")
    
    #proc_args = ["athena.py",  "-c", "EvtMax=%i; SkipEvents=%i" % (ne, se) ,  "../%s" % jobo]
    print "job command and options after template processing: %s" % proc_args
    
    if numa_set != None:
        numa_args = [ "numactl"]
        if (numa_set[0] == 'f' and numa_set[1] == 'f'):
            numa_args = list()
        elif ( numa_set[0]=='f' and numa_set[1]!='f'):
            numa_args.append( "--membind=%i"  % numa_set[1])
        elif ( numa_set[0]!='f' and numa_set[1]=='f'):                    
            numa_args.append( "--cpubind=%i"  % numa-set[0])
        elif (numa_set[0]!='f' and numa_set[1]!='f'):
            numa_args += ["--membind=%s" % numa_set[0], "--cpubind=%s" % numa_set[1] ]
        else:
            print "SOMETHING WRONG: numa_set=%s" % numa_set

        
    #proc_args = [   "numactl",  
    #                    "--cpubind=%i" % numa_set[0], 
    #                    "--membind=%i" % numa_set[1], 
    #                    "athena.py", "-c", "EvtMax=%i; SkipEvents=%i" % (ne, se) ,  "../%s" % jobo]
    
    proc_args = numa_args + proc_args
    
    print "<<<LAUNCH>>>: %s" % proc_args
    mproc = subprocess.Popen( proc_args, 
                             stdout=STDOUT_FILE, 
                             stderr=STDERR_FILE,
                             cwd = output_dir,
                             shell=False, 
                             close_fds = True)
    pid_list.append(mproc.pid)
    STDOUT_FILE.close();  STDERR_FILE.close()
    proc = ProcDict(mproc.pid, out=stdout_name, err = stderr_name,  child=False)
    
    return (mproc, stdout_name, stderr_name)

############# TERMINATOR AREA: stopping, killing, terminating processes ###############

def stop_proc(proc):
    """ terminate/kill a process by either proc_object or pid"""
    pid = 0
    try:
        if type(proc) is int:
            pid = proc
            os.kill(pid, signal.SIGKILL); #os.waitpid(pid, 0);
        else:
            pid = proc.pid 
            if proc.poll() is None: os.kill(pid, signal.SIGKILL); 
            proc.wait();
        print "process %s  terminated" % pid 
    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__),"  ## What:",e
        print sys.exc_info()[0], sys.exc_info()[1]
        return False
    pid_list.remove(pid)
    return True

def stop_proc_tree(pid):
    """ Terminate/kill recursively process tree by pid. Be precautious using this!"""
    out = commands.getoutput("ps --ppid %i" % pid)
    lines = out.splitlines(); lines.pop(0) #remove header
    try:
        if len(lines) > 0: 
            for line in lines:
                cpid = int(line.split()[0])
                print "child [%i:%i] being terminated..." % (pid, cpid)
                stop_proc_tree(cpid)
        if  pid in pid_list: pid_list.remove(pid) 
        os.kill(pid, signal.SIGKILL); #os.waitpid(pid, 0);
        print "[%i] - terminated." % pid 
    except Exception, e:
        print "[%i] - dead #while killing caught exception [%s] !!" % (pid, str(e.__class__)),"  ## What:",e
        #print sys.exc_info()[0], sys.exc_info()[1]
        return False
    return True


def stop_athenaMP(mproc):
    """ terminate/kill a process by either proc_object or pid"""
    try:
        pid = 0
        if type(mproc) is int:
            pid = mproc
            stop_proc_tree(pid) # killing athena-mp mproc-tree
        else:
            pid = mproc.pid;
            if mproc.poll() is None: os.kill(pid, signal.SIGKILL); 
            mproc.wait();
        print "process %s  terminated" % pid 
        return True
    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__),"  ## What:",e
        print sys.exc_info()[0], sys.exc_info()[1]
        return False
    return False




############# sar related wrappers ###########################

def launch_sar(log, time_step):
    """
      launch sar with 
     `sar -bBcdqrRuvwWy -I SUM -I XALL -n ALL -P ALL` = `sar -A`
    """
    sar_args = [ "sar", "-bBrvwu", "-o", log, "%i" % time_step, "0" ]
    print "launching: %s %s %s %s %s %s" % tuple(sar_args)    
    sc,out = commands.getstatusoutput('sar -b 1 1')
    if sc!=0:
        print 'launching failed - sar do not work on this system - please install if available!'
        return None
    FNULL = open('/dev/null', 'w')
    proc = subprocess.Popen(sar_args, 
                            executable="sar",  
                            stdout = FNULL,
                            stderr = subprocess.STDOUT,
                            shell=False, 
                            close_fds = True)
    FNULL.close()
    pid_list.append(proc.pid)
    return proc

def _num(str):
    """try converting str into int or float if fails return the same string"""
    try:
        if "." in str:
            out = float(str)
        else:
            out = int(str) 
    except ValueError:
        return str
    return out
        
def get_sar_stat(log, key):
    """ get statistics by issueing this cmd: `sar -key $log`"""
    print 'launching cmd: sar %s -f %s' % (key, log)        
    sc,out = commands.getstatusoutput("sar %s -f %s" % (key,log) )
    if sc!=0:
        print "launching failed - either file %s does not exist or sar does not work on this system - please check!" % log
        return None
    sar_dict = dict()
    #print"##################################"; print "out=\n", out; print "################################################"

    lines = out.splitlines()
    print "trim1=", lines.pop(0)#trimming output
    print "trim2=", lines.pop(0)#trimming output

    avg_line = lines.pop(); #trimming avg line at the end 
    print "avg_line1=", avg_line
    
    hstrs = lines.pop(0).replace('%', 'p').replace('/', 'p').split() #trimming header strings and replacing '%' and '/' to satisfy ROOT 
    hstrs[0] = "Time"
    print "Sar statistics fields found: ", hstrs

    #print"##################################"; print "lines=\n", lines; print "################################################"
    
    for hstr in hstrs:
        sar_dict[hstr] = list()
    for line in lines:
        lstrs = line.split()
        #print "lstrs=", lstrs
        for i,hstr in enumerate(hstrs):
            if i!=0:
                sar_dict[hstr].append( _num(lstrs[i]) )
            else:
                sar_dict[hstr].append(_seconds(lstrs[i])) #time conversion from "H:M:S" --> numero segundos
    return sar_dict 


### Tools for process statistics #######

def get_full_sar_stat(log):
    mp_stat["io"] =  get_sar_stat(log, "-b")
    mp_stat["mem"] = get_sar_stat(log, "-r")
    mp_stat["cpu"] = get_sar_stat(log, "-u")
    #return mp_stat
  
  
  ##############sysstat and other linux commands wrappers########

def _meminfo():
    out=commands.getoutput("cat /proc/meminfo")
    lines = out.splitlines()
    mem=dict()
    mem['total']= int(lines[0].split()[1]) / Kb
    mem['free'] = int(lines[1].split()[1]) / Kb
    mem['buffers']= int(lines[2].split()[1]) / Kb
    mem['cached'] = int(lines[3].split()[1]) / Kb
    print "meminfo.real_total: [%i Mb]", mem['total'] 
    print "meminfo.free: [%i Mb]", mem['free']
    print "meminfo.cached: [%i Mb]", mem['cached'] 
    print "meminfo.buffers: [%i Mb]", mem['buffers']
    return mem

def _get_iostat():
    out=commands.getoutput("iostat")
    io = dict()
    lines = out.splitlines()
    strs = lines[1].split()
    io['used'] = int(strs[2])
    mem['free'] = int(strs[3])
    mem['cached'] = int(strs[5])
    mem['buffers'] = int(strs[6])
    mem_strs = lines[2].split()
    mem['USED'] = int(strs[2])
    mem['FREE'] = int(strs[3])
    #print "mem: [%s Mbs]" %  mem
    return io
def _used_mem():
    out=commands.getoutput("free -m")
    mem_strs = out.splitlines()[2].split()
    used_mem = int(mem_strs[2]) 
    print "used_mem: [%i Mb]" % used_mem
    return used_mem            
def _free_mem():
    out=commands.getoutput("free -m")
    mem_strs = out.splitlines()[2].split()
    free_mem  = int(mem_strs[3]) 
    print "free_mem: [%i Mb]" % free_mem
    return free_mem

def _launch_iostat(log, time_step):
    print 'launching cmd: iostat $TIME_STEP -d -x > iostat.$jobo.$np.$ne &'
    sc,out = commands.getstatusoutput( "iostat" )
    if sc!=0:
        print 'launching failed - iostat do not work on this system'
        return None
    file = open(log, "w")
    iostat_proc = subprocess.Popen(
        [ "iostat",  "%i" % time_step, "-d", "-x"], 
        executable="iostat", 
        stdout = f_iostat, 
        shell=False, 
        close_fds = True)

    file.close()
    return iostat_proc  
def _launch_vmstat(log, time_step):
    print 'launching cmd: vmstat $TIME_STEP -n > vmstat.$jobo.$np.$ne &'        
    sc,out = commands.getstatusoutput( "vmstat -V" )
    if sc!=0:
        print 'launching failed - vmstat do not work on this system'
        return None
    file = open(log, "w")
    proc = subprocess.Popen([ "vmstat", "%i" % time_step, "-n" ], 
                            executable="vmstat",  
                            stdout = file, 
                            shell=False, 
                            close_fds = True)
    file.close()
    return proc
def __create_childProcDicts(ppid):
    """ creates stats dictionary with """
    out = commands.getoutput("ps --ppid %i -o pid, start" % ppid)
    ps_lines = out.splitlines()
    ps_lines.pop(0)
    
    if len(ps_lines) > 1:

        for line in ps_lines:
            ps_str = line.split()
            pid = int(ps_str[0])
            ProcDict(pid, start_time = _seconds(ps_str[1]))
            print "ppid: [%i]: child [%i] added" % (ppid, pid)
    else: 
        print "no children exist for parent: %s " % ppid


#######  adopted from AthenaMP/PyComps ###################
def print_shared_private(pid):
    print "CPROC-SHARED_PRIVATE_MEM for pid: [%i]" % pid
    for line in open("/proc/%i/status" % pid):
        if line.startswith('Vm'):
                print(line.strip())
    private,shared=_get_shared_private_mem()
    print "pid:[%i] ===> private: %s MB | shared: %s MB" % (pid, private/1024., shared /1024.)
def _get_shared_private_mem(pid='self'):
    """ Finds proc's shared and private memory size from /proc/pid/statm  and /proc/pid/smaps dir
       Coppied from AthenaMP/PyComps.py"""
    global have_pss
    private_lines=[]
    shared_lines=[]
    pss_lines=[]
    statm_name = "/proc/%s/statm" % pid
    smaps_name = "/proc/%s/smaps" % pid
    rss=int(open(statm_name).readline().split()[1])*PAGESIZE
    if os.path.exists(smaps_name): #stat
        for line in open(smaps_name).readlines(): #open
            if line.startswith("Shared"):
                shared_lines.append(line)
            elif line.startswith("Private"):
                private_lines.append(line)
            elif line.startswith("Pss"):
                have_pss=1
                pss_lines.append(line)
        shared=sum([int(line.split()[1]) for line in shared_lines])
        private=sum([int(line.split()[1]) for line in private_lines])
        #Note shared + private = rss above
        #The rss in smaps includes video card mem etc.
        if have_pss:
            pss_adjust=0.5 #add 0.5KiB as this average error due to trunctation
            Pss=sum([float(line.split()[1])+pss_adjust for line in pss_lines])
            shared = Pss - private
    elif (2,6,1) <= kv <= (2,6,9):
        shared=0 #lots of overestimation, but what can we do?
        private = rss
    else:
        shared=int(open(statm_name).readline().split()[2])
        shared*=PAGESIZE
        private = rss - shared
    return (private, shared)


#################################################
############# ROOT Output #######################

def _createRootFile(outName):
    """creating carcasus of report ROOT file"""
    print "create ROOT file..."
    from PerfMonAna.PyRootLib import importRoot
    from ROOT import TTree
    import array
    ROOT = importRoot( batch = True )
    outFile = ROOT.fopen( outName, 'RECREATE' )
    outFile.cd("/")
    
    i = array.array( 'i', [0] )
    d = array.array( 'f', [0.] )

    outFile.cd()
    tree =  TTree( "io", "IO statistics tree")
    tree.Branch('Time', i, 'int/I')
    tree.Branch('tps',d,'float/D') #transfers per second
    tree.Branch('rtps',d,'float/D') #read transfers per second
    tree.Branch('wtps',d,'float/D') #write transfers per second
    tree.Branch('breadps',d,'float/D')  #blocks read per second
    tree.Branch('bwrtnps',d,'float/D')  #blocks written per second
    tree.Write();
    
    outFile.cd()
    tree =  TTree( "mem", "Mem statistics tree")
    tree.Branch('Time', i, 'int/I')
    tree.Branch('kbmemfree', i, 'int/I') #free in kB
    tree.Branch('kbmemused', i, 'int/I') #used in kB
    tree.Branch('pmemused', d, 'float/D') #used in kB
    tree.Branch('kbbuffers', i, 'int/I') #buffers in kB
    tree.Branch('kbcached', i, 'int/I') #cached in kB
    tree.Branch('kbswpfree', i, 'int/I') #swap free in kB
    tree.Branch('kbswpused', i, 'int/I') #swap used in kB
    tree.Branch('pswpused', d, 'float/D') 
    tree.Branch('kbswpcad', i, 'int/I')
    tree.Write(); 
    
    outFile.cd()
    tree =  TTree("cpu", "CPU statistics tree")
    tree.Branch('Time', i, 'int/I')
    tree.Branch('CPU', i,'int/I')    #CPU number or ALL
    tree.Branch('puser',d,'float/D')  # CPU utilization percentage at user level
    tree.Branch('pnice',d,'float/D')  # CPU utilization at nice level
    tree.Branch('psystem',d,'float/D')  # CPU utilization at system level
    tree.Branch('piowait',d,'float/D')  # CPU idle percentage due to IO-wait
    tree.Branch('psteal',d,'float/D')  # virtual processes wait percentage
    tree.Branch('pidle',d,'float/D')  # CPU idling due to non IO reasons
    tree.Write();

    outFile.cd()
    tree =  TTree( "cp", "Children processes statistics")
    tree.Branch('Time', i, 'int/I') # start date/time
    tree.Branch('pid', i , 'int/I') # pid of worker process
    tree.Branch('state', i , 'int/I') #state of the process at the moment (important at the end of process)
    tree.Branch('vmem', i, 'int/I')
    tree.Branch('rss', i, 'int/I') 
    tree.Branch('sz', i, 'int/I') # Size in physical pages of the core image of the process. This includes text, data, and stack space.
    tree.Branch('shared', i, 'int/I')# shared memory as extracted from /proc/pid/smaps
    tree.Branch('private', i, 'int/I')# private memory as extracted from /proc/pid/smaps
    tree.Branch('cpu_time', i,'int/I')# cpu_time 
    tree.Branch('elap_time', i, 'int/I')# elapsed time (Wall Clock time)
    tree.Write()
    
    outFile.cd()
    tree =  TTree( "cp_summary", "children processes summary tree")
    tree.Branch('pid', i, 'int/I')
    tree.Branch('elap_time', i , 'int/I')
    tree.Branch('cpu_time', i, 'int/I')
    tree.Write();
    
    outFile.cd()
    tree =  TTree( "mpid", "Mother Process statistics tree")
    tree.Branch('pid', i , 'int/I')
    tree.Branch('state', i , 'int/I')
    tree.Branch('vmem', i, 'int/I')
    tree.Branch('rss', i, 'int/I')
    tree.Branch('sz', i, 'int/I')
    tree.Branch('shared', i, 'int/I')
    tree.Branch('private', i, 'int/I')
    tree.Branch('cpu_time', i,'int/I')
    tree.Branch('elap_time', i, 'int/I')
    tree.Write()

    outFile.cd()
    tree =  TTree( "mp_summary", "Mother process summary tree")
    tree.Branch('pid', i, 'int/I')
    tree.Branch('m_elap_time', i , 'int/I') #mother process total elapsed time
    tree.Branch('m_cpu_time', i, 'int/I') #mother process CPU-time (children-worker's time not included)
    tree.Branch('m_firstevent_time', i, 'int/I') #time spent on firstEvents(after init before forking) - measured inside mother process
    tree.Branch('m_par_time', i, 'int/I') #parallel time - time from forking till collecting data - measured inside mother process
    tree.Branch('x_init_time', i, 'int/I') #externally  observed MP SERIAL-INIT executin time (from start till forking) 
    tree.Branch('x_par_time', i, 'int/I') #externally observed MP PARALLEL execution time (from forking till joining)
    tree.Branch('x_fin_time', i, 'int/I') #externally observed MP FINALIZATION time (from joining till end of job)
    tree.Branch('free_mem_spike', i, 'int/I') #spike in "free+buffers+cached" at the end of MP-process -> physical used memory released.
    tree.Branch('swap_change', i, 'int/I') # change in swap, should be zero if not means MP causing a swapping 
    #tree.Branch('used_mem_spike', i, 'int/I')
    tree.Branch('event_proc_rate', i, 'float/D') #event processing rate defined as ne*60*m_par_time/np = events/proces/min
    tree.Write()

    outFile.cd()
    outFile.Write()
    outFile.Close()
    print "create ROOT file... [DONE]" 
    return

def createRootFile(outName, np):
    """creating structure of ROOT-report file from mp_stat dictionary """
    print "create ROOT file..."

    from PerfMonAna.PyRootLib import importRoot
    from ROOT import TTree
    import array
    ROOT = importRoot( batch = True )
    outFile = ROOT.fopen( outName, 'RECREATE' )
    print "ROOT.fopened"

    outFile.cd("/")
    
    i = array.array( 'i', [0] )
    d = array.array( 'f', [0.] )

    for t in mp_stat.keys():
        tree =  TTree( t, "%s stat tree" % t)
        tree.Branch('np', i, 'int/I') # each tree will have 'np' branch
        for b in mp_stat[t].keys():
            #print "tree=%s, branch=%s" % (t,b)
            if isinstance(mp_stat[t][b][0], int):
                tree.Branch(b, i, 'int/I')
            elif isinstance(mp_stat[t][b][0], float):
                tree.Branch(b, d,'float/F')
            else:
                #print "branch [%s] is not int or float type" % b
                tree.Branch(b, i, 'int/I')
        tree.Write()
    outFile.Write()
    outFile.Close()
    print "create ROOT file... [DONE]"


def fillRootTree(tree, stat, np):
    #print "writing %s statistics Tree:" % tree.GetName(),
    branches = stat.keys()
    #print "    branches=", branches, "...", 
    nbr  = len(branches)
    array_list = list()

    np_array = array.array('i', [np])
    tree.SetBranchAddress('np', np_array) #putting 'np' into each tree.
    for branch in branches:
        #print "fillRT: branch=%s" % branch
        if isinstance(stat[branch][0], float):
            f = stat[branch][0]
            nums = array.array('f', [0.0])
            array_list.append(nums)
        elif isinstance(stat[branch][0], int):
            i = stat[branch][0]
            nums =  array.array('i', [0])
            array_list.append(nums)
        else:
            #print "branch [%s] is not int or float type" % branch
            nums = array.array('i', [-1])
            array_list.append(nums)
        tree.SetBranchAddress(branch, array_list[-1]);
        
    for index in xrange(len(stat[branches[0]])):
        for array_index, branch in enumerate(branches):
            #print "stat[branch=%s][index=%i] array_index=%i " % (branch, index, array_index)
            array_list[array_index][0] = stat[branch][index] if array_list[array_index][0] is not -1 else -1
        tree.Fill()
    #print "[DONE]"



def writeRootFile(outName, np):
    """writes statistics into ROOT file"""
    print "write ROOT file %s...", outName 
    createRootFile(outName, np)
    from ROOT import TFile, TTree
    import array
    outFile = TFile( outName, 'update' )

    stat_keys = mp_stat.keys()
    #print "mp_stat.keys()", stat_keys
    for key in stat_keys:
        #print " writing [%s]" % key
        tree = outFile.Get( "%s" %   key )
        fillRootTree(tree, mp_stat[key], np)
        tree.Write()

    outFile.Write()
    outFile.Close()        
    print "write ROOT file... [DONE]" 
    return

def mergeRootFiles(file, ne):
    import glob
    file_list = glob.glob1(os.getcwd(), "%s.*.%i.root" % (file, ne) )
    import commands
    cmd = "hadd -f6 mp_stat.%s.ne%i" % (file, ne)
    for f in file_list:
        cmd = cmd + ' ' + f 
    sc, out = commands.getstatusoutput(cmd)

def mergeRootOutput(output_file, jobo, np_list, ne):
    import commands
    from ROOT import TFile, TTree
    #output_file = "merged.%s.ne%i.root" % (jobo, ne)
    cmd = "hadd -f6 %s" % output_file
    for np in np_list:
        # here we copy mp_summary and cp_summary trees in each root file from /$np dir into root dir for further merging
        file = "mj.%s.%i.%i.root" % (jobo, np, ne)
        print " ---> processing file = %s" % file
        #here we form the command for merging
        cmd = cmd + " %s" % file

    print "issueing root files merging command:[%s]" % cmd
    sc, out = commands.getstatusoutput(cmd)
    return #output_file

def _createGlobalRootFile(file, ne):
    from ROOT import TFile, TTree, Tlist
    import glob
    file_list = glob.glob1(os.getcwd(), "%s.*.%i.root" % (file, ne) )
    outFile = TFile ("%s.%i.root" % (file, ne), 'RECREATE' )    
    for f in file_list:
        print "Copying trees from [%s]" % f
        tf = TFile (f, 'READ' )
        mpt = tf.Get("mp_summary")
        cpt = tf.Get("cp_summary")
        outFile.cd('/')
        dir = "%s" % f.replace(file, "").split(".")[1]
        print "   creating dir for np = %s" % dir
        outFile.mkdir(dir) # creating dir for np
        outFile.cd(dir)
        mpTree = mpt.CloneTree(); mpTree.Write()
        cpTree = cpt.CloneTree(); cpTree.Write()
        outFile.Write()
        tf.Close()

    outFile.cd('/')
    
    ikeys = outFile.GetListOfKeys().MakeIterator()
    key = ikeys.Next()
    key_list = list()

    while key is not None:
        key_list.append(key.GetName())
    
    for np in key_list:
        outFile.Get("%s/mp_summary")


def report2(root_file, ne = 0, comments=""):
    print'  mpMonTools.report(): root_file=', root_file
    from ROOT import TFile, TTree, TBranch, TCanvas, TPad, TGraph, TLegend, TMultiGraph, gStyle, TLatex, TPaveLabel, TPaveText, TH2I, TMath

    def getTreeList(tree, column, condition):
        size = tree.Draw(column, condition,'goff'); 
        v1 = tree.GetV1(); v1.SetSize(size)
        return list(v1)

    def makeGraph(tree, name, formula, condition="", color = 1, lineWidth=1):
        tree.Draw(formula, condition, "goff")
        graph = TGraph(int(tree.GetSelectedRows()), tree.GetV2(), tree.GetV1())
        graph.SetLineColor(color);
        graph.SetLineWidth(lineWidth)
        graph.SetName(name);
        return graph

    def MakeMultiGraph(graph_data, mg, l):
        clr = 1
        gl = list()
        
        if graph_data is None:
            return []
        
        if graph_data['type'] is 'graph':
            for name, (tree, param) in graph_data['data'].items():
                clr+=1; formula =''; condition=''
                if type(param).__name__=='tuple':
                    formula = param[0]
                    condition = param[1]
                else:
                    print "MakeMG: ", formula, condition
                    formula = param
                    condition = ""

                print "name=%s, tree=%s, formula=%s, condition=%s" % (name, tree.GetName(), formula, condition) 
           
    
                #g = makeGraph(tree, name, formula, condition, color=clr)
                tree.Draw(formula, condition, "goff")
                g = TGraph(int(tree.GetSelectedRows()), tree.GetV2(), tree.GetV1()); gl.append(g)
                
                g.SetName(name); g.SetLineColor(clr); g.SetMarkerColor(clr); g.SetLineWidth(0)
                if name == "worker_rate":
                    g.SetLineColor(10)
                    
                mg.Add(g); 
                l.AddEntry(g, name)

        if graph_data['type'] is 'list':
            for name, (lx,ly) in graph_data['data'].items():
                print "name=%s" % name; print lx; print ly
                clr+=1
                g = TGraph( len(lx), array.array('f', lx), array.array('f', ly) )
                g.SetName(name); g.SetLineColor(clr); g.SetLineWidth(1); g.SetMarkerColor(clr); 
                mg.Add(g)
                l.AddEntry(g, name)
                gl.append(g)

        if graph_data['type'] is 'array':
            clr = 1
            g_list = list()
            data = arrayd['data']
            for name,(x,y) in graph_data['data'].items():
                print x; print y
                clr+=1;
                g = TGraph(len(x), x, y)
                g.SetName(name); g.SetLineColor(clr); g.SetLineWidth(1); g.SetMarkerColor(clr) 
                gl.append(g)
                mg.Add(g); 
                l.AddEntry(g, name)
        if graph_data['type'] is 'text':
            title.DrawPaveLabel(0.1,0.93,0.9,0.99, graph_data['title'], "brNDC")
            for s in graph_data['data']:
                print "graph_data['data']=%s" % s
                sp_pt.AddText(s)             
            sp_pt.SetTextAlign(12);
            sp_pt.SetTextSize(0.04)
            sp_pt.Draw()
            return []

        if graph_data['type'] is 'latex':
            title.DrawPaveLabel(0.1,0.93,0.9,0.99, graph_data['title'], "brNDC")
            tl = TLatex(); tl.SetTextSize(0.02); tl.SetTextAlign(12);
            txtd = graph_data['data']
            i = 0; x0 = 0.05; y0 = 0.90; dx = 0.08; dy = 0.05
            x1 = x0
            tl.DrawLatex(x1, y0, 'np')
            for s in txtd['np']:
                x1 = x1 + dx
                tl.DrawLatex(x1, y0, s)
            txtd.pop('np')

            for k in txtd.keys():
                y0 = y0 - dy
                tl.DrawLatex(x0-0.03, y0, k);
                x1 = x0
                for s in txtd[k]:
                    x1 = x1 + dx
                    tl.DrawLatex(x1, y0, s)
            return []
        if 'goptions' in graph_data.keys():
            mg.Draw(graph_data['goptions'])
        else:
            mg.Draw('ALP')
            
        l.Draw()
        h=mg.GetHistogram(); h.SetXTitle(graph_data['xtitle']); h.SetYTitle(graph_data['ytitle']); h.SetMinimum(0.1);
        title.DrawPaveLabel(0.1,0.91,0.9,0.99, graph_data['title'], "brNDC")
        if 'text' in graph_data.keys():
            title.DrawPaveLabel(0.2,0.88,0.8,0.92, graph_data['text'], "brNDC")
            #text_box = TPaveText(0.2,0.51,0.8,0.54);  text_box.AddText(graph_data['text']);
            #text_box.SetFillColor(42); text_box.SetTextAlign(12); text_box.SetTextfONt(40); text_box.Draw();
        
        return [] # gl #list of TGraph

    c = TCanvas("mpr", "AthenaMP-mp-scaling-charts", 10, 10, 800, 1024)
    c.SetFillColor(17);  c.SetBorderSize(1); c.cd()
 
    tfile = TFile(root_file, "READ"); print "   root compression factor = ", tfile.GetCompressionFactor()
    spSumTree = tfile.Get("sp_summary")
    #cpSumTree = tfile.Get("cp_summary")
    ioTree = tfile.Get("io")
    cpuTree = tfile.Get("cpu")
    numaTree = tfile.Get("numa")
    
    if ne is 0:
        ne = int(root_file.split('.')[-2].replace('ne', ''))
        print "extracted ne=[%i]" % ne

##### FORMING THE DATA FOR ROOT Graphing-Charting-Histogramming #####    
    np_list = list(set(getTreeList(spSumTree, 'np', ''))); np_list.sort() #uniqeify and sort np_list
    elap_time_stdev = list()
    elap_time_avg = list()
    elap_time_max = list()
    cpu_time_stdev = list()
    cpu_time_max = list()
    cpu_time_avg = list()

    total_rate = list()
    elap_time_rate = list()
    cpu_time_rate = list()
    user_time_rate = list()
    par_event_rate = list()

    elap_cpu_time = list() # elap - cpu time avg.

    elap_time_stdev_x = list()
    cpu_time_stdev_x = list()
    par_elap_time = list() # elap time as seen from mother
    
    sp_lb = [b.GetName() for b in list(spSumTree.GetListOfBranches())]
    #sp_lb = [b.GetName() for b in list(spSumTree.GetListOfBranches())]

    #mp_txt = "%s" % mp_lb + "\n"
    sp_txt = "%s" % sp_lb + "\n" 
    
    sp_pt = TPaveText(0.1,0.1,0.9,0.9) 
    sp_pt.SetFillColor(22)
    
    sp_latex = TLatex()
    sp_latex.SetTextAlign(12)

    txt_dict=dict()
    ltxt_dict = dict()
    for s in sp_lb:
        txt_dict[s] = "%20s" % s
        ltxt_dict[s] = list()
    np_txt = ""
    for np in np_list:
        size = spSumTree.Draw('elap_time_x:cpu_time_x:elap_time_x-cpu_time_x', "np==%i" % int(np), 'goff'); 
        elapv = spSumTree.GetV1(); 
        cpuv = spSumTree.GetV2();
        elap_cpuv = spSumTree.GetV3();
        #userv = spSumTree.GetV4();

        elap_time_stdev.append(float(TMath.RMS(size, elapv )))
        elap_time_avg.append(float(TMath.Mean(size, elapv )))
        elap_time_max.append(float(TMath.MaxElement(size, elapv )))

        cpu_time_stdev.append(float(TMath.RMS(size, cpuv )))
        cpu_time_avg.append(float(TMath.Mean(size, cpuv )))
        cpu_time_max.append(float(TMath.MaxElement(size, cpuv )))
        
        elap_cpu_time.append(float(TMath.Mean(size, elap_cpuv)))
        
        #elap_time_rate.append( float(60*ne)/float(np*elap_time_avg[-1]) )
        #cpu_time_rate.append( float(60*ne)/float(np*cpu_time_avg[-1]) )
        #user_time_rate.append( float(60*ne)/float( np * float(TMath.Mean(size, userv))) ) 

        elap_time_rate.append( float(60*ne)/float(elap_time_avg[-1]) )
        cpu_time_rate.append( float(60*ne)/float(cpu_time_avg[-1]) )
        total_rate.append(np * elap_time_rate[-1])
                
        #user_time_rate.append( float(60*ne)/float(float(TMath.Mean(size, userv))) ) 

        #elap_time_stdev_x.append(float(TMath.RMS(size, cpSumTree.GetV2())))
        #cpu_time_stdev_x.append(float(TMath.RMS(size,  cpSumTree.GetV4())))
        
        #msize = mpSumTree.Draw('m_par_time', "np==%i" % int(np), 'goff')
        #parv = mpSumTree.GetV1();
        #par_elap_time.append(float(TMath.Mean(msize, parv)))
        #par_event_rate.append(float(60.0*ne)/par_elap_time[-1])

        np_txt += "%10s" % np
        for s in sp_lb:
            txt_dict[s] += "%10.1f" % getTreeList(spSumTree, s, "np==%i" % int(np) )[0]
            ltxt_dict[s].append( "%10.1f" % getTreeList(spSumTree, s, "np==%i" % int(np))[0] )

    print "np_list=%s\n etime_stdev=%s \n cpu_time_stdev=%s" % (np_list, elap_time_stdev, cpu_time_stdev)
    print "elap-cpu=%s" % (elap_cpu_time)

    from socket import gethostname
    import platform
#########################
    graph_list = list()
###########################
    graph =dict()
    graph['type'] = 'text'
    graph['data'] = [
                "MP Times, Memory, IO, CPU PLOTS for ",
                " %s " % root_file,
                " machine: %s" % gethostname(),
                #"%s, %s, %s, %s, %s, %s" % platform.uname(),
                "%s" % platform.platform(),
                "%s" % os.getenv('CMTCONFIG'),
                " comments: %s" % comments,
                "np=%s " % [int(s) for s in np_list]
                ]
    graph['title'] = 'ATHENA MP MONITOR mpMon REPORT'
    graph_list.append(graph)
############################
    graph  = dict()
    graph['type'] = 'latex'
    graph['data'] = ltxt_dict 
    graph['title']= "sp_summary numbers:"
    graph_list.append(graph)
############################
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     #'parallel_elap': (np_list, par_elap_time),
                     'proc_elap_avg': (np_list, elap_time_avg),
                     'proc_elap_max': (np_list, elap_time_max),
                     'proc_cpu_avg':  (np_list, cpu_time_avg),
                     'proc_cpu_max':  (np_list, cpu_time_max),
                     #'elap_time_stdev_x': (np_list, elap_time_stdev_x),
                     #'cpu_time_stdev_x':  (np_list, cpu_time_stdev_x)
                     }
    graph['title'] = "Processes Elap and CPU Time Variations"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Time, sec" 
    graph_list.append(graph)

############################
    graph =dict()
    graph['type'] = 'text'
    graph['data'] = [
                "This plot intentially left blank"
                ]
    graph['title'] = 'BLANK CHART'
    graph_list.append(graph)
    
    """
    graph  = dict()
    graph['type'] = 'graph'
    graph['data'] = {
            "m_elap_time" : (mpSumTree, "m_elap_time:np"),
            "m_firstevent_time": (mpSumTree, "m_firstevent_time:np"),
            "m_par_time" :    (mpSumTree, "m_par_time:np"),
            "m_merging_time": (mpSumTree, "m_merging_time:np")
                   }
    graph['title'] = "PARENT PROCESS TIMES"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Time, sec"
    graph_list.append(graph)
    
##############################
    graph =dict()
    graph['type'] = 'text'
    graph['data'] = [
                "This plot intentially left blank"
                ]
    graph['title'] = 'BLANK CHART'
    graph_list.append(graph)
    """
############################
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     'elap_time_stdev': (np_list, elap_time_stdev),
                     'cpu_time_stdev':  (np_list, cpu_time_stdev),
                     'elap-cpu_avg':  (np_list, elap_cpu_time),
                     }
    graph['title'] = "Processes Time Deviations"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Time_StDev, sec" 
    graph_list.append(graph)

############################
    """ 
    graph  = dict()
    graph['type'] = 'graph'
    graph['data'] = {
            #"event_rate": (mpSumTree, "event_rate:np"),
            #"event_rate_x": (mpSumTree, "(60*%i)/(x_par_time):np" % ne),
            #"event_rate" :  (mpSumTree, "(60*%i)/(m_par_time):np" % ne),
            #"per_proc_rate": (mpSumTree, "event_proc_rate:np") 
            "event_rate" :  (mpSumTree, "(60*%i*np)/(m_par_time):np" % ne),
            "per_proc_rate": (mpSumTree, "event_proc_rate*np:np")
            #"per_proc_rate_x": (mpSumTree, "(60*%i)/(x_par_time*np):np" % ne)
                     }
    graph['title'] = "ATHENA MP EVENT PROCESSING RATE"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Evts/min, Evts/proc/min"
    graph_list.append(graph)
    """

############################
    
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     'total_rate': (np_list, total_rate),
                     'elap_avg_rate': (np_list, elap_time_rate),
                     'cpu_avg_rate':  (np_list, cpu_time_rate)
                    #'user_avg_rate': (np_list, user_time_rate)
                     }
    graph['title'] = "Worker event processing rates Wall-Time, User-CPU, Total-CPU time rates, averaged."
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph_list.append(graph)

############################
    graph = dict()
    graph['type'] = 'graph'
    graph['data'] = {
                     #'parallel_rate': (mpSumTree, "event_rate:np"), #"(event_rate + 207/(30*(np-2)+m_par_time)):np"),
                     'worker_rate': (spSumTree, "(60*%i)/(elap_time_x):np" % ne),
                     'cpu_rate':  (spSumTree, "(60*%i)/(cpu_time_x):np" % ne),
                     #'user_rate': (spSumTree, "(60*%i)/(user_time_x):np" % ne)
                     }
    graph['title'] = "Event Throughput per Process, wall-clock time"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph['goptions'] = "ALP"
    graph_list.append(graph)
############################
    
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     #'par_event_rate': (np_list, par_event_rate),
                     'elap_avg_rate': (np_list, elap_time_rate),
                     'cpu_avg_rate':  (np_list, cpu_time_rate),
                    #'user_avg_rate': (np_list, user_time_rate)
                     }
    graph['title'] = "Worker event processing rates Wall-Time, User-CPU, Total-CPU time rates, averaged."
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph_list.append(graph)

#############################
    graph = dict()
    graph['type'] = 'graph'
    graph['data'] ={
        "total_mem": (spSumTree, "free_mem_spike/1024:np"),
        "mem_per_proc": (spSumTree, "free_mem_spike/np/1024:np"),
        }
    graph['title'] = "PHYSICAL MEMORY CONSUMPTION BY Athena MJ (Multi Jobs)"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Memory Consumption, Mb"
    graph_list.append(graph)

    cond = "vmem!=0"
#############################
    graph = dict()
    graph['type'] = 'graph'
    graph['data'] ={
        "mem_per_proc": (spSumTree, "free_mem_spike/np/1024:np"),
        "vmem": (spSumTree, ("vmem:np", cond) ),
        "rss":  (spSumTree, ("rss:np",  cond) )
        }
    graph['title'] = "VMEM, RSS, RealMemory(from free-spike) per Proc"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Memory Consumption, Mb"
    graph_list.append(graph)

############################
    cond = "np>0"
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        "bread/sec": (ioTree,  ('breadps:Time', cond) ),
        "bwrite/sec": (ioTree, ('bwrtnps:Time', cond) )
        }
    graph['title'] = "IO Activity for Athena MJ (Multi Jobs)  %s " % cond
    graph['xtitle']= "Time"
    graph['ytitle']= "Total Amount of Data R/W in blocks per sec"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)  
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        "write_reqs/sec": (ioTree, ('wtps:Time', cond) ),
        "read_reqs/sec":  (ioTree, ('rtps:Time', cond) ),
        "total_reqs/sec": (ioTree, ('tps:Time',  cond) )
        }
    graph['title'] = "IO Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Transfer_requests/sec"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['data'] ={
        '%_user' :   (cpuTree, ('puser:Time',   cond) ),
        '%_system':  (cpuTree, ('psystem:Time', cond) ),
        '%_idle':    (cpuTree, ('pidle:Time',   cond) ),
        '%_io_wait': (cpuTree, ('piowait:Time', cond) )
        }
    graph['title'] = "CPU Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Percentage of CPU Utilization"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
############################  
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        '%_io_wait': (cpuTree, ('piowait:Time', cond) ),
        }
    graph['title'] = "CPU Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Percentage of CPU Utilization"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
#########################
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        'numa_hit': (numaTree, ('numa_hit:Time', cond) ),
        'numa_miss':(numaTree,('numa_miss:Time', cond) ),
        'numa_foreign': (numaTree, ('numa_foreign:Time', cond) ),
        'interleave_hit': (numaTree, ('interleave_hit:Time', cond) ),
        'local_node': (numaTree, ('local_node:Time', cond) ),
        'other_node': (numaTree, ('other_node:Time', cond) ),
        }
    graph['title'] = "NUMA Activity for athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Nbr of hits/misses"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)



    pads = list()

    cpp = 1 #charts per pad 
    ppc = 2 #pads per canvas

    c.cd()
    gStyle.SetOptStat(0);
    gStyle.SetPalette(1);
    gStyle.SetCanvasColor(33);
    gStyle.SetFrameFillColor(10);
    gStyle.SetMarkerStyle(21)
    gStyle.SetMarkerColor(2)
    gStyle.SetMarkerSize(0.4)
    print "gStyle.Set done"

    title = TPaveLabel(0.1,0.98,0.9,1, "Athena MP Plots");
    title.SetFillColor(42); title.SetTextFont(40); 
    #title.Draw();print "title Drawn"

    mgs =  list()  #List of TMultiGraphs
    ls =   list()  #List of TLegends
    gs =   list()  #List of TGraph

    for j in xrange(ppc):
        y_factor = 0.99;   x1 = 0.01; x2 = 0.99;  y1 = y_factor - (y_factor-0.01)*(j+1)/float(ppc); y2 = y_factor - (y_factor-0.01)*j/float(ppc)
        print "x1,y1,x2,y2",  x1, y1, x2, y2 
        pad = TPad("pad%i" % j, "pad%i" % j,   x1, y1, x2, y2,   33); pad.Draw()
        pads.append(pad);
    
    num_cans = len(graph_list) /(cpp*ppc) if len(graph_list) % (cpp*ppc)==0 else len(graph_list)/(cpp*ppc) + 1 
    graph_list += [None,]* (num_cans*cpp*ppc - len(graph_list))
    print "number of pages/canvases in report = ", num_cans
    
    pdf_file = root_file
    for s in ['merged.', '.py', '.root']:
        pdf_file = pdf_file.replace(s, '')
    pdf_file ="%s.pdf" % pdf_file

    for i in xrange(num_cans):
        for j in xrange(ppc):
            graph = graph_list[ppc*i+j]
            if graph is None:
                continue
            
            if 'logY' in graph.keys():
                if graph['logY']:
                    pads[j].SetLogy()
            else:
                pads[j].SetLogy(0)
            
            pads[j].cd()
            pads[j].SetRightMargin(0.2)
            l = TLegend(0.82,0.20,0.99,0.89); ls.append(l) 
            mg = TMultiGraph(); mgs.append(mg)
            print "graph=", graph
            gs.append(MakeMultiGraph(graph, mg, l))

        c.Update()
        if i == 0:
            print "pdf.start"
            c.Print(pdf_file+'(', 'pdf') #start page
        elif i < num_cans-1:
            print "pdf.body"
            c.Print(pdf_file, 'pdf')    #body pages
        else:
            print "pdf.end"
            c.Print(pdf_file + ')', 'pdf') #end page
        c.SaveAs("%s.%i.png" % (pdf_file, i))
        for pad in pads:
            pad.Clear()

def report(root_file, ne = 0, comments=""):
    print'  mpMonTools.report(): root_file=', root_file
    from ROOT import TFile, TTree, TBranch, TCanvas, TPad, TGraph, TLegend, TMultiGraph, gStyle, TLatex, TPaveLabel, TPaveText, TH2I, TMath

    def getTreeList(tree, column, condition):
        size = tree.Draw(column, condition,'goff'); 
        v1 = tree.GetV1(); v1.SetSize(size)
        return list(v1)

    def makeGraph(tree, name, formula, condition="", color = 1, lineWidth=1):
        tree.Draw(formula, condition, "goff")
        graph = TGraph(int(tree.GetSelectedRows()), tree.GetV2(), tree.GetV1())
        graph.SetLineColor(color);
        graph.SetLineWidth(lineWidth)
        graph.SetName(name);
        return graph

    def MakeMultiGraph(graph_data, mg, l):
        clr = 1
        gl = list()
        
        if graph_data is None:
            return []
        line_blank = 1
        if 'noline' in graph_data.keys(): 
            line_blank=0
            
        if graph_data['type'] is 'graph':
            for name, (tree, param) in graph_data['data'].items():
                clr+=1; formula =''; condition=''
                if type(param).__name__=='tuple':
                    formula = param[0]
                    condition = param[1]
                else:
                    print "MakeMG: ", formula, condition
                    formula = param
                    condition = ""

                print "name=%s, tree=%s, formula=%s, condition=%s" % (name, tree.GetName(), formula, condition) 
           
                tree.Draw(formula, condition, "goff")
                
                selection_size = tree.GetSelectedRows()
                if selection_size==-1:
                    print "-> SKIPPED (DO NOT EXIST): SELECTION_SIZE=%i" % selection_size 
                    continue
                else:
                    print "-> SELECTION_SIZE=%i" % selection_size 
                    pass

                g = TGraph(selection_size, tree.GetV2(), tree.GetV1()); gl.append(g)
                
                g.SetName(name); g.SetLineColor(clr*line_blank); g.SetMarkerColor(clr); g.SetLineWidth(0)
                #if "rate" in name:
                #    g.SetLineColor(0)
                    
                mg.Add(g); 
                l.AddEntry(g, name)

        if graph_data['type'] is 'list':
            for name, (lx,ly) in graph_data['data'].items():
                print "name=%s" % name; print lx; print ly
                clr+=1
                g = TGraph( len(lx), array.array('f', lx), array.array('f', ly) )
                g.SetName(name); g.SetLineColor(clr*line_blank); g.SetLineWidth(1); g.SetMarkerColor(clr); 
                mg.Add(g)
                l.AddEntry(g, name)
                gl.append(g)

        if graph_data['type'] is 'array':
            clr = 1
            g_list = list()
            data = arrayd['data']
            for name,(x,y) in graph_data['data'].items():
                print x; print y
                clr+=1;
                g = TGraph(len(x), x, y)
                g.SetName(name); g.SetLineColor(clr*line_blank); g.SetLineWidth(1); g.SetMarkerColor(clr) 
                gl.append(g)
                mg.Add(g); 
                l.AddEntry(g, name)
        if graph_data['type'] is 'text':
            title.DrawPaveLabel(0.1,0.93,0.9,0.99, graph_data['title'], "brNDC")
            for s in graph_data['data']:
                print "graph_data['data']=%s" % s
                sp_pt.AddText(s)             
            sp_pt.SetTextAlign(12);
            sp_pt.SetTextSize(0.04)
            sp_pt.Draw()
            return []

        if graph_data['type'] is 'latex':
            title.DrawPaveLabel(0.1,0.93,0.9,0.99, graph_data['title'], "brNDC")
            tl = TLatex(); tl.SetTextSize(0.02); tl.SetTextAlign(12);
            txtd = graph_data['data']
            i = 0; x0 = 0.05; y0 = 0.90; dx = 0.08; dy = 0.05
            x1 = x0
            tl.DrawLatex(x1, y0, 'np')
            for s in txtd['np']:
                x1 = x1 + dx
                tl.DrawLatex(x1, y0, s)
            txtd.pop('np')

            for k in txtd.keys():
                y0 = y0 - dy
                tl.DrawLatex(x0-0.03, y0, k);
                x1 = x0
                for s in txtd[k]:
                    x1 = x1 + dx
                    tl.DrawLatex(x1, y0, s)
            return []
        if 'goptions' in graph_data.keys():
            mg.Draw(graph_data['goptions'])
        else:
            mg.Draw('ALP')
            
        l.Draw()
        h=mg.GetHistogram(); h.SetXTitle(graph_data['xtitle']); h.SetYTitle(graph_data['ytitle']); h.SetMinimum(0.1);
        title.DrawPaveLabel(0.1,0.91,0.9,0.99, graph_data['title'], "brNDC")
        if 'text' in graph_data.keys():
            title.DrawPaveLabel(0.2,0.88,0.8,0.92, graph_data['text'], "brNDC")
            #text_box = TPaveText(0.2,0.51,0.8,0.54);  text_box.AddText(graph_data['text']);
            #text_box.SetFillColor(0); text_box.SetTextAlign(12); text_box.SetTextfONt(40); text_box.Draw();
        
        return [] # gl #list of TGraph

    c = TCanvas("mpr", "AthenaMJ-mp-scaling-charts", 1, 1, 800, 1024)
    c.SetFillColor(0);  c.SetBorderSize(1); c.cd()
 
    tfile = TFile(root_file, "READ"); print "   root compression factor = ", tfile.GetCompressionFactor()
    spSumTree = tfile.Get("sp_summary")
    #cpSumTree = tfile.Get("cp_summary")
    ioTree = tfile.Get("io")
    cpuTree = tfile.Get("cpu")
    numaTree = tfile.Get("numa")
    memTree = tfile.Get("mem")

    if ne is 0:
        ne = int(root_file.split('.')[-2].replace('ne', ''))
        print "extracted ne=[%i]" % ne

##### FORMING THE DATA FOR ROOT Graphing-Charting-Histogramming #####    
    np_list = list(set(getTreeList(spSumTree, 'np', ''))); np_list.sort() #uniqeify and sort np_list
    elap_time_stdev = list()
    elap_time_avg = list()
    elap_time_max = list()
    cpu_time_stdev = list()
    cpu_time_max = list()
    cpu_time_avg = list()

    total_rate = list()
    elap_time_rate = list()
    cpu_time_rate = list()
    user_time_rate = list()
    par_event_rate = list()

    elap_cpu_time = list() # elap - cpu time avg.

    elap_time_stdev_x = list()
    cpu_time_stdev_x = list()
    par_elap_time = list() # elap time as seen from mother
    
    sp_lb = [b.GetName() for b in list(spSumTree.GetListOfBranches())]
    #sp_lb = [b.GetName() for b in list(spSumTree.GetListOfBranches())]

    #mp_txt = "%s" % mp_lb + "\n"
    sp_txt = "%s" % sp_lb + "\n" 
    
    sp_pt = TPaveText(0.1,0.1,0.9,0.9) 
    sp_pt.SetFillColor(0)
    
    sp_latex = TLatex()
    sp_latex.SetTextAlign(12)

    txt_dict=dict()
    ltxt_dict = dict()
    for s in sp_lb:
        txt_dict[s] = "%20s" % s
        ltxt_dict[s] = list()
    ltxt_dict["total_rate"] = list()
    ltxt_dict["proc_rate_avg"] = list()
    
    np_txt = ""
    for np in np_list:
        size = spSumTree.Draw('(elap_time_x-init_time_x):cpu_time_x:elap_time_x-cpu_time_x', "np==%i" % int(np), 'goff'); 
        elapv = spSumTree.GetV1(); 
        cpuv = spSumTree.GetV2();
        elap_cpuv = spSumTree.GetV3();
        #userv = spSumTree.GetV4();

        elap_time_stdev.append(float(TMath.RMS(size, elapv )))
        elap_time_avg.append(float(TMath.Mean(size, elapv )))
        elap_time_max.append(float(TMath.MaxElement(size, elapv )))

        cpu_time_stdev.append(float(TMath.RMS(size, cpuv )))
        cpu_time_avg.append(float(TMath.Mean(size, cpuv )))
        cpu_time_max.append(float(TMath.MaxElement(size, cpuv )))
        
        elap_cpu_time.append(float(TMath.Mean(size, elap_cpuv)))
        
        #elap_time_rate.append( float(60*ne)/float(np*elap_time_avg[-1]) )
        #cpu_time_rate.append( float(60*ne)/float(np*cpu_time_avg[-1]) )
        #user_time_rate.append( float(60*ne)/float( np * float(TMath.Mean(size, userv))) ) 

        elap_time_rate.append( float(60*ne)/float(elap_time_avg[-1]) )
        cpu_time_rate.append( float(60*ne)/float(cpu_time_avg[-1]) )
        total_rate.append(np * elap_time_rate[-1])
                
        #user_time_rate.append( float(60*ne)/float(float(TMath.Mean(size, userv))) ) 

        #elap_time_stdev_x.append(float(TMath.RMS(size, cpSumTree.GetV2())))
        #cpu_time_stdev_x.append(float(TMath.RMS(size,  cpSumTree.GetV4())))
        
        #msize = mpSumTree.Draw('m_par_time', "np==%i" % int(np), 'goff')
        #parv = mpSumTree.GetV1();
        #par_elap_time.append(float(TMath.Mean(msize, parv)))
        #par_event_rate.append(float(60.0*ne)/par_elap_time[-1])

        np_txt += "%10s" % np
        for s in sp_lb:
            gtl = getTreeList(spSumTree, s, "np==%i" % int(np) )
            print "%s: getTreeList: %s" % (s,gtl), 
            gtl_avg = meanList(gtl)
            print " avg=%10.1f" % gtl_avg
            txt_dict[s] += "%10.1f" % gtl_avg
            ltxt_dict[s].append( "%10.1f" % gtl_avg)
        ltxt_dict["total_rate"].append("%10.1f" % 
                ( 60.0*float(np)*float(ne)/( float(ltxt_dict["elap_time_x"][-1]) - float(ltxt_dict["init_time_x"][-1]) ) ) )
        ltxt_dict["proc_rate_avg"].append("%10.1f" % 
                ( 60.0*float(ne)/( float(ltxt_dict["elap_time_x"][-1]) - float(ltxt_dict["init_time_x"][-1]) ) ) )
        
    print "np_list=%s\n etime_stdev=%s \n cpu_time_stdev=%s" % (np_list, elap_time_stdev, cpu_time_stdev)
    print "elap-cpu=%s" % (elap_cpu_time)

    from socket import gethostname
    import platform
    graph_list = list()
###########################
    graph =dict()
    graph['type'] = 'text'
    graph['data'] = [
                "MJ Times, Memory, IO, CPU PLOTS for ",
                " %s " % root_file,
                " machine: %s" % gethostname(),
                #"%s, %s, %s, %s, %s, %s" % platform.uname(),
                "%s" % platform.platform(),
                "%s" % os.getenv('CMTCONFIG'),
                " comments: %s" % comments,
                "np=%s " % [int(s) for s in np_list]
                ]
    graph['title'] = 'ATHENA MJ MONITOR mjMon REPORT'
    graph_list.append(graph)
############################
    graph  = dict()
    graph['type'] = 'latex'
    graph['data'] = ltxt_dict 
    graph['title']= "mj_summary numbers:"
    graph_list.append(graph)

#########################
    
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     'total_rate': (np_list, total_rate),
                    #'elap_avg_rate': (np_list, elap_time_rate),
                    #'cpu_avg_rate':  (np_list, cpu_time_rate)
                    #'user_avg_rate': (np_list, user_time_rate)
                     }
    graph['title'] = "Total Event processing rate, averaged from Wall-Time rates."
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph_list.append(graph)

############################
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     #'parallel_elap': (np_list, par_elap_time),
                     'proc_elap_avg': (np_list, elap_time_avg),
                     'proc_elap_max': (np_list, elap_time_max),
                     'proc_cpu_avg':  (np_list, cpu_time_avg),
                     'proc_cpu_max':  (np_list, cpu_time_max),
                     #'elap_time_stdev_x': (np_list, elap_time_stdev_x),
                     #'cpu_time_stdev_x':  (np_list, cpu_time_stdev_x)
                     }
    graph['title'] = "Processes Elap and CPU Time Variations"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Time, sec" 
    graph_list.append(graph)

############################
    graph =dict()
    graph['type'] = 'text'
    graph['data'] = [
                "This plot intentially left blank"
                ]
    graph['title'] = 'BLANK CHART'
    graph_list.append(graph)
    
############################
    graph = dict()
    graph['type'] = 'list'
    graph['data'] = {
                     'elap_time_stdev': (np_list, elap_time_stdev),
                     'cpu_time_stdev':  (np_list, cpu_time_stdev),
                     'elap-cpu_avg':  (np_list, elap_cpu_time),
                     }
    graph['title'] = "Processes Time Deviations"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Time_StDev, sec" 
    graph_list.append(graph)

############################
    
    graph = dict()
    graph['type'] = 'list'
    graph['noline']=0
    graph['data'] = {
                     'total_rate': (np_list, total_rate),
                     'elap_avg_rate': (np_list, elap_time_rate),
                     'cpu_avg_rate':  (np_list, cpu_time_rate)
                    #'user_avg_rate': (np_list, user_time_rate)
                     }
    graph['title'] = "Worker event processing rates Wall-Time, User-CPU, Total-CPU time rates, averaged."
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph_list.append(graph)

############################
    graph = dict()
    graph['type'] = 'graph'
    graph['noline']=0
    graph['data'] = {
                     #'parallel_rate': (mpSumTree, "event_rate:np"), #"(event_rate + 207/(30*(np-2)+m_par_time)):np"),
                     'worker_rate': (spSumTree, "(60*%i)/(elap_time_x):np" % ne),
                     'worker_rate_': (spSumTree, "(60*%i)/(elap_time_x-200):np" % ne),
                     'cpu_rate':  (spSumTree, "(60*%i)/(cpu_time_x):np" % ne),
                     #'user_rate': (spSumTree, "(60*%i)/(user_time_x):np" % ne)
                     }
    graph['title'] = "Event Throughput per Process, wall-clock time"
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph['goptions'] = "ALP"
    graph_list.append(graph)
############################
    
    graph = dict()
    graph['type'] = 'list'
    graph['noline']=0
    graph['data'] = {
                     #'par_event_rate': (np_list, par_event_rate),
                     'elap_avg_rate': (np_list, elap_time_rate),
                     'cpu_avg_rate':  (np_list, cpu_time_rate),
                    #'user_avg_rate': (np_list, user_time_rate)
                     }
    graph['title'] = "Worker event processing rates Wall-Time, User-CPU, Total-CPU time rates, averaged."
    graph['xtitle'] = "NbrProc"
    graph['ytitle'] = "Evts/Proc/Min" 
    graph_list.append(graph)

#############################
    graph = dict()
    graph['type'] = 'graph'
    graph['noline']=0
    graph['data'] ={
        "total_mem": (spSumTree, "free_mem_spike:np"),
        "mem_per_proc": (spSumTree, "free_mem_spike/np:np"),
        }
    graph['title'] = "PHYSICAL MEMORY CONSUMPTION BY Athena MJ (Multi Jobs)"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Memory Consumption, Mb"
    graph_list.append(graph)

    cond = "vmem!=0"
#############################
    graph = dict()
    graph['type'] = 'graph'
    graph['noline']=0
    graph['data'] ={
        "mem_per_proc": (spSumTree, "free_mem_spike/np:np"),
        "vmem": (spSumTree, ("vmem:np", cond) ),
        "rss":  (spSumTree, ("rss:np",  cond) )
        }
    graph['title'] = "VMEM, RSS, RealMemory(from free-spike) per Proc"
    graph['xtitle']= "Nbr of Processes"
    graph['ytitle']= "Memory Consumption, Mb"
    graph_list.append(graph)

############################  
    cond="np>0"
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = False
    graph['data'] ={
        'free_mem': (memTree, ('kbmemfree/1024:Time', cond) ),
        'used_mem': (memTree, ('kbmemused/1024:Time', cond) ),
        'cached_mem': (memTree, ('kbcached/1024:Time', cond) ),
        'buffers_mem': (memTree, ('kbbuffers/1024:Time', cond) ),
        'kbswpused': (memTree, ('kbswapused/1024:Time', cond) )
        #'commit_mem': (memTree, ('kbcommit/1024:Time', cond) )
        }
    graph['title'] = "Memory Activity for Athena MJ %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Memory, Mb "
    #graph['text']= "np = %s" % np_list
    graph_list.append(graph)
############################
    cond = "np>0"
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        "bread/sec": (ioTree,  ('breadps:Time', cond) ),
        "bwrite/sec": (ioTree, ('bwrtnps:Time', cond) )
        }
    graph['title'] = "IO Activity for Athena MJ (Multi Jobs)  %s " % cond
    graph['xtitle']= "Time"
    graph['ytitle']= "Total Amount of Data R/W in blocks per sec"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)  
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        "write_reqs/sec": (ioTree, ('wtps:Time', cond) ),
        "read_reqs/sec":  (ioTree, ('rtps:Time', cond) ),
        "total_reqs/sec": (ioTree, ('tps:Time',  cond) )
        }
    graph['title'] = "IO Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Transfer_requests/sec"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
############################    
    graph = dict()
    graph['type'] = 'graph'
    graph['data'] ={
        '%_user' :   (cpuTree, ('puser:Time',   cond) ),
        '%_system':  (cpuTree, ('psystem:Time', cond) ),
        '%_idle':    (cpuTree, ('pidle:Time',   cond) ),
        '%_io_wait': (cpuTree, ('piowait:Time', cond) )
        }
    graph['title'] = "CPU Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Percentage of CPU Utilization"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
############################  
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        '%_io_wait': (cpuTree, ('piowait:Time', cond) ),
        }
    graph['title'] = "CPU Activity for Athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Percentage of CPU Utilization"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)
#########################
    graph = dict()
    graph['type'] = 'graph'
    graph['logY'] = True
    graph['data'] ={
        'numa_hit': (numaTree, 'numa_hit:Time' ),
        'numa_miss':(numaTree,'numa_miss:Time' ),
        'numa_foreign': (numaTree, 'numa_foreign:Time' ),
        'interleave_hit': (numaTree, 'interleave_hit:Time' ),
        'local_node': (numaTree, 'local_node:Time' ),
        'other_node': (numaTree, 'other_node:Time')
        }
    graph['title'] = "NUMA Activity for athena MJ (Multi Jobs) %s, Logscale" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Nbr of hits/misses"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)

#########################
    graph = dict()
    graph['type'] = 'graph'
    #graph['logY'] = False
    graph['data'] ={
        'numa_hit': (numaTree, 'numa_hit:Time' ),
        'numa_miss':(numaTree,'numa_miss:Time' ),
        'numa_foreign': (numaTree, 'numa_foreign:Time' ),
        'interleave_hit': (numaTree, 'interleave_hit:Time' ),
        'local_node': (numaTree, 'local_node:Time' ),
        'other_node': (numaTree, 'other_node:Time' )
        }
    graph['title'] = "NUMA Activity for athena MJ (Multi Jobs) %s" % cond
    graph['xtitle']= "Time, sec since 00:00"
    graph['ytitle']= "Nbr of hits/misses"
    graph['text']= "np = %s" % np_list
    graph_list.append(graph)


    pads = list()

    cpp = 1 #charts per pad 
    ppc = 1 #pads per canvas

    c.cd()
    gStyle.SetOptStat(0);
    gStyle.SetPalette(0);
    gStyle.SetCanvasColor(0);
    gStyle.SetFrameFillColor(0);
    gStyle.SetMarkerStyle(21)
    gStyle.SetMarkerColor(2)
    gStyle.SetMarkerSize(0.5)
    print "gStyle.Set done"

    title = TPaveLabel(0.1,0.98,0.9,1, "Athena MJ Plots");
    title.SetFillColor(0); title.SetTextFont(40); 
    #title.Draw();print "title Drawn"

    mgs =  list()  #List of TMultiGraphs
    ls =   list()  #List of TLegends
    gs =   list()  #List of TGraph

    for j in xrange(ppc):
        y_factor = 0.99;   x1 = 0.01; x2 = 0.99;  y1 = y_factor - (y_factor-0.01)*(j+1)/float(ppc); y2 = y_factor - (y_factor-0.01)*j/float(ppc)
        print "x1,y1,x2,y2",  x1, y1, x2, y2 
        pad = TPad("pad%i" % j, "pad%i" % j,   x1, y1, x2, y2,   0); pad.Draw()
        pads.append(pad);
    
    num_cans = len(graph_list) /(cpp*ppc) if len(graph_list) % (cpp*ppc)==0 else len(graph_list)/(cpp*ppc) + 1 
    graph_list += [None,]* (num_cans*cpp*ppc - len(graph_list))
    print "number of pages/canvases in report = ", num_cans
    
    pdf_file = root_file
    for s in ['merged.', '.py', '.root']:
        pdf_file = pdf_file.replace(s, '')
    pdf_file ="%s.pdf" % pdf_file

    for i in xrange(num_cans):
        for j in xrange(ppc):
            graph = graph_list[ppc*i+j]
            if graph is None:
                continue
            
            if 'logY' in graph.keys():
                if graph['logY']:
                    pads[j].SetLogy()
            else:
                pads[j].SetLogy(0)
            
            pads[j].cd()
            pads[j].SetRightMargin(0.2)
            l = TLegend(0.82,0.20,0.99,0.89); ls.append(l) 
            mg = TMultiGraph(); mgs.append(mg)
            print "graph=", graph
            gs.append(MakeMultiGraph(graph, mg, l))

        c.Update()
        if i == 0:
            print "pdf.start"
            c.Print(pdf_file+'(', 'pdf') #start page
        elif i < num_cans-1:
            print "pdf.body"
            c.Print(pdf_file, 'pdf')    #body pages
        else:
            print "pdf.end"
            c.Print(pdf_file + ')', 'pdf') #end page
        #c.SaveAs("%s.%i.png" % (pdf_file, i))
        c.SaveAs("%s.%i.C" % (pdf_file, i))
        for pad in pads:
            pad.Clear()

########## THE END ################    


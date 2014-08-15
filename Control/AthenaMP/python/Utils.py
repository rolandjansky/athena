# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaMP.Utils
# @purpose: a set of tools to handle various aspects of AthenaMP
# @author Sebastien Binet <binet@cern.ch>
# @coauthor: Mous Tatarkhanov <tmmous@berkeley.edu>

__doc__     = "Misc. utils for AthenaMP"       
__version__ = "$Revision: 1.1 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import os

#-----Helper tools for AthenaMP--#

def get_mp_root(msg=""):
    tmp_root=os.getenv("ATHENA_MP_TMPDIR")
    if msg == "":
        from AthenaCommon.Logging import log as msg
    if tmp_root is None:
        import tempfile
        if tempfile.tempdir is None:
            tmp_root = "/tmp"
        else:
            tmp_root = tempfile.tempdir
    else:
        msg.debug("Using ATHENA_MP_TMPDIR environment variable to set athena-mp dir")
    username = "MP"    
    if os.getenv("LOGNAME") != None :
        username = os.getenv("LOGNAME")
    elif os.getenv("USER") != None :
        username = os.getenv("USER")
    return  os.sep.join([tmp_root,"athena-mp-tmp-%s" % username])

def hack_copy(srcDir, destDir):
    """ explicitly copy files not captured by IoRegistry"""
    #couple of sanity checks
    if srcDir == '': 
        srcDir = os.curdir
    if srcDir == destDir:
        msg.warning("hack_copy called with srcDir = destDir = "+srcDir)
        return

    #A hack for RDO reconstruction after nextEvent(n)
    filepattern_list = [ 'DigitParams.db', 'SimParams.db',
                         '*.pmon.stream' ]

    import fnmatch
    import shutil
    import os
    for pattern in filepattern_list:
        for f in os.listdir(srcDir):
            if fnmatch.fnmatch(f, pattern):
                import os.path
                src_path = os.path.join(srcDir,f)
                dest_path = os.path.join(destDir,f)
                if not os.path.isfile(dest_path):
                    shutil.copyfile(src_path, dest_path)

def slice_it(iterable, cols=2):
    from itertools import islice
    start = 0
    chunksz,extra = divmod (len(iterable), cols)
    if extra:
        chunksz += 1
    for i in xrange(cols):
        yield islice (iterable, start, start+chunksz)
        start += chunksz

#-----memory status tools---#
import sys
if 'linux' in sys.platform:
    def mem_status(msg):
        """memory usage information: shared/private"""
        for line in open('/proc/self/status'):
            if line.startswith('Vm'):
                msg.debug (line.strip())
        private,shared=_get_mem_stats()
        msg.info ("===> private: %s MB | shared: %s MB",
                  private/1024.,
                  shared /1024.)
else:
    def mem_status(msg):
        """memory usage information: dummy for non-linux"""
        return
    
PAGESIZE=os.sysconf("SC_PAGE_SIZE")/1024 #KiB
have_pss=0
def _get_mem_stats(pid='self'):
    """parses /proc/self/statm and smaps for memory usage info"""
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
    

#---- CPU-Proc affinty setting tools---#
if 'linux' in sys.platform:
    def get_cpu(pid):
        import commands
        """get core nbr where the proc-pid resides at that moment"""
        cmd = "ps --pid %i -o psr" % pid
        #print ">%s" % cmd
        out = commands.getoutput(cmd)
        cpu = int(out.splitlines()[1].split()[0])
        #print "pid: [%i] has cpu: [%i]" % (pid, cpu)
        return cpu
else:
    def get_cpu(pid):
        """dummy on non-linux"""
        return 0
    
def set_proc_affinity(pid, cpu):
    """set pid to cpu affinity for process"""
    import commands
    cmd = "taskset -pc %i %i" % (cpu, pid)
    #print "> taskset -pc %i %i" % (cpu, pid)                                                                                                       
    st,out = commands.getstatusoutput(cmd)
    return st


#---- AthenaMP profiling related tools---#
time_list = list()
time_list2 = list()

def watch(msg=None, message=""):
    import time
    """Timer (elap, user, system, child) with time-interval-reports into msg stream"""
    global time_list, time_list2
    time_list.append(os.times())
    time_list2.append(time.time())
    
    if msg is not None:
        (utime, stime, cutime, cstime, etime) = dt();
        elap_time =     "%s_ELAP_TIME=%.4f seconds" % (message, etime)
        user_time =     "%s_USER_TIME=%.2f" %   (message, utime)
        system_time =   "%s_SYSTEM_TIME=%.2f" % (message, stime) 
        child_utime =   "%s_CHILD_UTIME=%.2f" % (message, cutime)
        child_stime =   "%s_CHILD_STIME=%.2f" % (message, cstime)
        msg.info(elap_time)
        msg.debug("%s %s" % (user_time, system_time) )
        msg.debug("%s %s" % (child_utime, child_stime) )
    return len(time_list)

def dt(n=-1):
    import operator
    return  tuple( map(operator.sub, time_list[n], time_list[n-1]) )

def dt2(n=-1):
    return time_list2[n] - time_list2[n-1]




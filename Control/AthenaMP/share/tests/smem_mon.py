#!/usr/bin/env python

# @author:  Mous Tatarkhanov <tmmous@cern.ch>
# @date:    August 23, 2010
# @example:
# @code
# @endcode
#


__version__ = "$Revision: 000001 $"
__author__  = "Mous Tatarkhanov <tmmous@cern.ch>"

from optparse import OptionParser

import sys, os
import time, operator
import commands 

smem_exe = "/afs/cern.ch/user/t/tmmous/smem-0.9/smem" 
smem_log = "smem_log"
smem_ppid  = None
smem_time_step = 0.5

### helpers -------------------------------------------------------------------
def smem(ppid = None, message = None):

    if ppid is None:
        ppid = smem_ppid

    if message is not None:
        cmd = "echo %s >> %s" % (message, smem_log)
        out = commands.getoutput(cmd)

    cmd = "%s -P athena.py -s pid >> %s" % (smem_exe, smem_log) 
    out += commands.getoutput(cmd)
    
    print "smem: %s" % out
    
    if ps_line_nbr(ppid) > 0:
        return True
    else: 
        return False

def ps_line_nbr(ppid):
    import commands
    cmd = "ps --ppid %s -o pid,state,vsize,rss,sz,start,cputime,etime " % ppid
    (sc, out) = commands.getstatusoutput(cmd)
    
    if (sc != 0):
        print "%s\n" % cmd
        print " PS> ERRROR... sc=%i" % sc
        print " out=%s" % out 
        return 0
    
    print ">PS sc=%i" % sc
    print "%s" % out

    
    ln = len(out.splitlines()) - 1
    print "line_nbr=", ln
    return ln

def get_cpu(pid):
    import commands
    cmd = "ps --pid %i -o psr" % pid
    #print ">%s" % cmd
    out = commands.getoutput(cmd)
    cpu = int(out.splitlines()[1].split()[0])
    #print "pid: [%i] has cpu: [%i]" % (pid, cpu)
    return cpu

def set_proc_affinity(pid, cpu):
    import commands
    cmd = "taskset -pc %i %i" % (cpu, pid)
    #print "> taskset -pc %i %i" % (cpu, pid)                                                                                                       
    st,out = commands.getstatusoutput(cmd)
    return st

time_list = list()

def watch( message=None):
    time_list.append(time.time())
    
    if message is not None:
        return "[%i] %s " % (dt(), message)
    else:
        return len(time_list)

def dt(n=-1):
    return time_list[n] - time_list[n-1]

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] -f jobo")
    p = parser.add_option
    p( "-l",
        "--log",
        dest = "log_file",
        default = None,
        help ="smem log file " 
    )
    
    p( "-e",
        "--exe",
        dest="exe_file",
        default = "/afs/cern.ch/user/t/tmmous/smem-0.9/smem",
        help="location of smem executable"
    )
    p(  "-p",
        "--ppid",
        dest="ppid",
        default = None,
        help = "parent process pid"
    )
    p ( "-t",
        "--time_step",
        dest = "time_step",
        default = 0.5,
        help = "smem measurement time step"
    )
    (options, args) = parser.parse_args()
    
    if  options.log_file == None:
        str(parser.print_help() or "")
        sys.exit(1)
    
    if  options.ppid == None:
        str("invalid ppid given")
        str(parser.print_help() or "")
        sys.exit(1)

    if (options.exe_file == None) or  not os.path.exists(options.exe_file):
        str("invalid exe_file, please provide exe file location")
        str(parser.print_help() or "")
        sys.exit(1)
        
    smem_log = options.log_file
    smem_exe = options.exe_file
    smem_ppid = options.ppid
    smem_time_step = float(options.time_step)

    print "smem log_file = [%s]" % smem_log
    print "smem exe_file = [%s]" % smem_exe
    print "smem ppid = [%s]" % smem_ppid
    print "smem time_step = [%.1f]" % smem_time_step

    if os.path.exists(smem_log):
        print "  given smem_log name %s exists.. renaming it to old.%s" % (smem_log, smem_log)
        os.rename(smem_log, "OLD.%s" % smem_log)
    
    t0 = time.time()

    while( smem( message = "time=%.2f" % (time.time()-t0)) ):
        time.sleep(smem_time_step);
        pass

    print ("DONE...")

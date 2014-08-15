#!/usr/bin/env python

# @file:    mpMon.py
# @purpose: This script monitors the mp performance of AthenaMP
#           Rewriting the shell script - athenaMP_monitor.sh
#           Inspired by monitoring tools from Sebastien: PerfMon; 
# @author:  Mous Tatarkhanov <tmmous@cern.ch>
# @date:    December 2009
#
# @example:
# @code
# monitor_athenaMP 2 rdotoesd.py 60
# @endcode
#


"""
PROCESS STATE CODES
Here are the different values that the s, stat and state output specifiers (header "STAT" or "S") will
display to describe the state of a process.
D    Uninterruptible sleep (usually IO)
R    Running or runnable (on run queue)
S    Interruptible sleep (waiting for an event to complete)
T    Stopped, either by a job control signal or because it is being traced.
W    paging (not valid since the 2.6.xx kernel)
X    dead (should never be seen)
Z    Defunct ("zombie") process, terminated but not reaped by its parent.
"""

__version__ = "$Revision: 285809 $"
__author__  = "Mous Tatarkhanov <tmmous@cern.ch>"

from optparse import OptionParser
import AthenaMP.mpMonTools as mpt

import sys

class Writer:
    def __init__(self, stdout, filename):
        self.stdout = stdout
        self.logfile = file(filename, 'a')

    def write(self, text):
        self.stdout.write(text)
        self.logfile.write(text)

    def close(self):
        self.stdout.close()
        self.logfile.close()

    def flush(self):
        self.stdout.flush()
        self.logfile.flush()

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] -f jobo")
    p = parser.add_option
    p( "-j",
       "--jobo",
       dest = "jobo",
       help = "The path to the job options file to run in parallel" )
    p( "-p",
       "--np",
       dest    = "nbrProcs",
       default = [1,2],
       help = "Nbr of parallel processes to fork" )
    p( "-e",
       "--ne",
       dest    = "nbrEvts",
       default = "-1",
       help = "Number of events to process, EvtMax" )
    p( "-o",
       "--output",
       dest = "outFileName",
       default = 'mplog',
       help = "Name of the output file which will contain the informations gathered for  monitoring." )
    p( "-f",
        "--flush_cache",
        dest = "doFlushCache",
        action = "store_true",
        default = False,
        help ="switch to activate flushing of the machine cache of the machine before launching athenaMP" )
    p( "-c",
       "--comments",
       dest = "commentsStr",
       default = "",
       help = "comments to add to the name and report" )
    p( "-d",
       "--doPlots",  #FIX
       dest = "doPlots",
       action="store_true",
       default = False,
       help = "switch to activate plotting of report charts at the end"
       )
    
    (options, args) = parser.parse_args()
    
    import sys
    if options.jobo == None or options.nbrProcs ==None or options.nbrEvts == None:
        str(parser.print_help() or "")
        sys.exit(1)
        
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass
    
    #output redirection to file and display
    writer = Writer(sys.stdout, options.outFileName )
    sys.stdout = writer 
    np_list = eval(options.nbrProcs)
    if isinstance(np_list, int):
        np_list = [np_list,]

    ne = int(options.nbrEvts)
    jobo = options.jobo
    print "np_list = ", np_list
    print "ne = ", ne
    print "jobo = ", jobo
    print "mpMon.log =", options.outFileName
    print "doFluchCache=", options.doFlushCache, type(options.doFlushCache)
    if options.doFlushCache:
        options.commentsStr += ".doFlushCache"
    
    
    def cleanup():
        print ' Cleaning...Goodbye!'
        for pid in mpt.pid_list:
            mpt.stop_proc_tree(pid)

    import atexit
    atexit.register(cleanup)
    
    TIME_STEP = mpt.TIME_STEP
    
    import os
    import subprocess
    import signal
    import time
    import commands
    
    for np in np_list:
        writer.flush()
        
        suffix = "mp.%s.%i.%i" % (jobo, np, ne) 
        sar_log = "sar.%s" % suffix
        if os.path.exists(sar_log):
            os.remove(sar_log)
            
        def _print_mem():
            mpt.print_memstat("<np%i.ne%i>:" % (np, ne))
       
        if options.doFlushCache:
            print subprocess.call(['flush_cache.py',])
            time.sleep(TIME_STEP) 
        
        mpt.init_mp_stat()
        _mp_stat = mpt.mp_stat
        #_print_mem()
        
        sar_proc = mpt.launch_sar(sar_log, TIME_STEP) #launching sar for io,mem,cpu monitoring
        
        time.sleep(3*TIME_STEP)
        
        _print_mem();
        t0=t1=t2=t3=0   
        t0=time.time()
        mproc = mpt.launch_athenaMP(jobo, np, ne); #launching athena-MP
        mpid = mproc.pid #mother process pid
        print "parent launched ...[ %i]" % mpid       
        
        mp_log = os.path.join("mp.output", "stdout.%s" % suffix)
        #print "mpid_log = ", mp_log
        
        _mproc = mpt.ProcDict(mpid, child=False)        
       
        time.sleep(TIME_STEP); 

        #SERIAL: Mother Init Stage
        while not mpt.children_born(mp_log, mpid,np) and _mproc.proc_ps_stat():
            if np==0: break
            time.sleep(TIME_STEP)
        t1=time.time()
        
        
        #PARALLEL Stage
        while mpt.children_working(mpid) and _mproc.proc_ps_stat():
            if np==0: break
            _print_mem()
            time.sleep(TIME_STEP)
        t2 = time.time()
        
        _print_mem()
        
        print "children processes finished:"
        
        #SERIAL: Mother-Finalize stage
        while mproc.poll() is None:
            _mproc.proc_ps_stat()
            _print_mem()        
            time.sleep(TIME_STEP)
        t3 = time.time()
        
        mpt.summarize_proc_stat()
       
        #print "EXIT, thus have to terminate all created processes:"
        try:
            mproc.wait(); print "mproc joined-finished"
        except Exception, e:
            print "## while waiting mother process caught exception [%s] !!" % str(e.__class__), "## What:",e,
            print sys.exc_info()[0], sys.exc_info()[1]
            sc = 1
            pass
        
        for i in xrange(3):
            _print_mem()
            time.sleep(TIME_STEP)
        
        print "FINISHED MONITORING:"
        mpt.stop_proc(sar_proc)
        
        print "COLLECTING STATISTICS..."
        mpt.get_full_sar_stat(sar_log)
        print "FINISHED COLLECTING STATISTICS"
        
        print "START ANALYSIS..."
        
        cp_dir = mpt.grepPath(mp_log, "workdir", sep=':')
        #print "worker master cpid_dir = ", cp_dir
        print " ELAPSED TIMES: \n MotherInit: dt1=[%i sec] \n Parallel dt2=[%i sec] \n MotherFinalize dt3=[%i sec]" % (t1-t0, t2-t1, t3-t2)
        
        _mp_stat['cp_summary']=mpt.CPSummary(np)
        _mp_stat['mp_summary']=mpt.MPSummary(np)
        _mp_summary = _mp_stat['mp_summary']
        _cp_summary = _mp_stat['cp_summary']
        
        _mp_summary.extract_summary(mp_log)
        _cp_summary.extract_summary(cp_dir)
        _mp_summary['x_init_time']= [t1-t0, ] #externally observed time
        _mp_summary['x_par_time'] = [t2-t1, ] #externally observed time
        _mp_summary['x_fin_time'] = [t3-t2, ] #externally observed time

        _mp_summary['event_rate']= [ float(ne)*60.0/float(_mp_summary['m_par_time'][0]), ]
        _mp_summary['event_rate_x'] = [ float(ne)*60.0/float(t2-t1), ]
        _mp_summary['event_proc_rate']= [ _mp_summary['event_rate'][0] / float(np), ]
        _mp_summary['event_proc_rate_x'] = [_mp_summary['event_rate_x'][0] / float(np), ]

        
        mpt.print_summary()
        
        print "FINISHED ANALYSIS"
        
        print "START REPORT..."
        mpt.prepare_mp_stat() # preparing mp_stat dictionary for ROOT
        import pickle
        pickle.dump(_mp_stat, open("pickle.%s.f" % suffix,  "w"))        

        mpt.writeRootFile("%s.root" % suffix, np)
        print "FINISHED REPORT."
        
        cleanup()

    import platform
    from socket import gethostname
    host_name =  gethostname().split('.')[0]
    merged_root_file = "%s.mp.%s.ne%i.%s.root" % (host_name, jobo, ne, options.commentsStr)
    mpt.mergeRootOutput(merged_root_file, jobo, np_list, ne)    
    
    if options.doPlots:
        mpt.report(merged_root_file, ne, comments = options.commentsStr)
    
    cleanup()
    print "The End"
    sys.exit(0)


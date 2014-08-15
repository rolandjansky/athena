#!/usr/bin/env python

# @file:    mjMon.py
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


__version__ = "$Revision: 276792 $"
__author__  = "Mous Tatarkhanov <tmmous@cern.ch>"

from optparse import OptionParser
import AthenaMP.mjMonTools as mpt

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
    p( "-n",
       "--numa_set",  #FIX
       dest = "numaSet",
       #action="store_true",
       default = None,
       help = "this option activates numa settings. Format: [(0,0),(1,1),...]"
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

    if options.numaSet is not None:
        numa_list = eval(options.numaSet)
    else:
        numa_list = None

    if isinstance(numa_list, list):
        print "numa_list=%s" % numa_list
    elif numa_list is not None:
        print "Please input correct numa_list"
        str(parser.print_help() or "")
        sys.exit(1)

    ne = int(options.nbrEvts)
    jobo = options.jobo
    print "np_list = ", np_list
    print "ne = ", ne
    print "jobo = ", jobo
    job = jobo.split()[0] 
    print "mpMon.log =", options.outFileName
    print "doFluchCache=", options.doFlushCache, type(options.doFlushCache)
    if options.doFlushCache:
        options.commentsStr += ".doFlushCache"
    print "numa_list=%s" % numa_list

    def cleanup():
        print ' Cleaning...Goodbye!'
        for pid in mpt.pid_list:
            mpt.stop_proc_tree(pid)
    
    def _print_mem():
        mpt.print_memstat("<np%i.ne%i>:" % (np, ne))

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
        
        mpt.show_numactl()
        
        suffix = "mj.%s.%i.%i" % (jobo.split()[0], np, ne) 
        sar_log = "sar.%s" % suffix
        if os.path.exists(sar_log):
            os.remove(sar_log)
               
        if options.doFlushCache:
            print subprocess.call(['flush_cache.py',])
            
        #time.sleep(TIME_STEP) 
        
        mpt.init_mp_stat()
        _mp_stat = mpt.mp_stat
        #_print_mem()
        
        sar_proc = mpt.launch_sar(sar_log, TIME_STEP) #launching sar for io,mem,cpu monitoring
        
        #time.sleep(3*TIME_STEP)
        
        _print_mem();
        t0=t1=t2=t3=0   
        t0=time.time()
        
        #LAUNCH:
        proc_list = list()
        proc_dict = dict()
        #cpubind = [0,0,0,0,  1,1,1,1]
        #membind=   [0,0,0,0,  1,1,1,1]
        
        if numa_list is not None:
            if len(numa_list) < np:
                print "len(numa_list) < np:  need to append [('f','f'),]"
                numa_list += [('f','f'),] * (np - len(numa_list))
            else:
                print "len(numa_list)==len(range(np)): there are enough numa settings defined"
            
            iterator = zip(range(np), numa_list)
        else:
            iterator = zip(range(np), range(np))

        print "numa_list=%s" % numa_list

        se = 0 # skip events
        numa_set = None
        numa_str = ""
        for item in iterator:
            i = item[0]
            if numa_list is not None:
                numa_set = item[1]
                numa_str = "numactl --membind=%i --cpubind=%i " % (numa_set[0],numa_set[1])
            
            (proc, proc_out, proc_err) = mpt.launch_athena(jobo, ne, se, np, "ne%i.ni%i" % (ne,i), numa_set ); #launching athena
            proc_list.append(proc)
            proc_dict[proc.pid] = (proc_out, proc_err)
            #print "%s athena %i.%i.%i proc launched ...[pid %i]  out:%s err:%s" % (numa_str, ne, np, i, proc.pid, proc_out, proc_err )
            se +=ne
        time.sleep(TIME_STEP); 

        #EXECUTION: 
        sc = list()# status code 
        ppid=os.getpid()
        while mpt.launched_processes_working(ppid):
            _print_mem()
            time.sleep(TIME_STEP)
            for proc in proc_list:
                rc = proc.poll() # return code if subprocess finished
                if sc is not None:
                    sc.append(rc)
            
        t1=time.time()
        
        print "athena processes finished:"
        
        #SUMMARY
        mpt.summarize_proc_stat()
        for i in xrange(2):
            _print_mem()
            time.sleep(TIME_STEP)
        
        print "FINISHED MONITORING:"
        mpt.stop_proc(sar_proc)
        
        print "COLLECTING STATISTICS..."
        mpt.get_full_sar_stat(sar_log)
        print "FINISHED COLLECTING STATISTICS"
        
        print "START ANALYSIS..."
        
        print " ELAPSED TIMES: \n Time: dt1=[%i sec]" % (t1-t0)
        
    
        _mp_stat['sp_summary']=mpt.SPSummary(np)
        _mp_summary = _mp_stat['sp_summary'] 
        _mp_summary.extract_summary("dir") 
        
        #_mp_summary['x_init_time']= [t1-t0, ] #externally observed time
        #_mp_summary['x_par_time'] = [t2-t1, ] #externally observed time
        #_mp_summary['x_fin_time'] = [t3-t2, ] #externally observed time
        #_mp_summary['x_elap_time']= [t1-t0, ]
        #_mp_summary['event_rate']= [ float(ne)*60.0/float(_mp_summary['m_par_time'][0]), ]
        #_mp_summary['event_rate_x'] = [ float(ne)*60.0/float(t1-t0), ]
        #_mp_summary['event_proc_rate']= [ _mp_summary['event_rate'][0] / float(np), ]
        #_mp_summary['event_proc_rate_x'] = [_mp_summary['event_rate_x'][0] / float(np), ]

        
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
    merged_root_file = "%s.mj.%s.ne%i.%s.root" % (host_name, job, ne, options.commentsStr)
    mpt.mergeRootOutput(merged_root_file, job, np_list, ne)    
    
    if options.doPlots:
        mpt.report(merged_root_file, ne, comments = options.commentsStr)
    
    cleanup()
    print "The End"
    sys.exit(0)


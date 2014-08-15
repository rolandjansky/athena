#!/usr/bin/env python

import os, sys, time
import multiprocessing
from multiprocessing import Pool

def flush_cache(n):
    l = list()
    for i in xrange(n):
        l.append("a" * (1 << 30))
    print "%i gb of memory eaten" % n
    time.sleep(10)
    return n

if __name__ == '__main__':
    import commands
    out = commands.getoutput('free -m')
    print ">free -m \n", out

    total_mem_mb =int( out.splitlines()[1].split()[1])
    cached_mem_mb = int( out.splitlines()[1].split()[6])
    print "CACHED [%i Mb] - before flush" % cached_mem_mb
    
    if cached_mem_mb < 200:
        print "no need to flush the cache... bye!"
        sys.exit(0)

    gb = 1 + (total_mem_mb >> 10) 
    
    ncpus = multiprocessing.cpu_count() 
    print "ncpus= [%i]" % ncpus
    print "total available memory [%i Mb] [%i Gb]" % (total_mem_mb, gb)

    nprocs = 2*ncpus
    ngb = 1 + gb / nprocs

    
    print "Nbr of Procs to bite on memory [%i] " % nprocs
    print "Nbr of Gb to flush per process  [%i Gb]" % ngb


    pool = Pool(processes = nprocs)
    result = pool.map(flush_cache, [ngb,]*nprocs)
    print "Total memory eaten: [%i Gb]" % sum(result)

    out = commands.getoutput('free -m')        
    print ">free -m \n", out
    cached_mem_mb = int( out.splitlines()[1].split()[6])    
    print "CACHED [%i Mb] - after flush" % cached_mem_mb
    print "Your machine's memory cache is  flushed" 
    
    time.sleep(5)

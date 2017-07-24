#!/bin/env python
#file PrintFirstJobForRun.py
#Small utility for testing RunDMC: find out which job to look in for a given run.

from Digitization.RunDependentMCTaskIterator import *
import argparse

def findJobForRun(run,task,maxEvents,printq=True):
    """Get the job number that first processes this run.  Returns an integer."""
    i,jobs = (0,taskIterator(task,maxEvents))
    try:
        while True:
            jobs.next()
            for j in jobs.donejob:
                if j['run'] == run:
                    if printq:
                        print "-="*10
                        for j in jobs.donejob: print j
                        print "-="*10
                    return i
                pass
            i += 1
            pass
        pass
    except StopIteration:
        pass
    return -1

# main code
if __name__=='__main__':
    import re
    p = argparse.ArgumentParser(description='Find the job # in which the specified run would first be processed.')
    p.add_argument('--nev', type=int, default=50, help="the number of events per job")
    p.add_argument('run', type=int)
    p.add_argument('config_file') #type=argparse.FileType('r'))
    args = p.parse_args()

    ### read the file and get the jobmaker therein
    dictline = re.compile(r"{('\w+':[\d\w.]+,? ?)+},")    #requires trailing comma.
    RUNDMC_TEST_CONFIG = True
    RunDMC_testing_configuration = True #back compat
    execfile(args.config_file)
    if not JobMaker: print 'No configuration found in file. Make sure JobMaker was not deleted!'
    else:
        a = findJobForRun(args.run,JobMaker,args.nev)
        if (a != -1): print 'Job number = ',a
        else: print 'Run',args.run,'was not found.'

    



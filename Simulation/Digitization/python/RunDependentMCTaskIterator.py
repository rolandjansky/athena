# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## module RunDependentMCTaskIterator.py
# defines class taskIterator(runLumiInfo,evtsPerJob)
# defines primitive function findPlaceInTask(jobNum,runLumiInfo,evtsPerJob)
# defines user function getRunLumiInfoFragment(jobNum,runLumiInfo,evtsPerJob)
## author: ATA <ayana>, DMB <davidb> 
## date: 29 Oct 2014, 1 Sept 2010

import itertools

def getRunLumiInfoFragment(jobnumber,task,maxEvents,sequentialEventNumbers=False):
    """Calculate the specific configuration of the current job in the digi
    task. Try to make each fragment utilize the same amount of CPU and
    Cache resources.  Exploits the fact that the task when sorted by
    mu value will have long finishing pieces near the beginning and
    short pieces near the end.  A more even solution is obtained by
    pairing chunks of maxEvents/2 from the beginning and end of the
    sorted task.
    """
    hiMaxEvents,loMaxEvents=0,0
    if(maxEvents%2==0):
        hiMaxEvents=loMaxEvents=int(maxEvents/2)
    else:
        hiMaxEvents=int((maxEvents-1)/2)
        loMaxEvents=int((maxEvents+1)/2)
    hi_mu_frag=[]
    lo_mu_frag=[]
    if hiMaxEvents > 0:
        hi_mu_frag=getFragment(jobnumber,sorted(task,key=lambda job: job['mu'],reverse=True),hiMaxEvents)
    if loMaxEvents > 0:
        lo_mu_frag=getFragment(jobnumber,sorted(task,key=lambda job: job['mu']),loMaxEvents)        
    
    fragment=sorted(sum([hi_mu_frag,lo_mu_frag],[]),key=lambda job: job['run'])
    if sequentialEventNumbers:
        return defineSequentialEventNumbers(jobnumber,fragment,maxEvents)
    else:
        return fragment

def getFragment(jobnumber,task,maxEvents):
    """ Calculate the specific configuration of the current job in the digi task.
    """
    try: tIter = findPlaceInTask(jobnumber,task,maxEvents)
    except StopIteration:
        raise IndexError('There are only %i jobs in this task (not %i).' % (len([1 for i in taskIterator(task,maxEvents)]) + 1,jobnumber + 1))
    try: tIter.next()
    except StopIteration:
        pass
    return tIter.donejob
#
def findPlaceInTask(jobnumber,task,maxEvents):
    """ Get the 'i'th job in the task, where each job tries to do maxEvents events.
    The 'force_new' flag in the LB list ends the task before that LB, ignoring maxEvents.
    Returns a taskIterator. Can raise StopIteration, so you should nest in try.
    """
    jobnumber = max(int(jobnumber),0)
    i, jobs = (0,taskIterator(task,maxEvents))
    while True:
        if (i == jobnumber): return jobs
        i += 1
        jobs.next()
    #exit by exception
#
class taskIterator(object):
    """iterator over a list of dicts (the 'task'). Each dict must contain 'evts', optionally 'force_new'.
    """
    def __init__(self,task,step):
        """create the iterator from task (a list of dicts) and step (the max number of evts. per job)
        the iterator """
        self.step = step
        self.taskit = itertools.cycle(task)
        self.offset = 0
        self.current = None
        self.donejob = []
        try:
            if min(e['evts'] for e in task) < 0: #py2.4
                raise ValueError("Cannot use empty task lists or negative N(events).")
        except KeyError:
            raise ValueError("Cannot use tasks that don't always define 'evts':", task)
        if (step < 1): raise ValueError("Cannot use step size smaller than 1 in a taskIterator.")
        return

    def __eq__(self, another):
        return (self.current == another.current) and (self.step == another.step)

    def __iter__(self):
        return self

    def __repr__(self):
        return "offset=%i; row=%i,%i" % (self.offset,self.current.get('run',0),self.current.get('lb',0))

    def next(self):
        self.donejob = []
        if (self.current == None):  self.current = self.taskit.next()
        to_do = self.step
        while True:
            if (to_do == 0) : return self.offset, self.current
            can_do = self.current['evts'] - self.offset
            if ( can_do > to_do ) :
                self.donejob.append( self.current.copy() )
                self.donejob[-1].update({'evts':to_do})
                self.offset += to_do
                return self.offset, self.current
            else:
                to_do -= can_do
                self.offset = 0
                pass
            self.donejob.append( self.current.copy() )
            self.donejob[-1].update({'evts':can_do})
            self.current = self.taskit.next()
            if self.current.get('force_new',False): to_do = 0
        raise StopIteration
#

def defineSequentialEventNumbers(jobnumber,fragment,maxEvents):
    """ Calculate sequential event numbers for the defined getFragment.
    """
    new_frag = []
    evt_nbr = jobnumber * maxEvents
    for t in fragment:
        for i in range(t['evts']):
            evt_nbr += 1
            new_frag.append({
                'run': t['run'],
                'lb': t['lb'],
                'starttstamp': t['starttstamp'],
                'dt': t['dt'],
                'evts': 1,
                'evt_nbr': evt_nbr,
                'mu': t['mu'],
                'force_new': t['force_new']
            })
    return new_frag

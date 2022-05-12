# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

include.block('RunDependentSimData/configCommonSim.py')
if not 'logging' in dir(): import logging
simlog = logging.getLogger('Sim_tf')

# Compress the digitization profile into one which is appropriate for Simulation.
JobMakerSim = []
step = -1
cacheElement=None
if not 'scaleTaskLengthSim' in dir():
    profileTotalEvents=sum(lb['evts'] for lb in JobMaker)
    trfTotalEvents = runArgs.maxEvents
    corrTotalEvents = max(trfTotalEvents,50)
    scaleTaskLengthSim = float(corrTotalEvents)/float(profileTotalEvents)

def simEvts(x):
    return int(scaleTaskLengthSim * x)

for el in JobMaker:
    if el['step'] != step:
        if cacheElement is not None:
            cacheElement['evts'] =  simEvts(cacheElement['evts'])
            JobMakerSim += [cacheElement]
        cacheElement = el
        step = el['step']
    else:
        cacheElement['evts'] += el['evts']
cacheElement['evts'] =  simEvts(cacheElement['evts'])
JobMakerSim += [cacheElement]

import math
if 'runArgs' in dir():
    if hasattr(runArgs,"jobNumber") and hasattr(runArgs,"maxEvents"):
        trfJobNumber = runArgs.jobNumber
        trfMaxEvents = runArgs.maxEvents
        trfTotalEvents = runArgs.maxEvents
        trfSkipEvents = runArgs.skipEvents if hasattr(runArgs, "skipEvents") else 0

        # do executor step filtering
        if hasattr(runArgs, "totalExecutorSteps") and runArgs.totalExecutorSteps > 1:
            JobMaker = list(filter(lambda lb: 'step' not in lb or lb['step'] == runArgs.executorStep, JobMaker))
            if runArgs.totalExecutorSteps != len(runArgs.executorEventCounts):
                raise ValueError("Mismatch between total executor steps and event fractions size!")
            trfMaxEvents = runArgs.executorEventCounts[runArgs.executorStep]
            trfSkipEvents = runArgs.executorEventSkips[runArgs.executorStep]
            DoNotCorrectMaxEvents = True

        if runArgs.maxEvents==-1:
            raise SystemExit("maxEvents = %d is not supported! Please set this to the number of events per file times the number of files per job."%(runArgs.maxEvents,))
        if not 'DoNotCorrectMaxEvents' in dir():
            corrMaxEvents = math.ceil(float(trfMaxEvents)/50.0)*50.0 # round up to nearest 50 events..
        else:
            if not (hasattr(runArgs, "totalExecutorSteps") and runArgs.totalExecutorSteps > 1):
                simlog.warning('Using the actual number of EVNT input events for this job -- not for production use!')
            corrMaxEvents = trfMaxEvents
    else:
        raise SystemExit("Please provide jobNumber and maxEvents to runArgs.")
else:
    #this is a test job not a trf job
    trfJobNumber=1
    trfMaxEvents=10
    trfTotalEvents=10
    corrMaxEvents=float(trfMaxEvents)

#We may need to repeat this run for long production jobs.
#NB: unlike vanilla variable-mu jobs, it's possible to waste
#  up to trfMaxEvents-1 events per complete run in prodsys if
#  the number of events specified by this run is not evenly
#  divisible by trfMaxEvents.
runMaxEvents=sum(lb['evts'] for lb in JobMakerSim)
simlog.info('There are %d events in this run.' % runMaxEvents)
jobsPerRun=int(math.ceil(float(runMaxEvents)/corrMaxEvents))
simlog.info('Assuming there are usually %d events per job. (Based on %d events in this job.)', corrMaxEvents, trfMaxEvents)
simlog.info('There must be %d jobs per run.' % jobsPerRun)

# Override event numbers with sequential ones if requested
sequentialEventNumbers = True if 'SequentialEventNumbers' in dir() and SequentialEventNumbers else False
if sequentialEventNumbers:
    simlog.info('All event numbers will be sequential.')

# Random mu sampling
randomMuSampling = True if 'RandomMuSampling' in dir() and RandomMuSampling else False
if randomMuSampling:
    simlog.info('Mu values will be sampled randomly from the set profile.')
    #Load needed tools
    from RunDependentSimComps.RunDependentMCTaskIterator import getRandomlySampledRunLumiInfoFragment
    fragment=getRandomlySampledRunLumiInfoFragment(jobnumber=(trfJobNumber-1),task=JobMakerSim,maxEvents=trfMaxEvents,totalEvents=trfTotalEvents,skipEvents=trfSkipEvents,sequentialEventNumbers=sequentialEventNumbers)
else:
    #Load needed tools
    from RunDependentSimComps.RunDependentMCTaskIterator import getRunLumiInfoFragment
    fragment=getRunLumiInfoFragment(jobnumber=(trfJobNumber-1),task=JobMakerSim,maxEvents=trfMaxEvents,totalEvents=trfTotalEvents,skipEvents=trfSkipEvents,sequentialEventNumbers=sequentialEventNumbers)

from G4AtlasApps.SimFlags import simFlags
simFlags.RunAndLumiOverrideList=fragment

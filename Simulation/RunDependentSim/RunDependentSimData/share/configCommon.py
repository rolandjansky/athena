include.block('RunDependentSimData/configCommon.py')
from Digitization.DigitizationFlags import jobproperties
if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')

import math
if 'runArgs' in dir():
    if hasattr(runArgs,"jobNumber") and hasattr(runArgs,"maxEvents"):
        trfJobNumber = runArgs.jobNumber
        trfMaxEvents = runArgs.maxEvents
        if runArgs.maxEvents==-1:
            raise SystemExit("maxEvents = %d is not supported! Please set this to the number of events per file times the number of files per job."%(runArgs.maxEvents,))
        if not 'DoNotCorrectMaxEvents' in dir():
            corrMaxEvents = math.ceil(float(trfMaxEvents)/100.0)*100.0 # round up to nearest 100 events..
        else:
            digilog.warning('Using the actual number of HITS input events for this job -- not for production use!')
            corrMaxEvents = trfMaxEvents
    else: 
        raise SystemExit("Please provide jobNumber and maxEvents to runArgs.") 
else:
    #this is a test job not a trf job
    trfJobNumber=1
    trfMaxEvents=10
    corrMaxEvents=float(trfMaxEvents)
    
#We may need to repeat this run for long production jobs.
#NB: unlike vanilla variable-mu jobs, it's possible to waste
#  up to trfMaxEvents-1 events per complete run in prodsys if
#  the number of events specified by this run is not evenly
#  divisible by trfMaxEvents.
runMaxEvents=sum(lb['evts'] for lb in JobMaker)
digilog.info('There are %d events in this run.' % runMaxEvents)
jobsPerRun=int(math.ceil(float(runMaxEvents)/corrMaxEvents))
digilog.info('Assuming there are usually %d events per job. (Based on %d events in this job.)', corrMaxEvents, trfMaxEvents)
digilog.info('There must be %d jobs per run.' % jobsPerRun)

# Override event numbers with sequential ones if requested
sequentialEventNumbers = True if 'SequentialEventNumbers' in dir() and SequentialEventNumbers else False
if sequentialEventNumbers:
    digilog.info('All event numbers will be sequential.')

#Load needed tools 
from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
fragment=getRunLumiInfoFragment(jobnumber=(trfJobNumber-1),task=JobMaker,maxEvents=trfMaxEvents,sequentialEventNumbers=sequentialEventNumbers)

from RunDependentSimComps.RunLumiConfigTools import condenseRunLumiInfoFragment
digilog.info( 'Writing RunDMC trigger configuration fragment to file.  listOfRunsEvents = %s' %
              condenseRunLumiInfoFragment(fragment,"RunDMCTriggerRunsInfo.py"))

jobproperties.Digitization.RunAndLumiOverrideList=fragment

############################################################
## File faking the luminosity profile for 2011
## This file can be used to generate up to 1M events.
############################################################
#Run-dependent digi job configuration file.

from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
from Digitization.DigitizationFlags import jobproperties
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')

#Reducing ScaleTaskLength decreases the max. length of the job (from 100M events) AND decreases the period of the <mu> profile.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1.0
_evts = lambda x: int(ScaleTaskLength * x)

#Best-guess modeling of 2011 data luminosity profile: Hardcoded values are Beate's Scenario F. via email april 14th
if not 'Scenario' in dir():
    Scenario = {0:5, 1:5, 2:6, 3:12, 4:22, 5:34, 6:57, 7:57, 8:57, 9:57, 10:57, 11:40, 12:29, 13:17, 14:12, 15:11, 16:11, 17:6, 18:5}
elif Scenario == 'E':
    Scenario ={2:6, 3:12, 4:24, 5:36, 6:59, 7:59, 8:60, 9:59, 10:59, 11:42, 12:30, 13:18, 14:12, 15:12, 16:12}
elif Scenario == 'D':
    Scenario = {3:14, 4:47, 5:70, 6:77, 7:76, 8:64, 9:51, 10:38, 11:26, 12:25, 13:12}
      
if jobproperties.Digitization.simRunNumber.isDefault():
    #unset, try again.
    MCRunNumber = int(jobproperties.Digitization.getHitFileRunNumber(jobproperties.AthenaCommonFlags.PoolHitsInput.get_Value()[0]))
else:
    MCRunNumber = jobproperties.Digitization.simRunNumber.get_Value()
    
import random
if 'runArgs' in dir():
    digilog.info('Preparing to randomize the lumiblock sequence to minimize impact of short HITS files.')
    if hasattr(runArgs,"jobNumber") and hasattr(runArgs,"maxEvents"):
        trfJobNumber = runArgs.jobNumber
        trfMaxEvents = runArgs.maxEvents
    else:
        if not hasattr(runArgs,"jobNumber"):
            digilog.error('You must set the jobNumber parameter to use these job options (configLumi_FutureProfile.py).')
        else:
           digilog.error('You must set maxEvents to use these job options (configLumi_FutureProfile.py).')        
    random.seed(runArgs.jobNumber) #reproduceable jobs
else:
    digilog.warning('These job options are for the digitization transform! This is not a Digi_trf job: setting the jobNumber and EvtMax to default values.')
    trfJobNumber=0
    trfMaxEvents=10

JobMaker = []
runMaxEvents=sum([_evts(Scenario[ilb]) for ilb in Scenario]) #this is the length of the faked run in events.
#Calculate smallest configuration file required.  asNeeded = smallest number of repeated fake runs requred.
# This won't be larger than MaxEvents.
runLen = runMaxEvents
jobLen = trfMaxEvents
while jobLen:
    (runLen,jobLen) = (jobLen, runLen%jobLen)
asNeeded = trfMaxEvents/runLen
for iOffset in xrange(asNeeded):
    for iLB,scen in Scenario.items():
        if (_evts(scen)) > 0:
            lb = iLB
            if (lb == 0): lb = 100 #TagInfoMgr problems otherwise?
            JobMaker.append({'run':MCRunNumber, 'lb':lb, 'evts':_evts(scen), 'starttstamp':0, 'dt':0, 'mu':1.0 * iLB, 'force_new':False})
            pass
    pass

njobs=(runMaxEvents*asNeeded)/trfMaxEvents
digilog.info('using offset %g for job %g',trfJobNumber % njobs,trfJobNumber)
fragment=getRunLumiInfoFragment(jobnumber=(trfJobNumber % njobs), task=JobMaker, maxEvents=trfMaxEvents)
#For production trf, in case the input file is missing some events, we want to randomize which block comes last.
random.shuffle(fragment)
jobproperties.Digitization.RunAndLumiOverrideList=fragment
#import pprint, gc
#print len( gc.getreferrers( JobMaker ) )
#pprint.pprint( gc.getreferrers( JobMaker ) )      
del(JobMaker)
del(fragment)


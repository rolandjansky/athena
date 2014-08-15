############################################################
## File faking the luminosity profile for 2011
## This file can be used to generate up to 1M events.
############################################################
#Run-dependent digi job configuration file.
from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
from Digitization.DigitizationFlags import jobproperties
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
#We need to adjust for dataset sizes. 
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

#Best-guess modeling of 2011 data luminosity profile.
if not 'MCRunNumber' in dir():
    MCRunNumber = int(jobproperties.Digitization.getHitFileRunNumber(jobproperties.AthenaCommonFlags.PoolHitsInput.get_Value()[0]))
#Hardcoded values are Beate's Scenario F.
if not 'Scenario' in dir():
    Scenario = {0:5, 1:5, 2:6, 3:12, 4:22, 5:34, 6:57, 7:57, 8:57, 9:57, 10:57, 11:40, 12:29, 13:17, 14:12, 15:11, 16:11, 17:6, 18:5}
elif Scenario == 'E':
    Scenario ={2:6, 3:12, 4:24, 5:36, 6:59, 7:59, 8:60, 9:59, 10:59, 11:42, 12:30, 13:18, 14:12, 15:12, 16:12}
elif Scenario == 'D':
    Scenario = {3:14, 4:47, 5:70, 6:77, 7:76, 8:64, 9:51, 10:38, 11:26, 12:25, 13:12}


JobMaker = []                   
for iOffset in xrange(200000):
    for iLB in Scenario:
        if (_evts(Scenario[iLB])) > 0:
            JobMaker += [{'run':MCRunNumber, 'lb':iLB, 'evts':_evts(Scenario[iLB]), 'starttstamp':0, 'dt':0, 'mu':1.0 * iLB, 'force_new':False}]
        pass
    pass

from Digitization.DigitizationFlags import jobproperties

if 'runArgs' in dir():
    if hasattr(runArgs,"jobNumber") and hasattr(runArgs,"maxEvents"):
        trfJobNumber = runArgs.jobNumber
        trfMaxEvents = runArgs.maxEvents
    else: digilog.error('Tried to include RunLumiOverride configuration without knowing this runArgs.jobNumber or runArgs.maxEvents!')
else:
    #this is a test job not a trf job
    trfJobNumber=0
    trfMaxEvents=10
    
    
jobproperties.Digitization.RunAndLumiOverrideList=getRunLumiInfoFragment(jobnumber=trfJobNumber, task=JobMaker, maxEvents=trfMaxEvents)
del JobMaker

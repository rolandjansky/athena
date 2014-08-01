#Run-dependent digi job configuration file.
#SimulationJobOptions/ConfigLumi_preOptions.py
from Digitization.RunDependentMCTaskIterator import getRunLumiInfoFragment
from Digitization.DigitizationFlags import digitizationFlags
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
#<!-- these lines are hidden from parser. Test example runs with different muon DCS
    {'run':158116, 'lb':115, 'starttstamp':1277574167, 'dt':104.895, 'evts':7, 'mu':1.559, 'force_new':False},
    {'run':158392, 'lb':217, 'starttstamp':1277946999, 'dt':120.818, 'evts':6, 'mu':1.243, 'force_new':False},
#--> end hiding
]

if 'runArgs' in dir():
    if hasattr(runArgs,"jobNumber") and hasattr(runArgs,"maxEvents"):
        trfJobNumber = runArgs.jobNumber
        trfMaxEvents = runArgs.maxEvents
    else: digilog.error('Tried to include RunLumiOverride configuration without knowing this runArgs.jobNumber or runArgs.maxEvents!')
else:
    #this is a test job not a trf job
    trfJobNumber=0
    trfMaxEvents=10
    
    
digitizationFlags.RunAndLumiOverrideList=getRunLumiInfoFragment(jobnumber=trfJobNumber, task=JobMaker, maxEvents=trfMaxEvents)
del JobMaker

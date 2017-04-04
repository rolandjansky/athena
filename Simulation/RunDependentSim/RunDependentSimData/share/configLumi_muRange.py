####################
## File configLumi_muRange.py
####################
#Run-dependent digi job configuration file.
#User specifies run,lumiblock,timestamp, etc. via dictionary.
if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')

#userRunLumiOverride={'run':195847, 'startmu':15.0, 'endmu':25.0, 'stepmu':1.0, 'startlb':1, 'timestamp':1328039085}
#Run,start
#look for user preoption. Error otherwise!
try:
    if userRunLumiOverride.__class__ == dict: u = userRunLumiOverride
    else: u=eval(userRunLumiOverride) #str or bust
    digilog.info('doing RunLumiOverride with user-selected run(s) %s',u.get('run',-1))    #dict or bust
except:
    digilog.error("Set userRunLumiOverride=\'{\"run\":1234,...}\' in preExec to use configLumi_user.py!")
    raise RuntimeError("No valid userRunLumiOverride provided.")
keys=['run','startmu','endmu','stepmu','startlb','timestamp']
for key in userRunLumiOverride.keys():
    if key not in keys:
        raise RuntimeError("Invalid userRunLumiOverride provided, key: %s. Valid keys are: %s"%(key,keys)) 

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1

_evts = lambda x: int(ScaleTaskLength * x)
nsteps = int(((u.get('endmu',10.0)-u.get('startmu',0.0))/u.get('stepmu', 1.0)) + 1.0)
if nsteps < 2: raise RuntimeError("Step size too large!")

stepsWithExtraEvent=5000%nsteps
EventsPerStep= (5000 - (5000%nsteps))/nsteps
if _evts(EventsPerStep) < 2: raise RuntimeError("Step size too small!")

runlist = u.get('run', 195847)
if not isinstance(runlist,list): runlist = [runlist]
timelist = u.get('timestamp', 1328039085)
if not isinstance(timelist,list): timelist = [timelist]
lumilist = u.get('startlb', 1)
if not isinstance(lumilist,list): lumilist = [lumilist]

JobMaker = []
for currentRun,currentTime,currentLBStart in zip(runlist,timelist,lumilist):
    for step in range(nsteps):
        events = EventsPerStep+1 if step < stepsWithExtraEvent else EventsPerStep
        muval = float(u.get('startmu',0.0) +(step*u.get('stepmu', 1.0)))
        JobMaker += [{'run':currentRun, 'lb':currentLBStart+step, 'starttstamp':currentTime, 
                      'dt':0.000, 'evts':_evts(events), 'mu':muval, 'force_new':False}]
        pass #mu loop
    pass #conditions loop


include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

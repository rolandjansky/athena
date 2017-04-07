####################
## File configLumi_user.py
####################
#Run-dependent digi job configuration file.
#User specifies run,lumiblock,timestamp, etc. via dictionary.
if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')


#look for user preoption. Error otherwise!
try:
    if userRunLumiOverride.__class__ == dict: u = userRunLumiOverride
    else: u=eval(userRunLumiOverride) #str or bust
    digilog.info('doing RunLumiOverride with user-selected run %i',u.get('run',-1))    #dict or bust
except:
    digilog.error("Set userRunLumiOverride=\'{\"run\":1234,...}\' in preExec to use configLumi_user.py!")
    raise RuntimeError("No valid userRunLumiOverride provided.")
keys=['run','mu','evts','lb','starttstamp']
for key in userRunLumiOverride.keys():
    if key not in keys:
        raise RuntimeError("Invalid userRunLumiOverride provided, key: %s. Valid keys are: %s"%(key,keys)) 
    
#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

JobMaker=[{'run':u.get('run',189751),
           'lb':u.get('lb',116),
           'starttstamp':u.get('starttstamp',189751),
           'dt':0.000,
           'evts':_evts(u.get('evts',5000)),
           'mu':float(u.get('mu',30.000)),
           'force_new':False},#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

####################
## File configLumi_gTest.py
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
   {'run':180164, 'lb':69, 'starttstamp':1303554791, 'dt':0.000, 'evts':_evts(12), 'mu':5.0, 'force_new':False},
   {'run':183003, 'lb':177, 'starttstamp':1306973316, 'dt':0.000, 'evts':_evts(13), 'mu':6.0, 'force_new':False},
   {'run':185649, 'lb':52, 'starttstamp':1310870414, 'dt':0.000, 'evts':_evts(12), 'mu':7.0, 'force_new':False},
   {'run':185761, 'lb':182, 'starttstamp':1310954001, 'dt':0.000, 'evts':_evts(13), 'mu':8.0, 'force_new':False},
   #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

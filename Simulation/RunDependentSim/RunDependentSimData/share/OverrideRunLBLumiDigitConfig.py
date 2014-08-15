#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
#<!-- these lines are hidden from parser.
    {'run':158116, 'lb':115, 'starttstamp':1277574167, 'dt':104.895, 'evts':_evts(7), 'mu':1.559, 'force_new':False},
    {'run':158392, 'lb':217, 'starttstamp':1277946999, 'dt':120.818, 'evts':_evts(6), 'mu':1.243, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

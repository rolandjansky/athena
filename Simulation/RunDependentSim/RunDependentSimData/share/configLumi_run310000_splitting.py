# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
  {'run':310000, 'lb':1, 'starttstamp':1550000000, 'evts':_evts(1), 'mu':20.500, 'step': 0},
  {'run':310000, 'lb':2, 'starttstamp':1550000060, 'evts':_evts(1), 'mu':21.500, 'step': 0},
  {'run':310000, 'lb':3, 'starttstamp':1550000120, 'evts':_evts(1), 'mu':22.500, 'step': 0},
  {'run':310000, 'lb':4, 'starttstamp':1550000180, 'evts':_evts(1), 'mu':10.500, 'step': 1},
  {'run':310000, 'lb':5, 'starttstamp':1550000240, 'evts':_evts(1), 'mu':11.500, 'step': 1},
  {'run':310000, 'lb':6, 'starttstamp':1550000300, 'evts':_evts(1), 'mu':1.500, 'step': 2},
  {'run':310000, 'lb':7, 'starttstamp':1550000360, 'evts':_evts(1), 'mu':2.500, 'step': 2},
  {'run':310000, 'lb':8, 'starttstamp':1550000420, 'evts':_evts(1), 'mu':3.500, 'step': 2},
  {'run':310000, 'lb':9, 'starttstamp':1550000480, 'evts':_evts(1), 'mu':4.500, 'step': 2},
  {'run':310000, 'lb':10, 'starttstamp':1550000540, 'evts':_evts(1), 'mu':5.500, 'step': 2},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

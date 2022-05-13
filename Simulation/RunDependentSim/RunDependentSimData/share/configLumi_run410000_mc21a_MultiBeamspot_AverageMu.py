# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
    {'run':410000, 'lb':1, 'starttstamp':1625000060, 'evts':_evts(140), 'mu':51.5, 'step':0},
    {'run':410000, 'lb':2, 'starttstamp':1625000120, 'evts':_evts(140), 'mu':52.5, 'step':0},
    {'run':410000, 'lb':1001, 'starttstamp':1625060060, 'evts':_evts(140), 'mu':51.5, 'step':1},
    {'run':410000, 'lb':1002, 'starttstamp':1625060120, 'evts':_evts(140), 'mu':52.5, 'step':1},
    {'run':410000, 'lb':2001, 'starttstamp':1625120060, 'evts':_evts(140), 'mu':51.5, 'step':2},
    {'run':410000, 'lb':2002, 'starttstamp':1625120120, 'evts':_evts(140), 'mu':52.5, 'step':2},
    {'run':410000, 'lb':3001, 'starttstamp':1625180060, 'evts':_evts(43), 'mu':25.5, 'step':3},
    {'run':410000, 'lb':3002, 'starttstamp':1625180120, 'evts':_evts(43), 'mu':26.5, 'step':3},
    {'run':410000, 'lb':3003, 'starttstamp':1625180180, 'evts':_evts(43), 'mu':27.5, 'step':3},
    {'run':410000, 'lb':3004, 'starttstamp':1625180240, 'evts':_evts(43), 'mu':28.5, 'step':3},
    {'run':410000, 'lb':3005, 'starttstamp':1625180300, 'evts':_evts(43), 'mu':29.5, 'step':3},
    {'run':410000, 'lb':3006, 'starttstamp':1625180360, 'evts':_evts(43), 'mu':30.5, 'step':3},
    {'run':410000, 'lb':3007, 'starttstamp':1625180420, 'evts':_evts(43), 'mu':31.5, 'step':3},
    {'run':410000, 'lb':3008, 'starttstamp':1625180480, 'evts':_evts(43), 'mu':32.5, 'step':3},
    {'run':410000, 'lb':3009, 'starttstamp':1625180540, 'evts':_evts(43), 'mu':33.5, 'step':3},
    {'run':410000, 'lb':3010, 'starttstamp':1625180600, 'evts':_evts(43), 'mu':34.5, 'step':3},
    {'run':410000, 'lb':3011, 'starttstamp':1625180660, 'evts':_evts(43), 'mu':35.5, 'step':3},
    {'run':410000, 'lb':3012, 'starttstamp':1625180720, 'evts':_evts(43), 'mu':36.5, 'step':3},
    {'run':410000, 'lb':3013, 'starttstamp':1625180780, 'evts':_evts(42), 'mu':37.5, 'step':3},
    {'run':410000, 'lb':3014, 'starttstamp':1625180840, 'evts':_evts(43), 'mu':38.5, 'step':3},
    {'run':410000, 'lb':3015, 'starttstamp':1625180900, 'evts':_evts(43), 'mu':39.5, 'step':3},
    {'run':410000, 'lb':3016, 'starttstamp':1625180960, 'evts':_evts(43), 'mu':40.5, 'step':3},
    {'run':410000, 'lb':3017, 'starttstamp':1625181020, 'evts':_evts(43), 'mu':41.5, 'step':3},
    {'run':410000, 'lb':3018, 'starttstamp':1625181080, 'evts':_evts(43), 'mu':42.5, 'step':3},
    {'run':410000, 'lb':3019, 'starttstamp':1625181140, 'evts':_evts(43), 'mu':43.5, 'step':3},
    {'run':410000, 'lb':3020, 'starttstamp':1625181200, 'evts':_evts(43), 'mu':44.5, 'step':3},
    {'run':410000, 'lb':3021, 'starttstamp':1625181260, 'evts':_evts(43), 'mu':45.5, 'step':3},
    {'run':410000, 'lb':3022, 'starttstamp':1625181320, 'evts':_evts(43), 'mu':46.5, 'step':3},
    {'run':410000, 'lb':3023, 'starttstamp':1625181380, 'evts':_evts(43), 'mu':47.5, 'step':3},
    {'run':410000, 'lb':3024, 'starttstamp':1625181440, 'evts':_evts(43), 'mu':48.5, 'step':3},
    {'run':410000, 'lb':3025, 'starttstamp':1625181500, 'evts':_evts(43), 'mu':49.5, 'step':3},
    {'run':410000, 'lb':3026, 'starttstamp':1625181560, 'evts':_evts(43), 'mu':50.5, 'step':3},
    {'run':410000, 'lb':3027, 'starttstamp':1625181620, 'evts':_evts(43), 'mu':51.5, 'step':3},
    #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

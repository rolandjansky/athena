# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir(): ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
  {'run':410000, 'lb':1, 'starttstamp':1625000000, 'evts':_evts(71), 'mu':25.0},
  {'run':410000, 'lb':2, 'starttstamp':1625000060, 'evts':_evts(71), 'mu':26.0},
  {'run':410000, 'lb':3, 'starttstamp':1625000120, 'evts':_evts(71), 'mu':27.0},
  {'run':410000, 'lb':4, 'starttstamp':1625000180, 'evts':_evts(71), 'mu':28.0},
  {'run':410000, 'lb':5, 'starttstamp':1625000240, 'evts':_evts(71), 'mu':29.0},
  {'run':410000, 'lb':6, 'starttstamp':1625000300, 'evts':_evts(71), 'mu':30.0},
  {'run':410000, 'lb':7, 'starttstamp':1625000360, 'evts':_evts(71), 'mu':31.0},
  {'run':410000, 'lb':8, 'starttstamp':1625000420, 'evts':_evts(71), 'mu':32.0},
  {'run':410000, 'lb':9, 'starttstamp':1625000480, 'evts':_evts(71), 'mu':33.0},
  {'run':410000, 'lb':10, 'starttstamp':1625000540, 'evts':_evts(71), 'mu':34.0},
  {'run':410000, 'lb':11, 'starttstamp':1625000600, 'evts':_evts(71), 'mu':35.0},
  {'run':410000, 'lb':12, 'starttstamp':1625000660, 'evts':_evts(71), 'mu':36.0},
  {'run':410000, 'lb':13, 'starttstamp':1625000720, 'evts':_evts(71), 'mu':37.0},
  {'run':410000, 'lb':14, 'starttstamp':1625000780, 'evts':_evts(71), 'mu':38.0},
  {'run':410000, 'lb':15, 'starttstamp':1625000840, 'evts':_evts(71), 'mu':39.0},
  {'run':410000, 'lb':16, 'starttstamp':1625000900, 'evts':_evts(71), 'mu':40.0},
  {'run':410000, 'lb':17, 'starttstamp':1625000960, 'evts':_evts(72), 'mu':41.0},
  {'run':410000, 'lb':18, 'starttstamp':1625001020, 'evts':_evts(72), 'mu':42.0},
  {'run':410000, 'lb':19, 'starttstamp':1625001080, 'evts':_evts(72), 'mu':43.0},
  {'run':410000, 'lb':20, 'starttstamp':1625001140, 'evts':_evts(72), 'mu':44.0},
  {'run':410000, 'lb':21, 'starttstamp':1625001200, 'evts':_evts(72), 'mu':45.0},
  {'run':410000, 'lb':22, 'starttstamp':1625001260, 'evts':_evts(72), 'mu':46.0},
  {'run':410000, 'lb':23, 'starttstamp':1625001320, 'evts':_evts(72), 'mu':47.0},
  {'run':410000, 'lb':24, 'starttstamp':1625001380, 'evts':_evts(72), 'mu':48.0},
  {'run':410000, 'lb':25, 'starttstamp':1625001440, 'evts':_evts(72), 'mu':49.0},
  {'run':410000, 'lb':26, 'starttstamp':1625001500, 'evts':_evts(72), 'mu':50.0},
  {'run':410000, 'lb':27, 'starttstamp':1625001625, 'evts':_evts(72), 'mu':51.0},
  {'run':410000, 'lb':28, 'starttstamp':1625001620, 'evts':_evts(72), 'mu':52.0},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
  del JobMaker

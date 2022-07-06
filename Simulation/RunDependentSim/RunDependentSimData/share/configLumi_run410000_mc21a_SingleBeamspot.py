# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Equivalent of configLumi_run410000_mc21a_MultiBeamspot.py, but
# without the division into different beam spots.

# We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
    {'run':410000, 'lb':1, 'starttstamp':1625000060, 'evts':_evts(1),  'mu':15.5},
    {'run':410000, 'lb':2, 'starttstamp':1625000120, 'evts':_evts(2),  'mu':16.5},
    {'run':410000, 'lb':3, 'starttstamp':1625000180, 'evts':_evts(5),  'mu':17.5},
    {'run':410000, 'lb':4, 'starttstamp':1625000240, 'evts':_evts(9),  'mu':18.5},
    {'run':410000, 'lb':5, 'starttstamp':1625000300, 'evts':_evts(13), 'mu':19.5},
    {'run':410000, 'lb':6, 'starttstamp':1625000360, 'evts':_evts(18), 'mu':20.5},
    {'run':410000, 'lb':7, 'starttstamp':1625000420, 'evts':_evts(22), 'mu':21.5},
    {'run':410000, 'lb':8, 'starttstamp':1625000480, 'evts':_evts(24), 'mu':22.5},
    {'run':410000, 'lb':9, 'starttstamp':1625000540, 'evts':_evts(28), 'mu':23.5},
    {'run':410000, 'lb':10, 'starttstamp':1625000600, 'evts':_evts(27), 'mu':24.5},
    {'run':410000, 'lb':11, 'starttstamp':1625000660, 'evts':_evts(31), 'mu':25.5},
    {'run':410000, 'lb':12, 'starttstamp':1625000720, 'evts':_evts(32), 'mu':26.5},
    {'run':410000, 'lb':13, 'starttstamp':1625000780, 'evts':_evts(33), 'mu':27.5},
    {'run':410000, 'lb':14, 'starttstamp':1625000840, 'evts':_evts(38), 'mu':28.5},
    {'run':410000, 'lb':15, 'starttstamp':1625000900, 'evts':_evts(36), 'mu':29.5},
    {'run':410000, 'lb':16, 'starttstamp':1625000960, 'evts':_evts(38), 'mu':30.5},
    {'run':410000, 'lb':17, 'starttstamp':1625001020, 'evts':_evts(42), 'mu':31.5},
    {'run':410000, 'lb':18, 'starttstamp':1625001080, 'evts':_evts(38), 'mu':32.5},
    {'run':410000, 'lb':19, 'starttstamp':1625001140, 'evts':_evts(45), 'mu':33.5},
    {'run':410000, 'lb':20, 'starttstamp':1625001200, 'evts':_evts(43), 'mu':34.5},
    {'run':410000, 'lb':21, 'starttstamp':1625001260, 'evts':_evts(39), 'mu':35.5},
    {'run':410000, 'lb':22, 'starttstamp':1625001320, 'evts':_evts(44), 'mu':36.5},
    {'run':410000, 'lb':23, 'starttstamp':1625001380, 'evts':_evts(40), 'mu':37.5},
    {'run':410000, 'lb':24, 'starttstamp':1625001440, 'evts':_evts(37), 'mu':38.5},
    {'run':410000, 'lb':25, 'starttstamp':1625001500, 'evts':_evts(38), 'mu':39.5},
    {'run':410000, 'lb':26, 'starttstamp':1625001560, 'evts':_evts(36), 'mu':40.5},
    {'run':410000, 'lb':27, 'starttstamp':1625001620, 'evts':_evts(36), 'mu':41.5},
    {'run':410000, 'lb':28, 'starttstamp':1625001680, 'evts':_evts(36), 'mu':42.5},
    {'run':410000, 'lb':29, 'starttstamp':1625001740, 'evts':_evts(38), 'mu':43.5},
    {'run':410000, 'lb':30, 'starttstamp':1625001800, 'evts':_evts(41), 'mu':44.5},
    {'run':410000, 'lb':31, 'starttstamp':1625001860, 'evts':_evts(53), 'mu':45.5},
    {'run':410000, 'lb':32, 'starttstamp':1625001920, 'evts':_evts(57), 'mu':46.5},
    {'run':410000, 'lb':33, 'starttstamp':1625001980, 'evts':_evts(64), 'mu':47.5},
    {'run':410000, 'lb':34, 'starttstamp':1625002040, 'evts':_evts(68), 'mu':48.5},
    {'run':410000, 'lb':35, 'starttstamp':1625002100, 'evts':_evts(74), 'mu':49.5},
    {'run':410000, 'lb':36, 'starttstamp':1625002160, 'evts':_evts(80), 'mu':50.5},
    {'run':410000, 'lb':37, 'starttstamp':1625002220, 'evts':_evts(86), 'mu':51.5},
    {'run':410000, 'lb':38, 'starttstamp':1625002280, 'evts':_evts(91), 'mu':52.5},
    {'run':410000, 'lb':39, 'starttstamp':1625002340, 'evts':_evts(81), 'mu':53.5},
    {'run':410000, 'lb':40, 'starttstamp':1625002400, 'evts':_evts(73), 'mu':54.5},
    {'run':410000, 'lb':41, 'starttstamp':1625002460, 'evts':_evts(67), 'mu':55.5},
    {'run':410000, 'lb':42, 'starttstamp':1625002520, 'evts':_evts(55), 'mu':56.5},
    {'run':410000, 'lb':43, 'starttstamp':1625002580, 'evts':_evts(46), 'mu':57.5},
    {'run':410000, 'lb':44, 'starttstamp':1625002640, 'evts':_evts(33), 'mu':58.5},
    {'run':410000, 'lb':45, 'starttstamp':1625002700, 'evts':_evts(31), 'mu':59.5},
    {'run':410000, 'lb':46, 'starttstamp':1625002760, 'evts':_evts(24), 'mu':60.5},
    {'run':410000, 'lb':47, 'starttstamp':1625002820, 'evts':_evts(16), 'mu':61.5},
    {'run':410000, 'lb':48, 'starttstamp':1625002880, 'evts':_evts(16), 'mu':62.5},
    {'run':410000, 'lb':49, 'starttstamp':1625002940, 'evts':_evts(13), 'mu':63.5},
    {'run':410000, 'lb':50, 'starttstamp':1625003000, 'evts':_evts(9),  'mu':64.5},
    {'run':410000, 'lb':51, 'starttstamp':1625003060, 'evts':_evts(9),  'mu':65.5},
    {'run':410000, 'lb':52, 'starttstamp':1625003120, 'evts':_evts(8),  'mu':66.5},
    {'run':410000, 'lb':53, 'starttstamp':1625003180, 'evts':_evts(5),  'mu':67.5},
    {'run':410000, 'lb':54, 'starttstamp':1625003240, 'evts':_evts(4),  'mu':68.5},
    {'run':410000, 'lb':55, 'starttstamp':1625003300, 'evts':_evts(4),  'mu':69.5},
    {'run':410000, 'lb':56, 'starttstamp':1625003360, 'evts':_evts(4),  'mu':70.5},
    {'run':410000, 'lb':57, 'starttstamp':1625003420, 'evts':_evts(3),  'mu':71.5},
    {'run':410000, 'lb':58, 'starttstamp':1625003480, 'evts':_evts(2),  'mu':72.5},
    {'run':410000, 'lb':59, 'starttstamp':1625003540, 'evts':_evts(2),  'mu':73.5},
    {'run':410000, 'lb':60, 'starttstamp':1625003600, 'evts':_evts(2),  'mu':74.5},
    {'run':410000, 'lb':61, 'starttstamp':1625003660, 'evts':_evts(1),  'mu':75.5},
    {'run':410000, 'lb':62, 'starttstamp':1625003720, 'evts':_evts(1),  'mu':76.5},
    {'run':410000, 'lb':63, 'starttstamp':1625003780, 'evts':_evts(1),  'mu':77.5},
    {'run':410000, 'lb':64, 'starttstamp':1625003840, 'evts':_evts(1),  'mu':78.5},
    {'run':410000, 'lb':65, 'starttstamp':1625003900, 'evts':_evts(1),  'mu':79.5},
    {'run':410000, 'lb':66, 'starttstamp':1625003960, 'evts':_evts(1),  'mu':80.5},
    {'run':410000, 'lb':67, 'starttstamp':1625004020, 'evts':_evts(1),  'mu':81.5},
    {'run':410000, 'lb':68, 'starttstamp':1625004080, 'evts':_evts(1),  'mu':82.5},
    {'run':410000, 'lb':69, 'starttstamp':1625004140, 'evts':_evts(1),  'mu':83.5},
    {'run':410000, 'lb':70, 'starttstamp':1625004200, 'evts':_evts(1),  'mu':84.5},
    #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

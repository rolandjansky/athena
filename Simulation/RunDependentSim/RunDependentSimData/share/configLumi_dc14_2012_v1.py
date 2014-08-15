####################
## File configLumi_dc14_2012_v1.py: hacked from configLumi_mc12_v2.py
## Created on Thu Feb 04 18:26:00 2014
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
   {'run':212272, 'lb':1, 'starttstamp':1328039085, 'dt':0.000, 'evts':_evts(5), 'mu':0.0000001, 'force_new':False},
   {'run':212272, 'lb':2, 'starttstamp':1328039152, 'dt':0.000, 'evts':_evts(5), 'mu':1.000, 'force_new':False},
   {'run':212272, 'lb':3, 'starttstamp':1328039213, 'dt':0.000, 'evts':_evts(5), 'mu':2.000, 'force_new':False},
   {'run':212272, 'lb':4, 'starttstamp':1328039274, 'dt':0.000, 'evts':_evts(5), 'mu':3.000, 'force_new':False},
   {'run':212272, 'lb':5, 'starttstamp':1328039335, 'dt':0.000, 'evts':_evts(5), 'mu':4.000, 'force_new':False},
   {'run':212272, 'lb':6, 'starttstamp':1328039396, 'dt':0.000, 'evts':_evts(5), 'mu':5.000, 'force_new':False},
   {'run':212272, 'lb':7, 'starttstamp':1328039457, 'dt':0.000, 'evts':_evts(5), 'mu':6.000, 'force_new':False},
   {'run':212272, 'lb':8, 'starttstamp':1328039518, 'dt':0.000, 'evts':_evts(18), 'mu':7.000, 'force_new':False},
   {'run':212272, 'lb':9, 'starttstamp':1328039579, 'dt':0.000, 'evts':_evts(47), 'mu':8.000, 'force_new':False},
   {'run':212272, 'lb':10, 'starttstamp':1328039640, 'dt':0.000, 'evts':_evts(103), 'mu':9.000, 'force_new':False},
   {'run':212272, 'lb':11, 'starttstamp':1328039701, 'dt':0.000, 'evts':_evts(147), 'mu':10.000, 'force_new':False},
   {'run':212272, 'lb':12, 'starttstamp':1328039762, 'dt':0.000, 'evts':_evts(205), 'mu':11.000, 'force_new':False},
   {'run':212272, 'lb':13, 'starttstamp':1328039823, 'dt':0.000, 'evts':_evts(248), 'mu':12.000, 'force_new':False},
   {'run':212272, 'lb':14, 'starttstamp':1328039884, 'dt':0.000, 'evts':_evts(276), 'mu':13.000, 'force_new':False},
   {'run':212272, 'lb':15, 'starttstamp':1328039945, 'dt':0.000, 'evts':_evts(285), 'mu':14.000, 'force_new':False},
   {'run':212272, 'lb':16, 'starttstamp':1328040006, 'dt':0.000, 'evts':_evts(303), 'mu':15.000, 'force_new':False},
   {'run':212272, 'lb':17, 'starttstamp':1328040067, 'dt':0.000, 'evts':_evts(303), 'mu':16.000, 'force_new':False},
   {'run':212272, 'lb':18, 'starttstamp':1328040128, 'dt':0.000, 'evts':_evts(303), 'mu':17.000, 'force_new':False},
   {'run':212272, 'lb':19, 'starttstamp':1328040189, 'dt':0.000, 'evts':_evts(303), 'mu':18.000, 'force_new':False},
   {'run':212272, 'lb':20, 'starttstamp':1328040250, 'dt':0.000, 'evts':_evts(293), 'mu':19.000, 'force_new':False},
   {'run':212272, 'lb':21, 'starttstamp':1328040311, 'dt':0.000, 'evts':_evts(282), 'mu':20.000, 'force_new':False},
   {'run':212272, 'lb':22, 'starttstamp':1328040372, 'dt':0.000, 'evts':_evts(275), 'mu':21.000, 'force_new':False},
   {'run':212272, 'lb':23, 'starttstamp':1328040433, 'dt':0.000, 'evts':_evts(249), 'mu':22.000, 'force_new':False},
   {'run':212272, 'lb':24, 'starttstamp':1328040494, 'dt':0.000, 'evts':_evts(236), 'mu':23.000, 'force_new':False},
   {'run':212272, 'lb':25, 'starttstamp':1328040555, 'dt':0.000, 'evts':_evts(210), 'mu':24.000, 'force_new':False},
   {'run':212272, 'lb':26, 'starttstamp':1328040616, 'dt':0.000, 'evts':_evts(185), 'mu':25.000, 'force_new':False},
   {'run':212272, 'lb':27, 'starttstamp':1328040677, 'dt':0.000, 'evts':_evts(156), 'mu':26.000, 'force_new':False},
   {'run':212272, 'lb':28, 'starttstamp':1328040738, 'dt':0.000, 'evts':_evts(141), 'mu':27.000, 'force_new':False},
   {'run':212272, 'lb':29, 'starttstamp':1328040799, 'dt':0.000, 'evts':_evts(105), 'mu':28.000, 'force_new':False},
   {'run':212272, 'lb':30, 'starttstamp':1328040860, 'dt':0.000, 'evts':_evts(88), 'mu':29.000, 'force_new':False},
   {'run':212272, 'lb':31, 'starttstamp':1328040921, 'dt':0.000, 'evts':_evts(65), 'mu':30.000, 'force_new':False},
   {'run':212272, 'lb':32, 'starttstamp':1328040982, 'dt':0.000, 'evts':_evts(51), 'mu':31.000, 'force_new':False},
   {'run':212272, 'lb':33, 'starttstamp':1328041043, 'dt':0.000, 'evts':_evts(31), 'mu':32.000, 'force_new':False},
   {'run':212272, 'lb':34, 'starttstamp':1328041104, 'dt':0.000, 'evts':_evts(18), 'mu':33.000, 'force_new':False},
   {'run':212272, 'lb':35, 'starttstamp':1328041165, 'dt':0.000, 'evts':_evts(16), 'mu':34.000, 'force_new':False},
   {'run':212272, 'lb':36, 'starttstamp':1328041226, 'dt':0.000, 'evts':_evts(9), 'mu':35.000, 'force_new':False},
   {'run':212272, 'lb':37, 'starttstamp':1328041287, 'dt':0.000, 'evts':_evts(6), 'mu':36.000, 'force_new':False},
   {'run':212272, 'lb':38, 'starttstamp':1328041348, 'dt':0.000, 'evts':_evts(2), 'mu':37.000, 'force_new':False},
   {'run':212272, 'lb':39, 'starttstamp':1328041409, 'dt':0.000, 'evts':_evts(2), 'mu':38.000, 'force_new':False},
   {'run':212272, 'lb':40, 'starttstamp':1328041470, 'dt':0.000, 'evts':_evts(2), 'mu':39.000, 'force_new':False},
   {'run':212272, 'lb':41, 'starttstamp':1328041531, 'dt':0.000, 'evts':_evts(2), 'mu':40.000, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

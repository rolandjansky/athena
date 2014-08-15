####################
## Merge of two configuration files
## Created on Wed Dec 14 22:10:13 2011
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
   {'run':190728, 'lb':108, 'starttstamp':1318253359, 'dt':0.000, 'evts':_evts(32), 'mu':20.000, 'force_new':False},
   {'run':190728, 'lb':109, 'starttstamp':1318253421, 'dt':0.000, 'evts':_evts(32), 'mu':20.500, 'force_new':False},
   {'run':190728, 'lb':110, 'starttstamp':1318253481, 'dt':0.000, 'evts':_evts(32), 'mu':21.000, 'force_new':False},
   {'run':190728, 'lb':111, 'starttstamp':1318253542, 'dt':0.000, 'evts':_evts(32), 'mu':21.500, 'force_new':False},
   {'run':190728, 'lb':112, 'starttstamp':1318253603, 'dt':0.000, 'evts':_evts(32), 'mu':22.000, 'force_new':False},
   {'run':190728, 'lb':113, 'starttstamp':1318253663, 'dt':0.000, 'evts':_evts(32), 'mu':22.500, 'force_new':False},
   {'run':190728, 'lb':114, 'starttstamp':1318253724, 'dt':0.000, 'evts':_evts(32), 'mu':23.000, 'force_new':False},
   {'run':190728, 'lb':115, 'starttstamp':1318253785, 'dt':0.000, 'evts':_evts(32), 'mu':23.500, 'force_new':False},
   {'run':190728, 'lb':116, 'starttstamp':1318253797, 'dt':0.000, 'evts':_evts(32), 'mu':24.000, 'force_new':False},
   {'run':190728, 'lb':117, 'starttstamp':1318253857, 'dt':0.000, 'evts':_evts(32), 'mu':24.500, 'force_new':False},
   {'run':190728, 'lb':118, 'starttstamp':1318253918, 'dt':0.000, 'evts':_evts(32), 'mu':25.000, 'force_new':False},
   {'run':190728, 'lb':119, 'starttstamp':1318253979, 'dt':0.000, 'evts':_evts(32), 'mu':25.500, 'force_new':False},
   {'run':190728, 'lb':120, 'starttstamp':1318254041, 'dt':0.000, 'evts':_evts(32), 'mu':26.000, 'force_new':False},
   {'run':190728, 'lb':121, 'starttstamp':1318254101, 'dt':0.000, 'evts':_evts(32), 'mu':26.500, 'force_new':False},
   {'run':190728, 'lb':122, 'starttstamp':1318254162, 'dt':0.000, 'evts':_evts(32), 'mu':27.000, 'force_new':False},
   {'run':190728, 'lb':123, 'starttstamp':1318254173, 'dt':0.000, 'evts':_evts(32), 'mu':27.500, 'force_new':False},
   {'run':190728, 'lb':124, 'starttstamp':1318254234, 'dt':0.000, 'evts':_evts(32), 'mu':28.000, 'force_new':False},
   {'run':190728, 'lb':125, 'starttstamp':1318254295, 'dt':0.000, 'evts':_evts(32), 'mu':28.500, 'force_new':False},
   {'run':190728, 'lb':126, 'starttstamp':1318254356, 'dt':0.000, 'evts':_evts(32), 'mu':29.000, 'force_new':False},
   {'run':190728, 'lb':127, 'starttstamp':1318254417, 'dt':0.000, 'evts':_evts(32), 'mu':29.500, 'force_new':False},
   {'run':191628, 'lb':126, 'starttstamp':1319552726, 'dt':0.000, 'evts':_evts(218), 'mu':23.000, 'force_new':False},
   {'run':191628, 'lb':127, 'starttstamp':1319552787, 'dt':0.000, 'evts':_evts(218), 'mu':23.500, 'force_new':False},
   {'run':191628, 'lb':128, 'starttstamp':1319552848, 'dt':0.000, 'evts':_evts(218), 'mu':24.000, 'force_new':False},
   {'run':191628, 'lb':129, 'starttstamp':1319552909, 'dt':0.000, 'evts':_evts(218), 'mu':24.500, 'force_new':False},
   {'run':191628, 'lb':130, 'starttstamp':1319552970, 'dt':0.000, 'evts':_evts(218), 'mu':25.000, 'force_new':False},
   {'run':191628, 'lb':131, 'starttstamp':1319553031, 'dt':0.000, 'evts':_evts(218), 'mu':25.500, 'force_new':False},
   {'run':191628, 'lb':132, 'starttstamp':1319553092, 'dt':0.000, 'evts':_evts(218), 'mu':26.000, 'force_new':False},
   {'run':191628, 'lb':133, 'starttstamp':1319553153, 'dt':0.000, 'evts':_evts(218), 'mu':26.500, 'force_new':False},
   {'run':191628, 'lb':134, 'starttstamp':1319553214, 'dt':0.000, 'evts':_evts(218), 'mu':27.000, 'force_new':False},
   {'run':191628, 'lb':135, 'starttstamp':1319553275, 'dt':0.000, 'evts':_evts(218), 'mu':27.500, 'force_new':False},
   {'run':191628, 'lb':136, 'starttstamp':1319553336, 'dt':0.000, 'evts':_evts(218), 'mu':28.000, 'force_new':False},
   {'run':191628, 'lb':137, 'starttstamp':1319553397, 'dt':0.000, 'evts':_evts(218), 'mu':28.500, 'force_new':False},
   {'run':191628, 'lb':138, 'starttstamp':1319553458, 'dt':0.000, 'evts':_evts(218), 'mu':29.000, 'force_new':False},
   {'run':191628, 'lb':139, 'starttstamp':1319553519, 'dt':0.000, 'evts':_evts(218), 'mu':29.500, 'force_new':False},
   {'run':191628, 'lb':140, 'starttstamp':1319553580, 'dt':0.000, 'evts':_evts(218), 'mu':30.000, 'force_new':False},
   {'run':191628, 'lb':141, 'starttstamp':1319553641, 'dt':0.000, 'evts':_evts(218), 'mu':30.500, 'force_new':False},
   {'run':191628, 'lb':142, 'starttstamp':1319553702, 'dt':0.000, 'evts':_evts(218), 'mu':31.000, 'force_new':False},
   {'run':191628, 'lb':143, 'starttstamp':1319553763, 'dt':0.000, 'evts':_evts(218), 'mu':31.500, 'force_new':False},
   {'run':191628, 'lb':144, 'starttstamp':1319553824, 'dt':0.000, 'evts':_evts(218), 'mu':32.000, 'force_new':False},
   {'run':191628, 'lb':145, 'starttstamp':1319553885, 'dt':0.000, 'evts':_evts(218), 'mu':32.500, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

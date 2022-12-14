####################
## File configLumi_run219114.py: autogenerated configuration file from command
##/cvmfs/atlas-nightlies.cern.ch/repo/sw/nightlies/x86_64-slc6-gcc47-opt/devval/rel_1/AtlasSimulation/rel_1/InstallArea/share/bin/RunDepTaskMaker.py --externalDict={0.0:24.0,1.0:31.0, 2.0:38.0,3.0:45.0, 4.0:52.0,5.0:59.0, 6.0:65.0, 7.0:72.0, 8.0:79.0, 9.0:86.0, 10.0:93.0, 11.0:99.0, 12.0:105.0, 13.0:112.0, 14.0:119.0, 15.0:126.0, 16.0:133.0, 17.0:141.0, 18.0:147.0, 19.0:154.0, 20.0:161.0, 21.0:161.0, 22.0:161.0, 23.0:161.0, 24.0:161.0, 25.0:161.0, 26.0:161.0, 27.0:161.0, 28.0:161.0, 29.0:161.0, 30.0:161.0, 31.0:161.0, 32.0:161.0, 33.0:161.0, 34.0:161.0, 35.0:161.0, 36.0:161.0, 37.0:161.0, 38.0:161.0, 39.0:161.0} --lumimethod=EXTERNAL --nMC=1 --trigger=L1_MBTS_2 --outfile=configLumi_run219114.py run219114.xml
## Created on Tue May 20 18:26:16 2014
####################
#Run-dependent digi job configuration file.
#RunDependentSimData/OverrideRunLBLumiDigitConfig.py

# WARNING timestamps are derived ad hoc to put them outside run-1 or run-2 periods! 

#We need to be able to adjust for different dataset sizes.
if not 'ScaleTaskLength' in dir():   ScaleTaskLength = 1
_evts = lambda x: int(ScaleTaskLength * x)

if not 'logging' in dir(): import logging
digilog = logging.getLogger('Digi_trf')
digilog.info('doing RunLumiOverride configuration from file.')
JobMaker=[
    {'run':222525, 'lb':1, 'starttstamp':1376703331, 'dt':0.000, 'evts':_evts(5), 'mu':0.5, 'force_new':False},
    {'run':222525, 'lb':2, 'starttstamp':1376703392, 'dt':0.000, 'evts':_evts(5), 'mu':1.5, 'force_new':False},
    {'run':222525, 'lb':3, 'starttstamp':1376703453, 'dt':0.000, 'evts':_evts(5), 'mu':2.5, 'force_new':False},
    {'run':222525, 'lb':4, 'starttstamp':1376703514, 'dt':0.000, 'evts':_evts(5), 'mu':3.5, 'force_new':False},
    {'run':222525, 'lb':5, 'starttstamp':1376703575, 'dt':0.000, 'evts':_evts(5), 'mu':4.5, 'force_new':False},
    {'run':222525, 'lb':6, 'starttstamp':1376703636, 'dt':0.000, 'evts':_evts(8), 'mu':5.5, 'force_new':False},
    {'run':222525, 'lb':7, 'starttstamp':1376703697, 'dt':0.000, 'evts':_evts(21), 'mu':6.5, 'force_new':False},
    {'run':222525, 'lb':8, 'starttstamp':1376703758, 'dt':0.000, 'evts':_evts(45), 'mu':7.5, 'force_new':False},
    {'run':222525, 'lb':9, 'starttstamp':1376703819, 'dt':0.000, 'evts':_evts(80), 'mu':8.5, 'force_new':False},
    {'run':222525, 'lb':10, 'starttstamp':1376703880, 'dt':0.000, 'evts':_evts(132), 'mu':9.5, 'force_new':False},
    {'run':222525, 'lb':11, 'starttstamp':1376703941, 'dt':0.000, 'evts':_evts(183), 'mu':10.5, 'force_new':False},
    {'run':222525, 'lb':12, 'starttstamp':1376704002, 'dt':0.000, 'evts':_evts(209), 'mu':11.5, 'force_new':False},
    {'run':222525, 'lb':13, 'starttstamp':1376704063, 'dt':0.000, 'evts':_evts(209), 'mu':12.5, 'force_new':False},
    {'run':222525, 'lb':14, 'starttstamp':1376704124, 'dt':0.000, 'evts':_evts(209), 'mu':13.5, 'force_new':False},
    {'run':222525, 'lb':15, 'starttstamp':1376704185, 'dt':0.000, 'evts':_evts(209), 'mu':14.5, 'force_new':False},
    {'run':222525, 'lb':16, 'starttstamp':1376704246, 'dt':0.000, 'evts':_evts(209), 'mu':15.5, 'force_new':False},
    {'run':222525, 'lb':17, 'starttstamp':1376704307, 'dt':0.000, 'evts':_evts(209), 'mu':16.5, 'force_new':False},
    {'run':222525, 'lb':18, 'starttstamp':1376704368, 'dt':0.000, 'evts':_evts(209), 'mu':17.5, 'force_new':False},
    {'run':222525, 'lb':19, 'starttstamp':1376704429, 'dt':0.000, 'evts':_evts(209), 'mu':18.5, 'force_new':False},
    {'run':222525, 'lb':20, 'starttstamp':1376704490, 'dt':0.000, 'evts':_evts(209), 'mu':19.5, 'force_new':False},
    {'run':222525, 'lb':21, 'starttstamp':1376704551, 'dt':0.000, 'evts':_evts(209), 'mu':20.5, 'force_new':False},
    {'run':222525, 'lb':22, 'starttstamp':1376704612, 'dt':0.000, 'evts':_evts(209), 'mu':21.5, 'force_new':False},
    {'run':222525, 'lb':23, 'starttstamp':1376704673, 'dt':0.000, 'evts':_evts(209), 'mu':22.5, 'force_new':False},
    {'run':222525, 'lb':24, 'starttstamp':1376704734, 'dt':0.000, 'evts':_evts(209), 'mu':23.5, 'force_new':False},
    {'run':222525, 'lb':25, 'starttstamp':1376704795, 'dt':0.000, 'evts':_evts(209), 'mu':24.5, 'force_new':False},
    {'run':222525, 'lb':26, 'starttstamp':1376704856, 'dt':0.000, 'evts':_evts(209), 'mu':25.5, 'force_new':False},
    {'run':222525, 'lb':27, 'starttstamp':1376704917, 'dt':0.000, 'evts':_evts(209), 'mu':26.5, 'force_new':False},
    {'run':222525, 'lb':28, 'starttstamp':1376704978, 'dt':0.000, 'evts':_evts(207), 'mu':27.5, 'force_new':False},
    {'run':222525, 'lb':29, 'starttstamp':1376705039, 'dt':0.000, 'evts':_evts(190), 'mu':28.5, 'force_new':False},
    {'run':222525, 'lb':30, 'starttstamp':1376705100, 'dt':0.000, 'evts':_evts(166), 'mu':29.5, 'force_new':False},
    {'run':222525, 'lb':31, 'starttstamp':1376705161, 'dt':0.000, 'evts':_evts(141), 'mu':30.5, 'force_new':False},
    {'run':222525, 'lb':32, 'starttstamp':1376705222, 'dt':0.000, 'evts':_evts(115), 'mu':31.5, 'force_new':False},
    {'run':222525, 'lb':33, 'starttstamp':1376705283, 'dt':0.000, 'evts':_evts(90), 'mu':32.5, 'force_new':False},
    {'run':222525, 'lb':34, 'starttstamp':1376705344, 'dt':0.000, 'evts':_evts(67), 'mu':33.5, 'force_new':False},
    {'run':222525, 'lb':35, 'starttstamp':1376705405, 'dt':0.000, 'evts':_evts(48), 'mu':34.5, 'force_new':False},
    {'run':222525, 'lb':36, 'starttstamp':1376705466, 'dt':0.000, 'evts':_evts(33), 'mu':35.5, 'force_new':False},
    {'run':222525, 'lb':37, 'starttstamp':1376705527, 'dt':0.000, 'evts':_evts(21), 'mu':36.5, 'force_new':False},
    {'run':222525, 'lb':38, 'starttstamp':1376705588, 'dt':0.000, 'evts':_evts(13), 'mu':37.5, 'force_new':False},
    {'run':222525, 'lb':39, 'starttstamp':1376705649, 'dt':0.000, 'evts':_evts(8), 'mu':38.5, 'force_new':False},
    {'run':222525, 'lb':40, 'starttstamp':1376705710, 'dt':0.000, 'evts':_evts(5), 'mu':39.5, 'force_new':False},
    {'run':222525, 'lb':41, 'starttstamp':1376705771, 'dt':0.000, 'evts':_evts(5), 'mu':40.5, 'force_new':False},
    {'run':222525, 'lb':42, 'starttstamp':1376705832, 'dt':0.000, 'evts':_evts(5), 'mu':41.5, 'force_new':False},
    {'run':222525, 'lb':43, 'starttstamp':1376705893, 'dt':0.000, 'evts':_evts(5), 'mu':42.5, 'force_new':False},
    {'run':222525, 'lb':44, 'starttstamp':1376705954, 'dt':0.000, 'evts':_evts(5), 'mu':43.5, 'force_new':False},
    {'run':222525, 'lb':45, 'starttstamp':1376706015, 'dt':0.000, 'evts':_evts(5), 'mu':44.5, 'force_new':False},
    {'run':222525, 'lb':46, 'starttstamp':1376706076, 'dt':0.000, 'evts':_evts(5), 'mu':45.5, 'force_new':False},
    {'run':222525, 'lb':47, 'starttstamp':1376706137, 'dt':0.000, 'evts':_evts(5), 'mu':46.5, 'force_new':False},
    {'run':222525, 'lb':48, 'starttstamp':1376706198, 'dt':0.000, 'evts':_evts(5), 'mu':47.5, 'force_new':False},
    {'run':222525, 'lb':49, 'starttstamp':1376706259, 'dt':0.000, 'evts':_evts(5), 'mu':48.5, 'force_new':False},
    {'run':222525, 'lb':50, 'starttstamp':1376706320, 'dt':0.000, 'evts':_evts(4), 'mu':49.5, 'force_new':False}
    #--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

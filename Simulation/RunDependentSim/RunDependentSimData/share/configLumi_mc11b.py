####################
## File configLumi_mc11b.py: merged config file
## Created on Thu Oct 27 17:13:27 2011
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
    {'run':180164, 'lb':67, 'starttstamp':1303554669, 'dt':0.000, 'evts':_evts(27), 'mu':4.750, 'force_new':False},
    {'run':180164, 'lb':68, 'starttstamp':1303554730, 'dt':0.000, 'evts':_evts(33), 'mu':5.750, 'force_new':False},
    {'run':180164, 'lb':69, 'starttstamp':1303554791, 'dt':0.000, 'evts':_evts(2), 'mu':7.250, 'force_new':False},
    {'run':180164, 'lb':70, 'starttstamp':1303554852, 'dt':0.000, 'evts':_evts(12), 'mu':6.750, 'force_new':False},
    {'run':180164, 'lb':71, 'starttstamp':1303554913, 'dt':0.000, 'evts':_evts(36), 'mu':5.250, 'force_new':False},
    {'run':180164, 'lb':73, 'starttstamp':1303555035, 'dt':0.000, 'evts':_evts(20), 'mu':4.250, 'force_new':False},
    {'run':180164, 'lb':74, 'starttstamp':1303555062, 'dt':0.000, 'evts':_evts(25), 'mu':6.250, 'force_new':False},
    {'run':180164, 'lb':75, 'starttstamp':1303555090, 'dt':0.000, 'evts':_evts(4), 'mu':3.750, 'force_new':False},
    {'run':180164, 'lb':76, 'starttstamp':1303555151, 'dt':0.000, 'evts':_evts(1), 'mu':7.750, 'force_new':False},
    {'run':183003, 'lb':177, 'starttstamp':1306973316, 'dt':0.000, 'evts':_evts(117), 'mu':4.750, 'force_new':False},
    {'run':183003, 'lb':178, 'starttstamp':1306973377, 'dt':0.000, 'evts':_evts(6), 'mu':3.250, 'force_new':False},
    {'run':183003, 'lb':179, 'starttstamp':1306973438, 'dt':0.000, 'evts':_evts(145), 'mu':6.250, 'force_new':False},
    {'run':183003, 'lb':180, 'starttstamp':1306973499, 'dt':0.000, 'evts':_evts(93), 'mu':4.250, 'force_new':False},
    {'run':183003, 'lb':181, 'starttstamp':1306973560, 'dt':0.000, 'evts':_evts(166), 'mu':5.750, 'force_new':False},
    {'run':183003, 'lb':182, 'starttstamp':1306973621, 'dt':0.000, 'evts':_evts(32), 'mu':3.750, 'force_new':False},
    {'run':183003, 'lb':183, 'starttstamp':1306973682, 'dt':0.000, 'evts':_evts(1), 'mu':8.250, 'force_new':False},
    {'run':183003, 'lb':184, 'starttstamp':1306973743, 'dt':0.000, 'evts':_evts(45), 'mu':7.250, 'force_new':False},
    {'run':183003, 'lb':185, 'starttstamp':1306973779, 'dt':0.000, 'evts':_evts(14), 'mu':7.750, 'force_new':False},
    {'run':183003, 'lb':186, 'starttstamp':1306973840, 'dt':0.000, 'evts':_evts(153), 'mu':5.250, 'force_new':False},
    {'run':183003, 'lb':187, 'starttstamp':1306973901, 'dt':0.000, 'evts':_evts(100), 'mu':6.750, 'force_new':False},
    {'run':186169, 'lb':139, 'starttstamp':1311379706, 'dt':0.000, 'evts':_evts(103), 'mu':4.750, 'force_new':False},
    {'run':186169, 'lb':140, 'starttstamp':1311379766, 'dt':0.000, 'evts':_evts(13), 'mu':3.250, 'force_new':False},
    {'run':186169, 'lb':141, 'starttstamp':1311379827, 'dt':0.000, 'evts':_evts(70), 'mu':4.250, 'force_new':False},
    {'run':186169, 'lb':142, 'starttstamp':1311379888, 'dt':0.000, 'evts':_evts(111), 'mu':8.250, 'force_new':False},
    {'run':186169, 'lb':143, 'starttstamp':1311379949, 'dt':0.000, 'evts':_evts(100), 'mu':8.750, 'force_new':False},
    {'run':186169, 'lb':144, 'starttstamp':1311380010, 'dt':0.000, 'evts':_evts(73), 'mu':9.250, 'force_new':False},
    {'run':186169, 'lb':145, 'starttstamp':1311380071, 'dt':0.000, 'evts':_evts(45), 'mu':9.750, 'force_new':False},
    {'run':186169, 'lb':146, 'starttstamp':1311380132, 'dt':0.000, 'evts':_evts(11), 'mu':10.250, 'force_new':False},
    {'run':186169, 'lb':147, 'starttstamp':1311380193, 'dt':0.000, 'evts':_evts(34), 'mu':3.750, 'force_new':False},
    {'run':186169, 'lb':148, 'starttstamp':1311380254, 'dt':0.000, 'evts':_evts(4), 'mu':10.750, 'force_new':False},
    {'run':186169, 'lb':149, 'starttstamp':1311380315, 'dt':0.000, 'evts':_evts(2), 'mu':11.250, 'force_new':False},
    {'run':186169, 'lb':150, 'starttstamp':1311380376, 'dt':0.000, 'evts':_evts(110), 'mu':5.250, 'force_new':False},
    {'run':186169, 'lb':151, 'starttstamp':1311380437, 'dt':0.000, 'evts':_evts(130), 'mu':7.750, 'force_new':False},
    {'run':186169, 'lb':152, 'starttstamp':1311380498, 'dt':0.000, 'evts':_evts(110), 'mu':5.750, 'force_new':False},
    {'run':186169, 'lb':153, 'starttstamp':1311380559, 'dt':0.000, 'evts':_evts(125), 'mu':7.250, 'force_new':False},
    {'run':186169, 'lb':154, 'starttstamp':1311380620, 'dt':0.000, 'evts':_evts(126), 'mu':6.250, 'force_new':False},
    {'run':186169, 'lb':155, 'starttstamp':1311380681, 'dt':0.000, 'evts':_evts(4), 'mu':2.750, 'force_new':False},
    {'run':186169, 'lb':156, 'starttstamp':1311380742, 'dt':0.000, 'evts':_evts(120), 'mu':6.750, 'force_new':False},
    {'run':189751, 'lb':116, 'starttstamp':1316868299, 'dt':0.000, 'evts':_evts(85), 'mu':15.250, 'force_new':False},
    {'run':189751, 'lb':117, 'starttstamp':1316868360, 'dt':0.000, 'evts':_evts(47), 'mu':15.750, 'force_new':False},
    {'run':189751, 'lb':118, 'starttstamp':1316868414, 'dt':0.000, 'evts':_evts(189), 'mu':10.250, 'force_new':False},
    {'run':189751, 'lb':119, 'starttstamp':1316868474, 'dt':0.000, 'evts':_evts(115), 'mu':14.750, 'force_new':False},
    {'run':189751, 'lb':120, 'starttstamp':1316868535, 'dt':0.000, 'evts':_evts(132), 'mu':8.750, 'force_new':False},
    {'run':189751, 'lb':121, 'starttstamp':1316868596, 'dt':0.000, 'evts':_evts(147), 'mu':9.750, 'force_new':False},
    {'run':189751, 'lb':122, 'starttstamp':1316868657, 'dt':0.000, 'evts':_evts(186), 'mu':10.750, 'force_new':False},
    {'run':189751, 'lb':123, 'starttstamp':1316868718, 'dt':0.000, 'evts':_evts(198), 'mu':11.750, 'force_new':False},
    {'run':189751, 'lb':124, 'starttstamp':1316868779, 'dt':0.000, 'evts':_evts(172), 'mu':12.750, 'force_new':False},
    {'run':189751, 'lb':125, 'starttstamp':1316868840, 'dt':0.000, 'evts':_evts(51), 'mu':6.750, 'force_new':False},
    {'run':189751, 'lb':126, 'starttstamp':1316868851, 'dt':0.000, 'evts':_evts(153), 'mu':13.750, 'force_new':False},
    {'run':189751, 'lb':127, 'starttstamp':1316868912, 'dt':0.000, 'evts':_evts(24), 'mu':16.250, 'force_new':False},
    {'run':189751, 'lb':128, 'starttstamp':1316868973, 'dt':0.000, 'evts':_evts(153), 'mu':9.250, 'force_new':False},
    {'run':189751, 'lb':129, 'starttstamp':1316869034, 'dt':0.000, 'evts':_evts(4), 'mu':5.750, 'force_new':False},
    {'run':189751, 'lb':130, 'starttstamp':1316869095, 'dt':0.000, 'evts':_evts(31), 'mu':6.250, 'force_new':False},
    {'run':189751, 'lb':131, 'starttstamp':1316869156, 'dt':0.000, 'evts':_evts(122), 'mu':8.250, 'force_new':False},
    {'run':189751, 'lb':132, 'starttstamp':1316869217, 'dt':0.000, 'evts':_evts(117), 'mu':7.750, 'force_new':False},
    {'run':189751, 'lb':133, 'starttstamp':1316869278, 'dt':0.000, 'evts':_evts(180), 'mu':11.250, 'force_new':False},
    {'run':189751, 'lb':134, 'starttstamp':1316869339, 'dt':0.000, 'evts':_evts(6), 'mu':16.750, 'force_new':False},
    {'run':189751, 'lb':135, 'starttstamp':1316869400, 'dt':0.000, 'evts':_evts(181), 'mu':12.250, 'force_new':False},
    {'run':189751, 'lb':136, 'starttstamp':1316869461, 'dt':0.000, 'evts':_evts(164), 'mu':13.250, 'force_new':False},
    {'run':189751, 'lb':137, 'starttstamp':1316869522, 'dt':0.000, 'evts':_evts(128), 'mu':14.250, 'force_new':False},
    {'run':189751, 'lb':138, 'starttstamp':1316869583, 'dt':0.000, 'evts':_evts(92), 'mu':7.250, 'force_new':False},
#--> end hiding
]

include('RunDependentSimData/configCommon.py')

#cleanup python memory
if not "RunDMC_testing_configuration" in dir():
    del JobMaker

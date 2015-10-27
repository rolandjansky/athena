# python script to define the DB for b-tagging performance calibration
# Copied from the same functionality in the JetTagCalibration package
#
# This assumes:
# - a single calibration file for all channels (jet algorithms) / taggers / flavours / operating points
# - overall tag: tagname
# The ROOT file containing the calibrations should be present, with a name like: BTaggingPerformanceMyCalibV1_Cuts.root
# and it must have a GUID defined by the command: 'coolHist_setFileIdentifier.sh filename'.
#
# To run: python defineDB.py
# Results: a local file called mycool.db

import os,commands,sys

os.system('rm -f consolecmds')
f=open('consolecmds', 'w')

tagname = 'MyCalibV1'
hfileprefix='BTaggingPerformance'+tagname+'_'
froot = '/GLOBAL/BTagPerfCalib/'
db = 'OFLP200'

# With the use of the CalibrationBroker's "ShadowFoldersAndChannels" option, there remains a single folder and a single channel.
# Everything else (notably the Jet Collections) does not affect the DB.

folder = 'Cuts'
channel = ' 1 '

longfolder = froot + folder
hfile = hfileprefix + folder + '.root'
longtag = tagname + '_' + folder
cmd = db + ' ' + longfolder + channel + longtag + ' ' + hfile
os.system('coolHist_setReference.py ' + cmd)
smd = 'setchan ' + longfolder + channel + folder
f.write(smd+'\n')
smd = 'settag ' + longfolder + ' ' + longtag + ' ' + tagname
f.write(smd+'\n')

f.write('exit\n')
f.close()

#os.system('AtlCoolConsole.py "sqlite://;schema=mycool.db;dbname=OFLP200" < consolecmds')

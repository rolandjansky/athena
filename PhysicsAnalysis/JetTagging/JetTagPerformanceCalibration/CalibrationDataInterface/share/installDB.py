# python script to install the DB and the root files in the run area
# Copied from the same functionality in the JetTagCalibration package
#
# - first the local database should be created with the script defineDB.py
# - then this script can be run: python installDB.py

theCalibrationTag = "MyCalibV1"

localRootFiles = True
print '---> Installing performance calibration files for b-tagging. Tag is ',theCalibrationTag
import os,commands,sys

folders = [ 'Cuts' ]

for folder in folders:
  longname = 'BTaggingPerformance'+theCalibrationTag+'_'+folder+'.root'
  if localRootFiles:
    print '---> Inserting file ',longname
  else:
    print '---> Getting and inserting file ',longname
    os.system('get_files -data '+longname)
  os.system('coolHist_insertFileToCatalog.py '+longname)

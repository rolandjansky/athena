# python script to install the DB and the root files in the run area
# - first the local database should be created with the script defineDB.py
# - then this script can be run: python installLocalDB.py
# NB: if only a specific tagger is needed, change the folders list accordingly
# LV 2008/01/19


theCalibrationTag = "MyCalibV1"

localRootFiles = True
print '---> Installing calibration files for b-tagging. Tag is ',theCalibrationTag
import os,commands,sys

folders = [ 'IP1D','IP2D','IP3D','SV1','SV2','SoftMu','SoftEl','JetFitter','JetProb' ]

jets = [ 'Cone4H1Topo','Cone4H1Tower','Cone7H1Topo','Cone7H1Tower','Kt4H1Topo','Kt4H1Tower','Kt6H1Topo','Kt6H1Tower','Cone4Truth','Cone7Truth','Kt4Truth','Kt6Truth' ]

for folder in folders:
  longname = 'BTagging'+theCalibrationTag+'_'+folder
  if (folder != 'JetFitter'):
    longname = longname + '.root'
    if localRootFiles:
      print '---> Inserting file ',longname
    else:
      print '---> Getting and inserting file ',longname
      os.system('get_files -data '+longname)
    os.system('coolHist_insertFileToCatalog.py '+longname)
  else:
    for jet in jets:
      longname2 = longname+'_'+jet+'ParticleJets.root'
      if localRootFiles:
        print '---> Inserting file ',longname2
      else:
        print '---> Getting and inserting file ',longname2
        os.system('get_files -data '+longname2)
      os.system('coolHist_insertFileToCatalog.py '+longname2)


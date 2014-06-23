# python script to define the DB for b-tagging calibration
# This assumes:
# - the same calibration files for all the channels (jet algorithms)
# - similar tag names: tagname_TAGGER for each folder
# - overall tag: tagname
# All root files containing the calibrations should be present, with a name like: BTaggingMyCalibV1_JetFitter.root
# and they must have a GUID defined by the command: 'coolHist_setFileIdentifier.sh filename'.
# To run: python defineDB.py
# Results: a local file called mycool.db
# NB: the creation of the complete DB takes about 10mn. If only a specific tagger is run, 
#     change the folders list accordingly to save time. Ditto for the jet collections.
# LV 2007/12/18

import os,commands,sys

os.system('rm -f consolecmds')
f=open('consolecmds', 'w')

tagname = 'MyCalibV1'
hfileprefix='BTagging'+tagname+'_'
froot = '/BTAG/CALIB/'
db = 'OFLP200'

jets = [ 'Cone4H1Topo','Cone4H1Tower','Cone7H1Topo','Cone7H1Tower','Kt4H1Topo','Kt4H1Tower','Kt6H1Topo','Kt6H1Tower','Cone4Truth','Cone7Truth','Kt4Truth','Kt6Truth' ]

folders = [ 'IP1D','IP2D','IP3D','SV1','SV2','SoftMu','SoftEl','JetFitter','JetProb' ]

ichan=1

for jet in jets:

  print 'Channel',ichan,':',jet
  
  for folder in folders:
    longfolder=froot+folder
    if (folder!='JetFitter'):
      hfile=hfileprefix+folder+'.root'
    else:
      hfile=hfileprefix+folder+'_'+jet+'ParticleJets.root'
    longtag=tagname+'_'+folder
    cmd=db+' '+longfolder+' '+str(ichan)+' '+longtag+' '+hfile
    print 'Setting ref for',jet,':',cmd
    os.system('coolHist_setReference.py '+cmd)
    smd='setchan '+longfolder+' '+str(ichan)+' '+jet
    print smd
    f.write(smd+'\n')
    if ichan == 1:
      smd='settag '+longfolder+' '+longtag+' '+tagname
      print smd
      f.write(smd+'\n')

  ichan = ichan + 1

f.write('exit\n')
f.close()

#os.system('AtlCoolConsole.py "sqlite://;schema=mycool.db;dbname=OFLP200" < consolecmds')

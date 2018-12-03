#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadCellNoiseFromCoolCompare.py
# based on ReadCellNoiseFromCool.py and ReadFromCoolCompare.py
# Yuri Smirnov 2015-03-19
# 

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps noise constants from online or offline folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-s, --schema=   specify schema to use, ONL or OFL for RUN1 or ONL2 or OFL2 for RUN2 or MC"
    print "-f, --folder=   specify status folder to use f.i. /TILE/OFL02/NOISE/CELL or /CALO/Noise/CellNoise "
    print "-t, --tag=      specify tag to use, f.i. UPD1 or UPD4 or tag suffix like 14TeV-N200_dT50-01"
    print "-r, --run=      specify run  number, by default uses latest iov"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-s2, --schema2=   specify schema2 to use, ONL or OFL for RUN1 or ONL2 or OFL2 for RUN2 or MC"
    print "-f2, --folder2=   specify status folder2 to use f.i. /TILE/OFL02/NOISE/CELL or /CALO/Noise/CellNoise "
    print "-t2, --tag2=      specify tag2 to use, f.i. UPD1 or UPD4 or tag suffix like 14TeV-N200_dT50-01"
    print "-r2, --run2=      specify 2-nd run number, by default uses latest iov"
    print "-l2, --lumi2=     specify 2-nd lumi block number, default is 0"
    print "-m,  --maxdiff=        specify an absolute maximal difference to compare constants"
    print "-m2, --maxdiffpercent=   specify the maximal difference in percents to compare constants"
    print "-n, --channel=  specify cool channel to read (48 by defalt)"
    print "-c, --cell=     specify cell hash (0-5183), default is -1, means all cells"
    print "-g, --gain=     specify gain to print (0-3), default is -1, means all gains"
    print "-i, --index=    specify parameter index (0-4), default is -1, means all parameters"
    print "-b, --brief     print only numbers without character names"
    print "-d, --double    print values with double precision"
    
letters = "hs:t:f:r:l:s2:t2:f2:r2:l2:m:m2:n:c:g:i:bd"
keywords = ["help","schema=","tag=","folder=","run=","lumi=","schema2=","tag2=","folder2=","run2=","lumi2=","maxdiff=","maxdiffpercent=","channel=","cell=","gain=","index=","brief","double"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run    = 2147483647
run2   = 0      # will be set to "run" if not on command line
lumi   = 0
lumi2  = -1     # will be set to "lumi" if not on command line
schema = 'OFL2'
schema2 = "none"
folderPath = '/TILE/OFL02/NOISE/CELL'
folderPath2 = "none" # will be set to "folderPath" if not on command line
tag    = 'UPD4'
tag2   = "none"
maxdiff=-1.0 # dump all values
maxdiffpercent=-1.0
chan   = 48 # represents Tile
cell   = -1
gain   = -1
index  = -1
brief  = False
doubl  = False

for o, a in opts:
    if o in ("-s","--schema"):
        schema = a
    elif o in ("-s2","--schema2"):
        schema2 = a
    elif o in ("-f","--folder"):
        folderPath = a
    elif o in ("-f2","--folder2"):
        folderPath2 = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-t2","--tag2"):
        tag2 = a
    elif o in ("-n","--channel"):
        chan = int(a)
    elif o in ("-c","--cell"):
        cell = int(a)
    elif o in ("-g","--gain"):
        gain = int(a)
    elif o in ("-i","--index"):
        index = int(a)
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-r2","--run2"):
        run2 = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-l2","--lumi2"):
        lumi2 = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ('--maxdiff'):
        maxdiff = float(a)
    elif o in ('--maxdiffpercent'):
        maxdiffpercent = float(a)
    elif o in ("-b","--brief"):
        brief = True
    elif o in ("-d","--double"):
        doubl = True
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        usage()
        sys.exit(2)

if not run2:
    run2=run
if lumi2<0:
    lumi2=lumi
if folderPath2=="none":
    folderPath2=folderPath
if tag2=="none":
    tag2=tag
if schema2=="none":
    schema2=schema
        
tile=(chan==48)

import cppyy

from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileCellTools
hashMgr=TileCellTools.TileCellHashMgr()

#=== get a logger
log = CaloCondLogger.getLogger("ReadCellNoise")

#=== Read from COOL server 1:

if schema=='ONL': # shortcut for COOLONL_CALO/COMP200
    schema='COOLONL_CALO/COMP200'
    folderPath='/CALO/Noise/CellNoise'
    if tag=='UPD4': tag='UPD1-00' # change default to latest RUN1 tag
elif schema=='ONL2': # shortcut for COOLONL_CALO/CONDBR2
    schema='COOLONL_CALO/CONDBR2'
    folderPath='/CALO/Noise/CellNoise'
    if tag=='UPD4': tag='RUN2-UPD1-00' # change default to latest RUN2 tag
elif schema=='OFL': # shortcut for COOLOFL_TILE/COMP200 or COOLOFL_LAR/COMP200
    if chan!=48:
        schema='COOLOFL_LAR/COMP200'
        folderPath='/LAR/NoiseOfl/CellNoise'
        if tag=='UPD4': tag='UPD4-02' # change default to latest RUN1 tag
    else:
        schema='COOLOFL_TILE/COMP200'
        folderPath='/TILE/OFL02/NOISE/CELL'
        if tag=='UPD4': tag='UPD4-10' # change default to latest RUN1 tag
elif schema=='OFL2': # shortcut for COOLOFL_TILE/CONDBR2 or COOLOFL_LAR/CONDBR2
    if chan!=48:
        schema='COOLOFL_LAR/CONDBR2'
        folderPath='/LAR/NoiseOfl/CellNoise'
    else:
        schema='COOLOFL_TILE/CONDBR2'
        folderPath='/TILE/OFL02/NOISE/CELL'
elif schema=='ONLMC': # shortcut for COOLONL_CALO/OFLP200 
    schema='COOLONL_CALO/OFLP200'
    folderPath='/CALO/Noise/CellNoise'
    if tag=='UPD4': tag='IOVDEP-10' # change default to tag used in DC14
elif schema=='OFLMC': # shortcut for COOLOFL_CALO/OFLP200
    schema='COOLOFL_CALO/OFLP200'
    folderPath='/CALO/Ofl/Noise/CellNoise'
    if tag=='UPD4': tag='IOVDEP-10' # change default to tag used in DC14
elif schema=='MC': # shortcut for COOLOFL_TILE/OFLP200 or COOLOFL_LAR/OFLP200
    if chan!=48:
        schema='COOLOFL_LAR/OFLP200'
        folderPath='/LAR/NoiseOfl/CellNoise'
        if tag=='UPD4': tag='IOVDEP-00' # change default to tag used in DC14
    else:
        schema='COOLOFL_TILE/OFLP200'
        folderPath='/TILE/OFL02/NOISE/CELL'
        if tag=='UPD4': tag='IOVDEP-02' # change default to tag used in DC14

db = CaloCondTools.openDbConn(schema, "READONLY")

if folderPath.startswith('/TILE') or tag=='UPD1' or tag=='UPD4' or 'COND'in tag:
    folderTag = TileCalibTools.getFolderTag(db, folderPath, tag )
elif folderPath.startswith('/CALO/Ofl'):
    folderTag = 'CaloOflNoiseCellnoise-'+tag
elif folderPath.startswith('/CALO'):
    folderTag = 'CaloNoiseCellnoise-'+tag
elif folderPath.startswith('/LAR'):
    folderTag = 'LARNoiseOflCellNoise-'+tag

#=== Read from COOL server 2:                                                                                                                                                  

if schema2=='ONL': # shortcut for COOLONL_CALO/COMP200                                                                                                                          
    schema2='COOLONL_CALO/COMP200'
    folderPath2='/CALO/Noise/CellNoise'
    if tag2=='UPD4': tag2='UPD1-00' # change default to latest RUN1 tag                                                                                                          
elif schema2=='ONL2': # shortcut for COOLONL_CALO/CONDBR2                                                                                                                       
    schema2='COOLONL_CALO/CONDBR2'
    folderPath2='/CALO/Noise/CellNoise'
    if tag2=='UPD4': tag2='RUN2-UPD1-00' # change default to latest RUN2 tag                                                                                                     
elif schema2=='OFL': # shortcut for COOLOFL_TILE/COMP200 or COOLOFL_LAR/COMP200                                                                                                 
    if chan!=48:
        schema2='COOLOFL_LAR/COMP200'
        folderPath2='/LAR/NoiseOfl/CellNoise'
        if tag2=='UPD4': tag2='UPD4-02' # change default to latest RUN1 tag                                                                                                      
    else:
        schema2='COOLOFL_TILE/COMP200'
        folderPath='/TILE/OFL02/NOISE/CELL'
        if tag2=='UPD4': tag2='UPD4-10' # change default to latest RUN1 tag                                                                                                      
elif schema2=='OFL2': # shortcut for COOLOFL_TILE/CONDBR2 or COOLOFL_LAR/CONDBR2                                                                                                
    if chan!=48:
        schema2='COOLOFL_LAR/CONDBR2'
        folderPath2='/LAR/NoiseOfl/CellNoise'
    else:
        schema2='COOLOFL_TILE/CONDBR2'
        folderPath2='/TILE/OFL02/NOISE/CELL'
elif schema2=='ONLMC': # shortcut for COOLONL_CALO/OFLP200                                                                                                                      
    schema2='COOLONL_CALO/OFLP200'
    folderPath2='/CALO/Noise/CellNoise'
    if tag2=='UPD4': tag2='IOVDEP-10' # change default to tag used in DC14                                                                                                       
elif schema2=='OFLMC': # shortcut for COOLOFL_CALO/OFLP200                                                                                                                      
    schema2='COOLOFL_CALO/OFLP200'
    folderPath2='/CALO/Ofl/Noise/CellNoise'
    if tag2=='UPD4': tag2='IOVDEP-10' # change default to tag used in DC14                                                                                                       
elif schema2=='MC': # shortcut for COOLOFL_TILE/OFLP200 or COOLOFL_LAR/OFLP200                                                                                                  
    if chan!=48:
        schema2='COOLOFL_LAR/OFLP200'
        folderPath2='/LAR/NoiseOfl/CellNoise'
        if tag2=='UPD4': tag2='IOVDEP-00' # change default to tag used in DC14                                                                                                   
    else:
        schema2='COOLOFL_TILE/OFLP200'
        folderPath2='/TILE/OFL02/NOISE/CELL'
        if tag2=='UPD4': tag2='IOVDEP-02' # change default to tag used in DC14                                                                                                   

db2 = CaloCondTools.openDbConn(schema2, "READONLY")

if folderPath2.startswith('/TILE') or tag2=='UPD1' or tag2=='UPD4' or 'COND'in tag2:
    folderTag2 = TileCalibTools.getFolderTag(db2, folderPath2, tag2 )
elif folderPath2.startswith('/CALO/Ofl'):
    folderTag2 = 'CaloOflNoiseCellnoise-'+tag2
elif folderPath2.startswith('/CALO'):
    folderTag2 = 'CaloNoiseCellnoise-'+tag2
elif folderPath.startswith('/LAR'):
    folderTag2 = 'LARNoiseOflCellNoise-'+tag2


log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))
log.info("Initializing folder2 %s with tag2 %s" % (folderPath2, folderTag2))

folder = db.getFolder(folderPath)
folder2 = db2.getFolder(folderPath2)

#=== get the blob for a given tag and iov
iov = CaloCondTools.iovFromRunLumi( run, lumi )
iov2 = CaloCondTools.iovFromRunLumi( run2, lumi2 )

obj = folder.findObject( iov, chan, folderTag )
obj2 = folder2.findObject( iov2, chan, folderTag2 )

blob = obj.payload()[0]
blob2 = obj2.payload()[0]

#=== create CaloCondBlobFlt
blobFlt = cppyy.gbl.CaloCondBlobFlt.getInstance(blob)
blobFlt2 = cppyy.gbl.CaloCondBlobFlt.getInstance(blob2)

#=== retrieve data from the blob
#cell  = 0 # 0..5183 - Tile hash
#gain  = 0 # 0..3    - four Tile cell gains: -11, -12, -15, -16
#index = 0 # 0..4    - electronic or pile-up noise or 2-G noise parameters

ncell=blobFlt.getNChans()
ngain=blobFlt.getNGains()
nval=blobFlt.getObjSizeUint32()

if cell<0 or cell>=ncell:
    cellmin=0
    cellmax=ncell
else:
    cellmin=cell
    cellmax=cell+1

if gain<0 or gain>=ngain:
    gainmin=0
    gainmax=ngain
else:
    gainmin=gain
    gainmax=gain+1

if index<0 or index>=nval:
    indexmin=0
    indexmax=nval
else:
    indexmin=index
    indexmax=index+1

log.info("From DB:  ncell: %d ngain %d index nval %d" % (ncell, ngain, nval))

if brief or doubl:
  name1 = ["","","0.0     "]
  names = []


  dm=" "
  for i in xrange(indexmax): names += [""]
else:
  name1 = ["Noise cell ", "gain ","0.00"]
  names = ["RMS ", "pileup ", "RMS1 ", "RMS2 ", "Ratio "]
  for i in xrange(len(names),indexmax): names += ["c"+str(i)+" "]
  dm="\t"
for cell in xrange(cellmin,cellmax):
  if tile and len(name1[0]):
    name1[0] = "%s %6s hash " % hashMgr.getNames(cell) 
  for gain in xrange(gainmin,gainmax):
    msg="%s%4d %s%d\t" % ( name1[0], cell, name1[1], gain)
    for index in xrange(indexmin,indexmax):
      v=blobFlt.getData(cell, gain, index)
      v2=blobFlt2.getData(cell, gain, index)
      dv12 = abs(v - v2)
      if v == 0:
          dv12percent=0
      else:
          dv12percent=dv12*100/v

      #if doubl: msg += "%s%s%s" % (names[index],"{0:<15.10g}".format(v),dm)
      #elif v<5.e-7: msg += "%s%s%s" % (names[index],name1[2],dm)
      #elif v<1: msg += "%s%8.6f%s" % (names[index],v,dm)
      #else: msg += "%s%s%s" % (names[index],"{0:<8.7g}".format(v),dm)

      if abs(dv12) > maxdiff and abs(dv12percent) > maxdiffpercent:      
         if doubl: msg += "%s v1 %s v2 %s diff %s diffpercent %s%s" % (names[index],"{0:<15.10g}".format(v),"{0:<15.10g}".format(v2),"{0:<15.10g}".format(dv12),"{0:<15.10g}".format(dv12percent),dm)
         elif v<5.e-7: msg += "%s v1 %s v2 %s diff %s diffpercent %s%s" % (names[index],name1[2],"{0:<8.7g}".format(v2),"{0:<8.7g}".format(dv12),"{0:<8.7g}".format(dv12percent),dm)
         elif v<1: msg += "%s v1 %8.6f v2 %8.6f diff %8.6f diffpercent %8.6f%s" % (names[index],v,v2,dv12,dv12percent,dm)
         else: msg += "%s v1 %s v2 %s diff %s diffpercent %s%s" % (names[index],"{0:<8.7g}".format(v),"{0:<8.7g}".format(v2),"{0:<8.7g}".format(dv12),"{0:<8.7g}".format(dv12percent),dm)

    print msg

#=== close DB
db.closeDatabase()
db2.closeDatabase()

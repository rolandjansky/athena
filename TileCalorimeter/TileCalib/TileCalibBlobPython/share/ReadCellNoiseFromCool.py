#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# ReadFloatFromCaloCool.py
# Carlos.Solans <Carlos.Solans@cern.ch>
# Each Tile cell has 5 values stored in COOL.
# The first two values are the RMS of a sigle gaussian model of the electronic noise
# and the pile-up noise normalized at 10^33cm-2s-1, (backwards compatibility)
# The next three values are used for a two gaussian model. 
# These are: ratio between first and second gaussian, RMS of the first gaussian, and RMS of the second gaussian
# change Yuri Smirnov <iouri.smirnov@cern.ch> 2014-12-24

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
    print "-n, --channel=  specify cool channel to read (48 by defalt)"
    print "-c, --cell=     specify cell hash (0-5183), default is -1, means all cells"
    print "-g, --gain=     specify gain to print (0-3), default is -1, means all gains"
    print "-i, --index=    specify parameter index (0-4), default is -1, means all parameters"
    print "-b, --brief     print only numbers without character names"
    print "-d, --double    print values with double precision"
    
letters = "hs:t:f:r:l:n:c:g:i:bd"
keywords = ["help","schema=","tag=","folder=","run=","lumi=","channel=","cell=","gain=","index=","brief","double"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run    = 2147483647
lumi   = 0
schema = 'OFL2'
folderPath = '/TILE/OFL02/NOISE/CELL'
tag    = 'UPD4'
chan   = 48 # represents Tile
cell   = -1
gain   = -1
index  = -1
brief  = False
doubl  = False

for o, a in opts:
    if o in ("-s","--schema"):
        schema = a
    elif o in ("-f","--folder"):
        folderPath = a
    elif o in ("-t","--tag"):
        tag = a
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
    elif o in ("-l","--lumi"):
        lumi = int(a)
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
        
tile=(chan==48)

import cppyy

from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileCellTools
hashMgr=None
hashMgrDef=TileCellTools.TileCellHashMgr()
hashMgrA=TileCellTools.TileCellHashMgr("UpgradeA")
hashMgrBC=TileCellTools.TileCellHashMgr("UpgradeBC")
hashMgrABC=TileCellTools.TileCellHashMgr("UpgradeABC")

#=== get a logger
log = CaloCondLogger.getLogger("ReadCellNoise")

#=== Read from COOL server:

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
    else:
        schema='COOLOFL_TILE/OFLP200'
        folderPath='/TILE/OFL02/NOISE/CELL'
    if tag=='UPD4': tag='OFLCOND-RUN12-SDR-31' # change default to tag used in MC15

db = CaloCondTools.openDbConn(schema, "READONLY")

if folderPath.startswith('/TILE') or tag=='UPD1' or tag=='UPD4' or 'COND'in tag:
    folderTag = TileCalibTools.getFolderTag(db, folderPath, tag )
elif folderPath.startswith('/CALO/Ofl'):
    folderTag = 'CaloOflNoiseCellnoise-'+tag
elif folderPath.startswith('/CALO'):
    folderTag = 'CaloNoiseCellnoise-'+tag
elif folderPath.startswith('/LAR'):
    folderTag = 'LARNoiseOflCellNoise-'+tag

log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

folder = db.getFolder(folderPath)

#=== get the blob for a given tag and iov
iov = CaloCondTools.iovFromRunLumi( run, lumi )

obj = folder.findObject( iov, chan, folderTag )
blob = obj.payload()[0]

#=== create CaloCondBlobFlt
blobFlt = cppyy.gbl.CaloCondBlobFlt.getInstance(blob)

#=== retrieve data from the blob
#cell  = 0 # 0..5183 - Tile hash
#gain  = 0 # 0..3    - four Tile cell gains: -11, -12, -15, -16
#index = 0 # 0..4    - electronic or pile-up noise or 2-G noise parameters

ncell=blobFlt.getNChans()
ngain=blobFlt.getNGains()
nval=blobFlt.getObjSizeUint32()

if ncell>hashMgrA.getHashMax():
    hashMgr=hashMgrABC
elif ncell>hashMgrBC.getHashMax():
    hashMgr=hashMgrA
elif ncell>hashMgrDef.getHashMax():
    hashMgr=hashMgrBC
else:
    hashMgr=hashMgrDef
log.info("Using %s CellMgr with hashMax %d" % (hashMgr.getGeometry(),hashMgr.getHashMax()))

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
      if doubl: msg += "%s%s%s" % (names[index],"{0:<15.10g}".format(v),dm)
      elif v<5.e-7: msg += "%s%s%s" % (names[index],name1[2],dm)
      elif v<1: msg += "%s%8.6f%s" % (names[index],v,dm)
      else: msg += "%s%s%s" % (names[index],"{0:<8.7g}".format(v),dm)
    print msg

#=== close DB
db.closeDatabase()

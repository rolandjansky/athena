#!/usr/bin/env python
# ReadFloatFromCaloCool.py
# Carlos.Solans <Carlos.Solans@cern.ch>
# Each Tile cell has 5 values stored in COOL.
# The first two values are the RMS of a sigle gaussian model of the electronic noise
# and the pile-up noise normalized at 10^33cm-2s-1, (backwards compatibility)
# The next three values are used for a two gaussian model. 
# These are: ratio between first and second gaussian, RMS of the first gaussian, and RMS of the second gaussian

import getopt,sys,os,string

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps noise constants from online or offline folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-s, --schema=   specify schema to use, ONL or OFL"
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
schema = 'OFL'
folderPath = '/TILE/OFL02/NOISE/CELL'
tag    = 'UPD1'
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
        
import PyCintex
from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger
from TileCalibBlobPython import TileCalibTools

#=== get a logger
log = CaloCondLogger.getLogger("ReadCellNoise")

#=== Read from COOL server:

if schema=='ONL': # shortcut for COOLONL_CALO/COMP200
    schema='COOLONL_CALO/COMP200'
    if folderPath.startswith('/TILE'):
        folderPath='/CALO/Noise/CellNoise'
elif schema=='OFL': # shortcut for COOLOFL_TILE/COMP200
    schema='COOLOFL_TILE/COMP200'
    if folderPath.startswith('/CALO'):
        folderPath='/TILE/OFL02/NOISE/CELL'
elif schema=='ONLMC': # shortcut for COOLONL_CALO/OFLP200 
    schema='COOLONL_CALO/OFLP200'
    if folderPath.startswith('/TILE'):
        folderPath='/CALO/Noise/CellNoise'
    if tag=='UPD1': # change default to tag used in DC14
        tag='IOVDEP-10'
elif schema=='OFLMC': # shortcut for COOLOFL_CALO/OFLP200
    schema='COOLOFL_CALO/OFLP200'
    if folderPath.startswith('/TILE'):
        folderPath='/CALO/Ofl/Noise/CellNoise'
    if tag=='UPD1': # change default to tag used in DC14
        tag='IOVDEP-10'
elif schema=='MC': # shortcut for COOLOFL_TILE/OFLP200
    schema='COOLOFL_TILE/OFLP200'
    if folderPath.startswith('/CALO'):
        folderPath='/TILE/OFL02/NOISE/CELL'
    if tag=='UPD1': # change default to tag used in DC14
        tag='IOVDEP-01'

db = CaloCondTools.openDbConn(schema, "READONLY")

if tag=='UPD1':
    tag='UPD1-00'
if tag=='UPD4':
    sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
    from AtlCoolBKLib import resolveAlias
    gtagUPD4 = resolveAlias.getCurrent().replace('*','')
    log.info("global tag: %s" % gtagUPD4)
    folderTag = TileCalibTools.getFolderTag(db, folderPath, gtagUPD4 )
elif folderPath.startswith('/CALO/Ofl'):
    folderTag = 'CaloOflNoiseCellnoise-'+tag
elif folderPath.startswith('/CALO'):
    folderTag = 'CaloNoiseCellnoise-'+tag
else:
    folderTag = 'TileOfl02NoiseCell-'+tag

log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

folder = db.getFolder(folderPath)

#=== get the blob for a given tag and iov
iov = CaloCondTools.iovFromRunLumi( run, lumi )

obj = folder.findObject( iov, chan, folderTag )
blob = obj.payload()[0]

#=== create CaloCondBlobFlt
blobFlt = PyCintex.gbl.CaloCondBlobFlt.getInstance(blob)

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
  for gain in xrange(gainmin,gainmax):
    msg="%s%d %s%d\t" % ( name1[0], cell, name1[1], gain)
    for index in xrange(indexmin,indexmax):
      v=blobFlt.getData(cell, gain, index)
      if doubl: msg += "%s%s%s" % (names[index],"{0:<15.10g}".format(v),dm)
      elif v<5.e-7: msg += "%s%s%s" % (names[index],name1[2],dm)
      elif v<1: msg += "%s%8.6f%s" % (names[index],v,dm)
      else: msg += "%s%s%s" % (names[index],"{0:<8.7g}".format(v),dm)
    print msg

#=== close DB
db.closeDatabase()

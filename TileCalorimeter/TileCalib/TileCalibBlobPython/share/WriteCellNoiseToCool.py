#!/usr/bin/env python
#
# File:    WriteCellNoiseToCool.py
# Purpose: Manual update of cell noise constants from ascii file
#
# 2014-07-14 - Sasha, based on update_noise_bulk.py from Carlos,Gui,Blake,Yuri
#

import getopt,sys,os,string,math

#=== Get config from args

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Updates Cell Noise database using new values from ASCII file"
    print ""
    print "-h, --help      shows this help"
    print "-i, --infile=   specify the input sqlite file or full schema string"
    print "-o, --outfile=  specify the output sqlite file"
    print "-a, --intag=    specify the input tag"
    print "-g, --outtag=   specify the output tag"
    print "-f, --folder=   specify status folder to use e.g. /TILE/OFL02/NOISE/CELL "
    print "-d, --dbname=   specify the database name e.g. OFLP200"
    print "-t, --txtfile=  specify the text file with the new noise constants"
    print "-r, --run=      specify run number for start of IOV"
    print "-l, --lumi=     specify lumiblock number for start of IOV"
    print "if run number and lumiblock number are omitted - all IOVs from input file are updated"

letters = "hi:o:a:g:f:d:t:r:l:"
keywords = ["help","infile=","outfile=","intag=","outtag=","folder=","dbname=","txtfile=","run=","lumi="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults
inFile      = ''
outFile     = ''
inTag       = ''
outTag      = ''
folderPath  = ''
dbName      = ''
txtFile     = ''
run         = -1
lumi        = 0

for o, a in opts:
    if o in ("-i","--infile"):
        inFile = a
    elif o in ("-o","--outfile"):
        outFile = a
    elif o in ("-a","--intag"):
        inTag = a
    elif o in ("-g","--outtag"):
        outTag = a
    elif o in ("-f","--folder"):
        folderPath = a
    elif o in ("-d","--dbname"):
        dbName = a
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-t","--txtfile"):
        txtFile = a
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        print o, a
        usage()
        sys.exit(2)

#=== check presence of all parameters
print ""
if len(inFile)<1:     raise Exception("Please, provide infile (e.g. --infile=tileSqlite.db or --infile=COOLOFL_TILE/OFLP200)")
if len(outFile)<1:    raise Exception("Please, provide outfile (e.g. --outfile=tileSqlite_New.db)")
if len(inTag)<1:      raise Exception("Please, provide intag (e.g. --intag=TileOfl02NoiseCell-IOVDEP-01)")
if len(outTag)<1:     raise Exception("Please, provide outtag (e.g. --outtag=TileOfl02NoiseCell-IOVDEP-01)")
if len(folderPath)<1: raise Exception("Please, provide folder (e.g. --folder=/TILE/OFL02/NOISE/CELL)")
if len(dbName)<1:     raise Exception("Please, provide dbname (e.g. --dbname=OFLP200 or --dbname=COMP200)")

if os.path.isfile(inFile):
  ischema = 'sqlite://;schema='+inFile+';dbname='+dbName
else:
  print "File %s was not found, assuming it's full schema string" % inFile
  ischema = inFile
  # possible strings for inFile:
  # "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_CALO;dbname=OFLP200"
  # "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=OFLP200"
  # COOLOFL_TILE/OFLP200 COOLOFL_TILE/COMP200


import PyCintex
from CaloCondBlobAlgs import CaloCondTools
from TileCalibBlobPython import TileCalibTools

#=== Get list of IOVs by tricking TileCalibTools to read a Calo blob
idb = TileCalibTools.openDbConn(ischema,'READONLY')
iovList = []
try:
  blobReader = TileCalibTools.TileBlobReader(idb,folderPath, inTag)
  dbobjs = blobReader.getDBobjsWithinRange(1, 28)
  if (dbobjs == None): raise Exception("No DB objects retrieved when building IOV list!")
  while dbobjs.goToNext():
    obj = dbobjs.currentRef()
    objsince = obj.since()
    sinceRun = objsince >> 32
    sinceLum = objsince & 0xFFFFFFFF
    since    = (sinceRun, sinceLum)
    objuntil = obj.until()
    untilRun = objuntil >> 32
    untilLum = objuntil & 0xFFFFFFFF
    until    = (untilRun, untilLum)

    iov = (since, until)
    iovList.append(iov)
except:
  print "Warning: can not read IOVs from input DB file"
  if run<0:
    raise Exception("Please, provide run number at command line")
  else:
    print "Using IOV starting run run %d" %run
    since = (run, lumi)
    until = (0xFFFFFFFF, 0xFFFFFFFF)
    iov = (since, until)
    iovList.append(iov)
idb.closeDatabase()

#=== Open DB connections
oschema = 'sqlite://;schema='+outFile+';dbname='+dbName
dbr = CaloCondTools.openDbConn(ischema,'READONLY')
dbw = CaloCondTools.openDbConn(oschema,'RECREATE')
reader = CaloCondTools.CaloBlobReader(dbr,folderPath,inTag)
writer = CaloCondTools.CaloBlobWriter(dbw,folderPath,'Flt',True)

from TileCalibBlobPython.TileCalibTools import MAXRUN, MAXLBK
from math import sqrt

#== read input file
cellData = {}
ival=0
igain=0
icell=0

if len(txtFile):
  try:
    with open(txtFile,"r") as f:
      cellDataText = f.readlines()

    for line in cellDataText:
      fields = line.strip().split()
      #=== ignore empty and comment lines
      if not len(fields)          : continue
      if fields[0].startswith("#"): continue 
    
      cellHash = int(fields[0])
      cellGain = int(fields[1])
      noise    = fields[2:]
      dictKey  = (cellHash,cellGain)
      cellData[dictKey] = noise
      if icell<cellHash: icell=cellHash
      if igain<cellGain: igain=cellGain
      if ival<len(noise): ival=len(noise)
    icell=icell+1
    igain=igain+1
  except:
    raise Exception("Can not read input file %s" % (txtFile))
else:
  print "No input file provided, making copy from input DB to output DB"

nval=ival
ngain=igain
ncell=icell

print "IOV list in input DB:", iovList

#== update only IOVs starting from given run number
if run>=0 and len(iovList)>1:
  print "Updating only IOVs starting from run %d " % run
  start=0
  for iov in iovList:
    until    = iov[1]
    untilRun = until[0]
    if untilRun<run: start+=1
  if start>0: iovList = iovList[start:]
#== update only one IOV from input DB if we are reading numbers from file
if (ncell>0 and nval>2):
  if (run>0):
    if (len(iovList)>1): 
      print "Updating only single IOV"
      iovList = iovList[0:1]
    iov=iovList[0]
    since = (run, lumi)
    until = iov[1]
    iov = (since, until)
    iovList = [ iov ]
  else:
    if (len(iovList)>1): 
      print "Updating only last IOV"
      iovList = iovList[len(iovList)-1:]

#=== loop over all iovs
for iov in iovList:
  
  since    = iov[0]
  sinceRun = since[0]
  sinceLum = since[1]
  
  until    = iov[1]
  untilRun = until[0]
  untilLum = until[1]
  
  print "IOV in output DB [%d,%d]-[%d,%d)" % (sinceRun, sinceLum, untilRun, untilLum)

  blobR = reader.getCells(48,(sinceRun,sinceLum))
  mcell=blobR.getNChans()
  mgain=blobR.getNGains()
  mval=blobR.getObjSizeUint32()

  print "input file: ncell: %d ngain %d nval %d" % (icell, igain, ival)
  print "input db:   ncell: %d ngain %d nval %d" % (mcell, mgain, mval)
  if mcell>ncell: ncell=mcell
  if mgain>ngain: ngain=mgain
  if mval>nval: nval=mval
  print "output db:  ncell: %d ngain %d nval %d" % (ncell, ngain, nval)

  GainDefVec = PyCintex.gbl.std.vector('float')()
  for val in xrange(nval):
    GainDefVec.push_back(0.0)
  defVec = PyCintex.gbl.std.vector('std::vector<float>')()
  for gain in xrange(ngain):
    defVec.push_back(GainDefVec)
  blobW = writer.getCells(48)
  blobW.init(defVec,ncell,1)

  src = ['Default','DB','File']

  try:
    for cell in xrange(ncell):
      exist0 = (cell<mcell)
      for gain in xrange(ngain):
        exist1 = (exist0 and (gain<mgain))
        dictKey = (cell, gain)
        noise = cellData.get(dictKey,[])
        nF = len(noise)
        for field in xrange(nval):
          exist = (exist1 and (field<mval))
          value = GainDefVec[field]
          if field<nF:
            value = float(noise[field])
            if value<0 and exist: # negative value means that we don't want to update this field
              value = blobR.getData(cell,gain,field)
            elif exist:
              exist = 2
          elif exist:
            value = blobR.getData(cell,gain,field)
          blobW.setData( cell, gain, field, value )
          #print "Debug: Cell %d Gain %d Field %d Value %d Source %s" % (cell, gain, field, value, src[exist])
  except:
    print "Exception on IOV [%d,%d]-[%d,%d)" % (sinceRun, sinceLum, untilRun, untilLum)

  writer.register((sinceRun,sinceLum), (untilRun,untilLum), outTag)

#=== Cleanup
dbw.closeDatabase()
dbr.closeDatabase()


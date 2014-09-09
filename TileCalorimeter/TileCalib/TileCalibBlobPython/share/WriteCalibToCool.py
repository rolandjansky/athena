#!/bin/env python
# WriteCalibToCool.py
# Sanya Solodkov 2014-08-29

import getopt,sys,os,string


def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Dumps the TileCal calibrations from various schemas / folders / tags"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN or /TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
    print "-t, --tag=      specify tag to use, f.i. RUN2-HLT-UPD1-00 or RUN2-UPD4-00"
    print "-r, --run=      specify run  number, default is 0"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-c, --channel   if present, means that one constant per channel is expected (i.e. no gain field)"
    print "-d, --default   if present, means that also default values stored in AUX01-AUX20 should be updated"
    print "-a, --all       if present, means that also drawers are saved, otherwise only those which were updated"
    print "-n, --nval=     specify number of values to store to DB, default is 0 - means all"
    print "-x, --txtfile=  specify the text file with the new constants for reading"
    print "-p, --prefix=   specify prefix which is expected on every line in input file, default - no prefix"
    print "-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'"
    print "-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    print "-s, --schema=     specify input/output schema to use when both input and output schemas are the same"
    


letters = "hr:l:s:i:o:t:f:n:x:p:dca"
keywords = ["help","run=","lumi=","schema=","inschema=","outschema=","tag=","folder=","nval=","txtfile=","prefix=","default","channel","all"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetOptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = 0
lumi = 0
schema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
inSchema = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2"
outSchema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
folderPath =  "/TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
tag = "RUN2-HLT-UPD1-00"
readGain=True
rosmin = 1
all=False
nval = 0
txtFile= ""
prefix = ""

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
    elif o in ("-i","--inschema"):
        inSchema = a
    elif o in ("-o","--outschema"):
        outSchema = a
    elif o in ("-n","--nval"):
        nval = int(a)
    elif o in ("-d","--default"):
        rosmin = 0
    elif o in ("-c","--channel"):
        readGain = False
    elif o in ("-a","--all"):
        all = True
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-x","--txtfile"):
        txtFile = a
    elif o in ("-p","--prefix"):
        prefix = a
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

if not len(inSchema): inSchema=schema
if not len(outSchema): outSchema=schema

import PyCintex
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("WriteCalibToCool")
import logging
log.setLevel(logging.DEBUG)

#=== set database
dbr = TileCalibTools.openDbConn(inSchema,'READONLY')
if inSchema==outSchema:
    dbw = TileCalibTools.openDbConn(outSchema,'UPDATE')
else:
    dbw = TileCalibTools.openDbConn(outSchema,'RECREATE')

if "/TILE/ONL01" in folderPath:
    folderTag = ""
elif tag == "UPD4":
    sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
    from AtlCoolBKLib import resolveAlias
    gtagUPD4 = resolveAlias.getCurrent().replace('*','')
    log.info("global tag: %s" % gtagUPD4)
    folderTag = TileCalibTools.getFolderTag(dbr, folderPath, gtagUPD4 )
else:
    folderTag = TileCalibUtils.getFullTag(folderPath, tag )


since = (run, lumi)
until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)

log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== initialize blob reader to read previous comments
blobReader = TileCalibTools.TileBlobReader(dbr,folderPath, folderTag)
#=== get drawer with status at given run
log.info("Initializing for run %d, lumiblock %d" % (run,lumi))
log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

if len(tag): blobWriter = TileCalibTools.TileBlobWriter(dbw,folderPath,'Flt',True)
else: blobWriter = TileCalibTools.TileBlobWriter(dbw,folderPath,'Flt',False)

#=== create default: one number per ADC
default = PyCintex.gbl.std.vector('float')()
for n in xrange(nval):
    default.push_back(0.)

defConst = PyCintex.gbl.std.vector('std::vector<float>')()
defConst.push_back(default) # low  gain
defConst.push_back(default) # high gain

blobParser = TileCalibTools.TileASCIIParser2(txtFile,prefix,readGain);
mval=0

nold=0
nnew=0
#=== initialize defaults
#ros = 0
#for mod in xrange(20):
#    flt = blobWriter.zeroBlob(ros,mod)
#flt = blobWriter.getDrawer(0, 0)
#flt.init(defConst,1,0)        
#=== loop over whole detector
for ros in xrange(rosmin,5):
    for mod in xrange(min(64,TileCalibUtils.getMaxDrawer(ros))):
        newDrawer=True
        for chn in xrange(48):
            #=== loop over gains
            for adc in xrange(2):
                data = blobParser.getData(ros,mod,chn,adc)
                if not len(data) and not all:
                    if not rosmin: log.warning("%i/%2i/%2i/%i: No value found in file" % (ros,mod,chn,adc))
                    continue
                #=== need to invalidate previous blob in DB when reading from ASCII file
                if newDrawer:
                    newDrawer=False
                    blobWriter.zeroBlob(ros,mod)
                #=== init drawer for first entry
                calibDrawer = blobWriter.getDrawer(ros,mod)
                if not calibDrawer.getNObjs():
                    log.info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                    flt = blobReader.getDrawer(ros, mod,since)
                    if nval<1: 
                        mval = flt.getObjSizeUint32()
                        default.clear()
                        for n in xrange(mval):
                            default.push_back(0.)
                        defConst.clear()
                        defConst.push_back(default) # low  gain
                        defConst.push_back(default) # high gain
                    else:
                        mval = nval
                    calibDrawer.init(defConst,48,1)
                    for ch in xrange(48):
                        for ad in xrange(2):
                            nold+=1
                            for n in xrange(0,mval):
                                val=flt.getData(ch,ad,n)
                                log.debug("%i/%2i/%2i/%i: old data[%i] = %f" % (ros,mod,ch,ad, n, val))
                                calibDrawer.setData(ch,ad,n,val)
                                
                if not len(data):
                    if not rosmin: log.warning("%i/%2i/%2i/%i: No value found in file" % (ros,mod,chn,adc))
                    continue
                #=== loop over new data
                if nval<1:
                    mval = flt.getObjSizeUint32()
                else:
                    mval = nval
                nnew+=1
                for n in xrange(mval):
                    val = float(data[n])
                    log.debug("%i/%2i/%2i/%i: new data[%i] = %s" % (ros,mod,chn,adc, n, val))
                    calibDrawer.setData(chn,adc,n,val)


log.info("%d old values have been read from database" % nold)
log.info("%d new values have been read from input ascii file" % nnew)
if nold!=nnew: log.info("%d values remain unchanged" % (nold-nnew))

#=== close the database connection
if mval:
    blobWriter.setComment(os.getlogin(),("Update for run %i" % run))
    blobWriter.register(since, until, folderTag)
else:
    log.warning("Nothing to update")

dbr.closeDatabase()
dbw.closeDatabase()

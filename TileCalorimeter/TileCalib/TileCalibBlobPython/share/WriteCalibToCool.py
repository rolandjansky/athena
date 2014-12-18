#!/bin/env python
# WriteCalibToCool.py
# Sanya Solodkov 2014-08-29

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Update TileCal calibration constants in COOL"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN or /TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
    print "-t, --tag=      specify tag to use, f.i. RUN2-HLT-UPD1-00 or RUN2-UPD4-00"
    print "-r, --run=      specify run  number, default is 0"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-c, --channel   if present, means that one constant per channel is expected (i.e. no gain field)"
    print "-d, --default   if present, means that also default values stored in AUX01-AUX20 should be updated"
    print "-a, --all       if present, means that all drawers are saved, otherwise only those which were updated"
    print "-g, -gain=      specify number of gains to store to DB, default is 2"
    print "-n, --nval=     specify number of values to store to DB, default is 0 - means all"
    print "-v, --version=  specify blob version, by default version from input DB is used" 
    print "-x, --txtfile=  specify the text file with the new constants for reading"
    print "-p, --prefix=   specify prefix which is expected on every line in input file, default - no prefix"
    print "-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'"
    print "-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    print "-s, --schema=     specify input/output schema to use when both input and output schemas are the same"
    
letters = "hr:l:s:i:o:t:f:g:n:v:x:p:dca"
keywords = ["help","run=","lumi=","schema=","inschema=","outschema=","tag=","folder=","gain=","nval=","version=","txtfile=","prefix=","default","channel","all"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetOptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = -1
lumi = 0
schema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
inSchema = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2"
outSchema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
folderPath =  "/TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
tag = "RUN2-HLT-UPD1-00"
readGain=True
rosmin = 1
all=False
nchan = 48
ngain = 2
nval = 0
blobVersion = -1
txtFile= ""
prefix = ""

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-s","--schema"):
        schema = a
        inSchema = a
        outSchema = a
    elif o in ("-i","--inschema"):
        inSchema = a
    elif o in ("-o","--outschema"):
        outSchema = a
    elif o in ("-n","--nval"):
        nval = int(a)
    elif o in ("-g","--gain"):
        ngain = int(a)
    elif o in ("-v","--version"):
        blobVersion = int(a)
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

if not len(outSchema): outSchema=schema
else: schema=outSchema
if not len(inSchema): inSchema=schema

import PyCintex
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("WriteCalibToCool")
import logging
log.setLevel(logging.DEBUG)

if run<0:
    run=TileCalibTools.getLastRunNumber()
    log.warning( "Run number is not specified, using current run number %d" %run )
    if run<0:
        log.error( "Bad run number" )
        sys.exit(2)
since = (run, lumi)
until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)

#=== set database
dbr = TileCalibTools.openDbConn(inSchema,'READONLY')
dbw = TileCalibTools.openDbConn(outSchema,('UPDATE' if inSchema==outSchema else 'RECREATE'))
folderTag = TileCalibTools.getFolderTag(dbr, folderPath, tag )
log.info("Initializing folder %s with tag %s" % (folderPath, folderTag))

#=== initialize blob reader to read previous comments
blobReader = TileCalibTools.TileBlobReader(dbr,folderPath, folderTag)
#=== get drawer with status at given run
log.info("Initializing for run %d, lumiblock %d" % (run,lumi))
flt=None
r=5
d=0
while not flt:
    d-=1
    if d<0:
        r-=1
        if r<0: break
        d=TileCalibUtils.getMaxDrawer(r)-1
    flt = blobReader.getDrawer(r, d, since, False, False)
if flt:
    blobT=flt.getObjType()
    blobV=flt.getObjVersion()
    mchan=flt.getNChans()
    mgain=flt.getNGains()
    mval=flt.getObjSizeUint32()
    log.info( "Blob type: %d  version: %d  Nchannels: %d  Ngains: %d  Nval: %d" % (blobT,blobV,mchan,mgain,mval) )
    if blobVersion<0: blobVersion = blobV 
    if nchan<mchan:   nchan=mchan
    if ngain<mgain:   ngain=mgain
else:
    if nchan<1: nchan=TileCalibUtils.max_chan()
    if ngain<1: ngain=TileCalibUtils.max_gain()

log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

blobWriter = TileCalibTools.TileBlobWriter(dbw,folderPath,'Flt',(True if len(tag) else False))

#=== create default: one number per ADC
default = PyCintex.gbl.std.vector('float')()
for n in xrange(nval):
    default.push_back(0.)

defConst = PyCintex.gbl.std.vector('std::vector<float>')()
for ng in xrange(ngain):
    defConst.push_back(default) # low/high gain

blobParser = TileCalibTools.TileASCIIParser2(txtFile,prefix,readGain);
mval=0

nold=0
nnew=0
ndef=0
nvold=0
nvnew=0
nvdef=0
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
        flt1 = blobReader.getDrawer(ros, mod, since, False, False)
        for chn in xrange(nchan):
            #=== loop over gains
            for adc in xrange(ngain):
                data = blobParser.getData(ros,mod,chn,adc)
                if not len(data) and (not all or (not flt1 and not rosmin)):
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
                    flt = blobReader.getDrawer(ros, mod, since)
                    if nval<1: 
                        mval = flt.getObjSizeUint32()
                        default.clear()
                        for n in xrange(mval):
                            default.push_back(0.)
                        defConst.clear()
                        for ng in xrange(ngain):
                            defConst.push_back(default) # low/high  gain
                    else:
                        mval = nval
                    kval = mval if mval < flt.getObjSizeUint32() else flt.getObjSizeUint32() 
                    if blobVersion<0:
                        blobVersion = flt.getObjVersion()
                    calibDrawer.init(defConst,nchan,blobVersion)
                    for ch in xrange(nchan):
                        for ad in xrange(ngain):
                            nold+=1
                            for n in xrange(0,kval):
                                nvold+=1
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
                kval=mval-len(data)
                if kval>0:
                    ndef+=1
                    mval-=kval
                for n in xrange(mval):
                    nvnew+=1
                    val = float(data[n])
                    log.debug("%i/%2i/%2i/%i: new data[%i] = %s" % (ros,mod,chn,adc, n, val))
                    calibDrawer.setData(chn,adc,n,val)
                for n in xrange(mval,kval+mval):
                    nvdef+=1
                    val = calibDrawer.getData(chn,adc,n)
                    log.debug("%i/%2i/%2i/%i: def data[%i] = %s" % (ros,mod,chn,adc, n, val))


log.info("%d/%d old channels*gains/values have been read from database" % (nold,nvold))
log.info("%d/%d new channels*gains/values have been read from input ascii file" % (nnew,nvnew))
if nold>nnew or nvold>nvnew: log.info("%d/%d old channels*gains/values remain unchanged" % (nold-nnew,nvold-nvnew))
if nold<nnew or nvold<nvnew: log.info("%d/%d new channels*gains/values have been added to database" % (nnew-nold,nvnew-nvold))
if ndef: log.info("%d/%d new channels*gains/values with default values have been added to database" % (ndef-nold,nvdef))

#=== commit changes
if mval:
    blobWriter.setComment(os.getlogin(),("Update for run %i from file %s" % (run,txtFile)))
    blobWriter.register(since, until, folderTag)
else:
    log.warning("Nothing to update")

#=== close DB
dbr.closeDatabase()
dbw.closeDatabase()

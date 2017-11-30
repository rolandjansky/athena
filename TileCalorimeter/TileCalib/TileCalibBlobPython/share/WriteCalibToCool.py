#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# WriteCalibToCool.py
# Sanya Solodkov 2014-08-29
# change Yuri Smirnov 2014-12-24

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ",sys.argv[0]," [OPTION] ... "
    print "Update TileCal calibration constants in COOL"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN or /TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
    print "-F, --outfolder=  specify the name of output folder if different from input folder"
    print "-t, --tag=      specify tag to use, f.i. RUN2-HLT-UPD1-00 or RUN2-UPD4-00"
    print "-T, --outtag=     specify output tag if different from input tag"
    print "-r, --run=      specify run  number, default is 0"
    print "-R, --run2=     specify run  number for new IOV where correction is undone"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-L, --lumi2=    specify lumi block number for new IOV where correction is undone"
    print "-c, --channel   if present, means that one constant per channel is expected (i.e. no gain field)"
    print "-d, --default   if present, means that also default values stored in AUX01-AUX20 should be updated"
    print "-a, --all       if present, means that all drawers are saved, otherwise only those which were updated"
    print "-z, --zero      if present, means that zero-sized blob is written for missing drawers"
    print "-Z, --allzero   if present, means that zero-sized blob is created for all drawers which are not present in input file"
    print "-C, --nchannel= specify number of channels to store to DB, default is 0 - means the same as in input DB"
    print "-G, --ngain=    specify number of gains to store to DB, default is 0 - means the same as in input DB"
    print "-n, --nval=     specify number of values to store to DB, default is 0 - means all"
    print "-v, --version=  specify blob version, by default version from input DB is used" 
    print "-x, --txtfile=  specify the text file with the new constants for reading"
    print "-m, --comment=  specify comment to write"
    print "-p, --prefix=   specify prefix which is expected on every line in input file, default - no prefix"
    print "-k, --keep=     field numbers or channel numbers to ignore, e.g. '0,2,3,EBch0,EBch1,EBch12,EBch13,EBspD4ch18,EBspD4ch19,EBspC10ch4,EBspC10ch5' "
    print "-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'"
    print "-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    print "-s, --schema=     specify input/output schema to use when both input and output schemas are the same"
    print "-u  --update      set this flag if output sqlite file should be updated, otherwise it'll be recreated"
    
letters = "hr:l:R:L:s:i:o:t:T:f:F:C:G:n:v:x:m:p:dcazZuk:"
keywords = ["help","run=","lumi=","run2=","lumi2=","schema=","inschema=","outschema=","tag=","outtag=","folder=","outfolder=","nchannel=","ngain=","nval=","version=","txtfile=","comment=","prefix=","default","channel","all","zero","allzero","update","keep="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)

# defaults 
run = -1
lumi = 0
run2 = -1
lumi2 = 0
schema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
inSchema = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2"
outSchema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
folderPath =  "/TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS"
tag = "UPD1"
outfolderPath = None
outtag = None
readGain=True
rosmin = 1
all=False
zero=False
allzero=False
nchan = 0
ngain = 0
nval = 0
blobVersion = -1
txtFile= ""
comment = ""
prefix = ""
update = False
keep=[]

for o, a in opts:
    if o in ("-f","--folder"):
        folderPath = a
    elif o in ("-F","--outfolder"):
        outfolderPath = a
    elif o in ("-t","--tag"):
        tag = a
    elif o in ("-T","--outtag"):
        outtag = a
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
    elif o in ("-G","--ngain"):
        ngain = int(a)
    elif o in ("-C","--nchannel"):
        nchan = int(a)
    elif o in ("-v","--version"):
        blobVersion = int(a)
    elif o in ("-d","--default"):
        rosmin = 0
    elif o in ("-c","--channel"):
        readGain = False
    elif o in ("-a","--all"):
        all = True
    elif o in ("-z","--zero"):
        zero = True
    elif o in ("-Z","--allzero"):
        allzero = True
    elif o in ("-u","--update"):
        update = True
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-R","--run2"):
        run2 = int(a)
    elif o in ("-L","--lumi2"):
        lumi2 = int(a)
    elif o in ("-x","--txtfile"):
        txtFile = a
    elif o in ("-m","--comment"):
        comment = a
    elif o in ("-p","--prefix"):
        prefix = a
    elif o in ("-k","--keep"):
        keep = a.split(",")
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

if not len(outSchema): outSchema=schema
else: schema=outSchema
if not len(inSchema): inSchema=schema
update = update or (inSchema==outSchema)

if outfolderPath is None:
    outfolderPath=folderPath
elif tag=='UPD5' and outfolderPath!=folderPath:
    print '--tag="UPD5" option is not compatible with --outfolderPath option'
    sys.exit(2)
if outtag is None:
    outtag = tag

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("WriteCalibToCool")
import logging
log.setLevel(logging.DEBUG)

if run==0: begin=(0,0)
if run<=0:
    run=TileCalibTools.getLastRunNumber()
    log.warning( "Run number is not specified, using current run number %d" %run )
    if run<0:
        log.error( "Bad run number" )
        sys.exit(2)
since = (run, lumi)
if not "begin" in dir(): begin=since
until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)

#=== set database
dbr = TileCalibTools.openDbConn(inSchema,'READONLY')
dbw = TileCalibTools.openDbConn(outSchema,('UPDATE' if update else 'RECREATE'))
if tag=='UPD5':
    tag='UPD4'
    outtag='UPD4'
    outfolderPath=folderPath
    folderTag = TileCalibTools.getFolderTag(dbr, folderPath, tag )
    tag2=folderTag.split('-')
    tag2[len(tag2)-1]="%02d"%(int(tag2[len(tag2)-1])+1)
    outfolderTag="-".join(tag2)
else:
    folderTag = TileCalibTools.getFolderTag(dbr, folderPath, tag )
    if outfolderPath==folderPath and outtag==tag:
        outfolderTag = folderTag
    else:
        outfolderTag = TileCalibTools.getFolderTag(dbr, outfolderPath, outtag )
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
else:
    mval=-1
nchanDef=nchan
ngainDef=ngain

log.info("Comment: %s" % blobReader.getComment((run,lumi)))
log.info( "\n" )

blobWriter = TileCalibTools.TileBlobWriter(dbw,outfolderPath,'Flt',(True if len(outtag) else False))
if run2<0: run2=begin[0]
if run2>begin[0] or (run2==begin[0] and lumi2>begin[1]):
    undo=True
    blobWriter2 = TileCalibTools.TileBlobWriter(dbw,outfolderPath,'Flt',(True if len(outtag) else False))
else:
    undo=False

if len(txtFile)>0:
    #=== create default: one number per ADC
    default = cppyy.gbl.std.vector('float')()
    for n in xrange(nval):
        default.push_back(0.)

    defConst = cppyy.gbl.std.vector('std::vector<float>')()
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
            modName = TileCalibUtils.getDrawerString(ros,mod)
            if modName in ['EBA39','EBA40','EBA41','EBA42','EBA55','EBA56','EBA57','EBA58',
                           'EBC39','EBC40','EBC41','EBC42','EBC55','EBC56','EBC57','EBC58' ]:
                modSpec = 'EBspC10'
            elif modName in ['EBA15','EBC18']:
                modSpec = 'EBspD4'
            elif modName in ['EBC29','EBC32','EBC34','EBC37']:
                modSpec = 'EBspE4'
            elif modName in ['EBA07', 'EBA25', 'EBA44', 'EBA53',
                             'EBC07', 'EBC25', 'EBC44', 'EBC53',
                             'EBC28', 'EBC31', 'EBC35', 'EBC38' ]:
                modSpec = 'EBspE1'
            elif modName in ['EBA08', 'EBA24', 'EBA43', 'EBA54',
                             'EBC08', 'EBC24', 'EBC43', 'EBC54' ]:
                modSpec = 'EBMBTS'
            else:
                modSpec = modName
            newDrawer=True
            flt1 = blobReader.getDrawer(ros, mod, since, False, False)
            nchan = nchanDef if nchanDef>0 else (flt1.getNChans() if flt1 else TileCalibUtils.max_chan())
            ngain = ngainDef if ngainDef>0 else (flt1.getNGains() if flt1 else TileCalibUtils.max_gain())
            for chn in xrange(nchan):
                #=== loop over gains
                for adc in xrange(ngain):
                    data = blobParser.getData(ros,mod,chn,adc)
                    if not len(data) and allzero:
                        continue
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
                        if undo:
                            calibDrawer2 = blobWriter2.getDrawer(ros,mod)
                            calibDrawer2.init(defConst,nchan,blobVersion)
                        for ch in xrange(nchan):
                            for ad in xrange(ngain):
                                nold+=1
                                for n in xrange(0,kval):
                                    nvold+=1
                                    val=flt.getData(ch,ad,n)
                                    log.debug("%i/%2i/%2i/%i: old data[%i] = %f" % (ros,mod,ch,ad, n, val))
                                    calibDrawer.setData(ch,ad,n,val)
                                    if undo:
                                        calibDrawer2.setData(ch,ad,n,val)
                                    
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
                        coef=None
                        strval=data[n]
                        if strval.startswith("*"):
                            coef=float(strval[1:])
                            val = calibDrawer.getData(chn,adc,n)*coef
                            log.debug("%i/%2i/%2i/%i: new data[%i] = %s  scale old value by %s" % (ros,mod,chn,adc, n, val, coef))
                        elif strval.startswith("++") or strval.startswith("+-") :
                            coef=float(strval[1:])
                            val = calibDrawer.getData(chn,adc,n)+coef
                            log.debug("%i/%2i/%2i/%i: new data[%i] = %s  shift old value by %s" % (ros,mod,chn,adc, n, val, coef))
                        elif strval=="sync":
                            val = calibDrawer.getData(chn,adc,n)
                            if val==0.0 or val==-1.0 or val==1.0: # copy from another gain only if in this gain one of default values
                                val = calibDrawer.getData(chn,1-adc,n)
                        elif strval=="copy":
                            val = calibDrawer.getData(chn,1-adc,n) # copy from another gain
                        elif strval=="lg" and adc==1:
                            val = calibDrawer.getData(chn,0,n) # copy from low gain
                        elif strval=="hg" and adc==0:
                            val = calibDrawer.getData(chn,1,n) # copy from high gain
                        elif strval=="keep" or strval=="None" or str(n) in keep or modName in keep or  modSpec in keep or modName[:3] in keep or  modName[:2] in keep \
                             or ("%sch%i"% (modName,chn)) in keep or ("%sch%i"% (modSpec,chn)) in keep or ("%sch%i"% (modName[:3],chn)) in keep or ("%sch%i"% (modName[:2],chn)) in keep \
                             or ("%sch%ig%i"% (modName,chn,adc)) in keep or ("%sch%ig%i"% (modSpec,chn,adc)) in keep or ("%sch%ig%i"% (modName[:3],chn,adc)) in keep or ("%sch%ig%i"% (modName[:2],chn,adc)) in keep:
                            val = None
                        else:
                            val = float(strval)
                        if val is not None:
                            nvnew+=1
                            calibDrawer.setData(chn,adc,n,val)
                            if coef is None:
                                log.debug("%i/%2i/%2i/%i: new data[%i] = %s" % (ros,mod,chn,adc, n, val))
                    for n in xrange(mval,kval+mval):
                        nvdef+=1
                        val = calibDrawer.getData(chn,adc,n)
                        log.debug("%i/%2i/%2i/%i: def data[%i] = %s" % (ros,mod,chn,adc, n, val))
            if (zero or allzero) and newDrawer:
                blobWriter.zeroBlob(ros,mod)
                if ros==0 and mod==0:
                    if blobVersion<0:
                        blobVersion = flt.getObjVersion()
                    calibDrawer = blobWriter.getDrawer(ros,mod)
                    calibDrawer.init(defConst,1,blobVersion)
    
    log.info("%d/%d old channels*gains/values have been read from database" % (nold,nvold))
    log.info("%d/%d new channels*gains/values have been read from input ascii file" % (nnew,nvnew))
    if nold>nnew or nvold>nvnew: log.info("%d/%d old channels*gains/values remain unchanged" % (nold-nnew,nvold-nvnew))
    if nold<nnew or nvold<nvnew: log.info("%d/%d new channels*gains/values have been added to database" % (nnew-nold,nvnew-nvold))
    if ndef: log.info("%d/%d new channels*gains/values with default values have been added to database" % (ndef-nold,nvdef))
    
#=== commit changes
if mval!=0 and (len(comment)>0 or len(txtFile)>0):
    if (comment is None) or (comment == "None"):
        blobWriter.setComment("None","None")
    else:
        if len(comment)==0:
            if begin[1]==0:
                comment="Update for run %i from file %s" % (begin[0],txtFile)
            else:
                comment="Update for run,lumi %i,%i from file %s" % (begin[0],begin[1],txtFile)
        blobWriter.setComment(os.getlogin(),comment)
    blobWriter.register(begin, until, outfolderTag)
    if undo:
        if (comment is None) or (comment == "None"):
            blobWriter2.setComment("None","None")
        else:
            if run2>begin[0] and lumi2==0 and begin[1]==0:
                comment="Update for run %i - undoing changes done for run %i from file %s" % (run2,begin[0],txtFile)
            else:
                comment="Update for run,lumi %i,%i - undoing changes done for %i,%i from file %s" % (run2,lumi2,begin[0],begin[1],txtFile)
            blobWriter2.setComment(os.getlogin(),comment)
        blobWriter2.register((run2,lumi2), until, outfolderTag)
    elif run2>=0 and (run2<begin[0] or (run2==begin[0] and lumi2<begin[1]) and lumi2!=0):
        log.warning("(run2,lumi2)=(%i,%i) is smaller than (run,lumi)=(%i,%i) - will not create second IOV" % (run2,lumi2,begin[0],begin[1]))
else:
    log.warning("Nothing to update")
    
#=== close DB
dbr.closeDatabase()
dbw.closeDatabase()

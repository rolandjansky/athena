#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# WriteCalibToCool.py
# Sanya Solodkov 2014-08-29
# change Yuri Smirnov 2014-12-24

from __future__ import print_function

import getopt,sys,os,bisect
os.environ['TERM'] = 'linux'

def usage():
    print ("Usage: ",sys.argv[0]," [OPTION] ... ")
    print ("Update TileCal calibration constants in COOL")
    print ("")
    print ("-h, --help      shows this help")
    print ("-f, --folder=   specify folder to use f.i. /TILE/OFL02/CALIB/CIS/LIN or /TILE/OFL02/TIME/CHANNELOFFSET/GAP/LAS")
    print ("-F, --outfolder=  specify the name of output folder if different from input folder")
    print ("-t, --tag=      specify tag to use, f.i. RUN2-HLT-UPD1-00 or RUN2-UPD4-00")
    print ("-T, --outtag=     specify output tag if different from input tag")
    print ("-r, --run=      specify run  number, default is 0")
    print ("-R, --run2=     specify run  number for new IOV where correction is undone")
    print ("-l, --lumi=     specify lumi block number, default is 0")
    print ("-L, --lumi2=    specify lumi block number for new IOV where correction is undone")
    print ("-b, --begin=    specify run number of first iov in multi-iov mode, by default uses very first iov")
    print ("-e, --end=      specify run number of last iov in multi-iov mode, by default uses latest iov")
    print ("-A, --adjust    in multi-iov mode adjust iov boundaries to nearest iov available in DB, default is False")
    print ("-D, --module=   specify module to use in multi-IOV update, default is all")
    print ("-c, --channel   if present, means that one constant per channel is expected (i.e. no gain field)")
    print ("-d, --default   if present, means that also default values stored in AUX01-AUX20 should be updated")
    print ("-a, --all       if present, means that all drawers are saved, otherwise only those which were updated")
    print ("-z, --zero      if present, means that zero-sized blob is written for missing drawers")
    print ("-Z, --allzero   if present, means that zero-sized blob is created for all drawers which are not present in input file")
    print ("-C, --nchannel= specify number of channels to store to DB, default is 0 - means the same as in input DB")
    print ("-G, --ngain=    specify number of gains to store to DB, default is 0 - means the same as in input DB")
    print ("-n, --nval=     specify number of values to store to DB, default is 0 - means all")
    print ("-v, --version=  specify blob version, by default version from input DB is used" )
    print ("-x, --txtfile=  specify the text file with the new constants for reading")
    print ("-m, --comment=  specify comment which should be written to DB, in multi-iov mode it is appended to old comment")
    print ("-M, --Comment=  specify comment which should be written to DB, in mutli-iov mode it overwrites old comment")
    print ("-U, --user=     specify username for comment")
    print ("-p, --prefix=   specify prefix which is expected on every line in input file, default - no prefix")
    print ("-k, --keep=     field numbers or channel numbers to ignore, e.g. '0,2,3,EBch0,EBch1,EBch12,EBch13,EBspD4ch18,EBspD4ch19,EBspC10ch4,EBspC10ch5' ")
    print ("-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'")
    print ("-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'")
    print ("-s, --schema=     specify input/output schema to use when both input and output schemas are the same")
    print ("-u  --update      set this flag if output sqlite file should be updated, otherwise it'll be recreated")

letters = "hr:l:R:L:b:e:AD:s:i:o:t:T:f:F:C:G:n:v:x:m:M:U:p:dcazZuk:"
keywords = ["help","run=","lumi=","run2=","lumi2=","begin=","end=","adjust","module=","schema=","inschema=","outschema=","tag=","outtag=","folder=","outfolder=","nchannel=","ngain=","nval=","version=","txtfile=","comment=","Comment=","user=","prefix=","default","channel","all","zero","allzero","update","keep="]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError as err:
    print (str(err))
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
Comment = None
prefix = ""
update = False
keep=[]
iov = False
beg = 0
end = 2147483647
moduleList = []
adjust = False

try:
    user=os.getlogin()
except Exception:
    user="UnknownUser"

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
    elif o in ("-b","--begin"):
        beg = int(a)
        iov = True
    elif o in ("-e","--end"):
        end = int(a)
        iov = True
    elif o in ("-A","--adjust"):
        adjust = True
    elif o in ("-D","--module"):
        moduleList += a.split(",")
    elif o in ("-x","--txtfile"):
        txtFile = a
    elif o in ("-m","--comment"):
        comment = a
    elif o in ("-M","--Comment"):
        Comment = a
        comment = a
    elif o in ("-U","--user"):
        user = a
    elif o in ("-p","--prefix"):
        prefix = a
    elif o in ("-k","--keep"):
        keep = a.split(",")
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

if not len(outSchema):
    outSchema=schema
else:
    schema=outSchema
if not len(inSchema):
    inSchema=schema
update = update or (inSchema==outSchema)

if outfolderPath is None:
    outfolderPath=folderPath
elif tag=='UPD5' and outfolderPath!=folderPath:
    print ('--tag="UPD5" option is not compatible with --outfolderPath option')
    sys.exit(2)
if outtag is None:
    outtag = tag

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import TileCalibUtils

if iov and end >= TileCalibTools.MAXRUN:
    end = TileCalibTools.MAXRUN
    lumi2 = TileCalibTools.MAXLBK
until = (TileCalibTools.MAXRUN,TileCalibTools.MAXLBK)

from TileCalibBlobPython.TileCalibLogger import getLogger
log = getLogger("WriteCalibToCool")
import logging
if iov:
    log.setLevel(logging.INFO)
else:
    log.setLevel(logging.DEBUG)

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
log.info("Initializing folder %s with tag %s", folderPath, folderTag)

iovAll = []
iovList = []
iovUntil = []
iovListMOD = []
iovListCMT = []
iovUntilCMT = []
blobReader = TileCalibTools.TileBlobReader(dbr,folderPath, folderTag)
if iov:
    #=== filling the iovList
    log.info( "Looking for IOVs" )
    if moduleList!=['CMT']:
        for ros in range(rosmin,5):
            for mod in range(min(64,TileCalibUtils.getMaxDrawer(ros))):
                modName = TileCalibUtils.getDrawerString(ros,mod)
                if len(moduleList)>0 and modName not in moduleList and 'ALL' not in moduleList:
                    iovAll+=[[]]
                else:
                    iovMod = blobReader.getIOVsWithinRange(ros,mod)
                    iovAll+=[iovMod]
                    iovList+=iovMod
        if 'ALL' in moduleList:
            moduleList.remove('ALL')
    else:
        for ros in range(rosmin,5):
            iovAll+=[[]]*min(64,TileCalibUtils.getMaxDrawer(ros))
    if 'CMT' in moduleList:
        iovListMOD = iovList
        iovListCMT = blobReader.getIOVsWithinRange(-1,1000)
        if len(iovList)==0:
            iovList = iovListCMT
            iovAll+=[iovListCMT]
        else:
            moduleList.remove('CMT')

    import functools
    def compare(item1,item2):
        if item1[0]!=item2[0]:
            return item1[0]-item2[0]
        else:
            return item1[1]-item2[1]
    iovList=list(set(iovList))
    iovList=sorted(iovList,key=functools.cmp_to_key(compare))
    iovList+=[until]
    iovListCMT+=[until]

    since=(beg,lumi)
    ib=bisect.bisect(iovList,since)-1
    if ib<0:
        ib=0
    if iovList[ib] != since:
        if adjust:
            since = iovList[ib]
            log.info( "Moving beginning of first IOV with new constants from (%d,%d) to (%d,%d)", beg,lumi,since[0],since[1])
        else:
            iovList[ib] = since
            log.info( "Creating new IOV starting from (%d,%d) with new constants", beg,lumi)

    if end<0:
        ie=ib+1
        if ie>=len(iovList):
            ie=ib
        until=iovList[ie]
        log.info( "Next IOV without new constants starts from (%d,%d)", until[0],until[1])
    else:
        until=(end,lumi2)
        ie=bisect.bisect_left(iovList,until)
        if ie>=len(iovList):
            ie=len(iovList)-1

        if iovList[ie] != until:
            if adjust:
                until=iovList[ie]
                log.info( "Moving end of last IOV from (%d,%d) to (%d,%d)", end,lumi2,until[0],until[1])
            else:
                log.info( "Keeping end of last IOV at (%d,%d) - new IOV is shorter than IOV in input DB", end,lumi2)
                iovList[ie] = until


    iovList = iovList[ib:ie]
    iovUntil = iovList[1:] + [until]
    begin = since
    run = since[0]
    lumi = since[1]
    undo = False
    log.info( "IOVs: %s", str(iovList))

    if len(iovListMOD)>0 and len(iovListCMT)>0:
        if Comment is None:
            iovList += [until]  # one more IOV for "UNDO" comment
            iovUntil += [until] # one more IOV for "UNDO" comment
        iovUntilCMT = []
        for io,since in enumerate(iovList):
            p = bisect.bisect(iovListCMT,since)
            if p<len(iovListCMT):
                iovUntilCMT += [iovListCMT[p]]
                if iovUntil[io] != iovListCMT[p]:
                    log.info( "End of iov %s in comments record is %s", str(since),str(iovListCMT[p]))
            else:
                if since!=until:
                    iovUntilCMT += [since]
                else:
                    iovList.pop(-1)
                    break
        since = begin
    else:
        iovUntilCMT = iovUntil

    log.info( "%d IOVs in total, end of last IOV is %s", ie-ib,str(until))

else:
    #=== set run number
    if run==0:
        begin=(0,0)
    if run<=0:
        if "UPD4" in outtag:
            run=TileCalibTools.getPromptCalibRunNumber()
            log.warning( "Run number is not specified, using minimal run number in calibration loop %d", run )
        else:
            run=TileCalibTools.getLastRunNumber()
            log.warning( "Run number is not specified, using current run number %d", run )
        if run<0:
            log.error( "Bad run number" )
            sys.exit(2)

    since = (run, lumi)
    iovList = [since]
    iovUntil = [until]
    iovUntilCMT = [until]
    if "begin" not in dir():
        begin=since
    if run2<0:
        run2=begin[0]
    if run2>begin[0] or (run2==begin[0] and lumi2>begin[1]):
        undo=True
        blobWriter2 = TileCalibTools.TileBlobWriter(dbw,outfolderPath,'Flt',(True if len(outtag) else False))
    else:
        undo=False

    log.info("Initializing for run %d, lumiblock %d", run,lumi)

flt=None
r=5
d=0
while not flt:
    d-=1
    if d<0:
        r-=1
        if r<0:
            break
        d=TileCalibUtils.getMaxDrawer(r)-1
    flt = blobReader.getDrawer(r, d, since, False, False)
if flt:
    blobT=flt.getObjType()
    blobV=flt.getObjVersion()
    mchan=flt.getNChans()
    mgain=flt.getNGains()
    mval=flt.getObjSizeUint32()
    log.info( "Blob type: %d  version: %d  Nchannels: %d  Ngains: %d  Nval: %d", blobT,blobV,mchan,mgain,mval)
    if blobVersion<0:
        blobVersion = blobV
else:
    mchan=-1
    mgain=-1
    mval=-1
nchanDef=nchan
ngainDef=ngain

comments = []
blobWriters = []
nvalUpdated = []
commentsSplit = []
for since in iovList:
    comm=blobReader.getComment(since)
    log.info("Comment: %s", comm)
    comments+=[comm]
    nvalUpdated += [0]
    commentsSplit+=[blobReader.getComment(since,True)]
    blobWriters += [TileCalibTools.TileBlobWriter(dbw,outfolderPath,'Flt',(True if len(outtag) else False))]
log.info( "\n" )

if len(txtFile)>0:
    #=== create default: one number per ADC
    default = cppyy.gbl.std.vector('float')()
    for n in range(nval if nval>0 else mval):
        default.push_back(0.)

    defConst = cppyy.gbl.std.vector('std::vector<float>')()
    for ng in range(ngain if ngain>0 else mgain):
        defConst.push_back(default) # low/high gain

    blobParser = TileCalibTools.TileASCIIParser2(txtFile,prefix,readGain)
    mval=0

    #=== loop over all IOVs
    for io,since in enumerate(iovList):

        if since==iovUntil[io]: # dummy IOV without update
            continue

        log.info( "Updating IOV %s", str(since) )
        nold=0
        nnew=0
        ndef=0
        nvold=0
        nvnew=0
        nvdef=0
        nvnewdef=0

        #=== loop over whole detector
        irm=-1
        for ros in range(rosmin,5):
            for mod in range(min(64,TileCalibUtils.getMaxDrawer(ros))):
                irm+=1
                if iov and since!=begin and (since not in iovAll[irm]):
                    continue
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
                if flt1:
                    oldNchan = flt1.getNChans()
                    oldNgain = flt1.getNGains()
                    oldVsize = flt1.getObjSizeUint32()
                else:
                    oldNchan = 0
                    oldNgain = 0
                    oldVsize = 0
                nchan = nchanDef if nchanDef>0 else (flt1.getNChans() if flt1 else TileCalibUtils.max_chan())
                ngain = ngainDef if ngainDef>0 else (flt1.getNGains() if flt1 else TileCalibUtils.max_gain())
                for chn in range(nchan):
                    #=== loop over gains
                    for adc in range(ngain):
                        data = blobParser.getData(ros,mod,chn,adc,since)
                        if not len(data) and allzero:
                            continue
                        if not len(data) and (not all or (not flt1 and not rosmin)):
                            if not rosmin:
                                log.warning("%i/%2i/%2i/%i: No value found in file", ros,mod,chn,adc)
                            continue
                        #=== need to invalidate previous blob in DB when reading from ASCII file
                        if newDrawer:
                            newDrawer=False
                            blobWriters[io].zeroBlob(ros,mod)
                        #=== init drawer for first entry
                        calibDrawer = blobWriters[io].getDrawer(ros,mod)
                        if not calibDrawer.getNObjs():
                            log.info("Initializing drawer %s", modName)
                            flt = blobReader.getDrawer(ros, mod, since)
                            if nval<1:
                                mval = flt.getObjSizeUint32()
                                default.clear()
                                for n in range(mval):
                                    default.push_back(0.)
                                defConst.clear()
                                for ng in range(ngain):
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
                            for ch in range(nchan):
                                for ad in range(ngain):
                                    nold+=1
                                    for n in range(0,kval):
                                        nvold+=1
                                        val=flt.getData(ch,ad,n)
                                        log.debug("%i/%2i/%2i/%i: old data[%i] = %f", ros,mod,ch,ad, n, val)
                                        calibDrawer.setData(ch,ad,n,val)
                                        if undo:
                                            calibDrawer2.setData(ch,ad,n,val)
                                    for n in range(kval,nval):
                                        nvdef+=1
                                        val=calibDrawer.getData(ch,ad,n)
                                        log.debug("%i/%2i/%2i/%i: def data[%i] = %f", ros,mod,ch,ad, n, val)

                        if not len(data):
                            if not rosmin:
                                log.warning("%i/%2i/%2i/%i: No value found in file", ros,mod,chn,adc)
                            continue
                        #=== loop over new data
                        if nval<1:
                            mval = flt.getObjSizeUint32()
                        else:
                            mval = nval
                        nnew+=1
                        kval=mval-len(data)
                        if kval>0:
                            if chn>=oldNchan or adc>=oldNgain:
                                ndef+=1
                            mval-=kval
                        for n in range(mval):
                            coef=None
                            strval=data[n]
                            if strval.startswith("*"):
                                coef=float(strval[1:])
                                val = calibDrawer.getData(chn,adc,n)*coef
                                log.debug("%i/%2i/%2i/%i: new data[%i] = %s  scale old value by %s", ros,mod,chn,adc, n, val, coef)
                            elif strval.startswith("++") or strval.startswith("+-") :
                                coef=float(strval[1:])
                                val = calibDrawer.getData(chn,adc,n)+coef
                                log.debug("%i/%2i/%2i/%i: new data[%i] = %s  shift old value by %s", ros,mod,chn,adc, n, val, coef)
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
                                if n>=oldVsize:
                                    nvdef-=1
                                    nvnewdef+=1
                                calibDrawer.setData(chn,adc,n,val)
                                if coef is None:
                                    log.debug("%i/%2i/%2i/%i: new data[%i] = %s", ros,mod,chn,adc, n, val)
                            else:
                                val = calibDrawer.getData(chn,adc,n)
                                if n>=oldVsize:
                                    log.debug("%i/%2i/%2i/%i: DEF data[%i] = %s", ros,mod,chn,adc, n, val)
                                else:
                                    log.debug("%i/%2i/%2i/%i: OLD data[%i] = %s", ros,mod,chn,adc, n, val)
                        for n in range(mval,kval+mval):
                            val = calibDrawer.getData(chn,adc,n)
                            if n>=flt.getObjSizeUint32():
                                log.debug("%i/%2i/%2i/%i: DEF data[%i] = %s", ros,mod,chn,adc, n, val)
                            else:
                                log.debug("%i/%2i/%2i/%i: OLD data[%i] = %s", ros,mod,chn,adc, n, val)
                if (zero or allzero) and newDrawer:
                    blobWriters[io].zeroBlob(ros,mod)
                    if ros==0 and mod==0:
                        if blobVersion<0:
                            blobVersion = flt.getObjVersion()
                        calibDrawer = blobWriters[io].getDrawer(ros,mod)
                        calibDrawer.init(defConst,1,blobVersion)

        nvalUpdated[io]=nvnew
        log.info("%d/%d old channels*gains/values have been read from database", nold,nvold)
        log.info("%d/%d new channels*gains/values have been read from input ascii file", nnew,nvnew)
        if nold>nnew or nvold>(nvnew-nvnewdef):
            log.info("%d/%d old channels*gains/values remain unchanged", nold-nnew,nvold-nvnew+nvnewdef)
        if nold<nnew or nvold<(nvnew-nvnewdef):
            log.info("%d/%d new channels*gains/values have been added to database", nnew-nold,nvnew-nvold-nvnewdef)
        if ndef or nvdef:
            log.info("%d/%d new channels*gains/values with default values have been added to database", ndef,nvdef)

#=== commit changes
if (mval!=0 or Comment is not None) and (len(comment)>0 or len(txtFile)>0):
    if not iov:
        iovList = [begin]

    #=== loop over all IOVs
    for io,since in enumerate(iovList):

        untilMod = iovUntil[io]
        untilCmt = iovUntilCMT[io]
        appendCmt = (untilCmt < (TileCalibTools.MAXRUN,TileCalibTools.MAXLBK))

        if since==untilMod: # empty IOV
            if since==untilCmt:
                continue # nothing to do
            undoCmt = True # only comments with UNDO text will be written
        else:
            undoCmt = False
            if len(moduleList)!=1:
                untilMod = until # more than one module updated
                untilCmt = until # will use IOV till very end
            elif untilCmt>untilMod:
                untilCmt = untilMod

        log.info( "Updating IOV %s", str(since) )

        #=== set comment
        if Comment is not None:
            comm = Comment
            if undoCmt:
                comm = "UNDO " + comm
            author = user
        else:
            if (comment is None) or (comment == "None"):
                comm = "None"
                author = "None"
            else:
                if len(comment)==0:
                    if undoCmt:
                        if since[1]==0 and begin[1]==0:
                            comm="Update for run %i - undoing changes done for run %i from file %s" % (since[0],begin[0],txtFile)
                        else:
                            comm="Update for run,lumi %i,%i - undoing changes done for %i,%i from file %s" % (since[0],since[1],begin[0],begin[1],txtFile)
                    else:
                        if since[1]==0:
                            comm="Update for run %i from file %s" % (since[0],txtFile)
                        else:
                            comm="Update for run,lumi %i,%i from file %s" % (since[0],since[1],txtFile)
                else:
                    comm = comment
                    if undoCmt:
                        comm = "UNDO " + comm
                if iov and appendCmt:
                    comm += "  //  " + comments[io]
                if not undoCmt and iov and (nvalUpdated[io]==0 or comment=="keep"):
                    author = commentsSplit[io]
                else:
                    author = user
        blobWriters[io].setComment(author,comm)

        #=== commit changes
        if untilCmt!=untilMod:
            cmtOnly = (since in iovListCMT and since not in iovListMOD)
            if not cmtOnly and untilMod>since:
                blobWriters[io].register(since,untilMod,outfolderTag,1)
            if untilCmt>since:
                blobWriters[io].register(since,untilCmt,outfolderTag,-1)
        else:
            blobWriters[io].register(since,untilMod,outfolderTag)

    if undo:
        if (comment is None) or (comment == "None"):
            blobWriter2.setComment("None","None")
        else:
            if run2>begin[0] and lumi2==0 and begin[1]==0:
                comment="Update for run %i - undoing changes done for run %i from file %s" % (run2,begin[0],txtFile)
            else:
                comment="Update for run,lumi %i,%i - undoing changes done for %i,%i from file %s" % (run2,lumi2,begin[0],begin[1],txtFile)
            blobWriter2.setComment(user,comment)
        blobWriter2.register((run2,lumi2), until, outfolderTag)
    elif run2>=0 and (run2<begin[0] or (run2==begin[0] and lumi2<begin[1]) and lumi2!=0):
        log.warning("(run2,lumi2)=(%i,%i) is smaller than (run,lumi)=(%i,%i) - will not create second IOV", run2,lumi2,begin[0],begin[1])
else:
    log.warning("Nothing to update")

#=== close DB
dbr.closeDatabase()
dbw.closeDatabase()

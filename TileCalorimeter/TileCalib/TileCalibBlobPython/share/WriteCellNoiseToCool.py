#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File:    WriteCellNoiseToCool.py
# Purpose: Manual update of cell noise constants from ascii file
#
# 2014-07-14 - Sasha, based on update_noise_bulk.py from Carlos,Gui,Blake,Yuri
# 2016-12-14 - Yuri Smirnov, change for PyCintex->cppyy for ROOT6
# 2020-06-06 - Sasha - introducing --end and --endlumi parameters

from __future__ import print_function

import getopt,sys,os,re,bisect
os.environ['TERM'] = 'linux'

def usage():
    print ("Usage: ",sys.argv[0]," [OPTION] ... ")
    print ("Updates Cell Noise database using new values from ASCII file")
    print ("")
    print ("-h, --help      shows this help")
    print ("-i, --infile=   specify the input sqlite file or full schema string")
    print ("-o, --outfile=  specify the output sqlite file")
    print ("-t, --intag=    specify the input tag")
    print ("-T, --outtag=   specify the output tag")
    print ("-f, --folder=   specify status folder to use e.g. /TILE/OFL02/NOISE/CELL ")
    print ("-d, --dbname=   specify the database name e.g. OFLP200")
    print ("-x, --txtfile=  specify the text file with the new noise constants")
    print ("-r, --run=      specify run number for start of IOV")
    print ("-l, --lumi=     specify lumiblock number for start of IOV, default is 0")
    print ("-b, --begin=    specify run number of first iov in multi-iov mode, by default uses very first iov")
    print ("-e, --end=      specify run number of last iov in multi-iov mode, by default uses latest iov")
    print ("-L, --endlumi=  specify lumi block number for last iov in multi-iov mode, default is 0")
    print ("-A, --adjust    in multi-iov mode adjust iov boundaries to nearest iov available in DB, default is False")
    print ("-n, --channel=  specify cool channel to use (48 by defalt)")
    print ("-s, --scale=    specify scale factor for all the fields except ratio field")
    print ("-u  --update    set this flag if output sqlite file should be updated, otherwise it'll be recreated")
    print ("--scaleElec=    specify separate scale factor for all electronic noise fields except ratio field")
    print ("if run number and lumiblock number are omitted - all IOVs from input file are updated")

letters = "hi:o:t:T:f:d:x:r:l:b:e:L:A:n:s:u"
keywords = ["help","infile=","outfile=","intag=","outtag=","folder=","dbname=","txtfile=","run=","lumi=","begin=","end=","endlumi=","adjust",
            "channel=","scale=","scaleA=","scaleB=","scaleD=","scaleE=","scaleD4=","scaleC10=","scaleD4sp=","scaleC10sp=","scaleElec=","update"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:],letters,keywords)
except getopt.GetoptError as err:
    print (str(err))
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
beg         = -1
end         = 2147483647
endlumi     = 0
iov         = True
adjust      = False
update      = False
chan        = 48 # represents Tile
scale       = 0.0 # means do not scale
scaleA      = 0.0 # scale for pileup term in A cells
scaleB      = 0.0 # scale for pileup term in B cells
scaleD      = 0.0 # scale for pileup term in D cells
scaleE      = 0.0 # scale for pileup term in E cells
scaleD4     = 0.0 # scale for pileup term in D4
scaleC10    = 0.0 # scale for pileup term in C10
scaleD4sp   = 0.0 # scale for pileup term in D4 special
scaleC10sp  = 0.0 # scale for pileup term in C10 special
scaleElec   = 0.0 # scale for electronic noise

for o, a in opts:
    if o in ("-i","--infile"):
        inFile = a
    elif o in ("-o","--outfile"):
        outFile = a
    elif o in ("-t","--intag"):
        inTag = a
    elif o in ("-T","--outtag"):
        outTag = a
    elif o in ("-f","--folder"):
        folderPath = a
    elif o in ("-d","--dbname"):
        dbName = a
    elif o in ("-r","--run"):
        run = int(a)
        if run>=0:
            iov = False
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-b","--begin"):
        beg = int(a)
        iov = True
    elif o in ("-e","--end"):
        end = int(a)
    elif o in ("-L","--endlumi"):
        endlumi = int(a)
    elif o in ("-A","--adjust"):
        adjust = True
    elif o in ("-u","--update"):
        update = True
    elif o in ("-n","--channel"):
        chan = int(a)
    elif o in ("-s","--scale"):
        scale = float(a)
    elif o in ("-s","--scaleA"):
        scaleA = float(a)
    elif o in ("-s","--scaleB"):
        scaleB = float(a)
    elif o in ("-s","--scaleD"):
        scaleD = float(a)
    elif o in ("-s","--scaleE"):
        scaleE = float(a)
    elif o in ("-s","--scaleD4"):
        scaleD4 = float(a)
    elif o in ("-s","--scaleC10"):
        scaleC10 = float(a)
    elif o in ("-s","--scaleD4sp"):
        scaleD4sp = float(a)
    elif o in ("-s","--scaleC10sp"):
        scaleC10sp = float(a)
    elif o in ("-s","--scaleElec"):
        scaleElec = float(a)
    elif o in ("-x","--txtfile"):
        txtFile = a
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        print (o, a)
        usage()
        sys.exit(2)

tile=(chan==48)

rescale=(scale>0.0)
if scaleElec   == 0.0:
    scaleElec   = scale
else:
    rescale=True
if scaleA      == 0.0:
    scaleA      = scale
else:
    rescale=True
if scaleB      == 0.0:
    scaleB      = scale
else:
    rescale=True
if scaleD      == 0.0:
    scaleD      = scale
else:
    rescale=True
if scaleE      == 0.0:
    scaleE      = scale
else:
    rescale=True
if scaleD4     == 0.0:
    scaleD4     = scaleD
else:
    rescale=True
if scaleC10    == 0.0:
    scaleC10    = scaleB
else:
    rescale=True
if scaleD4sp   == 0.0:
    scaleD4sp   = scaleD4
else:
    rescale=True
if scaleC10sp  == 0.0:
    scaleC10sp  = scaleC10
else:
    rescale=True

#=== check presence of all parameters
print ("")
if len(inFile)<1:
    raise Exception("Please, provide infile (e.g. --infile=tileSqlite.db or --infile=COOLOFL_TILE/OFLP200)")
if len(outFile)<1:
    raise Exception("Please, provide outfile (e.g. --outfile=tileSqlite_New.db)")
if len(inTag)<1:
    raise Exception("Please, provide intag (e.g. --intag=TileOfl02NoiseCell-IOVDEP-01)")
if len(outTag)<1:
    raise Exception("Please, provide outtag (e.g. --outtag=TileOfl02NoiseCell-IOVDEP-01)")
if len(folderPath)<1:
    raise Exception("Please, provide folder (e.g. --folder=/TILE/OFL02/NOISE/CELL)")
if len(dbName)<1:
    raise Exception("Please, provide dbname (e.g. --dbname=OFLP200 or --dbname=CONDBR2)")

import cppyy

from CaloCondBlobAlgs import CaloCondTools, CaloCondLogger
from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileCellTools

# get a logger
log = CaloCondLogger.getLogger("WriteCellNoise")
import logging
if iov:
    log.setLevel(logging.INFO)
else:
    log.setLevel(logging.DEBUG)


if inTag=="HEAD":
    inTag=""
if outTag=="HEAD":
    outTag=""

if os.path.isfile(inFile):
    ischema = 'sqlite://;schema='+inFile+';dbname='+dbName
else:
    log.info("File %s was not found, assuming it's full schema string" , inFile)
    ischema = inFile
    # possible strings for inFile:
    # "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_CALO;dbname=OFLP200"
    # "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=OFLP200"
    # COOLOFL_TILE/OFLP200 COOLOFL_TILE/COMP200 COOLOFL_TILE/CONDBR2

rb = max(run,beg)
if run<0:
    cabling = 'RUN2a'
elif run<222222 or 'COMP200' in ischema:
    cabling = 'RUN1'
else:
    if ('OFLP200' in ischema and rb>=310000) or rb>=343000:
        cabling = 'RUN2a'
    else:
        cabling = 'RUN2'
hashMgr=None
hashMgrDef=TileCellTools.TileCellHashMgr(cabling=cabling)
hashMgrA=TileCellTools.TileCellHashMgr("UpgradeA")
hashMgrBC=TileCellTools.TileCellHashMgr("UpgradeBC")
hashMgrABC=TileCellTools.TileCellHashMgr("UpgradeABC")

iovList = []
iovUntil = []
until = (TileCalibTools.MAXRUN,TileCalibTools.MAXLBK)
if end >= TileCalibTools.MAXRUN:
    end = TileCalibTools.MAXRUN
    endlumi = TileCalibTools.MAXLBK

if iov:
    #=== Get list of IOVs by tricking TileCalibTools to read a Calo blob
    idb = TileCalibTools.openDbConn(ischema,'READONLY')
    try:
        blobReader = TileCalibTools.TileBlobReader(idb,folderPath, inTag)
        iovList = blobReader.getIOVsWithinRange(-1,chan)
    except Exception:
        log.warning("Can not read IOVs from input DB file")
    idb.closeDatabase()

    iovList += [until]
    if beg<0:
        since = iovList[0]
    else:
        since = (beg, lumi)
    ib=bisect.bisect(iovList,since)-1
    if ib<0:
        ib=0
    if iovList[ib] != since:
        if adjust:
            since = iovList[ib]
            log.info( "Moving beginning of first IOV with new cell noise from (%d,%d) to (%d,%d)" , beg,lumi,since[0],since[1])
        else:
            iovList[ib] = since
            log.info( "Creating new IOV starting from (%d,%d) with new cell noise" , beg,lumi)

    if end<0:
        ie=ib+1
        if ie>=len(iovList):
            ie=ib
        until=iovList[ie]
        log.info( "Next IOV with old cell noise starts from (%d,%d)" , until[0],until[1])
    else:
        until=(end,endlumi)
        ie=bisect.bisect_left(iovList,until)
        if ie>=len(iovList):
            ie=len(iovList)-1

        if iovList[ie] != until:
            if adjust:
                until=iovList[ie]
                log.info( "Moving end of last IOV from (%d,%d) to (%d,%d)" , end,endlumi,until[0],until[1])
            else:
                log.info( "Keeping end of last IOV at (%d,%d) - new IOV is shorter than IOV in input DB" , end,endlumi)
                iovList[ie] = until

    iovList = iovList[ib:ie]
    iovUntil = iovList[1:] + [until]
    run = since[0]
    lumi = since[1]
    log.info( "IOVs: %s" , str(iovList))
    log.info( "%d IOVs in total, end of last IOV is %s" , ie-ib,str(until))

if len(iovList)==0:
    if run<0:
        raise Exception("Please, provide run number at command line")
    else:
        log.info( "Creating single IOV starting from run,lumi %d,%d" , run,lumi)
        since = (run, lumi)
        until = (end, endlumi)
        iovList = [since]
        iovUntil = [until]

#=== Open DB connections
oschema = 'sqlite://;schema='+outFile+';dbname='+dbName
dbr = CaloCondTools.openDbConn(ischema,'READONLY')
update = update or (inFile==outFile)
dbw = CaloCondTools.openDbConn(oschema,('UPDATE' if update else 'RECREATE'))
reader = CaloCondTools.CaloBlobReader(dbr,folderPath,inTag)
writer = CaloCondTools.CaloBlobWriter(dbw,folderPath,'Flt',(outTag!="" and outTag!="HEAD"))


#== read input file
cellData = {}
ival=0
igain=0
icell=[0,0,0,0,0,0,0]
gain=-1
useNames=None
useModuleNames=None
useGain=None

if len(txtFile):
#  try:
    with open(txtFile,"r") as f:
        cellDataText = f.readlines()

    for line in cellDataText:
        fields = line.strip().split()
        #=== ignore empty and comment lines
        if not len(fields)          :
            continue
        if fields[0].startswith("#"):
            continue

        if fields[0][:1].isalpha():
            print (fields)
            if useNames is not None and not useNames:
                raise Exception("Mixture of formats in inpyt file %s - useNames" % (txtFile))
            useNames=True
            if fields[0]=='Cell':
                if useModuleNames is not None and useModuleNames:
                    raise Exception("Mixture of formats in inpyt file %s - useModuleNames" % (txtFile))
                useModuleNames=False
                modName=''
                cellName=fields[1]
            else:
                if useModuleNames is not None and not useModuleNames:
                    raise Exception("Mixture of formats in inpyt file %s - useModuleNames" % (txtFile))
                useModuleNames=True
                modName=fields[0]
                cellName=fields[1]
            if fields[2].isdigit():
                if useGain is not None and not useGain:
                    raise Exception("Mixture of formats in inpyt file %s - useGain" % (txtFile))
                useGain=True
                gain=int(fields[2])
                noise = fields[3:]
                if ival<len(noise):
                    ival=len(noise)
                if igain<gain:
                    igain=gain
            else:
                if useGain is not None and useGain:
                    raise Exception("Mixture of formats in inpyt file %s - useGain" % (txtFile))
                if len(fields)>3:
                    raise Exception("Too many fields in input file %s" % (txtFile))
                useGain=False
                gain=-1
                noise = [-1]+fields[2:]
                ival=1
            if cellName=='D0':
                cellName='D*0'
            if cellName.startswith('BC'):
                cellName='B'+cellName[2:]
            if not ('+' in cellName or '-' in cellName or '*' in cellName):
                p = re.search("\\d", cellName).start()
                cellPos = modName+cellName[:p] + '+' + cellName[p:]
                cellNeg = modName+cellName[:p] + '-' + cellName[p:]
                dictKey  = (cellPos,gain)
                cellData[dictKey] = noise
                dictKey  = (cellNeg,gain)
                cellData[dictKey] = noise
                if (cellName=='spE1'):
                    for cellNm in ['mbE+1','mbE-1','e4E+1','e4E-1']:
                        cellN = modName+cellNm
                        dictKey  = (cellN,gain)
                        if dictKey not in cellData:
                            cellData[dictKey] = noise
            else:
                cellN = modName+cellName
                dictKey  = (cellN,gain)
                cellData[dictKey] = noise
                if (cellName=='spE+1'):
                    for cellNm in ['mbE+1','e4E+1']:
                        cellN = modName+cellNm
                        dictKey  = (cellN,gain)
                        if dictKey not in cellData:
                            cellData[dictKey] = noise
                if (cellName=='spE-1'):
                    for cellNm in ['mbE-1','e4E-1']:
                        cellN = modName+cellNm
                        dictKey  = (cellN,gain)
                        if dictKey not in cellData:
                            cellData[dictKey] = noise
            icell[gain]+=1
        else:
            if useNames is not None and useNames:
                raise Exception("Mixture of formats in inpyt file %s - useNames" % (txtFile))
            useNames=False
            cellHash = int(fields[0])
            cellGain = int(fields[1])
            noise    = fields[2:]
            dictKey  = (cellHash,cellGain)
            cellData[dictKey] = noise
            if icell[gain]<cellHash:
                icell[gain]=cellHash
            if igain<cellGain:
                igain=cellGain
            if ival<len(noise):
                ival=len(noise)
    if not useNames:
        icell[gain]+=1
    else:
        print (cellData)
    igain=igain+1
#  except:
#    raise Exception("Can not read input file %s" % (txtFile))
else:
    log.info("No input txt file provided, making copy from input DB to output DB")

nval=ival
ngain=igain
ncell=max(icell)

if not tile:
    modName="LAr %2d" % chan
    cellName=""
    fullName=modName

#=== loop over all iovs
for io,since in enumerate(iovList):

    sinceRun = since[0]
    sinceLum = since[1]

    until=iovUntil[io]
    untilRun = until[0]
    untilLum = until[1]

    log.info("")
    log.info("Updating IOV [%d,%d] - [%d,%d)" , sinceRun, sinceLum, untilRun, untilLum)

    blobR = reader.getCells(chan,(sinceRun,sinceLum))
    if blobR is None:
        continue
    mcell=blobR.getNChans()
    mgain=blobR.getNGains()
    mval=blobR.getObjSizeUint32()

    log.info("input file: ncell: %d ngain %d nval %d" , max(icell), igain, ival)
    log.info("input db:   ncell: %d ngain %d nval %d" , mcell, mgain, mval)
    if mcell>ncell:
        ncell=mcell
    if mgain>ngain:
        ngain=mgain
    if mval>nval:
        nval=mval

    log.info("output db:  ncell: %d ngain %d nval %d" , ncell, ngain, nval)

    if ncell>hashMgrA.getHashMax():
        hashMgr=hashMgrABC
    elif ncell>hashMgrBC.getHashMax():
        hashMgr=hashMgrA
    elif ncell>hashMgrDef.getHashMax():
        hashMgr=hashMgrBC
    else:
        hashMgr=hashMgrDef
    log.info("Using %s CellMgr with hashMax %d" , hashMgr.getGeometry(),hashMgr.getHashMax())

    GainDefVec = cppyy.gbl.std.vector('float')()
    for val in range(nval):
        GainDefVec.push_back(0.0)
    defVec = cppyy.gbl.std.vector('std::vector<float>')()
    for gain in range(ngain):
        defVec.push_back(GainDefVec)
    blobW = writer.getCells(chan)
    blobW.init(defVec,ncell,1)

    src = ['Default','DB','File','Scale']
    FullName=None
    cell=None
    gain=None
    field=None
    strval=None
    noise=None

    try:
        for cell in range(ncell):
            exist0 = (cell<mcell)
            if tile:
                (modName,cellName)=hashMgr.getNames(cell)
                fullName="%s %6s" % (modName,cellName)
            for gain in range(ngain):
                exist1 = (exist0 and (gain<mgain))
                if useNames:
                    if useGain:
                        gn=gain
                    else:
                        gn=-1
                    if useModuleNames:
                        dictKey = (modName+cellName,gn)
                    else:
                        dictKey = (cellName,gn)
                else:
                    dictKey = (cell, gain)
                noise = cellData.get(dictKey,[])
                nF = len(noise)
                for field in range(nval):
                    exist = (exist1 and (field<mval))
                    value = GainDefVec[field]
                    if field<nF:
                        strval=str(noise[field])
                        if strval.startswith("*"):
                            coef=float(strval[1:])
                            value = blobR.getData(cell,gain,field)*coef
                        elif strval.startswith("++") or strval.startswith("+-") :
                            coef=float(strval[1:])
                            value = blobR.getData(cell,gain,field)+coef
                        elif strval=="keep" or strval=="None":
                            value = None
                        else:
                            value = float(strval)
                        if (value is None or value<0) and exist: # negative value means that we don't want to update this field
                            value = blobR.getData(cell,gain,field)
                        elif exist:
                            exist = 2
                    elif exist:
                        value = blobR.getData(cell,gain,field)
                        if rescale:
                            if field==1 or field>4:
                                if 'spC' in cellName:
                                    sc = scaleC10sp
                                elif 'spD' in cellName:
                                    sc = scaleD4sp
                                elif 'C' in cellName and '10' in cellName:
                                    sc = scaleC10
                                elif 'D' in cellName and '4'  in cellName:
                                    sc = scaleD4
                                elif 'E' in cellName:
                                    sc = scaleE
                                elif 'D' in cellName:
                                    sc = scaleD
                                elif 'B' in cellName:
                                    sc = scaleB
                                elif 'A' in cellName:
                                    sc = scaleA
                                else:
                                    sc = scale
                                if sc>0.0:
                                    exist = 3
                                    value *= sc
                                    src[exist] = "ScalePileUp %s" % str(sc)
                            elif field<4 and scaleElec>0.0:
                                exist = 3
                                value *= scaleElec
                                src[exist] = "ScaleElec %s" % str(scaleElec)

                    blobW.setData( cell, gain, field, value )
                    if rescale or exist>1:
                        print ("%s hash %4d gain %d field %d value %f Source %s" % (fullName, cell, gain, field, value, src[exist]))
    except Exception as e:
        log.warning("Exception on IOV [%d,%d]-[%d,%d)" , sinceRun, sinceLum, untilRun, untilLum)
        print (FullName,"Cell",cell,"gain",gain,"field",field,"value",strval,"noise vector",noise)
        #e = sys.exc_info()[0]
        print (e)

    writer.register((sinceRun,sinceLum), (untilRun,untilLum), outTag)

log.info("Using %s CellMgr with hashMax %d" , hashMgr.getGeometry(),hashMgr.getHashMax())
#=== Cleanup
dbw.closeDatabase()
dbr.closeDatabase()

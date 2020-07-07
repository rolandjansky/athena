#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# WriteBchToCool.py
# Alexander Solodkov <Sanya.Solodkov@cern.ch>, 2014-09-09
# change Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

from __future__ import print_function

import getopt,sys,os,bisect
os.environ['TERM'] = 'linux'

def usage():
    print ("Usage: ", sys.argv[0]," [OPTION] ... ")
    print ("Update TileCal bad channels in COOL")
    print ("")
    print ("-h, --help      shows this help")
    print ("-f, --folder=   specify folder to use, default is /TILE/OFL02/STATUS/ADC")
    print ("-t, --tag=      specify tag to use, default is RUN2-HLT-UPD1-00")
    print ("-r, --run=      specify run  number, default is 0")
    print ("-l, --lumi=     specify lumi block number, default is 0")
    print ("-b, --begin=    specify run number of first iov in multi-iov mode, by default uses very first iov")
    print ("-e, --end=      specify run number of last iov in multi-iov mode, by default uses latest iov")
    print ("-L, --endlumi=  specify lumi block number for last iov in multi-iov mode, default is 0")
    print ("-A, --adjust    in multi-iov mode adjust iov boundaries to nearest iov available in DB, default is False")
    print ("-M, --module=   specify module to use in multi-IOV update, default is all")
    print ("-m, --mode=     specify update mode: 1 or 2; if not set - choosen automatically, depending on schema")
    print ("-c, --comment=    specify comment which should be written to DB, in multi-iov mode it is appended to old comment")
    print ("-C, --Comment=    specify comment which should be written to DB, in mutli-iov mode it overwrites old comment")
    print ("-U, --user=       specify username for comment")
    print ("-x, --execfile=   specify python file which should be executed, default is bch.py")
    print ("-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'")
    print ("-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'")
    print ("-n, --online      write additional sqlite file with online bad channel status")
    print ("-p, --upd4        write additional sqlite file with CURRENT UPD4 tag")
    print ("-v, --verbose     verbose mode")
    print ("-s, --schema=     specify input/output schema to use when both input and output schemas are the same")
    print ("-u  --update      set this flag if output sqlite file should be updated, otherwise it'll be recreated")

letters = "hr:l:b:e:L:AM:m:s:i:o:t:f:x:c:C:U:npvu"
keywords = ["help","run=","lumi=","begin=","end=","endlumi=","adjust","module=","mode=","schema=","inschema=","outschema=","tag=","folder=","execfile=","comment=","Comment=","user=","online","upd4","verbose","update"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:], letters, keywords)
except getopt.GetoptError as err:
    print (str(err))
    usage()
    sys.exit(2)

# defaults
run = -1
lumi = 0
mode = 0
schema = 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'
oraSchema = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2"
inSchema = oraSchema
outSchema = schema
folderPath =  "/TILE/OFL02/STATUS/ADC"
onlSuffix = ""
curSuffix = ""
tag = "UPD1"
execFile = "bch.py"
comment = ""
Comment = None
verbose = False
update = False
iov = False
beg = 0
end = 2147483647
endlumi = 0
moduleList = []
adjust = False

try:
    user=os.getlogin()
except Exception:
    user="UnknownUser"

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
    elif o in ("-n","--online"):
        onlSuffix = "_onl"
    elif o in ("-p","--upd4"):
        curSuffix = "_upd4"
    elif o in ("-u","--update"):
        update = True
    elif o in ("-r","--run"):
        run = int(a)
    elif o in ("-l","--lumi"):
        lumi = int(a)
    elif o in ("-b","--begin"):
        beg = int(a)
        iov = True
    elif o in ("-e","--end"):
        end = int(a)
        iov = True
    elif o in ("-L","--endlumi"):
        endlumi = int(a)
    elif o in ("-A","--adjust"):
        adjust = True
    elif o in ("-M","--module"):
        moduleList += a.split(",")
    elif o in ("-m","--mode"):
        mode = int(a)
    elif o in ("-x","--execfile"):
        execFile = a
    elif o in ("-c","--comment"):
        comment = a
    elif o in ("-C","--Comment"):
        Comment = a
        comment = a
    elif o in ("-U","--user"):
        user = a
    elif o in ("-v","--verbose"):
        verbose = True
    elif o in ("-h","--help"):
        usage()
        sys.exit(2)
    else:
        assert False, "unhandeled option"

onl=("/TILE/ONL01" in folderPath)
if onl:
    if inSchema == oraSchema:
        inSchema = inSchema.replace("COOLOFL","COOLONL")
    oraSchema = oraSchema.replace("COOLOFL","COOLONL")
if not len(outSchema):
    outSchema = schema
else:
    schema = outSchema
if not len(inSchema):
    inSchema = schema
update = update or (inSchema==outSchema)

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobObjs.Classes import TileBchPrbs, TileBchDecoder, TileCalibUtils

if iov and end >= TileCalibTools.MAXRUN:
    end = TileCalibTools.MAXRUN
    endlumi = TileCalibTools.MAXLBK
until = (TileCalibTools.MAXRUN,TileCalibTools.MAXLBK)

from TileCalibBlobPython.TileCalibLogger import getLogger
log = getLogger("WriteBchToCool")
import logging
log.setLevel(logging.DEBUG)

dbr = TileCalibTools.openDbConn(inSchema,'READONLY')
folderTag = TileCalibTools.getFolderTag(dbr, folderPath, tag)
log.info("Initializing bad channels from %s folder %s with tag %s", inSchema, folderPath, tag)

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
        for ros in range(1,5):
            for mod in range(0,64):
                modName = TileCalibUtils.getDrawerString(ros,mod)
                if len(moduleList)>0 and modName not in moduleList and 'ALL' not in moduleList:
                    continue
                iovList += blobReader.getIOVsWithinRange(ros,mod)
    if 'CMT' in moduleList:
        iovListMOD = iovList
        iovListCMT = blobReader.getIOVsWithinRange(-1,1000)
        if len(iovList)==0:
            iovList = iovListCMT

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
            log.info( "Moving beginning of first IOV with new bad channels from (%d,%d) to (%d,%d)", beg,lumi,since[0],since[1])
        else:
            iovList[ib] = since
            log.info( "Creating new IOV starting from (%d,%d) with new bad channels", beg,lumi)

    if end<0:
        ie=ib+1
        if ie>=len(iovList):
            ie=ib
        until=iovList[ie]
        log.info( "Next IOV without new bad channels starts from (%d,%d)", until[0],until[1])
    else:
        until=(end,endlumi)
        ie=bisect.bisect_left(iovList,until)
        if ie>=len(iovList):
            ie=len(iovList)-1

        if iovList[ie] != until:
            if adjust:
                until=iovList[ie]
                log.info( "Moving end of last IOV from (%d,%d) to (%d,%d)", end,endlumi,until[0],until[1])
            else:
                log.info( "Keeping end of last IOV at (%d,%d) - new IOV is shorter than IOV in input DB", end,endlumi)
                iovList[ie] = until


    iovList = iovList[ib:ie]
    iovUntil = iovList[1:] + [until]
    begin = since
    run = since[0]
    lumi = since[1]
    log.info( "IOVs: %s", str(iovList) )

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
    else:
        iovUntilCMT = iovUntil

    log.info( "%d IOVs in total, end of last IOV is %s", ie-ib,str(until))

else:
    #=== set run number
    if run<0:
        lumi=0
        if "UPD4" in folderTag:
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
    begin=since

    log.info("Initializing for run %d, lumiblock %d", run,lumi)


if mode == 0:
    if inSchema == outSchema:
        mode = 1
    else:
        mode = 2
elif mode != 1 and mode != 2:
    log.error( "Bad mode %d", mode )
    sys.exit(2)

#=== create bad channel manager
log.info("")
comments = []
mgrWriters = []
nvalUpdated = []
commentsSplit = []
for since in iovList:
    comm=blobReader.getComment(since)
    #log.info("Comment: %s", comm)
    comments+=[comm]
    nvalUpdated += [0]
    commentsSplit+=[blobReader.getComment(since,True)]
    mgr = TileBchTools.TileBchMgr()
    mgr.setLogLvl(logging.DEBUG)
    mgr.initialize(dbr, folderPath, folderTag, since, mode)
    mgrWriters += [mgr]
log.info("")

#=== Tuples of empty channels
emptyChannelLongBarrel =     (30, 31, 43)
emptyChannelExtendedBarrel = (18, 19, 24, 25, 26, 27, 28, 29, 33, 34, 42, 43, 44, 45, 46, 47)

# remember: addAdcProblem(ros, module, channel, adc), where:
# ros = 1 LBA
# ros = 2 LBC
# ros = 3 EBA
# ros = 4 EBC
# module = 0 - 63
# channel = 0 - 47
# adc: 0 = low gain, 1 = high gain.

#=== print bad channels
if verbose and not iov:
    log.info("============================================================== ")
    log.info("bad channels before update")
    mgr.listBadAdcs()

#=== Add problems with mgr.addAdcProblem(ros, drawer, channel, adc, problem)
#=== Remove problems with mgr.delAdcProblem(ros, drawer, channel, adc, problem)


if len(execFile):
    log.info("Masking new bad channels, including file %s", execFile )
    dbw = TileCalibTools.openDbConn(outSchema,('UPDATE' if update else 'RECREATE'))

    #=== loop over all IOVs
    for io,since in enumerate(iovList):

        until=iovUntil[io]
        untilCmt=iovUntilCMT[io]
        if since==until and since==untilCmt:
            continue # nothing to do

        log.info( "Updating IOV %s", str(since) )
        mgr = mgrWriters[io]

        if since==until or (since in iovListCMT and since not in iovListMOD):
            mList = ['CMT']
        else:
            mList = moduleList
            try:
                exec(compile(open(execFile).read(),execFile,'exec'))
                if len(comment)==0:
                    log.error( "Comment string is not provided, please put comment='bla-bla-bla' line in %s", execFile )
                    sys.exit(2)
            except Exception as e:
                log.error( e )
                log.error( "Problem reading include file %s", execFile )
                sys.exit(2)

            #=== print bad channels
            if verbose:
                log.info("============================================================== ")
                log.info("bad channels after update")
                mgr.listBadAdcs()

        #====================== Write new bad channel list =================

        #=== commit changes
        if Comment is not None:
            comment = Comment
            author = user
        else:
            if comment=="None":
                comment = comments[io]
            elif iov and comments[io] not in comment:
                comment += "  //  " + comments[io]
            if io>0 and since!=until and 'ALL' not in moduleList:
                author=commentsSplit[io]
                for m in moduleList:
                    if m in comments[io]:
                        author=user
                        break
            else:
                author=user
        # UNDO comment in IOV after the "end"
        if since==until and comment!=comments[io]:
            comment = "UNDO " + comment
        mgr.commitToDb(dbw, folderPath, folderTag, (TileBchDecoder.BitPat_onl01 if onl else TileBchDecoder.BitPat_ofl01), author, comment, since, until, untilCmt, mList)
else:
    dbw = None


since = iovList[0]
until = (TileCalibTools.MAXRUN,TileCalibTools.MAXLBK)

if len(curSuffix) and not onl and "sqlite" in outSchema:

    if len(comment) == 0:
        reader = TileCalibTools.TileBlobReader(dbr, folderPath, folderTag)
        comment=reader.getComment(since)
        if comment.find("): ") > -1:
            comment = comment[(comment.find("): ")) + 3:]

    log.info("")
    log.info("============================================================== ")
    log.info("")
    log.info("creating DB with CURRENT UPD4 tag")

    folderTagUPD4 = TileCalibTools.getFolderTag(dbr, folderPath, "UPD4" )
    if folderTagUPD4 == folderTag:
        log.warning("CURRENT UPD4 tag %s is identical to the tag in DB which was created already", folderTagUPD4)
        folderTagUPD4 = TileCalibTools.getFolderTag(dbr, folderPath, "UPD1" )
        log.warning("Additional UPD1 DB with tag %s will be created instead", folderTagUPD4 )
        curSchema = outSchema.replace(".db","_upd1.db")
    else:
        curSchema = outSchema.replace(".db",curSuffix+".db")

    folder = dbr.getFolder(folderPath)
    if folderTagUPD4 not in folder.listTags():
        log.warning( "Tag %s not found in input schema, reading previous status from Oracle", folderTagUPD4)
        dbR = TileCalibTools.openDbConn(oraSchema,'READONLY')
        mgr.updateFromDb(dbR, folderPath, folderTagUPD4, since, 0, 2)
        dbR.closeDatabase()
    else:
        mgr.updateFromDb(dbr, folderPath, folderTagUPD4, since, 0, 2)

    #=== commit changes
    dbW = TileCalibTools.openDbConn(curSchema,('UPDATE' if update else 'RECREATE'))
    mgr.commitToDb(dbW, folderPath, folderTagUPD4, TileBchDecoder.BitPat_ofl01, user, comment, since, until)
    dbW.closeDatabase()


if len(onlSuffix) and not onl and "sqlite" in outSchema:

    if len(comment)==0:
        reader = TileCalibTools.TileBlobReader(dbr, folderPath, folderTag)
        comment = reader.getComment(since)
        if comment.find("): ") > -1:
            comment = comment[(comment.find("): ")) + 3:]

    log.info("")
    log.info("============================================================== ")
    log.info("")
    log.info("creating DB with ONLINE status")

    #if dbw:
    #    mgr.updateFromDb(dbw, folderPath, folderTag, since, -1)

    #--- create online bad channel manager
    folderOnl = "/TILE/ONL01/STATUS/ADC"
    folderTagOnl = ""

    inSchemaOnl = inSchema.replace("COOLOFL", "COOLONL")
    if inSchemaOnl != inSchema:
        dbr.closeDatabase()
        dbr = TileCalibTools.openDbConn(inSchemaOnl, 'READONLY')
    else:
        try:
            reader = TileCalibTools.TileBlobReader(dbr, folderOnl, folderTagOnl)
        except Exception:
            log.warning( "No %s folder in %s, reading from Oracle", folderOnl, inSchema)
            inSchemaOnl = oraSchema.replace("COOLOFL", "COOLONL")
            dbr.closeDatabase()
            dbr = TileCalibTools.openDbConn(inSchemaOnl, 'READONLY')

    mgrOnl = TileBchTools.TileBchMgr()
    mgrOnl.setLogLvl(logging.DEBUG)
    mgrOnl.initialize(dbr, folderOnl, folderTagOnl, since, 2)

    #=== print online channel status
    if verbose:
        log.info("============================================================== ")
        log.info("online channel status BEFORE update")
        mgrOnl.listBadAdcs()

    #=== synchronize
    for ros in range(1, 5):
        for mod in range(0, 64):
            for chn in range(0, 48):
                statlo = mgr.getAdcStatus(ros, mod, chn, 0)
                stathi = mgr.getAdcStatus(ros, mod, chn, 1)

                # remove all trigger problems first
                for prb in [TileBchPrbs.TrigGeneralMask,
                            TileBchPrbs.TrigNoGain,
                            TileBchPrbs.TrigHalfGain,
                            TileBchPrbs.TrigNoisy]:
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, prb)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, prb)
                # and now set new trigger problems (if any)
                if not statlo.isGood():
                    prbs = statlo.getPrbs()
                    for prb in prbs:
                        if prb in [TileBchPrbs.TrigGeneralMask,
                                   TileBchPrbs.TrigNoGain,
                                   TileBchPrbs.TrigHalfGain,
                                   TileBchPrbs.TrigNoisy]:
                            mgrOnl.addAdcProblem(ros, mod, chn, 0, prb)
                            mgrOnl.addAdcProblem(ros, mod, chn, 1, prb)

                #--- add IgnoreInHlt if either of the ADCs has isBad
                #--- add OnlineGeneralMaskAdc if the ADCs has isBad
                if statlo.isBad() and stathi.isBad():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                elif statlo.isBad():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                elif stathi.isBad():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                else:
                    #--- delete IgnoreInHlt and OnlineGeneralMaskAdc if both ADCs are not Bad
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)

                #--- add OnlineBadTiming if either of the ADCs has isBadTiming
                if statlo.isBadTiming() or stathi.isBadTiming():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineBadTiming)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineBadTiming)
                else:
                    #--- delete OnlineBadTiming if the both ADCs has not isBadTiming
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineBadTiming)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineBadTiming)

                #--- add OnlineTimingDmuBcOffset if either of the ADCs has isTimingDmuBcOffset
                if statlo.isTimingDmuBcOffset() or stathi.isTimingDmuBcOffset():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineTimingDmuBcOffset)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineTimingDmuBcOffset)
                else:
                    #--- delete OnlineTimingDmuBcOffset if the both ADCs has not isTimingDmuBcOffset
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineTimingDmuBcOffset)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineTimingDmuBcOffset)

                #--- add OnlineWrongBCID if either of the ADCs has isWrongBCID
                if statlo.isWrongBCID() or stathi.isWrongBCID():
                    mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineWrongBCID)
                    mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineWrongBCID)
                else:
                    #--- delete OnlineWrongBCID if the both ADCs has not isWrongBCID
                    mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineWrongBCID)
                    mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineWrongBCID)


    #=== print online channel status
    if verbose:
        log.info("online channel status AFTER update")
        mgrOnl.listBadAdcs()

    #=== commit changes
    onlSchema = outSchema.replace(".db", onlSuffix + ".db")
    dbW = TileCalibTools.openDbConn(onlSchema,('UPDATE' if update else 'RECREATE'))
    mgrOnl.commitToDb(dbW, folderOnl, folderTagOnl, TileBchDecoder.BitPat_onl01, user, comment, since, until)
    dbW.closeDatabase()

#=== close DB
if dbr:
    dbr.closeDatabase()
if dbw:
    dbw.closeDatabase()

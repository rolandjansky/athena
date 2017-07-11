#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# WriteBchToCool.py
# Alexander Solodkov <Sanya.Solodkov@cern.ch>, 2014-09-09
# change Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import getopt,sys,os,string
os.environ['TERM'] = 'linux'

def usage():
    print "Usage: ", sys.argv[0]," [OPTION] ... "
    print "Update TileCal bad channels in COOL"
    print ""
    print "-h, --help      shows this help"
    print "-f, --folder=   specify folder to use, default is /TILE/OFL02/STATUS/ADC"
    print "-t, --tag=      specify tag to use, default is RUN2-HLT-UPD1-00"
    print "-r, --run=      specify run  number, default is 0"
    print "-l, --lumi=     specify lumi block number, default is 0"
    print "-m, --mode=     specify update mode: 1 or 2; if not set - choosen automatically, depending on schema"
    print "-c, --comment=    specify comment which should be written to DB"
    print "-e, --execfile=   specify python file which should be executed, default is bch.py"
    print "-i, --inschema=   specify the input schema to use, default is 'oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_TILE;dbname=CONDBR2'"
    print "-o, --outschema=  specify the output schema to use, default is 'sqlite://;schema=tileSqlite.db;dbname=CONDBR2'"
    print "-n, --online      write additional sqlite file with online bad channel status"
    print "-p, --upd4        write additional sqlite file with CURRENT UPD4 tag"
    print "-v, --verbose     verbose mode"
    print "-s, --schema=     specify input/output schema to use when both input and output schemas are the same"
    print "-u  --update      set this flag if output sqlite file should be updated, otherwise it'll be recreated"
    
letters = "hr:l:m:s:i:o:t:f:e:c:npvu"
keywords = ["help","run=","lumi=","mode=","schema=","inschema=","outschema=","tag=","folder=","execfile=","comment=","online","upd4","verbose","update"]

try:
    opts, extraparams = getopt.getopt(sys.argv[1:], letters, keywords)
except getopt.GetoptError, err:
    print str(err)
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
verbose = False
update = False

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
    elif o in ("-m","--mode"):
        mode = int(a)
    elif o in ("-e","--execfile"):
        execFile = a
    elif o in ("-c","--comment"):
        comment = a
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
if not len(outSchema): outSchema = schema
else: schema = outSchema
if not len(inSchema): inSchema = schema
update = update or (inSchema==outSchema)

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("WriteBchToCool")
import logging
log.setLevel(logging.DEBUG)

dbr = TileCalibTools.openDbConn(inSchema,'READONLY')
folderTag = TileCalibTools.getFolderTag(dbr, folderPath, tag)

if run < 0: since = (TileCalibTools.MAXRUN, TileCalibTools.MAXLBK - 1) 
else: since = (run, lumi)
until = (TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)

if mode == 0:
    if inSchema == outSchema: mode = 1
    else: mode = 2
elif mode != 1 and mode != 2:
    log.error( "Bad mode %d" % mode )
    sys.exit(2)

#=== create bad channel manager
log.info("")
log.info("Initializing bad channels from %s" % (inSchema))
log.info("with tag=%s and IOV=%s" % (folderTag, since))
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logging.DEBUG)
mgr.initialize(dbr, folderPath, folderTag, since, mode)

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
if verbose:
    log.info("============================================================== ")
    log.info("bad channels before update")
    mgr.listBadAdcs()

#=== Add problems with mgr.addAdcProblem(ros, drawer, channel, adc, problem)
#=== Remove problems with mgr.delAdcProblem(ros, drawer, channel, adc, problem)

if run<0:
    run=TileCalibTools.getLastRunNumber()
    log.warning( "Run number is not specified, using current run number %d" %run )
    if run<0:
        log.error( "Bad run number" )
        sys.exit(2)
since = (run, lumi)

if len(execFile):
    log.info("Masking new bad channels, including file %s" % execFile )

    try:
        execfile(execFile)
        if len(comment)==0:
            log.error( "Comment string is not provided, pleae put comment='bla-bla-bla' line in %s" % execFile )
            sys.exit(2)
    except Exception, e:
        log.error( e )
        log.error( "Problem reading include file %s" % execFile )
        sys.exit(2)

    #=== print bad channels
    if verbose:
        log.info("============================================================== ")
        log.info("bad channels after update")
        mgr.listBadAdcs()

    #====================== Write new bad channel list =================

    #=== commit changes
    dbw = TileCalibTools.openDbConn(outSchema,('UPDATE' if update else 'RECREATE'))
    mgr.commitToDb(dbw, folderPath, folderTag, (TileBchDecoder.BitPat_onl01 if onl else TileBchDecoder.BitPat_ofl01), os.getlogin(), comment, since, until)
else:
    dbw = None


if len(curSuffix) and not onl and "sqlite" in outSchema:

    if len(comment) == 0:
        reader = TileCalibTools.TileBlobReader(dbr, folderPath, folderTag)
        comment=reader.getComment(since)
        if comment.find("): ") > -1: comment = comment[(comment.find("): ")) + 3:]

    log.info("")
    log.info("============================================================== ")
    log.info("")
    log.info("creating DB with CURRENT UPD4 tag")

    folderTagUPD4 = TileCalibTools.getFolderTag(dbr, folderPath, "UPD4" )
    if folderTagUPD4 == folderTag:
        log.warning("CURRENT UPD4 tag %s is identical to the tag in DB which was created already" % folderTagUPD4)
        folderTagUPD4 = TileCalibTools.getFolderTag(dbr, folderPath, "UPD1" )
        log.warning("Additional UPD1 DB with tag %s will be created instead" % folderTagUPD4 )
        curSchema = outSchema.replace(".db","_upd1.db")
    else:
        curSchema = outSchema.replace(".db",curSuffix+".db")

    folder = dbr.getFolder(folderPath)
    if not folderTagUPD4 in folder.listTags():
        log.warning( "Tag %s not found in input schema, reading previous status from Oracle" % folderTagUPD4)
        dbR = TileCalibTools.openDbConn(oraSchema,'READONLY')
        mgr.updateFromDb(dbR, folderPath, folderTagUPD4, since, 0, 2)
        dbR.closeDatabase()
    else:
        mgr.updateFromDb(dbr, folderPath, folderTagUPD4, since, 0, 2)

    #=== commit changes
    dbW = TileCalibTools.openDbConn(curSchema,('UPDATE' if update else 'RECREATE'))
    mgr.commitToDb(dbW, folderPath, folderTagUPD4, TileBchDecoder.BitPat_ofl01, os.getlogin(), comment, since, until)
    dbW.closeDatabase()


if len(onlSuffix) and not onl and "sqlite" in outSchema:

    if len(comment)==0:
        reader = TileCalibTools.TileBlobReader(dbr, folderPath, folderTag)
        comment = reader.getComment(since)
        if comment.find("): ") > -1: comment = comment[(comment.find("): ")) + 3:]

    log.info("")
    log.info("============================================================== ")
    log.info("")
    log.info("creating DB with ONLINE status")

    # if dbw: mgr.updateFromDb(dbw, folderPath, folderTag, since, -1)

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
        except Exception, e:
            log.warning( "No %s folder in %s, reading from Oracle" % (folderOnl, inSchema))
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
    for ros in xrange(1, 5):
        for mod in xrange(0, 64):
            for chn in xrange(0, 48):
                statlo = mgr.getAdcStatus(ros, mod, chn, 0)
                stathi = mgr.getAdcStatus(ros, mod, chn, 1)
        
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


    #=== print online channel status
    if verbose:
        log.info("online channel status AFTER update")
        mgrOnl.listBadAdcs()

    #=== commit changes
    onlSchema = outSchema.replace(".db", onlSuffix + ".db")
    dbW = TileCalibTools.openDbConn(onlSchema,('UPDATE' if update else 'RECREATE'))
    mgrOnl.commitToDb(dbW, folderOnl, folderTagOnl, TileBchDecoder.BitPat_onl01, os.getlogin(), comment, since, until)
    dbW.closeDatabase()

#=== close DB
if dbr: dbr.closeDatabase()
if dbw: dbw.closeDatabase()

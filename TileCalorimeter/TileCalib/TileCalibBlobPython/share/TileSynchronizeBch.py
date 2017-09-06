#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileSynchronizeBch.py <TAG1> <TAG2> <MASKONLY> <RUN1> <RUN2>
# sanya.solodkov@cern.ch July 2016
# copy bad status from one tag to another (e.g. from UPD4 to UPD1)
# <TAG1> - origin tag (no default), can be UPD1 or UPD4 or exact tag
# <TAG2> - destination (no default) can be UPD1 or UPD4 or ONL (for online DB)
# <MASKONLY> - if "1" or "yes" or "True", channels are only masked and never unmasked
# <RUN1> - run number to use for <TAG1> (default = MAXRUN)
# <RUN2> - run number to use for <TAG2> and for sqlite (default = current run)

import os,sys

tag1 = "" if len(sys.argv) < 2 else sys.argv[1].rpartition("=")[2]
tag2 = "" if len(sys.argv) < 3 else sys.argv[2].rpartition("=")[2]
opt  = "" if len(sys.argv) < 4 else sys.argv[3].rpartition("=")[2]
run1 = None if len(sys.argv) < 5 else int(sys.argv[4].rpartition("=")[2])
run2 = None if len(sys.argv) < 6 else int(sys.argv[5].rpartition("=")[2])


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("SyncBch")
import logging
log.setLevel(logging.DEBUG)


log.info("")
if tag1=="":
    log.error( "Please, use non-empty tag as first parameter (e.g. UPD1)")
    sys.exit(2)
if tag2=="":
    log.error( "Please, use non-empty tag as second parameter (e.g. UPD4)")
    sys.exit(2)
if tag1==tag2:
    log.error( "Please, use different tags as first and second parameter (e.g. UPD1 UPD4)")
    sys.exit(2)

opt=opt[0].upper() if len(opt)>0 else " "
copyall = not (opt=="1" or opt=="Y" or opt=="T" or opt=="B")
if copyall:
    log.info( "Copying all statuses from %s to %s" % (tag1,tag2) )
else:
    log.info( "Copying only BAD statuses from %s to %s" % (tag1,tag2) )

if run1 is None or run1 < 0:
    badrun=run1
    run1=MAXRUN
    if badrun is None:
        log.info( "First run number was not specified, using maximal possible run number %d for input DB" % run1 )
    else:
        log.warning( "First run number %d is bad, using maximal possible run number %d for input DB" % (badrun, run1) )
if run2 is None or run2 < 0:
    badrun=run2
    run2=TileCalibTools.getLastRunNumber()
    if badrun is None:
        log.info( "Second run number was not specified, using current run number %d for output DB" % run2 )
    else:
        log.warning( "Second run number %d is bad, using current run number %d for output DB" % (badrun, run2) )
    if run2 is None or run2<0:
        log.error( "Still bad run number")
        sys.exit(2)
log.info("")

#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== get DB1
db1 = TileCalibTools.openDb('ORACLE', 'CONDBR2', 'READONLY', 'COOLOFL_TILE')
folder1 = "/TILE/OFL02/STATUS/ADC"
folderTag1 = TileCalibTools.getFolderTag(db1, folder1, tag1)

#--- create first bad channel manager
mgr1 = TileBchTools.TileBchMgr()
mgr1.setLogLvl(logging.DEBUG)
log.info("Initializing with offline bad channels at tag=%s and time=%s" % (folderTag1, (run1, 0)))
mgr1.initialize(db1, folder1, folderTag1, (run1,0))

#=== get DB2
online = tag2[0:3].upper()=="ONL"
if online:
    db2 = TileCalibTools.openDb('ORACLE', 'CONDBR2', 'READONLY', 'COOLONL_TILE')
    folder2 = "/TILE/ONL01/STATUS/ADC"
    folderTag2 = ""
else:
    db2 = TileCalibTools.openDb('ORACLE', 'CONDBR2', 'READONLY', 'COOLOFL_TILE')
    folder2 = "/TILE/OFL02/STATUS/ADC"
    folderTag2 = TileCalibTools.getFolderTag(db2, folder2, tag2)

#--- create second bad channel manager
mgr2 = TileBchTools.TileBchMgr()
mgr2.setLogLvl(logging.DEBUG)
mgr2.initialize(db2, folder2, folderTag2, (run2,0), 2)

#=== synchronize
comment=""
for ros in xrange(1,5):
    for mod in xrange(0,64):
        modName = TileCalibUtils.getDrawerString(ros, mod)
        comm = ""
        for chn in xrange(0, 48):
            statlo = mgr1.getAdcStatus(ros, mod, chn, 0)
            stathi = mgr1.getAdcStatus(ros, mod, chn, 1)

            statloBefore = mgr2.getAdcProblems(ros,mod,chn,0)
            stathiBefore = mgr2.getAdcProblems(ros,mod,chn,1)

            if online:
                if copyall or statlo.isBad() or stathi.isBad():
                    #--- add IgnoreInHlt if either of the ADCs has isBad
                    #--- add OnlineGeneralMaskAdc if the ADCs has isBad            
                    if statlo.isBad() and stathi.isBad():
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                    elif statlo.isBad():
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                        mgr2.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                    elif stathi.isBad():
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                        mgr2.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
                    else:
                        #--- delete IgnoreInHlt and OnlineGeneralMaskAdc if both ADCs are not Bad
                        mgr2.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                        mgr2.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                        mgr2.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                        mgr2.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
            
                    #--- add OnlineBadTiming if either of the ADCs has isBadTiming
                    if statlo.isBadTiming() or stathi.isBadTiming():
                        mgr2.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineBadTiming)
                        mgr2.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineBadTiming)
                    else:
                        #--- delete OnlineBadTiming if the both ADCs has not isBadTiming
                        mgr2.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineBadTiming)
                        mgr2.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineBadTiming)
            else:
                if copyall or (statlo.isBad() and not mgr2.getAdcStatus(ros, mod, chn, 0).isBad()): 
                    mgr2.setAdcStatus(ros,mod,chn,0,statlo)
                if copyall or (stathi.isBad() and not mgr2.getAdcStatus(ros, mod, chn, 1).isBad()):
                    mgr2.setAdcStatus(ros,mod,chn,1,stathi)

            statloAfter = mgr2.getAdcProblems(ros,mod,chn,0)
            stathiAfter = mgr2.getAdcProblems(ros,mod,chn,1)

            if (statloBefore != statloAfter) or (stathiBefore != stathiAfter):
                pbm = [statloBefore, stathiBefore, statloAfter, stathiAfter]
                #print modName,"%3d 0"%chn,statloBefore,"=>",statloAfter
                #print modName,"%3d 1"%chn,stathiBefore,"=>",stathiAfter
                for adc in xrange(2):
                    if pbm[adc] != pbm[adc + 2]:
                        for pb in xrange(2):
                            if pb: msg += "  =>"
                            else: msg = "%s %2i %1i " % (modName, chn, adc) 
                            prbs = pbm[adc+pb*2]
                            if len(prbs):
                                for prbCode in sorted(prbs.keys()):
                                    prbDesc = prbs[prbCode]
                                    msg += " %5i (%s)" % (prbCode, prbDesc)
                            else:
                                msg += "  GOOD"
                        log.info(msg)
                comm += " ch %d" % chn
        if len(comm):
            comment += " " + modName + comm

#=== commit changes
if len(comment):
    if online:
        dbw = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'RECREATE','COOLONL_TILE')
        mgr2.commitToDb(dbw, folder2, folderTag2, TileBchDecoder.BitPat_onl01, "tilebeam", "synchronizing with %s; updated channels:%s" %(tag1, comment), (run2,0))
    else:
        dbw = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'RECREATE','COOLOFL_TILE')
        mgr2.commitToDb(dbw, folder2, folderTag2, TileBchDecoder.BitPat_ofl01, "tilebeam", "synchronizing with %s; updated channels:%s" %(tag1, comment), (run2,0))
    dbw.closeDatabase()
else:
    log.warning("Folders are in sync, nothing to update")

#=== close databases
db1.closeDatabase()
db2.closeDatabase()


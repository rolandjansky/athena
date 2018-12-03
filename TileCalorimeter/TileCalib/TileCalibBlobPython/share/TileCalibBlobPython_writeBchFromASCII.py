#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_badChannelExample.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-12-19

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *
import os

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeBch")
import logging
log.setLevel(logging.DEBUG)


#
#_______________________________________________________________________________
def fillBadChannels(db ,tag, bchFile,
                    since, until=(TileCalibTools.MAXRUN,TileCalibTools.MAXLBK)):
    
    #=== ADC status folder (write to ONLINE from M7 onwards!)
##    folder = TileCalibTools.getTilePrefix(False)+"STATUS/ADC"
    folder="/TILE/OFL02/STATUS/ADC"
    #=== create bad channel manager
    mgr = TileBchTools.TileBchMgr()
    mgr.setLogLvl(logging.DEBUG)
    
    #=== always initialize with no bad channels
    log.info("Initializing with no bad channels at tag=%s and time=%s" % (tag,(0,0)))
    mgr.initialize(db, folder, tag, since)
    
    #=== update from file
    log.info("Updating from file %s" % bchFile)
    mgr.updateFromFile(bchFile)

    #=== add definition for TileBchStatus::isBad from online side
    #    mgr.addAdcProblem(0, 1, 0,0, TileBchPrbs.IgnoredInDsp)
    #    mgr.addAdcProblem(0, 1, 0,1, TileBchPrbs.IgnoredInDsp)

    #=== print bad channels
    log.info("bad channels after update")
    mgr.listBadAdcs()
    
    #=== commit changes
    #--- to ONLINE folder, i.e. tag="" and using onl01 bit pattern
##    mgr.commitToDb(db, folder, "", TileBchDecoder.BitPat_onl01, os.getlogin(), bchFile, since, until)
    mgr.commitToDb(db, folder, tag, TileBchDecoder.BitPat_ofl01, os.getlogin(), bchFile, since, until)
##    mgr.commitToDb(db, "/TILE/", "", TileBchDecoder.BitPat_onl01, os.getlogin(), bchFile, since, until)


#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
##tag = ""
tag = "TileOfl02StatusAdc-RUN2-UPD4-08"



#=== M4
#fillBadChannels(db,tag,
#                "/afs/cern.ch/user/n/ngollub/links/blob-13.X.0-nightly/TileCalorimeter/TileConditions/share/TileM4.bch",
#                (20000,0))
#=== M5
#fillBadChannels(db,tag,
#                "/afs/cern.ch/user/n/ngollub/links/blob-13.X.0-nightly/TileCalorimeter/TileConditions/share/TileM5.bch",
#                (28000,0))
#=== M6
#fillBadChannels(db,tag,
#                "/afs/cern.ch/user/n/ngollub/links/blob-13.X.0-nightly/TileCalorimeter/TileConditions/share/TileM6.bch",
#                (40000,0))

#=== M_Calo after M6 week (20080331-20080404)
#fillBadChannels(db,tag,
#                "/afs/cern.ch/user/n/ngollub/links/blob-13.X.0-nightly/TileCalorimeter/TileConditions/share/TileM6Calo.bch",
#                (52249,0))

#=== M_Calo after M6 week (20080331-20080404) update 01
#fillBadChannels(db,tag,
#                "/afs/cern.ch/user/n/ngollub/links/blob-13.X.0-nightly/TileCalorimeter/TileConditions/share/TileM6Calo01.bch",
#                (52681,0))

#=== M7
##fillBadChannels(db,tag,
##                "/afs/cern.ch/user/n/ngollub/public/badChannels/TileM7.bch",
##                (68340,0))
fillBadChannels(db,tag,
##                "TileTest.bch",
                "chanlists_to_db.txt",
                (1,0))


#=== close DB
db.closeDatabase()

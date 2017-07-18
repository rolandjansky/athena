#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeBchOnlM8.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-07-14
# Luca Fiorini <fiorini@ifae.es>

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *
import os

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeBch")
import logging
log.setLevel(logging.DEBUG)


#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================
db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')

#=== ADC status folder
folder = TileCalibTools.getTilePrefix(ofl=False)+"STATUS/ADC"

#=== no tag for online folder
folderTag = ""
    
#=== create bad channel manager
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logging.DEBUG)

#=== NOTE: the tileSqlite copy from Oracle db must be prepared to contain only IOV
#=== for runs, that have not been taken yet. Do it as follows (with -getonline and -truncate):
#=== AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" \
#=== -create -copytaglock -getonline -truncate -folder /TILE/ONL01/STATUS/ADC

#=== Initialize with a status of bad channels at a given run.
#=== It is not possible to use runnumber = 0 (all channels are good), as it can not be
#=== contained in the tileSqlite.db - runnumber already taken would block the merge with db
#=== as this is a single-version folder

log.info("Initializing with online bad channels at time=%s" % ((99000,0),))
mgr.initialize(db, folder, folderTag, (99000,0))

#=== Delete all online bad channels
for p in xrange(1,5):
    for d in xrange(0,64):
        for  i in xrange(0, 48):
            mgr.delAdcProblem(p, d, i, 0, TileBchPrbs.IgnoredInHlt)
            mgr.delAdcProblem(p, d, i, 1, TileBchPrbs.IgnoredInHlt)


emptyChannelLongBarrel =     (30, 31, 43)
emptyChannelExtendedBarrel = (18, 19, 24, 25, 26, 27, 28, 29, 33, 34, 42, 43, 44, 45, 46, 47)
emptyChannelSpecialExtendedBarrel = (0, 1, 2, 3, 24, 25, 26, 27, 28, 29, 33, 34, 42, 43, 44, 45, 46, 47)

#=== Add Online masked channels
# LBA
for i in xrange(30, 36):
   if i not in emptyChannelLongBarrel: # LBA11 DMU10-11
      mgr.addAdcProblem(1, 10, i, 0, TileBchPrbs.IgnoredInHlt)
      mgr.addAdcProblem(1, 10, i, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(1, 17, 38, 0, TileBchPrbs.IgnoredInHlt)
for i in xrange(0, 48):
   if i not in emptyChannelLongBarrel:
      mgr.addAdcProblem(1, 52, i, 0, TileBchPrbs.IgnoredInHlt)
      mgr.addAdcProblem(1, 52, i, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(1, 56, 21, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(1, 56, 21, 1, TileBchPrbs.IgnoredInHlt)

# LBC
# We don't mask this case yet, as it doesn't pose reconstruction problems
#for i in xrange(30, 36):
#   if i not in emptyChannelLongBarrel:
#      mgr.addAdcProblem(2, 2, i, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 14, 19, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(2, 14, 19, 1, TileBchPrbs.IgnoredInHlt)
for i in xrange(42, 48):
   if i not in emptyChannelLongBarrel:
      mgr.addAdcProblem(2, 19, i, 0, TileBchPrbs.IgnoredInHlt)
      mgr.addAdcProblem(2, 19, i, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(2, 23, 42, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(2, 23, 42, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(2, 26, 46, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(2, 46, 44, 0, TileBchPrbs.IgnoredInHlt)
for i in xrange(15, 18):
   if i not in emptyChannelLongBarrel:
      mgr.addAdcProblem(2, 63, i, 0, TileBchPrbs.IgnoredInHlt)
      mgr.addAdcProblem(2, 63, i, 1, TileBchPrbs.IgnoredInHlt)


# EBA
mgr.addAdcProblem(3, 24, 15, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(3, 24, 15, 1, TileBchPrbs.IgnoredInHlt)
for i in xrange(21, 24):
   if i not in emptyChannelExtendedBarrel:
      mgr.addAdcProblem(3, 43,  i, 0, TileBchPrbs.IgnoredInHlt)
      mgr.addAdcProblem(3, 43,  i, 1, TileBchPrbs.IgnoredInHlt)

mgr.addAdcProblem(3, 54, 14, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(3, 54, 14, 1, TileBchPrbs.IgnoredInHlt)

# EBC
mgr.addAdcProblem(4,  8,  0, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4,  8,  0, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 12,  6, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 12,  7, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 12,  8, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 17,  4, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 17,  4, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 33,  0, 1, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 36, 40, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 36, 40, 1, TileBchPrbs.IgnoredInHlt)
#DataCorrption is not enough to justify the masking
#for i in xrange(0, 48):
#   if i not in emptyChannelExtendedBarrel:
#      mgr.addAdcProblem(4, 41, i, 0, TileBchPrbs.DataCorruption)
#      mgr.addAdcProblem(4, 41, i, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(4, 47, 35, 0, TileBchPrbs.IgnoredInHlt)
mgr.addAdcProblem(4, 47, 35, 1, TileBchPrbs.IgnoredInHlt)
#DataCorrption is not enough to justify the masking
#for i in xrange(30, 33):
#   if i not in emptyChannelExtendedBarrel:
#      mgr.addAdcProblem(4, 62, i, 0, TileBchPrbs.DataCorruption)
#      mgr.addAdcProblem(4, 62, i, 1, TileBchPrbs.DataCorruption)

#=== print bad channels
log.info("bad channels after update")
mgr.listBadAdcs()

#=== commit changes
mgr.commitToDb(db, folder, folderTag, TileBchDecoder.BitPat_onl01, "lfiorini", "writing online bch 2008-07-11", (76480,0))

#=== close DB
db.closeDatabase()


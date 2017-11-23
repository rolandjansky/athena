#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# File:    maskDeadModules.py
# Purpose: Mask a dead module over a range.  A new IOV gets created in the process.
#
# The script takes conditions from reference tag in tileSqlite.db, and
# masks all channels in a (dead) module.  The module to be masked is
# provided by the first 2 arguments: ros (=1-4), mod (0-63).  Next
# four arguments are run,lum for IOV's "since", and run,lum for IOV's
# "until", defining the IOV for the masked module.  Last argument is
# the comment to be added.
#
# 2013-09-11 Guilherme Lima - Adapted code received from Tibor

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *
import os

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
import logging
log = getLogger("writeBch")
log.setLevel(logging.DEBUG)

#=== ADC status folder
folder = "/TILE/OFL02/STATUS/ADC"
tag = "clean"
instance="OFLP200"

#.. Parsing arguments 

import sys
ros=int(sys.argv[1])
mod=int(sys.argv[2])
run=int(sys.argv[3])
lum=int(sys.argv[4])
since=(run,lum)
run1=int(sys.argv[5])
lum1=int(sys.argv[6])
until=(run1,lum1)
comment=sys.argv[7]

#.. very basic input validation
if ros<1 or ros>4:  raise Exception("Invalid ros=%i" % ros)
if mod<0 or mod>63: raise Exception("Invalid module=%i" % mod)
if run1<run:        raise Exception("Invalid validity range: %i < %i" % (run1,run))
log.info("ros=%i mod=%i since=%s until=%s comment=%s" % (ros,mod,since,until,comment) )

#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================
db = TileCalibTools.openDb('SQLITE', 'OFLP200', 'UPDATE')

#=== specify folder and tag
folderTag = TileCalibUtils.getFullTag(folder, tag )

#=== create bad channel manager
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logging.DEBUG)

#=== initialize bch mgr with conditions from DB
log.info("Initializing with conditions from tag=%s and time=%s" % (folderTag, since))
mgr.initialize(db, folder, folderTag, since)

#=== Tuples of empty channels
emptyChannelLongBarrel =     (30, 31, 43)
emptyChannelExtendedBarrel = (18, 19, 24, 25, 26, 27, 28, 29, 33, 34, 42, 43, 44, 45, 46, 47)
#=== Add problems with mgr.addAdcProblem(ros, drawer, channel, adc, problem)

log.info("bad channels before update")
mgr.listBadAdcs()

### Mask channels as NoHV

#.. loop over channels to be updated
#for adc in xrange(0,2):
#  for mod in range(38, 42) + range(54, 58):
#    #.. apply updates
#    mgr.delAdcProblem(3, mod, 4, adc, TileBchPrbs.GeneralMaskChannel)
#    mgr.delAdcProblem(4, mod, 4, adc, TileBchPrbs.GeneralMaskChannel)

for ichan in range(0,48):
  if (ros<3 and ichan not in emptyChannelLongBarrel) or (ros>2 and ichan not in emptyChannelExtendedBarrel):
    log.info("Masking channel %i off" % ichan)
    mgr.addAdcProblem( ros, mod, ichan, 0, TileBchPrbs.NoHV)
    mgr.addAdcProblem( ros, mod, ichan, 1, TileBchPrbs.NoHV)

#=== print bad channels
log.info("bad channels after update")
mgr.listBadAdcs()

#=== commit changes
mgr.commitToDb(db, folder, folderTag, TileBchDecoder.BitPat_ofl01, "Guilherme", comment, since, until)

#=== close DB
db.closeDatabase()

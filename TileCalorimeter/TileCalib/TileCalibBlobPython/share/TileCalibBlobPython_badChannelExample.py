#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_badChannelExample.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-12-19

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileCalibDefaultWriter
from TileCalibBlobPython import TileBchTools
from TileCalibBlobObjs.Classes import * 

#=======================================================
#=== create bad channel manager
#=======================================================
mgr = TileBchTools.TileBchMgr()
import logging
mgr.setLogLvl(logging.INFO)

#============================================================
#=== recreate DB and initialize with default (no bad channels)
#============================================================
folder = "/TILE/OFL02/STATUS/ADC"
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
tag = "TileOfl02StatusAdc-RUN2-UPD4-08"
#=== check if status folder exist, otherwise create
if not db.existsFolder(folder):
    defaultWriter = TileCalibDefaultWriter.TileCalibDefaultWriter(db,tag)
    defaultWriter.writeBadChannels()

#=======================================================
#=== initialize with current db, (MAXRUN,MAXLBK) is default
#=======================================================
mgr.initialize(db, folder, tag, (191714,0))
mgr.listBadAdcs()

#=======================================================
#=== update from file
#=== (can be found in offline/TileCalorimter/TileConditions/share))
#=======================================================
#mgr.updateFromFile("Tile2007.bch")

#=======================================================
#=== add some more problems
#=======================================================
prbs = [TileBchPrbs.LargeHfNoise, TileBchPrbs.NoCis]
mgr.setAdcProblems(4,63,14,1,prbs)
mgr.addAdcProblem( 4,63,14,1,TileBchPrbs.StuckBit )
mgr.addAdcProblem( 4,63,14,1,TileBchPrbs.NoHV     )
mgr.addAdcProblem( 4,63,14,1,TileBchPrbs.NoCis    )

mgr.setAdcProblems(1,0,14,1,prbs)
mgr.addAdcProblem( 1,0,14,1,TileBchPrbs.StuckBit )
mgr.addAdcProblem( 1,0,14,1,TileBchPrbs.NoHV     )
mgr.addAdcProblem( 1,0,14,1,TileBchPrbs.NoCis    )
#=======================================================
#=== remove some problems
#=======================================================
mgr.delAdcProblem( 4,62,13,1,TileBchPrbs.AdcDead )
mgr.delAdcProblem( 4,62,14,1,TileBchPrbs.StuckBit )


#=======================================================
#=== print bad channels
#=======================================================
mgr.listBadAdcs()

#=======================================================
#=== commit changes
#=======================================================
author  = "solodkov"
comment = "testing bad channels"
since= (191715 , 0)
mgr.commitToDb(db, folder, tag, TileBchDecoder.BitPat_ofl01, author, comment, since)
db.closeDatabase()



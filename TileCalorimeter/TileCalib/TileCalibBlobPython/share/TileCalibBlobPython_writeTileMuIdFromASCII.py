#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeTileMuIdFromASCII.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2009-04-29
# change: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 
import os

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeTileMuId")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillTileMuId(file, since,
               until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):
    
    #=== construct folder path
    folder = TileCalibTools.getTilePrefix(False)+"MUID"

    #=== common TileMuId defaults
    default = cppyy.gbl.std.vector('float')()
    for i in xrange(20):
        default.push_back(150.)
        default.push_back(5000.)
    defVec = cppyy.gbl.std.vector('std::vector<float>')()
    defVec.push_back(default)
    defVec.push_back(default)    
    
    #=====================================================
    #=== fill 
    #=====================================================
    writer = TileCalibTools.TileBlobWriter(db,folder,'Flt',False)
    writer.setComment(os.getlogin(),"Arantxa's values, april 2009.")
    parser = TileCalibTools.TileASCIIParser(file,"TileMuId");

    #=== initialize all channels and write global default
    util = cppyy.gbl.TileCalibUtils()
    for ros in xrange(util.max_ros()):
        for drawer in xrange(util.getMaxDrawer(ros)):
            flt = writer.zeroBlob(ros,drawer)
    calibDrawer = writer.getDrawer(0,0)
    calibDrawer.init(defVec,1,1)

    #=== loop over whole detector
    for ros in xrange(1,5):
        for mod in xrange(64):
            #=== need to invalidate previous blob in DB when reading from ASCII file
            writer.zeroBlob(ros,mod)
            #=== we write 40 numbers (thresholds for 20 cells) only to channel 0 in each drawer
            chn = 0
            values = parser.getData(ros,mod,chn)
            if not len(values):
                log.warning("%i/%2i/%2i/x: No value found in file" % (ros,mod,chn))
                continue
            #=== init drawer with defaults for first entry
            calibDrawer = writer.getDrawer(ros,mod)
            if not calibDrawer.getNObjs():
                log.info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                calibDrawer.init(defVec,48,1)
            #=== fill in realistic values (just for the low gain index, store 40 values)
            adc = 0
            line = "%i/%2i/%2i/%i: " % (ros,mod,chn,adc)            
            for i in xrange(40):
                value = float(values[i])
                calibDrawer.setData(chn,adc,i,value)
                line += "%f " % (value,)

            log.debug(line)

    #=== register in DB
    writer.register(since, until, "")

#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')

#=== source data
runfrom = 0
fillTileMuId("Tile.muId", (runfrom,0))

#=== close the database connection
db.closeDatabase()

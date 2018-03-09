#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writePedFromASCII.py
# Nils Gollub <nils.gollub@cern.ch>, 2008-03-03
# modified: Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-06-27
# modified: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-14

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 
import os

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeAutoCr")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillAutoCr(filePed, tag, since,
               until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):
    
    #=== construct folder path
    folder = TileCalibTools.getTilePrefix(True,True)+"NOISE/AUTOCR"

    #=== get full folder tag
    folderTag = TileCalibUtils.getFullTag(folder, tag)

    #=== common noise autocr defaults (no correlation)
    default = cppyy.gbl.std.vector('float')()
    for i in xrange(6):
        default.push_back(0.)
    defVec = cppyy.gbl.std.vector('std::vector<float>')()
    defVec.push_back(default)
    defVec.push_back(default)    
    
    #=====================================================
    #=== fill 
    #=====================================================
    writer = TileCalibTools.TileBlobWriter(db,folder,'Flt')
    writer.setComment(os.getlogin(),"Giulio's file for LBC test n.0, 2009-02-27")
    parser = TileCalibTools.TileASCIIParser(filePed,"AutoCr");

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
            for chn in xrange(48):
                values = parser.getData(ros,mod,chn)
                if not len(values):
                    log.warning("%i/%2i/%2i/x: No value found in file" % (ros,mod,chn))
                    continue
                #=== init drawer with defaults for first entry
                calibDrawer = writer.getDrawer(ros,mod)
                if not calibDrawer.getNObjs():
                    log.info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                    calibDrawer.init(defVec,48,1)
                #=== fill in realistic values
                for adc in xrange(2):
                    line = "%i/%2i/%2i/%i: " % (ros,mod,chn,adc)            
                    for i in xrange(6):
                        value = float(values[adc*6+i])
                        calibDrawer.setData(chn,adc,i,value)
                        line += "%f " % (value,)

                    log.debug(line)
    #=== register in DB
    writer.register(since, until, folderTag)

#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
tag = "TEST-00"

#=== source data
runfrom = 0
fillAutoCr("Tile.autoCr", tag, (runfrom,0))

#=== close the database connection
db.closeDatabase()

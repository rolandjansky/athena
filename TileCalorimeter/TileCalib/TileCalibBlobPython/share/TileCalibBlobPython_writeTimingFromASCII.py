#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeTimingFromASCII.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-26
# change: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 
import os

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeTiming")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillTimingTc(fileTclas, tag, since,
               until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):

    #=== construct folder path
    folderTclas = TileCalibTools.getTilePrefix(True,True)+"TIME/CHANNELOFFSET/LAS"

    #=== create default: one number
    #--- per ADC    for Tclas
    default = cppyy.gbl.std.vector('float')()
    default.push_back(0.)

    #=====================================================
    #=== fill Tclas
    #=====================================================
    tclasWriter = TileCalibTools.TileBlobWriter(db,folderTclas,'Flt')
    tclasWriter.setComment(os.getlogin(),fileTclas)
    parserTclas = TileCalibTools.TileASCIIParser(fileTclas,"Tclas");
    defTclas = cppyy.gbl.std.vector('std::vector<float>')()
    defTclas.push_back(default) # low  gain
    defTclas.push_back(default) # high gain
    #=== initialize defaults
    ros = 0
    for mod in xrange(20):
        flt = tclasWriter.zeroBlob(ros,mod)
    flt = tclasWriter.getDrawer(0, 0)
    flt.init(defTclas,1,0)        
    #=== loop over whole detector
    for ros in xrange(1,5):
        for mod in xrange(64):
            #=== need to invalidate previous blob in DB when reading from ASCII file
            tclasWriter.zeroBlob(ros,mod)
            for chn in xrange(48):
                tclas = parserTclas.getData(ros,mod,chn)
                if not len(tclas):
                    log.warning("%i/%2i/%2i/x: No value found in file" % (ros,mod,chn))
                    continue
                #=== init drawer for first entry
                calibDrawer = tclasWriter.getDrawer(ros,mod)
                if not calibDrawer.getNObjs():
                    log.info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                    calibDrawer.init(defTclas,48,0)
                #=== loop over gainsa
                for adc in xrange(2):
                    val = float(tclas[adc])
                    log.debug("%i/%2i/%2i/%i: tclas = %f" % (ros,mod,chn,adc, val))
                    calibDrawer.setData(chn,adc,0,val)

    folderTag = TileCalibUtils.getFullTag(folderTclas, tag)
    tclasWriter.setComment(os.getlogin(),"Timing update for all partitions")
    tclasWriter.register(since, until, folderTag)


#
#________________________________________________________________________
def fillTimingTd(fileTdlas, tag, since,
               until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):

    #=== construct folder path
    folderTdlas = TileCalibTools.getTilePrefix(True,True)+"TIME/DRAWEROFFSET/LAS"

    #=== create default: one number
    #--- per drawer for Tdlas
    default = cppyy.gbl.std.vector('float')()
    default.push_back(0.)

    #=====================================================
    #=== fill Tdlas
    #=====================================================
    tdlasWriter = TileCalibTools.TileBlobWriter(db,folderTdlas,'Flt')
    tdlasWriter.setComment(os.getlogin(),fileTdlas)
    parserTdlas = TileCalibTools.TileASCIIParser(fileTdlas,"Tdlas");
    defTdlas = cppyy.gbl.std.vector('std::vector<float>')()
    defTdlas.push_back(default)
    #=== initialize defaults
    ros = 0
    for mod in xrange(20):
        flt = tdlasWriter.zeroBlob(ros,mod)
    flt = tdlasWriter.getDrawer(0, 0)
    flt.init(defTdlas,1,0)    
    #=== loop over whole detector
    for ros in xrange(1,5):
        for mod in xrange(64):
            #=== need to invalidate previous blob in DB when reading from ASCII file
            tdlasWriter.zeroBlob(ros,mod)
            tdlas = parserTdlas.getData(ros,mod,0)
            if not len(tdlas):
                log.warning("(ros,mod)=(%i,%2i): No value found in file" % (ros,mod))
                continue
            defTdlas[0][0] = float(tdlas[0])
            log.debug("(ros,mod)=(%i,%2i): tdlas = %f" % (ros,mod,defTdlas[0][0]))
            calibDrawer = tdlasWriter.getDrawer(ros,mod)
            calibDrawer.init(defTdlas,1,0)

    folderTag = TileCalibUtils.getFullTag(folderTdlas, tag)
    tdlasWriter.setComment(os.getlogin(),"Timing update for all partitions")
    tdlasWriter.register(since, until, folderTag)


#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

timingDict = {
#    "/afs/cern.ch/user/n/ngollub/public/timing/residuals-2008-05-07" : (68340,0)
#    "/afs/cern.ch/user/b/bjorn" : (68340,0)
    "./" : (68340,0)
    }


#=== open the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')

tag = "RUN2-HLT-UPD1-00"
for directory in sorted(timingDict.iterkeys()):
    since = timingDict[directory]
#    fileTdlas = directory+"/Tile.tdlas"
    fileTclas = directory+"/Tile.tclas"
    fillTimingTc(fileTclas,tag,since)
#    fillTimingTd(fileTdlas,tag,since)

#=== close the database connection
db.closeDatabase()

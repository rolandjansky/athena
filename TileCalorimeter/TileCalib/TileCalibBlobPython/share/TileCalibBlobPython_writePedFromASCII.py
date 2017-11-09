#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writePedFromASCII.py
# Nils Gollub <nils.gollub@cern.ch>, 2008-03-03
# modified: Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-06-27
# modified: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeNoise")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillPed(filePed, tag, comment, since,
            until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):
    
    #=== construct folder path
    folder = TileCalibTools.getTilePrefix(True,True)+"NOISE/SAMPLE"

    #=== get full folder tag
    folderTag = TileCalibUtils.getFullTag(folder, tag)

    #=== create default (ADC counts)
    pedDef    = [30.,50.]
    loGainDef = 0.8
    hiGainDef = 1.6
    defaultLo = cppyy.gbl.std.vector('float')()
    defaultLo.push_back(pedDef[0]) # pedestal mean value  
    defaultLo.push_back(loGainDef) # pedestal rms
    defaultLo.push_back(0.0)       # pedestal low frequency noise
    defaultLo.push_back(loGainDef) # pedestal HFN1 
    defaultLo.push_back(0.0)       # pedestal HFN2 
    defaultLo.push_back(0.0)       # pedestal HFN2/HFN1 ratio 
    defaultHi = cppyy.gbl.std.vector('float')()
    defaultHi.push_back(pedDef[1]) # pedestal mean value
    defaultHi.push_back(hiGainDef) # pedestal rms
    defaultHi.push_back(0.0)       # pedestal low frequency noise
    defaultHi.push_back(hiGainDef) # pedestal HFN1
    defaultHi.push_back(0.0)       # pedestal HFN2
    defaultHi.push_back(0.0)       # pedestal HFN2/HFN1 ratio
    defVec = cppyy.gbl.std.vector('std::vector<float>')()
    defVec.push_back(defaultLo)
    defVec.push_back(defaultHi)
    
    #=====================================================
    #=== fill 
    #=====================================================
    writer = TileCalibTools.TileBlobWriter(db,folder,'Flt')
    writer.setComment(os.getlogin(),comment)
    parser = TileCalibTools.TileASCIIParser2(filePed,"");
    #=== loop over whole detector
    for ros in xrange(0,5):
        #for mod in xrange(64):
        for mod in xrange(0, min(64,TileCalibUtils.getMaxDrawer(ros))):
            #=== need to invalidate previous blob in DB when reading from ASCII file
            writer.zeroBlob(ros,mod)
            #=== init drawer with defaults for first entry
            calibDrawer = writer.getDrawer(ros,mod)
            if not calibDrawer.getNObjs():
                log.info("Initializing drawer %i/%2i\t%i" % (ros,mod,calibDrawer.getNObjs()))
                calibDrawer.init(defVec,48,0)
            for chn in xrange(48):
                #=== loop over gains
                for adc in xrange(2):
                    calibDrawer.setData(chn,adc,0,pedDef[adc]+(chn+1)/48.)
                    values = parser.getData(ros,mod,chn,adc)
                    if not len(values):
                        log.warning("%i/%2i/%2i/%i: No value found in file" % (ros,mod,chn,adc))
                        values = parser.getData(0,ros*4,chn,adc)
                        if not len(values):
                            log.warning("No default per partition available")
                            values = parser.getData(0,0,chn,adc)
                            if not len(values):
                                log.warning("No global default available - give up")
                                continue
                            else:
                                log.warning("Using global default")
                        else:
                            log.warning("Using default per partition")
                    #=== the order of columns in the ASCII file is different to the order in DB
                    lvl = float(values[0]) # pedestal level
                    hfn = float(values[1]) # high frequency noise
                    lfn = float(values[2]) # low frequency noise
                    hfn1= float(values[3]) # hfn1
                    hfn2= float(values[4]) # hfn2
                    norm= float(values[5]) # hfn2/hfn1
                    log.debug("%i/%2i/%2i/%i: pedLvl=%f\thfn=%f\tlfn=%f\thfn1=%f\thfn2=%f\tnorm=%f" % (ros,mod,chn,adc, lvl,hfn,lfn,hfn1,hfn2,norm))
                    calibDrawer.setData(chn,adc,0,lvl)
                    calibDrawer.setData(chn,adc,1,hfn)
                    calibDrawer.setData(chn,adc,2,lfn)
                    calibDrawer.setData(chn,adc,3,hfn1)
                    calibDrawer.setData(chn,adc,4,hfn2)
                    calibDrawer.setData(chn,adc,5,norm)
    #=== register in DB
    writer.register(since, until, folderTag)



#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'OFLP200', 'UPDATE')
tag = "TwoGauss-19"
file = "TileDefault.ped"
run = 222222
comment = "Intial tag with 2G noise for RUN2, all modules are identical, 2014-07-11"

#=== read ascii file and put it in DB
fillPed(file, tag, comment, (run,0))

#=== close the database connection
db.closeDatabase()

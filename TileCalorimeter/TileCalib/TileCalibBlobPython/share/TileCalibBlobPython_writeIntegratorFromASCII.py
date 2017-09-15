#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeIntegratorFromASCII.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-12-05
# change: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

import cppyy

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import * 
import os

#=== some preparation
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeIntegrator")
import logging
log.setLevel(logging.DEBUG)

#
#________________________________________________________________________
def fillIntegrator(fileInt, tag, since,
                   until=(TileCalibTools.MAXRUN, TileCalibTools.MAXLBK)):
    
    #=== construct folder path
    folder = TileCalibTools.getTilePrefix(True,True)+"INTEGRATOR"

    #=== get full folder tag
    folderTag = TileCalibUtils.getFullTag(folder, tag)

    #=== create default values for each of the six gains 1 to 6
    # for each gain there is:
    # gain, error of the gain, chi2 of the fit, pedestal
    # DAC for pedestal, sigma of pedestal distribution
    # RMS of pedestal distribution, sigma of the RMS

    dv = []
    dv.append((  2.814, 0.023, -1, -1, 80, -1, -1, -1)) 
    dv.append(( 26.010, 0.230, -1, -1, 80, -1, -1, -1)) 
    dv.append(( 28.810, 0.240, -1, -1, 80, -1, -1, -1)) 
    dv.append(( 54.810, 0.480, -1, -1, 70, -1, -1, -1)) 
    dv.append(( 75.790, 0.677, -1, -1, 70, -1, -1, -1)) 
    dv.append((101.800, 0.900, -1, -1, 70, -1, -1, -1))

    #=== number of integrator gains and value per gain
    ngain = 6
    nperg = 8

    defVec = cppyy.gbl.std.vector('std::vector<float>')()

    for i in xrange(ngain):
        defaultGain = cppyy.gbl.std.vector('float')()
        for v in dv[i]:
            defaultGain.push_back(v)
        defVec.push_back(defaultGain)
    
    #=====================================================
    #=== fill 
    #=====================================================
    writer = TileCalibTools.TileBlobWriter(db,folder,'Flt')
    writer.setComment(os.getlogin(),"Jalal's values with non-zero defaults, 2008-12-05")
    parser = TileCalibTools.TileASCIIParser(fileInt,"IntGain");
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

                #=== loop over gains
                for adc in xrange(ngain):
                    line = "%i/%2i/%2i/%i: " % (ros,mod,chn,adc)
                    for v in xrange(nperg):
                        value = float(values[adc*nperg+v])
                        calibDrawer.setData(chn,adc,v,value)
                        line += "%f " % (value,)

                    log.debug(line)

    #=== register in DB
    writer.register(since, until, folderTag)


#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
tag = "RUN2-HLT-UPD1-00"

#=== source data file
fillIntegrator("/afs/cern.ch/user/l/lpribyl/CalibDB/Integrator/Calib_Int.dat", tag, (60000,0))

#=== close the database connection
db.closeDatabase()

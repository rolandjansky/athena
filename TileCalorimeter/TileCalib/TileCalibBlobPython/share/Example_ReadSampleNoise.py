#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# Example_ReadSampleNoise.py
# Nils Gollub <nils.gollub@cern.ch>, 2008-06-16


from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *
import ROOT


#=== specify which time to read
folder      = "/TILE/OFL02/NOISE/SAMPLE"
pointInTime = (999999999,0) # (run number, luminosity block number)
globalTag   = "UPD4"

#=== open the database
db = TileCalibTools.openDb('ORACLE', 'CONDBR2', 'READONLY', 'COOLOFL_TILE')

#=== resolve folder tag from global tag
folderTag = TileCalibTools.getFolderTag(db, folder, globalTag)
#--- or the folderTag can be specified like this:
#folderTag = TileCalibUtils.getFullTag(folder, "COM-00")


#=== get a blob reader
blobReader = TileCalibTools.TileBlobReader(db,folder,folderTag)

#=== write out the comment associated to current values in DB
comment = blobReader.getComment(pointInTime)
print("Comment: \"%s\"" % comment)

#=== Loop over the whole detector
#--- including default drawers with ros = [0,...,19]
for ros in xrange(TileCalibUtils.max_ros()):
    for mod in xrange(TileCalibUtils.getMaxDrawer(ros)):

        #=== get the TileCalibDrawerFlt correcponding to module
        drawer = blobReader.getDrawer(ros,mod,pointInTime)

        #=== loop over all the channels
        for chn in xrange(TileCalibUtils.max_chan()):
            for adc in xrange(TileCalibUtils.max_gain()):

                ped = drawer.getData(chn,adc, 0)
                hfn = drawer.getData(chn,adc, 1)

                #=== currently lfn is not filled, but should be in the future
                lfn = 0.
                if drawer.getObjSizeUint32() > 2:
                    lfn = drawer.getData(chn,adc, 2)

                print("%s, channel=%2i, gain=%i  -> ped=%5.4f\thfn=%5.4f\tlfn=%5.4f"
                      %(TileCalibUtils.getDrawerString(ros,mod),chn,adc,ped,hfn,lfn) )

#=== close the database connection
db.closeDatabase()


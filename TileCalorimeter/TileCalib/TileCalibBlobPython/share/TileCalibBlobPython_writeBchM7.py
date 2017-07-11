#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeBchM7.py
# Lukas Pribyl <lukas.pribyl@cern.ch>, 2008-05-29

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
folder = TileCalibTools.getTilePrefix(True,True)+"STATUS/ADC"

#=== specify folder and tag
folderTag = TileCalibUtils.getFullTag(folder, "COM-04")
    
#=== create bad channel manager
mgr = TileBchTools.TileBchMgr()
mgr.setLogLvl(logging.DEBUG)

#=== always initialize with no bad channels
log.info("Initializing with no bad channels at tag=%s and time=%s" % (folderTag,(0,0)))
mgr.initialize(db, folder, folderTag, (0,0))

#=== Define TileBchStatus.isBad()
#=== Currently all channels with any problem are considered as bad and masked

#=== ADC problems
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.GeneralMaskAdc)
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.AdcDead)                    
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.StuckBit)                   
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.DataCorruption)             
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.VeryLargeHfNoise)           
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoData)                     
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.WrongDspConfig)             
#--- impact on data quality
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.LargeHfNoise)               
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.CorrelatedNoise)            
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.LargeLfNoise)               
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoCis)                      
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.BadCis)                     

#=== Channel problems
#--- serious problems
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.GeneralMaskChannel)         
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoPmt)                      
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoHV)                       
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.WrongHV)                    
#--- impact on data quality
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoLaser)                    
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.BadLaser)                   
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.NoCesium)                   
mgr.addAdcProblem(0, 1, 0, 0, TileBchPrbs.BadCesium)                  

#=== Define TileBchStatus.isNoisy()
# mgr.addAdcProblem(0, 1, 1, 0, TileBchPrbs.LargeHfNoise)               
# mgr.addAdcProblem(0, 1, 1, 0, TileBchPrbs.CorrelatedNoise)
# mgr.addAdcProblem(0, 1, 1, 0, TileBchPrbs.LargeLfNoise)               

#=== Tuples of empty channels
emptyChannelLongBarrel =     (30, 31, 43)
emptyChannelExtendedBarrel = (18, 19, 24, 25, 26, 27, 28, 29, 33, 34, 42, 43, 44, 45, 46, 47)

#=== Add problems with mgr.addAdcProblem(ros, drawer, channel, adc, problem)
# LBA
mgr.addAdcProblem(1, 1, 42, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(1, 1, 47, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(1, 1, 46, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(1, 1, 45, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(1, 6, 32, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 32, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 33, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 33, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 34, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 34, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 35, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 6, 35, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 13, 6, 1, TileBchPrbs.StuckBit)
mgr.addAdcProblem(1, 13, 6, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(1, 18, 23, 0, TileBchPrbs.BadLaser)
mgr.addAdcProblem(1, 18, 23, 1, TileBchPrbs.BadLaser)
mgr.addAdcProblem(1, 22, 32, 0, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(1, 25, 0, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(1, 25, 0, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(1, 31, 20, 0, TileBchPrbs.BadCis)
mgr.addAdcProblem(1, 33, 28, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 34, 47, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 34, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 36, 36, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(1, 41, 39, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 41, 42, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 41, 46, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 41, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 44, 29, 0, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(1, 47, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 48, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(1, 49, 45, 0, TileBchPrbs.VeryLargeHfNoise)
for i in xrange(0, 48):
   if i not in emptyChannelLongBarrel:
      mgr.addAdcProblem(1, 52, i, 0, TileBchPrbs.DataCorruption)
      mgr.addAdcProblem(1, 52, i, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(1, 61, 28, 0, TileBchPrbs.LargeHfNoise)

# LBC
mgr.addAdcProblem(2, 3, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 4, 24, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 5, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 6, 45, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(2, 10, 28, 0, TileBchPrbs.StuckBit)
mgr.addAdcProblem(2, 14, 19, 0, TileBchPrbs.BadCis)
mgr.addAdcProblem(2, 14, 19, 1, TileBchPrbs.BadCis)
mgr.addAdcProblem(2, 14, 45, 1, TileBchPrbs.VeryLargeHfNoise)
for i in xrange(0, 48):
   if i not in emptyChannelLongBarrel:
      mgr.addAdcProblem(2, 16, i, 0, TileBchPrbs.DataCorruption)
      mgr.addAdcProblem(2, 16, i, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 17, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 19, 44, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 44, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 42, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 42, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 47, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 47, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 46, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 46, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 45, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 19, 45, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 23, 42, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 23, 42, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(2, 25, 22, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 25, 23, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 25, 27, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 25, 44, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 26, 47, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 26, 46, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 26, 45, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 33, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 34, 42, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 34, 47, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 34, 46, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 34, 45, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 35, 32, 0, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(2, 36, 45, 1, TileBchPrbs.StuckBit)
mgr.addAdcProblem(2, 37, 47, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(2, 39, 25, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 24, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 29, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 42, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 47, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 46, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 39, 45, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 40, 47, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 40, 45, 1, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(2, 44, 46, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 44, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 45, 3, 0, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(2, 45, 45, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 46, 44, 0, TileBchPrbs.StuckBit)
mgr.addAdcProblem(2, 52, 34, 1, TileBchPrbs.StuckBit)
mgr.addAdcProblem(2, 54, 17, 0, TileBchPrbs.StuckBit)
mgr.addAdcProblem(2, 54, 20, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 58, 45, 1, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(2, 63, 15, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 63, 15, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 63, 16, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 63, 16, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 63, 17, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(2, 63, 17, 1, TileBchPrbs.DataCorruption)

# EBA
mgr.addAdcProblem(3, 6, 31, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 6, 32, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 6, 30, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 8, 32, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 8, 30, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 8, 35, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 11, 31, 1, TileBchPrbs.StuckBit)
mgr.addAdcProblem(3, 13, 31, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(3, 15, 31, 0, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(3, 17, 20, 1, TileBchPrbs.StuckBit)
mgr.addAdcProblem(3, 18, 3, 1, TileBchPrbs.BadCis)
mgr.addAdcProblem(3, 24, 15, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(3, 24, 15, 1, TileBchPrbs.NoData)
mgr.addAdcProblem(3, 32, 36, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(3, 38, 31, 1, TileBchPrbs.LargeLfNoise)
mgr.addAdcProblem(3, 43, 21, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 43, 21, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 43, 22, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 43, 22, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 43, 23, 0, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 43, 23, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(3, 45, 31, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 45, 32, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 45, 30, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(3, 54, 14, 0, TileBchPrbs.NoData)
mgr.addAdcProblem(3, 54, 14, 1, TileBchPrbs.NoData)

# EBC
mgr.addAdcProblem(4, 7, 32, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(4, 8, 0, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 8, 0, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 6, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 6, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 7, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 7, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 8, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 12, 8, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 17, 4, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(4, 21, 35, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 35, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 38, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 38, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 37, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 37, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 41, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 41, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 40, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 40, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 39, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 39, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 36, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 21, 36, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 25, 41, 0, TileBchPrbs.LargeHfNoise)
mgr.addAdcProblem(4, 31, 32, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(4, 31, 30, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(4, 33, 0, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 35, 36, 1, TileBchPrbs.BadCis)
mgr.addAdcProblem(4, 36, 40, 0, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 36, 40, 1, TileBchPrbs.NoCis)
mgr.addAdcProblem(4, 39, 36, 1, TileBchPrbs.BadCis)
for i in xrange(0, 48):
   if i not in emptyChannelExtendedBarrel:
      mgr.addAdcProblem(4, 41, i, 0, TileBchPrbs.DataCorruption)
      mgr.addAdcProblem(4, 41, i, 1, TileBchPrbs.DataCorruption)
mgr.addAdcProblem(4, 45, 1, 0, TileBchPrbs.BadCis)
mgr.addAdcProblem(4, 47, 35, 0, TileBchPrbs.BadCis)
mgr.addAdcProblem(4, 47, 35, 1, TileBchPrbs.BadCis)
mgr.addAdcProblem(4, 54, 36, 1, TileBchPrbs.BadCis)
mgr.addAdcProblem(4, 62, 13, 1, TileBchPrbs.VeryLargeHfNoise)
mgr.addAdcProblem(4, 63, 32, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(4, 63, 30, 0, TileBchPrbs.CorrelatedNoise)
mgr.addAdcProblem(4, 63, 35, 0, TileBchPrbs.CorrelatedNoise)


#=== print bad channels
log.info("bad channels after update")
mgr.listBadAdcs()

#=== commit changes
mgr.commitToDb(db, folder, folderTag, TileBchDecoder.BitPat_ofl01, "lpribyl", "translated from Brian's badChM7.xls", (68340,0))

#=== close DB
db.closeDatabase()


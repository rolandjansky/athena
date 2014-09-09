#!/bin/env python
# TileCalibBlobPython_writeDefaults.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-26

import PyCintex
from PyCool import cool, coral
from TileCalibBlobPython import TileCalibTools, TileCalibDefaultWriter
from TileCalibBlobObjs.Classes import *

#=== open the commissioning database
db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')


#=== create default writer
defaultWriter = TileCalibDefaultWriter.TileCalibDefaultWriter(db)

#=== increase TileCalibTools log level
import logging
defaultWriter.setLogLvl(logging.DEBUG)

#=== tag for commissioning db
tag = "COM-00"

#=== write all sorts of defaults
#=== Attention: defaults for simulation are commented below in some cases
defaultWriter.writeCis(tag)
defaultWriter.writeLas(tag)
defaultWriter.writeLasFiber(tag)
defaultWriter.writeCes(tag, False, "none", 1.2, (75816,0))
defaultWriter.writeEmscale(tag)
defaultWriter.writeIntegrator(tag)
defaultWriter.writeTcfib(tag)
defaultWriter.writeTclas(tag)
defaultWriter.writeTdlas(tag)
defaultWriter.writeTof(tag)
defaultWriter.writeNoiseFit(tag)
defaultWriter.writeNoiseSample(tag)
defaultWriter.writeNoiseAutoCr(tag)
defaultWriter.writeIntegrator(tag)
defaultWriter.writeBadChannels(tag)
defaultWriter.writePulseShape(tag)
defaultWriter.writeMuid()

#=== write noise defaults for simulation db OFLP200 (not COMP200)
#defaultWriter.writeNoiseSampleMC(tag)
#defaultWriter.writeEmscaleMC(tag) # does not in
#defaultWriter.writeOldCes(tag) # MBTS in EBs connected in channel 47. No module type 11 anomaly.

#=== close the database connection
db.closeDatabase()


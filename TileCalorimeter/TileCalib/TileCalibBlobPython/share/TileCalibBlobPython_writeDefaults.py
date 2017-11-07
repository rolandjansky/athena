#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeDefaults.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-26
# change: Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

from TileCalibBlobPython import TileCalibTools, TileCalibDefaultWriter
from TileCalibBlobObjs.Classes import *

#=== open the commissioning database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')

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
defaultWriter.writeCes(tag, False, "13TeV")
defaultWriter.writeEmscale(tag)
defaultWriter.writeIntegrator(tag)
defaultWriter.writeTcfib(tag)
defaultWriter.writeTclas(tag)
defaultWriter.writeTdlas(tag)
defaultWriter.writeTof(tag)
#defaultWriter.writeNoiseFit(tag)
defaultWriter.writeNoiseOnl(tag)
defaultWriter.writeNoiseSample(tag)
defaultWriter.writeNoiseAutoCr(tag)
defaultWriter.writeIntegrator(tag)
defaultWriter.writeBadChannels(tag)
defaultWriter.writePulseShape(tag)
defaultWriter.writeMuonReceiverPulseShape(tag)
defaultWriter.writeMuid()

#=== write noise defaults for simulation db OFLP200 (not CONDBR2)
#defaultWriter.writeNoiseSampleMC(tag)
#defaultWriter.writeEmscaleMC(tag) # does not in
#defaultWriter.writeOldCes(tag) # MBTS in EBs connected in channel 47. No module type 11 anomaly.

#=== close the database connection
db.closeDatabase()


#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeDefaults.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-26
# change Yuri Smirnov <iouri.smirnov@cern.ch>, 2014-12-24

from TileCalibBlobPython import TileCalibTools, TileCalibDefaultWriter
from TileCalibBlobObjs.Classes import *

# choose if we want to write defaults for simulation or for real data
Simulation = True

#=== open database
if Simulation:
    db = TileCalibTools.openDb('SQLITE', 'OFLP200', 'UPDATE')
    #tag = "SIM-02"
    #MBTSflag="900GeV"
    #tag = "SIM-03"
    #MBTSflag="7TeV"
    #tag = "SIM-04"
    #MBTSflag="8TeV"
    tag = "SIM-05"
    #tag = "RUN2-00"
    MBTSflag="13TeV"
    run=222222
else:
    db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
    tag = "RUN2-HLT-UPD1-01"
    MBTSflag="none"
    run=0

#=== create default writer
defaultWriter = TileCalibDefaultWriter.TileCalibDefaultWriter(db)

#=== increase TileCalibTools log level
import logging
defaultWriter.setLogLvl(logging.DEBUG)

#=== write Cesium defaults Simulation=True - for simulation, False - for real data
defaultWriter.writeCes(tag, Simulation, MBTSflag, 1.2, 1.5, (run,0))

#=== close the database connection
db.closeDatabase()


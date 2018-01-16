#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# TileCalibBlobPython_writeDefaults.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-26
# Andrei Artamonov 2011-05-26
# Yuri Smirnov 2014-12-24

from TileCalibBlobPython import TileCalibTools, TileCalibDefaultWriter
from TileCalibBlobObjs.Classes import *

#=== open database
db = TileCalibTools.openDb('SQLITE', 'CONDBR2', 'UPDATE')
tag = ""

#=== create default writer
defaultWriter = TileCalibDefaultWriter.TileCalibDefaultWriter(db)

#=== increase TileCalibTools log level
import logging
defaultWriter.setLogLvl(logging.DEBUG)

#=== write noise defaults
defaultWriter.writeNoiseOnl(tag)

#=== close the database connection
db.closeDatabase()


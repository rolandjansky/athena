#!/bin/env python

from TileCalibBlobPython import TileCalibTools, TileCalibDefaultWriter

#=== open the database
db = TileCalibTools.openDb('SQLITE', 'OFLP200', 'UPDATE', 'COOLOFL_TILE')


#=== create default writer
defaultWriter = TileCalibDefaultWriter.TileCalibDefaultWriter(db)

#=== increase TileCalibTools log level
import logging
defaultWriter.setLogLvl(logging.DEBUG)

#=== tag for commissioning db
tag = "COM-00"

#=== write default pulse shape for TileMuonReceiver board
defaultWriter.writeMuonReceiverPulseShape(tag)

#=== close the database connection
db.closeDatabase()


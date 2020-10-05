#!/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
# TileCalibLogger.py
# Nils Gollub <nils.gollub@cern.ch>, 2007-11-29

import logging
NAMELENGTH = 15

#_____________________________________________________________________
def setupFormat(lvl=logging.INFO):
    myFormat = "%%(name)-%is: %%(levelname)-8s %%(message)s" % NAMELENGTH
    logging.basicConfig(format=myFormat, level=lvl)

#_____________________________________________________________________
def getLogger(name="TileCalib"):
    if len(name)>NAMELENGTH:
        name = "..."+name[-NAMELENGTH+3:]
    return logging.getLogger(name)

#_____________________________________________________________________
class TileCalibLogger:
    """
    Base class, providing logging functionality
    """
    __slots__ = ["_log"]
    def __init__(self, name, lvl=logging.INFO):
        self._log = getLogger(name)
        self.setLogLvl(lvl)
    def setLogLvl(self, lvl):
        self._log.setLevel(lvl)
    def getLogLvl(self, lvl):
        self._log.getEffectiveLevel()
    def log(self):
        return self._log


#=== setup format
setupFormat()

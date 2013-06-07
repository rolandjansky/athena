# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class MDTCalibValConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # list of attributes
    WriterConnectionString     = String("Calibration Db Writer Connection String", "oracle://atlas-oracle-01.roma1.infn.it/atlrome.roma1.infn.it/atlas_muoncalib_rome")
    ReaderConnectionString     = String("Calibration Db Reader Connection String", "oracle://atlas-oracle-01.roma1.infn.it/atlrome.roma1.infn.it/atlas_muoncalib_rome")
    ReaderWorkingSchema        = String("Calibration Db Reader Working Schema", "atlas_muoncalib_rome")

    def __init__(self,name='mdtCalibValConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'ABORT'

class ValAlgorithmArg(trfarg.StringChoicesArg):
    """choose here the calibration algorithm to use"""
    def __init__(self, choices, caseSensitive=False, help='calibration algorithm to be used', name='calibrationAlgorithm'):
        trfarg.StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True

class SelectChamberArg(trfarg.StringArg):
    """chamber selection"""
    def __init__(self, help='select the chamber to be used', name='selectChamber'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class CalibSiteArg(trfarg.StringArg):
    """calibration site name"""
    def __init__(self, help='select the calibration site name', name='calibSite'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class HeaderIdArg(trfarg.IntegerArg):
    """DB header to validate"""
    def __init__(self, help='select the calibration ID to validate', name='headId'):
        trfarg.IntegerArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

# make configuration object
mdtCalibValConfig = MDTCalibValConfig()

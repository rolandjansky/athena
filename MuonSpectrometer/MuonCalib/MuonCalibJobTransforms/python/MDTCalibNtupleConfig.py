# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class MDTCalibNtupleConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # list of attributes
    postOptions  = String("jobOptions fragment to be appended at the end of the skeleton", None)
    #doMScombined = Boolean("Use MScombined", False)
    #doMDT        = Boolean("Process MDT hits", True)
    #doRPC        = Boolean("Process RPC hits", True)
    #doMoMu       = Boolean("Use NIKHEF cosmic pattern", True)
    #doMoore      = Boolean("Use Moore reconstruction", False)
    ##doMuonBoy    = Boolean("Use Muonboy reconstruction", False)

    def __init__(self,name='mdtCalibNtupleConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'ABORT'

class DbT0TagArg(trfarg.StringArg):
    """MDT T0 DB folder tag to be used, overwriting the one
    from the global tag, set to 'DEFAULT' to keep global tag"""
    def __init__(self, help='DB tag for T0', name='dbT0Tag'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return 'DEFAULT'

class DbRTTagArg(trfarg.StringArg):
    """MDT RT DB folder tag to be used, overwriting the one
    from the global tag, set to 'DEFAULT' to keep global tag"""
    def __init__(self, help='DB tag for RT', name='dbRTTag'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return 'DEFAULT'

class DbT0SqlArg(trfarg.StringArg):
    """SQLite file containing the MDT T0 folder to be used, overwriting default
    COOL DB for that folder, set to 'DEFAULT' to keep default"""
    def __init__(self, help='SQLite file for T0', name='dbT0Sql'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return 'DEFAULT'

class DbRTSqlArg(trfarg.StringArg):
    """SQLite file containing the MDT RT folder to be used, overwriting default
    COOL DB for that folder, set to 'DEFAULT' to keep default"""
    def __init__(self, help='SQLite file for RT', name='dbRTSql'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return 'DEFAULT'
 
class UseAlignArg(trfarg.BoolArg):
    """run segment reconstruction and not tracks"""
    def __init__(self, help='run only segment reconstruction', name='useAlign'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class SegOnlyArg(trfarg.BoolArg):
    """run segment reconstruction and not tracks"""
    def __init__(self, help='run only segment reconstruction', name='segOnly'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class MooreArg(trfarg.BoolArg):
    """select Moore for the reconstruction run before filling the calibration ntuple"""
    def __init__(self, help='selected muon reconstruction', name='moore'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class MboyArg(trfarg.BoolArg):
    """select Mboy for the reconstruction run before filling the calibration ntuple"""
    def __init__(self, help='selected muon reocnstruction', name='mboy'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class StandaloneArg(trfarg.BoolArg):
    """select 3rd chain for the reconstruction run before filling the calibration ntuple"""
    def __init__(self, help='selected muon reocnstruction', name='standalone'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class DoSegmentT0FitArg(trfarg.BoolArg):
    """ use the T0 refinement """
    def __init__(self, help='selected muon reocnstruction', name='doSegmentT0Fit'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class CalibNtupleDoSimArg(trfarg.BoolArg):
    """process simulated data for the calibration ntuple"""
    def __init__(self, help='process simulated data', name='doSim'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class DoCSCArg(trfarg.BoolArg):
    """process CSC data"""
    def __init__(self, help='process CSC data', name='doCSC'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class RawTgcArg(trfarg.BoolArg):
    """put rawTgc and rawTgcCoin branches in calib ntuple"""
    def __init__(self, help='put rawTgc and rawTgcCoin branches in calib ntuple', name='rawTgc'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class LumiBlockNumberFromCool(trfarg.BoolArg):
   """get lumi block number from coditions database according to time-stamp """
   def __init__(self, help='get lumi block number from coditions database according to time-stamp', name='lumiBlockNumberFromCool'):
        trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
        return True
   
class RunNumberFromCool(trfarg.BoolArg):
   """get run number from coditions database according to time-stamp """
   def __init__(self, help='get run number from coditions database according to time-stamp', name='runNumberFromCool'):
        trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
        return True

class CosmicRun(trfarg.BoolArg):
   """Set to true if the run is a cosmic run. TOF will be set to 0 and time slewing correction will be applied"""
   def __init__(self, help='Set to true if the run is a cosmic run. TOF will be set to 0 and time slewing correction will be applied', name='cosmicRun'):
       trfarg.BoolArg.__init__(self, help, name)
   def isFullArgument(self):
        return True
      

# make configuration object
mdtCalibNtupleConfig = MDTCalibNtupleConfig()

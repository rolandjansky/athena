# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
__all__ = []

from PyJobTransformsCore.TransformConfig import *
import PyJobTransformsCore.basic_trfarg as trfarg

# define configuration properties
class RPCCalibNtupleConfig(TransformConfig):
    # prevent any mistypings by not allowing dynamic members
    __slots__ = ()

    # list of attributes
    postOptions              = String("jobOptions fragment to be appended at the end of the skeleton", None)
    doExtrapolNtuple         = Boolean("Dump extrapolations to ntuple", True)
    clusterTimeSpread        = Float("Cluster time spread", 15)
    fieldMask                = String("defines which parts of the field are on","111")
    conditionsTag            = String("conditions tag","COMCOND-REPC-001-03")
   
    def __init__(self,name='rpcCalibNtupleConfig',metaData=None):
        TransformConfig.__init__(self,name,metaData)
        self.maxeventsstrategy = 'ABORT'

# make configuration object
rpcCalibNtupleConfig = RPCCalibNtupleConfig()

class ConditionsTagArg(trfarg.StringArg):
    """choose here the IOVDb global tag, i.e. the misalignemnts db"""
    def __init__(self, help='IOVDb global tag', name='conditionsTag'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class FieldMaskArg(trfarg.StringArg):
    """choose which fields must be turned on"""
    def __init__(self, help='field mask', name='fieldMask'):
        trfarg.StringArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class trigNtupleArg(trfarg.BoolArg):
    """turns on trig ntuple"""
    def __init__(self, help='trigger ntuple', name='trigNtuple'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True

class resNtupleArg(trfarg.BoolArg):
    """turns on rpc res ntuple"""
    def __init__(self, help='residuals ntuple', name='resNtuple'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True
    
class extrNtupleArg(trfarg.BoolArg):
    """turns on rpc extr ntuple"""
    def __init__(self, help='extrapolations ntuple', name='extrNtuple'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True


class PRDNtupleArg(trfarg.BoolArg):
    """turns on trig ntuple"""
    def __init__(self, help='rpc prd ntuple', name='prdNtuple'):
        trfarg.BoolArg.__init__(self, help, name)
    def isFullArgument(self):
        return True


class recoAlgArg(trfarg.StringChoicesArg):
    def __init__(self, choices, caseSensitive=False, help='reco algorithm to be used', name='recoAlg'):
        trfarg.StringChoicesArg.__init__(self, choices, help, name, caseSensitive)
    def isFullArgument(self):
        return True

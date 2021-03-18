# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Muon slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = [] 
class doTrigMuonConfig(JobProperty):
    """ configure for trigger if true, offline if false """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doTrigMuonConfig)

class doMuonCalibrationStream(JobProperty):
    """ do or not to do the setup for the muon calibration stream"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False
_flags.append(doMuonCalibrationStream)

class doEFRoIDrivenAccess(JobProperty):
    """ do or not do the EF Roi driven access"""
    statusOn = True
    allowedType = ['bool']
    StoredValue = False

_flags.append(doEFRoIDrivenAccess)

class signatures(JobProperty):
    """ signatures in Muon slice """
    statusOn=True
    allowedTypes = ['list']
    StoredValue  = []

_flags.append(signatures)


# create container

class MuonSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Muon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MuonSlice)

for flag in _flags:
    TriggerFlags.MuonSlice.add_JobProperty(flag)

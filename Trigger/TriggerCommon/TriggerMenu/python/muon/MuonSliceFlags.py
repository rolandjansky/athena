# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Muon slice specific flags  """

from AthenaCommon.JobProperties         import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'T. Bold, B.Demirkoz'
__version__="$Revision: 1.46 $"
__doc__="Muon slice specific flags  "


_flags = [] 
class doSiTrack(JobProperty):
    """ do or not to do SiTrack algo """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

_flags.append(doSiTrack)

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
    #allowedValues = AllowedList(["mu4T"])

_flags.append(signatures)


# create container

class MuonSlice(JobPropertyContainer, CommonSliceHelper):
    """ Muon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MuonSlice)

# add add common slice flags
TriggerFlags.MuonSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.MuonSlice.add_JobProperty(flag)
del _flags

# make an alias
MuonSliceFlags = TriggerFlags.MuonSlice

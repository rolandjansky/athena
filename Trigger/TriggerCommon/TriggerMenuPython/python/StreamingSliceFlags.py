# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Streaming slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'P. Bell, after A. Cerri'
__version__="$Revision: 0.1 $"
__doc__="Streaming slice specific flags  "


_flags = []

class SliceList(JobProperty):
    """ List of chains provided by the Streaming slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[
        "SingleBeamRNDM",       
        "SingleBeamBPTX",       
        "SingleBeamL1Calo",
        "SingleBeamL1CaloEMFilter_V2",
        "SingleBeamMinBias",
        "SingleBeamCosmicMuons",
        "SingleBeamMuonswBeam_V2",
        "SingleBeamMinBiasFilter",
        "SeededStreamerRNDM",       
        "SeededStreamerBPTX",       
        "SeededStreamerL1Calo",
        "SeededStreamerL1CaloEM",
        "SeededStreamerEMtoL1Calo",
        "SeededStreamerL1CaloTau",
        "SeededStreamerL1CaloJet",
        "SeededStreamerL1CaloMET",
        "SeededStreamerL1CaloXS",
        "SeededStreamerL1CaloEMFilter_V2",
        "SeededStreamerMinBias",
        "SeededStreamerZeroBias",
        "SeededStreamerMBTS",
        "SeededStreamerMBTS_ZDC",
        "SeededStreamerBCM_LUCID_ZDC",
        "SeededStreamerBCM",
        "SeededStreamerLUCID",
        "SeededStreamerCosmicMuons",
        "SeededStreamerCosmicCalo",
        "SeededStreamerCosmicCaloEM",
        "SeededStreamerCosmicMinBias",
        "SeededStreamerMuonswBeam",
        "SeededStreamerWarmStart",
        "L1Calo_NoAlg",
        "L1Muon_NoAlg",
        "L1MinBias_NoAlg",
        "Standby",
        "Background",
        'Background_RD0_UNPAIRED_ISO',
        'Background_RD0_UNPAIRED_NONISO',
        'Background_RD0_EMPTY',
        "SeededStreamerExpressPhysicsHI",
	"SeededStreamerExpressStandbyHI",
        ]
    allowedValues=AllowedList(StoredValue)
_flags.append(SliceList)

class signatures(JobProperty):
    """ signatures in Streaming slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= SliceList.StoredValue
    allowedValues = AllowedList(StoredValue)
_flags.append(signatures)

# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class StreamingSlice(JobPropertyContainer, CommonSliceHelper):
    """ Streaming Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(StreamingSlice)

# add add common slice flags
TriggerFlags.StreamingSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.StreamingSlice.add_JobProperty(flag)
del _flags

# make an alias
StreamingSliceFlags = TriggerFlags.StreamingSlice


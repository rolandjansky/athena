# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" BeamSpot slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'David W. Miller'
__version__="$Revision: 1.6 $"
__doc__="BeamSpot slice specific flags  "


_flags = []
class signatures(JobProperty):
    """ signatures in BeamSpot slice """
    statusOn=True
    allowedTypes  = ['list']
    allowedValues = AllowedList( [
        "beamspot_vertex_list",
        "beamspot_vertex_jet10", 
        "beamspot_vertex_bjet18",
        "beamspot_vertex_FSTracks_IDScan",
        "beamspot_vertex_activeTE_IDScan_peb",
        "beamspot_vertex_allTE_IDScan_peb",
        "beamspot_vertex_FSTracks_SiTrk",
        "beamspot_vertex_activeTE_SiTrk_peb",
        "beamspot_vertex_allTE_SiTrk_peb",
        ## For reverting back to an older trigger
        "beamspot_vertex_FSTracks_SiTrk_backup",
        "beamspot_vertex_activeTE_SiTrk_peb_backup",
        "beamspot_vertex_allTE_SiTrk_peb_backup",
        ##For low-lumi running
        "beamspot_vertex_FSTracks_IDScan_MBTS",
        "beamspot_vertex_activeTE_IDScan_peb_MBTS",
        "beamspot_vertex_allTE_IDScan_peb_MBTS",
        "beamspot_vertex_FSTracks_SiTrk_MBTS",
        "beamspot_vertex_activeTE_SiTrk_peb_MBTS",
        "beamspot_vertex_allTE_SiTrk_peb_MBTS",
        ## For L2Star chains
        "beamspot_vertex_FSTracks_L2Star_A",
        "beamspot_vertex_activeTE_L2Star_A_peb",
        "beamspot_vertex_allTE_L2Star_A_peb",
        "beamspot_vertex_FSTracks_L2Star_B",
        "beamspot_vertex_activeTE_L2Star_B_peb",
        "beamspot_vertex_allTE_L2Star_B_peb",        
        'ftk_vertex_FSTracks_L2Star_F' ,
        ##For FTK
        "beamspot_vertex_FSTracks_L2Star_F",
        ] )
    StoredValue   = [
        "beamspot_vertex_list",
        "beamspot_vertex_jet10", 
        "beamspot_vertex_bjet18",
        "beamspot_vertex_FSTracks_IDScan",
        "beamspot_vertex_activeTE_IDScan_peb",
        "beamspot_vertex_allTE_IDScan_peb",
        "beamspot_vertex_FSTracks_SiTrk",
        "beamspot_vertex_activeTE_SiTrk_peb",
        "beamspot_vertex_allTE_SiTrk_peb",
        ##For low-lumi running
        "beamspot_vertex_FSTracks_IDScan_MBTS",
        "beamspot_vertex_activeTE_IDScan_peb_MBTS",
        "beamspot_vertex_allTE_IDScan_peb_MBTS",
        "beamspot_vertex_FSTracks_SiTrk_MBTS",
        "beamspot_vertex_activeTE_SiTrk_peb_MBTS",
        "beamspot_vertex_allTE_SiTrk_peb_MBTS",
        "beamspot_vertex_FSTracks_L2Star_A",
        "beamspot_vertex_activeTE_L2Star_A_peb",
        "beamspot_vertex_allTE_L2Star_A_peb",
        "beamspot_vertex_FSTracks_L2Star_B",
        "beamspot_vertex_activeTE_L2Star_B_peb",
        "beamspot_vertex_allTE_L2Star_B_peb",
        ]

_flags.append(signatures)

# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class BeamSpotSlice(JobPropertyContainer, CommonSliceHelper):
    """ BeamSpot Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BeamSpotSlice)

# add add common slice flags
TriggerFlags.BeamSpotSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BeamSpotSlice.add_JobProperty(flag)
del _flags

# make an alias
BeamSpotSliceFlags = TriggerFlags.BeamSpotSlice

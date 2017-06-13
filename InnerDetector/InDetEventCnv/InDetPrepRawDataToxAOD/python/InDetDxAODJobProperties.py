# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file InDetPrepRawDataToxAOD/python/InDetDxAODJobProperties.py
## @purpose Python module to hold common flags to configure xAOD-making jobOptions
##

""" InDetDxAODJobProperties
    Python module to hold common flags to configure ID xAOD-making jobOptions.

"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class InDetDxAODJobPropertyContainer(JobPropertyContainer):
    """ The jobProperty container for InDetDxAOD-making flags """
jobproperties.add_Container(InDetDxAODJobPropertyContainer)


class DumpPixelInfo(JobProperty):
    """write out pixel info """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpPixelInfo)

class DumpSctInfo(JobProperty):
    """write out SCT info """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpSctInfo)

class DumpTrtInfo(JobProperty):
    """write out TRT info """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpTrtInfo)

class ThinHitsOnTrack(JobProperty):
    """thin hits to store only those on-track """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(ThinHitsOnTrack)

class ThinTrackSelection(JobProperty):
    """thinning selection for tracks """
    statusOn = True
    allowedTypes = ["string"]
    StoredValue = "InDetTrackParticles.pt > 0.1*GeV"
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(ThinTrackSelection)

class DumpByteStreamErrors(JobProperty):
    """BS errors, for those systems that have implemented it """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpByteStreamErrors)

class DumpUnassociatedHits(JobProperty):
    """dump hits not-on-track """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpUnassociatedHits)

class DumpLArCollisionTime(JobProperty):
    """dump LAr collision time """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpLArCollisionTime)

class DumpTruthInfo(JobProperty):
    """dump truth in fo """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpTruthInfo)

class DumpTriggerInfo(JobProperty):
    """dump trigger info """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpTriggerInfo)

class PrintIdTrkDxAODConf(JobProperty):
    """ print configuration """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(PrintIdTrkDxAODConf)

class MakeSplitCosmicTracks(JobProperty):
    """ create split tracks if running on cosmics """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(MakeSplitCosmicTracks)

class DumpSctRdoInfo(JobProperty):
    """dump RDO info for SCT (only works when running from RAW, or ESD that was made with InDetFlags.writeRDOs=True """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpSctRdoInfo)

class DumpPixelRdoInfo(JobProperty):
    """dump RDO info for Pixel (only works when running from RAW, or ESD that was made with InDetFlags.writeRDOs=True """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpPixelRdoInfo)

class DumpPixelNNInfo(JobProperty):
    """dump NN info for Pixel  """
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(DumpPixelNNInfo)

class TrtZSelection(JobProperty):
    """Z event selection based on TRT tag and probe"""
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(TrtZSelection)

class TrtJpsiSelection(JobProperty):
    """J/psi event selection based on TRT tag and probe"""
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
jobproperties.InDetDxAODJobPropertyContainer.add_JobProperty(TrtJpsiSelection)


InDetDxAODFlags = jobproperties.InDetDxAODJobPropertyContainer

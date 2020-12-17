# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        BeamSpotFlags
##
## Author:      James Walder
## Created:     March 2010
##
## Description: Here, all neccessary job flags for the BeamSpot DPD
##              are defined.
##
##=============================================================================

__doc__ = """Here, all neccessary job flags for the BeamSpot reconstruction are defined."""
__version__ = "0.0.1"
__author__  = "James Walder <jwalder@cern.ch>"
__version__ = "$Id $"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

_beamspotFlags = []

class BeamSpotJobProperty(JobProperty):
    """ This class stores if a user changed a property once in the variable setByUser
    Taken from InnerDetector/InDetExample/InDetRecExample/python/InDetJobProperties.py#076
    """
    setByUser = False
    
    def _do_action (self):
        self.setByUser = True
        
    def _undo_action (self):
        self.setByUser = True
            
    def get_Value(self):
        if (self.allowedTypes[0] == 'bool'):
            return self.statusOn and self.StoredValue
        else:
            return self.StoredValue



### Define the properties
class turnOnIDOnly(BeamSpotJobProperty):
    """ Do only ID  and switch off the other sub-detectors """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
_beamspotFlags.append(turnOnIDOnly)

class doIDReco(BeamSpotJobProperty):
    """ Activate the ID reconstrcution algorithms """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    
_beamspotFlags.append(doIDReco)


class setIndetAlignTag(BeamSpotJobProperty):
    """ Set and override the Indet alignment tag """
    statusOn    = False
    allowedTypes = ['str']
    StoredValue  = "NONE"
_beamspotFlags.append(setIndetAlignTag)

class setTRTAlignTag(BeamSpotJobProperty):
    """ Set and override the TRT alignment tag """
    statusOn    = False
    allowedTypes = ['str']
    StoredValue  = "NONE"
_beamspotFlags.append(setTRTAlignTag)

class setTrkErrorScalingTag(BeamSpotJobProperty):
    """ Set and override the TrkErrorScaling tag """
    statusOn    = False
    allowedTypes = ['str']
    StoredValue  = "NONE"
_beamspotFlags.append(setTrkErrorScalingTag)

class setBeamposTag(BeamSpotJobProperty):
    """ Set and override the TRT alignment tag """
    statusOn    = False
    allowedTypes = ['str']
    StoredValue  = "NONE"
_beamspotFlags.append(setBeamposTag)


class BeamSpotJobProperties(JobPropertyContainer):
    """ Container for the BeamSpot processing jobProperties """

jobproperties.add_Container(BeamSpotJobProperties)

for flag in _beamspotFlags:
    jobproperties.BeamSpotJobProperties.add_JobProperty(flag)
del _beamspotFlags

BeamSpotFlags = jobproperties.BeamSpotJobProperties

    






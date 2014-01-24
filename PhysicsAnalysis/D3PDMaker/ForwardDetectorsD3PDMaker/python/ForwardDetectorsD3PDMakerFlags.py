# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: ForwardDetectorsD3PDMakerFlags.py
# @purpose: a container of flags for Forward Detectors D3PD maker variables
# @author: Marco Leite <leite@cern.ch>
# Based on Heavy Ions MakerFlags from Andrzej Olszewski

"""
     A container of flags for Forward Detectors D3PD maker

"""
#
#
__author__  = 'Marco Leite'
__version__ = "$Revision: 470583 $"
__doc__     = "A container of flags for Forward Detectors D3PD maker"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FileName(JobProperty):
    """
        Filename of ForwardDetectors D3PD file
    """
    statusOn=False
    allowedTypes=['str']
    StoredValue="ForwardDetectorsD3PD.root"

class Content(JobProperty):
    """
        Switch to change content
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "full"

class reReco(JobProperty):
    """
        Run D3PD maker with rereconstruction (on ESD)
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoSkimming(JobProperty):
    """
        Switch to turn on/off skimming
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoTrigger(JobProperty):
    """
        Add trigger components
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoTruth(JobProperty):
    """
        Add truth components
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoHijingPars(JobProperty):
    """
        Add Hijing event params
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

# Defines the container for heavy ion d3pd maker flags
class ForwardDetectorsMakerFlags(JobPropertyContainer):
    """
        The Forward Detectors maker flag/job property container.
    """
    pass

# add the flags container to the top container
jobproperties.add_Container(ForwardDetectorsD3PDMakerFlags)


# We want always the following flags in the container
list_jobproperties = [
    FileName,
    Content,
    reReco,
    DoSkimming,
    DoTrigger,
    DoTruth,
    DoHijingPars
    ]

for i in list_jobproperties:
    jobproperties.ForwardDetectorsMakerFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties

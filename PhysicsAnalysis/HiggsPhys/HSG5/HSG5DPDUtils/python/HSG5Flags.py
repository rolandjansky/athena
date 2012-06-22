# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
## @file HSG5D3PDUtils/python/HSG5Flags.py
## @purpose Python module to hold common flags to configure HSG5D3PD JobOptions
##

##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

# General flags
_flags = []

class doHadronicRecoil(JobProperty):
    """ Turn on filling of hadronic recoil branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doHadronicRecoil)

class doGoodTracks(JobProperty):
    """ Turn on filling of hadronic recoil branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doGoodTracks)

class doHighPtTracks(JobProperty):
    """ Turn on filling of HighPtTracks branches
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doHighPtTracks)

class doHighPtClusters(JobProperty):
    """ Turn on filling of HighPtClusters branches
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doHighPtClusters)

class doHighPtEMClusters(JobProperty):
    """ Turn on filling of HighPtEMClusters branches
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doHighPtEMClusters)

class doTaus(JobProperty):
    """ Turn on filling of tau branches
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doTaus)

class doPhotons(JobProperty):
    """ Turn on filling of photon branches
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
_flags.append(doPhotons)

class doAllMissingET(JobProperty):
    """ Turn on filling of all MissingET branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
_flags.append(doAllMissingET)

# V0D3PDObject flags


##-----------------------------------------------------------------------------
## Definition of the HSG5 flag container

class HSG5Flags(JobPropertyContainer):
    """ The HSG5 base D3PD flag container
    """

class HSG5WHFlags(JobPropertyContainer):
    """ The HSG5WH D3PD flag container
    """

class HSG5WHUFlags(JobPropertyContainer):
    """ The HSG5WHU D3PD flag container
    """

class HSG5WHQFlags(JobPropertyContainer):
    """ The HSG5WHU D3PD flag container
    """

class HSG5ZHLLFlags(JobPropertyContainer):
    """ The HSG5ZHLL D3PD flag container
    """

class HSG5ZHLLUFlags(JobPropertyContainer):
    """ The HSG5ZHLLU D3PD flag container
    """
        
class HSG5ZHMETFlags(JobPropertyContainer):
    """ The HSG5ZHMET D3PD flag container
    """

class HSG5ZHMETUFlags(JobPropertyContainer):
    """ The HSG5ZHMETU D3PD flag container
    """

class HSG5GAMZFlags(JobPropertyContainer):
    """ The HSG5GAMZ D3PD flag container
    """

class HSG5ZBBFlags(JobPropertyContainer):
    """ The HSG5ZBB D3PD flag container
    """

# add the reconstruction flags container to the top containers
jobproperties.add_Container(HSG5Flags)
jobproperties.add_Container(HSG5WHFlags)
jobproperties.add_Container(HSG5WHUFlags)
jobproperties.add_Container(HSG5WHQFlags)
jobproperties.add_Container(HSG5ZHLLFlags)
jobproperties.add_Container(HSG5ZHLLUFlags)
jobproperties.add_Container(HSG5ZHMETFlags)
jobproperties.add_Container(HSG5ZHMETUFlags)
jobproperties.add_Container(HSG5GAMZFlags)
jobproperties.add_Container(HSG5ZBBFlags)

for flag in _flags:
    jobproperties.HSG5Flags.add_JobProperty(flag)
    jobproperties.HSG5WHFlags.add_JobProperty(flag)
    jobproperties.HSG5WHUFlags.add_JobProperty(flag)
    jobproperties.HSG5WHQFlags.add_JobProperty(flag)
    jobproperties.HSG5ZHLLFlags.add_JobProperty(flag)
    jobproperties.HSG5ZHLLUFlags.add_JobProperty(flag)
    jobproperties.HSG5ZHMETFlags.add_JobProperty(flag)
    jobproperties.HSG5ZHMETUFlags.add_JobProperty(flag)
    jobproperties.HSG5GAMZFlags.add_JobProperty(flag)
    jobproperties.HSG5ZBBFlags.add_JobProperty(flag)
del _flags

HSG5Flags=jobproperties.HSG5Flags
HSG5WHFlags=jobproperties.HSG5WHFlags
HSG5WHUFlags=jobproperties.HSG5WHUFlags
HSG5WHQFlags=jobproperties.HSG5WHQFlags
HSG5ZHLLFlags=jobproperties.HSG5ZHLLFlags
HSG5ZHLLUFlags=jobproperties.HSG5ZHLLUFlags
HSG5ZHMETFlags=jobproperties.HSG5ZHMETFlags
HSG5ZHMETUFlags=jobproperties.HSG5ZHMETUFlags
HSG5GAMZFlags=jobproperties.HSG5GAMZFlags
HSG5ZBBFlags=jobproperties.HSG5ZBBFlags



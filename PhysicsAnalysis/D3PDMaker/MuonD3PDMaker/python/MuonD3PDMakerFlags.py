# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
## @file MuonD3PDMaker/python/MuonD3PDMakerFlags.py
## @purpose Python module to hold common flags to configure MuonD3PDMaker JobOptions
##



##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

# General flags

class doSingleMuons(JobProperty):
     """ Turn on when using single muon samples
     """
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False

class doNewChainOnly(JobProperty):
     """ Turn on when Muid and Staco chains are de-activated
     """
     statusOn=True
     allowedTypes=['bool']
     StoredValue=True

class doSegmentTruth(JobProperty):
     """ Turn on to dump segment truth information
     """
     statusOn=True
     allowedTypes=['bool']
     StoredValue=False

     


##-----------------------------------------------------------------------------
## Definition of the MuonD3PDMaker flag container

class MuonD3PDMakerFlags(JobPropertyContainer):
    """ The Muon D3PD flag container
    """

        
# add the reconstruction flags container to the top container
jobproperties.add_Container(MuonD3PDMakerFlags)

jobproperties.MuonD3PDMakerFlags.add_JobProperty(doSingleMuons)
jobproperties.MuonD3PDMakerFlags.add_JobProperty(doNewChainOnly)
jobproperties.MuonD3PDMakerFlags.add_JobProperty(doSegmentTruth)

MuonD3PDFlags=jobproperties.MuonD3PDMakerFlags

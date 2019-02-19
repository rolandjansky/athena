# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ISF_ParSimTuning.py
## @brief Python module to hold common flags to configure JobOptions
##

__author__ = "Miha Muskinja"
__version__= "$Revision: 698492 $"
__doc__    = "ParSimJobProperties"

__all__    = [ "ISF_ParSimTuning" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step
## Define the flag

class MuonSmearerMode(JobProperty):
    """Mode of the muon smearer
       1: ID Only
       2: MS Only
       3: Combined
       """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class MC12MuonSmearerPtBinsFileName(JobProperty):
    """name of the MC12 Muon Smearer pt bins file
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = "MC12MuonParametrisationPtBins.txt"

class MC12MuonSmearerEtaBinsFileName(JobProperty):
    """name of the MC12 Muon Smearer eta bins file
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = "MC12MuonParametrisationEtaBins.txt"

class MC12MuonSmearerParametrisationFileNames(JobProperty):
    """names of the MC12 Muon Smearer parametrisation files
    """
    statusOn     = True
    allowedTypes = ['map<int, string>']
    StoredValue  = { 1 : "MC12MuonParametrisationID.root",
                     2 : "MC12MuonParametrisationMS.root",
                     3 : "MC12MuonParametrisationCOMB.root"} 

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class ISF_ParSimTuningJobProperties(JobPropertyContainer):
    """Container for the ISF_ParSim key flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(ISF_ParSimTuningJobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding the flags to the  container
jobproperties.ISF_ParSimTuningJobProperties.add_JobProperty(MuonSmearerMode)
jobproperties.ISF_ParSimTuningJobProperties.add_JobProperty(MC12MuonSmearerPtBinsFileName)
jobproperties.ISF_ParSimTuningJobProperties.add_JobProperty(MC12MuonSmearerEtaBinsFileName)
jobproperties.ISF_ParSimTuningJobProperties.add_JobProperty(MC12MuonSmearerParametrisationFileNames)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select ParSimJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_ParSimServices.ISF_ParSimJobProperties import ISF_ParSimFlags
ISF_ParSimTuningFlags = jobproperties.ISF_ParSimTuningJobProperties

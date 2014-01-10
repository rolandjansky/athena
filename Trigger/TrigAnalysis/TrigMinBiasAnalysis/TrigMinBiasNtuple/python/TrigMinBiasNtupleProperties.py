# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file Trigger/TrigAnalysis/TrigMinBiasAnalysis/TrigMinBiasNtuple/python/TrigMinBiasNtupleProperties.py
## @purpose Python module to hold Ntuple properties
##
## Adapted from AthenaCommonFlags.py
##
##

__author__ = "W. H. Bell"
__version__= "$Revision: 1.1 $"
__doc__    = "TrigMinBiasNtupleProperties"
 
__all__    = [ "trigMinBiasNtupleProperties" ]


##-----------------------------------------------------------------------------
## Import
 
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class ntupleName(JobProperty):
    statusOn = True
    allowedTypes = ['string']
    StoredValue = "TrigMinBias.root"

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the TrigMinBiasNtupleProperties container
class TrigMinBiasNtupleProperties(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(TrigMinBiasNtupleProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the TrigMinBiasNtupleProperties container
jobproperties.TrigMinBiasNtupleProperties.add_JobProperty(ntupleName)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select AthenaCommonFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from TrigMinBias.TrigMinBiasNtupleProperties import trigMinBiasNtupleProperties
trigMinBiasNtupleProperties = jobproperties.TrigMinBiasNtupleProperties

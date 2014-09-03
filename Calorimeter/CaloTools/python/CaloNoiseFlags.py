# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Flags for define calo noise tool configuration
"""
#
#
__author__  = 'G.Unal'


#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FixedLuminosity(JobProperty):
   """ Flag to force luminosity (10**33 units) to a given value corresponding to a given mu 
        Lumi is related to mu by   Lumi = (mu/2.3) * 25ns/BunchSpacing(ns)
         BunchSpacing should be consistent with what is used from DB, as noise/sqrt(Lumi) depends on BunchSpacing
       if -1 uses lumi from database (Data) or Beam properties (MC) to perform the sqrt(Lumi) pileup noise scaling
   """
   statusOn = True
   allowedType=['float']
   StoredValue = -1.

class UseCaloLuminosity(JobProperty):
   """ Flag to tell to use the luminosity normalization for pileup noise from dedicated calo folder
       instead of online per LumiBlock
       Only active if FixedLuminosity is not >0
   """
   statusOn = True
   allowedType=['bool']
   StoredValue = True

class CaloNoiseFlags(JobPropertyContainer):
   """ The CaloNoiseFlag container
   """
   pass


# add container to jobProperties
jobproperties.add_Container(CaloNoiseFlags)

list_jobproperties = [
  FixedLuminosity,UseCaloLuminosity
  ]

for i in list_jobproperties:
    jobproperties.CaloNoiseFlags.add_JobProperty(i)

del list_jobproperties

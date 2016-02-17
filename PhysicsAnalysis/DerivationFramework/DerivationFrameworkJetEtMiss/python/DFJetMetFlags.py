# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class DFJetMetFlags(JobPropertyContainer):
  """ The DF Jet fix flag property container
  """
  pass

class doJetTrackMomentFix(JobProperty):
  """ Toggle jet track moment fixes
  """
  statusOn     = True     
  allowedTypes = ['bool']  # type
  StoredValue  = True      # default value

jobproperties.add_Container(DFJetMetFlags)
jobproperties.DFJetMetFlags.add_JobProperty(doJetTrackMomentFix)
dfjmFlags = jobproperties.DFJetMetFlags

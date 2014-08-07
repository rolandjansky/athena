# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##Creation: David Cote (CERN), September 2010

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class TrfFlags(JobPropertyContainer):
    """Flag/job property container for job transforms."""
jobproperties.add_Container(TrfFlags)

class KeepFullCommandUntouched(JobProperty):
    """ Blocks the execution of PopSynonyms. """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.TrfFlags.add_JobProperty(KeepFullCommandUntouched)

class ApplyIfMatchPattern(JobProperty):
    """ Activates output vs stream pattern matching """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.TrfFlags.add_JobProperty(ApplyIfMatchPattern)


trfFlags = jobproperties.TrfFlags

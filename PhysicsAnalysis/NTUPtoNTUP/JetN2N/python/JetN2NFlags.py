# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class JetN2NFlags (JobPropertyContainer):
    """Flags for Jet N2N making.
    """

jobproperties.add_Container (JetN2NFlags)

class JetN2NYear (JobProperty):
    """Define which year (2011 or 2012)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = '2012'
jobproperties.JetN2NFlags.add_JobProperty(JetN2NYear)

# shortcut name
JetN2NFlags = jobproperties.JetN2NFlags

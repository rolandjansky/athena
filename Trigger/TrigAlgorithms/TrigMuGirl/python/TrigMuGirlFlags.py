# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class doCSC(JobProperty):
    """ 
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False


class TrigMuGirlFlags(JobPropertyContainer):
    """ TrigMuGirl Job Properties
    """
    pass

jobproperties.add_Container(TrigMuGirlFlags)
jobproperties.TrigMuGirlFlags.add_JobProperty(doCSC)

trigMuGirlFlags = jobproperties.TrigMuGirlFlags
trigMuGirlFlags.print_JobProperties('tree&value')

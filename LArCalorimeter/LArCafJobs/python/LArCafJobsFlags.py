# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class outputFile(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "LArNoiseBurst.root"
    pass


class LArNoiseBurstFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(LArNoiseBurstFlags)

jobproperties.LArNoiseBurstFlags.add_JobProperty(outputFile)

larNoiseBurstFlags=jobproperties.LArNoiseBurstFlags

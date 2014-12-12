# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class CellQualityCut(JobProperty):
    #Cut on cell quality factor
    statusOn=True
    allowedTypes=['float']
    StoredValue=4000
    pass


class BadChanPerFEB(JobProperty):
    #Number of bad channels per Feb
    statusOn=True
    allowedTypes=['float']
    StoredValue=5
    pass

class BadFEBCut(JobProperty):
    #Number of bad febs per partition 
    statusOn=True
    allowedTypes=['int']
    StoredValue=5
    pass


class LArNoisyROFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(LArNoisyROFlags)


jobproperties.LArNoisyROFlags.add_JobProperty(CellQualityCut)
jobproperties.LArNoisyROFlags.add_JobProperty(BadChanPerFEB)
jobproperties.LArNoisyROFlags.add_JobProperty(BadFEBCut)

larNoisyROFlags = jobproperties.LArNoisyROFlags

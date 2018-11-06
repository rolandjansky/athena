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
    allowedTypes=['int']
    StoredValue=30
    pass

class BadFEBCut(JobProperty):
    #Number of bad febs per partition 
    statusOn=True
    allowedTypes=['int']
    StoredValue=5
    pass

class MNBLooseCut(JobProperty):
    #Number of channels to declare MNB-Loose
    statusOn=True
    allowedTypes=['int']
    StoredValue=5
    pass

class MNBTightCut(JobProperty):
    #Number of channels to declare MNB-Tight
    statusOn=True
    allowedTypes=['int']
    StoredValue=17
    pass

class MNBTight_PsVetoCut(JobProperty):
    #Number of channels to declare MNB-Tight
    statusOn=True
    allowedTypes=['list']
    StoredValue=[13,3]
    pass


class LArNoisyROFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(LArNoisyROFlags)


jobproperties.LArNoisyROFlags.add_JobProperty(CellQualityCut)
jobproperties.LArNoisyROFlags.add_JobProperty(BadChanPerFEB)
jobproperties.LArNoisyROFlags.add_JobProperty(BadFEBCut)
jobproperties.LArNoisyROFlags.add_JobProperty(MNBLooseCut)
jobproperties.LArNoisyROFlags.add_JobProperty(MNBTightCut)
jobproperties.LArNoisyROFlags.add_JobProperty(MNBTight_PsVetoCut)

larNoisyROFlags = jobproperties.LArNoisyROFlags

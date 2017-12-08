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

class KnownMNBFEBs(JobProperty):
    #Number of bad febs per partition 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[951255040, # EMBC FT 22 Slot 7
                 953810944, # EMBC FT 27 Slot 5
                 954105856, # EMBC FT 27 Slot 14
                 961052672, # EMBA FT 9 Slot 2
                 961839104, # EMBA FT 10 Slot 10
                 961970176, # EMBA FT 10 Slot 14
                 972980224, # EMBA FT 31 Slot 14
                 961445888, # EMBA FT 9 Slot 14
                 944144384  # EMBC FT 8 Slot 14 - added June 20 2017
                 ]
    pass


class LArNoisyROFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(LArNoisyROFlags)


jobproperties.LArNoisyROFlags.add_JobProperty(CellQualityCut)
jobproperties.LArNoisyROFlags.add_JobProperty(BadChanPerFEB)
jobproperties.LArNoisyROFlags.add_JobProperty(BadFEBCut)
jobproperties.LArNoisyROFlags.add_JobProperty(KnownMNBFEBs)

larNoisyROFlags = jobproperties.LArNoisyROFlags

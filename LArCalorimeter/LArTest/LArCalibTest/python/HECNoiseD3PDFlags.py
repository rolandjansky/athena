# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FileName(JobProperty):
    statusOn = False
    allowedTypes = ['str']
    StoredValue = "HECNoiseD3PD.root"
    pass


class HECNoiseD3PDFlags(JobPropertyContainer):
    pass

jobproperties.add_Container(HECNoiseD3PDFlags)
list_jobproperties = [
    FileName]

for i in list_jobproperties:
    jobproperties.HECNoiseD3PDFlags.add_JobProperty(i)
    pass

del list_jobproperties
    
HECNoiseflags=jobproperties.HECNoiseD3PDFlags

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class ExoticsN2NFlags (JobPropertyContainer):
    """Flags for Exotics N2N making.
    """

jobproperties.add_Container (ExoticsN2NFlags)

class EXMJTriggers (JobProperty):
    """Triggers for event selection"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'EF_j170_a4tchad_ht700,EF_j15_a4tchad,EF_j25_a4tchad,EF_j35_a4tchad,EF_j45_a4tchad,EF_j55_a4tchad,EF_j80_a4tchad,EF_j110_a4tchad,EF_j145_a4tchad,EF_j180_a4tchad,EF_j220_a4tchad,EF_j280_a4tchad,EF_j360_a4tchad,EF_j460_a4tchad'
jobproperties.ExoticsN2NFlags.add_JobProperty(EXMJTriggers)

class EXMJDoMC (JobProperty):
    """Keep MC Truth"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.ExoticsN2NFlags.add_JobProperty(EXMJDoMC)

class EXMJDoMCSlim (JobProperty):
    """Do MC Slimming (keep hard process and stable)"""
    """note: hard process is valid only for Pythia8"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.ExoticsN2NFlags.add_JobProperty(EXMJDoMCSlim)

class EXMJDoBH (JobProperty):
    """Do blackhole reconstruction from hard process"""
    """note: hard process is valid only for Pythia8"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.ExoticsN2NFlags.add_JobProperty(EXMJDoBH)

class EXMJDoMCSkim (JobProperty):
    """Force skim even for MC samples"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.ExoticsN2NFlags.add_JobProperty(EXMJDoMCSkim)

# shortcut name
ExoticsN2NFlags = jobproperties.ExoticsN2NFlags

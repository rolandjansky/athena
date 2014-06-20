# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FileName(JobProperty):
    """ Filename of TrigMbD3PD file
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TrigMbNtup.root"

class BSDBFolderName(JobProperty):
    """ Beam Spot tag
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="IndetBeampos-ES1-UPD2"


class TrigMbD3PDFlag(JobPropertyContainer):
    """ The TrigMb D3PD flag container
    """

# add the reconstruction flags container to the top container
jobproperties.add_Container(TrigMbD3PDFlag)

jobproperties.TrigMbD3PDFlag.add_JobProperty(FileName)
jobproperties.TrigMbD3PDFlag.add_JobProperty(BSDBFolderName)

trigMbD3PDflags=jobproperties.TrigMbD3PDFlag

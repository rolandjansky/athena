# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class FileName(JobProperty):
    """ Filename of MinBiasD3PD file
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="MinBiasNtup.root"

class BSDBFolderName(JobProperty):
    """ Beam Spot tag
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="IndetBeampos-ES1-UPD2"

class doMBTruth(JobProperty):
    """ Store truth information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPixelTracklets(JobProperty):
    """ Store pixel tracklets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doSCTTracklets(JobProperty):
    """ Store SCT tracklets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class doTRTTracklets(JobProperty):
    """ Store TRT tracklets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
                    
class doResolvedTracklets(JobProperty):
    """ Store Resolved tracklets
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
                    
class doTrigger(JobProperty):
    """ Store Trigger information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doDetailedTrigger(JobProperty):
    """ Store detailed trigger information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doBeamSpot(JobProperty):
    """ Store beam spot information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    
class doUnassociatedHits(JobProperty):
    """ Store unassociated hits
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
                    
class doV0s(JobProperty):
    """ Store V0 information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doLucid(JobProperty):
    """ Store LUCID information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
                
class doZDC(JobProperty):
    """ Store ZDC information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True #False

class doClusterHad(JobProperty):
    """ Store Hadro Calo information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doClusterEM(JobProperty):
    """ Store EM Calo information
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

                    
class MinBiasD3PDFlag(JobPropertyContainer):
    """ The MinBias D3PD flag container
    """

# add the reconstruction flags container to the top container
jobproperties.add_Container(MinBiasD3PDFlag)

jobproperties.MinBiasD3PDFlag.add_JobProperty(FileName)
jobproperties.MinBiasD3PDFlag.add_JobProperty(BSDBFolderName)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doMBTruth)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doPixelTracklets)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doSCTTracklets)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doTRTTracklets)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doResolvedTracklets)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doTrigger)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doDetailedTrigger)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doBeamSpot)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doUnassociatedHits)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doV0s)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doLucid)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doZDC)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doClusterHad)
jobproperties.MinBiasD3PDFlag.add_JobProperty(doClusterEM)

minbiasD3PDflags=jobproperties.MinBiasD3PDFlag

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class eflowAlgType(JobProperty):
    """ Flag to choose whether to do cluster level or cell level subtraction
    CellLevel is default, cluster level not used anymore...
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValue = ["CellLevel","EOverP"]
    StoredValue = "CellLevel"

class CalType(JobProperty):
    """ Flag to select calibration type
    """
    statusOn = True
    allowedTypes = ['str']
    allowedValue = ["LocalHad","NoCal"]
    StoredValue = "LocalHad"

class useLocalHadWeightsOOCC(JobProperty):
   """ Flag to decide if should use OutOfClusterCorrection
   """
   statusOn = True
   allowedTypes = ['bool']
   StoredValue = True

class useOverLapShowerCells(JobProperty):
    """ Flag to decide whether to calibrate cells in partially subtracted clusters
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useSplitShowers(JobProperty):
    """ Flag to decide if should recover split hadronic showers
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useEEtaFirstInt(JobProperty):
    """ Whether to bin E/P in E,Eta and layer of First Interaction
    or just E,Eta
    """
    
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class recoverIsolatedTracks(JobProperty):
    """ Flag to decide whether to recover isolated tracks
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class UseElectronHadronID(JobProperty):
    """ Flag to decide whether to use electron/pion ID
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class runTauMode(JobProperty):
    """ Flag to decide whether to run on clusters/tracks from tau cands only
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useLeptons(JobProperty):
    """ Flag to toggle usage of lepton tracks on/off - if on lepton tracks are masked out
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class storeLeptonCells(JobProperty):
    """ Flag to toggle storage of lepton cells on/off - if on will put lepton cells in storegate in eflowPreparation
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class useLCInput(JobProperty):
    """ Flag to toggle usage of EM and LC inputs
    """
    statusOn = False
    allowedTypes = ['bool']
    StoredValue = False

class useUpdated2015ChargedShowerSubtraction(JobProperty):
    """ Flag to toggle use of 2015 charged shower subtraction - this disables the subtraction for tracks found to be in dense environments
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useAODReductionClusterMomentList(JobProperty):
    """ Flag to toggle use of AOD reduction Task force reduced moment list
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class fixMuonLogic(JobProperty):
    """ Flag to toggle fix of the muon logic, for testing in physics validation only currently
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    
# Defines the container for the eflowRec flags

class eflowRecFlags(JobPropertyContainer):
     """ The eflowRec flag property container
     """
     pass
 
# add the flags container to the top container 
jobproperties.add_Container(eflowRecFlags)

eflowJobProperties = [eflowAlgType,CalType,useLocalHadWeightsOOCC,useOverLapShowerCells,useSplitShowers,useEEtaFirstInt,recoverIsolatedTracks,UseElectronHadronID,runTauMode, useLeptons,storeLeptonCells, useLCInput, useUpdated2015ChargedShowerSubtraction,useAODReductionClusterMomentList,fixMuonLogic]

for i in eflowJobProperties :
    jobproperties.eflowRecFlags.add_JobProperty(i)

del eflowJobProperties


# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


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

class recoverIsolatedTracks(JobProperty):
    """ Flag to decide whether to recover isolated tracks
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class useElectrons(JobProperty):
    """ Flag to toggle usage of electron tracks on/off - if on electron tracks are masked out
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useMuons(JobProperty):
    """ Flag to toggle usage of muon tracks on/off - if on muon tracks are masked out
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

class useUpdated2015ChargedShowerSubtraction(JobProperty):
    """ Flag to toggle use of 2015 charged shower subtraction - this disables the subtraction for tracks found to be in dense environments
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class useCalibHitTruth(JobProperty):
    """ Flag to toggle use of calibration hit truth, which is off by default because most Monte Carlo samples do not contain the calibration hit truth information
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class usePFEGammaPFOAssoc(JobProperty):
    """ Flag to toggle use of linking between Egamma objects and PFOs
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class usePFFlowElementAssoc(JobProperty):
    """ Flag to toggle use of linking  between objects (egamma,muon,tau) and flow elements
    """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class provideShowerSubtractedClusters(JobProperty):
    """ Flag to toggle provision of ElementLink to charged shower subtracted calorimeter clusters. Such links are added to
    neutral PFO and we write out the relevant CaloClusterContainer to AOD such that the links remain valid """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class useFlowElements(JobProperty):
    """ Flag to toggle whether to create the new FlowElement EDM objects in addition to the PFO EDM objects. """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True

class doFlowElementValidation(JobProperty):
    """Flag to turn on Validation plots for Flow Elements (only works if useFlowElements also set ON)"""
    statusOn=True
    allowedTypes= ['bool']
    StoredValue= False

# Defines the container for the eflowRec flags

class eflowRecFlags(JobPropertyContainer):
     """ The eflowRec flag property container
     """
     pass

# add the flags container to the top container
jobproperties.add_Container(eflowRecFlags)

eflowJobProperties = [eflowAlgType,recoverIsolatedTracks, useElectrons, useMuons ,storeLeptonCells, useUpdated2015ChargedShowerSubtraction,useCalibHitTruth,usePFEGammaPFOAssoc,usePFFlowElementAssoc,provideShowerSubtractedClusters, useFlowElements, doFlowElementValidation]

for i in eflowJobProperties :
    jobproperties.eflowRecFlags.add_JobProperty(i)

del eflowJobProperties

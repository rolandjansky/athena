# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

class UnsubtractedSuffix(JobProperty):
    """ Common suffix for all unsubtracted jet collections
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'Unsubtracted'

class SeedSuffix(JobProperty):
    """ Common suffix for all unsubtracted jet collections
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'Seed'

class SeedRValue(JobProperty):
    """ R used to reconstruct jets for seed finding
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2

class SeedPtMin(JobProperty):
    """ Minimum pT for seeds after iteration
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 25*Units.GeV

class RecoOutputPtMin(JobProperty):
    """ Minimum pT for seeds after iteration
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 15*Units.GeV


class ClusterDRAssociation(JobProperty):
    """ DR used to associate clusters w/ jets. separate from jet constituents
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.8

class ExclusionRadius(JobProperty):
    """ Towers w/in this DR from seed jets are excluded from background calc
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.4

class ApplyOriginCorrection(JobProperty):
    """ Option to apply origin correction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class ApplyEtaJESCalibration(JobProperty):
    """ Option to apply MC-derived calibration 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class ApplyTowerEtaPhiCorrection(JobProperty):
    """ Option to apply MC-derived calibration 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DCutMaxOverMean(JobProperty):
    """ Cut for first step in seed finding: max/mean of constituent ET's must be > that this.
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4

class DCutMax(JobProperty):
    """ Cut for first step in seed finding: max consituent ET must be > than this.
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 3*Units.GeV


class AntiKtRValues(JobProperty):
    """ Values of R used for anti-kt jet finding
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [0.2,0.3,0.4,0.8,1.0]

class DoCellBasedSubtraction(JobProperty):
    """ option to use cell based subtraction
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class HarmonicsForSubtraction(JobProperty):
    """ List of flow harmonics applied to jet subtraction
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [2,3,4]
    #StoredValue  = []

class ModulationScheme(JobProperty):
    """ 1, 2 or 3 correspond to total calo, total calo w/ fcal phase, fcal only
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class Remodulate(JobProperty):
    """ Correct average UE for incomplete cancellation of flow harmonics
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class ExtraFlowMoments(JobProperty):
    """ Calculate extra flow moments
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoHIBTagging(JobProperty):
    """ Perform b-tagging over HI jet collections
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True


class HIClusterKey(JobProperty):
    """ Key of CaloClusterContainer containing HI clusters
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'HIClusters'

class IteratedEventShapeKey(JobProperty):
    """ Key of HIEventShapeContainer that has jet contribution excluded
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'HIEventShape_iter1'

class TruthJetPtMin(JobProperty):
    """ Minimum pT of saved truth jets
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 5*Units.GeV

class TrackInputPtMin(JobProperty):
    """ Minimum track pT used in track jet finding
xo    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4*Units.GeV

class TrackJetPtMin(JobProperty):
    """ Minimum pT of saved track jets
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 7*Units.GeV

class TrackJetContainerName(JobProperty):
    """ Name of main track jets used in iteration and ghost matching
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AntiKt4HITrackJets'

class TrackJetSeeds(JobProperty):
    """ Flag to control whether track jets are used as seeds in addition to calo jets
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class HIJetOutputList(JobProperty):
    """ List of items to be written to AOD, initially empty but filled based on configuration
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class WriteClusters(JobProperty):
    """ Option to write HIClusters to AOD
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class WriteClusterMoments(JobProperty):
    """ If False, the dynamic cluster variables are all dropped
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class MomentsSkipped(JobProperty):
    """ List of moments not to write
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

class WriteUnsubtracted(JobProperty):
    """ Option to write unsubtracted containers
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class WriteSeeds(JobProperty):
    """ Option to write seed containers
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class UseHITracks(JobProperty):
    """ Use track selectors w/ HI specific cuts
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class HIJetRecFlags(JobPropertyContainer):
    """ The HIJetRec property container.
    """
    pass
class TWConfigFile(JobProperty):
    """ Name of main track jets used in iteration and ghost matching
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'cluster.geo.HIJING_2018.root'
# add the flags container to the top container 
jobproperties.add_Container(HIJetRecFlags)

# We want always the following flags in the container  
list_jobproperties = [UnsubtractedSuffix,
                      SeedSuffix,
                      SeedRValue,
                      SeedPtMin,
                      RecoOutputPtMin,
                      ClusterDRAssociation,
                      ExclusionRadius,
                      ApplyOriginCorrection,
                      ApplyEtaJESCalibration,
                      DCutMaxOverMean,
                      DCutMax,
                      AntiKtRValues,
                      DoCellBasedSubtraction,
                      HarmonicsForSubtraction,
                      ModulationScheme,
                      Remodulate,
                      ExtraFlowMoments,
                      DoHIBTagging,
                      HIClusterKey,
                      IteratedEventShapeKey,
                      TruthJetPtMin,
                      TrackInputPtMin,
                      TrackJetPtMin,
                      TrackJetContainerName,
                      TrackJetSeeds,
                      HIJetOutputList,
                      WriteClusters,
                      WriteClusterMoments,
                      WriteUnsubtracted,
                      WriteSeeds,
                      UseHITracks,
                      MomentsSkipped,
                      TWConfigFile,
                      ApplyTowerEtaPhiCorrection
                      ]

for i in list_jobproperties:
    jobproperties.HIJetRecFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties

HIJetFlags=jobproperties.HIJetRecFlags

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

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
    StoredValue  = 3000


class AntiKtRValues(JobProperty):
    """ Values of R used for anti-kt jet finding
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [0.2,0.3,0.4]

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
    StoredValue  = 5000.

class InputTrackPtMin(JobProperty):
    """ Minimum track pT used in track jet finding
xo    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4000.

class TrackJetPtMin(JobProperty):
    """ Minimum pT of saved track jets
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 7000.

class TrackJetContainerName(JobProperty):
    """ Name of main track jets used in iteration and ghost matching
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AntiKt4HITrackJets'

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

# add the flags container to the top container 
jobproperties.add_Container(HIJetRecFlags)

# We want always the following flags in the container  
list_jobproperties = [UnsubtractedSuffix,
                      SeedSuffix,
                      ClusterDRAssociation,
                      ExclusionRadius,
                      DCutMaxOverMean,
                      DCutMax,
                      AntiKtRValues,
                      HIClusterKey,
                      IteratedEventShapeKey,
                      TruthJetPtMin,
                      InputTrackPtMin,
                      TrackJetPtMin,
                      TrackJetContainerName,
                      HIJetOutputList,
                      WriteClusters,
                      WriteUnsubtracted,
                      WriteSeeds,
                      UseHITracks
                      ]

for i in list_jobproperties:
    jobproperties.HIJetRecFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties

HIJetFlags=jobproperties.HIJetRecFlags

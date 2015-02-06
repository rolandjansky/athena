# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class UnsubtractedSuffix(JobProperty):
    """ Common suffix for all unsubtracted jet collections
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'Unsubtracted'

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

class InputTrackPtMin(JobProperty):
    """ Minimum track pT used in track jet finding
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4000.

class TrackJetPtMin(JobProperty):
    """ Minimum pT of saved track jets
    """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 7000.

class HIJetAODItemList(JobProperty):
    """ List of items to be written to AOD, initially empty but filled based on configuration
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

    
class HIJetRecFlags(JobPropertyContainer):
    """ The HIJetRec property container.
    """
    pass

# add the flags container to the top container 
jobproperties.add_Container(HIJetRecFlags)

# We want always the following flags in the container  
list_jobproperties = [UnsubtractedSuffix,
                      ClusterDRAssociation,
                      ExclusionRadius,
                      DCutMaxOverMean,
                      DCutMax,
                      AntiKtRValues,
                      HIClusterKey,
                      InputTrackPtMin,
                      TrackJetPtMin,
                      HIJetAODItemList
                      ]

for i in list_jobproperties:
    jobproperties.HIJetRecFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties

HIJetFlags=jobproperties.HIJetRecFlags

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   egammaRec/python/egammaRecFlags.py
#=======================================================================
__author__  = 'D. Rousseau'
__version__="$Revision: 1.3 $"
__doc__="egamma flags . "


#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class Enabled(JobProperty):
    """ jobproperty to disable/enable the egamma algorithms as a group (container) in one go.
    Can enable/disable the full egamma
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class egammaRecFlagsJobProperty(JobProperty):
    """ special flag . Subclass  which  has get_value depending on job properties.egammaRecFlags.Enabled
    so properties inheriting from can be disabled in batch
    """
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.egammaRecFlags.Enabled()
    
class doEgammaCaloSeeded (egammaRecFlagsJobProperty):
    """ switch for standard cluster-based egamma algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doEgammaForwardSeeded (egammaRecFlagsJobProperty):
    """ switch for standard forward electrons algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doEgammaTruthAssociation (egammaRecFlagsJobProperty):
    """ switch for truth association alg
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTopoCaloSeeded (egammaRecFlagsJobProperty):
    """ switch for SW TopoCalo algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doConversions (JobProperty):
    """ switch for Conversions
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class useBremTracks (JobProperty):
    """ switch using brem tracks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class cellContainerName (JobProperty):
    """Allows overriding the name of the cell container used for building EM object clusters.
    """
    statusOn=False
    allowedTypes=['str']
    StoredValue=''
 
class doSwitchTRTGas (JobProperty):
    """ switch using Xenon gas in TRT
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doSuperclusters (egammaRecFlagsJobProperty):
    """ switch to enable/disable the supercluster based algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class inputTopoClusterCollection (JobProperty):
    """Name of input cluster container from which the topological cluster to be used during superclustering
    are selected 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='CaloTopoCluster'

class egammaTopoClusterCollection (JobProperty):
    """Name of cluster container containing the selected topological clusters 
    used during superclustering
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='egammaTopoCluster'

class doBremFinding (egammaRecFlagsJobProperty):
    """ switch to use GSF-based brem finder during the search of satellites for superclustering
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class clusterCorrectionVersion (JobProperty):
    """Version of calo cluster corrections used for calibration.
    """
    statusOn=True
    allowedTypes=['str', 'None']
    StoredValue='v12phiflip_noecorrnogap'


# Defines a sub-container holding the jobProperties for egamma
class egammaRecFlags(JobPropertyContainer):
    """ egamma information """

# add the egammaRec flags container to the top container 
jobproperties.add_Container(egammaRecFlags)

# I want always the following flags in the container  
_list_Egamma=[Enabled,doEgammaCaloSeeded,doEgammaForwardSeeded,doConversions,doTopoCaloSeeded,useBremTracks,cellContainerName,doSwitchTRTGas,doEgammaTruthAssociation, clusterCorrectionVersion, doSuperclusters, inputTopoClusterCollection, egammaTopoClusterCollection, doBremFinding]

for j in _list_Egamma: 
    jobproperties.egammaRecFlags.add_JobProperty(j)
del _list_Egamma



#=======================================================================


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
    """ govern all algorithms
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class egammaRecFlagsJobProperty(JobProperty):
    """ special flag classing with job properties.egammaRecFlags.Enabled
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

class doShowerBuilding (JobProperty):
    """ switch for Shower Building
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doAODRender (JobProperty):
    """ switch for egamma AOD render algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

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
 
class makeNewConvContainer (JobProperty):
    """ make a new conversion vertex container in conversion builder to be able to add single-track vertices built there
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class doSwitchTRTGas (JobProperty):
    """ switch using Xenon gas in TRT
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#New switch for turning off/on electron supercluster building.
class doSuperclusters (egammaRecFlagsJobProperty):
    """ switch for supercluster TopoCalo algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class inputTopoClusterCollection (JobProperty):
    """Name of input cluster container for superclustering
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='CaloTopoCluster'

class egammaTopoClusterCollection (JobProperty):
    """Name of input cluster container for superclustering
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='egammaTopoCluster'

class doBremFinding (egammaRecFlagsJobProperty):
    """ switch to use GSF-based brem finder in superclustering
    """
    statusOn=False
    allowedTypes=['bool']
    StoredValue=False
#
# 
# Defines a sub-container for the algorithm switches
class egammaRecFlags(JobPropertyContainer):
    """ egamma information """

class clusterCorrectionVersion (JobProperty):
    """Version of calo cluster corrections used for calibration.
    """
    statusOn=True
    allowedTypes=['str', 'None']
    StoredValue='v12phiflip_noecorrnogap'


# add the egammaRec flags container to the top container 
jobproperties.add_Container(egammaRecFlags)


# I want always the following flags in the  container  
_list_Egamma=[Enabled,doEgammaCaloSeeded,doEgammaForwardSeeded,doAODRender,doConversions,doShowerBuilding,doTopoCaloSeeded,useBremTracks,makeNewConvContainer,cellContainerName,doSwitchTRTGas,doEgammaTruthAssociation, clusterCorrectionVersion, doSuperclusters, inputTopoClusterCollection, egammaTopoClusterCollection, doBremFinding]

for j in _list_Egamma: 
    jobproperties.egammaRecFlags.add_JobProperty(j)
del _list_Egamma



#=======================================================================


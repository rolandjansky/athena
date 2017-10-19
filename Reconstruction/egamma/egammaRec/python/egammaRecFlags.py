# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   egammaRec/python/egammaRecFlags.py
#=======================================================================
__author__  = 'C. Anastopoulos'
__version__="$Revision: 2.0 $"
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
    so properties inheriting from this will be disables if egammaRec is not Enabled.
    """
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.egammaRecFlags.Enabled()

#Different egamma seeding styles    
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

class doTopoCaloSeeded (JobProperty):
    """ switch for SW TopoCalo algorithm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doBremFinding (egammaRecFlagsJobProperty):
    """ switch for whether to do the brem binding
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doVertexBuilding (egammaRecFlagsJobProperty):
    """ switch for whether to do the conversion vertex building
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#Other options
class doEgammaTruthAssociation (JobProperty):
    """ switch for truth association alg
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doConversions (JobProperty):
    """ switch for doing conversion matching
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

# Super cluster options
class doSuperclusters (JobProperty):
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

#Calibration Options
class clusterCorrectionVersion (JobProperty):
    """Version of calo cluster corrections used for calibration.
    """
    statusOn=True
    allowedTypes=['str', 'None']
    StoredValue='v12phiflip_noecorrnogap'

class calibMVAVersion (JobProperty):
    """Version of calo cluster corrections used for calibration.
    """
    statusOn=True
    allowedTypes=['str', 'None']
    StoredValue="egammaMVACalib/offline/v6"

# Defines a sub-container holding the jobProperties for egamma
class egammaRecFlags(JobPropertyContainer):
    """ egamma information """

# add the egammaRec flags container to the top container 
jobproperties.add_Container(egammaRecFlags)

# I want always the following flags in the container  
_list_Egamma=[Enabled, doEgammaCaloSeeded, doEgammaForwardSeeded, doBremFinding, doVertexBuilding,
              doConversions, doTopoCaloSeeded, cellContainerName, doSwitchTRTGas,
              doEgammaTruthAssociation, clusterCorrectionVersion, calibMVAVersion, doSuperclusters, 
              inputTopoClusterCollection, egammaTopoClusterCollection]

for j in _list_Egamma: 
    jobproperties.egammaRecFlags.add_JobProperty(j)
del _list_Egamma



#=======================================================================


# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   DerivationFrameworkEGamma/python/egammaDFFlags.py
#=======================================================================
__author__  = 'G. Marchiori'
__version__="$Revision: 1.0 $"
__doc__="egamma DF flags . "

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

class egammaDFFlagsJobProperty(JobProperty):
    """ special flag . Subclass  which  has get_value depending on job properties.egammaDFFlags.Enabled
    so properties inheriting from this will be disables if egammaRec is not Enabled.
    """
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.egammaDFFlags.Enabled()

# enable or disable cell-level reweighting
class doEGammaCellReweighting (egammaDFFlagsJobProperty):
    """ switch for enabling cell-level reweighting of e/gamma clusters
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# enable or disable cell-level reweighting variations
class doEGammaCellReweightingVariations (egammaDFFlagsJobProperty):
    """ switch for enabling cell-level reweighting of e/gamma clusters with max/min variations
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doEGammaDAODTrackThinning (egammaDFFlagsJobProperty):
    """ switch for enabling track-thinning in egamma DAODs
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


# class calibMVAVersion (JobProperty):
#     """Version of calo cluster corrections used for calibration.
#     """
#     statusOn=True
#     allowedTypes=['str', 'None']
#     StoredValue="egammaMVACalib/offline/v7_pre"

# Defines a sub-container holding the jobProperties for egamma
class egammaDFFlags(JobPropertyContainer):
    """ egamma information """

# add the egammaRec flags container to the top container 
jobproperties.add_Container(egammaDFFlags)

# I want always the following flags in the container  
_list_Egamma=[Enabled,doEGammaCellReweighting,doEGammaCellReweightingVariations,doEGammaDAODTrackThinning]

for j in _list_Egamma: 
    jobproperties.egammaDFFlags.add_JobProperty(j)
del _list_Egamma




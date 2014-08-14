# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file tauRecFlags.py
#
#@brief define some tauRec flags 
################################################################################

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class Enabled(JobProperty):
    """ if all tau algorithm to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTauRec(JobProperty):
    """ if TauRec to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.tauRecFlags.Enabled()


class doRunTauDiscriminant(JobProperty):
    """ switch for TauDiscriminant running
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class useVertexBasedConvFinder(JobProperty):
    """ switch for PhotonConversionVertex.cxx/h conversion veto
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class useNewPIDBasedConvFinder(JobProperty):
    """ switch for TauConversionTagger.cxx/h conversion veto
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPanTau(JobProperty):
    """ if pantau should run after tauRec
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doBonnPi0(JobProperty):
    """ switch on new (Bonn) Pi0 Finder
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class useOldVertexFitterAPI(JobProperty):
    """ use the old (AOD-style) API of the AdaptiveVertexFitter.
    The AdaptiveVertexFitter is used for finding the tau decay vertex (aka. secondary vertex) and called in TauVertexVariables.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


# Defines a sub-container for the algorithm switches
class tauRecFlags(JobPropertyContainer):
    """ tau information """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecFlags)

# I want always the following flags in the Rec container  
_list_tau=[Enabled,doTauRec,doRunTauDiscriminant,useVertexBasedConvFinder,useNewPIDBasedConvFinder,doPanTau,doBonnPi0,useOldVertexFitterAPI]
for j in _list_tau: 
    jobproperties.tauRecFlags.add_JobProperty(j)
del _list_tau



#################################################
#### AOD flags ##################################
#                                               #
# only for re-running tau reco on AODs          #
# !not for normal reconstruction!               #
#################################################

class doUpdate(JobProperty):
    """ update the tau containers (if running in AODmode)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class tauRecAODFlags(JobPropertyContainer):
    """ tau information if re-running on AODs """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecAODFlags)

# I want always the following flags in the Rec container  
_list_tau=[doUpdate]
for j in _list_tau: 
    jobproperties.tauRecAODFlags.add_JobProperty(j)
del _list_tau



#=======================================================================
    

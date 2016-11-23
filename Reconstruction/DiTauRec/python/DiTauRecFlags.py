# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file DiTauRecFlags.py
#
#@brief define some DiTauRec flags 
################################################################################

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


class Enabled(JobProperty):
    """ if all DiTau algorithm to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doDiTauRec(JobProperty):
    """ if TauRec to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.DiTauRecFlags.Enabled()

class diTauRecJetSeedPt(JobProperty):
    """ configure inpute jet seed
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=300000

class doVtxFinding(JobProperty):
    """ force Vertex Finding on, needed in non standard production
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    


class doRunDiTauDiscriminant(JobProperty):
    """ switch for DiTauDiscriminant running
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class doCellFinding(JobProperty):
    """ switch of cell handling
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True



# Defines a sub-container for the algorithm switches
class DiTauRecFlags(JobPropertyContainer):
    """ tau information """

# add the tau flags container to the top container 
jobproperties.add_Container(DiTauRecFlags)

# I want always the following flags in the Rec container  
_list_tau=[Enabled,doDiTauRec,diTauRecJetSeedPt,doVtxFinding,doRunDiTauDiscriminant,doCellFinding,]
for j in _list_tau: 
    jobproperties.DiTauRecFlags.add_JobProperty(j)
del _list_tau


diTauFlags=jobproperties.DiTauRecFlags
#=======================================================================
    

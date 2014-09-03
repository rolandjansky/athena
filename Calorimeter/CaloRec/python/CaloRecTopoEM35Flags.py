# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Flags for fixed size3x5 seeded by topo clusters
"""
#
#
__author__  = 'D. Rousseau, M. Gallas'
__version__="$Revision: 1.7 $"
__doc__="Calo flags . "



#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class EtSeedCut(JobProperty):
    """ Et cut on input EMtopo to define fixed clusters around this seed
    """
    statusOn = True
    allowedTypes=['float']
    StoredValue=-1.

# Defines a sub-container for the algorithm switches
class CaloRecTopoEM35Flags(JobPropertyContainer):
    """ Calo information """


# add the Calo flags container to the top container 
jobproperties.add_Container(CaloRecTopoEM35Flags)


# I want always the following flags in the Rec container  
_list_Calo=[EtSeedCut]
for j in _list_Calo: 
    jobproperties.CaloRecTopoEM35Flags.add_JobProperty(j)
del _list_Calo



#=======================================================================


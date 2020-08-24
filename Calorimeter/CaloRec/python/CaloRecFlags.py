# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   RecExConfig/python/CaloFlags.py
#=======================================================================
""" Flags for beam structure
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

class Enabled(JobProperty):
    """ bla
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class CaloRecFlagsJobProperty(JobProperty):
    """ special flag class ANDing with jobproperies.CaloRecFlags.Enabled
    """
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.CaloRecFlags.Enabled()
    

class doCaloTopoCluster(CaloRecFlagsJobProperty):
    """ switch for calo topo clustering
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class doCaloEMTopoCluster(CaloRecFlagsJobProperty):
    """ switch for calo EM topo clustering
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class emTopoClusterThreshold(JobProperty):
    """ switch for calo EM topo clustering threshold (633, 430,...)
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=['633','430']
    StoredValue= '430'
    for v in allowedValues:
        exec( "def is_%s(self): return self() == '%s'" % (v,v) )

class doEmCluster(CaloRecFlagsJobProperty):
    """ switch for em sliding window cluster
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doCaloCluster(CaloRecFlagsJobProperty):
    """ switch for combined calo cluster
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doCaloTowerFromCells(CaloRecFlagsJobProperty):
    """ switch for combined calo cluster
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doCaloTowerFromCluster(CaloRecFlagsJobProperty):
    """ switch for combined calo cluster
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class doCaloTopoTower(CaloRecFlagsJobProperty):
    """ switch noise suppressed towers based on standard tower + topo clusters
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doTileMuId(CaloRecFlagsJobProperty):
    """ switch for TileMuId - muon finding algorighm
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTileCellCorrection(CaloRecFlagsJobProperty):
    """ switch to enable TileCell correction when reading from ESD
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doLArAffectedRegion(CaloRecFlagsJobProperty):
    """ switch to store in metedata affected LAr regions
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doLArAutoConfiguration(CaloRecFlagsJobProperty):
    """ switch to activate LAr autoconfiguration based on data format info stored in Cool
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doLArNoisyRO(CaloRecFlagsJobProperty):
   """ switch to find noisy/bad events in LAr
   """
   statusOn=True
   allowedTypes=['bool']
   StoredValue=True

class doEMDigits(CaloRecFlagsJobProperty):
   """ switch to activate writing of digits in EM clusters in ESD (data only)
   """
   statusOn = True
   allowedTypes=['bool']
   StoredValue=False

class doFillMBTSBackgroundBit(CaloRecFlagsJobProperty):
    """ swtich to active filling of MBTS bit in the background word of the EventInfo object
    """
    statusOn = True
    allowedTypes=['bool']
    StoredValue=True

class doLArNoiseBurstVeto(CaloRecFlagsJobProperty):
    """ switch to activate algorithm to fill noise burst veto in EventInfo
    """
    statusOn = True
    allowedTypes=['bool']
    StoredValue=True

class clusterCellGetterName(JobProperty):
    """Name of the getter used to build cells used for cluster reconstruction.
The name should include the full module name."""
    statusOn = True
    allowedTypes=['str']
    StoredValue = 'CaloRec.CaloCellGetter.CaloCellGetter'
    

 
# Defines a sub-container for the algorithm switches
class CaloRecFlags(JobPropertyContainer):
    """ Calo information """


# add the Calo flags container to the top container 
jobproperties.add_Container(CaloRecFlags)


# I want always the following flags in the Rec container  
_list_Calo=[Enabled,doCaloTopoCluster,doEmCluster,doCaloEMTopoCluster,emTopoClusterThreshold,doCaloCluster,doCaloTopoTower,doTileMuId,doTileCellCorrection,doLArAffectedRegion,doLArAutoConfiguration,doLArNoisyRO,doEMDigits,doFillMBTSBackgroundBit,doLArNoiseBurstVeto,clusterCellGetterName,doCaloTowerFromCells,doCaloTowerFromCluster]
for j in _list_Calo: 
    jobproperties.CaloRecFlags.add_JobProperty(j)
del _list_Calo



#=======================================================================


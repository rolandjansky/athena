# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#=======================================================================
# File:   RecExConfig/python/RecAlgsFlags
#=======================================================================
""" Reconstruction specific switches for algorithms. ONLY FOR A TRANSITION PERIOD

"""
#
#
__author__  = 'D. Rousseau, M. Gallas'
__version__="$Revision: 1.29 $"
__doc__="Switches for reconstruction algorithms."

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer

from RecExConfig.RecFlags import rec


class doTrackRecordFilter(JobProperty):
    """ Switch for TrackRecordFilter (special muon truth)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doMuonIDStandAlone(JobProperty):
    """ Switch for MuonID standalone reconstruction"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doMuonIDCombined(JobProperty):
    """ Switch for MuonID combined (muon + ID)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True       
class doMuidLowPt(JobProperty):
    """ Switch for MuonID low pt reconstruction"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False       
class doMuGirl(JobProperty):
    """ Switch for MuGirl (low pt muon reconstruction)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True       
class doStaco(JobProperty):
    """ Switch for Staco (statistical ID + muon combination)"""    
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doMuTag(JobProperty):
    """ Switch for MuTag (low pt muon reconstruction)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doTileMuID(JobProperty):
    """ Switch for TileMuId (muon identification in tile)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doMuonSpShower(JobProperty):
    """ Switch for MuonSpShower (muon identification in jets)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=False 
class doCaloTrkMuId(JobProperty):
    """ Switch for CaloTrkMuid (muon identification in calorimeter with tracks)"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doMissingET(JobProperty):
    """ Switch for MissingET"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=True 
class doObjMissingET(JobProperty):
    """ Switch for object based missingET"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False 
class doMissingETSig(JobProperty):
    """ Switch for missingET significance"""
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=False 
class doEFlow(JobProperty):
    """ Switch for energy flow"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True 
class doEFlowJet(JobProperty):
    """ Switch for jet reconstruction from energy flow"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False 
class doEgammaBremReco(JobProperty):
    """ Switch for new egamma brem reco"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False 
class doTrigger(JobProperty):
    """ Switch for trigger"""
    # TODO: Remove this flag and assume False in all client configuration
    # once the Run-2 trigger execution is removed from release 22
    statusOn=True 
    allowedTypes=['bool']
    StoredValue=False
class doAtlfast(JobProperty):
    """ Switch for fast simulation (but normally run at ESD->AOD stage)"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doMonteCarloReact(JobProperty):
    """ Switch for AOD to AOD correction (but really ESD) """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class doTrackParticleCellAssociation(JobProperty):
    """ Switch for TrackParticle cell association  """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

# Defines a sub-container for the algorithm switches
class RecAlgs(JobPropertyContainer):
    """ Algorithm switches. """

rec.add_Container(RecAlgs)
# The short name (carefull do not select same name as shot name as well. 
# otherwise problems with pickle)  
recAlgs=rec.RecAlgs

# I want always the following flags in the RecAlgs container  
_list_Rec=[doTrackRecordFilter,\
           doMissingET,doObjMissingET,doMissingETSig,\
           doEFlow,doEFlowJet,doEgammaBremReco,\
           doTrigger,\
           doMuonIDStandAlone,doMuonIDCombined,doMuidLowPt,doMuGirl,\
           doStaco,doMuTag,doTileMuID,doMuonSpShower,doCaloTrkMuId,
           doAtlfast,doMonteCarloReact,doTrackParticleCellAssociation ]
for j in _list_Rec: 
    recAlgs.add_JobProperty(j)
del _list_Rec


#=======================================================================

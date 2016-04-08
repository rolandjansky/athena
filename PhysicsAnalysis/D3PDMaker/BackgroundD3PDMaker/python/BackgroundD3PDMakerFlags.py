# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file BackgroundD3PDMaker/python/BackgroundD3PDMakerFlags.py
## @author Mark Tibbetts <Mark.James.Tibbetts@cern.ch>
## @brief Commond flags for backgroundD3PD
#

"""Common flags for D3PD making.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class BackgroundD3PDMakerFlags (JobPropertyContainer):
    """Common flags for BackgroundD3PD making.
    """

jobproperties.add_Container (BackgroundD3PDMakerFlags)

class AddTriggerBits (JobProperty):
    """ add trigger bits"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(AddTriggerBits)

class doMet (JobProperty):
    """If true fill MET; true by default, may cause problems for cosmics"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doMet)


class doLUCID (JobProperty):
    """If true fill LUCID hit info; only in ESD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doLUCID)

class doBCM (JobProperty):
    """If true fill BCM RDO and time info; only in ESD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doBCM)   

class doZDC (JobProperty):
    """If true fill ZDC info; only in ESD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doZDC)    

class doCaloMuon (JobProperty):
    """If true fill calo muons; missing from HI reco"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doCaloMuon)

class doMuGirlSeg (JobProperty):
    """If true fill muon girl segments; missing from HI reco"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doMuGirlSeg)

class doBeamBkgd (JobProperty):
    """If true fill beam background tool block; missing from HI reco"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doBeamBkgd)

class doTrk (JobProperty):
    """If true fill track info; veto from HI reco due to size"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doTrk)

class doMuonTrk (JobProperty):
    """If true fill muon track info; veto by default due to size"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doMuonTrk)

class doMuonHits (JobProperty):
    """If true fill muon hits; veto by default due to size"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doMuonHits)

class doCaloJet (JobProperty):
    """If true fill calo jets, fill track jets if false; HI reco uses track jets"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doCaloJet)

class doPixelClus (JobProperty):
    """If true fill pixel clusters; veto by default due to size"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doPixelClus)

class doCaloClus (JobProperty):
    """If true fill calorimeter clusters; veto by default due to size"""
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
jobproperties.BackgroundD3PDMakerFlags.add_JobProperty(doCaloClus)

BackgroundD3PDMakerFlags = jobproperties.BackgroundD3PDMakerFlags

def BackgroundD3PDMakerFlagsSetDefaults():
    flags=BackgroundD3PDMakerFlags
    pass

def BackgroundD3PDMakerFlagsSetESD():
    flags=BackgroundD3PDMakerFlags
    flags.doLUCID.set_Value(True)
    flags.doBCM.set_Value(True)
    flags.doZDC.set_Value(True)

def BackgroundD3PDMakerFlagsSetAOD():
    flags=BackgroundD3PDMakerFlags
    flags.doLUCID.set_Value(False)
    flags.doBCM.set_Value(False)
    flags.doZDC.set_Value(False)
    
def BackgroundD3PDMakerFlagsSetHI():
    flags=BackgroundD3PDMakerFlags
    flags.doCaloMuon.set_Value(False)
    flags.doMuGirlSeg.set_Value(False)
    flags.doBeamBkgd.set_Value(False)
    flags.doTrk.set_Value(False)
    flags.doCaloJet.set_Value(False)

def BackgroundD3PDMakerFlagsSetpp():
    flags=BackgroundD3PDMakerFlags
    flags.doCaloMuon.set_Value(True)
    flags.doMuGirlSeg.set_Value(True)
    flags.doBeamBkgd.set_Value(True)
    flags.doTrk.set_Value(True)
    flags.doCaloJet.set_Value(True)

    

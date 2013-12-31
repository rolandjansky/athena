# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file QcdD3PDMaker/python/QcdD3PDMakerFlags.py
## @author Michiru Kaneda <Michiru.Kaneda@cern.ch>
## @brief Commond flags for QcdD3PD
#

"""Common flags for D3PD making.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class QcdD3PDMakerFlags (JobPropertyContainer):
    """Common flags for QcdD3PD making.
    """

jobproperties.add_Container (QcdD3PDMakerFlags)


class doCollCan (JobProperty):
    """If true, put collision candidate information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doCollCan)

class doElectron (JobProperty):
    """If true, put electron information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doElectron)

class electronLevel (JobProperty):
    """electron level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 2
jobproperties.QcdD3PDMakerFlags.add_JobProperty(electronLevel)

class electronInclude (JobProperty):
    """electron include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(electronInclude)

class electronExclude (JobProperty):
    """electron exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TruthClassification','Truth', 'JetMatch',]
jobproperties.QcdD3PDMakerFlags.add_JobProperty(electronExclude)

class doPhoton (JobProperty):
    """If true, put photon information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doPhoton)

class photonLevel (JobProperty):
    """photon level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10
jobproperties.QcdD3PDMakerFlags.add_JobProperty(photonLevel)

class photonInclude (JobProperty):
    """photon include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['UDTruthFlags']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(photonInclude)

class photonExclude (JobProperty):
    """photon exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TruthClassification','Truth','JetCollection','TrueJetKinematics', 'JetKinematics', 'TruthJetKinematics']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(photonExclude)

class doMuon (JobProperty):
    """If true, put muon information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doMuon)

class muonLevel (JobProperty):
    """muon level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 2
jobproperties.QcdD3PDMakerFlags.add_JobProperty(muonLevel)

class muonInclude (JobProperty):
    """muon include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(muonInclude)

class muonExclude (JobProperty):
    """muon exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['Truth']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(muonExclude)

class doTau (JobProperty):
    """If true, put tau information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTau)

class tauLevel (JobProperty):
    """tau level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
jobproperties.QcdD3PDMakerFlags.add_JobProperty(tauLevel)

class tauInclude (JobProperty):
    """tau include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TrkOO']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(tauInclude)

class tauExclude (JobProperty):
    """tau exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(tauExclude)

class fullJetLevel (JobProperty):
    """fullJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 4
jobproperties.QcdD3PDMakerFlags.add_JobProperty(fullJetLevel)

class slimJetLevel (JobProperty):
    """slimJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3
jobproperties.QcdD3PDMakerFlags.add_JobProperty(slimJetLevel)

class towerJetLevel (JobProperty):
    """towerJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 2
jobproperties.QcdD3PDMakerFlags.add_JobProperty(towerJetLevel)

class jetInclude (JobProperty):
    """jet include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['JetRoIword']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(jetInclude)

class jetExclude (JobProperty):
    """jet exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(jetExclude)

class doTopoJet (JobProperty):
    """If true, put topo jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTopoJet)

class doTopoEMJet (JobProperty):
    """If true, put TopoEM jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTopoEMJet)

class doTopoEMLowPtJet (JobProperty):
    """If true, put TopoEMLowPt jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTopoEMLowPtJet)

class doTowerJet (JobProperty):
    """If true, put tower jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTowerJet)

class doTowerEMJet (JobProperty):
    """If true, put TowerEM jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTowerEMJet)

class doGhostJet (JobProperty):
    """If true, put ghost jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doGhostJet)

class doLCTopoJet (JobProperty):
    """If true, put LCTopo jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doLCTopoJet)

class doLCTopoCBJet (JobProperty):
    """If true, put LCTopoCB jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doLCTopoCBJet)

class doLCTopoLowPtJet (JobProperty):
    """If true, put LCTopoLowPt jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doLCTopoLowPtJet)

class doTrackJet (JobProperty):
    """If true, put Track jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTrackJet)

class doGSCJet (JobProperty):
    """If true, put GSC calibration in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doGSCJet)

class doRecalibJet (JobProperty):
    """If true, apply re-calibartion for the default jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doRecalibJet)

class doCopyJetMoments (JobProperty):
    """If true, copy the jet moments from AOD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doCopyJetMoments)

class doSpecialJet (JobProperty):
    """If true, put Special jet collections in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doSpecialJet)

class doMET (JobProperty):
    """If true, put missing et in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doMET)

class missEtLevel (JobProperty):
    """missEt level (only for not doAdditionalMissingET case)"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10
jobproperties.QcdD3PDMakerFlags.add_JobProperty(missEtLevel)

class missEtInclude (JobProperty):
    """missEt include list (only for not doAdditionalMissingET case)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(missEtInclude)

class missEtExclude (JobProperty):
    """missEt exclude list (only for not doAdditionalMissingET case)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(missEtExclude)

class doAdditionalMissingET (JobProperty):
    """If true, put some additional Missing ET in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doAdditionalMissingET)

class doMissingETGoodness (JobProperty):
    """If true, put MissingETGoodness in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doMissingETGoodness)

class doTrack (JobProperty):
    """If true, put track in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTrack)

class doClusterHad (JobProperty):
    """If true, put CaloCalTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doClusterHad)

class doClusterEM (JobProperty):
    """If true, put CaloTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doClusterEM)

class doCell (JobProperty):
    """If true, put cell in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doCell)

class doCaloTower (JobProperty):
    """If true, put calo towers in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doCaloTower)

class doTopoTower (JobProperty):
    """If true, put topo towers in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTopoTower)

class doMuonSpShower2 (JobProperty):
    """If true, put MuonSpShower in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doMuonSpShower2)

class doEventShape (JobProperty):
    """If true, put EventShape in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doEventShape)

class doMoreTrig (JobProperty):
    """If true, put JetROI and TrigMET"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doMoreTrig)

class doTruthJet (JobProperty):
    """If true, put truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTruthJet)

class truthJetLevel (JobProperty):
    """truthJet include list """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
jobproperties.QcdD3PDMakerFlags.add_JobProperty(truthJetLevel)

class truthJetInclude (JobProperty):
    """truthJet include list """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.QcdD3PDMakerFlags.add_JobProperty(truthJetInclude)

class truthJetExclude (JobProperty):
    """missEt exclude list """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['El02Match','Mu02Match','KineMoments','child_TruthParticleChildAssociation','parent_TruthParticleParentAssociation']
jobproperties.QcdD3PDMakerFlags.add_JobProperty(truthJetExclude)

class doRecJet (JobProperty):
    """If true, put jet reconstreucted on-the-fly."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doRecJet)

class doBTagging (JobProperty):
    """If true, run BTagging """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doBTagging)

class doRecJetOnlyAdditional (JobProperty):
    """If true, make only additional jet with on the fly (not re-reconstruct jets in ESD/AOD)"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doRecJetOnlyAdditional)

class doRecMET (JobProperty):
    """If true, put MET reconstreucted on-the-fly."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doRecMET)

class doRecTruthJet (JobProperty):
    """If true, put additional truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doRecTruthJet)

class doTruthPileupJet (JobProperty):
    """If true, put additional truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doTruthPileupJet)

class doEMJESFix (JobProperty):
    """If true, do EMJESFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doEMJESFix)

class doJVFFix (JobProperty):
    """If true, do JVFFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doJVFFix)

class doGSCFix (JobProperty):
    """If true, do GSCFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doGSCFix)

class doHR (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doHR)

class doSMQCD (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doSMQCD)

class doJETMET (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.QcdD3PDMakerFlags.add_JobProperty(doJETMET)

QcdD3PDMakerFlags = jobproperties.QcdD3PDMakerFlags

def QcdD3PDMakerFlagsSetDefaults():
    flags=QcdD3PDMakerFlags
    flags.doJETMET.set_Value(True)

#setting for physics analysis (no reconstruction for jet/met w/o truth)
def QcdD3PDMakerFlagsSetPhysics():
    flags=QcdD3PDMakerFlags
    flags.doCollCan.set_Value(True)
    flags.doElectron.set_Value(True)
    flags.electronLevel.set_Value(1)
    flags.electronInclude.set_Value(['TrkFitQuality','Vertex'])
    flags.electronExclude.set_Value(['TruthClassification','Truth','JetMatch',
                                     'EFIndex','L2Index','L1Index','EFInfo','EFTrkInfo','L2Info','L1Info'])
    flags.doPhoton.set_Value(True)
    flags.photonLevel.set_Value(1)
    flags.photonInclude.set_Value([])
    flags.photonExclude.set_Value(['TruthClassification','Truth','JetMatch','TrueJetMatch',
                                   'EFIndex','L2Index','L1Index','EFInfo','L2Info','L1Info'])
    flags.doMuon.set_Value(True)
    flags.muonLevel.set_Value(1)
    flags.muonInclude.set_Value(['MuonSpectrometerHitSummary','TrkFitQuality'])
    flags.muonExclude.set_Value(['Truth',
                                 'EFInfo','L2CBInfo','L1Info'])
    flags.doTau.set_Value(True)
    flags.jetExclude.set_Value(['Special','TrackMF'])
    flags.fullJetLevel.set_Value(3)
    flags.slimJetLevel.set_Value(3)
    flags.doTopoEMJet.set_Value(True)
    flags.doTopoEMLowPtJet.set_Value(False)
    flags.doTopoJet.set_Value(False)
    flags.doGhostJet.set_Value(False)
    flags.doLCTopoJet.set_Value(True)
    flags.doLCTopoCBJet.set_Value(False)
    flags.doLCTopoLowPtJet.set_Value(False)
    flags.doTrackJet.set_Value(False)
    flags.doGSCJet.set_Value(False)
    flags.doSpecialJet.set_Value(False)
    flags.doMET.set_Value(True)
    flags.doAdditionalMissingET.set_Value(False)
    flags.missEtLevel.set_Value(0)
    flags.missEtInclude.set_Value(['MET_Calo_Samplings','MET_Topo','MET_LocHadTopo','MET_MuonBoy','MET_RefMuon_Track','MET_Muid','MET_Staco','MET_CellOut','MET_RefEle','MET_RefMuon','MET_Truth_NonInt','MET_Truth_Int','MET_RefJet','MET_RefGamma','MET_LocHadTopoObj','MET_MuonBoy_Track','MET_MuonBoy_Spectro'])
    flags.missEtExclude.set_Value([])
    flags.doMissingETGoodness.set_Value(False)
    flags.doTrack.set_Value(True)
    flags.doClusterHad.set_Value(False)
    flags.doClusterEM.set_Value(False)
    flags.doCell.set_Value(False)
    flags.doMuonSpShower2.set_Value(True)
    flags.doTruthJet.set_Value(True)
    flags.doRecJet.set_Value(False)
    flags.doBTagging.set_Value(False)
    flags.doRecalibJet.set_Value(False)
    flags.doCopyJetMoments.set_Value(False)
    flags.doRecJetOnlyAdditional.set_Value(True)
    flags.doRecMET.set_Value(False)
    flags.doRecTruthJet.set_Value(True)
    flags.doTruthPileupJet.set_Value(True)
    flags.doEMJESFix.set_Value(False)
    flags.doHR.set_Value(False)
    flags.doSMQCD.set_Value(True)

#to force all the jets to be reconstructed in on-the-fly 
def QcdD3PDMakerFlagsSetNewJETMET():
    flags=QcdD3PDMakerFlags
    flags.doRecJet.set_Value(True)
    flags.doRecJetOnlyAdditional.set_Value(False)

#for the calibration sample
def QcdD3PDMakerFlagsSetCalibJETMET():
    flags=QcdD3PDMakerFlags
    flags.doHR.set_Value_and_Lock(False)
    flags.doElectron.set_Value_and_Lock(False)
    flags.doPhoton.set_Value_and_Lock(False)
    flags.doMuon.set_Value_and_Lock(False)
    flags.doTau.set_Value_and_Lock(False)
    flags.doTrackJet.set_Value_and_Lock(False)
    flags.doMET.set_Value_and_Lock(False)
    flags.doTrack.set_Value_and_Lock(False)
    flags.doClusterHad.set_Value_and_Lock(False)
    flags.doMuonSpShower2.set_Value_and_Lock(False)
    flags.doEventShape.set_Value_and_Lock(False)
    flags.doMoreTrig.set_Value_and_Lock(False)
    flags.doRecJet.set_Value(True)
    flags.doRecJetOnlyAdditional.set_Value(False)

#for the "Full" D3PD with Tower jets from DESD_CALJET/PHOJET
def QcdD3PDMakerFlagsSetFull():
    flags=QcdD3PDMakerFlags
    flags.doTowerJet.set_Value(True)
    flags.doTowerEMJet.set_Value(True)

#for the WZ D3PD with TopoGCW jets from DESD_ZEE/ZMUMU, D2ESD_WENU/WMUNU
def QcdD3PDMakerFlagsSetWZ():
    flags=QcdD3PDMakerFlags

#for a special D3PD with EMTopoCluster
def QcdD3PDMakerFlagsSetEMCL():
    flags=QcdD3PDMakerFlags
    flags.doClusterEM.set_Value(True)

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file D3PDMakerConfig/python/CommonD3PDMakerFlags.py
## author Lei Zhou <lei.zhou@cern.ch>
## brief commond flags for Common D3PD
#

"""Flags for Common D3PD making.
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class CommonD3PDMakerFlags (JobPropertyContainer):
    """Flags for Common D3PD making
    """

jobproperties.add_Container (CommonD3PDMakerFlags)

class electronInclude (JobProperty):
    """electron include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(electronInclude)

class electronExclude (JobProperty):
    """electron exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TruthClassification','Truth', 'JetMatch',]
jobproperties.CommonD3PDMakerFlags.add_JobProperty(electronExclude)

class photonInclude (JobProperty):
    """photon include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(photonInclude)

class photonExclude (JobProperty):
    """photon exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TruthClassification','Truth','JetCollection','TrueJetKinematics', 'JetKinematics', 'TruthJetKinematics']
jobproperties.CommonD3PDMakerFlags.add_JobProperty(photonExclude)

class muonInclude (JobProperty):
    """muon include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(muonInclude)

class muonExclude (JobProperty):
    """muon exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['Truth']
jobproperties.CommonD3PDMakerFlags.add_JobProperty(muonExclude)

class doCaloMuon (JobProperty):
    """If true, put calo muon information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doCaloMuon)

class tauInclude (JobProperty):
    """tau include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['TrkOO']
jobproperties.CommonD3PDMakerFlags.add_JobProperty(tauInclude)

class tauExclude (JobProperty):
    """tau exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(tauExclude)

class smallJetLevel (JobProperty):
    """smallJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3
jobproperties.CommonD3PDMakerFlags.add_JobProperty(smallJetLevel)

class smallJetInclude (JobProperty):
    """smallJet include"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(smallJetInclude)

class smallJetExclude (JobProperty):
    """smallJet exclude"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(smallJetExclude)

class towerJetLevel (JobProperty):
    """towerJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 2
jobproperties.CommonD3PDMakerFlags.add_JobProperty(towerJetLevel)

class eljetInclude (JobProperty):
    """electron jet include"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(eljetInclude)    

class largeJetLevel (JobProperty):
    """ largeJet level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 4
jobproperties.CommonD3PDMakerFlags.add_JobProperty(largeJetLevel)

class largeJetInclude (JobProperty):
    """largeJet include"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(largeJetInclude)


class missingEtInclude (JobProperty):
    """missingEt include list (only for not doAdditionalMissingET case)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(missingEtInclude)

class missingEtExclude (JobProperty):
    """missingEt exclude list (only for not doAdditionalMissingET case)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['MET_Phi', 'MET_Et']
jobproperties.CommonD3PDMakerFlags.add_JobProperty(missingEtExclude)

class doHadronicMissingEt (JobProperty):
    """hardonic missing Et"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doHadronicMissingEt)

class hadronicMissingEtLevel (JobProperty):
    """hardronic missing Et level"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
jobproperties.CommonD3PDMakerFlags.add_JobProperty(hadronicMissingEtLevel)

class hadronicMissingEtInclude (JobProperty):
    """hadronic missing Et include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(hadronicMissingEtInclude)

class hadronicMissingEtExclude (JobProperty):
    """hadronic missing Et exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(hadronicMissingEtExclude)

class truthHadronicMissingEtInclude (JobProperty):
    """truth hadronic missing Et include list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(truthHadronicMissingEtInclude)    
    
class doAdditionalMissingET (JobProperty):
    """If true, put some additional Missing ET in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doAdditionalMissingET)

class doMissingETGoodness (JobProperty):
    """If true, put MissingETGoodness in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doMissingETGoodness)    

class doTrack (JobProperty):
    """If true, put track in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTrack)

class doClusterHad (JobProperty):
    """If true, put CaloCalTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doClusterHad)

class doClusterEM (JobProperty):
    """If true, put CaloTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doClusterEM)

class doEventShape (JobProperty):
    """If true, put EventShape in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doEventShape)

class doMuonSpShower2 (JobProperty):
    """If true, put MuonSpShower in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doMuonSpShower2)

class doTruthJet (JobProperty):
    """If true, put truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTruthJet)

class truthJetLevel (JobProperty):
    """truthJet include list """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
jobproperties.CommonD3PDMakerFlags.add_JobProperty(truthJetLevel)

class truthJetInclude (JobProperty):
    """truthJet include list """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []
jobproperties.CommonD3PDMakerFlags.add_JobProperty(truthJetInclude)

class truthJetExclude (JobProperty):
    """truthJet exclude list"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['KineMoments','child_TruthParticleChildAssociation','parent_TruthParticleParentAssociation']
jobproperties.CommonD3PDMakerFlags.add_JobProperty(truthJetExclude)

class doRecJet (JobProperty):
    """If true, put jet reconstreucted on-the-fly."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doRecJet)

class doRecJetOnlyAdditional (JobProperty):
    """If true, make only additional jet with on the fly (not re-reconstruct jets in ESD/AOD)"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doRecJetOnlyAdditional)

class doLCTopoCBJet (JobProperty):
    """If true, put LCTopoCB jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doLCTopoCBJet)    

class doBTagging (JobProperty):
    """If true, run BTagging """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doBTagging)

class doRecMET (JobProperty):
    """If true, put MET reconstreucted on-the-fly."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doRecMET)

class doRecTruthJet (JobProperty):
    """If true, put additional truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doRecTruthJet)

class doJVFFix (JobProperty):
    """If true, do JVFFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doJVFFix)

class doHR (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doHR)

class doSMQCD (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doSMQCD)

class doJETMET (JobProperty):
    """If true, put HadronicRecoil info."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doJETMET)

class doClusterHad (JobProperty):
    """If true, put CaloCalTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doClusterHad)

class doClusterEM (JobProperty):
    """If true, put CaloTopoCluster in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doClusterEM)

class doSoftQCD (JobProperty):
    """if true, write softQCD info into D3PD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doSoftQCD)


# sth QCD group likes to add
class doTruthPileupJet (JobProperty):
    """If true, put additional truth jet in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTruthPileupJet)

class doTrackJet (JobProperty):
    """If true, put Track jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTrackJet)

class doTopoJet (JobProperty):
    """If true, put topo jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTopoJet)

class doTopoEMJet (JobProperty):
    """If true, put TopoEM jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTopoEMJet)

class doTopoEMLowPtJet (JobProperty):
    """If true, put TopoEMLowPt jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTopoEMLowPtJet)

class doTowerJet (JobProperty):
    """If true, put Tower jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTowerJet)

class doTowerEMJet (JobProperty):
    """If true, put TowerEM jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doTowerEMJet)

class doGhostJet (JobProperty):
    """If true, put ghost jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doGhostJet)

class doLCTopoJet (JobProperty):
    """If true, put LCTopo jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doLCTopoJet)

class doLCTopoLowPtJet (JobProperty):
    """If true, put LCTopoLowPt jet information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doLCTopoLowPtJet)

class doRecalibJet (JobProperty):
    """If true, apply re-calibartion for the default jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doRecalibJet)

class doJVFFix (JobProperty):
    """If true, do JVFFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doJVFFix)

class doGSCFix (JobProperty):
    """If true, do GSCFix for original jets"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doGSCFix)

class doCopyJetMoments (JobProperty):
    """If true, copy the jet moments from AOD"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.CommonD3PDMakerFlags.add_JobProperty(doCopyJetMoments)    

# end of stuff for QCD group

CommonD3PDMakerFlags = jobproperties.CommonD3PDMakerFlags

def CommonD3PDMakerFlagsSetDefaults():
    flags = CommonD3PDMakerFlags
    flags.electronInclude.set_Value(['TrkFitQuality','Vertex', 'Isolation_500MeV','MaxEcell','Iso','IsoPtCorrected','PAU','PAUVariables','Pointing','ClusterKin','HadLeakage','Layer1Shape','Layer2Shape','Samplings', 'EMTrackFitDetailsRefit', 'EMTrackFitDetailsRefitCov', 'miniiso10', 'miniiso15'])
    
    flags.photonInclude.set_Value(['MaxEcell','Pointing','EMTrackFitDetailsBrem','EMTrackFitDetailsRefit', 'UDTruthFlags'])

    flags.muonInclude.set_Value(["L1InfoIndex","Isolation_500MeV","Isolation_elstyletracks", "miniiso10", "miniiso15", "ParametersAtPV", "IDTrackDiagonalCovarianceAtPV", "PrimaryTrackDiagonalCovarianceAtPV"])

    flags.tauInclude.set_Value(['TauLeadTrack', 'OtherTrk00', 'OtherTrkTJVA', 'OtherTrkPV', 'OtherIDHits', 'TauCalcVars'])

    from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
    from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
    #flags.smallJetInclude.set_Value([JetTagD3PDKeys.BTagWeightsBlockName(), JetTagD3PDKeys.TruthInfoBlockName(),
    #                                 JetTagD3PDKeys.JetFitterCharmTagInfoBlockName(),
    #                                 JetTagD3PDKeys.SVInfoPlusBlockName(),JetTagD3PDKeys.SV0InfoPlusBlockName(),
    #                                 JetTagD3PDKeys.JetFitterInfoBaseBlockName(),JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
    #                                 JetTagD3PDKeys.SoftMuonInfoMuonBlockName(),JetTagD3PDKeys.JetFitterCharmInfoBaseBlockName(),
    #                                 JetTagD3PDKeys.MultiSVInfoPlusBlockName(),
    #                                 JetTagD3PDKeys.IP2DInfoBaseBlockName(),JetTagD3PDKeys.IP2DIPInfoBaseBlockName(),
    #                                 JetTagD3PDKeys.IP3DInfoBaseBlockName(),JetTagD3PDKeys.IP3DIPInfoBaseBlockName(),
    #                                 JetTagD3PDKeys.SV1InfoBaseBlockName(),JetTagD3PDKeys.SV2InfoBaseBlockName(),
    #                                 JetTagD3PDKeys.JetFitterTagInfoBlockName(),JetTagD3PDKeys.SoftMuonChi2InfoMuonBlockName(),
    #                                 JetTagD3PDKeys.IPInfoPlusTrackAssocBlockName(),JetTagD3PDKeys.IPInfoPlusTrackBlockName(),
    #                                 JetTagD3PDKeys.SVInfoPlusTrackAssocBlockName(),
    #                                 JetTagD3PDFlags.StdInfoInclude(),
    #                                 'Uncertainties','BeamBackground','ConstituentScale','Constituents','Samplings','AssocTrackCont',
    #                                 'JetVertexFraction','JVtx','TracksMoments',
    #                                 'Kinematics','DQMoments','EMFraction','JESMoments','EMScale','CaloQual',
    #                                 'JetRoIword','CaloClusterNavigableConstituentAssociationTool','TrackAssocblockindex','JetLabel',
    #                                 'Special','TrackMF'])
    flags.eljetInclude.set_Value(['AssocTrackCont',
                                  JetTagD3PDKeys.BTagWeightsBlockName(),
                                  'JetVertexFraction', 'JVtx', JetTagD3PDKeys.TruthInfoBlockName()])
    
    #tmpMEtInclude = CommonD3PDMakerFlagsSetMEtInclude()
    #flags.missingEtInclude.set_Value(tmpMEtInclude)

    #tmpMEtExclude = CommonD3PDMakerFlagsSetMEtExclude()
    #flags.missingEtExclude.set_Value(tmpMEtExclude)

    tmpHadronicMEtInclude = CommonD3PDMakerFlagsSetHadronicMEtInclude()
    flags.hadronicMissingEtInclude.set_Value(tmpHadronicMEtInclude)

    tmpTruthHadronicMEtInclude = CommonD3PDMakerFlagsSetTruthHadronicMEtInclude()
    flags.truthHadronicMissingEtInclude.set_Value(tmpTruthHadronicMEtInclude)


def CommonD3PDMakerFlagsSetMEtInclude():

    tmpMissingEtInclude = []
    
    tmpMissingEtInclude += ['MET_MuonBoy','MET_Track','MET_RefGamma','MET_RefEle','MET_RefJet',
                           'MET_RefMuon','MET_RefMuon_Track','MET_RefTau','MET_RefFinal','MET_RefFinal_em',
                           'MET_SoftJets','MET_SoftJets_em','MET_Cryo','MET_CellOut','MET_CellOut_Eflow',
                           'MET_Muid','MET_Calo_Samplings', 'MET_Staco','MET_LocHadTopoObj', #'MET_Truth_Int',
                           'MET_Muon','MET_Final','MET_Topo_Samplings','MET_Topo_Regions',
                           'MET_MuonBoy_Track','MET_MuonBoy_Spectro', #'MET_LocHadTopo','MET_Topo',
                           'MET_LocHadTopo_Samplings','MET_LocHadTopo_Regions',#'MET_Truth_NonInt',
                           'MET_RefMuon_Staco','MET_RefMuon_Muid','MET_RefEle_em','MET_RefJet_em','MET_RefMuon_em',
                           'MET_RefMuon_Track_em','MET_RefGamma_em','MET_RefTau_em','MET_CellOut_em','MET_Muon_Isol_Staco',   # include met_region since here?
                           'MET_Muon_NonIsol_Staco','MET_Muon_Total_Staco','MET_Muon_Isol_Muid','MET_Muon_NonIsol_Muid',
                           'MET_Muon_Total_Muid','MET_RefJet_JVF','MET_RefJet_JVFCut','MET_CellOut_Eflow_STVF',
                           'MET_CellOut_Eflow_JetArea','MET_CellOut_Eflow_JetAreaJVF','MET_CellOut_Eflow_JetAreaRhoEta5JVF',
                           'MET_RefMuon_Track_Staco','MET_RefMuon_Track_Muid','MET_TopoObj',
                           'MET_CellOut_Eflow_JetAreaJVF_LCW_pt20','MET_CellOut_Eflow_JetAreaRhoEta5JVF_LCW_pt20',
                           'MET_RefMuon_LCW_pt20','MET_RefMuon_Muid_LCW_pt20','MET_RefMuon_Track_Muid_LCW_pt20',
                           'MET_CellOut_Eflow_JetAreaJVF_LCW_NI_eflow6','MET_CellOut_Eflow_JetAreaRhoEta5JVF_LCW_NI_eflow6',
                           'MET_RefMuon_LCW_NI_eflow6','MET_RefMuon_Track_Staco_LCW_NI_eflow6','MET_RefEle_em_jetelepp',
                           'MET_RefJet_em_jetelepp','MET_SoftJets_em_jetelepp','MET_CellOut_em_jetelepp',
                           'MET_Muon_Isol_Muid_em_jetelepp','MET_Muon_NonIsol_Muid_em_jetelepp','MET_Muon_Total_Muid_em_jetelepp',
                           'MET_RefGamma_em_jetelepp','MET_RefFinal_em_jetelepp','MET_Muon_Isol_Muons_em_jetelepp',
                           'MET_Muon_NonIsol_Muons_em_jetelepp','MET_Muon_Total_Muons_em_jetelepp','MET_RefMuon_STVF',
                           'MET_RefFinal_STVF_AntiKt4LCTopoJets_tightpp_ExclRefCalib','MET_RefMuon_STVF_top',
                           'METComposition']

    customMETList = ['MET_RefFinal', 'MET_RefGamma', 'MET_RefEle', 'MET_RefTau', 'MET_RefJet', 'MET_CellOut',
                     'MET_SoftJets', 'MET_RefMuon_Staco','MET_Muon_Total_Staco','MET_Muon_Total_Muid',
                     'MET_Muon_Isol_Muid', 'MET_Muon_Isol_Muons', 'MET_Muon_NonIsol_Muons', 'MET_Muon_Total_Muons',
                     'MET_Cryo', 'MET_RefJet_JVF', 'MET_RefJet_JVFCut', 'MET_CellOut_Eflow_STVF',
                     'MET_Muon_NonIsol_Muid', 'MET_CellOut_Eflow_JetArea', 'MET_RefMuon_STVF',
                     'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'Muon_Total_Staco',
                     'MET_RefMuon_Track', 'MET_RefMuon_Track_StacoMET_CellOut','MET_CellOut_Track',
                     'MET_CellOut_TrackPV', 'MET_CellOutCorr']
    customMETConf = ['BDTMedium','AntiKt4LCTopo_tightpp', 'AntiKt4LCTopoJets_tightpp',
                     'AntiKt4LCTopoJets_tightpp_photonTightAR', 'AntiKt4LCTopoJets_mediumpp_photonTightAR',
                     'AntiKt4LCTopoJets_tightpp_photonLooseAR', 'AntiKt4LCTopoJets_mediumpp',
                     'AntiKt4LCTopoJets_loosepp', 'AntiKt4LCTopoJets_tightpp_ExclRefCalib',
                     'STVF', 'STVF_top', 'LCW_NI_eflow6', 'LCW_pt20', 'STVF_LCW_pt20', 'STVF_LCW_NI_eflow6']
    for conf in customMETConf:
        for item in customMETList:
            if conf == 'STVF' and item == 'MET_RefFinal':  # avoid duplicate ?
                continue
            pass
            tmpMissingEtInclude += [item+'_'+conf]

    custom_parts_prefix = ['MET_RefGamma', 'MET_RefFinal', 'MET_RefJet', 'MET_RefEle', 'MET_Muon_Total_Staco',
                    'MET_Muon_Isol_Staco', 'MET_Muon_NonIsol_Staco', 'MET_CellOut','MET_Muon_Isol_Muid',
                    'MET_Muon_NonIsol_Muid', 'MET_Muon_Total_Muid', 'MET_Track', 'MET_SoftJets',
                    'MET_RefMuon','MET_MuonBoy', 'MET_CellOut_Track','MET_CellOut_TrackPV','MET_CellOutCorr']
    
    custom_parts_suffix = ['_Simplified20', '_Egamma10NoTau_STVF', '_LooseEgamma10NoTau', #'_Egamma10NoTau',
                           '_Egamma10NoTauTightPhotonRef', #'_Egamma10NoTauLoosePhotonRef',
                           '_Egamma10NoTauLoosePhotonEm', '_LooseEgamma10NoTau_STVF',
                           '_Egamma10NoTauTightPhotonRef_STVF', '_Egamma10NoTauLoosePhotonRef_STVF',
                           '_Egamma10NoTauLoosePhotonEm_STVF']
    for custom_pre in custom_parts_prefix:
        for custom_suf in custom_parts_suffix:
            tmpMissingEtInclude += [custom_pre + custom_suf]

    #custom_parts_pre_2 = ['MET_Egamma10LooseTau', 'MET_LooseEgamma10NoTauLoosePhotonRef']  # from SUSY 
    #custom_parts_ref_2 = ['RefGamma', 'RefFinal', 'RefJet', 'RefEle', 'Muon_Total_Staco',
    #                      'Muon_Isol_Staco', 'Muon_NonIsol_Staco', 'CellOut', 'CellOut_Eflow', 'Muon_Isol_Muid',
    #                      'Muon_NonIsol_Muid', 'Muon_Total_Muid', 'Track','RefFinal_STVF', 'CellOut_Eflow_STVF',
    #                      'CellOut_Eflow_JetArea', 'RefJet_JVF', 'RefJet_JVFCut', 'CellOut_Eflow_JetAreaJVF',
    #                      'CellOut_Eflow_JetAreaRhoEta5JVF', 'SoftJets', 'RefMuon', 'MuonBoy', 'RefMuon_Staco',
    #                      'RefMuon_Muid', 'RefMuon_Track', 'RefMuon_Track_Staco', 'RefMuon_Track_Muid',
    #                      'RefTau']
    #for pre_2 in custom_parts_pre_2:
    #    for ref_2 in custom_parts_ref_2:
    #        tmpMissingEtInclude += [pre_2 + '_' + ref_2]

    customTopMET_pre = ["MET_RefEle","MET_RefJet","MET_SoftJets","MET_CellOut","MET_Muon_Isol_Muid",
                    "MET_Muon_NonIsol_Muid","MET_Muon_Total_Muid","MET_RefGamma","MET_RefFinal",
                    "MET_Muon_Isol_Muons","MET_Muon_NonIsol_Muons","MET_Muon_Total_Muons"]
    customTopMET_suf = ['_em_tightpp','_em_mediumpp','_em_loosepp','_em_jetele','_em_jetelepp'
                        '_em_tightpp_photonTightAR','_em_mediumpp_photonTightAR',
                        '_em_tightpp_photonLooseAR']
    for top_pre in customTopMET_pre:
        for top_suf in customTopMET_suf:
            tmpMissingEtInclude += [top_pre + top_suf]

    
    return tmpMissingEtInclude


def CommonD3PDMakerFlagsSetMEtExclude():
    tmpMissingEtExclude = []
    tmpMissingEtExclude += ['MET_RefFinal_Comps','MET_Calib_Comps','MET_CellOut_Comps',
                            'MET_CorrTopo_Comps','MET_Cryo_Comps','MET_CryoCone_Comps',
                            'MET_Final_Comps','MET_LocHadTopo_Comps',
                            'MET_LocHadTopoObj_Comps','MET_Muid_Comps',
                            'MET_Muid_Spectro_Comps','MET_Muid_Track_Comps',
                            'MET_MuonBoy_Comps','MET_MuonBoy_Spectro_Comps',
                            'MET_MuonBoy_Track_Comps','MET_MuonMuid_Comps',
                            'MET_Muon_Comps','MET_Muon_Isol_Muid_Comps',
                            'MET_Muon_Isol_Staco_Comps','MET_Muon_NonIsol_Muid_Comps'
                            'MET_Muon_NonIsol_Staco_Comps','MET_Muon_Total_Muid_Comps',
                            'MET_Muon_Total_Staco_Comps','MET_RefEle_Comps',
                            'MET_RefEle_em_Comps','MET_RefGamma_Comps',
                            'MET_RefGamma_em_Comps','MET_RefJet_Comps',
                            'MET_RefJet_em_Comps','MET_RefMuon_Comps',
                            'MET_RefMuon_Muid_Comps','MET_RefMuon_Staco_Comps',
                            'MET_RefMuon_Track_Muid_Comps','MET_RefMuon_Track_Staco_Comps',
                            'MET_RefMuon_Track_em_Comps','MET_RefMuon_Track_Comps',
                            'MET_RefMuon_em_Comps','MET_RefTau_Comps',
                            'MET_RefTau_em_Comps','MET_SoftJets_Comps',
                            'MET_SoftJets_em_Comps','MET_Topo_Comps',
                            'MET_TopoObj_Comps','MET_Track_Comps','MET_Comps']

    return tmpMissingEtExclude




def CommonD3PDMakerFlagsSetHadronicMEtInclude():
    sgkeyList = ['RoughRecoil_15', 'ueCorrection_15', 'RefinedRecoil_15', 'MET_HR_15',
                 'RoughRecoil_Eflow_15', 'ueCorrection_Eflow_15', 'RefinedRecoil_Eflow_15', 'MET_HR_Eflow_15',
                 'RoughRecoil_track_15', 'ueCorrection_track_15', 'RefinedRecoil_track_15', 'MET_HR_track_15',
                 'RoughRecoil_20', 'ueCorrection_20', 'RefinedRecoil_20', 'MET_HR_20',
                 'RoughRecoil_track_20', 'ueCorrection_track_20', 'RefinedRecoil_track_20', 'MET_HR_track_20',
                 'RoughRecoil_clusNoTrack_20', 'ueCorrection_clusNoTrack_20', 'RefinedRecoil_clusNoTrack_20', 'MET_HR_clusNoTrack_20',
                 'RoughRecoil_Eflow_20', 'ueCorrection_Eflow_20', 'RefinedRecoil_Eflow_20', 'MET_HR_Eflow_20',
                 'RoughRecoil_25', 'ueCorrection_25', 'RefinedRecoil_25', 'MET_HR_25',
                 'RoughRecoil_Eflow_25', 'ueCorrection_Eflow_25', 'RefinedRecoil_Eflow_25', 'MET_HR_Eflow_25',
                 'RoughRecoil_track_25', 'ueCorrection_track_25', 'RefinedRecoil_track_25', 'MET_HR_track_25']
    return sgkeyList


def CommonD3PDMakerFlagsSetTruthHadronicMEtInclude():
    truthSgkeyList = ['RoughRecoil_TruthLep_15', 'ueCorrection_TruthLep_15', 'RefinedRecoil_TruthLep_15',
                      'RoughRecoil_TruthLep_Eflow_15', 'ueCorrection_TruthLep_Eflow_15', 'RefinedRecoil_TruthLep_Eflow_15',
                      'RoughRecoil_TruthLep_track_15', 'ueCorrection_TruthLep_track_15', 'RefinedRecoil_TruthLep_track_15',
                      'RoughRecoil_TruthLep_20', 'ueCorrection_TruthLep_20', 'RefinedRecoil_TruthLep_20',
                      'RoughRecoil_TruthLep_Eflow_20', 'ueCorrection_TruthLep_Eflow_20', 'RefinedRecoil_TruthLep_Eflow_20',
                      'RoughRecoil_TruthLep_track_20', 'ueCorrection_TruthLep_track_20', 'RefinedRecoil_TruthLep_track_20',
                      'RoughRecoil_TruthLep_25', 'ueCorrection_TruthLep_25', 'RefinedRecoil_TruthLep_25',
                      'RoughRecoil_TruthLep_Eflow_25', 'ueCorrection_TruthLep_Eflow_25', 'RefinedRecoil_TruthLep_Eflow_25',
                      'RoughRecoil_TruthLep_track_25', 'ueCorrection_TruthLep_track_25', 'RefinedRecoil_TruthLep_track_25']
    return truthSgkeyList










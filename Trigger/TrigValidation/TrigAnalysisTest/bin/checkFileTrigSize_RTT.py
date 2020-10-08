#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#################################################################
# a script to grab the trigger information from checkfile.py output
# Fabrizio Salvatore - University of Sussex - May 2009
# Edit - July 2013 - Rodger Mantifel - get trigger category from TriggerEDM.py
# Edit - Oct 2018 - Catrin Bernius
#################################################################

import sys;
import string;
import os;
import math;
import re;
from TrigEDMConfig.TriggerEDM import *



# 'Counter' just contains the list of classes you want to include and the size
class Counter:
    size = 0
    def __init__(self,myname,mylist):
        self.name = myname
        self.itemList = mylist

       
class checkFileTrigSize_RTT:
    """
    Class to calculate total trigger AOD size
    """

    def __init__(self, paramDict={}):

        try: 
            from Logger import Logger
            self.logger    = Logger()
        except:
           print("can't import logger in test mode")
    
      
        self.success   =  0
        self.error     = -1

        self.checkFile = paramDict.get('checkFile', 'AOD.pool.root.checkFile')
        self.triggerAlgList = []
        self.triggerAlgListNotIncluded = []
        self.nontriggerAlgList = []
        self.triggerAlgSize = {}
        self.totalAlgSize = 0.0
        self.totalAlgSizeInLists = 0.0
        self.total = 0.0

    def run(self): 

#sort all classes into 'Counters'
        atlfastList = [
            'ElectronContainer_p1_AtlfastElectronCollection',
            'INav4MomAssocs_p2_AtlfastMcAodAssocs',
            'MuonContainer_p3_AtlfastMuonCollection',
            'MuonContainer_p3_AtlfastNonIsoMuonCollection',
            'ParticleJetContainer_p1_AtlfastParticleJetContainer',
            'PhotonContainer_p1_AtlfastPhotonCollection',
            'Rec::TrackParticleContainer_tlp1_AtlfastTrackParticles',
            'TauJetContainer_p1_AtlfastTauJet1p3pContainer',
            'TauJetContainer_p1_AtlfastTauJetContainer',
            'JetCollection_p2_AtlfastJetContainer',
            'JetCollection_tlp5_AtlfastJetContainer',
            'TauJetContainer_p2_AtlfastTauJetContainer',
            'TauJetContainer_p2_AtlfastTauJet1p3pContainer',
            'TauJetContainer_p3_AtlfastTauJet1p3pContainer',
            'TauJetContainer_p3_AtlfastTauJetContainer',
            'JetCollection_tlp2_AtlfastJetContainer',
            'MuonContainer_p4_AtlfastNonIsoMuonCollection',
            'MuonContainer_p4_AtlfastMuonCollection',
            'egDetailContainer_p1_AtlfastIsoEleShowerContainer',
            'egDetailContainer_p1_AtlfastEleShowerContainer',
            'ElectronContainer_p1_AtlfastIsoElectronCollection',
            'MuonContainer_p4_AtlfastIsoMuonCollection',
            'egDetailContainer_p1_AtlfastIsoPhoShowerContainer',
            'PhotonContainer_p1_AtlfastIsoPhotonCollection',
            'egDetailContainer_p1_AtlfastPhoShowerContainer',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationCombined_Electron_EFID',  
      
            
            #    'Rec::TrackParticleTruthCollection_p1_IPatTrackParticleTruthCollection',
            #    'Rec::TrackParticleTruthCollection_p1_XKalTrackParticleTruthCollection',
            #    'POOLContainer_Rec::TrackParticleTruthCollection_p1',
            #    'McEventCollection_p3_GEN_AOD',
            #    'JetCollection_tlp2_Cone4TruthJets',
            #    'JetCollection_tlp2_Cone4H1TopoJets',
        ]
        atlfastCounter = Counter('atlfast',atlfastList)
        
        caloList = [
            'CaloClusterContainer_p2_CaloCalTopoCluster',
            'CaloClusterContainer_p2_CombinedCluster',
            'CaloClusterContainer_p2_EMTopoCluster',
            'CaloClusterContainer_p2_LArClusterEMSofte',
            'CaloClusterContainer_p3_CaloCalTopoCluster',
            'CaloClusterContainer_p3_EMTopoCluster',
            'CaloClusterContainer_p3_LArClusterEMSofte',
            'CaloClusterContainer_p3_MuonClusterCollection',
            'CaloClusterContainer_p4_CaloCalTopoCluster',
            'CaloClusterContainer_p4_EMTopoCluster',
            'CaloClusterContainer_p4_LArClusterEMSofte',
            'CaloClusterContainer_p5_Tau1P3PCellEM012ClusterContainer',
            'CaloClusterContainer_p5_Tau1P3PCellCluster',
            'CaloClusterContainer_p5_LArClusterEMFrwd',
            'CaloClusterContainer_p5_Tau1P3PPi0ClusterContainer',
            'CaloClusterContainer_p5_MuonClusterCollection',
            'CaloClusterContainer_p5_egClusterCollection',
            'CaloClusterContainer_p5_LArClusterEMSofte',
            'CaloClusterContainer_p5_EMTopoCluster430',
            'CaloClusterContainer_p5_CaloCalTopoCluster',
            'CaloClusterContainer_p7_LArClusterEMFrwd',
            'CaloClusterContainer_p7_LArClusterEMSofte',
            'CaloClusterContainer_p7_egClusterCollection',
            'CaloClusterContainer_p7_LArClusterEMSofte',
            'CaloClusterContainer_p7_egClusterCollection',
            'CaloClusterContainer_p7_EMTopoCluster430',
            'LArNoisyROSummary_p1_LArNoisyROSummary',
            'CaloClusterContainer_p7_CaloCalTopoCluster',
            'TileCellVec_MBTSContainer',
            'TileContainer<TileMu>_TileMuObj',

            #    'CaloCompactCellContainer_AODCellContainer',
            #    'CaloClusterContainer_p4_Tau1P3PPi0ClusterContainer',
            #    'CaloClusterContainer_p4_MuonClusterCollection',
            #    'CaloClusterContainer_p4_LArClusterEMSofte',
            #    'CaloClusterContainer_p4_egClusterCollection',
            #    'CaloClusterContainer_p4_CaloCalTopoCluster',
            #    'Trk::TrackCollection_tlp2_Tracks',
        ]
        caloCounter = Counter('calo',caloList)
    
        egammaList = [
            'CaloCellLinkContainer_p2_egClusterCollection_Link',
            'CaloClusterContainer_p2_egClusterCollection',
            'CaloClusterContainer_p3_egClusterCollection',
            'CaloClusterContainer_p4_egClusterCollection',
            'egDetailContainer_p1_egDetailAOD',
            'egDetailContainer_p2_egDetailAOD',
            'ElectronContainer_p1_ElectronAODCollection',
            'PhotonContainer_p1_PhotonAODCollection',
            'PhotonContainer_p4_PhotonAODCollection',
            'CaloClusterContainer_p4_EMTopoCluster430',
            'CaloClusterContainer_p4_LArClusterEMFrwd',
            'ElectronContainer_p4_ElectronAODCollection',
            'TrigPassFlagsCollection_p1_HLT_isEM',
            'TrigPassFlagsCollection_p1_HLT_passflags', 
      #    'CaloClusterContainer_p2_LArClusterEMSofte',
        ]
        egammaCounter = Counter('egamma',egammaList)
    
        eventinfoList = [
            'DataHeader',
            'EventInfo_p2_ByteStreamEventInfo',
            'EventInfo_p2_McEventInfo',
            'EventStreamInfo_p1_StreamAOD',
            'IOVMetaDataContainer_p1__GLOBAL_DETSTATUS_LBSUMM',
            'LumiBlockCollection_p1_LumiBlocks',
            'MetaData',
            'MetaDataHdrDataHeader',
            'PileUpEventInfo_p2_OverlayEvent',
            'EventInfo_p2_MyEvent',
            'IOVMetaDataContainer_p1__Simulation_Parameters',
            'IOVMetaDataContainer_p1__Digitization_Parameters',
            'BCM_RDO_Container_p0_BCM_CompactDOs',
            'MetaDataHdrDataHeaderForm',
            'EventBookkeeperCollection_p1_EventSelector.Counter',
            'IOVMetaDataContainer_p1__LAR_LArAffectedRegionInfo',
            'EventStreamInfo_p2_StreamAOD',
            'RawInfoSummaryForTag_p1_RawInfoSummaryForTag',
            'EventInfo_p3_McEventInfo',
            'POOLContainer_DataHeaderForm',
            'IOVMetaDataContainer_p1__EXT_DCS_MAGNETS_SENSORDATA',
            'IOVMetaDataContainer_p1__GLOBAL_BField_Map',
            'IOVMetaDataContainer_p1__TagInfo',
      
        ]
        eventinfoCounter = Counter('eventinfo',eventinfoList)
        
        emtList = [
            'CaloCompactCellContainer_AODCellContainer',
            'CaloCompactCellContainer_AllCalo',
            'InDet::Track_tlp1',
            'TPCnv::MuonMeasurements_tlp1',
            'Trk::TrackCollection_tlp1_egammaSlimmedTrackCollection',
            'Trk::TrackCollection_tlp1_MuonSlimmedTrackCollection',
            'Trk::TrackCollection_tlp2_Tracks',
            'Trk::TrackCollection_tlp3_Tracks',

            #    'CaloClusterContainer_p4_Tau1P3PCellCluster',
            #    'CaloClusterContainer_p4_Tau1P3PPi0ClusterContainer',
            #    'CaloClusterContainer_p4_MuonClusterCollection',
            #    'CaloClusterContainer_p4_LArClusterEMSofte',
            #    'CaloClusterContainer_p4_egClusterCollection',
            #    'CaloClusterContainer_p4_EMTopoCluster430',
            #    'CaloClusterContainer_p4_CaloCalTopoCluster',
        ]
        emtCounter = Counter('emt',emtList)
    
        indetList = [
        'Rec::TrackParticleContainer_tlp1_TrackParticleCandidate',
            'Trk::MVFVxContainer_tlp1',
            'Trk::V0Container_tlp1_V0Candidates',
            'Trk::VxContainer_tlp1_ConversionCandidate',   
            'Trk::VxContainer_tlp1_Conversions',
            'Trk::VxContainer_tlp1_VxPrimaryCandidate',
            'Trk::VxContainer_tlp1_SecVertices',
            'Trk::VxContainer_tlp1_AllPhotonsVxCandidates',
            'InDetLowBetaContainer_tlp1_InDetLowBetaCandidates',
         #    'POOLContainer_Trk::VxContainer_tlp1',
            #    'Trk::VxContainer_tlp1_IPatVxPrimaryCandidate',
            #    'Trk::VxContainer_tlp1_XKalVxPrimaryCandidate',
            #    'Rec::TrackParticleContainer_tlp1_XKalTrackParticleCandidate',
            #    'Rec::TrackParticleContainer_tlp1_IPatTrackParticleCandidate',
        ]
        indetCounter = Counter('indet',indetList)
    
        jetList = [
            'ParticleJetContainer_p1_Cone4H1TopoParticleJets',
            'ParticleJetContainer_p1_Cone4H1TowerParticleJets',
            'ParticleJetContainer_p1_Cone7H1TopoParticleJets',
            'ParticleJetContainer_p1_Cone7H1TowerParticleJets',
            'ParticleJetContainer_p1_Kt4H1TopoParticleJets',
            'ParticleJetContainer_p1_Kt4H1TowerParticleJets',
            'ParticleJetContainer_p1_Kt6H1TopoParticleJets',
            'ParticleJetContainer_p1_Kt6H1TowerParticleJets',
            'JetCollection_p2_Cone4H1TopoJets',
            'JetCollection_p2_Cone4H1TowerJets',
            'JetCollection_p2_Cone7H1TopoJets',
            'JetCollection_p2_Cone7H1TowerJets',
            'JetCollection_p2_Kt4H1TopoJets',
            'JetCollection_p2_Kt4H1TowerJets',
            'JetCollection_p2_Kt6H1TopoJets',
            'JetCollection_p2_Kt6H1TowerJets',
            'JetKeyDescriptor_p1_JetKeyMap',
            'JetCollection_tlp2_Cone4H1TopoJets',
            'JetCollection_tlp2_Cone7H1TopoJets',
            'JetCollection_tlp2_Cone4H1TowerJets',
            'JetCollection_tlp2_Cone7H1TowerJets',
            'JetCollection_tlp2_Kt4H1TopoJets',
            'JetCollection_tlp2_Kt4H1TowerJets',
            'JetCollection_tlp2_Kt6H1TopoJets',
            'JetCollection_tlp2_Kt6H1TowerJets',
            'Analysis::JetTagInfo_tlp1',
            'JetCollection_tlp5_Cone4TruthJets',
            'JetCollection_tlp5_Cone4H1TopoJets',
            'JetCollection_tlp5_Cone4H1TowerJets',
            'JetCollection_tlp5_Cone7H1TowerJets',

            'JetCollection_tlp5_AntiKt6H1TopoJets',
            'JetCollection_tlp5_AntiKt6LCTopoJets',
            'JetCollection_tlp5_AntiKt4H1TopoJets',
            'JetCollection_tlp5_AntiKt4H1TowerJets',
            'JetCollection_tlp5_AntiKt6H1TowerJets',

            'JetMomentMap_p6_TrigJetRecMomentMap',
            #    'POOLContainer_JetCollection_p1',
            #    'JetCollection_tlp2_Cone4H1TowerJets',
            #    'JetCollection_tlp2_Cone7H1TowerJets',
            ]
        jetCounter = Counter('jet',jetList)
    
        jtList = [
        'ParticleJetContainer_p1_Cone4TruthParticleJets',
            'ParticleJetContainer_p1_Cone7TruthParticleJets',
            'ParticleJetContainer_p1_Kt4TruthParticleJets',
            'ParticleJetContainer_p1_Kt6TruthParticleJets',
            'JetCollection_p2_JetTruthParticles',
            'JetCollection_p2_JetTruthPileupParticles',
            'JetCollection_p2_Cone4TruthJets',
            'JetCollection_p2_Cone7TruthJets',
            'JetCollection_p2_Cone4TruthPileupJets',
            'JetCollection_p2_Cone7TruthPileupJets',
            'JetCollection_p2_Kt4TruthJets',
            'JetCollection_p2_Kt6TruthJets',
            'JetCollection_p2_Kt4TruthPileupJets',
            'JetCollection_p2_Kt6TruthPileupJets',
            'JetCollection_tlp2_Cone4TruthJets',
            'JetCollection_tlp2_Cone7TruthJets',
            'JetCollection_tlp2_Kt4TruthJets',
            'JetCollection_tlp2_Kt6TruthJets',

            #    'POOLContainer_JetCollection_p1',
            #    'JetCollection_tlp2_Cone4H1TowerJets',
            #    'JetCollection_tlp2_Cone7H1TowerJets',
            #    'Analysis::JetTagInfo_tlp1',
         ]
        jtCounter = Counter('jt',jtList)

        metList = [
            'MissingET_p1_MET_CellOut_MiniJet',
            'MissingET_p1_AtlfastMissingEt',
            'MissingET_p1_MET_CellOut',
            'MissingET_p1_MET_Cryo',
            'MissingET_p1_MET_CryoCone',
            'MissingET_p1_MET_DM_All',
            'MissingET_p1_MET_DM_Crack1',
            'MissingET_p1_MET_DM_Crack2',
            'MissingET_p1_MET_DM_Cryo',
            'MissingET_p1_MET_Final',
            'MissingET_p1_MET_LocHadTopoObj',
            'MissingET_p1_MET_Muon',
            'MissingET_p1_MET_MuonBoy',
            'MissingET_p1_MET_RefEle',
            'MissingET_p1_MET_RefFinal',
            'MissingET_p1_MET_RefGamma',
            'MissingET_p1_MET_RefJet',
            'MissingET_p1_MET_RefMuon',
            'MissingET_p1_MET_RefTau',
            'MissingET_p1_MET_TopoObj',
            'MissingET_p1_ObjMET_Elec',
            'MissingET_p1_ObjMET_Final',
            'MissingET_p1_ObjMET_IdTrk',
            'MissingET_p1_ObjMET_Jet',
            'MissingET_p1_ObjMET_MiniJet',
            'MissingET_p1_ObjMET_Muon',
            'MissingET_p1_ObjMET_Rest',
            'MissingET_p1_ObjMET_TauJet',
            'MissingEtCalo_p1_MET_Base',
            'MissingEtCalo_p1_MET_Calib',
            'MissingEtCalo_p1_MET_CorrTopo',
            'MissingEtCalo_p1_MET_LocHadTopo',
            'MissingEtCalo_p1_MET_Topo',
            'MissingETSig_p1_METSig',
            'MissingETSigObjContainer_p1_EtMissObjCollection',
            'MissingEtTruth_MET_Truth',
            'MissingET_p1_MET_RefMuon_Track',
            'MissingET_p1_MET_MuonBoy_Spectro',
            'MissingET_p1_MET_MuonBoy_Track',
            'MissingETSigHypoContainer_p1_EtMissHypoCollection',
            'MissingEtCalo_p1_MET_Base0',
            'MissingET_p2_MET_RefTau',
            'MissingET_p2_MET_RefGamma',
            'MissingET_p2_MET_MuonBoy',
            'MissingET_p2_MET_MuonBoy_Track',
            'MissingET_p2_MET_MuonBoy_Spectro',
            'MissingET_p2_MET_RefEle',
            'MissingET_p2_MET_CryoCone',
            'MissingET_p2_MET_Cryo',
            'MissingET_p2_MET_Muon',
            'MissingET_p2_MET_DM_Cryo',
            'MissingET_p2_MET_TopoObj',
            'MissingET_p2_MET_DM_All',
            'MissingET_p2_MET_LocHadTopoObj',
            'MissingET_p2_MET_RefJet',
            'MissingET_p2_MET_RefFinal',
            'MissingET_p2_MET_Final',
            'MissingET_p2_MET_DM_Crack2',
            'MissingET_p2_AtlfastMissingEt',
            'MissingET_p2_MET_CellOut',
            'MissingET_p2_MET_DM_Crack1',
            'MissingEtCalo_p2_MET_Base0',
            'MissingEtCalo_p2_MET_Base',
            'MissingEtCalo_p2_MET_Calib',
            'MissingEtCalo_p2_MET_LocHadTopo',
            'MissingEtCalo_p2_MET_Topo',
            'MissingEtCalo_p2_MET_CorrTopo',
            'MissingET_p3_MET_RefGamma'
            'MissingET_p3_MET_RefMuon',
            'MissingET_p3_MET_RefEle',
            'MissingET_p3_MET_RefMuon_Track',
            'MissingET_p3_MET_CryoCone',
            'MissingET_p3_MET_Muid_Track',
            'MissingET_p3_MET_MuonBoy_Track',
            'MissingET_p3_MET_Cryo',
            'MissingET_p3_MET_TopoObj',
            'MissingET_p3_MET_DM_All',
            'MissingET_p3_MET_DM_Crack2',
            'MissingET_p3_MET_Track',
            'MissingET_p3_MET_DM_Cryo',
            'MissingET_p3_MET_LocHadTopoObj',
            'MissingET_p3_MET_DM_Crack1',
            'MissingET_p3_MET_MuonBoy_Spectro',
            'MissingET_p3_MET_RefTau',
            'MissingET_p3_MET_MuonBoy',
            'MissingET_p3_MET_Muid',
            'MissingET_p3_MET_Muid_Spectro',
            'MissingET_p3_MET_Muon',
            'MissingET_p3_MET_MuonMuid',
            'MissingET_p3_MET_Final',
            'MissingET_p3_MET_RefJet',
            'MissingET_p3_MET_CellOut_Eflow',
            'MissingET_p3_MET_CellOut',
            'MissingET_p3_MET_RefFinal',
            'MissingET_p3_MET_RefGamma',
            'MissingET_p3_MET_RefMuon',
            'MissingEtCalo_p3_MET_Base0',
            'MissingEtCalo_p3_MET_Calib',
            'MissingEtCalo_p3_MET_Base',
            'MissingEtCalo_p3_MET_Topo',
            'MissingEtCalo_p3_MET_LocHadTopo',
            'MissingEtCalo_p3_MET_CorrTopo',
            'MissingETComposition_p1_MET_RefComposition',

            #    'MissingETSigObjContainer_PostJetObjCollection',
            #    'MissingETSigObjContainer_PreJetObjCollection',
        ]
        metCounter = Counter('met',metList)
        
        muonList = [
            'CaloCellLinkContainer_p2_MuonClusterCollection_Link',
            'CaloClusterContainer_p2_MuonClusterCollection',
            'CaloClusterContainer_p4_MuonClusterCollection',
            'MuonCaloEnergyContainer_p1_MuonCaloEnergyCollection',
            'MuonContainer_p1_CaloMuonCollection',
            'MuonContainer_p1_MuidMuonCollection',
            'MuonContainer_p1_StacoMuonCollection',
            'MuonContainer_p3_CaloMuonCollection',
            'MuonContainer_p3_MuidMuonCollection',
            'MuonContainer_p3_StacoMuonCollection',
            'MuonContainer_p4_CaloMuonCollection',
            'MuonContainer_p4_MuidMuonCollection',
            'MuonContainer_p4_StacoMuonCollection',
            'MuonContainer_p6_CaloMuonCollection',
            'MuonContainer_p6_StacoMuonCollection',
            'MuonContainer_p6_MuidMuonCollection',
            'MuonContainer_p6_MuGirlLowBetaCollection',
            'MuonSpShowerContainer_p1_MuonSpShowers',
            'POOLContainer_MuonFeature_p1',
            'Rec::TrackParticleContainer_tlp1_MooreTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuidCombTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuidExtrTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuonboyMuonSpectroOnlyTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuonboyTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuTagTrackParticles',
            'Rec::TrackParticleContainer_tlp1_StacoTrackParticles',
            'Trk::SegmentCollection_tlp1_ConvertedMBoySegments',
            'Trk::SegmentCollection_tlp1_MooreSegments',
            'Trk::SegmentCollection_tlp1_MooreSegments_',
            'Trk::SegmentCollection_tlp1_MuGirlSegments',
            'Trk::TrackCollection_tlp2_MuonSlimmedTrackCollection',
            'Rec::TrackParticleContainer_tlp1_MuGirlRefittedTrackParticles',
            'Rec::TrackParticleContainer_tlp1_MuTagIMOTrackParticles',
            'TileMuContainer_p1_TileMuObj',    
            'MuonContainer_p4_MuGirlLowBetaCollection',
            'Trk::TrackCollection_tlp3_MuonSlimmedTrackCollection',
            'MuonCaloEnergyContainer_tlp1_MuonCaloEnergyCollection',
            'Muon::ChamberT0s_p1_MboyMuonChamberT0s',   
            'Muon::ChamberT0s_p1_MooreMuonChamberT0s',
            'CaloClusterContainer_p7_MuonClusterCollection',
            #    'MuonContainer_p1_AtlfastMuonCollection',
            #    'Rec::MuonSpShowerContainer_MuonSpShowers',
            ]
        muonCounter = Counter('muon',muonList)
    
        tauList = [
            'CaloClusterContainer_p2_Tau1P3PPi0ClusterContainer',
            'CaloClusterContainer_p3_Tau1P3PPi0ClusterContainer',
            'CaloClusterContainer_p4_Tau1P3PPi0ClusterContainer',
            'TauDetailsContainer_tlp1_Tau1P3PDetailsContainer',
            'TauDetailsContainer_tlp1_TauRecDetailsContainer',
            'TauDetailsContainer_tlp2_TauRecDetailsContainer',
            'TauJetContainer_p1_Tau1P3PContainer',
            'TauJetContainer_p1_TauRecContainer',
            'TauJetContainer_p2_TauRecContainer',
            'TauJetContainer_p3_TauRecContainer',
            'TauJetContainer_p4_TauRecContainer',
            'CaloClusterContainer_p4_Tau1P3PCellCluster',
            'CaloClusterContainer_p4_Tau1P3PCellEM012ClusterContainer',
            'CaloClusterContainer_p7_Tau1P3PCellEM012ClusterContainer',
            'CaloClusterContainer_p7_Tau1P3PCellCluster',
            'CaloClusterContainer_p7_Tau1P3PPi0ClusterContainer',
            ]
        tauCounter = Counter('tau',tauList)

		
# trigger lists

        triggerListBjet = [
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Bjet_EFID',
            'TrigEFBjetContainer_tlp2_HLT_EFBjetFex',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Jet',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Jet',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Jet',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Jet',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Jet',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Jet',
        'TrigInDetTrackCollection_tlp3_HLT_TrigL2SiTrackFinder',
            'TrigL2BjetContainer_tlp2_HLT_L2BjetFex',
            'TrigL2BjetContainer_p3_HLT_L2BjetFex',
            'TrigVertexCollection_tlp1_HLT_T2HistoPrmVtx',
        'TrigVertexCollection_tlp1_HLT_TrigBeamSpotVertex',
        'TrigVertexCollection_tlp1_HLT_TrigBeamSpotVertex_SiTrack',
        'TrigVertexCollection_tlp2_HLT_T2HistoPrmVtx',
        'TrigVertexCollection_tlp2_HLT_TrigBeamSpotVertex',
            'TrigVertexCollection_tlp2_HLT_EFHistoPrmVtx',
            'Trk::VxContainer_tlp1_HLT_PrimVx',
            'Trk::VxContainer_tlp1_HLT_ConversionVx',
            'Trk::VxContainer_tlp2_HLT_ConversionVx',
            'Trk::VxContainer_tlp2_HLT_PrimVx',
            'TrigVertexCollection_tlp2_HLT_TrigBeamSpotVertex_SiTrack',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Bjet_EFID',

            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Bjet_EFID',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Bjet_EFID',
            
            ]
        triggerCounterBjet = Counter('triggerBjet',triggerListBjet)
    
        triggerListBphys = [
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Bphysics_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_FullScan_EFID',
            'TrigEFBphysContainer_tlp1_HLT_EFBMuMuFex',
            'TrigEFBphysContainer_tlp1_HLT_EFDsPhiPiFex',
            'TrigEFBphysContainer_tlp1_HLT_EFMuPairs',
        'TrigEFBphysContainer_tlp1_HLT_EFTrackMass',
        'TrigEFBphysContainer_tlp2_HLT_EFMuPairs',
        'TrigEFBphysContainer_tlp2_HLT_EFTrackMass',
        'TrigEFBphysContainer_tlp2_HLT_EFDsPhiPiFex',
        'TrigEFBphysContainer_tlp2_HLT_EFBMuMuFex',
        'TrigEFBphysContainer_tlp2_HLT_EFMultiMuFex',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Bphysics',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Bphysics',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Bphysics',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_FullScan',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_FullScan',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_FullScan',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Bphysics',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Bphysics',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Bphysics',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_FullScan',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_FullScan',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_FullScan',
            'TrigL2BphysContainer_tlp1_HLT',
            'TrigL2BphysContainer_tlp1_HLT_L2BMuMuFex',
            'TrigL2BphysContainer_tlp1_HLT_L2BMuMuXFex',
            'TrigL2BphysContainer_tlp1_HLT_L2DiMuXFex',
            'TrigL2BphysContainer_tlp1_HLT_L2DsPhiPiFexDs',
            'TrigL2BphysContainer_tlp1_HLT_L2DsPhiPiFexPhi',
            'TrigL2BphysContainer_tlp1_HLT_L2JpsieeFex',
            'TrigL2BphysContainer_tlp1_HLT_TrigDiMuon',
            'TrigL2BphysContainer_tlp1_HLT_L2TrackMass',
        'TrigL2BphysContainer_tlp2_HLT',
        'TrigL2BphysContainer_tlp2_HLT_L2BMuMuXFex',
        'TrigL2BphysContainer_tlp2_HLT_L2DsPhiPiFexDs',
        'TrigL2BphysContainer_tlp2_HLT_L2DsPhiPiFexPhi',
        'TrigL2BphysContainer_tlp2_HLT_L2MultiMuFex',
        'TrigL2BphysContainer_tlp2_HLT_L2BMuMuFex',
            'TrigL2BphysContainer_tlp2_HLT_L2JpsieeFex',
            'TrigL2BphysContainer_tlp2_HLT_L2TrackMass',
        'TrigL2BphysContainer_tlp2_HLT_L2DiMuXFex',
        'TrigL2BphysContainer_tlp2_HLT_TrigDiMuon',
        'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Bphysics_EFID',
            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Bphysics_EFID',


        ]
        triggerCounterBphys = Counter('triggerBphys',triggerListBphys)
  
        triggerListConfigID = [
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_FullScan_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',

            'HLT_xAOD__RoiDescriptorStore_forID',            
            'HLT_xAOD__RoiDescriptorStore_forIDAux.',            
            'HLT_xAOD__RoiDescriptorStore_forID1',
            'HLT_xAOD__RoiDescriptorStore_forID1Aux.',
            'HLT_xAOD__RoiDescriptorStore_forID2',
            'HLT_xAOD__RoiDescriptorStore_forID2Aux.',
            'HLT_xAOD__RoiDescriptorStore_forID3',
            'HLT_xAOD__RoiDescriptorStore_forID3Aux.',
        ]
        triggerCounterID = Counter('triggerID',triggerListConfigID)
  
        triggerListConfig = [
            'IOVMetaDataContainer_p1__TRIGGER_HLT_Menu',
            'IOVMetaDataContainer_p1__TRIGGER_LVL1_Lvl1ConfigKey',
            'IOVMetaDataContainer_p1__TRIGGER_LVL1_Menu',
            'IOVMetaDataContainer_p1__TRIGGER_LVL1_Prescales',
            'IOVMetaDataContainer_p1__TRIGGER_HLT_PrescaleKey',
            'IOVMetaDataContainer_p1__TRIGGER_HLT_ConfigKeys',
            '_TRIGGER_HLT_Menu', 
            '_TRIGGER_HLT_Prescales',
            '_TRIGGER_HLT_PrescaleKey',
            '_TRIGGER_HLT_HltConfigKeys', 
            '_TRIGGER_LVL1_Menu',
            '_TRIGGER_LVL1_Lvl1ConfigKey',
            '_TRIGGER_LVL1_Prescales',
            '_TRIGGER_LVL1_ItemDef',
            '_TRIGGER_LVL1_Thresholds',
            'TriggerMenu',
            'TriggerMenuAux.',
        ]
        triggerCounterConfig = Counter('triggerConfig',triggerListConfig)

        triggerListCosmics = [
            'CosmicMuonCollection_tlp1_HLT_CosmicMuons',
            'MdtTrackSegmentCollection_tlp1_HLT_MDTsegments',
            'MdtTrackSegmentCollection_p2_HLT_MDTsegments',
            'TrigEMClusterContainer_tlp1_HLT_TrigT2CaloCosmic',
            'TrigEMClusterContainer_p3_HLT_TrigT2CaloCosmic',
            'TrigEMClusterContainer_p4_HLT_TrigT2CaloCosmic',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Cosmics',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Cosmics',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Cosmics',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Cosmics',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Cosmics',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Cosmics',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_CosmicsN_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
            'TrigT2JetContainer_tlp1_HLT_TrigT2CosmicJet',
            'TrigT2JetContainer_p3_HLT_TrigT2CosmicJet',
            'TrigRoiDescriptorCollection_tlp1_HLT_TrigT2CosmicJet',
            'TrigRoiDescriptorCollection_p2_HLT_TrigT2CosmicJet',
            'JetCollection_tlp5_HLT_TrigCosmicJetRec',
            'JetCollection_tlp6_HLT_TrigCosmicJetRec',
            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_CosmicsN_EFID'
            ]
        triggerCounterCosmics = Counter('triggerCosmics',triggerListCosmics)
            
        triggerListEgamma = [
            'egammaContainer_p1_HLT_egamma',
            'egammaContainer_p2_HLT_egamma',
            'egammaContainer_p5_HLT_egamma',
            'egammaContainer_p2_HLT_egamma_Photons',
            'egDetailContainer_p1_HLT_egamma',
            'egDetailContainer_p2_HLT_egamma',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Electron_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Photon_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationCombined_Electron_EFID',
            'TrigElectronContainer_tlp2_HLT_L2IDCaloFex',
            'TrigElectronContainer_p3_HLT_L2IDCaloFex',
            'TrigEMClusterContainer_tlp1_HLT',   
            'TrigEMClusterContainer_tlp1_HLT_TrigT2CaloEgamma',
            'TrigEMClusterContainer_p3_HLT',   
            'TrigEMClusterContainer_p4_HLT',   
            'TrigEMClusterContainer_p3_HLT_TrigT2CaloEgamma',
            'TrigEMClusterContainer_p4_HLT_TrigT2CaloEgamma',
        'TrigEMClusterContainer_p4_HLT_T2CaloSwCluster',

            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_eGamma',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_eGamma',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_eGamma',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_eGamma_Brem',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_eGamma_Brem',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_eGamma_Brem',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_eGamma',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_eGamma',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_eGamma',
            'TrigPhotonContainer_tlp2_HLT_L2PhotonFex',
            'TrigPhotonContainer_p3_HLT_L2PhotonFex',
            'egammaContainer_p1_HLT_egamma_Electrons',
            'egammaContainer_p1_HLT_egamma_Photons',
            'egammaContainer_p2_HLT_egamma_Electrons',
            'egammaContainer_p5_HLT_egamma_Photons',
            'egammaContainer_p5_HLT_egamma_Electrons',
            'egDetailContainer_p1_HLT_egamma_Electrons',
            'egDetailContainer_p2_HLT_egamma_Electrons',
            'egDetailContainer_p1_HLT_egamma_Photons',
            'egDetailContainer_p2_HLT_egamma_Photons',
            'RingerRingsContainer_tlp1_HLT_TrigT2CaloEgamma',
            'RingerRingsContainer_p2_HLT_TrigT2CaloEgamma',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_eGamma_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_eGamma_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_eGamma_robust',
            'TrigEMClusterContainer_tlp1_HLT_TrigcaloSwCluster',
            'TrigEMClusterContainer_p3_HLT_TrigcaloSwCluster',
            'TrigEMClusterContainer_p4_HLT_TrigcaloSwCluster',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
            'CaloClusterContainer_p6_HLT_TrigCaloClusterMaker_topo',
            'TrigRNNOutputContainer_HLT_TrigRingerNeuralFex',
            'TrigRNNOutputContainer_p2_HLT_TrigRingerNeuralFex',
            'TrigElectronContainer_tlp2_HLT_L2ElectronFex',
            'TrigElectronContainer_p3_HLT_L2ElectronFex',
            'CaloClusterContainer_p6_HLT_TrigCaloClusterMaker_slw',
            'CaloClusterContainer_p7_HLT_TrigCaloClusterMaker_slw',
            'TrigEMClusterContainer_tlp1_HLT_T2CaloSwCluster',
            'TrigEMClusterContainer_p3_HLT_T2CaloSwCluster',
            'TrigCaloClusterContainer_p3_HLT_TrigT2Calo',
            'egammaContainer_p3_HLT_egamma',
            'egammaContainer_p4_HLT_egamma',
            'egammaContainer_p3_HLT_egamma_Photons',
            'egammaContainer_p4_HLT_egamma_Photons',
            'egammaContainer_p3_HLT_egamma_Electrons',
            'egammaContainer_p4_HLT_egamma_Electrons',
            'TrigRNNOutputContainer_tlp1_HLT_TrigRingerNeuralFex',
        'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Electron_EFID',

            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Photon_EFID',
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Electron_EFID',
            'CaloCompactCellContainer_HLT_TrigT2CaloEgammaCells',
            'TrigEMClusterContainer_tlp1_HLT_TrigCaloRinger',
            'TrigEMClusterContainer_p3_HLT_TrigCaloRinger',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Photon_EFID',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Electron_EFID',
        'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Photon_EFID',
        ]
        triggerCounterEgamma = Counter('triggerEgamma',triggerListEgamma)
    
        triggerListJet = [
            'JetCollection_tlp2_HLT',
            'JetCollection_tlp2_HLT_TrigJetRec',
            'JetCollection_tlp5_HLT_TrigJetRec',
            'JetCollection_tlp6_HLT_TrigJetRec',
            'TrigT2JetContainer_tlp1_HLT_TrigT2CaloJet',
            'TrigT2JetContainer_p3_HLT_TrigT2CaloJet',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Jet_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Jet_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Jet_robust',
            #'JetCollection_tlp5_AntiKt4TruthPileupJets',
            #'JetCollection_tlp5_Cone4TruthPileupJets',
           
        ]
        triggerCounterJet = Counter('triggerJet',triggerListJet)
    
        triggerListMET = [
            'TrigMissingETContainer_tlp1_HLT_T2MissingET',
            'TrigMissingETContainer_tlp1_HLT_TrigEFMissingET',
            'TrigMissingETContainer_tlp1_HLT_TrigEFMissingET_FEB',
            'TrigMissingETContainer_tlp1_HLT_TrigEFMissingET_noiseSupp',
            'TrigMissingETContainer_tlp1_HLT_TrigEFMissingET_allCells',    
            'TrigMissingETContainer_tlp1_HLT_L2JetEtSum',
            'TrigMissingETContainer_tlp1_HLT_EFJetEtSum',
            'TrigMissingETContainer_p3_HLT_T2MissingET',
            'TrigMissingETContainer_p3_HLT_TrigEFMissingET',
            'TrigMissingETContainer_p3_HLT_TrigEFMissingET_FEB',
            'TrigMissingETContainer_p3_HLT_TrigEFMissingET_noiseSupp',
            'TrigMissingETContainer_p3_HLT_TrigEFMissingET_allCells',    
            'TrigMissingETContainer_p3_HLT_L2JetEtSum',
            'TrigMissingETContainer_p3_HLT_EFJetEtSum',
        'TrigMissingETContainer_p3_HLT_L2MissingET_FEB',
            'TrigMissingETContainer_p3_HLT_TrigEFMissingET_topocl',
            #    'MissingETSigHypoContainer_p1_EtMissHypoCollection',
            ]
        triggerCounterMET = Counter('triggerMET',triggerListMET)
    
        triggerListMinBias = [
            'TrigSpacePointCountsCollection_p4_HLT_spacepoints',
            'TrigSpacePointCountsCollection_tlp1_HLT_spacepoints',
            'TrigTrackCountsCollection_tlp1_HLT',
            'TrigT2MbtsBitsContainer_tlp1_HLT_T2Mbts',
            'TrigT2MbtsBitsContainer_p3_HLT_T2Mbts',
            'TrigTrackCountsCollection_tlp1_HLT_trackcounts',
        'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_minBias_EFID',
            'TrigVertexCountsCollection_tlp1_HLT_vertexcounts',
            ]
        triggerCounterMinBias = Counter('triggerMinBias',triggerListMinBias)
    
        triggerListMuon = [
            'HLT_MuonFeatureContainer',
            'HLT_MuonFeatureDetailsContainer',
            'HLT_CombinedMuonFeatureContainer',
            'CombinedMuonFeatureContainer_tlp1_HLT',
            'IsoMuonFeatureContainer_tlp1_HLT',
            'CombinedMuonFeatureContainer_p2_HLT',
            'CombinedMuonFeatureContainer_p3_HLT',
            'CombinedMuonFeatureContainer_p4_HLT',
            'MuonFeatureContainer_p3_HLT',
            'MuonFeatureContainer_tlp1_HLT',
            'MuonFeatureContainer_tlp2_HLT',
            'MuonFeatureDetailsContainer_p2_HLT',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Muon_EFID',
            'TileMuFeatureContainer_p2_HLT',
            'TileMuFeatureContainer_tlp1_HLT',
            'TileTrackMuFeatureContainer_tlp1_HLT',
            'TileTrackMuFeatureContainer_p3_HLT',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Muon',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Muon',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Muon',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_muonIso',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_muonIso',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_muonIso',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Tile',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Tile',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Tile',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Muon',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Muon',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Muon',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_muonIso',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_muonIso',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_muonIso',
            'TrigMuonEFIsolationContainer_p1_HLT_MuonEFIsolation',
            'TrigMuonEFIsolationContainer_p2_HLT_MuonEFIsolation',
            'TrigMuonEFContainer_tlp1_HLT_MuonEF',
            'TrigMuonEFInfoContainer_tlp1_HLT_MuonEFInfo',
            'TrigMuonEFInfoContainer_tlp2_HLT_MuonEFInfo',
            'TrigMuonEFInfoContainer_tlp1_HLT_MuonEF',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Muon_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Muon_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Muon_robust',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Tile',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Tile',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Tile',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Tile_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Tile_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Tile_robust',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_muonIso_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_muonIso_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_muonIso_robust',
            'MuonFeatureDetailsContainer_tlp1_HLT',
            'TrigMuonEFInfoContainer_tlp1_HLT_MuTagIMO_EF',
            'TrigMuonEFInfoContainer_tlp2_HLT_MuTagIMO_EF',
            'TrigMuonEFInfoContainer_tlp1_HLT_eMuonEFInfo',
            'TrigMuonEFInfoContainer_tlp2_HLT_eMuonEFInfo',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
            'TrigMuonClusterFeatureContainer_tlp1_HLT_MuonCluster',
            'IsoMuonFeatureContainer_p2_HLT',  
            'IsoMuonFeatureContainer_p3_HLT',  
            'ElectronMuonTopoInfoContainer_p1_HLT_EF_EgMuTopoFEX',
            'ElectronMuonTopoInfoContainer_p1_HLT_EgMuTopoFEX',
            'ElectronMuonTopoInfoContainer_p1_HLT_EF_PhotonMuonTopoFEX',
            'ElectronMuonTopoInfoContainer_p1_HLT_L2_PhotonMuonTopoFEX',
            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Muon_EFID',
            'Muon::TgcCoinDataContainer_tlp1_TrigT1CoinDataCollectionNextBC',
            'Muon::TgcCoinDataContainer_tlp1_TrigT1CoinDataCollectionPriorBC',
            'Muon::TgcCoinDataContainer_tlp1_TrigT1CoinDataCollection',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Muon_EFID',
            'Muon::TgcCoinDataContainer_tlp2_TrigT1CoinDataCollectionNextBC',
            'Muon::TgcCoinDataContainer_tlp2_TrigT1CoinDataCollectionPriorBC',
            'Muon::MuonPRD_Container_p2<Muon::RpcCoinData_p1>_RPC_triggerHits',
            'Muon::TgcCoinDataContainer_tlp2_TrigT1CoinDataCollection',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Muon_EFID', 
            'HLT_xAOD__RoiDescriptorStore_forMS',
            'HLT_xAOD__RoiDescriptorStore_forMSAux.',
        ]
        triggerCounterMuon = Counter('triggerMuon',triggerListMuon)
    
        triggerListSteer = [
            'HLT::HLTResult_p1_HLTResult_EF',
            'HLT::HLTResult_p1_HLTResult_L2',
            'TrigDec::TrigDecision_p1_TrigDecision',
            'TrigDec::TrigDecision_p2_TrigDecision',
            'TrigDec::TrigDecision_p3_TrigDecision',
            'TrigDec::TrigDecision_p4_TrigDecision',
            'TrigDec::TrigDecision_p5_TrigDecision',
            'TrigRoiDescriptorCollection_p2_HLT',
            'TrigRoiDescriptorCollection_p2_HLT_forID',
            'TrigRoiDescriptorCollection_p2_HLT_forMS',
            'TrigRoiDescriptorCollection_p2_HLT_initialRoI',
            'TrigRoiDescriptorCollection_p2_HLT_secondaryRoI_EF',
            'TrigRoiDescriptorCollection_p2_HLT_secondaryRoI_L2',
            'TrigRoiDescriptorCollection_p2_HLT_T2TauFinal',
            'TrigRoiDescriptorCollection_p2_HLT_TrigCaloRinger',
            'TrigRoiDescriptorCollection_p2_HLT_TrigT2CaloEgamma',
            'TrigRoiDescriptorCollection_p2_HLT_TrigT2CaloJet',
            'TrigRoiDescriptorCollection_p2_HLT_TrigT2CaloTau',
            'TrigRoiDescriptorCollection_tlp1_HLT',
            'TrigRoiDescriptorCollection_tlp1_HLT_forID',
            'TrigRoiDescriptorCollection_tlp1_HLT_forMS',
            'TrigRoiDescriptorCollection_tlp1_HLT_initialRoI',
            'TrigRoiDescriptorCollection_tlp1_HLT_secondaryRoI_EF',
            'TrigRoiDescriptorCollection_tlp1_HLT_secondaryRoI_L2',
            'TrigRoiDescriptorCollection_tlp1_HLT_T2TauFinal',
            'TrigRoiDescriptorCollection_tlp1_HLT_TrigCaloRinger',
            'TrigRoiDescriptorCollection_tlp1_HLT_TrigT2CaloEgamma',
            'TrigRoiDescriptorCollection_tlp1_HLT_TrigT2CaloJet',
            'TrigRoiDescriptorCollection_tlp1_HLT_TrigT2CaloTau',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_L2',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_L2Aux.',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_EF',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_EFAux.',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_HLT',
            'HLT_xAOD__RoiDescriptorStore_secondaryRoI_HLTAux.',
            'HLT_xAOD__RoiDescriptorStore_initialRoI',
            'HLT_xAOD__RoiDescriptorStore_initialRoIAux.',
            # Only in ESDs
            'TrigOperationalInfoCollection_tlp1_HLT_OPI_EF',
            'TrigOperationalInfoCollection_tlp1_HLT_OPI_L2',
            'CTP_RDO_p2_CTP_RDO',
            'RoIBResult_p1_RoIBResult',
            'RingerRingsContainer_tlp1_HLT_TrigCaloRinger',
            #new 
            'TrigPassBitsCollection_p1_HLT_passbits',
            'IOVMetaDataContainer_p1__TRIGGER_HLT_Prescales',
            ]
        triggerCounterSteer = Counter('triggerSteer',triggerListSteer)
    
        triggerListTau = [
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Tau_EFID',
            'TauDetailsContainer_tlp1_HLT_TrigTauDetailsCalo',
            'TauDetailsContainer_tlp1_HLT_TrigTauDetailsMerged',
            'TauDetailsContainer_tlp1_HLT_TrigTauExtraDetailsMerged',
            'TauJetContainer_p3_HLT_TrigTauRecCalo',
            'TauJetContainer_p3_HLT_TrigTauRecMerged',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Tau',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Tau',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Tau',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Tau',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Tau',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Tau',
            'TrigTauClusterContainer_tlp1_HLT_TrigT2CaloTau',
            'TrigTauClusterContainer_p4_HLT_TrigT2CaloTau',
            'TrigTauContainer_tlp1_HLT',
            'TrigTauContainer_p3_HLT',
            'TrigTauClusterContainer_p3_HLT_TrigT2CaloTau',
            'TrigTauClusterContainer_p5_HLT_TrigT2CaloTau',
            'TrigTauClusterDetailsContainer_p2_HLT_TrigT2CaloTauDetails',
            'TrigTauTracksInfoCollection_tlp1_HLT',
            'TrigTauTracksInfoCollection_p2_HLT',
            'TrigTauClusterDetailsContainer_tlp1_HLT_TrigT2CaloTau',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Tau_SiTrk_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreation_Tau_IDOnly_EFID',
            'TrigInDetTrackCollection_tlp1_HLT_TrigSiTrack_Tau_robust',
            'TrigInDetTrackCollection_tlp2_HLT_TrigSiTrack_Tau_robust',
            'TrigInDetTrackCollection_tlp3_HLT_TrigSiTrack_Tau_robust',
            'TrigInDetTrackCollection_tlp1_HLT_TrigIDSCAN_Tau_IDOnly',
            'TrigInDetTrackCollection_tlp2_HLT_TrigIDSCAN_Tau_IDOnly',
            'TrigInDetTrackCollection_tlp3_HLT_TrigIDSCAN_Tau_IDOnly',
            'TrigTauClusterDetailsContainer_tlp1_HLT_TrigT2CaloTauDetails',
            'TauDetailsContainer_tlp2_HLT_TrigTauDetailsCalo',
            'TauDetailsContainer_tlp3_HLT_TrigTauDetailsCalo',
            'TauDetailsContainer_tlp2_HLT_TrigTauDetailsMerged',
            'TauDetailsContainer_tlp3_HLT_TrigTauDetailsMerged',
            'TauDetailsContainer_tlp2_HLT_TrigTauExtraDetailsMerged',
            'TauJetContainer_p4_HLT_TrigTauRecCalo',
            'TauJetContainer_p4_HLT_TrigTauRecMerged',
            'JetCollection_tlp6_HLT_TrigTauJet', 
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreation_Tau_EFID',
            # Only in ESDs
            'Trk::TrackCollection_tlp2_HLT_InDetTrigTrackSlimmer_Tau_EFID',
            'CaloCompactCellContainer_HLT_TrigT2CaloTauCells',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_Tau_EFID',
            'Rec::TrackParticleContainer_tlp2_HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
        ]
        triggerCounterTau = Counter('triggerTau',triggerListTau)
    
        triggerListL1 = [
            'CPMHitsCollection_tlp1_CPMHits',
            'JetElementCollection_tlp1_JetElements',
            'CaloTowerContainer_p1_HLT_TrigCaloTowerMaker',   
            'CMMCPHitsCollection_tlp1_CMMCPHits', 
            'CMMEtSumsCollection_tlp1_CMMEtSums',
            'CMMJetHitsCollection_tlp1_CMMJetHits',
            'CMMRoI_p1_CMMRoIs',
            'CPMRoICollection_tlp1_CPMRoIs',
            'CPMTowerCollection_tlp1_CPMTowers',
            'CTP_Decision_p2_CTP_Decision',
            'JEMEtSumsCollection_tlp1_JEMEtSums',
            'JEMHitsCollection_tlp1_JEMHits',
            'JEMRoICollection_tlp1_JEMRoIs',
            'LVL1_ROI_p1_LVL1_ROI',
            'TriggerTowerCollection_tlp1_TriggerTowers',
            'IOVMetaDataContainer_p1__TRIGGER_LVL1_ItemDef',
        ]
        triggerCounterL1 = Counter('triggerL1',triggerListL1)

        triggerListCommon = [
            'CaloClusterContainer_p4_HLT',
            'CaloClusterContainer_p5_HLT',
            'CaloClusterContainer_p4_HLT_TrigCaloClusterMaker',  
            'CaloClusterContainer_p5_HLT_TrigCaloClusterMaker',  
            'CaloShowerContainer_p2_HLT',
            'TrigInDetTrackCollection_tlp1_HLT',
            'TrigInDetTrackCollection_tlp2_HLT',
            'TrigInDetTrackCollection_tlp3_HLT',
            'TrigInDetTrackCollection_tlp1_HLT_TRTSegmentFinder',
            'TrigInDetTrackCollection_tlp2_HLT_TRTSegmentFinder',
            'TrigInDetTrackCollection_tlp3_HLT_TRTSegmentFinder',
            'TrigInDetTrackCollection_tlp1_HLT_TRTxK',
            'TrigInDetTrackCollection_tlp2_HLT_TRTxK',
            'TrigInDetTrackCollection_tlp3_HLT_TRTxK',
            'TrigInDetTrackTruthMap_tlp1_TrigInDetTrackTruthMap',
            'TrigInDetTrackTruthMap_tlp2_TrigInDetTrackTruthMap',
            'Trk::TrackCollection_tlp3_HLT_InDetTrigTrackSlimmer_FullScan_EFID',
            'Rec::TrackParticleContainer_tlp1_HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
            'CaloClusterContainer_p6_HLT',
            'CaloClusterContainer_p7_HLT',
            'CaloClusterContainer_p6_HLT_TrigCaloClusterMaker',
            'CaloClusterContainer_p7_HLT_TrigCaloClusterMaker',
            'TrigTrtHitCountsCollection_tlp1_HLT_TrtHitCount',
            'TrigTrtHitCountsCollection_p2_HLT_TrtHitCount',

        ]
        triggerCounterCommon = Counter('triggerCommon',triggerListCommon)
        triggerListHLTNav = []
        triggerCounterHLTNav = Counter('triggerHLTNav',triggerListHLTNav)
    
        truthList = [
            'McEventCollection_p4_GEN_AOD',
            'MissingEtTruth_p1_MET_Truth',
            'MissingEtTruth_p1_MET_Truth_PileUp',
            'MissingEtTruth_p3_MET_Truth_PileUp',
            'MissingEtTruth_p3_MET_Truth',
            'Rec::TrackParticleTruthCollection_p1_TrackParticleTruthCollection',
            'TrackRecordCollection_p2_MuonEntryLayerFilter',
            'TruthParticleContainer_p5_SpclMC',
            'TruthParticleContainer_p5_INav4MomTruthEvent',
            'JetCollection_tlp5_AntiKt4TruthJets',  
        ]
        truthCounter = Counter('truth',truthList)
            
        listofCounters = [
        atlfastCounter,
        caloCounter,
        egammaCounter,
        eventinfoCounter,
        emtCounter,
        indetCounter,
        jetCounter,
        jtCounter,
        metCounter,
        muonCounter,
        tauCounter,
        triggerCounterBjet,
        triggerCounterBphys,
        triggerCounterConfig,
        triggerCounterCosmics,
        triggerCounterEgamma,
        triggerCounterID,
        triggerCounterJet,
        triggerCounterMET,
        triggerCounterMinBias,
        triggerCounterMuon,
        triggerCounterTau,
        triggerCounterCommon,
        triggerCounterSteer,
        triggerCounterL1,
        triggerCounterHLTNav,
        truthCounter,
        ]
    
        listofTriggerCounters = [
        triggerCounterBjet,
        triggerCounterBphys,
        triggerCounterConfig,
        triggerCounterCosmics,
        triggerCounterEgamma,
        triggerCounterID,
        triggerCounterJet,
        triggerCounterMET,
        triggerCounterMinBias,
        triggerCounterMuon,
        triggerCounterTau,
        triggerCounterCommon,
        triggerCounterSteer,
        triggerCounterL1,
        triggerCounterHLTNav,
        ]
    
        listofNonTrigCounters = [
        atlfastCounter,
        caloCounter,
        egammaCounter,
        eventinfoCounter,
        emtCounter,
        indetCounter,
        jetCounter,
        jtCounter,
        metCounter,
        muonCounter,
        tauCounter,
        truthCounter,
        ]

        print("file:",self.checkFile)
        try:
            file = open(self.checkFile,'r')
        except:
            parentFile = self.checkFile.replace(".checkFile","")
            print("WARNING: generating %s from %s", self.checkFile, parentFile)
            os.system("checkFile.py " + parentFile + " >"+self.checkFile+"0")
            file = open(self.checkFile+"0",'r')
            if file == 0:
                return self.error

        self.total = 0
        notUsedList = [] #if an entry in the checkFile output is not included in a Counter
        doublesList = {} #if an entry in the checkFile output matches >=2 Counter items
    
        sumNU = 0 #sum of non used containers

        ## list of trigger algorithms found in AOD, their category, and their size in the AOD
        self.triggerAlgList = []
        triggerAlgListNotIncluded = []
        nontriggerAlgList = []
        self.totalAlgSize = 0.0
        
        while 1:
    
            line = file.readline()
            if not line:
                break

            splitline = line.split()

            #get the total from the last line of the file
            if re.search("TOTAL",line):
                self.total = float(splitline[4])

            #for new xAOD    
            if re.search("Total",line):
                if (str(splitline[4])).isnumeric():
                    self.total = float(splitline[4])

            #only count the good lines (ie. not "=====", etc.)
            if len(splitline)!=10:
                continue

            #the name of the class    
            name = splitline[9]

            #number of events
            nEvents = splitline[7]

            #size per events
            sizePerEvent = splitline[4]

            isLineCounted = False
            
            # sum of all algorithm sizes in checkFile
            self.totalAlgSize += float(sizePerEvent)
            
            # Variable for testing if algorithm might be trigger related
            # Used in the last elif statement
            TrigInName = name.find("Trig") + name.find("HLT") + name.find("LVL1")
            
            ## Add trigger algorithm to trigger list if it's found in TriggerEDM.py dictionary
            ## !! It should definately be found if it's a trigger algorithm !!
            ## Some exceptions for Steering info and RoIDescriptorStore
            if getCategory(name) != 'NOTFOUND':
                
                
                

                self.triggerAlgList.append([name, getCategory(name), float(sizePerEvent)])
                
            ## IOVMetaDataContainer* are not in TriggerEDM.py dictionary -> Add manually to list
            elif getCategory(name) == 'NOTFOUND' and name.count('IOVMetaDataContainer'):
                self.triggerAlgList.append([name, 'Config', float(sizePerEvent)])
                
            elif getCategory(name) == 'NOTFOUND' and name in triggerListConfig:
                self.triggerAlgList.append([name, 'Config', float(sizePerEvent)])

            elif getCategory(name) == 'NOTFOUND' and name in triggerListMuon:
                self.triggerAlgList.append([name, 'Muon', float(sizePerEvent)])

            elif getCategory(name) == 'NOTFOUND' and name in triggerListSteer:
                self.triggerAlgList.append([name, 'Steer', float(sizePerEvent)])

            elif getCategory(name)  == 'NOTFOUND' and name in triggerListConfigID:
                self.triggerAlgList.append([name, 'Tracking', float(sizePerEvent)])
            elif getCategory(name)  == 'NOTFOUND' and name.startswith("HLTNav_"):
                self.triggerAlgList.append([name, 'HLTNav', float(sizePerEvent)])
            ## Do some simple checks if algorithm is not found in dictionary (and isn't IOVMetaDataContainer*)
            ## Add these to triggerAlgsNotIncluded
            ## Can be used to debug the search algorithm of getCategory() in TriggerEDM.py
            elif getCategory(name) == 'NOTFOUND' and TrigInName != -3:
                triggerAlgListNotIncluded.append([name, getCategory(name), float(sizePerEvent)])

                    
            ## Otherwise, put in non-trigger algorithm list
            else:
                nontriggerAlgList.append([name, 'NonTrigger', float(sizePerEvent)])
            ## ========================================================

            #loop over each counter
            for counter in listofCounters:

                #loop over each item in each counter
                for item in counter.itemList:

                #check that item has not already been counted
                    if item==name and isLineCounted:
                        doublesList[name]=item
                    #if not already counted, increment the size of the Counter
                    if item==name and not isLineCounted:
                        counter.size += float(splitline[4])
                        isLineCounted = True

            #append the notUsedList
            if not isLineCounted:
                try:
                 sumNU += float(splitline[4])
                except: pass
                TrigInName = name.find("Trig") + name.find("HLT") + name.find("LVL1")

        sum = 0 #the sum of all counters (should be equal to 'total')
        sumTrig = 0 #the sum of trigger component
        sumNonTrig = 0 #the sum of non trigger component
        
        ## Calculate trigger category sizes and store in dictionary
        ## ========================================================
        self.triggerAlgSize = {}
        self.triggerAlgSize[ 'Total' ] = 0.0
        for triggerAlg in self.triggerAlgList:
            if not triggerAlg[1] in self.triggerAlgSize: self.triggerAlgSize[ triggerAlg[1] ] = triggerAlg[2]
            else: self.triggerAlgSize[ triggerAlg[1] ] += triggerAlg[2]
            self.triggerAlgSize[ 'Total' ] += triggerAlg[2]
            
        self.totalAlgSizeInLists = 0.0
        allAlgList = self.triggerAlgList + triggerAlgListNotIncluded + nontriggerAlgList
        for item in allAlgList:
            self.totalAlgSizeInLists += item[2]
        ## ========================================================

        fout =open(self.checkFile+"trigSize.txt",'w')

            
        print()
        print("Summary of categories:")
        fout.write( "\n Summary of categories:\n")

        #print the size of each counter (and calculate the sum)
        for counter in listofCounters:
            sum += counter.size

        #Compute the total size of trigger component
        for counter in listofTriggerCounters:
            sumTrig += counter.size
            # print counter.name.ljust(24), "%6.3f" % counter.size
            # fout.write( counter.name.ljust(24) + "%6.3f" % counter.size + "\n")
        for key in sorted(self.triggerAlgSize):
            if not key == 'Total':
                print("trigger"+key.ljust(24), "%6.3f" % self.triggerAlgSize[key])
                fout.write( "trigger"+key.ljust(24) + "%6.3f" % self.triggerAlgSize[key] + "\n")

        #Compute the total size of non trigger component
        for counter in listofNonTrigCounters:
            sumNonTrig += counter.size

            fout.write("\n=====================\n")

            print()
            print("=====================")
            print("Total Trigger Size".ljust(23), "%6.3f" % self.triggerAlgSize[ 'Total' ])
            fout.write("Total Trigger Size".ljust(23) + "%6.3f" % self.triggerAlgSize[ 'Total' ] + "\n")
            tsize = sumNU+sum
            print("Total file size".ljust(23), self.totalAlgSize)
            fout.write("Total file size".ljust(23) + "%6.3f" % self.totalAlgSize + "\n")
            print("Total file size (list)".ljust(23), self.totalAlgSizeInLists)
            fout.write("Total file size (list)".ljust(23) + "%6.3f" % self.totalAlgSizeInLists + "\n")
            print("Total (from checkFile)".ljust(23), self.total)
            fout.write("Total (from checkFile)".ljust(23)+"%6.3f" % self.total + "\n")
            print()

            if len(self.triggerAlgList) > 0:
                fout.write( "\nThe following were found in TriggerEDM and counted but have a classification 'Unknown': \n")
                for item in self.triggerAlgList:
                    if item[1] == 'Unknown':
                        fout.write( "INFO:  "+item[0]+"\n" )
                
            if len(triggerAlgListNotIncluded) > 0:
                fout.write( "\nThe following were not found in TriggerEDM and were not counted. \n")
                fout.write( "Check if they are included in TriggerEDM. They must be if they are here! \n")
                fout.write( "The problem must be with the search method in TriggerEDM.getCategory().  Please debug. \n")
                for item in triggerAlgListNotIncluded:
                    fout.write( "WARNING:  Unknown item "+item[0]+"\n")
                return self.error
            return self.success

# This is just for testing outside of the RTT
if __name__ == "__main__":
    #params = {'checkFile':'AOD.pool.root.checkFile'}
    params = {}
    r =  checkFileTrigSize_RTT(params)
    status = r.run()
    
    sys.exit(status)

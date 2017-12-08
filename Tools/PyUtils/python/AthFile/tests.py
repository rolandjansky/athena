# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file PyUtils/python/AthFile/tests.py
# @purpose a simple abstraction of a file to retrieve informations out of it
# @author Sebastien Binet <binet@cern.ch>
# @date October 2008
from __future__ import with_statement

import unittest, sys

verbose = False

def _compare_fileinfos(af, fileinfos):
    all_good = True
    err_log = []
    for k in ('file_md5sum',
              'file_name',
              'file_type',
              'file_guid',
              'nentries',
              'run_number',
              'run_type',
              'evt_type',
              'evt_number',
              'lumi_block',
              'beam_energy',
              'beam_type',
              'stream_tags',
              'metadata_items',
              'eventdata_items',
              'stream_names',
              'geometry',
              'conditions_tag',
              'det_descr_tags',
              #'metadata',
              'tag_info',
              ):
        chk = af.fileinfos[k]
        ref = fileinfos[k]
        if isinstance(chk, (list,tuple)):
            chk = sorted(chk)
        if isinstance(ref, (list,tuple)):
            ref = sorted(ref)
        if chk != ref:
            all_good = False
            err_log.append("key [%s] miscompare:\nref:%s\nchk:%s"%(k,ref,chk))
    assert all_good, "\n".join(err_log)

class AthFileTest(unittest.TestCase):

    def setUp(self):
        import PyUtils.AthFile as af
        import PyUtils.Logging as L
        af.msg.setLevel(L.logging.WARNING)
        # we don't want to accidentally read informations from a sticky cache
        af.server.flush_cache()

    def tearDown(self):
        # we don't want to accidentally write informations to a sticky cache
        import PyUtils.AthFile as af
        af.server.flush_cache()

    def test001(self):
        """test cosmics POOL file"""
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root'
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root')
        assert af.exists(fname)

        f1 = af.fopen(fname)
        if verbose:
            print "::: f1.fileinfos:"
            print f1.fileinfos
        f1_ref = {'file_md5sum':'36ff1ef242bd3240227016e71e241a89', 'metadata_items': [('EventStreamInfo', 'StreamESD'), ('LumiBlockCollection', 'LumiBlocks'), ('DataHeader', ';00;MetaDataSvc'), ('IOVMetaDataContainer', '/GLOBAL/DETSTATUS/LBSUMM')], 'stream_names': ['StreamESD'], 'run_type': ['N/A'], 'stream_tags': [{'obeys_lbk': True, 'stream_type': 'physics', 'stream_name': 'IDCosmic'}], 'tag_info': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPermanent-01', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', '/MUONALIGN/MDT/ENDCAP/SIDEC': 'MuonAlignMDTEndCapCAlign-REPRO-08', '/MUONALIGN/MDT/BARREL': 'MuonAlignMDTBarrelAlign-0100-SEC0109', 'GeoAtlas': 'ATLAS-GEO-02-01-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections.topophioff-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections.topogap-v1', 'AtlasRelease': 'AtlasTier0-15.0.0.4', 'IOVDbGlobalTag': 'COMCOND-ES1C-001-00', '/MUONALIGN/TGC/SIDEA': 'MuonAlignTGCEndCapAAlign-REPRO-01', '/SCT/DAQ/Calibration/NoiseOccupancyDefects': 'HEAD', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections.etaoff-v4_1', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections.trcorr-v5', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/MUONALIGN/MDT/ENDCAP/SIDEA': 'MuonAlignMDTEndCapAAlign-REPRO-08', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-CSC05-BERT', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections.topolw-v1', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-CSC05-BERT', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFrac-CSC05-BERT', '/MUONALIGN/TGC/SIDEC': 'MuonAlignTGCEndCapCAlign-REPRO-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-CSC05-BERT', '/SCT/DAQ/Calibration/NPtGainDefects': 'HEAD', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections.etamod-v4', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections.phimod-v4', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections.rfac-v4', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections.calhits-v5', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections.phioff-v4', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections.topoetaoff-v1', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections.gap-v4', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections.topophimod-v1'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root', 'file_guid': '5A6CD469-D01D-DE11-82E4-000423D67746', 'beam_type': ['N/A'], 'lumi_block': [1L], 'conditions_tag': 'COMCOND-ES1C-001-00', 'det_descr_tags': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPermanent-01', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', '/MUONALIGN/MDT/ENDCAP/SIDEC': 'MuonAlignMDTEndCapCAlign-REPRO-08', '/MUONALIGN/MDT/BARREL': 'MuonAlignMDTBarrelAlign-0100-SEC0109', 'GeoAtlas': 'ATLAS-GEO-02-01-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections.topophioff-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections.topogap-v1', 'AtlasRelease': 'AtlasTier0-15.0.0.4', 'IOVDbGlobalTag': 'COMCOND-ES1C-001-00', '/MUONALIGN/TGC/SIDEA': 'MuonAlignTGCEndCapAAlign-REPRO-01', '/SCT/DAQ/Calibration/NoiseOccupancyDefects': 'HEAD', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections.etaoff-v4_1', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections.trcorr-v5', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/MUONALIGN/MDT/ENDCAP/SIDEA': 'MuonAlignMDTEndCapAAlign-REPRO-08', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-CSC05-BERT', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections.topolw-v1', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-CSC05-BERT', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFrac-CSC05-BERT', '/MUONALIGN/TGC/SIDEC': 'MuonAlignTGCEndCapCAlign-REPRO-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-CSC05-BERT', '/SCT/DAQ/Calibration/NPtGainDefects': 'HEAD', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections.etamod-v4', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections.phimod-v4', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections.rfac-v4', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections.calhits-v5', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections.phioff-v4', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections.topoetaoff-v1', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections.gap-v4', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections.topophimod-v1'}, 'nentries': 10L, 'eventdata_items': [('EventInfo', 'ByteStreamEventInfo'), ('PixelRDO_Container', 'PixelRDOs'), ('SCT_RDO_Container', 'SCT_RDOs'), ('TRT_RDO_Container', 'TRT_RDOs'), ('InDet::PixelClusterContainer', 'PixelClusters'), ('InDet::SCT_ClusterContainer', 'SCT_Clusters'), ('BCM_RDO_Container', 'BCM_RDOs'), ('LArDigitContainer', 'LArDigitContainer_IIC'), ('LArDigitContainer', 'LArDigitContainer_Thinned'), ('CaloCellContainer', 'AllCalo'), ('CaloTowerContainer', 'CombinedTower'), ('CaloClusterContainer', 'CaloCalTopoCluster'), ('CaloClusterContainer', 'CaloTopoCluster'), ('CaloClusterContainer', 'EMTopoCluster430'), ('CaloClusterContainer', 'LArClusterEM'), ('CaloClusterContainer', 'LArClusterEM7_11Nocorr'), ('CaloClusterContainer', 'LArClusterEMFrwd'), ('CaloClusterContainer', 'LArClusterEMSofte'), ('CaloClusterContainer', 'LArMuClusterCandidates'), ('CaloClusterContainer', 'MuonClusterCollection'), ('CaloClusterContainer', 'Tau1P3PCellCluster'), ('CaloClusterContainer', 'Tau1P3PCellEM012ClusterContainer'), ('CaloClusterContainer', 'Tau1P3PPi0ClusterContainer'), ('CaloClusterContainer', 'egClusterCollection'), ('TileDigitsContainer', 'TileDigitsFlt'), ('TileCellContainer', 'MBTSContainer'), ('TileL2Container', 'TileL2Cnt'), ('TileMuContainer', 'TileMuObj'), ('TileCosmicMuonContainer', 'TileCosmicMuonHT'), ('ElectronContainer', 'ElectronAODCollection'), ('ElectronContainer', 'ElectronCollection'), ('PhotonContainer', 'PhotonAODCollection'), ('PhotonContainer', 'PhotonCollection'), ('ElectronContainer', 'egammaForwardCollection'), ('ElectronContainer', 'softeCollection'), ('Analysis::TauJetContainer', 'TauRecContainer'), ('JetKeyDescriptor', 'JetKeyMap'), ('MissingETSig', 'METSig'), ('MissingEtCalo', 'MET_Base'), ('MissingEtCalo', 'MET_Base0'), ('MissingEtCalo', 'MET_Calib'), ('MissingET', 'MET_CellOut'), ('MissingET', 'MET_CellOut_MiniJet'), ('MissingEtCalo', 'MET_CorrTopo'), ('MissingET', 'MET_Cryo'), ('MissingET', 'MET_CryoCone'), ('MissingET', 'MET_Final'), ('MissingEtCalo', 'MET_LocHadTopo'), ('MissingET', 'MET_LocHadTopoObj'), ('MissingET', 'MET_Muon'), ('MissingET', 'MET_MuonBoy'), ('MissingET', 'MET_MuonBoy_Spectro'), ('MissingET', 'MET_MuonBoy_Track'), ('MissingET', 'MET_RefEle'), ('MissingET', 'MET_RefFinal'), ('MissingET', 'MET_RefGamma'), ('MissingET', 'MET_RefJet'), ('MissingET', 'MET_RefMuon'), ('MissingET', 'MET_RefMuon_Track'), ('MissingET', 'MET_RefTau'), ('MissingEtCalo', 'MET_Topo'), ('MissingET', 'MET_TopoObj'), ('MissingET', 'ObjMET_Elec'), ('MissingET', 'ObjMET_Final'), ('MissingET', 'ObjMET_IdTrk'), ('MissingET', 'ObjMET_Jet'), ('MissingET', 'ObjMET_MiniJet'), ('MissingET', 'ObjMET_Muon'), ('MissingET', 'ObjMET_Rest'), ('MissingET', 'ObjMET_TauJet'), ('Trk::SegmentCollection', 'ConvertedMBoySegments'), ('Trk::SegmentCollection', 'MooreSegments'), ('Trk::SegmentCollection', 'MuGirlSegments'), ('TrackCollection', 'CombinedInDetTracks'), ('TrackCollection', 'CombinedInDetTracks_CTB'), ('TrackCollection', 'Combined_Tracks'), ('TrackCollection', 'ConvertedMBoyMuonSpectroOnlyTracks'), ('TrackCollection', 'ConvertedMBoyTracks'), ('TrackCollection', 'ConvertedMuIdCBTracks'), ('TrackCollection', 'ConvertedMuTagTracks'), ('TrackCollection', 'ConvertedStacoTracks'), ('TrackCollection', 'MooreExtrapolatedTracks'), ('TrackCollection', 'MooreTracks'), ('TrackCollection', 'MuGirlRefittedTracks'), ('TrackCollection', 'MuTagIMOTracks'), ('TrackCollection', 'MuidExtrapolatedTracks'), ('TrackCollection', 'ResolvedPixelTracks_CTB'), ('TrackCollection', 'ResolvedSCTTracks_CTB'), ('TrackCollection', 'TRTStandaloneTRTTracks_CTB'), ('InDet::PixelGangedClusterAmbiguities', 'PixelClusterAmbiguitiesMap'), ('LArFebErrorSummary', 'LArFebErrorSummary'), ('ComTime', 'TRT_Phase'), ('Analysis::TauDetailsContainer', 'TauRecDetailsContainer'), ('Analysis::TauDetailsContainer', 'TauRecExtraDetailsContainer'), ('Analysis::MuonContainer', 'CaloESDMuonCollection'), ('Analysis::MuonContainer', 'CaloESDMuonCollection2'), ('Analysis::MuonContainer', 'CaloMuonCollection'), ('Analysis::MuonContainer', 'MuGirlLowBetaCollection'), ('Analysis::MuonContainer', 'MuidESDMuonCollection'), ('Analysis::MuonContainer', 'MuidMuonCollection'), ('Analysis::MuonContainer', 'StacoESDMuonCollection'), ('Analysis::MuonContainer', 'StacoMuonCollection'), ('MissingETSigHypoContainer', 'EtMissHypoCollection'), ('TRT_BSIdErrContainer', 'TRT_ByteStreamIdErrs'), ('InDet::TRT_DriftCircleContainer', 'TRT_DriftCircles'), ('MissingETSigObjContainer', 'EtMissObjCollection'), ('Muon::MdtPrepDataContainer', 'MDT_DriftCircles'), ('JetCollection', 'Cone4H1TopoJets'), ('JetCollection', 'Cone4H1TowerJets'), ('JetCollection', 'Cone7H1TowerJets'), ('egDetailContainer', 'SofteDetailContainer'), ('egDetailContainer', 'egDetailAOD'), ('egDetailContainer', 'egDetailContainer'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollection'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollectionNextBC'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollectionPriorBC'), ('Muon::RpcPrepDataContainer', 'RPC_Measurements'), ('CaloShowerContainer', 'CaloCalTopoCluster_Data'), ('CaloShowerContainer', 'CaloTopoCluster_Data'), ('CaloShowerContainer', 'EMTopoCluster430_Data'), ('CaloShowerContainer', 'LArClusterEM7_11Nocorr_Data'), ('CaloShowerContainer', 'LArClusterEMSofte_Data'), ('CaloShowerContainer', 'LArClusterEM_Data'), ('CaloShowerContainer', 'LArMuClusterCandidates_Data'), ('CaloShowerContainer', 'MuonClusterCollection_Data'), ('CaloShowerContainer', 'Tau1P3PCellCluster_Data'), ('CaloShowerContainer', 'Tau1P3PCellEM012ClusterContainer_Data'), ('CaloShowerContainer', 'Tau1P3PPi0ClusterContainer_Data'), ('CaloShowerContainer', 'egClusterCollection_Data'), ('InDetBSErrContainer', 'PixelByteStreamErrs'), ('InDetBSErrContainer', 'SCT_ByteStreamErrs'), ('TRT_BSErrContainer', 'TRT_ByteStreamErrs'), ('CaloCellLinkContainer', 'CaloCalTopoCluster_Link'), ('CaloCellLinkContainer', 'CaloTopoCluster_Link'), ('CaloCellLinkContainer', 'EMTopoCluster430_Link'), ('CaloCellLinkContainer', 'LArClusterEM7_11Nocorr_Link'), ('CaloCellLinkContainer', 'LArClusterEMSofte_Link'), ('CaloCellLinkContainer', 'LArClusterEM_Link'), ('CaloCellLinkContainer', 'LArMuClusterCandidates_Link'), ('CaloCellLinkContainer', 'MuonClusterCollection_Link'), ('CaloCellLinkContainer', 'Tau1P3PCellCluster_Link'), ('CaloCellLinkContainer', 'Tau1P3PCellEM012ClusterContainer_Link'), ('CaloCellLinkContainer', 'Tau1P3PPi0ClusterContainer_Link'), ('CaloCellLinkContainer', 'egClusterCollection_Link'), ('Rec::MuonSpShowerContainer', 'MuonSpShowers'), ('Rec::TrackParticleContainer', 'Combined_TrackParticles'), ('Rec::TrackParticleContainer', 'MooreTrackParticles'), ('Rec::TrackParticleContainer', 'MuGirlRefittedTrackParticles'), ('Rec::TrackParticleContainer', 'MuTagIMOTrackParticles'), ('Rec::TrackParticleContainer', 'MuTagTrackParticles'), ('Rec::TrackParticleContainer', 'MuidExtrTrackParticles'), ('Rec::TrackParticleContainer', 'MuonboyMuonSpectroOnlyTrackParticles'), ('Rec::TrackParticleContainer', 'MuonboyTrackParticles'), ('Rec::TrackParticleContainer', 'StacoTrackParticles'), ('Rec::TrackParticleContainer', 'TrackParticleCandidate'), ('Muon::TgcPrepDataContainer', 'TGC_Measurements'), ('Muon::TgcPrepDataContainer', 'TGC_MeasurementsNextBC'), ('Muon::TgcPrepDataContainer', 'TGC_MeasurementsPriorBC'), ('MuonCaloEnergyContainer', 'MuonCaloEnergyCollection'), ('DataHeader', 'StreamESD')], 'run_number': [91900L], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-GEO-02-01-00', 'evt_number': [2244L], 'evt_type': ('IS_DATA', 'IS_ATLAS', 'IS_PHYSICS'), 'metadata': {'/GLOBAL/DETSTATUS/LBSUMM': []}}
        _compare_fileinfos(f1,f1_ref)
        assert f1.run_number==f1_ref['run_number']
        assert f1.evt_number==f1_ref['evt_number']
        assert f1.lumi_block==f1_ref['lumi_block']
        assert f1.run_type  ==f1_ref['run_type']
        assert f1.beam_type ==f1_ref['beam_type']
        assert f1.beam_energy==f1_ref['beam_energy']
        pass # test1

    def test002(self):
        """test bytestream file on CASTOR"""
        if 1:
            return # FIXME
    
        import PyUtils.AthFile as af
        fname = 'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data'
        assert af.exists(fname)
        assert af.ftype(fname) == ('bs', 'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data')
        
        f2 = af.fopen(fname)
        if verbose:
            print "::: f2.fileinfos:"
            print f2.fileinfos
        f2_ref = {'file_md5sum':'e3e301bca63e4b5acb3b3cba43127ff9', 'metadata_items': None, 'stream_names': None, 'run_type': ['TEST'], 'stream_tags': [{'obeys_lbk': True, 'stream_type': 'physics', 'stream_name': 'IDCosmic'}, {'obeys_lbk': False, 'stream_type': 'calibration', 'stream_name': 'IDTracks'}], 'tag_info': None, 'file_type': 'bs', 'file_name': 'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data', 'file_guid': '7B1EABBD-12E0-4184-ABF0-84EB677D92E7', 'beam_type': [0], 'lumi_block': [54], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 417, 'eventdata_items': None, 'run_number': [92226], 'beam_energy': [0], 'geometry': None, 'evt_number': [8349492], 'evt_type': [], 'metadata': None}
        _compare_fileinfos(f2,f2_ref)
        assert f2.run_number==f2_ref['run_number']
        assert f2.evt_number==f2_ref['evt_number']
        assert f2.lumi_block==f2_ref['lumi_block']
        assert f2.run_type  ==f2_ref['run_type']
        assert f2.beam_type ==f2_ref['beam_type']
        assert f2.beam_energy==f2_ref['beam_energy']

    def test003(self):
        """test RDO POOL file"""
        import PyUtils.AthFile as af
        fname = '/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root'
        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', '/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root')
        
        f3 = af.fopen(fname)
        if verbose:
            print "::: f3.fileinfos:"
            print f3.fileinfos
        f3_ref = {'file_md5sum':'85f7b3d2da72cb387a8345091c2e00ca','metadata_items': [('EventStreamInfo', 'Stream1'), ('DataHeader', ';00;MetaDataSvc'), ('IOVMetaDataContainer', '/Digitization/Parameters'), ('IOVMetaDataContainer', '/Simulation/Parameters')], 'stream_names': ['Stream1'], 'run_type': ['N/A'], 'stream_tags': [], 'tag_info': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPerm-02_test', '/TRT/Cond/Status': 'TrtStrawStatus-02', '/LAR/Identifier/FebRodAtlas': 'FebRodAtlas-005', '/LAR/ElecCalibMC': 'LARElecCalibMC-CSC02-J-QGSP_BERT', 'GeoAtlas': 'ATLAS-GEO-02-01-00', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', 'AtlasRelease': 'AtlasProduction-14.2.25.3', 'TGC_support': 'TGC Big Wheel', '/GLOBAL/BField/Map': 'BFieldMap-000', 'IOVDbGlobalTag': 'OFLCOND-SIM-00-00-00', 'MDT_support': 'MDT Big Wheel', '/LAR/Identifier/OnOffIdAtlas': 'OnOffIdAtlas-012'}, 'file_type': 'pool', 'file_name': '/afs/cern.ch/atlas/offline/ReleaseData/v3/testfile/valid1.005200.T1_McAtNlo_Jimmy.digit.RDO.e322_s488_d151_tid039414_RDO.039414._00001_extract_10evt.pool.root', 'file_guid': 'E29E4282-D8ED-DD11-8435-000423D59D52', 'beam_type': ['N/A'], 'lumi_block': [0L], 'conditions_tag': 'OFLCOND-SIM-00-00-00', 'det_descr_tags': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPerm-02_test', '/TRT/Cond/Status': 'TrtStrawStatus-02', '/LAR/Identifier/FebRodAtlas': 'FebRodAtlas-005', '/LAR/ElecCalibMC': 'LARElecCalibMC-CSC02-J-QGSP_BERT', 'GeoAtlas': 'ATLAS-GEO-02-01-00', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', 'AtlasRelease': 'AtlasProduction-14.2.25.3', 'TGC_support': 'TGC Big Wheel', '/GLOBAL/BField/Map': 'BFieldMap-000', 'IOVDbGlobalTag': 'OFLCOND-SIM-00-00-00', 'MDT_support': 'MDT Big Wheel', '/LAR/Identifier/OnOffIdAtlas': 'OnOffIdAtlas-012'}, 'nentries': 10L, 'eventdata_items': [('EventInfo', 'McEventInfo'), ('PixelRDO_Container', 'PixelRDOs'), ('SCT_RDO_Container', 'SCT_RDOs'), ('TRT_RDO_Container', 'TRT_RDOs'), ('InDetSimDataCollection', 'BCM_SDO_Map'), ('InDetSimDataCollection', 'PixelSDO_Map'), ('InDetSimDataCollection', 'SCT_SDO_Map'), ('InDetSimDataCollection', 'TRT_SDO_Map'), ('BCM_RDO_Container', 'BCM_RDOs'), ('LArDigitContainer', 'LArDigitContainer_MC_Thinned'), ('LArRawChannelContainer', 'LArRawChannels'), ('LArTTL1Container', 'LArTTL1EM'), ('LArTTL1Container', 'LArTTL1HAD'), ('TileRawChannelContainer', 'TileRawChannelCnt'), ('TileTTL1Container', 'TileTTL1Cnt'), ('TileTTL1Container', 'TileTTL1MBTS'), ('TileHitVector', 'MBTSHits'), ('CscRawDataContainer', 'CSCRDO'), ('TgcRdoContainer', 'TGCRDO'), ('MdtCsmContainer', 'MDTCSM'), ('RpcPadContainer', 'RPCPAD'), ('ROIB::RoIBResult', 'RoIBResult'), ('CTP_RDO', 'CTP_RDO'), ('DataVector<LVL1::JetElement>', 'JetElements'), ('DataVector<LVL1::TriggerTower>', 'TriggerTowers'), ('MuCTPI_RDO', 'MUCTPI_RDO'), ('McEventCollection', 'TruthEvent'), ('DataVector<LVL1::JEMEtSums>', 'JEMEtSums'), ('MuonSimDataCollection', 'MDT_SDO'), ('MuonSimDataCollection', 'RPC_SDO'), ('MuonSimDataCollection', 'TGC_SDO'), ('DataVector<LVL1::CPMTower>', 'CPMTowers'), ('DataVector<LVL1::CPMHits>', 'CPMHits'), ('DataVector<LVL1::CMMEtSums>', 'CMMEtSums'), ('DataVector<LVL1::JEMRoI>', 'JEMRoIs'), ('LVL1::CMMRoI', 'CMMRoIs'), ('DataVector<LVL1::JEMHits>', 'JEMHits'), ('DataVector<LVL1::CPMRoI>', 'CPMRoIs'), ('DataVector<LVL1::CMMJetHits>', 'CMMJetHits'), ('DataVector<LVL1::CMMCPHits>', 'CMMCPHits'), ('CscSimDataCollection', 'CSC_SDO'), ('TrackRecordCollection', 'CaloEntryLayer'), ('TrackRecordCollection', 'MuonEntryLayer'), ('TrackRecordCollection', 'MuonExitLayer'), ('CaloCalibrationHitContainer', 'LArCalibrationHitActive'), ('CaloCalibrationHitContainer', 'LArCalibrationHitDeadMaterial'), ('CaloCalibrationHitContainer', 'LArCalibrationHitInactive'), ('DataHeader', 'Stream1')], 'run_number': [5200L], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-GEO-02-01-00', 'evt_number': [30002L], 'evt_type': ('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS'), 'metadata': {'/Digitization/Parameters': {'physicsList': 'QGSP_BERT', 'N_beamGasInputFiles': 0, 'doBeamHalo': False, 'N_cavernInputFiles': 0, 'overrideMetadata': False, 'numberOfBeamHalo': 1.0, 'doCavern': False, 'IOVDbGlobalTag': 'default', 'N_beamHaloInputFiles': 0, 'initialBunchCrossing': -36, 'doCaloNoise': True, 'N_minBiasInputFiles': 0, 'numberOfCollisions': 0.0, 'rndmSvc': 'AtRanluxGenSvc', 'rndmSeedList': ['BCM_Digitization 49261511 105132395', 'PixelDigitization 10513240 492615105', 'SCT_Digitization 49261511 105132395', 'TRT_ElectronicsNoise 124 346', 'TRT_Noise 1235 3457', 'TRT_ThresholdFluctuations 12346 34568', 'TRT_ProcessStraw 123457 345679', 'TRT_SimDriftTime 1234568 3456790', 'TRT_PAI 12345679 34567891', 'TRT_FakeConditions 123456790 345678902', 'LArDigitization 1235 5679', 'Tile_HitVecToCnt 4789900 989240513', 'Tile_DigitsMaker 4789900 989240513', 'CSC_Digitization 49261511 105132395', 'MDTResponse 49261511 105132395', 'MDT_Digitization 49261511 105132395', 'MDT_DigitizationTwin 393242562 857132382', 'TGC_Digitization 49261511 105132395', 'RPC_Digitization 49261511 105132395', 'CscDigitToCscRDOTool 49261511 105132395', 'Tile_HitToTTL1 4789900 989240513', 'CTPSimulation 1979283044 1924452190'], 'numberOfCavern': 2, 'doMuonNoise': True, 'doInDetNoise': True,'numberOfBeamGas': 1.0, 'finalBunchCrossing': 32, 'doBeamGas': False, 'doMinimumBias': False, 'bunchSpacing': 25, 'DetDescrVersion': 'ATLAS-GEO-02-01-00', 'lvl1TriggerMenu': 'lumi1E31_no_Bphysics_no_prescale', 'rndmSeedOffset2': 1, 'rndmSeedOffset1': 1}, '/Simulation/Parameters': {'EtaPhiStatus': True, 'PhysicsList': 'QGSP_BERT', 'CalibrationRun': 'DeadLAr', 'SimLayout': 'ATLAS-GEO-02-01-00', 'DoLArBirk': False, 'LArParameterization': 0, 'MagneticField': 'OracleDB', 'WorldRRange': 'default', 'SeedsG4': 'default', 'NeutronTimeCut': 150.0, 'WorldZRange': 'default', 'Seeds': 'default', 'G4Version': 'geant4.9.1.patch03.atlas01', 'RunType': 'atlas', 'VertexStatus': True, 'IOVDbGlobalTag': 'default', 'VRangeStatus': True}}}
        _compare_fileinfos(f3,f3_ref)
        assert f3.run_number==f3_ref['run_number']
        assert f3.evt_number==f3_ref['evt_number']
        assert f3.lumi_block==f3_ref['lumi_block']
        assert f3.run_type  ==f3_ref['run_type']
        assert f3.beam_type ==f3_ref['beam_type']
        assert f3.beam_energy==f3_ref['beam_energy']

    def test004(self):
        """test empty POOL file"""
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/empty-file.pool'
        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/empty-file.pool')
        
        f4 = af.fopen(fname)
        if verbose:
            print "::: f4.fileinfos:"
            print f4.fileinfos
        f4_ref = {'file_md5sum':'519643438bf3a0e7a1e637463d73d9e9','metadata_items': [('DataHeader', ';00;MetaDataSvc'), ('EventBookkeeperCollection', 'EventBookkeepers'), ('EventBookkeeperCollection', 'EventSelector.Counter'), ('EventStreamInfo', 'DPD_EGAMTAUCOMM'), ('IOVMetaDataContainer', '/GLOBAL/DETSTATUS/LBSUMM'), ('IOVMetaDataContainer', '/TRIGGER/HLT/HltConfigKeys'), ('IOVMetaDataContainer', '/TRIGGER/HLT/Menu'), ('IOVMetaDataContainer', '/TRIGGER/LVL1/Lvl1ConfigKey'), ('IOVMetaDataContainer', '/TRIGGER/LVL1/Menu'), ('IOVMetaDataContainer', '/TRIGGER/LVL1/Prescales'), ('IOVMetaDataContainer', '/TagInfo'), ('LumiBlockCollection', 'IncompleteLumiBlocks')], 'stream_names': ['DPD_EGAMTAUCOMM'], 'run_type': ['N/A'], 'stream_tags': [], 'tag_info': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPerm-02_test', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', 'GeoAtlas': 'ATLAS-GEO-03-00-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections.topophioff-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections.topogap-v1', 'AtlasRelease': 'any', 'IOVDbGlobalTag': 'COMCOND-ES1C-000-00', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections.etaoff-v4_1', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections.phioff-v4', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-003-01', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections.topolw-v1', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-003-01', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFRac-003-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-003-01', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections.phimod-v4', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections.etamod-v4', 'AMITag': 'f57', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections.rfac-v4', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections.calhits-v5', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections.trcorr-v5', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections.topoetaoff-v1', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections.gap-v4', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections.topophimod-v1'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/empty-file.pool', 'file_guid': 'CC6B79F4-043E-DE11-BD81-000423D67862', 'conditions_tag': 'COMCOND-ES1C-000-00', 'beam_type': ['N/A'], 'lumi_block': [], 'det_descr_tags': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPerm-02_test', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', 'GeoAtlas': 'ATLAS-GEO-03-00-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections.topophioff-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections.topogap-v1', 'AtlasRelease': 'any', 'IOVDbGlobalTag': 'COMCOND-ES1C-000-00', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections.etaoff-v4_1', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections.phioff-v4', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-003-01', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections.topolw-v1', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-003-01', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFRac-003-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-003-01', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections.phimod-v4', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections.etamod-v4', 'AMITag': 'f57', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections.rfac-v4', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections.calhits-v5', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections.trcorr-v5', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections.topoetaoff-v1', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections.gap-v4', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections.topophimod-v1'}, 'nentries': 0, 'eventdata_items': [], 'run_number': [], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-GEO-03-00-00', 'evt_number': [], 'evt_type': [], 'metadata': None}

        f4.fileinfos['tag_info']['AtlasRelease'] = 'any'
        f4.fileinfos['det_descr_tags']['AtlasRelease'] = 'any'
        _compare_fileinfos(f4,f4_ref)
        assert f4.run_number==f4_ref['run_number']
        assert f4.evt_number==f4_ref['evt_number']
        assert f4.lumi_block==f4_ref['lumi_block']
        assert f4.run_type  ==f4_ref['run_type']
        assert f4.beam_type ==f4_ref['beam_type']
        assert f4.beam_energy==f4_ref['beam_energy']

        return

    def test005(self):
        """test (old) HITS POOL file"""
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10'

        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10')
        
        f5 = af.fopen(fname)
        if verbose:
            print "::: f5.fileinfos:"
            print f5.fileinfos
        f5_ref = {'file_md5sum':'b109aa2689abeb8aa282605c29087d64', 'metadata_items': [], 'stream_names': ['Stream1'], 'run_type': ['N/A'], 'stream_tags': [], 'tag_info': {'AtlasRelease': 'AtlasOffline-12.0.31', 'GeoAtlas': 'ATLAS-CSC-01-02-00', 'IOVDbGlobalTag': 'OFLCOND-CSC-00-01-00'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10', 'file_guid': '00C5C040-EB75-DB11-9308-00E0812B9987', 'beam_type': ['N/A'], 'lumi_block': [0L], 'conditions_tag': 'OFLCOND-CSC-00-01-00', 'det_descr_tags': {'AtlasRelease': 'AtlasOffline-12.0.31', 'GeoAtlas': 'ATLAS-CSC-01-02-00', 'IOVDbGlobalTag': 'OFLCOND-CSC-00-01-00'}, 'nentries': 50L, 'eventdata_items': [('EventInfo', 'McEventInfo'), ('SiHitCollection', 'PixelHits'), ('SiHitCollection', 'SCT_Hits'), ('LArHitContainer', 'LArHitEMB'), ('LArHitContainer', 'LArHitEMEC'), ('LArHitContainer', 'LArHitFCAL'), ('LArHitContainer', 'LArHitHEC'), ('TileHitVector', 'MBTSHits'), ('TileHitVector', 'TileHitVec'), ('RPCSimHitCollection', 'RPC_Hits'), ('TGCSimHitCollection', 'TGC_Hits'), ('CSCSimHitCollection', 'CSC_Hits'), ('MDTSimHitCollection', 'MDT_Hits'), ('McEventCollection', 'TruthEvent'), ('TRTUncompressedHitCollection', 'TRTUncompressedHits'), ('TrackRecordCollection', 'CaloEntryLayer'), ('TrackRecordCollection', 'MuonEntryLayer'), ('TrackRecordCollection', 'MuonExitLayer'), ('CaloCalibrationHitContainer', 'LArCalibrationHitActive'), ('CaloCalibrationHitContainer', 'LArCalibrationHitDeadMaterial'), ('CaloCalibrationHitContainer', 'LArCalibrationHitInactive'), ('CaloCalibrationHitContainer', 'TileCalibrationCellHitCnt'), ('CaloCalibrationHitContainer', 'TileCalibrationDMHitCnt'), ('DataHeader', 'Stream1')], 'run_number': [5200L], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-CSC-01-02-00', 'evt_number': [6136L], 'evt_type': ('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS'), 'metadata': {}}
        _compare_fileinfos(f5,f5_ref)
        assert f5.run_number==f5_ref['run_number']
        assert f5.evt_number==f5_ref['evt_number']
        assert f5.lumi_block==f5_ref['lumi_block']
        assert f5.run_type  ==f5_ref['run_type']
        assert f5.beam_type ==f5_ref['beam_type']
        assert f5.beam_energy==f5_ref['beam_energy']
        return

    def test006(self):
        """test old EVGEN POOL file"""
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/testSim.0011.mu_pt5_eta60.EVGEN.pool.root'

        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/testSim.0011.mu_pt5_eta60.EVGEN.pool.root')
        f6 = af.fopen(fname)
        if verbose:
            print "::: f6.fileinfos:"
            print f6.fileinfos
        f6_ref = {'file_md5sum':'b6b58e325235b4fbbf0aebd5e028ab08', 'metadata_items': [], 'stream_names': ['Stream1'], 'run_type': ['N/A'], 'stream_tags': [], 'tag_info': {'AtlasRelease': 'any'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/testSim.0011.mu_pt5_eta60.EVGEN.pool.root', 'file_guid': 'ACC40752-51BB-DB11-8437-000423D65662', 'beam_type': ['N/A'], 'lumi_block': [0L], 'conditions_tag': None, 'det_descr_tags': {'AtlasRelease': 'any'}, 'nentries': 1053L, 'eventdata_items': [('EventInfo', 'McEventInfo'), ('McEventCollection', 'GEN_EVENT'), ('DataHeader', 'Stream1')], 'run_number': [11L], 'beam_energy': ['N/A'], 'geometry': None, 'evt_number': [1L], 'evt_type': ('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS'), 'metadata': {}}
        f6.fileinfos['tag_info']['AtlasRelease'] = 'any'
        f6.fileinfos['det_descr_tags']['AtlasRelease'] = 'any'
        _compare_fileinfos(f6,f6_ref)
        assert f6.run_number==f6_ref['run_number']
        assert f6.evt_number==f6_ref['evt_number']
        assert f6.lumi_block==f6_ref['lumi_block']
        assert f6.run_type  ==f6_ref['run_type']
        assert f6.beam_type ==f6_ref['beam_type']
        assert f6.beam_energy==f6_ref['beam_energy']
        return

    def test007(self):
        """test ESD commissionning POOL file"""
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.commissionning.15.2.0.pool'

        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.commissionning.15.2.0.pool')
        
        f7 = af.fopen(fname)
        if verbose:
            print "::: f7.fileinfos:"
            print f7.fileinfos
        f7_ref = {'file_md5sum':'c52c2056f049094abe559af10216937c', 'metadata_items': [('EventStreamInfo', 'StreamESD'), ('LumiBlockCollection', 'LumiBlocks'), ('DataHeader', ';00;MetaDataSvc'), ('IOVMetaDataContainer', '/GLOBAL/DETSTATUS/LBSUMM'), ('IOVMetaDataContainer', '/TagInfo')], 'stream_names': ['StreamESD'], 'run_type': ['N/A'], 'stream_tags': [{'obeys_lbk': True, 'stream_type': 'physics', 'stream_name': 'IDCosmic'}], 'tag_info': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPermanent-01', '/GLOBAL/BTagCalib/IP3D': 'BTagCalib-03-00', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', '/MUONALIGN/MDT/ENDCAP/SIDEC': 'MuonAlignMDTEndCapCAlign-REPRO-08', '/MUONALIGN/MDT/BARREL': 'MuonAlignMDTBarrelAlign-0100-SEC0109', '/CALO/H1Weights/H1WeightsCone4Topo': 'CaloH1WeightsCone4Topo-02-000', '/TILE/OFL01/CALIB/LAS/LIN': 'TileOfl01CalibLasLin-HLT-UPD1-00', 'GeoAtlas': 'ATLAS-GEO-03-00-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections-00-02-00-DC3-v2', 'AtlasRelease': 'any', 'IOVDbGlobalTag': 'COMCOND-ES1C-001-01', '/MUONALIGN/TGC/SIDEA': 'MuonAlignTGCEndCapAAlign-REPRO-01', '/MUONALIGN/TGC/SIDEC': 'MuonAlignTGCEndCapCAlign-REPRO-01', '/CALO/CaloSwClusterCorrections/larupdate': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/clcon': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/CES': 'TileOfl01CalibCes-HLT-UPD1-01', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SV1': 'BTagCalib-03-00', '/MUONALIGN/MDT/ENDCAP/SIDEA': 'MuonAlignMDTEndCapAAlign-REPRO-08', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-CSC05-BERT', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-CSC05-BERT', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFrac-CSC05-BERT', '/GLOBAL/BTagCalib/JetProb': 'BTagCalib-03-00', '/CALO/EMTopoClusterCorrections/larupdate': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/GLOBAL/BTagCalib/SoftEl': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/lwc': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/EMS': 'TileOfl01CalibEms-HLT-UPD1-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-CSC05-BERT', '/TILE/OFL01/CALIB/CIS/FIT/LIN': 'TileOfl01CalibCisFitLin-HLT-UPD1-00', '/GLOBAL/BTagCalib/IP2D': 'BTagCalib-03-00', '/GLOBAL/BTagCalib/JetFitter': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SoftMu': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/EMTopoClusterCorrections/topoetaoffsw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections-00-02-00-DC3-v2'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/esd.commissionning.15.2.0.pool', 'file_guid': '487184A1-9343-DE11-AACC-001E4F3E5C1F', 'beam_type': ['N/A'], 'lumi_block': [1L], 'conditions_tag': 'COMCOND-ES1C-001-01', 'det_descr_tags': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPermanent-01', '/GLOBAL/BTagCalib/IP3D': 'BTagCalib-03-00', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', '/MUONALIGN/MDT/ENDCAP/SIDEC': 'MuonAlignMDTEndCapCAlign-REPRO-08', '/MUONALIGN/MDT/BARREL': 'MuonAlignMDTBarrelAlign-0100-SEC0109', '/CALO/H1Weights/H1WeightsCone4Topo': 'CaloH1WeightsCone4Topo-02-000', '/TILE/OFL01/CALIB/LAS/LIN': 'TileOfl01CalibLasLin-HLT-UPD1-00', 'GeoAtlas': 'ATLAS-GEO-03-00-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections-00-02-00-DC3-v2', 'AtlasRelease': 'any', 'IOVDbGlobalTag': 'COMCOND-ES1C-001-01', '/MUONALIGN/TGC/SIDEA': 'MuonAlignTGCEndCapAAlign-REPRO-01', '/MUONALIGN/TGC/SIDEC': 'MuonAlignTGCEndCapCAlign-REPRO-01', '/CALO/CaloSwClusterCorrections/larupdate': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/clcon': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/CES': 'TileOfl01CalibCes-HLT-UPD1-01', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SV1': 'BTagCalib-03-00', '/MUONALIGN/MDT/ENDCAP/SIDEA': 'MuonAlignMDTEndCapAAlign-REPRO-08', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-CSC05-BERT', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-CSC05-BERT', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFrac-CSC05-BERT', '/GLOBAL/BTagCalib/JetProb': 'BTagCalib-03-00', '/CALO/EMTopoClusterCorrections/larupdate': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/GLOBAL/BTagCalib/SoftEl': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/lwc': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/EMS': 'TileOfl01CalibEms-HLT-UPD1-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-CSC05-BERT', '/TILE/OFL01/CALIB/CIS/FIT/LIN': 'TileOfl01CalibCisFitLin-HLT-UPD1-00', '/GLOBAL/BTagCalib/IP2D': 'BTagCalib-03-00', '/GLOBAL/BTagCalib/JetFitter': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SoftMu': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/EMTopoClusterCorrections/topoetaoffsw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections-00-02-00-DC3-v2'}, 'nentries': 10L, 'eventdata_items': [('EventInfo', 'ByteStreamEventInfo'), ('PixelRDO_Container', 'PixelRDOs'), ('SCT_RDO_Container', 'SCT_RDOs'), ('TRT_RDO_Container', 'TRT_RDOs'), ('InDet::PixelClusterContainer', 'PixelClusters'), ('InDet::SCT_ClusterContainer', 'SCT_Clusters'), ('BCM_RDO_Container', 'BCM_RDOs'), ('LArDigitContainer', 'LArDigitContainer_IIC'), ('LArDigitContainer', 'LArDigitContainer_Thinned'), ('CaloCellContainer', 'AllCalo'), ('CaloTowerContainer', 'CombinedTower'), ('CaloClusterContainer', 'CaloCalTopoCluster'), ('CaloClusterContainer', 'CaloTopoCluster'), ('CaloClusterContainer', 'EMTopoCluster430'), ('CaloClusterContainer', 'LArClusterEM'), ('CaloClusterContainer', 'LArClusterEM7_11Nocorr'), ('CaloClusterContainer', 'LArClusterEMFrwd'), ('CaloClusterContainer', 'LArClusterEMSofte'), ('CaloClusterContainer', 'LArMuClusterCandidates'), ('CaloClusterContainer', 'MuonClusterCollection'), ('CaloClusterContainer', 'Tau1P3PCellCluster'), ('CaloClusterContainer', 'Tau1P3PCellEM012ClusterContainer'), ('CaloClusterContainer', 'Tau1P3PPi0ClusterContainer'), ('CaloClusterContainer', 'egClusterCollection'), ('TileDigitsContainer', 'TileDigitsFlt'), ('TileCellContainer', 'MBTSContainer'), ('TileL2Container', 'TileL2Cnt'), ('TileMuContainer', 'TileMuObj'), ('TileCosmicMuonContainer', 'TileCosmicMuonHT'), ('ElectronContainer', 'ElectronAODCollection'), ('ElectronContainer', 'ElectronCollection'), ('PhotonContainer', 'PhotonAODCollection'), ('PhotonContainer', 'PhotonCollection'), ('ElectronContainer', 'egammaForwardCollection'), ('ElectronContainer', 'softeCollection'), ('Analysis::TauJetContainer', 'TauRecContainer'), ('JetKeyDescriptor', 'JetKeyMap'), ('MissingEtCalo', 'MET_Base'), ('MissingEtCalo', 'MET_Base0'), ('MissingEtCalo', 'MET_Calib'), ('MissingET', 'MET_CellOut'), ('MissingEtCalo', 'MET_CorrTopo'), ('MissingET', 'MET_Cryo'), ('MissingET', 'MET_CryoCone'), ('MissingET', 'MET_Final'), ('MissingEtCalo', 'MET_LocHadTopo'), ('MissingET', 'MET_LocHadTopoObj'), ('MissingET', 'MET_Muon'), ('MissingET', 'MET_MuonBoy'), ('MissingET', 'MET_MuonBoy_Spectro'), ('MissingET', 'MET_MuonBoy_Track'), ('MissingET', 'MET_RefEle'), ('MissingET', 'MET_RefFinal'), ('MissingET', 'MET_RefGamma'), ('MissingET', 'MET_RefJet'), ('MissingET', 'MET_RefTau'), ('MissingEtCalo', 'MET_Topo'), ('MissingET', 'MET_TopoObj'), ('Trk::SegmentCollection', 'ConvertedMBoySegments'), ('Trk::SegmentCollection', 'MooreSegments'), ('Trk::SegmentCollection', 'MuGirlSegments'), ('TrackCollection', 'CombinedInDetTracks'), ('TrackCollection', 'CombinedInDetTracks_CTB'), ('TrackCollection', 'Combined_Tracks'), ('TrackCollection', 'ConvertedMBoyMuonSpectroOnlyTracks'), ('TrackCollection', 'ConvertedMBoyTracks'), ('TrackCollection', 'ConvertedMuIdCBTracks'), ('TrackCollection', 'ConvertedMuTagTracks'), ('TrackCollection', 'ConvertedStacoTracks'), ('TrackCollection', 'MooreExtrapolatedTracks'), ('TrackCollection', 'MooreTracks'), ('TrackCollection', 'MuGirlRefittedTracks'), ('TrackCollection', 'MuTagIMOTracks'), ('TrackCollection', 'MuidExtrapolatedTracks'), ('TrackCollection', 'ResolvedPixelTracks_CTB'), ('TrackCollection', 'ResolvedSCTTracks_CTB'), ('TrackCollection', 'TRTStandaloneTRTTracks_CTB'), ('InDet::PixelGangedClusterAmbiguities', 'PixelClusterAmbiguitiesMap'), ('LArFebErrorSummary', 'LArFebErrorSummary'), ('ComTime', 'TRT_Phase'), ('Analysis::TauDetailsContainer', 'TauRecDetailsContainer'), ('Analysis::TauDetailsContainer', 'TauRecExtraDetailsContainer'), ('Muon::CscPrepDataContainer', 'CSC_Clusters'), ('Analysis::MuonContainer', 'CaloESDMuonCollection'), ('Analysis::MuonContainer', 'CaloMuonCollection'), ('Analysis::MuonContainer', 'MuGirlLowBetaCollection'), ('Analysis::MuonContainer', 'MuidESDMuonCollection'), ('Analysis::MuonContainer', 'MuidMuonCollection'), ('Analysis::MuonContainer', 'StacoESDMuonCollection'), ('Analysis::MuonContainer', 'StacoMuonCollection'), ('TRT_BSIdErrContainer', 'TRT_ByteStreamIdErrs'), ('InDet::TRT_DriftCircleContainer', 'TRT_DriftCircles'), ('Muon::MdtPrepDataContainer', 'MDT_DriftCircles'), ('JetCollection', 'Cone4H1TopoJets'), ('JetCollection', 'Cone4H1TowerJets'), ('JetCollection', 'Cone7H1TowerJets'), ('egDetailContainer', 'SofteDetailContainer'), ('egDetailContainer', 'egDetailAOD'), ('egDetailContainer', 'egDetailContainer'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollection'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollectionNextBC'), ('Muon::TgcCoinDataContainer', 'TrigT1CoinDataCollectionPriorBC'), ('Muon::RpcCoinDataContainer', 'RPC_triggerHits'), ('Muon::CscStripPrepDataContainer', 'CSC_Measurements'), ('Muon::RpcPrepDataContainer', 'RPC_Measurements'), ('CaloShowerContainer', 'CaloCalTopoCluster_Data'), ('CaloShowerContainer', 'CaloTopoCluster_Data'), ('CaloShowerContainer', 'EMTopoCluster430_Data'), ('CaloShowerContainer', 'LArClusterEM7_11Nocorr_Data'), ('CaloShowerContainer', 'LArClusterEMSofte_Data'), ('CaloShowerContainer', 'LArClusterEM_Data'), ('CaloShowerContainer', 'LArMuClusterCandidates_Data'), ('CaloShowerContainer', 'MuonClusterCollection_Data'), ('CaloShowerContainer', 'Tau1P3PCellCluster_Data'), ('CaloShowerContainer', 'Tau1P3PCellEM012ClusterContainer_Data'), ('CaloShowerContainer', 'Tau1P3PPi0ClusterContainer_Data'), ('CaloShowerContainer', 'egClusterCollection_Data'), ('InDetBSErrContainer', 'PixelByteStreamErrs'), ('InDetBSErrContainer', 'SCT_ByteStreamErrs'), ('TRT_BSErrContainer', 'TRT_ByteStreamErrs'), ('CaloCellLinkContainer', 'CaloCalTopoCluster_Link'), ('CaloCellLinkContainer', 'CaloTopoCluster_Link'), ('CaloCellLinkContainer', 'EMTopoCluster430_Link'), ('CaloCellLinkContainer', 'LArClusterEM7_11Nocorr_Link'), ('CaloCellLinkContainer', 'LArClusterEMSofte_Link'), ('CaloCellLinkContainer', 'LArClusterEM_Link'), ('CaloCellLinkContainer', 'LArMuClusterCandidates_Link'), ('CaloCellLinkContainer', 'MuonClusterCollection_Link'), ('CaloCellLinkContainer', 'Tau1P3PCellCluster_Link'), ('CaloCellLinkContainer', 'Tau1P3PCellEM012ClusterContainer_Link'), ('CaloCellLinkContainer', 'Tau1P3PPi0ClusterContainer_Link'), ('CaloCellLinkContainer', 'egClusterCollection_Link'), ('Rec::MuonSpShowerContainer', 'MuonSpShowers'), ('Rec::TrackParticleContainer', 'Combined_TrackParticles'), ('Rec::TrackParticleContainer', 'MooreTrackParticles'), ('Rec::TrackParticleContainer', 'MuGirlRefittedTrackParticles'), ('Rec::TrackParticleContainer', 'MuTagIMOTrackParticles'), ('Rec::TrackParticleContainer', 'MuTagTrackParticles'), ('Rec::TrackParticleContainer', 'MuidExtrTrackParticles'), ('Rec::TrackParticleContainer', 'MuonboyMuonSpectroOnlyTrackParticles'), ('Rec::TrackParticleContainer', 'MuonboyTrackParticles'), ('Rec::TrackParticleContainer', 'StacoTrackParticles'), ('Rec::TrackParticleContainer', 'TrackParticleCandidate'), ('Muon::TgcPrepDataContainer', 'TGC_Measurements'), ('Muon::TgcPrepDataContainer', 'TGC_MeasurementsNextBC'), ('Muon::TgcPrepDataContainer', 'TGC_MeasurementsPriorBC'), ('MuonCaloEnergyContainer', 'MuonCaloEnergyCollection'), ('DataHeader', 'StreamESD')], 'run_number': [91900L], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-GEO-03-00-00', 'evt_number': [2244L], 'evt_type': ('IS_DATA', 'IS_ATLAS', 'IS_PHYSICS'), 'metadata': {'/GLOBAL/DETSTATUS/LBSUMM': [], '/TagInfo': {'/TRT/Cond/StatusPermanent': 'TrtStrawStatusPermanent-01', '/GLOBAL/BTagCalib/IP3D': 'BTagCalib-03-00', '/CALO/HadCalibration/CaloDMCorr2': 'CaloHadDMCorr-002-00', '/MUONALIGN/MDT/ENDCAP/SIDEC': 'MuonAlignMDTEndCapCAlign-REPRO-08', '/MUONALIGN/MDT/BARREL': 'MuonAlignMDTBarrelAlign-0100-SEC0109', '/CALO/H1Weights/H1WeightsCone4Topo': 'CaloH1WeightsCone4Topo-02-000', '/TILE/OFL01/CALIB/LAS/LIN': 'TileOfl01CalibLasLin-HLT-UPD1-00', 'GeoAtlas': 'ATLAS-GEO-03-00-00', '/CALO/EMTopoClusterCorrections/topophioff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topogap': 'EMTopoClusterCorrections-00-02-00-DC3-v2', 'AtlasRelease': 'AtlasOffline-rel_1', 'IOVDbGlobalTag': 'COMCOND-ES1C-001-01', '/MUONALIGN/TGC/SIDEA': 'MuonAlignTGCEndCapAAlign-REPRO-01', '/MUONALIGN/TGC/SIDEC': 'MuonAlignTGCEndCapCAlign-REPRO-01', '/CALO/CaloSwClusterCorrections/larupdate': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/clcon': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/etaoff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phimod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/CES': 'TileOfl01CalibCes-HLT-UPD1-01', '/CALO/CaloSwClusterCorrections/trcorr': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SV1': 'BTagCalib-03-00', '/MUONALIGN/MDT/ENDCAP/SIDEA': 'MuonAlignMDTEndCapAAlign-REPRO-08', '/CALO/HadCalibration/CaloOutOfClusterPi0': 'CaloHadOOCCorrPi0-CSC05-BERT', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/CALO/EMTopoClusterCorrections/topolw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/HadCalibration/H1ClusterCellWeights': 'CaloH1CellWeights-CSC05-BERT', '/CALO/HadCalibration/CaloEMFrac': 'CaloEMFrac-CSC05-BERT', '/GLOBAL/BTagCalib/JetProb': 'BTagCalib-03-00', '/CALO/EMTopoClusterCorrections/larupdate': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/GLOBAL/BTagCalib/SoftEl': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/lwc': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/TILE/OFL01/CALIB/EMS': 'TileOfl01CalibEms-HLT-UPD1-01', '/CALO/HadCalibration/CaloOutOfCluster': 'CaloHadOOCCorr-CSC05-BERT', '/TILE/OFL01/CALIB/CIS/FIT/LIN': 'TileOfl01CalibCisFitLin-HLT-UPD1-00', '/GLOBAL/BTagCalib/IP2D': 'BTagCalib-03-00', '/GLOBAL/BTagCalib/JetFitter': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/etamod': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/GLOBAL/BTagCalib/SoftMu': 'BTagCalib-03-00', '/CALO/CaloSwClusterCorrections/rfac': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/calhits': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/CaloSwClusterCorrections/phioff': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/H1Weights/H1WeightsConeTopo': 'CaloH1WeightsConeTopo-00-000', '/GLOBAL/TrackingGeo/LayerMaterial': 'TagInfo/AtlasLayerMat_v11_/GeoAtlas', '/CALO/EMTopoClusterCorrections/topoetaoffsw': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/EMTopoClusterCorrections/topoetaoff': 'EMTopoClusterCorrections-00-02-00-DC3-v2', '/CALO/CaloSwClusterCorrections/gap': 'CaloSwClusterCorrections-00-02-00-v6_calh', '/CALO/EMTopoClusterCorrections/topophimod': 'EMTopoClusterCorrections-00-02-00-DC3-v2'}}}
    
        f7.fileinfos['tag_info']['AtlasRelease'] = 'any'
        f7.fileinfos['det_descr_tags']['AtlasRelease'] = 'any'
        _compare_fileinfos(f7,f7_ref)
        assert f7.run_number==f7_ref['run_number']
        assert f7.evt_number==f7_ref['evt_number']
        assert f7.lumi_block==f7_ref['lumi_block']
        assert f7.run_type  ==f7_ref['run_type']
        assert f7.beam_type ==f7_ref['beam_type']
        assert f7.beam_energy==f7_ref['beam_energy']
        return

    def test008(self):
        """test RDO POOL file via xrootd"""
        import PyUtils.AthFile as af

        fname="root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.005640.CharybdisJimmy.digit.RDO.e322_s483/RDO.027377._00069.pool.root.3"

        assert af.exists(fname)
        assert af.ftype(fname) == ('pool', 'root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.005640.CharybdisJimmy.digit.RDO.e322_s483/RDO.027377._00069.pool.root.3')
        
        f8 = af.fopen(fname)
        if verbose:
            print "::: f8.fileinfos:"
            print f8.fileinfos

        f8_ref = {'file_md5sum': '7f6798d2115b5c1cdad02eb98dec5d68', 'stream_tags': [], 'tag_info': {'IOVDbGlobalTag': 'OFLCOND-SIM-00-00-00','/TRT/Cond/Status': 'TrtStrawStatus-02', '/LAR/Identifier/FebRodAtlas': 'FebRodAtlas-005', '/LAR/ElecCalibMC': 'LARElecCalibMC-CSC02-J-QGSP_BERT', 'GeoAtlas': 'ATLAS-GEO-02-01-00', 'TGC_support': 'TGC Big Wheel', 'AtlasRelease': 'any', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/GLOBAL/BField/Map': 'BFieldMap-000', 'MDT_support': 'MDT Big Wheel', '/LAR/Identifier/OnOffIdAtlas': 'OnOffIdAtlas-012'}, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.005640.CharybdisJimmy.digit.RDO.e322_s483/RDO.027377._00069.pool.root.3', 'beam_type': ['N/A'], 'det_descr_tags': {'IOVDbGlobalTag': 'OFLCOND-SIM-00-00-00','/TRT/Cond/Status': 'TrtStrawStatus-02', '/LAR/Identifier/FebRodAtlas': 'FebRodAtlas-005', '/LAR/ElecCalibMC': 'LARElecCalibMC-CSC02-J-QGSP_BERT', 'GeoAtlas': 'ATLAS-GEO-02-01-00', 'TGC_support': 'TGC Big Wheel', 'AtlasRelease': 'any', '/LAR/Identifier/LArTTCellMapAtlas': 'LARIdentifierLArTTCellMapAtlas-DC3-05', '/GLOBAL/BField/Map': 'BFieldMap-000', 'MDT_support': 'MDT Big Wheel', '/LAR/Identifier/OnOffIdAtlas': 'OnOffIdAtlas-012'}, 'nentries': 25L, 'evt_number': [1814L], 'file_guid': '4E971C9E-A9A4-DD11-8A9A-00145E6D4F72', 'metadata': {'/Digitization/Parameters': {'physicsList': 'QGSP_BERT', 'N_beamGasInputFiles': 0, 'doBeamHalo': False, 'N_cavernInputFiles': 0, 'overrideMetadata': False, 'numberOfBeamHalo': 1.0, 'doCavern': False, 'IOVDbGlobalTag': 'default', 'N_beamHaloInputFiles': 0, 'initialBunchCrossing': -36, 'doCaloNoise': True, 'N_minBiasInputFiles': 0, 'numberOfCollisions': 2.2999999999999998, 'rndmSvc': 'AtRanluxGenSvc', 'rndmSeedList': ['PixelDigitization 10513308 492615173', 'SCT_Digitization 49261579 105132463', 'TRT_ElectronicsNoise 192 414', 'TRT_Noise 1303 3525', 'TRT_ThresholdFluctuations 12414 34636', 'TRT_ProcessStraw 123525 345747', 'TRT_SimDriftTime 1234636 3456858', 'TRT_PAI 12345747 34567959', 'TRT_FakeConditions 123456858 345678970', 'BCM_Digitization 49261579 105132463', 'LArDigitization 1303 5747', 'Tile_HitVecToCnt 4789968 989240581', 'Tile_DigitsMaker 4789968 989240581', 'CSC_Digitization 49261579 105132463', 'MDTResponse 49261579 105132463', 'MDT_Digitization 49261579 105132463', 'MDT_DigitizationTwin 393242630 857132450', 'TGC_Digitization 49261579 105132463', 'RPC_Digitization 49261579 105132463', 'CscDigitToCscRDOTool 49261579 105132463', 'Tile_HitToTTL1 4789968 989240581', 'CTPSimulation 1979283112 1924452258'], 'numberOfCavern': 2, 'doMuonNoise': True, 'doInDetNoise': True, 'numberOfBeamGas': 1.0, 'finalBunchCrossing': 32, 'doBeamGas': False, 'doMinimumBias': False, 'bunchSpacing': 25, 'DetDescrVersion': 'ATLAS-GEO-02-01-00', 'lvl1TriggerMenu': 'lumi1E31_no_Bphysics_no_prescale', 'rndmSeedOffset2': 69, 'rndmSeedOffset1': 69}, '/Simulation/Parameters': {'MagneticField': 'OracleDB', 'PhysicsList': 'QGSP_BERT', 'CalibrationRun': 'DeadLAr', 'SimLayout': 'ATLAS-GEO-02-01-00', 'DoLArBirk': False, 'LArParameterization': 0, 'VertexStatus': True, 'EtaPhiStatus': True, 'WorldRRange': 'default', 'RunType': 'atlas', 'WorldZRange': 'default', 'Seeds': 'default', 'G4Version': 'geant4.8.3.patch02.atlas04', 'NeutronTimeCut': 150.0, 'SeedsG4': 'default', 'IOVDbGlobalTag': 'default', 'VRangeStatus': True}}, 'metadata_items': [('DataHeader', ';00;MetaDataSvc'), ('IOVMetaDataContainer', '/Digitization/Parameters'), ('IOVMetaDataContainer', '/Simulation/Parameters')], 'stream_names': ['Stream1'], 'run_type': ['N/A'], 'conditions_tag': 'OFLCOND-SIM-00-00-00', 'lumi_block': [1L], 'eventdata_items': [('EventInfo', 'McEventInfo'), ('PixelRDO_Container', 'PixelRDOs'), ('SCT_RDO_Container', 'SCT_RDOs'), ('TRT_RDO_Container', 'TRT_RDOs'), ('InDetSimDataCollection', 'BCM_SDO_Map'), ('InDetSimDataCollection', 'PixelSDO_Map'), ('InDetSimDataCollection', 'SCT_SDO_Map'), ('InDetSimDataCollection', 'TRT_SDO_Map'), ('BCM_RDO_Container', 'BCM_RDOs'), ('LArDigitContainer', 'LArDigitContainer_MC_Thinned'), ('LArRawChannelContainer', 'LArRawChannels'), ('LArTTL1Container', 'LArTTL1EM'), ('LArTTL1Container', 'LArTTL1HAD'), ('TileRawChannelContainer', 'TileRawChannelCnt'), ('TileTTL1Container', 'TileTTL1Cnt'), ('TileTTL1Container', 'TileTTL1MBTS'), ('TileHitVector', 'MBTSHits'), ('CscRawDataContainer', 'CSCRDO'), ('TgcRdoContainer', 'TGCRDO'), ('MdtCsmContainer', 'MDTCSM'), ('RpcPadContainer', 'RPCPAD'), ('ROIB::RoIBResult', 'RoIBResult'), ('CTP_RDO', 'CTP_RDO'), ('DataVector<LVL1::JetElement>', 'JetElements'), ('DataVector<LVL1::TriggerTower>', 'TriggerTowers'), ('MuCTPI_RDO', 'MUCTPI_RDO'), ('McEventCollection', 'TruthEvent'), ('DataVector<LVL1::JEMEtSums>', 'JEMEtSums'), ('MuonSimDataCollection', 'MDT_SDO'), ('MuonSimDataCollection', 'RPC_SDO'), ('MuonSimDataCollection', 'TGC_SDO'), ('DataVector<LVL1::CPMTower>', 'CPMTowers'), ('DataVector<LVL1::CPMHits>', 'CPMHits'), ('DataVector<LVL1::CMMEtSums>', 'CMMEtSums'), ('DataVector<LVL1::JEMRoI>', 'JEMRoIs'), ('LVL1::CMMRoI', 'CMMRoIs'), ('DataVector<LVL1::JEMHits>', 'JEMHits'), ('DataVector<LVL1::CPMRoI>', 'CPMRoIs'), ('DataVector<LVL1::CMMJetHits>', 'CMMJetHits'), ('DataVector<LVL1::CMMCPHits>', 'CMMCPHits'), ('CscSimDataCollection', 'CSC_SDO'), ('TrackRecordCollection', 'CaloEntryLayer'), ('TrackRecordCollection', 'MuonEntryLayer'), ('TrackRecordCollection', 'MuonExitLayer'), ('CaloCalibrationHitContainer', 'LArCalibrationHitActive'), ('CaloCalibrationHitContainer', 'LArCalibrationHitDeadMaterial'), ('CaloCalibrationHitContainer', 'LArCalibrationHitInactive'), ('DataHeader', 'Stream1')], 'run_number': [5640L], 'beam_energy': ['N/A'], 'geometry': 'ATLAS-GEO-02-01-00', 'evt_type': ('IS_SIMULATION', 'IS_ATLAS', 'IS_PHYSICS')}

        f8.fileinfos['tag_info']['AtlasRelease'] = 'any'
        f8.fileinfos['det_descr_tags']['AtlasRelease'] = 'any'
        _compare_fileinfos(f8,f8_ref)
        assert f8.run_number==f8_ref['run_number']
        assert f8.evt_number==f8_ref['evt_number']
        assert f8.lumi_block==f8_ref['lumi_block']
        assert f8.run_type  ==f8_ref['run_type']
        assert f8.beam_type ==f8_ref['beam_type']
        assert f8.beam_energy==f8_ref['beam_energy']

        return

    def test009(self):
        """test URI formation"""
        import PyUtils.AthFile as af

        # test POOL files
        for fname in [
            'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root',
            #'rfio://castor/cern.ch/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root',
            'rfio:///castor/cern.ch/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root',
            '/castor/cern.ch/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root',
            ]:
            """ ####FIXME####
            assert af.exists(fname), "problem with filename [%s]" % fname
            assert af.ftype(fname) == ('pool', 'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/esd.gcc34.15.1.x.pool.root'), "problem with filename [%s]" % fname
            """
        
        # test bytestream files
        for fname in [
            'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data',
            'rfio://castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data',
            'rfio:///castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data',
            ]:
            """ ####FIXME####
            assert af.exists(fname), "problem with filename [%s]" % fname
            assert af.ftype(fname) == ('bs', 'rfio:/castor/cern.ch/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data')
            """
            
        # test pool files served by xrootd
        for fname in [
            "root://eosatlas//eos/atlas/user/b/binet/utests/athfile/TAG.102229._000001.pool.root.1"
            ]:
            assert af.exists(fname), "problem with filename [%s]" % fname
            assert af.ftype(fname) == (
                'pool',
                "root://eosatlas//eos/atlas/user/b/binet/utests/athfile/TAG.102229._000001.pool.root.1")
            pass

        # test AMI urls
        for fname in [
            "ami:data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW",
            "ami:/data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW",
            "ami://data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW",
            ]:
            assert af.ftype(fname) == (
                'bs',
                'ami://data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW'
                )
        return # test9

    def test010(self):
        """test TAG file"""
        import PyUtils.AthFile as af
        fname = "root://eosatlas//eos/atlas/user/b/binet/utests/athfile/TAG.102229._000001.pool.root.1"
        assert af.exists(fname)
        assert af.ftype(fname) == (
            'pool',
            'root://eosatlas//eos/atlas/user/b/binet/utests/athfile/TAG.102229._000001.pool.root.1')
        
        f10 = af.fopen(fname)
        if verbose:
            print "::: f10.fileinfos:"
            print f10.fileinfos

        f10_ref = {'metadata_items': None, 'stream_names': ['TAG'], 'run_type': [], 'stream_tags': [], 'evt_type': [], 'tag_info': None, 'file_type': 'pool', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/utests/athfile/TAG.102229._000001.pool.root.1', 'evt_number': [25], 'beam_energy': [], 'eventdata_items': None, 'run_number': [142391], 'geometry': None, 'beam_type': [], 'file_guid': '10A1A6D0-98EF-DE11-8D70-003048C6617E', 'file_md5sum': 'bce350a81aa253cc7eb8385a62775938', 'lumi_block': [], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 71L, 'metadata': None}

        _compare_fileinfos(f10,f10_ref)
        assert f10.run_number==f10_ref['run_number']
        assert f10.evt_number==f10_ref['evt_number']
        assert f10.lumi_block==f10_ref['lumi_block']
        assert f10.run_type  ==f10_ref['run_type']
        assert f10.beam_type ==f10_ref['beam_type']
        assert f10.beam_energy==f10_ref['beam_energy']
        
        return # test10
    
    def test011(self):
        """test AMI dataset query (RAW)"""
        import PyUtils.AthFile as af
        fname = "ami:data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW"

        assert af.exists(fname)
        assert af.ftype(fname) == (
            'bs',
            'ami://data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW'
            )
        
        f11 = af.fopen(fname)
        if verbose:
            print "::: f11.fileinfos:"
            print f11.fileinfos

        f11_ref = {'metadata_items': None, 'stream_names': ['StreamRAW'], 'run_type': [], 'stream_tags': [{'obeys_lbk': None, 'stream_type': 'calibration', 'stream_name': 'LArElec-Delay-32s-High-Em'}], 'evt_type': [], 'tag_info': None, 'file_type': 'bs', 'file_name': 'ami://data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW', 'evt_number': [], 'beam_energy': [], 'eventdata_items': None, 'run_number': [150430], 'geometry': None, 'beam_type': [], 'file_guid': 'ami://data10_calib.00150430.calibration_LArElec-Delay-32s-High-Em.daq.RAW', 'file_md5sum': None, 'lumi_block': [], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 3072, 'metadata': None}

        _compare_fileinfos(f11,f11_ref)
        assert f11.run_number==f11_ref['run_number']
        assert f11.evt_number==f11_ref['evt_number']
        assert f11.lumi_block==f11_ref['lumi_block']
        assert f11.run_type  ==f11_ref['run_type']
        assert f11.beam_type ==f11_ref['beam_type']
        assert f11.beam_energy==f11_ref['beam_energy']
        
        return # test11

    def test012(self):
        """test AMI dataset query (ESD)"""
        import PyUtils.AthFile as af
        fname = "ami:data09_900GeV.00142191.physics_BPTX.merge.ESD.r1093_p101"

        # FIXME
        assert af.exists(fname)
        assert af.ftype(fname) == (
            'pool',
            'ami://data09_900GeV.00142191.physics_BPTX.merge.ESD.r1093_p101',
            )
        
        f12 = af.fopen(fname)
        if verbose:
            print "::: f12.fileinfos:"
            print f12.fileinfos

        f12_ref = {'metadata_items': None, 'stream_names': ['StreamESD'], 'run_type': [], 'stream_tags': [{'obeys_lbk': None, 'stream_type': 'physics', 'stream_name': 'BPTX'}], 'evt_type': [], 'tag_info': None, 'file_type': 'pool', 'file_name': 'ami://data09_900GeV.00142191.physics_BPTX.merge.ESD.r1093_p101', 'evt_number': [], 'beam_energy': [], 'eventdata_items': None, 'run_number': [142191], 'geometry': None, 'beam_type': ['collisions'], 'file_guid': 'ami://data09_900GeV.00142191.physics_BPTX.merge.ESD.r1093_p101', 'file_md5sum': None, 'lumi_block': [], 'conditions_tag': 'COMCOND-REPPST-004-00', 'det_descr_tags': None, 'nentries': 1256124, 'metadata': None}

        _compare_fileinfos(f12,f12_ref)
        assert f12.run_number==f12_ref['run_number']
        assert f12.evt_number==f12_ref['evt_number']
        assert f12.lumi_block==f12_ref['lumi_block']
        assert f12.run_type  ==f12_ref['run_type']
        assert f12.beam_type ==f12_ref['beam_type']
        assert f12.beam_energy==f12_ref['beam_energy']
        
        return # test12

    def test013(self):
        """test AMI dataset query (AOD)"""
        import PyUtils.AthFile as af
        fname = "ami:data09_idcomm.00111427.physics_L1TT-b6.merge.AOD.f97_m48"

        # FIXME
        assert af.exists(fname)
        assert af.ftype(fname) == (
            'pool',
            'ami://data09_idcomm.00111427.physics_L1TT-b6.merge.AOD.f97_m48',
            )
        
        f13 = af.fopen(fname)
        if verbose:
            print "::: f13.fileinfos:"
            print f13.fileinfos

        f13_ref = {'metadata_items': None, 'stream_names': ['StreamAOD'], 'run_type': [], 'stream_tags': [{'obeys_lbk': None, 'stream_type': 'physics', 'stream_name': 'L1TT-b6'}], 'evt_type': [], 'tag_info': None, 'file_type': 'pool', 'file_name': 'ami://data09_idcomm.00111427.physics_L1TT-b6.merge.AOD.f97_m48', 'evt_number': [], 'beam_energy': [], 'eventdata_items': None, 'run_number': [111427], 'geometry': None, 'beam_type': [], 'file_guid': 'ami://data09_idcomm.00111427.physics_L1TT-b6.merge.AOD.f97_m48', 'file_md5sum': None, 'lumi_block': [], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 27, 'metadata': None}

        _compare_fileinfos(f13,f13_ref)
        assert f13.run_number==f13_ref['run_number']
        assert f13.evt_number==f13_ref['evt_number']
        assert f13.lumi_block==f13_ref['lumi_block']
        assert f13.run_type  ==f13_ref['run_type']
        assert f13.beam_type ==f13_ref['beam_type']
        assert f13.beam_energy==f13_ref['beam_energy']
        
        return # test13

    def test014(self):
        """test AMI dataset query (TAG)"""
        import PyUtils.AthFile as af
        fname = "ami:data10_1beam.00150419.express_express.merge.TAG_COMM.x2_m396"

        # FIXME
        assert af.exists(fname)
        assert af.ftype(fname) == (
            'pool',
            'ami://data10_1beam.00150419.express_express.merge.TAG_COMM.x2_m396',
            )
        
        f14 = af.fopen(fname)
        if verbose:
            print "::: f14.fileinfos:"
            print f14.fileinfos

        f14_ref = {'metadata_items': None, 'stream_names': ['StreamTAG_COMM'], 'run_type': [], 'stream_tags': [{'obeys_lbk': None, 'stream_type': 'express', 'stream_name': 'express'}], 'evt_type': [], 'tag_info': None, 'file_type': 'pool', 'file_name': 'ami://data10_1beam.00150419.express_express.merge.TAG_COMM.x2_m396', 'evt_number': [], 'beam_energy': [], 'eventdata_items': None, 'run_number': [150419], 'geometry': None, 'beam_type': [], 'file_guid': 'ami://data10_1beam.00150419.express_express.merge.TAG_COMM.x2_m396', 'file_md5sum': None, 'lumi_block': [], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 407, 'metadata': None}

        _compare_fileinfos(f14,f14_ref)
        assert f14.run_number==f14_ref['run_number']
        assert f14.evt_number==f14_ref['evt_number']
        assert f14.lumi_block==f14_ref['lumi_block']
        assert f14.run_type  ==f14_ref['run_type']
        assert f14.beam_type ==f14_ref['beam_type']
        assert f14.beam_energy==f14_ref['beam_energy']
        
        return # test14

    def test015(self):
        """test bytestream file via XROOTD"""
    
        import PyUtils.AthFile as af
        fname = 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data'
        assert af.exists(fname)
        assert af.ftype(fname) == ('bs', 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data')
        
        f15 = af.fopen(fname)
        if verbose:
            print "::: f15.fileinfos:"
            print f15.fileinfos
        f15_ref = {'file_md5sum':'e3e301bca63e4b5acb3b3cba43127ff9', 'metadata_items': None, 'stream_names': None, 'run_type': ['TEST'], 'stream_tags': [{'obeys_lbk': True, 'stream_type': 'physics', 'stream_name': 'IDCosmic'}, {'obeys_lbk': False, 'stream_type': 'calibration', 'stream_name': 'IDTracks'}], 'tag_info': None, 'file_type': 'bs', 'file_name': 'root://eosatlas//eos/atlas/user/b/binet/regr-tests/athfile/daq.ATLAS.0092226.physics.IDCosmic.LB0054.SFO-1._0001.data', 'file_guid': '72013664-ECA3-DD11-A90E-0015171A45AC', 'beam_type': [0], 'lumi_block': [54], 'conditions_tag': None, 'det_descr_tags': None, 'nentries': 417, 'eventdata_items': None, 'run_number': [92226], 'beam_energy': [0], 'geometry': None, 'evt_number': [8349492], 'evt_type': [], 'metadata': None}
        _compare_fileinfos(f15,f15_ref)
        assert f15.run_number==f15_ref['run_number']
        assert f15.evt_number==f15_ref['evt_number']
        assert f15.lumi_block==f15_ref['lumi_block']
        assert f15.run_type  ==f15_ref['run_type']
        assert f15.beam_type ==f15_ref['beam_type']
        assert f15.beam_energy==f15_ref['beam_energy']

        return # test15
    
        
### tests ---------------------------------------------------------------------
def main(verbose=False):
    import PyUtils.AthFile as af
    af.server
    loader = unittest.TestLoader()
    testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

    runner = unittest.TextTestRunner( verbosity = 2 )
    result = not runner.run( testSuite ).wasSuccessful()
    return result

if __name__ == "__main__":
    import sys
    print __file__
    sys.exit(main())
    

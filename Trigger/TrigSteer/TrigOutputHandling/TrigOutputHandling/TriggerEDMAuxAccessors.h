#include "xAODBase/IParticleContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "AthContainers/AuxElement.h"

/**
 * Instantiate SG::AuxElementAccessor for all decorations in Trigger EDM before they are used in output writing.
 * This is to avoid xAOD::AuxSelection WARNING Selected dynamic Aux atribute not found in the registry
 */
namespace TriggerEDMAuxAccessors {

template<typename T, typename ...U>
constexpr auto initAccessors(U... names) {
  return std::array<SG::AuxElement::Accessor<T>, sizeof...(names)>{(SG::AuxElement::Accessor<T>(names))...};
}

auto boolAccessors = initAccessors<bool>(
  "hasGenericRoiError", "hasGenericDaqError", "hasCrcTobError", "hasCrcFibreError",
  "hasCrcDaqError", "hasRoibDaqDifference", "hasRoibCtpDifference", "hasDaqCtpDifference");

auto charAccessors = initAccessors<char>(
  "IP2D_isDefaults", "IP3D_isDefaults", "SV1_isDefaults", "rnnip_isDefaults",
  "JetFitterSecondaryVertex_isDefaults", "JetFitter_isDefaults", "passPFTrackPresel",
  "muonCaloTag", "tagFakeTrack", "tagIsoTrack", "tagMuonTrack");

auto intAccessors = initAccessors<int>(
  "SctSpBarrel", "SctSpEndcapA", "SctSpEndcapC", "pixClBarrel", "pixClEndcapA", "pixClEndcapC",
  "totNumPixCL_1", "totNumPixCL_2", "totNumPixCLmin3", "totNumPixSP", "totNumSctSP", "ntrks",
  "IP2D_nTrks", "IP3D_nTrks", "JetFitterSecondaryVertex_nTracks", "JetFitter_N2Tpair",
  "JetFitter_nSingleTracks", "JetFitter_nTracksAtVtx", "JetFitter_nVTX", "JetFitter_ndof",
  "SV1_N2Tpair", "SV1_NGTinSvx", "sp_isPix", "sp_isSct", "sp_layer", "sp_usedTrkId", "seed_et_small", "seed_et_large",
  "pixCL", "pixCL_1", "pixCL_2", "pixCLmin3", "pixCLBarrel", "pixCLEndcapA", "pixCLEndcapC",
  "nCells",
  "sctSP", "sctSPBarrel", "sctSPEndcapA", "sctSPEndcapC",
  "hit_iblovfl", "hit_loc", "hit_trkid", "hit_layer", "trk_id", "trk_dedx_n_usedhits", "trk_n_hdedx_hits", 
  "zfinder_tool");

auto int16Accessors = initAccessors<int16_t>("view");

auto int32Accessors = initAccessors<int32_t>("roi");

auto uint8Accessors = initAccessors<uint8_t>("EBUnbiased");

auto uint32Accessors = initAccessors<uint32_t>("alg", "store", "thread", "thash", "slot", "lvl1ID");

auto uint64Accessors = initAccessors<uint64_t>("start", "stop", "thresholdPatterns");

auto sizeAccessors = initAccessors<size_t>("alg_idx");

auto floatAccessors = initAccessors<float>(
  "EBWeight", "Jvt", "JvtRpt", "IP2D_bc", "IP2D_bu", "IP2D_cu", "IP3D_bc", "IP3D_bu", "IP3D_cu",
  "ActiveArea", "ActiveArea4vec_eta", "ActiveArea4vec_m", "ActiveArea4vec_phi", "ActiveArea4vec_pt",
  "JetEtaJESScaleMomentum_eta", "JetEtaJESScaleMomentum_m", "JetEtaJESScaleMomentum_phi", "JetEtaJESScaleMomentum_pt",
  "JetGSCScaleMomentum_eta", "JetGSCScaleMomentum_m", "JetGSCScaleMomentum_phi", "JetGSCScaleMomentum_pt",
  "JetPileupScaleMomentum_eta", "JetPileupScaleMomentum_m", "JetPileupScaleMomentum_phi", "JetPileupScaleMomentum_pt",
  "JetFitterSecondaryVertex_averageAllJetTrackRelativeEta", "JetFitterSecondaryVertex_averageTrackRelativeEta",
  "JetFitterSecondaryVertex_displacement2d", "JetFitterSecondaryVertex_displacement3d",
  "JetFitterSecondaryVertex_energy", "JetFitterSecondaryVertex_energyFraction", "JetFitterSecondaryVertex_mass",
  "JetFitterSecondaryVertex_minimumAllJetTrackRelativeEta", "JetFitterSecondaryVertex_maximumAllJetTrackRelativeEta",
  "JetFitterSecondaryVertex_minimumTrackRelativeEta", "JetFitterSecondaryVertex_maximumTrackRelativeEta",
  "JetFitter_chi2", "JetFitter_dRFlightDir", "JetFitter_deltaR", "JetFitter_deltaeta", "JetFitter_deltaphi",
  "JetFitter_energyFraction", "JetFitter_mass", "JetFitter_massUncorr", "JetFitter_significance3d",
  "SV1_L3d", "SV1_Lxy", "SV1_deltaR", "SV1_dstToMatLay", "SV1_efracsvx", "SV1_energyTrkInJet",
  "SV1_masssvx", "SV1_normdist", "SV1_significance3d",
  "DL1d20210519r22_pb",
  "DL1d20210519r22_pc",
  "DL1d20210519r22_pu",
  "DL1d20210528r22_pb",
  "DL1d20210528r22_pc",
  "DL1d20210528r22_pu",
  "dips20210517_pb",
  "dips20210517_pc",
  "dips20210517_pu",
  "dipsLoose20210517_pb",
  "dipsLoose20210517_pc",
  "dipsLoose20210517_pu",
  "DetectorEta", "DetectorPhi",
  "EMFrac", "HECFrac", "JVFCorr", "sp_eta", "sp_phi", "sp_r", "sp_z", "seed_eta", "seed_phi", "trk_a0beam",
  "trk_eta", "trk_phi", "trk_pt", "trk_n_hits_inner", "trk_n_hits_innermost", "trk_n_hits_pix", "trk_n_hits_sct",
  "hit_dedx", "hit_tot", "hit_trkchi2", "hit_trkndof", "trk_dedx",
  "btagIp_d0", "btagIp_d0Uncertainty", "btagIp_z0SinTheta", "btagIp_z0SinThetaUncertainty",
  "EOverP", "RErr", "etConeCore", "muonScore", "ptCone20", "trackIso", "trkPtFraction", 
  "zfinder_vtx_z", "zfinder_vtx_weight", "caloIso");

auto doubleAccessors = initAccessors<double>("ptcone02", "ptcone03");

auto vboolAccessors = initAccessors<std::vector<bool>>("IP2D_flagFromV0ofTracks", "IP3D_flagFromV0ofTracks");

auto vintAccessors = initAccessors<std::vector<int>>(
  "counts", "IP2D_gradeOfTracks", "IP3D_gradeOfTracks", "NumTrkPt1000", "NumTrkPt500");

auto vushortAccessors = initAccessors<std::vector<unsigned short>>("robs_status");

auto vuintAccessors = initAccessors<std::vector<unsigned>>("robs_history");

auto vuint32Accessors = initAccessors<std::vector<uint32_t>>("robs_id", "robs_size", "PEBROBList", "PEBSubDetList");

auto vfloatAccessors = initAccessors<std::vector<float>>(
  "IP2D_weightBofTracks", "IP2D_weightCofTracks", "IP2D_weightUofTracks", "IP2D_sigD0wrtPVofTracks",
  "IP3D_weightBofTracks", "IP3D_weightCofTracks", "IP3D_weightUofTracks", "IP3D_sigD0wrtPVofTracks",
  "IP3D_sigZ0wrtPVofTracks", "IP3D_valD0wrtPVofTracks", "IP3D_valZ0wrtPVofTracks",
  "JetFitter_fittedCov", "JetFitter_fittedPosition", "JetFitter_tracksAtPVchi2", "JetFitter_tracksAtPVndf",
  "EnergyPerSampling", "SumPtChargedPFOPt500", "SumPtTrkPt1000", "SumPtTrkPt500", "TrackWidthPt1000",
  "pTcuts", "z0cuts", "btagIp_trackMomentum", "btagIp_trackDisplacement");

auto elroiAccessors = initAccessors<ElementLink<TrigRoiDescriptorCollection>>("viewIndex");

auto elbtagAccessors = initAccessors<ElementLink<xAOD::BTaggingContainer>>("btaggingLink");

auto veltrkpAccessors = initAccessors<std::vector<ElementLink<xAOD::TrackParticleContainer>>>(
  "BTagTrackToJetAssociator", "JetFitter_tracksAtPVlinks", "SV1_badTracksIP");

auto velbvtxAccessors = initAccessors<std::vector<ElementLink<xAOD::BTagVertexContainer>>>("JetFitter_JFvertices");

auto velmuAccessors = initAccessors<std::vector<ElementLink<xAOD::MuonContainer>>>("Muons");

auto velvtxAccessors = initAccessors<std::vector<ElementLink<xAOD::VertexContainer>>>("SV1_vertices");

auto velipAccessors = initAccessors<std::vector<ElementLink<xAOD::IParticleContainer>>>("GhostTrack");

} // namespace TriggerEDMAuxAccessors

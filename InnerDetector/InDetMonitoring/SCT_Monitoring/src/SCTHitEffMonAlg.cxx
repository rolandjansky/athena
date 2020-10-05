/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTHitEffMonAlg.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/ReadHandle.h"

// InDet
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
// Conditions
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"

#include "MagFieldElements/AtlasFieldCache.h"

// Track
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkTrackSummary/TrackSummary.h"

// SCT
#include "SCT_NameFormatter.h"
#include "SCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"

// std and STL includes
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>       // std::numeric_limits
#include <memory>
#include <sstream>

// #include "TRandom.h" // Only for Testing

using namespace SCT_Monitoring;

namespace {// anonymous namespace for functions at file scope
  static const bool testOffline{false};

  static const std::string histogramPath[N_REGIONS_INC_GENERAL] = {
    "SCT/SCTEC/eff", "SCT/SCTB/eff", "SCT/SCTEA/eff", "SCT/GENERAL/eff"
  };
  static const std::string histogramPathRe[N_REGIONS] = {
    "SCT/SCTEC/eff/perLumiBlock", "SCT/SCTB/eff/perLumiBlock", "SCT/SCTEA/eff/perLumiBlock"
  };

  template< typename T > Identifier
  surfaceOnTrackIdentifier(const T& tsos, const bool useTrackParameters = true) {
    Identifier result; // default constructor produces invalid value
    const Trk::MeasurementBase* mesb{tsos->measurementOnTrack()};

    if (mesb and mesb->associatedSurface().associatedDetectorElement()) {
      result = mesb->associatedSurface().associatedDetectorElement()->identify();
    } else if (useTrackParameters and tsos->trackParameters()) {
      result = tsos->trackParameters()->associatedSurface().associatedDetectorElementIdentifier();
    }
    return result;
  }

  constexpr double radianDegrees{180. / M_PI};

  static const double stripWidth{79.95e-3}; // in mm

  static const std::array < std::string, N_REGIONS > mapName = {
    "m_eff_", "eff_", "p_eff_"
  };
  static const std::array < std::string, N_REGIONS > ineffmapName = {
    "ineffm_", "ineff_", "ineffp_"
  };
  static const std::array < std::string, N_REGIONS > regionNames = {
    "SCTHitEffMonitorEC", "SCTHitEffMonitorB", "SCTHitEffMonitorEA"
  };

}// namespace end

SCTHitEffMonAlg::SCTHitEffMonAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :AthMonitorAlgorithm(name,pSvcLocator) {
}

StatusCode SCTHitEffMonAlg::initialize() {
  ATH_MSG_INFO("Initializing SCTHitEffMonAlg");

  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK(detStore()->retrieve(m_trtId, "TRT_ID"));
  
  ATH_CHECK(m_residualPullCalculator.retrieve());
  ATH_MSG_INFO("Retrieved pull calculator tool " << m_residualPullCalculator);
  
  ATH_CHECK(m_holeSearchTool.retrieve());
  ATH_MSG_INFO("Retrieved hole search tool " << m_holeSearchTool);
  
  ATH_CHECK(m_rotcreator.retrieve());

  ATH_MSG_INFO("Retrieved tool " << m_rotcreator);
  ATH_CHECK( m_bunchCrossingKey.initialize());
  ATH_CHECK(m_configConditions.retrieve());

  m_path = (m_useIDGlobal) ? ("/InDetGlobal/") : ("");
  ATH_MSG_INFO("End");

  if ((m_minSCTHits == -1) and (m_minTRTHits == -1) and (m_minOtherHits == -1)) {
    if (m_isCosmic) {
      m_minTRTHits = 45;
      m_minSCTHits = 7;
      m_minOtherHits = 5;
    } else {
      m_minTRTHits = 30;
      m_minSCTHits = 4;
      m_minOtherHits = 3;
    }
  }

  ATH_CHECK( m_comTimeName.initialize(m_useTRTPhase or m_isCosmic) );
  ATH_CHECK( m_TrackName.initialize() );
  ATH_CHECK( m_sctContainerName.initialize() );

  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  
  return AthMonitorAlgorithm::initialize();
}


int SCTHitEffMonAlg::becIdxLayer2Index(const int becIdx, const int layer) const {
  switch( becIdx ) {
  case SCT_Monitoring::ENDCAP_C_INDEX:
    return layer;
  case SCT_Monitoring::BARREL_INDEX:
    return layer + SCT_Monitoring::N_DISKS;
  case SCT_Monitoring::ENDCAP_A_INDEX:
    return layer + SCT_Monitoring::N_DISKS + SCT_Monitoring::N_BARRELS;
  default:
    return -1;
  }
}

int SCTHitEffMonAlg::getWaferIndex(const int barrel_ec, const int layer_disk, const int side) const {
  int waferIndex = -1;
  if (barrel_ec == BARREL) {
    // corresponds to the waferIndex of B3 side0
    waferIndex = 0; 
  } else if (barrel_ec == ENDCAP_A) {
    // corresponds to the waferIndex of EA0 side0
    waferIndex = N_BARRELS*N_SIDES;
  } else if (barrel_ec == ENDCAP_C) {
    // corresponds to the waferIndex of EC0 side0
    waferIndex = N_BARRELS*N_SIDES + N_ENDCAPS*N_SIDES;
  } else {
    ATH_MSG_WARNING("The barrel_bc index" << barrel_ec << " is not defined.");
    return waferIndex;
  }
  return waferIndex + layer_disk * N_SIDES + side;
}

double SCTHitEffMonAlg::getResidual(const Identifier& surfaceID,
                                    const Trk::TrackParameters* trkParam,
                                    const InDet::SCT_ClusterContainer* p_sctclcontainer) const {
  double trackHitResidual{-999.};

  if (trkParam==nullptr) {
    ATH_MSG_WARNING("Not track parameters found. Returning default residual value.");
    return trackHitResidual;
  }
  IdentifierHash idh{m_sctId->wafer_hash(surfaceID)};
  auto containerIterator{p_sctclcontainer->indexFindPtr(idh)};
  if (containerIterator != nullptr) {
    for (const InDet::SCT_Cluster* cluster: *containerIterator) {
      if ((cluster==nullptr) or (cluster->detectorElement()==nullptr)) {
        ATH_MSG_WARNING("nullptr to RIO or detElement");
        continue;
      }
      if (surfaceID == m_sctId->wafer_id(cluster->detectorElement()->identify())) {
        const Trk::PrepRawData* rioo{dynamic_cast<const Trk::PrepRawData *>(cluster)};
        std::unique_ptr<const Trk::RIO_OnTrack> rio{m_rotcreator->correct(*rioo, *trkParam)};
        if (not m_residualPullCalculator.empty()) {
          std::unique_ptr<const Trk::ResidualPull> residualPull{m_residualPullCalculator->residualPull(rio.get(), trkParam,
                                                                                                       Trk::ResidualPull::Unbiased)};
          if (not residualPull) continue;
          if (std::abs(residualPull->residual()[Trk::loc1]) < std::abs(trackHitResidual)) {
            trackHitResidual = residualPull->residual()[Trk::loc1];
          }
        }
      }
    }
  }
  return trackHitResidual;
}

StatusCode
SCTHitEffMonAlg::findAnglesToWaferSurface(const Amg::Vector3D& mom,
                                          const Identifier id,
                                          const InDetDD::SiDetectorElementCollection* elements,
                                          double& theta,
                                          double& phi) const {
  phi = 90.;
  theta = 90.;

  const Identifier waferId{m_sctId->wafer_id(id)};
  const IdentifierHash waferHash{m_sctId->wafer_hash(waferId)};
  const InDetDD::SiDetectorElement* element{elements->getDetectorElement(waferHash)};
  if (not element) {
    ATH_MSG_VERBOSE("findAnglesToWaferSurface: failed to find detector element for id = " << m_sctId->print_to_string(id));
    return StatusCode::FAILURE;
  }
  double pNormal{mom.dot(element->normal())};
  double pEta{mom.dot(element->etaAxis())};
  double pPhi{mom.dot(element->phiAxis())};
  if (pPhi < 0.) {
    phi = -90.;
  }
  if (pEta < 0.) {
    theta = -90.;
  }
  if (pNormal != 0.) {
    phi = std::atan(pPhi / pNormal) * radianDegrees;
    theta = std::atan(pEta / pNormal) * radianDegrees;
  }
  return StatusCode::SUCCESS;
}

int SCTHitEffMonAlg::previousChip(double xl, int side, bool swap) const {
  double xLeftEdge{xl + N_STRIPS / 2. * stripWidth}; // xl defined wrt center of module, convert to edge of module
  int chipPos{static_cast<int>(xLeftEdge / (stripWidth * N_STRIPS) * N_CHIPS)};

  if (side == 0) {
    chipPos = swap ? 5 - chipPos : chipPos;
  } else {
    chipPos = swap ? 11 - chipPos : 6 + chipPos;
  }
  return chipPos;
}

StatusCode SCTHitEffMonAlg::failCut(bool value, std::string name) const {
  if (value) {
    ATH_MSG_VERBOSE("Passed " << name);
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Failed " << name);
  return StatusCode::SUCCESS;
}

StatusCode SCTHitEffMonAlg::fillHistograms(const EventContext& ctx) const {
  ATH_MSG_VERBOSE("SCTHitEffMonTool::fillHistograms()");

  const std::map<Identifier, unsigned int>* badChips{nullptr};
  if (m_vetoBadChips) {
    badChips = m_configConditions->badChips(ctx);
  }

  double timecor{-20.};
  if (m_useTRTPhase or m_isCosmic) {
    SG::ReadHandle<ComTime> theComTime{m_comTimeName, ctx};
    if (theComTime.isValid()) {
      timecor = theComTime->getTime();
      ATH_MSG_VERBOSE("Retrieved ComTime object with name " << m_comTimeName.key() << " found: Time = " << timecor);
    } else {
      timecor = -18.;
      ATH_MSG_WARNING("ComTime object not found with name " << m_comTimeName.key());
    }
  }
  // If we are going to use TRT phase in anger, need run-dependent corrections.
  const EventIDBase& pEvent{ctx.eventID()};
  unsigned BCID{pEvent.bunch_crossing_id()};
  SG::ReadCondHandle<BunchCrossingCondData> bcidHdl(m_bunchCrossingKey,ctx);
  if (!bcidHdl.isValid()) {
     ATH_MSG_ERROR( "Unable to retrieve BunchCrossing conditions object" );
     return StatusCode::FAILURE;
  }
  const BunchCrossingCondData* bcData{*bcidHdl};
  int BCIDpos{bcData->distanceFromFront(BCID, BunchCrossingCondData::BunchCrossings)};

  SG::ReadCondHandle<AtlasFieldCacheCondObj> fieldHandle{m_fieldCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*fieldHandle};
  if (fieldCondObj==nullptr) {
    ATH_MSG_ERROR("AtlasFieldCacheCondObj cannot be retrieved.");
    return StatusCode::RECOVERABLE;
  }
  MagField::AtlasFieldCache fieldCache;
  fieldCondObj->getInitializedCache(fieldCache);
  const bool solenoidOn{fieldCache.solenoidOn()};

  // ---- First try if m_tracksName is a TrackCollection
  SG::ReadHandle<TrackCollection>tracks{m_TrackName, ctx};
  if (not tracks.isValid()) {
    ATH_MSG_WARNING("Tracks not found: " << tracks << " / " << m_TrackName.key());
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_VERBOSE("Successfully retrieved " << m_TrackName.key() << " : " << tracks->size() << " items");
  }

  SG::ReadHandle<InDet::SCT_ClusterContainer> p_sctclcontainer{m_sctContainerName, ctx};
  if (not p_sctclcontainer.isValid()) {
    ATH_MSG_WARNING("SCT clusters container not found: " << m_sctContainerName.key());
    return StatusCode::SUCCESS;
  }

  // cut on number of tracks (skip this cut for online)
  if (environment() != Environment_t::online) {
    if (failCut(tracks->size() <= m_maxTracks, "# of tracks cut")) {
      return StatusCode::SUCCESS;
    }
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle{m_SCTDetEleCollKey, ctx};
  const InDetDD::SiDetectorElementCollection* elements{sctDetEle.retrieve()};
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved in fillHistograms()");
    return StatusCode::FAILURE;
  }

  int nTrk{0}, nTrkPars{0}, nTrkGood{0};

  // Loop over track collection to count tracks
  for (const Trk::Track* pthisTrack: *tracks) {
    nTrk++;
    if (pthisTrack==nullptr) {
      continue;
    }
    if (failCut(pthisTrack and pthisTrack->trackParameters() and pthisTrack->trackParameters()->size(),
                "track cut: presence")) {
      continue;
    }
    nTrkPars++;

    if (m_insideOutOnly and failCut(pthisTrack->info().patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder),
                                    "track cut: inside-out only")) {
      continue;
    }
    if (pthisTrack->perigeeParameters() == nullptr) {
      continue;
    }
    const Trk::Perigee* perigee{pthisTrack->perigeeParameters()};
    const AmgVector(5)& perigeeParameters{perigee->parameters()};
    const double d0{perigeeParameters[Trk::d0]};
    const double z0{perigeeParameters[Trk::z0]};
    const double perigeeTheta{perigeeParameters[Trk::theta]};

    if (solenoidOn and failCut(perigee->pT() >= m_minPt, "track cut: Min Pt")) {
      continue;
    }
    if (not m_isCosmic and failCut(std::abs(d0) <= m_maxD0, "track cut: max D0")) {
      continue;
    }
    if (m_maxZ0sinTheta and failCut(std::abs(z0 * sin(perigeeTheta)) <= m_maxZ0sinTheta, "track cut: Max Z0sinTheta")) {
      continue;
    }
    nTrkGood++;
  }

  // Loop over original track collection
  for (const Trk::Track* pthisTrack: *tracks) {

    // First, go through all cuts in this block
    ATH_MSG_VERBOSE("Starting new track");
    if (pthisTrack==nullptr) {
      continue;
    }
    if (failCut(pthisTrack and pthisTrack->trackParameters() and pthisTrack->trackParameters()->size(),
                "track cut: presence")) {
      continue;
    }

    if (m_insideOutOnly and failCut(pthisTrack->info().patternRecoInfo(Trk::TrackInfo::SiSPSeededFinder),
                                    "track cut: inside-out only")) {
      continue;
    }
    if (pthisTrack->perigeeParameters() == 0) {
      continue;
    }
    const Trk::Perigee* perigee{pthisTrack->perigeeParameters()};
    const AmgVector(5)& perigeeParameters{perigee->parameters()};
    const double d0{perigeeParameters[Trk::d0]};
    const double z0{perigeeParameters[Trk::z0]};
    const double perigeeTheta{perigeeParameters[Trk::theta]};

    if (failCut(perigee->pT() >= m_minPt, "track cut: Min Pt")) {
      continue;
    }
    if (not m_isCosmic and failCut(std::abs(d0) <= m_maxD0, "track cut: max D0")) {
      continue;
    }
    if (m_maxZ0sinTheta and failCut(std::abs(z0 * sin(perigeeTheta)) <= m_maxZ0sinTheta, "track cut: Max Z0sinTheta")) {
      continue;
    }

    const Trk::TrackSummary* summary{pthisTrack->trackSummary()};

    if (summary and (summary->get(Trk::numberOfSCTHits) < 1)) {
      continue;
    }

    std::unique_ptr<const Trk::Track> trackWithHoles(m_holeSearchTool->getTrackWithHoles(*pthisTrack));
    if (not trackWithHoles) {
      ATH_MSG_WARNING("trackWithHoles pointer is invalid");
      continue;
    }
    ATH_MSG_VERBOSE("Found " << trackWithHoles->trackStateOnSurfaces()->size() << " states on track");

    int NHits[N_REGIONS] = {
      0, 0, 0
    };
    int pixelNHits{0};
    int pixelNHoles{0};
    int trtNHits{0};

    int sctNHitsPerRegion[N_LAYERS_TOTAL*N_SIDES] = {0};
    int sctNHolesPerRegion[N_LAYERS_TOTAL*N_SIDES] = {0};
    // Above two variables hold the number of hits for each SCT disk / layer.
    // [N_LAYERS_TOTAL*N_SIDES(= 44)] indicates the waferIndex defined as below.
    //  0- 7: B3 side0, B3 side1, B4 side0, ... B6 side1
    //  8-25: EA0 side0, EA1 side1, ... EA8 side1
    // 26-43: EC0 side0, EC1 side1, ... EC8 side1

    std::map < Identifier, double > mapOfTrackHitResiduals;
    double zmin = std::numeric_limits<float>::max();
    double zmax = -std::numeric_limits<float>::max();
    double zpos{0.};
    float layerSide{-1};
    float min_layerSide{999.};
    float max_layerSide{-1.};
    Identifier surfaceID;

    // Loop over all TSOS (track state on surface) on track to check number of hits / holes on Pixel, SCT and TRT
    for (const Trk::TrackStateOnSurface* tsos: *(trackWithHoles->trackStateOnSurfaces())) {
      surfaceID = surfaceOnTrackIdentifier(tsos);

      if (not surfaceID.is_valid()) {
        continue;
      }
      
      // Check waferIndex; if the default value of -1 is kept, the corresponding TSOS is not associated with SCT.
      int waferIndex = -1;
      // Calculate waferIndex
      if (m_sctId->is_sct(surfaceID)) {
        waferIndex = getWaferIndex(m_sctId->barrel_ec(surfaceID),
                                   m_sctId->layer_disk(surfaceID),
                                   m_sctId->side(surfaceID));
      }
      
      if (tsos->type(Trk::TrackStateOnSurface::Measurement) or tsos->type(Trk::TrackStateOnSurface::Outlier)) {
        if (m_pixelId->is_pixel(surfaceID)) {
          pixelNHits++;
        }
        if (m_trtId->is_trt(surfaceID)) {
          trtNHits++;
        }
        if (m_sctId->is_sct(surfaceID)) {
          NHits[bec2Index(m_sctId->barrel_ec(surfaceID))]++;
          mapOfTrackHitResiduals[surfaceID] = getResidual(surfaceID, tsos->trackParameters(), &*p_sctclcontainer);
          sctNHitsPerRegion[waferIndex]++;
        }
      } else if (tsos->type(Trk::TrackStateOnSurface::Hole)) {
        if (m_pixelId->is_pixel(surfaceID)) {
          pixelNHoles++;
        } else if (m_sctId->is_sct(surfaceID)) {
          sctNHolesPerRegion[waferIndex]++;
        }
      }

      if (tsos->type(Trk::TrackStateOnSurface::Measurement)) { // Determine zmin and zmax taking multiple
        // hits/side into account
        if (m_isCosmic) { // If this is cosmics use z, bad method but ok...
          if (tsos->trackParameters()) {
            zpos = tsos->trackParameters()->position().z();
            zmax = std::max(zpos, zmax);
            zmin = std::min(zpos, zmin);
          } else {
            ATH_MSG_WARNING("No track parameter found. Zmin and Zmax not recalculated.");
          }
        } else { // else use layer/side number : better but does not work for cosmics
          if (m_sctId->is_sct(surfaceID)) {
            layerSide = (m_sctId->barrel_ec(surfaceID) != 0) * N_BARRELS +
              static_cast<float>(m_sctId->layer_disk(surfaceID)) + (static_cast<float>(m_sctId->side(surfaceID)) == 0) * 0.5;
            min_layerSide = std::min(min_layerSide, layerSide);
            max_layerSide = std::max(max_layerSide, layerSide);
          } else if (m_pixelId->is_pixel(surfaceID)) {
            min_layerSide = -1;
          } else if (m_trtId->is_trt(surfaceID)) {
            max_layerSide = N_BARRELS + N_ENDCAPS + 1;
          }
        }
      }
    }

    int sctNHits{NHits[ENDCAP_C_INDEX] + NHits[BARREL_INDEX] + NHits[ENDCAP_A_INDEX]};
    std::vector<bool> layersCrossedByTrack[N_REGIONS];
    std::vector<int> nHolesOnLayer[N_REGIONS];
    std::vector<int> nHolesDistOnLayer[N_REGIONS];
    for (int i{0}; i < N_REGIONS; ++i) {
      nHolesDistOnLayer[i].resize(n_layers[i] * 2, 0);
      nHolesOnLayer[i].resize(n_layers[i] * 2, 0);
      layersCrossedByTrack[i].resize(n_layers[i] * 2, false);
    }

    // Loop over all TSOS again; this time, to extract SCT-related hits and holes.
    for (const Trk::TrackStateOnSurface* tsos: *(trackWithHoles->trackStateOnSurfaces())) {
      ATH_MSG_VERBOSE("Starting new hit");
      surfaceID = surfaceOnTrackIdentifier(tsos);

      if (failCut(m_sctId->is_sct(surfaceID), "hit cut: is in SCT")) {
        continue;
      }


      int side{m_sctId->side(surfaceID)};
      int layer{m_sctId->layer_disk(surfaceID)};
      int bec{m_sctId->barrel_ec(surfaceID)};
      unsigned int isub{bec2Index(bec)};
      ATH_MSG_VERBOSE("New SCT candidate: " << m_sctId->print_to_string(surfaceID));

      int waferIndex = getWaferIndex(bec, layer, side);

      Int_t sctNHitsExceptThisWafer{0};
      Int_t sctNHolesExceptThisWafer{0};

      for (Int_t i=0; i<N_LAYERS_TOTAL*N_SIDES; i++) {
        if (i != waferIndex) {
          sctNHitsExceptThisWafer  += sctNHitsPerRegion[i];
          sctNHolesExceptThisWafer += sctNHolesPerRegion[i];
        }
      }

      // The track is required to satisfy:
      // - Number of Si hits to be >= 8
      // - Number of Si holes to be <= 1
      // without counting on this TSOS object. (avoid tracking bias.)
      if ((unsigned int)(sctNHitsExceptThisWafer + pixelNHits) < m_minSiHits) {
        ATH_MSG_VERBOSE("This track is rejected due to the number of hits: " << sctNHitsExceptThisWafer * pixelNHits);
        continue;
      }
      if ((unsigned int)(sctNHolesExceptThisWafer + pixelNHoles) > m_maxSiHoles) {
        ATH_MSG_VERBOSE("This track is rejected due to the number of holes: " << sctNHolesExceptThisWafer * pixelNHoles);
        continue;
      }

      std::string etaPhiSuffix = "_" + std::to_string(layer) + "_" + std::to_string(side);
      const int detIndex{becIdxLayer2Index(isub, layer)};
      if (detIndex == -1) {
        ATH_MSG_WARNING("The detector region (barrel, endcap A, endcap C) could not be determined");
        continue;
      }
      float eff{0.};
      IdentifierHash sideHash{m_sctId->wafer_hash(surfaceID)};
      Identifier module_id{m_sctId->module_id(surfaceID)};
      float layerPlusHalfSide{static_cast<float>(layer) + static_cast<float>(side) * 0.5f};
      float dedicated_layerPlusHalfSide{static_cast<float>(layer) + static_cast<float>((side + 1) % 2) * 0.5f};
      const Trk::TrackParameters* trkParamOnSurface{tsos->trackParameters()};
      double trackHitResidual{getResidual(surfaceID, trkParamOnSurface, &*p_sctclcontainer)};
      
      float distCut{m_effdistcut};

      if (tsos->type(Trk::TrackStateOnSurface::Measurement) or tsos->type(Trk::TrackStateOnSurface::Outlier)) {
        eff = 1.;
      } else if (tsos->type(Trk::TrackStateOnSurface::Hole) and (std::abs(trackHitResidual) < distCut)) {
        eff = 1.;
      }

      bool otherFaceFound{false};
      IdentifierHash otherSideHash;
      Identifier otherSideSurfaceID;
      IdContext context{m_sctId->wafer_context()};
      m_sctId->get_other_side(sideHash, otherSideHash);
      m_sctId->get_id(otherSideHash, otherSideSurfaceID, &context);
      otherFaceFound = mapOfTrackHitResiduals.find(otherSideSurfaceID) != mapOfTrackHitResiduals.end();

      int nOther{sctNHits};
      if (tsos->type(Trk::TrackStateOnSurface::Measurement) or tsos->type(Trk::TrackStateOnSurface::Outlier)) {
        --nOther;
      }

      // Get the track phi; we may cut on it.
      double phiUp{90.};
      double theta{90.};
      if (trkParamOnSurface and (not findAnglesToWaferSurface(trkParamOnSurface->momentum(), surfaceID, elements, theta, phiUp))) {
        ATH_MSG_WARNING("Error from findAngles");
      }

      if (m_useSCTorTRT) {
        if (failCut(trtNHits >= m_minTRTHits or
                    sctNHits >= m_minSCTHits, "track cut: min TRT or SCT hits")) {
          continue;
        }
      } else {
        if (failCut(trtNHits >= m_minTRTHits, "track cut: min TRT hits")) {
          continue;
        }
        if (failCut(sctNHits >= m_minSCTHits, "track cut: min SCT hits")) {
          continue;
        }
        if (failCut(pixelNHits >= m_minPixelHits, "track cut: min Pixel hits")) {
          continue;
        }
      }

      if (failCut(nOther >= m_minOtherHits, "track cut: minOtherHits")) {
        continue;
      }

      ATH_MSG_DEBUG("Use TRT phase " << m_useTRTPhase << " is cosmic? " << m_isCosmic << " timecor " << timecor);
      if (m_useTRTPhase or m_isCosmic) {
        if (timecor == 0) {
          continue;
        }
        static const double tmin{-15.};
        static const double tmax{10.};
        if (failCut((timecor >= tmin) and (timecor <= tmax), "track cut: timing cut")) {
          continue;
        }
        ATH_MSG_DEBUG(timecor << " " << tmin << " " << tmax);
      }

      bool enclosingHits{true};
      if (m_isCosmic) {
        if (tsos->trackParameters()) {
          zpos = tsos->trackParameters()->position().z();
          enclosingHits = ((zpos > zmin) and (zpos < zmax));
        } else {
          ATH_MSG_WARNING("No track parameters found. Cannot determine whether it is an enclosed hit.");
          enclosingHits = false;
        }
      } else {
        layerSide = (m_sctId->barrel_ec(surfaceID) != 0) * N_BARRELS
          + static_cast<float>(m_sctId->layer_disk(surfaceID))
          + (static_cast<float>(m_sctId->side(surfaceID)) == 0) * 0.5;
        enclosingHits = ((layerSide > min_layerSide) and (layerSide < max_layerSide));
      }

      if (m_requireEnclosingHits and
          (not (layerPlusHalfSide == 0.5)) and
          (not ((isub == 1) and (layerPlusHalfSide == 3))) and
          (not (layerPlusHalfSide == 8))) {
        if (failCut(enclosingHits, "hit cut: enclosing hits")) {
          continue;
        }
      }

      // Now fill with the local z
      double chi2{trackWithHoles->fitQuality()->chiSquared()};
      int ndf{trackWithHoles->fitQuality()->numberDoF()};
      double chi2_div_ndf{ndf > 0. ? chi2 / ndf : -1.};

      if (failCut(std::abs(phiUp) <= m_maxPhiAngle, "hit cut: incidence angle")) {
        continue;
      }

      if (failCut((ndf > 0) and (chi2_div_ndf <= m_maxChi2), "track cut: chi2 cut")) {
        continue;
      }

      if (m_requireOtherFace and failCut(otherFaceFound, "hit cut: other face found")) {
        continue;
      }

      if (not trkParamOnSurface) continue;
      double xl{trkParamOnSurface->localPosition()[0]};
      double yl{trkParamOnSurface->localPosition()[1]};

      // Check guard ring
      bool insideGuardRing{true};
      if (isub == BARREL_INDEX) {
        const float xGuard{m_effdistcut};
        static const float yGuard{3.};
        if (xl < -30.7 + xGuard) {
          insideGuardRing = false;
        }
        if (xl > 30.7 - xGuard) {
          insideGuardRing = false;
        }

        static const double yend{63.960 + 0.03 - 1.};  // The far sensitive end
        static const double ydead{2.06 / 2.}; // the near sensitive end
        if (yl > yend - yGuard) {
          insideGuardRing = false;
        }
        if (yl < -yend + yGuard) {
          insideGuardRing = false;
        }
        if ((yl < ydead + yGuard) and (yl > -ydead - yGuard)) {
          insideGuardRing = false;
        }
      } else {
        // No guard ring for the endcaps for now...just set true.
        insideGuardRing = true;
      }

      if (m_requireGuardRing and failCut(insideGuardRing, "hit cut: inside guard ring")) {
        continue;
      }

      // Check bad chips
      if (m_vetoBadChips) {
        bool nearBadChip{false};
        IdentifierHash waferHash{m_sctId->wafer_hash(surfaceID)};
        const InDetDD::SiDetectorElement* pElement{elements->getDetectorElement(waferHash)};
        bool swap{(pElement->swapPhiReadoutDirection()) ? true : false};
        int chipPos{previousChip(xl, side, swap)};
        unsigned int status{0};
        std::map<Identifier, unsigned int>::const_iterator badChip{badChips->find(module_id)};
        if (badChip != badChips->end()) {
          status = (*badChip).second;
          // Veto if either of closest two chips is dead
          const bool nearBadChipDead{(status & (1 << chipPos)) != 0};
          const bool nextBadChipDead{(status & (1 << (chipPos + 1))) != 0};
          const bool isNotEndChip{(chipPos != 5) and (chipPos != 11)}; // cant have a 'next' if its the end chip on that
          // side
          // nearBadChip = status & (1 << chipPos) or
          //  (chipPos != 5 and chipPos != 11 and status & (1 << (chipPos + 1)));
          // clarify logic:
          nearBadChip = nearBadChipDead or (isNotEndChip and nextBadChipDead);
        }
        if (failCut(not nearBadChip, "hit cut: not near bad chip")) {
          continue;
        }
      }
      ATH_MSG_VERBOSE("Candidate passed all cuts");

      const int ieta{m_sctId->eta_module(surfaceID)};
      const int iphi{m_sctId->phi_module(surfaceID)};

      auto effAcc{Monitored::Scalar<float>("eff", eff)};
      auto ineffAcc{Monitored::Scalar<float>("ineff", (testOffline ? 1. : 1.-eff))};
      auto ietaAcc{Monitored::Scalar<int>("ieta"+etaPhiSuffix, ieta)};
      auto iphiAcc{Monitored::Scalar<int>("iphi"+etaPhiSuffix, iphi)};
      auto layerAcc{Monitored::Scalar<float>("layerPlusHalfSide", dedicated_layerPlusHalfSide)};
      auto lumiAcc{Monitored::Scalar<int>("LumiBlock", ctx.eventID().lumi_block())};
      auto isubAcc{Monitored::Scalar<int>("isub", isub)};
      auto sideHashAcc{Monitored::Scalar<int>("sideHash", sideHash)};
      auto isFirstBCIDAcc{Monitored::Scalar<bool>("isFirstBCID", (BCIDpos <= 0))};

      //fill the histograms
      fill(regionNames[isub].data(), effAcc, ineffAcc, ietaAcc, iphiAcc, layerAcc, lumiAcc, isFirstBCIDAcc);
      fill("SCTHitEffMonitor", effAcc, lumiAcc, isubAcc, sideHashAcc, isFirstBCIDAcc);

      if (testOffline) {
        ATH_MSG_INFO("Filling " << detIndex << ", " << side << " eta " << ieta << " phi " << iphi);
      }
    } // End of loop over hits/holes
  }
  ATH_MSG_VERBOSE("finished loop over tracks = " << tracks->size());
  
  return StatusCode::SUCCESS;
}


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKFastSimTruth/TrigFTKFastSimTruth.h"

#include "TrigFTKSim/FTKDataInput.h"
#include "TrigFTKSim/FTKClusteringEngine.h"


#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

// HepMC
#include "HepMC/GenParticle.h"

#include <algorithm>

static const double IBL_PHI_COORD_MAX = 2680.; //335*8=2680


TrigFTKFastSimTruth::TrigFTKFastSimTruth(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_ftkRawTrackContainerKey("FTK_RDO_Tracks"),
  m_hitInputTool("FTK_SGHitInput/FTK_SGHitInput", this),
  m_sectorMatchTool("TrigFTKSectorMatchTool/TrigFTKSectorMatchTool", this),
  m_trackBuilderTool("TrigFTKTrackBuilderTool/TrigFTKTrackBuilderTool", this),
  m_pixelId(0),
  m_sctId(0),
  m_IBLMode(0),
  m_pmap_path_8L(""),
  m_pmap_path_12L(""),
  m_clustering(true),
  m_saveClusterContent(false),
  m_diagClustering(true),
  m_sctClustering(false),
  m_pixelClusteringMode(1),
  m_ibl3DRealistic(false),
  m_duplicateGanged(true),
  m_gangedPatternRecognition(false),
  m_splitBlayerModules(false),
  m_trainingBeamspotX(0.),
  m_trainingBeamspotY(0.),
  m_trainingBeamspotTiltX(0.),
  m_trainingBeamspotTiltY(0.),
  m_reverseIBLlocx(true)
{
  declareProperty("RawTrackContainer",        m_ftkRawTrackContainerKey);

  declareProperty("HitInputTool",             m_hitInputTool);
  declareProperty("SectorMatchTool",          m_sectorMatchTool);
  declareProperty("TrackBuilderTool",         m_trackBuilderTool);

  declareProperty("IBLMode",                  m_IBLMode);
  declareProperty("PMapPath_8L",              m_pmap_path_8L);
  declareProperty("PMapPath_12L",             m_pmap_path_12L);

  // clustering options
  declareProperty("Clustering",               m_clustering);
  declareProperty("SaveClusterContent",       m_saveClusterContent);
  declareProperty("DiagClustering",           m_diagClustering);
  declareProperty("SctClustering",            m_sctClustering);
  declareProperty("PixelClusteringMode",      m_pixelClusteringMode);
  declareProperty("Ibl3DRealistic",           m_ibl3DRealistic);
  declareProperty("DuplicateGanged",          m_duplicateGanged);
  declareProperty("GangedPatternRecognition", m_gangedPatternRecognition);
  declareProperty("SplitBlayerModules",       m_splitBlayerModules);

  declareProperty("TrainingBeamspotX",        m_trainingBeamspotX);
  declareProperty("TrainingBeamspotY",        m_trainingBeamspotY);
  declareProperty("TrainingBeamspotTiltX",    m_trainingBeamspotTiltX);
  declareProperty("TrainingBeamspotTiltY",    m_trainingBeamspotTiltY);

  declareProperty("ReverseIBLlocX",           m_reverseIBLlocx, "Reverse the direction of IBL locX from FTK");
}


StatusCode TrigFTKFastSimTruth::initialize()
{
  ATH_MSG_VERBOSE("TrigFTKFastSimTruth::initialize()");

  // FTK library global setup variables
  FTKSetup::getFTKSetup().setIBLMode(m_IBLMode);

  ATH_CHECK(m_ftkRawTrackContainerKey.initialize());

  if (m_pmap_path_8L.empty()) {
    ATH_MSG_FATAL("Main 8L plane map definition missing");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Loading 8L plane map from " << m_pmap_path_8L);
  m_pmap_8L.reset(new FTKPlaneMap(m_pmap_path_8L.c_str()));
  if (!m_pmap_8L->getIsOK()) {
      ATH_MSG_FATAL("Error using plane map: " << m_pmap_path_8L);
      return StatusCode::FAILURE;
  }

  if (m_pmap_path_12L.empty()) {
    ATH_MSG_FATAL("Main 12L plane map definition missing");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Loading 12L plane map from " << m_pmap_path_12L);
  m_pmap_12L.reset(new FTKPlaneMap(m_pmap_path_12L.c_str()));
  if (!m_pmap_12L->getIsOK()) {
      ATH_MSG_FATAL("Error using plane map: " << m_pmap_path_12L);
      return StatusCode::FAILURE;
  }


  ATH_CHECK(m_hitInputTool.retrieve());
  m_hitInputTool->reference()->setPlaneMaps(m_pmap_12L.get(), nullptr);

  ATH_CHECK(m_sectorMatchTool.retrieve());
  ATH_CHECK(m_trackBuilderTool.retrieve());

  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));


  if (m_clustering) {
    ATH_MSG_INFO("Creating FTKClusteringEngine");
    m_clusteringEngine.reset(new FTKClusteringEngine(m_saveClusterContent,
                                       m_diagClustering,
                                       m_sctClustering,
                                       m_ibl3DRealistic,
                                       m_duplicateGanged,
                                       m_gangedPatternRecognition,
                                       m_splitBlayerModules,
                                       m_pixelClusteringMode));
  }
  else {
    ATH_MSG_INFO("Clustering disabled");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigFTKFastSimTruth::execute()
{
  // retrieve the pointer to the datainput object
  FTKDataInput *datainput = m_hitInputTool->reference();

  // read the data in the current event
  datainput->readData();

  // retrieve the original list of hits, the list is copied because the clustering will change it
  std::vector<FTKRawHit> ftkHits = datainput->getOriginalHits();

  ATH_MSG_DEBUG("Got " << ftkHits.size() << " FTK hits");

  // if the clustering is requested it has to be done before the hits are distributed
  if ( m_clustering ) {
    m_clusteringEngine->atlClusteringLNF(ftkHits);
  }

  // Create and record the output container for FTK_RawTracks
  SG::WriteHandle<FTK_RawTrackContainer> ftkRawTrackContainer(m_ftkRawTrackContainerKey);
  ATH_CHECK(ftkRawTrackContainer.record(std::make_unique<FTK_RawTrackContainer>()));


  // Get all truth trajectories from clustered hits
  const auto truthTrajectories = buildTruthTrajectories(ftkHits);
  ATH_MSG_DEBUG("Got " << truthTrajectories.size() << " truth traj. from FTK hits");


  // Loop over all truth trajectories, see if they are selected
  // by the selectors and then create an FTK track from the truth
  // hits.
  for (const auto& traj: truthTrajectories) {
    // For each valid sector, store the mapping from hit to
    // FTK plane.
    std::vector<std::unordered_map<int, int> > sectorHitToPlaneMap;

    // Store sectorID and region (aka tower) for each matched sector
    std::vector<int> regions;
    std::vector<int> sectors;

    // Check if trajectory matches one or many FTK sectors
    // and store the sector, region and plane information
    if (!m_sectorMatchTool->match(traj, sectorHitToPlaneMap, regions, sectors)) {
      ATH_MSG_DEBUG("Trajectory does not match an FTK sector");
      continue;
    }

    // Create an FTKTrack from the hits
    std::unique_ptr<FTKTrack> ftktrack = m_trackBuilderTool->createFTKTrack(traj, sectorHitToPlaneMap, regions, sectors);

    if (!ftktrack) {
      ATH_MSG_DEBUG("Track creation for FTk truth trajectory with size " << traj.hits.size() << " failed. Skipping ...");
      continue;
    }

    ATH_MSG_VERBOSE("Track creation for FTk truth trajectory with size " << traj.hits.size() << " successful." );

    // Convert to FTK_RawTrack and store in container
    auto rawTrack = SimToRaw(*ftktrack);
    ftkRawTrackContainer->push_back(rawTrack.release());
  }

  return StatusCode::SUCCESS;
}

std::vector<FTKTruthTrajectory> TrigFTKFastSimTruth::buildTruthTrajectories(const std::vector<FTKRawHit>& ftkClusters) const
{
  // Combine all clusters that come from the same truth particle into
  // truth trajectories.
  // Note: as we store pointers to the FTKRawHits in the ftkClusters container
  // the output is only valid as long as the container is in scope, and is not
  // modified (which is ok for the purpose of TrigFTKFastSimTruth).

  // MultiTruth::Barcode is a std::pair<long, long> = (event index, barcode)
  std::map<MultiTruth::Barcode, FTKTruthTrajectory> truthTrajectories;

  for (const auto& hit: ftkClusters) {
    const auto truth = hit.getTruth();
    assert(truth);


    MultiTruth::Barcode barcode;
    MultiTruth::Weight weight;
    if (!truth->best(barcode, weight)) {
      ATH_MSG_DEBUG("Could not find 'best' particle from MultiTruth");
      continue;
    }

    if ((HepMcParticleLink::index_type)barcode.first == 0xFFFF) {
      // Bail out early in case of an invalid event index (= hit not associated to a truth particle),
      // because otherwise HepMcParticleLink will output a WARNING for the cptr() call.
      continue;
    }

    if (truthTrajectories.find(barcode) == truthTrajectories.end()) {
      HepMcParticleLink particleLink(barcode.second, barcode.first);

      FTKTruthTrajectory traj;
      traj.genParticle = particleLink.cptr();

      // The cluster is not associated to a truth particle
      if (!traj.genParticle)
        continue;

      // To speed up this part we could keep a 'blacklist' of geantinos/low pT
      // particles and not do the check every time, but probably that's not worth it.

      // ignore geantinos
      if (std::abs(traj.genParticle->pdg_id()) == 999)
        continue;

      // min pT: 400 MeV
      if (traj.genParticle->momentum().perp() < 400.0)
        continue;

      truthTrajectories[barcode] = traj;
    }

    truthTrajectories[barcode].hits.push_back(&hit);
  }

  // Put all truth trajectories into a std::vector for simpler usage
  std::vector<FTKTruthTrajectory> result;
  result.reserve(truthTrajectories.size());
  for (auto barcode_traj: truthTrajectories) {
    result.push_back(barcode_traj.second);
  }
  return result;
}

std::unique_ptr<FTK_RawTrack> TrigFTKFastSimTruth::SimToRaw(const FTKTrack &track) const
{
  // Convert the FTKTrack to an FTK_RawTrack
  auto raw_track = std::make_unique<FTK_RawTrack>();

  const unsigned int sctHashMax = m_sctId->wafer_hash_max();
  const unsigned int pixHashMax = m_pixelId->wafer_hash_max();

  unsigned int layerMap = 0;

  for (unsigned int iPlane = 0, isct=0; iPlane<static_cast<unsigned int>(track.getNPlanes()); ++iPlane){
    const FTKHit& hit = track.getFTKHit(iPlane);
    if (hit.getDim()== 0) {
      ATH_MSG_DEBUG("No hit for layer " << iPlane);
      continue;
    } 

    layerMap |= (1 << iPlane);

    ATH_MSG_VERBOSE("Information from FTKHit " << iPlane << ": sector " << hit.getSector()<< " plane " << hit.getPlane() << " etaCode " << hit.getEtaCode());
    ATH_MSG_VERBOSE(" isBarrel " << hit.getIsBarrel() << " Aside " << hit.getASide() << " Cside "<< hit.getCSide() );
    ATH_MSG_VERBOSE(" eta module " << hit.getEtaModule() << " phiModule " << hit.getPhiModule() << " section " << hit.getSection());
    ATH_MSG_VERBOSE(" etaWidth " << hit.getEtaWidth() << " phiWidth " << hit.getPhiWidth() << " nStrips " << hit.getPhiWidth() << " Dim " << hit.getDim());
    ATH_MSG_VERBOSE(" coord(0) " << hit.getHwCoord(0) << " localCoord(0) " << hit.getLocalCoord(0));
    if ( hit.getDim()> 1) ATH_MSG_VERBOSE(" coord(1) "<< hit.getHwCoord(1)<< " localCoord(1) " << hit.getLocalCoord(1));

    const IdentifierHash hitHashId = hit.getIdentifierHash();
    ATH_MSG_VERBOSE( "Decoding hit.getIdentifierHash() 0x"<<std::hex<< hit.getIdentifierHash() << " HashId 0x" << hitHashId<<std::dec);

    if (iPlane < 4) {
      ATH_MSG_VERBOSE( m_pixelId->print_to_string(m_pixelId->wafer_id(hitHashId)));
      ATH_MSG_VERBOSE(" phi module " << m_pixelId->phi_module(m_pixelId->wafer_id(hitHashId)) << " eta module " <<  m_pixelId->eta_module(m_pixelId->wafer_id(hitHashId)));
    } else {
      ATH_MSG_VERBOSE( m_sctId->print_to_string(m_sctId->wafer_id(hitHashId)));
      ATH_MSG_VERBOSE(" phi module " << m_sctId->phi_module(m_sctId->wafer_id(hitHashId)) << " eta module " <<  m_sctId->eta_module(m_sctId->wafer_id(hitHashId)));
    }

    const MultiTruth hittruth= hit.getTruth();
    MultiTruth::Barcode hitbarcode;
    MultiTruth::Weight tfrac;
    const bool truth_ok = hittruth.best(hitbarcode,tfrac);

    if ( iPlane<4 ) {
      // Pixel plane
      bool valid = true;
      if (hitHashId >= pixHashMax) {
        ATH_MSG_VERBOSE( " Invalid Pixel hashId " << hitHashId << " pixHashMax " << sctHashMax);
        valid = false;
      }
      ATH_MSG_VERBOSE( " Creating Pixel cluster for layer " << iPlane);

      float phi_index= hit.getHwCoord(0);
      float eta_index = hit.getHwCoord(1);

      if (m_reverseIBLlocx) {
        // Fix reverse IBL x (phi) coordinate for FTK in MC
        // This is controlled by the ReverseIBLlocX property
        const Identifier wafer_id = m_pixelId->wafer_id(hitHashId);
        const bool isBarrel = (m_pixelId->barrel_ec(wafer_id)==0);
        const unsigned int layer = m_pixelId->layer_disk(wafer_id);

        if (isBarrel && layer==0) {
          phi_index = IBL_PHI_COORD_MAX - phi_index;
        }
      }

      if (valid) {
        ATH_MSG_VERBOSE( " Creating Pixel cluster for layer " << iPlane << " hit.getDim() " <<hit.getDim());
        FTK_RawPixelCluster pixel_clus;
        pixel_clus.setModuleID(hit.getIdentifierHash());
        pixel_clus.setRowCoord(phi_index);
        pixel_clus.setRowWidth(hit.getPhiWidth());
        pixel_clus.setColCoord(eta_index);
        pixel_clus.setColWidth(hit.getEtaWidth());

        if (truth_ok) pixel_clus.setBarcode(hitbarcode.second);
        ATH_MSG_VERBOSE( " Adding Pixel cluster to track at layer " << iPlane);
        raw_track->setPixelCluster(iPlane,pixel_clus);
      } else {
        ATH_MSG_VERBOSE( " No valid Pixel cluster added for layer " << iPlane);
      }
    }
    else { // SCT plane
      bool valid = true;
      if (hitHashId >= sctHashMax) {
        ATH_MSG_INFO( " Invalid SCT hashId " << hitHashId << " sctHashMax " << sctHashMax);
        valid = false;
      }

      float sct_strip = hit.getHwCoord(0);

      if (valid) {
        ATH_MSG_VERBOSE( " Creating SCT cluster for layer " << iPlane);
        FTK_RawSCT_Cluster sct_clus; // iPlane and not iSCT
        sct_clus.setModuleID(hit.getIdentifierHash());
        sct_clus.setHitCoord(sct_strip); // loc pos
        sct_clus.setHitWidth(hit.getPhiWidth());

        if (truth_ok) {
          sct_clus.setBarcode(hitbarcode.second);
        }
        ATH_MSG_VERBOSE( " Adding SCT cluster to track for layer " << iPlane);
        raw_track->setSCTCluster(iPlane,sct_clus);
        isct++;
      }
      else {
        ATH_MSG_VERBOSE( " No valid SCT cluster added for layer " << iPlane);
      }
    }
  } // end of hit loop

  ATH_MSG_VERBOSE( " Setting track properties ");
  raw_track->setSectorID(track.getSectorID());
  raw_track->setLayerMap(layerMap);
  raw_track->setTower(track.getRegion());
  raw_track->setRoadID((unsigned int)track.getRoadID());

  float track_phi = track.getPhi();
  while (track_phi>TMath::Pi()) {
    track_phi -=TMath::TwoPi(); // get phi in range -pi to pi
  }
  while (track_phi<-TMath::Pi()) {
    track_phi +=TMath::TwoPi(); // get phi in range -pi to pi
  }
  raw_track->setPhi(track_phi);

  double dx = m_trainingBeamspotX + m_trainingBeamspotTiltX*track.getZ0(); //correction for tilt
  double dy = m_trainingBeamspotY + m_trainingBeamspotTiltY*track.getZ0(); //correction for tilt

  double d0 = track.getIP() - dx*sin(track_phi)+dy*cos(track_phi);
  double z0 = track.getZ0() - ((cos(track_phi) *dx + sin(track_phi)*dy))*track.getCotTheta();

  raw_track->setD0(d0);
  raw_track->setZ0(z0);

  raw_track->setCotTh(track.getCotTheta());
  raw_track->setChi2(track.getChi2());
  float invpt=0;
  if (track.getPt() != 0) {
    invpt = 1./track.getPt();
  }
  raw_track->setInvPt(invpt);
  raw_track->setBarcode(track.getBarcode());

  return raw_track;
}

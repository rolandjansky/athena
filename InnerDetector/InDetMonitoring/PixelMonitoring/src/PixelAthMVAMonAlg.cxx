/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelAthMVAMonAlg.cxx
 * @brief Collects reconstructed event information to evaluate per-module (or per-FE for IBL) classifier
 * @author Iskander Ibragimov
 **/
#include "PixelAthMVAMonAlg.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TTree.h"
#include "TFile.h"

PixelAthMVAMonAlg::PixelAthMVAMonAlg( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthMonitorAlgorithm(name, pSvcLocator),
  m_holeSearchTool("InDet::InDetTrackHoleSearchTool/InDetHoleSearchTool", this),
  m_trackSelTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this),
  m_trkextrapolator("Trk::Extrapolator/InDetExtrapolator"),
  m_atlasid(nullptr)
{
  declareProperty("HoleSearchTool", m_holeSearchTool);
  declareProperty("TrackSelectionTool", m_trackSelTool);
  declareProperty("Extrapolator", m_trkextrapolator);
  declareProperty("calibFolder", m_calibFolder = "20220503");
  declareProperty("dumpTree", m_dumpTree = false);
}

PixelAthMVAMonAlg::~PixelAthMVAMonAlg() {}

StatusCode PixelAthMVAMonAlg::initialize() {
  ATH_CHECK( PixelAthMonitoringBase::initialize());
  ATH_CHECK( detStore()->retrieve(m_atlasid, "AtlasID") );
  ATH_CHECK( m_pixelRDOName.initialize() );
  if ( !m_holeSearchTool.empty() ) ATH_CHECK( m_holeSearchTool.retrieve() );
  if ( !m_trackSelTool.empty() ) ATH_CHECK( m_trackSelTool.retrieve() );
  if ( !m_trkextrapolator.empty() ) ATH_CHECK( m_trkextrapolator.retrieve() );
  ATH_CHECK( m_trackParticlesKey.initialize() );
  ATH_CHECK( m_clustersKey.initialize() );
  
  for (int ii = 0; ii < PixLayers::NBASELAYERS; ++ii) {

    std::string partitionLabel("Disks");
    if (ii>=PixLayers::kBLayer) partitionLabel = pixBaseLayersLabel[ii];

    //    std::string fullPathToTrainingFile = partitionLabel + "_training.root"; //TEST
    std::string fullPathToTrainingFile = PathResolverFindCalibFile("PixelDQMonitoring/" + m_calibFolder + "/" + partitionLabel + "_training.root");
    
    std::unique_ptr<TFile> calibFile(TFile::Open(fullPathToTrainingFile.c_str(), "READ"));
    if ( !calibFile ) {
      ATH_MSG_ERROR("Can not retrieve PixelMonitoring calibration file: " << fullPathToTrainingFile);
      return StatusCode::FAILURE;
    }
    std::unique_ptr<TTree> trainingWeights( (TTree*)calibFile->Get("lgbm") );
    m_classBDT.emplace( std::make_pair(partitionLabel, std::make_unique<MVAUtils::BDT>( trainingWeights.get())) );

    m_modData[ii]  = Monitored::buildToolMap<int>(m_tools, 
						  pixBaseLayersLabel[ii], 
						  PixMon::pixEtaSteps[ii]*PixMon::pixPhiSteps[ii]);
  }
  return StatusCode::SUCCESS;
}


StatusCode PixelAthMVAMonAlg::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("Pixel MVAMon: Filling MVA Monitoring Histograms");

  using namespace Monitored;
  int lb = GetEventInfo(ctx)->lumiBlock();
  auto lbval    = Monitored::Scalar<int>( "pixmvamontool_lb", lb );
  
  const int MAXHASH = m_pixelid->wafer_hash_max(); // 2048

  //
  // fill status vector from conditions summary
  //
  SG::ReadHandle<InDet::SiDetectorElementStatus> pixel_status = getPixelDetElStatus(m_pixelDetElStatus, ctx);
  SG::ReadHandle<InDet::SiDetectorElementStatus> pixel_active = getPixelDetElStatus(m_pixelDetElStatusActiveOnly, ctx);
  std::vector<float> status(MAXHASH*2);

  PixelID::const_id_iterator idIt = m_pixelid->wafer_begin();
  PixelID::const_id_iterator idItEnd = m_pixelid->wafer_end();
  for (; idIt != idItEnd; ++idIt) {

    Identifier waferID = *idIt;
    IdentifierHash modHash = m_pixelid->wafer_hash(waferID);
    int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
    if (pixlayer == 99) continue;

    if (pixlayer == PixLayers::kIBL) 
      {
	// per FE for IBL
	//
	int nFE = getNumberOfFEs(pixlayer, m_pixelid->eta_module(waferID));
	for (int iFE=0; iFE<nFE; iFE++) {
	  Identifier pixID = m_pixelReadout->getPixelIdfromHash(modHash, iFE, 1, 1);
	  if (pixID.is_valid()) {
	    auto [is_active,is_good] = isChipGood( !m_pixelDetElStatusActiveOnly.empty() ? pixel_active.cptr() : nullptr,
						    !m_pixelDetElStatus.empty() ? pixel_status.cptr() : nullptr,
						    modHash, iFE);
	    if (is_active)
	      {
		if (is_good) status[modHash+MAXHASH*iFE] = 0;
		else status[modHash+MAXHASH*iFE] = 1;
	      } 
	    else status[modHash+MAXHASH*iFE] = 2;
	  }
	}
      } 
    else 
      {
	// per module for the old pixel layers
	//
	bool is_active = isActive( !m_pixelDetElStatusActiveOnly.empty() ? pixel_active.cptr() :  nullptr, modHash);
	bool is_good   = isGood( !m_pixelDetElStatus.empty() ? pixel_status.cptr() :  nullptr, modHash);
	if (is_active)
	  {
	    if (is_good) status[modHash] = 0;
	    else status[modHash] = 1;
	  } 
	else status[modHash] = 2;
      }
  }
  //
  // input data from tracks
  //

  auto trackParticles = SG::makeHandle(m_trackParticlesKey, ctx);
  if ( !(trackParticles.isValid()) ) {
    ATH_MSG_ERROR("Pixel MVAMon: TrackParticle container "<< m_trackParticlesKey.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel MVAMon: TrackParticle container "<< trackParticles.name() <<" is found.");
  }

  bool eventHasPixHits(false);
  std::vector<std::pair<Identifier, double> > ClusterIDs;

  std::vector<float> holes(MAXHASH*2);
  std::vector<float> outliers(MAXHASH*2);
  std::vector<float> measurements(MAXHASH*2);
  std::vector<float> trkalpha(MAXHASH*2);
  std::vector<float> trkchi2byndf(MAXHASH*2);
  std::vector<float> trknpixdead(MAXHASH*2);
  std::vector<float> trknblayerhits(MAXHASH*2);

  uint8_t iSummaryValue(0); // Dummy counter to retrieve summary values

  for (auto trackPart: *trackParticles) {
    const Trk::Track * track = trackPart->track();
    int numberOfPixelHits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHits) ? iSummaryValue : 0;

    if (track == nullptr || track->perigeeParameters() == nullptr || track->trackSummary() == nullptr || numberOfPixelHits == 0) {
      ATH_MSG_DEBUG("Pixel MVAMon: Track either invalid or it does not contain pixel hits, continuing...");
      continue;
    }

    int nPixelHits = 0;
    bool hasIBLTSOS(false);

    bool passJOTrkCut = static_cast<bool>( m_trackSelTool->accept(*track) );
    if (!passJOTrkCut) continue;
    float trkfitndf = track->fitQuality()->numberDoF();
    float trkfitchi2byndf = track->fitQuality()->chiSquared();
    if (trkfitndf != 0) trkfitchi2byndf /= trkfitndf;
    else continue; 
    float npixdead   = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelDeadSensors) ? iSummaryValue : 0;
    float nblayerhits(0);
    int expBLhit = trackPart->summaryValue(iSummaryValue, xAOD::expectNextToInnermostPixelLayerHit) ? iSummaryValue : 0;
    if (expBLhit==1) {
      nblayerhits = trackPart->summaryValue(iSummaryValue, xAOD::numberOfNextToInnermostPixelLayerHits) ? iSummaryValue : 0;
    }
    
    const Trk::Track* trackWithHoles( track );
    std::unique_ptr<const Trk::Track> trackWithHolesUnique = nullptr;
    int nPixHoles = trackPart->summaryValue(iSummaryValue, xAOD::numberOfPixelHoles) ? iSummaryValue : 0;
    if ( nPixHoles > 0 ) {
      trackWithHolesUnique.reset( m_holeSearchTool->getTrackWithHoles(*track) );
      trackWithHoles = trackWithHolesUnique.get();
    }
    const DataVector<const Trk::TrackStateOnSurface> *trackStates = trackWithHoles->trackStateOnSurfaces();
    for (auto trackStateOnSurface: *trackStates) {

      const Trk::MeasurementBase* mesBase = trackStateOnSurface->measurementOnTrack();

      // skip pseudomeasurements (only hits, holes, outliers are considered)
      //
      const Trk::RIO_OnTrack* RIOOnTrack = nullptr;
      if ( mesBase && mesBase->type(Trk::MeasurementBaseType::RIO_OnTrack) ) {
        RIOOnTrack = static_cast<const Trk::RIO_OnTrack*>(mesBase);
      }
      if (mesBase && !RIOOnTrack) continue;  

      // obtaining surfaceID
      //
      Identifier surfaceID;
      const Trk::TrackParameters* trkParameters = trackStateOnSurface->trackParameters();
      if (mesBase && mesBase->associatedSurface().associatedDetectorElement()) {
        surfaceID = mesBase->associatedSurface().associatedDetectorElement()->identify();
      } else {  // holes, perigee
	if (trkParameters) {
	  surfaceID = trkParameters->associatedSurface().associatedDetectorElementIdentifier();
	} else {
	  ATH_MSG_DEBUG("Pixel MVAMon: can't obtain track parameters for TSOS.");
	  continue;
	}
      }

      // selecting only pixel part
      //
      if (!m_atlasid->is_pixel(surfaceID)) continue;
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(surfaceID), m_pixelid->layer_disk(surfaceID) );
      if (pixlayer == 99) continue;
      if (pixlayer == PixLayers::kIBL) hasIBLTSOS = true;

      int indexModule = static_cast<int>( m_pixelid->wafer_hash(surfaceID) ); // [0,2047]

      const InDetDD::SiDetectorElement *sde = dynamic_cast<const InDetDD::SiDetectorElement *>(trkParameters->associatedSurface().associatedDetectorElement());
      const Trk::AtaPlane *trackAtPlane = dynamic_cast<const Trk::AtaPlane *>(trkParameters);
      const InDetDD::SiLocalPosition& trkLocalPos = trkParameters->localPosition();
      Identifier locPosID;
      int iFE(0);
      bool goodPixelMeasurement(false);

      if (trackStateOnSurface->type(Trk::TrackStateOnSurface::Outlier)) 
	{
	  if ( isIBL2D(indexModule) ) 
	    { 
	      const InDet::PixelClusterOnTrack *pixClus = dynamic_cast<const InDet::PixelClusterOnTrack *>(mesBase);
	      if ( mesBase && pixClus) { 
		locPosID = pixClus->identify();
		if ( !(locPosID.is_valid()) ) {
		  ATH_MSG_WARNING("Pixel MVAMon: got invalid track local position on surface for an outlier.");
		  continue;
		}
		iFE = m_pixelReadout->getFE(locPosID, locPosID);
		outliers[indexModule+MAXHASH*iFE]   += 1;
	      }
	    } 
	  else 
	    {
	      outliers[indexModule]                 += 1;
	    }
	} 
      else if (trackStateOnSurface->type(Trk::TrackStateOnSurface::Hole)) 
	{
	  if ( isIBL2D(indexModule) ) 
	    { 
	      locPosID = sde->identifierOfPosition(trkLocalPos);
	      if ( !(locPosID.is_valid()) ) {
		ATH_MSG_WARNING("Pixel MVAMon: got invalid track local position on surface for a hole.");
		continue;
	      }
	      iFE = m_pixelReadout->getFE(locPosID, locPosID);
	      holes[indexModule+MAXHASH*iFE]        += 1;
	    }
	  else 
	    {
	      holes[indexModule]                    += 1;
	    }
	} 
      else if (trackStateOnSurface->type(Trk::TrackStateOnSurface::Measurement)) 
	{
	  // making sure we get raw pixel clusters data
	  //
	  if (not mesBase) continue;
	  sde  = dynamic_cast<const InDetDD::SiDetectorElement *>(mesBase->associatedSurface().associatedDetectorElement());
	  const InDet::PixelClusterOnTrack *pixClus = dynamic_cast<const InDet::PixelClusterOnTrack *>(mesBase);
	  if (!sde || !pixClus) continue;
	  const InDet::PixelCluster *rawPixClus = pixClus->prepRawData();
	  if (!rawPixClus) continue;

	  nPixelHits++;
	  goodPixelMeasurement = true;
	  locPosID = pixClus->identify();
	  if ( !(locPosID.is_valid()) ) {
	    ATH_MSG_WARNING("Pixel MVAMon: got invalid cluster on track ID.");
	    continue;
	  }
	  if ( isIBL2D(indexModule) ) iFE = m_pixelReadout->getFE(locPosID, locPosID);
	  measurements[indexModule+MAXHASH*iFE] += 1;
	} 
      else continue;

      if (trackAtPlane) {
	Amg::Vector3D normal  = sde->normal();
	Amg::Vector3D trackp  = trackAtPlane->momentum();
	double trackpnormcomp = trackp.dot(normal);
	double trackp_mag     = trackp.mag();
	double alpha          = 0.5 * M_PI;
	if (trackp_mag != 0) alpha = std::acos(std::abs(trackpnormcomp / trackp_mag));

	trkalpha[indexModule+MAXHASH*iFE] += alpha;

	if (goodPixelMeasurement) ClusterIDs.emplace_back(locPosID, alpha);
      }
      trkchi2byndf[indexModule+MAXHASH*iFE]   += trkfitchi2byndf;
      trknpixdead[indexModule+MAXHASH*iFE]    += npixdead;
      trknblayerhits[indexModule+MAXHASH*iFE] += nblayerhits;
    } // end of TSOS loop
    eventHasPixHits = eventHasPixHits || (nPixelHits > 0);

    int expIBLhit = trackPart->summaryValue(iSummaryValue, xAOD::expectInnermostPixelLayerHit) ? iSummaryValue : 0;
    if (!hasIBLTSOS && expIBLhit==1 && nPixHoles==0 )
      {
	const Trk::Perigee *perigee = track->perigeeParameters();
	Amg::Transform3D transSurf;
	transSurf.setIdentity();
	const Trk::CylinderSurface BiggerThanIBLSurface(transSurf, 40.0, 400.0);
	std::vector<std::unique_ptr<Trk::TrackParameters> > iblTrkParameters = 
	  m_trkextrapolator->extrapolateStepwise(ctx, *perigee, BiggerThanIBLSurface, Trk::alongMomentum, false);
	Identifier sensorPosID, chipPosID;
	double alpha = 0.5 * M_PI;
	bool foundIBLElem(false); //more than one IBL element possible, only the last one is considered

	for (auto& tp: iblTrkParameters) {
	  const InDetDD::SiDetectorElement *sde = dynamic_cast<const InDetDD::SiDetectorElement *>(tp->associatedSurface().associatedDetectorElement());
	  if (!(sde != nullptr && sde->identify() != 0)) {
	    continue;
	  }
	  sensorPosID = sde->identify();
	  if (!m_atlasid->is_pixel(sensorPosID) || !m_pixelid->is_barrel(sensorPosID) || m_pixelid->layer_disk(sensorPosID)!= 0) {
	    ATH_MSG_DEBUG("Pixel MVAMon: found non-IBL element: "<< sensorPosID);
	    continue;
	  }
	  const InDetDD::SiLocalPosition& trkLocalPos = tp->localPosition();
	  chipPosID = sde->identifierOfPosition(trkLocalPos);
	  if ( !(chipPosID.is_valid()) ) {
	    foundIBLElem=false;
	    ATH_MSG_DEBUG("Pixel MVAMon: got invalid track local position on surface for an expected IBL hit.");
	    continue;
	  } else foundIBLElem=true;
	  double trackpnormcomp = tp->momentum().dot(tp->associatedSurface().normal());
	  double trackp_mag     = tp->momentum().mag();
	  if (trackp_mag != 0) alpha = std::acos(std::abs(trackpnormcomp / trackp_mag));
	}
	if (!foundIBLElem) {
	  ATH_MSG_DEBUG("Pixel MVAMon: couldn't find IBL pos ID: "<< sensorPosID);
	  continue;
	}
	int indexModule = static_cast<int>( m_pixelid->wafer_hash(sensorPosID) );
	int iFE = m_pixelReadout->getFE(chipPosID, chipPosID);
	holes[indexModule+MAXHASH*iFE]          += 1;
	trkalpha[indexModule+MAXHASH*iFE]       += alpha;
	trkchi2byndf[indexModule+MAXHASH*iFE]   += trkfitchi2byndf;
	trknpixdead[indexModule+MAXHASH*iFE]    += npixdead;
	trknblayerhits[indexModule+MAXHASH*iFE] += nblayerhits;
      }
  } // end of track loop

  if (!eventHasPixHits) {
    ATH_MSG_DEBUG("Pixel MVAMon: event doesn't contain any pixel hits on tracks, skipping evaluation.");
    return StatusCode::SUCCESS;
  }


  sort(ClusterIDs.begin(), ClusterIDs.end(), [](const std::pair<Identifier, double> &left, const std::pair<Identifier, double> &right) {
      return left.first < right.first;
    });

  std::vector<float> misshitsf(MAXHASH*2);
  float alltrackinfo;
  for (int ih=0; ih<MAXHASH; ++ih) {
    alltrackinfo = holes[ih] + outliers[ih] + measurements[ih]; 
    if (alltrackinfo) {
      misshitsf[ih]      = (holes[ih]+outliers[ih])/alltrackinfo;
      trkalpha[ih]       = trkalpha[ih]/alltrackinfo;
      trkchi2byndf[ih]   = trkchi2byndf[ih]/alltrackinfo;
      trknpixdead[ih]    = trknpixdead[ih]/alltrackinfo;
      trknblayerhits[ih] = trknblayerhits[ih]/alltrackinfo;
    }
    if ( isIBL2D(ih) ) {
      alltrackinfo = holes[ih+MAXHASH]+outliers[ih+MAXHASH]+measurements[ih+MAXHASH];
      if (alltrackinfo) {
	misshitsf[ih+MAXHASH]      = (holes[ih+MAXHASH]+outliers[ih+MAXHASH])/alltrackinfo;
	trkalpha[ih+MAXHASH]       = trkalpha[ih+MAXHASH]/alltrackinfo;
	trkchi2byndf[ih+MAXHASH]   = trkchi2byndf[ih+MAXHASH]/alltrackinfo;
	trknpixdead[ih+MAXHASH]    = trknpixdead[ih+MAXHASH]/alltrackinfo;
	trknblayerhits[ih+MAXHASH] = trknblayerhits[ih+MAXHASH]/alltrackinfo;
      }
    }
  }


  //
  // get CLUSTER information from container
  //
  std::vector<float> clsall(MAXHASH*2); 
  std::vector<float> clsontrkf(MAXHASH*2); 
  std::vector<float> clsontrk(MAXHASH*2); 
  std::vector<float> clsofftrk(MAXHASH*2);


  std::vector<float> clsontrksize(MAXHASH*2);
  std::vector<float> clsontrkrowsize(MAXHASH*2);
  std::vector<float> clsontrkcolsize(MAXHASH*2);
  std::vector<float> clsofftrksize(MAXHASH*2);
  std::vector<float> clsofftrkrowsize(MAXHASH*2);
  std::vector<float> clsofftrkcolsize(MAXHASH*2);

  std::vector<float> clsontrktot(MAXHASH*2); 
  std::vector<float> clsofftrktot(MAXHASH*2);

  auto pixel_clcontainer = SG::makeHandle(m_clustersKey, ctx);

  if ( !(pixel_clcontainer.isValid()) ) {
    ATH_MSG_ERROR("Pixel MVAMon: Pixel Cluster container "<< m_clustersKey.key() << " could not be found.");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Pixel MVAMon: Pixel Cluster container "<< pixel_clcontainer.name() <<" is found.");
  }

  Identifier clusID;

  for (auto colNext: *pixel_clcontainer) {

    const InDet::PixelClusterCollection* ClusterCollection(colNext);
    if (!ClusterCollection) {
      ATH_MSG_DEBUG("Pixel MVAMon: Pixel Cluster container is empty.");
      continue;
    }

    IdentifierHash clusIDHash = ClusterCollection->identifyHash();
    int indexModule = static_cast<int>( clusIDHash );

    for (auto p_clus: *ClusterCollection) {

      clusID = p_clus->identify();

      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(clusID), m_pixelid->layer_disk(clusID) );
      if (pixlayer == 99) continue;

      int iFE=0;
      if ( isIBL2D(indexModule) ) {
	iFE = m_pixelReadout->getFE(clusID, clusID);
      }
      int idxCluster = indexModule+MAXHASH*iFE;

      clsall[idxCluster] += 1;

      const InDet::PixelCluster& cluster = *p_clus;
      double alpha = 0.5 * M_PI;
      if ( isClusterOnTrack(clusID, ClusterIDs, alpha) ) {
	clsontrk[idxCluster]        += 1;
	clsontrksize[idxCluster]    += cluster.rdoList().size();
	clsontrkrowsize[idxCluster] += cluster.width().colRow().x();
	clsontrkcolsize[idxCluster] += cluster.width().colRow().y();
	clsontrktot[idxCluster]     += cluster.totalToT();
      } else {
	clsofftrk[idxCluster]       += 1;
	clsofftrksize[idxCluster]   += cluster.rdoList().size();
	clsofftrkrowsize[idxCluster]+= cluster.width().colRow().x();
	clsofftrkcolsize[idxCluster]+= cluster.width().colRow().y();
	clsofftrktot[idxCluster]    += cluster.totalToT();
      }
    }
  }

  ClusterIDs.clear();

  //
  // normalize cluster information
  //
  for (int ih=0; ih<MAXHASH; ++ih) {
    if (clsontrk[ih]) {
      clsontrksize[ih]   /= clsontrk[ih];
      clsontrkrowsize[ih]/= clsontrk[ih];
      clsontrkcolsize[ih]/= clsontrk[ih];
      clsontrktot[ih]    /= clsontrk[ih];
    }
    if (clsofftrk[ih]) {
      clsofftrksize[ih]   /= clsofftrk[ih];
      clsofftrkrowsize[ih]/= clsofftrk[ih];
      clsofftrkcolsize[ih]/= clsofftrk[ih];
      clsofftrktot[ih]    /= clsofftrk[ih];
    }
    if (clsall[ih]) {
      clsontrkf[ih]  = clsontrk[ih]/clsall[ih];
    }

    if ( isIBL2D(ih) ) {
      if (clsontrk[ih+MAXHASH]) {
	clsontrksize[ih+MAXHASH]   /= clsontrk[ih+MAXHASH];
	clsontrkrowsize[ih+MAXHASH]/= clsontrk[ih+MAXHASH];
	clsontrkcolsize[ih+MAXHASH]/= clsontrk[ih+MAXHASH];
	clsontrktot[ih+MAXHASH]    /= clsontrk[ih+MAXHASH];
      }
      if (clsofftrk[ih+MAXHASH]) {
	clsofftrksize[ih+MAXHASH]   /= clsofftrk[ih+MAXHASH];
	clsofftrkrowsize[ih+MAXHASH]/= clsofftrk[ih+MAXHASH];
	clsofftrkcolsize[ih+MAXHASH]/= clsofftrk[ih+MAXHASH];
	clsofftrktot[ih+MAXHASH]    /= clsofftrk[ih+MAXHASH];
      }
      if (clsall[ih+MAXHASH]) {
	clsontrkf[ih+MAXHASH]  = clsontrk[ih+MAXHASH]/clsall[ih+MAXHASH];
      }
    }
  }

  //
  // compute BDT weights
  //
  std::vector<float> bdtweights(MAXHASH*2);
  VecAccumulator2DMap BDT_Weights(*this, "BDTWeights");

  for (int ih=12; ih<MAXHASH-12; ++ih) 
    {
      int nFE(1);
      int module(0);
      float mod_eta(0);
      float el_eta(0);
      
      if ( ih>=156 && ih<=435 ) //IBL
	{ 
	  module = (ih-156) % 20;
	  nFE = 2;
	  if (module<4 || module>15) continue; //3D's are out of acceptance
	  else mod_eta = module-9.5;
	}
      else if (ih>=436 && ih<=721 ) //BLayer
	{
	  module = (ih-436) % 13;
	  el_eta = std::abs(module-6);
	}
      else if (ih>=722 && ih<=1215) //Layer1
	{
	  module = (ih-722) % 13;
	  el_eta = std::abs(module-6);
	}
      else if (ih>=1216 && ih<=1891) //Layer2
	{
	  module = (ih-1216) % 13;
	  el_eta = std::abs(module-6);
	}
      else if ( (ih>=12 && ih<=155) || (ih>=1892 && ih<=2035) ) //Disks
	{
	  //int idxdisks = ih - 12;
	  //if (ih>=1892) idxdisks-=1880;
	  el_eta = 0; // idxdisks / 48; 
	}
      else continue;
      
      Identifier waferID = m_pixelid->wafer_id(ih);
      int pixlayer = getPixLayersID(m_pixelid->barrel_ec(waferID), m_pixelid->layer_disk(waferID) );
      if (pixlayer == 99) continue;
      std::string partitionLabel("Disks");
      if (pixlayer>=PixLayers::kBLayer) partitionLabel = pixBaseLayersLabel[pixlayer];
      
      for (int iFE=0; iFE<nFE; iFE++) {
	Identifier pixID = waferID;
	if (pixlayer == PixLayers::kIBL) {
	  pixID = m_pixelReadout->getPixelIdfromHash(ih, iFE, 1, 1);
	  if (!pixID.is_valid()) {
	    ATH_MSG_ERROR("Pixel MVAMon: got invalid pixID " << pixID);
	    continue;
	  }
	  el_eta = mod_eta*2 + iFE;
	  if (mod_eta > 0) el_eta-=1.0;
	  el_eta = std::abs(el_eta);
	}
	
	int idx = ih+MAXHASH*iFE;
	if ( status[idx]==2 ) BDT_Weights.add(pixlayer, pixID, 0);
	if ( status[idx]!=0 || (measurements[idx]+holes[idx]+outliers[idx]==0) ) continue;
	
	std::vector<float> bdtVars;
	if (pixlayer == PixLayers::kIBL) {
	  bdtVars = { el_eta, misshitsf[idx], clsontrkf[idx], clsontrkrowsize[idx],
		      clsontrkcolsize[idx], clsofftrkrowsize[idx], clsofftrkcolsize[idx],
		      clsontrktot[idx], clsofftrktot[idx], trkalpha[idx],
		      trkchi2byndf[idx], trknpixdead[idx], trknblayerhits[idx] };
	} else {
	  bdtVars = { el_eta, misshitsf[idx], clsontrkf[idx], clsontrkrowsize[idx],
		      clsontrkcolsize[idx], clsofftrkrowsize[idx], clsofftrkcolsize[idx],
		      clsontrktot[idx], clsofftrktot[idx], trkalpha[idx],
		      trkchi2byndf[idx], trknpixdead[idx] };
	}
	bdtweights[idx] = m_classBDT.at(partitionLabel)->GetClassification(bdtVars);
	BDT_Weights.add(pixlayer, pixID, bdtweights[idx]);
      }
    }
  fill2DProfLayerAccum( BDT_Weights );
  fill1DModProfAccum( BDT_Weights, lb );
 
  if (m_dumpTree) {
    auto mvaGroup = getGroup("MVA");
    auto mon_status_vec           = Monitored::Collection("status_vec", status); 
    auto mon_h_vec                = Monitored::Collection("holes_vec", holes);
    auto mon_o_vec                = Monitored::Collection("outliers_vec", outliers);
    auto mon_m_vec                = Monitored::Collection("meas_vec", measurements);
    
    auto mon_clsontrkf_vec        = Monitored::Collection("clsontrkf_vec", clsontrkf);
    auto mon_clsontrk_vec         = Monitored::Collection("clsontrk_vec", clsontrk);
    auto mon_clsofftrk_vec        = Monitored::Collection("clsofftrk_vec", clsofftrk);
    auto mon_clsall_vec           = Monitored::Collection("clsall_vec", clsall);
    
    auto mon_clsontrksize_vec     = Monitored::Collection("clsontrksize_vec", clsontrksize);
    auto mon_clsontrkrowsize_vec  = Monitored::Collection("clsontrkrowsize_vec", clsontrkrowsize);
    auto mon_clsontrkcolsize_vec  = Monitored::Collection("clsontrkcolsize_vec", clsontrkcolsize);
    auto mon_clsofftrksize_vec    = Monitored::Collection("clsofftrksize_vec", clsofftrksize);
    auto mon_clsofftrkrowsize_vec = Monitored::Collection("clsofftrkrowsize_vec", clsofftrkrowsize);
    auto mon_clsofftrkcolsize_vec = Monitored::Collection("clsofftrkcolsize_vec", clsofftrkcolsize);
    
    auto mon_clsontrktot_vec      = Monitored::Collection("clsontrktot_vec", clsontrktot);
    auto mon_clsofftrktot_vec     = Monitored::Collection("clsofftrktot_vec", clsofftrktot);
    auto mon_trkalpha_vec         = Monitored::Collection("trkalpha_vec", trkalpha);
    auto mon_trkchi2byndf_vec     = Monitored::Collection("trkchi2byndf_vec", trkchi2byndf);
    auto mon_trknpixdead_vec      = Monitored::Collection("trknpixdead_vec", trknpixdead);
    auto mon_trknblayerhits_vec   = Monitored::Collection("trknblayerhits_vec", trknblayerhits);

    auto mon_mva_vec              = Monitored::Collection("mva_vec", bdtweights);

    fill( mvaGroup, lbval, mon_status_vec, mon_h_vec, mon_o_vec, mon_m_vec,
	  mon_clsontrkf_vec, mon_clsontrk_vec, mon_clsofftrk_vec, mon_clsall_vec,
	  mon_clsontrksize_vec, mon_clsontrkrowsize_vec, mon_clsontrkcolsize_vec,
	  mon_clsofftrksize_vec, mon_clsofftrkrowsize_vec, mon_clsofftrkcolsize_vec,
	  mon_clsontrktot_vec, mon_clsofftrktot_vec, mon_trkalpha_vec, mon_trkchi2byndf_vec,
	  mon_trknpixdead_vec, mon_trknblayerhits_vec, mon_mva_vec );
  }

  return StatusCode::SUCCESS;
}


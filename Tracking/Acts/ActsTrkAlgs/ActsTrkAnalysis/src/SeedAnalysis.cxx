/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SeedAnalysis.h"

#include "ReadoutGeometryBase/SiLocalPosition.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/PixelID.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "Acts/Definitions/Units.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

namespace ActsTrk {

  SeedAnalysis::SeedAnalysis(const std::string& name, ISvcLocator* pSvcLocator)
    : AthMonitorAlgorithm(name, pSvcLocator) 
  {}

  StatusCode SeedAnalysis::initialize() {
    ATH_MSG_INFO("Initializing " << name() << " ...");

    ATH_CHECK( m_beamSpotKey.initialize() );
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );

    ATH_CHECK( m_inputSeedColletionKey.initialize() );

    ATH_CHECK( m_prdTruth.initialize(not m_prdTruth.empty()));
    ATH_CHECK( m_detEleCollKey.initialize(not m_detEleCollKey.empty()) );

    ATH_CHECK( m_pixelClusterContainerKey.initialize(not m_prdTruth.empty() and m_usePixel) );
    ATH_CHECK( m_stripClusterContainerKey.initialize(not m_prdTruth.empty() and not m_usePixel) );

    ATH_CHECK( m_evtKey.initialize() );

    if (not m_prdTruth.empty()) {
      ATH_CHECK( m_paramEstimationTool.retrieve() );
      ATH_CHECK( m_trackingGeometryTool.retrieve() );
      ATH_CHECK( m_ATLASConverterTool.retrieve() );
    }

    return AthMonitorAlgorithm::initialize();
  }

  StatusCode SeedAnalysis::fillHistograms(const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Filling Histograms for " << name() << " ... " );

    // CONDS
    // Read the Beam Spot information
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    ATH_CHECK( beamSpotHandle.isValid() );
    auto beamSpotData = beamSpotHandle.cptr();
    
    // Read the b-field information
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle { m_fieldCondObjInputKey, ctx };
    ATH_CHECK( readHandle.isValid() );
    const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
    
    // Get the magnetic field
    // Using ACTS classes in order to be sure we are consistent
    Acts::MagneticFieldContext magFieldContext(fieldCondObj);
    // Beam Spot Position
    Acts::Vector2 beamPos( beamSpotData->beamPos()[ Amg::x ] * Acts::UnitConstants::mm,
			   beamSpotData->beamPos()[ Amg::y ] * Acts::UnitConstants::mm );
    
    // Magnetic Field
    ATLASMagneticFieldWrapper magneticField;
    Acts::MagneticFieldProvider::Cache magFieldCache = magneticField.makeCache( magFieldContext );
    Acts::Vector3 bField = *magneticField.getField( Acts::Vector3(beamPos.x(), beamPos.y(), 0),
						    magFieldCache );


    // SEEDS
    ATH_MSG_DEBUG( "Reading input collection with key " << m_inputSeedColletionKey.key() );
    SG::ReadHandle< ActsTrk::SeedContainer > handle = SG::makeHandle( m_inputSeedColletionKey, ctx );
    ATH_CHECK( handle.isValid() );
    const ActsTrk::SeedContainer* seed_collection = handle.get();
    ATH_MSG_DEBUG( "Retrieved " << seed_collection->size() << " input elements from key " << m_inputSeedColletionKey.key() );
    
    auto monitor_nseed = Monitored::Scalar<int>("Nseed", seed_collection->size());
    fill("ActsTrkSeedAnalysis", monitor_nseed);

    // bottom  
    auto monitor_x1 = 
      Monitored::Collection("x1", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->x(); });
    auto monitor_y1 = 
      Monitored::Collection("y1", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->y(); });
    auto monitor_z1 = 
      Monitored::Collection("z1", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[0]->z(); });
    auto monitor_r1 =
      Monitored::Collection("r1", *seed_collection,
			    [] (const auto* seed) -> double
			    {
			      const auto* sp = seed->sp()[0];
			      return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
			    });

    // middle
    auto monitor_x2 =
      Monitored::Collection("x2", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[1]->x(); });
    auto monitor_y2 =
      Monitored::Collection("y2", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[1]->y(); });
    auto monitor_z2 =
      Monitored::Collection("z2", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[1]->z(); });
    auto monitor_r2 =
      Monitored::Collection("r2", *seed_collection,
                            [] (const auto* seed) -> double
                            {
                              const auto* sp = seed->sp()[1];
                              return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
                            });
    
    // top
    auto monitor_x3 =
      Monitored::Collection("x3", *seed_collection,
			    [] (const auto* seed) -> double
			    { return seed->sp()[2]->x(); });
    auto monitor_y3 =
      Monitored::Collection("y3", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[2]->y(); });
    auto monitor_z3 =
      Monitored::Collection("z3", *seed_collection,
                            [] (const auto* seed) -> double
                            { return seed->sp()[2]->z(); });
    auto monitor_r3 =
      Monitored::Collection("r3", *seed_collection,
                            [] (const auto* seed) -> double
                            {
                              const auto* sp = seed->sp()[2];
                              return std::sqrt(sp->x()*sp->x() + sp->y()*sp->y());
                            });

    std::vector< std::array<float, 7> > parametersCollection;
    parametersCollection.reserve(seed_collection->size());

    for (const auto* seed : *seed_collection) {
      parametersCollection.push_back( estimateParameters(*seed, 300. * bField[2] / 1000.) );
    }
    
    auto monitor_param_pt = Monitored::Collection("pt", parametersCollection,
						  [] (const auto& params) -> float
						  { return params[0]; });
    auto monitor_param_theta = Monitored::Collection("theta", parametersCollection,
						     [] (const auto& params) -> float
						     { return params[1]; });
    auto monitor_param_eta = Monitored::Collection("eta", parametersCollection,
						   [] (const auto& params) -> float
						   { return params[2]; });
    auto monitor_param_d0 = Monitored::Collection("d0", parametersCollection,
						  [] (const auto& params) -> float
						  { return params[3]; });

    auto monitor_param_dzdr_b = Monitored::Collection("dzdr_b", parametersCollection,
						      [] (const auto& params) -> float
						      { return params[4]; });
    auto monitor_param_dzdr_t = Monitored::Collection("dzdr_t", parametersCollection,
						      [] (const auto& params) -> float
						      { return params[5]; });

    
    auto monitor_param_penalty = Monitored::Collection("penalty", parametersCollection,
						       [] (const auto& params) -> float
						       { return params[6]; });
      

    SG::ReadHandle<xAOD::EventInfo> eventInfo = SG::makeHandle(m_evtKey, ctx);
    ATH_CHECK(eventInfo.isValid());

    auto monitor_event_number = Monitored::Scalar<long>("event_number", static_cast<long>(eventInfo->eventNumber()));
    auto monitor_actual_mu = Monitored::Scalar<float>("actual_mu", eventInfo->actualInteractionsPerCrossing());

    std::vector<int> vec_truthBarcode;
    std::vector<double> vec_truthProb;
    if (not m_prdTruth.empty())
      ATH_CHECK( fillTruthHistograms(ctx, *seed_collection, vec_truthBarcode, vec_truthProb) );
    auto monitor_truth_barcode = Monitored::Collection("truth_barcode", vec_truthBarcode);
    auto monitor_truth_prob = Monitored::Collection("truth_prob", vec_truthProb);

    fill("ActsTrkSeedAnalysis",
	 monitor_x1, monitor_y1, monitor_z1, monitor_r1,
	 monitor_x2, monitor_y2, monitor_z2, monitor_r2,
	 monitor_x3, monitor_y3, monitor_z3, monitor_r3,
	 monitor_param_pt, monitor_param_theta, monitor_param_eta, monitor_param_d0,
	 monitor_param_dzdr_b, monitor_param_dzdr_t,
	 monitor_param_penalty,
	 monitor_event_number, monitor_actual_mu,
	 monitor_truth_barcode, monitor_truth_prob);
        
    return StatusCode::SUCCESS;
  }

  StatusCode SeedAnalysis::fillTruthHistograms(const EventContext& ctx,
					       const ActsTrk::SeedContainer& seed_container,
					       std::vector<int>& truthBarCodeVec,
					       std::vector<double>& truthProbVec) const
  {
    ATH_MSG_DEBUG( "Filling Truth Histograms for " << name() << " ... " );

    SG::ReadHandle< PRD_MultiTruthCollection > prdTruthHandle = SG::makeHandle( m_prdTruth, ctx );
    ATH_CHECK(prdTruthHandle.isValid());
    const PRD_MultiTruthCollection* prdTruth = prdTruthHandle.get();

    SG::ReadCondHandle< InDetDD::SiDetectorElementCollection > detEleHandle( m_detEleCollKey, ctx );
    ATH_CHECK( detEleHandle.isValid() );
    const InDetDD::SiDetectorElementCollection* detEle = detEleHandle.retrieve();

    // Read the b-field information
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle { m_fieldCondObjInputKey, ctx };
    ATH_CHECK( readHandle.isValid() );
    
    const AtlasFieldCacheCondObj* fieldCondObj{ *readHandle };
    ATH_CHECK( fieldCondObj != nullptr );

    Acts::MagneticFieldContext magFieldContext(fieldCondObj);
    auto geo_context = m_trackingGeometryTool->getNominalGeometryContext();



    // Need to retrieve Clusters
    std::variant<const xAOD::PixelClusterContainer*, const xAOD::StripClusterContainer*> input_cluster_collection;

    if (m_usePixel) {
      SG::ReadHandle<xAOD::PixelClusterContainer> inputClusterContainer( m_pixelClusterContainerKey, ctx );
      ATH_CHECK( inputClusterContainer.isValid() );
      input_cluster_collection = inputClusterContainer.cptr();
    } else {
      SG::ReadHandle<xAOD::StripClusterContainer> inputClusterContainer( m_stripClusterContainerKey, ctx );
      ATH_CHECK( inputClusterContainer.isValid() );
      input_cluster_collection = inputClusterContainer.cptr();
    }


    // utilities
    // Used for param estimation
    auto retrieveSurfaceFunction = 
      [this, &input_cluster_collection, &detEle] (const Acts::Seed<ActsTrk::SpacePoint>& seed) -> const Acts::Surface& 
      { 
	std::size_t bottom_idx = seed.sp().front()->measurementIndexes()[0];

	const InDetDD::SiDetectorElement* Element = std::visit([&detEle,&bottom_idx] (const auto* collection) {
	    const auto* cluster = collection->at(bottom_idx);
	    return detEle->getDetectorElement(cluster->identifierHash());
	  }, input_cluster_collection);
	
	const Trk::Surface& atlas_surface = Element->surface();
	return this->m_ATLASConverterTool->ATLASSurfaceToActs(atlas_surface);
      };


    // computation
    std::vector<bool> vec_pass;
    vec_pass.reserve(seed_container.size());

    std::vector<double> estimated_pt;
    std::vector<double> estimated_eta;
    estimated_pt.reserve(seed_container.size());
    estimated_eta.reserve(seed_container.size());

    for (const auto* seed : seed_container) { 
      std::optional<Acts::BoundTrackParameters> optTrackParams =
        m_paramEstimationTool->estimateTrackParameters(ctx,
						       *seed,
						       geo_context.context(),
						       magFieldContext,
						       retrieveSurfaceFunction);

      if ( not optTrackParams.has_value() ) continue;

      const auto param = optTrackParams.value();
      estimated_pt.push_back( param.transverseMomentum() );
      estimated_eta.push_back( -std::log( std::tan(0.5 * param.parameters()[Acts::eBoundTheta]) ) );

      std::map<int, int> truthHits;
      
      const auto& sps = seed->sp();
      for (const auto* sp : sps) {
	int number_of_clusters = m_usePixel ? 1 : 2;
	for (int cluster_number(0); cluster_number < number_of_clusters; cluster_number++) {
	  int cluster_index = sp->measurementIndexes()[cluster_number];
	  const Identifier id = std::visit([&cluster_index,this] (const auto* collection) -> const Identifier
					   {
					     const auto* cluster = collection->at(cluster_index);
					     return identify(*cluster);
					   }, input_cluster_collection);
	  matchParticleToSeedClusters(prdTruth, id, truthHits);
	}
      }

      auto [barcode, prob] = findSeedMajorityTruthParticle(truthHits);
      truthBarCodeVec.push_back(barcode);
      truthProbVec.push_back(prob);
      vec_pass.push_back( barcode != 0 and prob > 0.5 );
    }

    auto monitor_estimated_pt = Monitored::Collection("estimated_pt", estimated_pt);
    auto monitor_estimated_eta = Monitored::Collection("estimated_eta", estimated_eta);
    auto monitor_pass = Monitored::Collection("passed", vec_pass);

    fill("ActsTrkSeedAnalysis",
	 monitor_pass,
	 monitor_estimated_pt, monitor_estimated_eta);

    return StatusCode::SUCCESS;
  }

  const Identifier SeedAnalysis::identify(const xAOD::PixelCluster& cluster) const
  {
    static const SG::AuxElement::Accessor< ElementLink< InDet::PixelClusterCollection > > pixelLinkAcc("pixelClusterLink");

    // TO-DO -- AODs will not have this decoration, we'll need to provide a function for recomputing 
    // the identifier from local position
    if (not cluster.isAvailable< ElementLink< InDet::PixelClusterCollection > >("pixelClusterLink"))
      return Identifier();

    ElementLink<InDet::PixelClusterCollection> pixelLink = pixelLinkAcc(cluster);
    return (*pixelLink)->identify();
  }

  const Identifier SeedAnalysis::identify(const xAOD::StripCluster& cluster) const
  {
    static const SG::AuxElement::Accessor< ElementLink< InDet::SCT_ClusterCollection > > stripLinkAcc("sctClusterLink");

    // TO-DO -- AODs will not have this decoration, we'll need to provide a function for recomputing 
    // the identifier from local position
    if (not cluster.isAvailable< ElementLink< InDet::SCT_ClusterCollection > >("sctClusterLink"))
      return Identifier();

    ElementLink<InDet::SCT_ClusterCollection> stripLink = stripLinkAcc(cluster);
    return (*stripLink)->identify();
  }


  void SeedAnalysis::matchParticleToSeedClusters(const PRD_MultiTruthCollection* prdTruth, 
						 const Identifier& id, 
						 std::map<int, int>& countMap) const {
    auto n1 = prdTruth->count(id);
    if (n1 == 0) {
      int bc = 0;
      auto nBC = countMap.count(bc);
      if (nBC == 0) {
	countMap[bc] = 1;
      } else {
	countMap[bc] += 1;
      }
    } else {
      using iprdt = PRD_MultiTruthCollection::const_iterator;
      std::pair<iprdt, iprdt> range = prdTruth->equal_range(id);
      for (iprdt itr = range.first; itr != range.second; ++itr) {
	auto bc = itr->second.barcode();
	auto nBC = countMap.count(bc);
	if (nBC == 0) {
	  countMap[bc] = 1;
	} else {
	  countMap[bc] += 1;
	}
      }
    }
  }



  std::pair<int, double> SeedAnalysis::findSeedMajorityTruthParticle(const std::map<int, int>& countMap) const {
    int bestCount = 0;
    int bestBarcode = std::numeric_limits<int>::min();
  
    for (auto const& [barcode, count] : countMap) {
      if (count > bestCount) {
	bestCount = count;
	bestBarcode = barcode;
      }
    }

    // 3 spacepoints per seed, 1 (2) clusters per spacepoint for pixel (strip)
    double nTotal = m_usePixel ? 3. : 6.;
    double prob = bestCount / nTotal;

    return std::make_pair(bestBarcode, prob);
  }

  // Same computation as in ActsTrk::SiSpacePointsSeedMaker
  std::array<float, 7> SeedAnalysis::estimateParameters(const ActsTrk::Seed& seed,
							float pTPerHelixRadius) const
  {
    auto extractCoordinates = 
      [] (const ActsTrk::SpacePoint* sp) -> std::array<float,4>
      {
	std::array<float, 4> coordinates {static_cast<float>(sp->x()), 
					  static_cast<float>(sp->y()), 
					  static_cast<float>(sp->z()), 
					  static_cast<float>(std::sqrt(sp->x()*sp->x() + sp->y()*sp->y()))};
	return coordinates;
      };
    
    auto extractQuantities = 
      [] (const std::array<float, 4>& sp,
	  const std::array<float, 4>& spM,
	  bool isBottom) -> std::array<float, 5>
      {
	auto& [xM, yM, zM, rM] = spM;
	auto& [xO, yO, zO, rO] = sp;

	float cosPhiM = xM / rM;
	float sinPhiM = yM / rM;
	float deltaX = xO - xM;
	float deltaY = yO - yM;
	float deltaZ = zO - zM;
	float x = deltaX * cosPhiM + deltaY * sinPhiM;
	float y = deltaY * cosPhiM - deltaX * sinPhiM;
	float iDeltaR2 = 1. / (deltaX * deltaX + deltaY * deltaY);
	float iDeltaR = std::sqrt(iDeltaR2);
	int bottomFactor = int(not isBottom) - int(isBottom);
	float cot_theta = deltaZ * iDeltaR * bottomFactor;

	// cotTheta, Zo, iDeltaR, U, V
	std::array<float, 5> params =
	{ 
	  cot_theta, 
	  zM - rM * cot_theta,
	  iDeltaR,
	  x * iDeltaR2,
	    y * iDeltaR2
	};

	return params;
      };

    const auto& sps = seed.sp();
    const auto* bottom = sps[0];
    const auto* medium = sps[1];
    const auto* top = sps[2];

    auto coo_b = extractCoordinates(bottom);
    auto coo_m = extractCoordinates(medium);
    auto coo_t = extractCoordinates(top);

    // Compute the variables we need
    auto [cotThetaB, Zob, iDeltaRB, Ub, Vb] = extractQuantities(coo_b, coo_m, true);
    auto [cotThetaT, Zot, iDeltaRT, Ut, Vt] = extractQuantities(coo_t, coo_m, false);

    float squarediDeltaR2B = iDeltaRB*iDeltaRB;
    float squarediDeltaR2T = iDeltaRB*iDeltaRT;
    float squarediDeltaR = std::min(squarediDeltaR2B, squarediDeltaR2T);

    auto& [xB, yB, zB, rB] = coo_b;
    auto& [xM, yM, zM, rM] = coo_m;
    auto& [xT, yT, zT, rT] = coo_t;

    float ax = xM / rM;
    float ay = yM/ rM;

    float dxb = xM - xB;
    float dyb = yM - yB;
    float dzb = zM - zB;
    float xb = dxb * ax + dyb *ay;
    float yb = dyb * ax - dxb * ay; 
    float dxyb = xb * xb + yb * yb;                                                                                                                                                                                                                                            
    float dxt = xT - xM;
    float dyt = yT - yM;
    float dzt = zT - zM;
    float xt = dxt * ax + dyt *ay;
    float yt = dyt * ax - dxt * ay;
    float dxyt = xt * xt + yt * yt;

    float tzb = dzb * std::sqrt( 1./dxyb );
    float tzt = dzt * std::sqrt( 1./dxyt );

    float sTzb2 = std::sqrt(1 + tzb*tzb);

    float dU = Ut - Ub;
    if (dU == 0.) {
      return {-1, -1, -1, -1, -1, -1, -1};
    }

    float A = (Vt - Vb) / dU;
    float S2 = 1. + A * A;
    float B = Vb - A * Ub;
    float B2 = B * B;

    // dzdr
    float dzdr_b = (zM - zB) / (rM - rB);
    float dzdr_t = (zT - zM) / (rT - rM);    

    // eta
    float theta = std::atan(1. / std::sqrt(cotThetaB * cotThetaT));
    float eta = -std::log(std::tan(0.5 * theta));

    // pt
    float pt = pTPerHelixRadius * std::sqrt(S2 / B2) / 2.;

    // d0
    float d0 = std::abs((A - B * rM) * rM);

    // curvature 
    // not used in current version of the code. We may want to use it later
    //    float curvature = B / std::sqrt(S2);
    float penalty = std::abs((tzb - tzt) / (squarediDeltaR * sTzb2));

    return {pt, theta, eta, d0, dzdr_b, dzdr_t, penalty};
  }

}

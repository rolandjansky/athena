/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "SeedAnalysis.h"

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
    
    fill("ActsTrkSeedAnalysis",
	 monitor_x1, monitor_y1, monitor_z1, monitor_r1,
	 monitor_x2, monitor_y2, monitor_z2, monitor_r2,
	 monitor_x3, monitor_y3, monitor_z3, monitor_r3,
	 monitor_param_pt, monitor_param_theta, monitor_param_eta, monitor_param_d0,
	 monitor_param_dzdr_b, monitor_param_dzdr_t,
	 monitor_param_penalty);

    return StatusCode::SUCCESS;
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

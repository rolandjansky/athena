/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"

#include "ActsGeometry/ActsATLASConverterTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"

// PACKAGE
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ATLASSourceLink.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"

#include "ActsInterop/IdentityHelper.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Definitions/Units.hpp"

// STL
#include <iostream>
#include <memory>
#include <cmath>


ActsATLASConverterTool::ActsATLASConverterTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode
ActsATLASConverterTool::initialize()
{
  ATH_MSG_INFO("Initializing ACTS to ATLAS converter tool");

  ATH_CHECK( m_trackingGeometryTool.retrieve() );
  m_trackingGeometry = m_trackingGeometryTool->trackingGeometry();

  m_trackingGeometry->visitSurfaces([&](const Acts::Surface *surface) {
    // find acts surface with the same detector element ID
    if (surface and surface->associatedDetectorElement())
    {
      const auto* actsElement = dynamic_cast<const ActsDetectorElement*>(surface->associatedDetectorElement());
      // Conversion from Acts to ATLAS surface impossible for the TRT so the TRT surface are not sure in this map
      bool isTRT = (dynamic_cast<const InDetDD::TRT_BaseElement*>(actsElement->upstreamDetectorElement()) != nullptr);
      if(actsElement && !isTRT
                     && m_actsSurfaceMap.find(actsElement->identify()) == m_actsSurfaceMap.end()){
        m_actsSurfaceMap.insert(std::pair<Identifier, const Acts::Surface &>(actsElement->identify(), *surface));
      }
    }
    return;
  });  
  return StatusCode::SUCCESS;
}

const Trk::Surface&
ActsATLASConverterTool::ActsSurfaceToATLAS(const Acts::Surface &actsSurface) const {

  const auto* actsElement = dynamic_cast<const ActsDetectorElement*>(actsSurface.associatedDetectorElement());
  if ( actsSurface.associatedDetectorElement() && actsElement){
    return actsElement->atlasSurface();
  }
  throw std::domain_error("No ATLAS surface corresponding to to the Acts one");
}

const Acts::Surface&
ActsATLASConverterTool::ATLASSurfaceToActs(const Trk::Surface &atlasSurface) const {
  
  Identifier atlasID = atlasSurface.associatedDetectorElementIdentifier();
  if (m_actsSurfaceMap.find(atlasID) != m_actsSurfaceMap.end()){
    return m_actsSurfaceMap.at(atlasID);
  }
  throw std::domain_error("No Acts surface corresponding to to the ATLAS one");
}

const ATLASSourceLink
ActsATLASConverterTool::ATLASMeasurementToSourceLink(const Trk::MeasurementBase *measurement) const {

  const Acts::Surface& surface = ATLASSurfaceToActs(measurement->associatedSurface());
  Acts::BoundVector loc = Acts::BoundVector::Zero();
  Acts::BoundMatrix cov = Acts::BoundMatrix::Zero();
  int dim = measurement->localParameters().dimension();
  if(dim==1){
    loc[Acts::eBoundLoc0] = measurement->localParameters()[Trk::locX];
    cov.topLeftCorner<1, 1>() = measurement->localCovariance();
  }
  else if(dim>=2){
  loc[Acts::eBoundLoc0] = measurement->localParameters()[Trk::locX];
  loc[Acts::eBoundLoc1] = measurement->localParameters()[Trk::locY];
  cov.topLeftCorner<2, 2>() = measurement->localCovariance();
  }
  else{
    throw std::domain_error("Incorect local parameter dimension");
  }
  return ATLASSourceLink(surface, *measurement, dim, loc, cov);
}

const std::vector<ATLASSourceLink>
ActsATLASConverterTool::ATLASTrackToSourceLink(const Trk::Track &track) const {
  
  auto hits = track.measurementsOnTrack();   
  auto outliers = track.outliersOnTrack();  

  std::vector<ATLASSourceLink> sourceLinks;
  sourceLinks.reserve(hits->size()+outliers->size());

  for (auto it = hits->begin(); it != hits->end(); ++it){
    sourceLinks.push_back(ATLASMeasurementToSourceLink(*it));
  }
  for (auto it = outliers->begin(); it != outliers->end(); ++it){
    sourceLinks.push_back(ATLASMeasurementToSourceLink(*it));
  }
  return sourceLinks;
}

const Acts::BoundTrackParameters
ActsATLASConverterTool::ATLASTrackParameterToActs(const Trk::TrackParameters *atlasParameter) const {

  using namespace Acts::UnitLiterals;
  std::shared_ptr<const Acts::Surface> actsSurface;
  Acts::BoundVector params;
  
  // get the associated surface
  if (atlasParameter->hasSurface() && atlasParameter->associatedSurface().owner() != Trk::SurfaceOwner::noOwn)
  {
    actsSurface = ATLASSurfaceToActs(atlasParameter->associatedSurface()).getSharedPtr();
  }
  // no associated surface create a perigee one
  else{
    actsSurface = Acts::Surface::makeShared<const Acts::PerigeeSurface>(
          Acts::Vector3(0., 0., 0.));
  }

  // Construct track parameters
  auto atlasParam = atlasParameter->parameters();
  params << atlasParam[Trk::locX], atlasParam[Trk::locY], atlasParam[Trk::phi0], atlasParam[Trk::theta], atlasParameter->charge() / (atlasParameter->momentum().mag() * 1_MeV), 0.;

  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Identity();
  cov.topLeftCorner(5, 5) = *atlasParameter->covariance();

  // Convert the covariance matrix to MeV
  for(int i=0; i < cov.rows(); i++){
    cov(i, 4) = cov(i, 4)/1_MeV;
  }
  for(int i=0; i < cov.cols(); i++){
    cov(4, i) = cov(4, i)/1_MeV;
  }
    
  return Acts::BoundTrackParameters(actsSurface, params, atlasParameter->charge(), cov);
}

const Trk::TrackParameters*
ActsATLASConverterTool::ActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const {

  using namespace Acts::UnitLiterals;
  std::optional<AmgSymMatrix(5)> cov = std::nullopt;
  if (actsParameter.covariance()){
    AmgSymMatrix(5) newcov(actsParameter.covariance()->topLeftCorner<5, 5>(0, 0));
    // Convert the covariance matrix to GeV
    for(int i=0; i < newcov.rows(); i++){
      newcov(i, 4) = newcov(i, 4)*1_MeV;
    }
    for(int i=0; i < newcov.cols(); i++){
      newcov(4, i) = newcov(4, i)*1_MeV;
    }
    cov =  std::optional<AmgSymMatrix(5)>(newcov);
  }

  const Acts::Surface& actsSurface = actsParameter.referenceSurface();
  
  switch (actsSurface.type()){

    case Acts::Surface::SurfaceType::Cone : {
      const Trk::ConeSurface &coneSurface = dynamic_cast<const Trk::ConeSurface &>(ActsSurfaceToATLAS(actsSurface));
      const Trk::AtaCone * cone = new Trk::AtaCone(actsParameter.get<Acts::eBoundLoc0>(),
                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                   coneSurface,
                                                   cov);
      return cone;
      break;
    }
    case Acts::Surface::SurfaceType::Cylinder : {
      const Trk::CylinderSurface &cylSurface = dynamic_cast<const Trk::CylinderSurface &>(ActsSurfaceToATLAS(actsSurface));
      const Trk::AtaCylinder * cyl = new Trk::AtaCylinder(actsParameter.get<Acts::eBoundLoc0>(),
                                                          actsParameter.get<Acts::eBoundLoc1>(),
                                                          actsParameter.get<Acts::eBoundPhi>(),
                                                          actsParameter.get<Acts::eBoundTheta>(),
                                                          actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                          cylSurface,
                                                          cov);
      return cyl;
      break;
    } 
    case Acts::Surface::SurfaceType::Disc : {
      const Trk::DiscSurface &discSurface = dynamic_cast<const Trk::DiscSurface &>(ActsSurfaceToATLAS(actsSurface));
      const Trk::AtaDisc * disc = new Trk::AtaDisc(actsParameter.get<Acts::eBoundLoc0>(),
                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                   discSurface,
                                                   cov);
      return disc;
      break;
    } 
    case Acts::Surface::SurfaceType::Perigee : {
      const Trk::PerigeeSurface perSurface(actsSurface.center(gctx));
      const Trk::Perigee * per = new Trk::Perigee(actsParameter.get<Acts::eBoundLoc0>(),
                                                  actsParameter.get<Acts::eBoundLoc1>(),
                                                  actsParameter.get<Acts::eBoundPhi>(),
                                                  actsParameter.get<Acts::eBoundTheta>(),
                                                  actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                  perSurface,
                                                  cov);
      return per;
      break;
    } 
    case Acts::Surface::SurfaceType::Plane : {
      const Trk::PlaneSurface &plaSurface = dynamic_cast<const Trk::PlaneSurface &>(ActsSurfaceToATLAS(actsSurface));
      const Trk::AtaPlane * plan = new Trk::AtaPlane(actsParameter.get<Acts::eBoundLoc0>(),
                                                     actsParameter.get<Acts::eBoundLoc1>(),
                                                     actsParameter.get<Acts::eBoundPhi>(),
                                                     actsParameter.get<Acts::eBoundTheta>(),
                                                     actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                     plaSurface,
                                                     cov);
      return plan;
      break;
    } 
    case Acts::Surface::SurfaceType::Straw : {
      const Trk::StraightLineSurface &lineSurface = dynamic_cast<const Trk::StraightLineSurface &>(ActsSurfaceToATLAS(actsSurface));
      const Trk::AtaStraightLine * line = new Trk::AtaStraightLine(actsParameter.get<Acts::eBoundLoc0>(),
                                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                                   lineSurface,
                                                                   cov);
      return line;
      break;
    }
    case Acts::Surface::SurfaceType::Curvilinear : {
      const Trk::CurvilinearParameters * curv = new Trk::CurvilinearParameters(actsParameter.position(gctx),
                                                                               actsParameter.get<Acts::eBoundPhi>(),
                                                                               actsParameter.get<Acts::eBoundTheta>(),
                                                                               actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                                               cov);
      return curv;                                                                               
      break;
    }  
    case Acts::Surface::SurfaceType::Other : {
      break;
    }
  }

  throw std::domain_error("Surface type not found");
} 

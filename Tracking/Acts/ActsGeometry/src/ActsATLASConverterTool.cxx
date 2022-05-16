/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "TrkExUtils/RungeKuttaUtils.h"

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
#include "Acts/EventData/detail/TransformationFreeToBound.hpp"
#include "Acts/EventData/detail/TransformationBoundToFree.hpp"
#include "Acts/Propagator/CovarianceTransport.hpp"

// STL
#include <iostream>
#include <memory>
#include <cmath>
#include <random>


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
      if(actsElement){
        bool isTRT = (dynamic_cast<const InDetDD::TRT_BaseElement*>(actsElement->upstreamDetectorElement()) != nullptr);
        if(!isTRT && m_actsSurfaceMap.find(actsElement->identify()) == m_actsSurfaceMap.end()){
          m_actsSurfaceMap.insert(std::pair<Identifier, const Acts::Surface &>(actsElement->identify(), *surface));
        }
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
  throw std::domain_error("No Acts surface corresponding to the ATLAS one");
}

const ATLASSourceLink
ActsATLASConverterTool::ATLASMeasurementToSourceLink(
    const Acts::GeometryContext& gctx, 
    const Trk::MeasurementBase *measurement) const {

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
    throw std::domain_error("Cannot handle measurement dim>2");
  }

  const Trk::Surface& surf = measurement->associatedSurface();
  if(surf.bounds().type() == Trk::SurfaceBounds::Annulus) {
    const auto* bounds = dynamic_cast<const Trk::AnnulusBounds*>(&surf.bounds());
    if(bounds == nullptr) {
      throw std::runtime_error{"Annulus but not XY"};
    }

    Amg::Vector2D locxy = loc.head<2>();
    
    Acts::ActsMatrix<2, 2> covxy = measurement->localCovariance();

    Amg::Vector3D global = surf.localToGlobal(locxy, Amg::Vector3D{});
    Acts::Vector2 locpc;
    if(auto res = surface.globalToLocal(gctx, global, Acts::Vector3{}); res.ok()) {
      locpc = *res;
    } 
    else {
      throw std::runtime_error{"Global position not on target surface"};
    }


    // use ACTS jacobian math to convert cluster covariance from cartesian to polar
    auto planeSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(surf.transform());
    Acts::BoundVector locxypar;
    locxypar.head<2>() = locxy;
    locxypar[2] =0;
    locxypar[3] = M_PI_2;
    locxypar[4] = 1;
    locxypar[5] = 1;
    auto boundToFree = planeSurface->boundToFreeJacobian(gctx, locxypar);
    Acts::ActsSymMatrix<2> xyToXyzJac = boundToFree.topLeftCorner<2, 2>();

    Acts::BoundVector locpcpar;
    locpcpar.head<2>() = locpc;
    locpcpar[2] = 0;
    locpcpar[3] = M_PI_2;
    locpcpar[4] = 1;
    locpcpar[5] = 1;

    boundToFree = surface.boundToFreeJacobian(gctx, locpcpar);
    Acts::ActsSymMatrix<2> pcToXyzJac = boundToFree.topLeftCorner<2, 2>();
    Acts::ActsSymMatrix<2> xyzToPcJac = pcToXyzJac.inverse();

    // convert cluster covariance
    Acts::ActsMatrix<2, 2>  covpc = covxy;
    covpc = xyToXyzJac * covpc * xyToXyzJac.transpose();
    covpc = xyzToPcJac * covpc * xyzToPcJac.transpose();

    if (m_visualDebugOutput) {
      static std::mt19937 gen{42};
      std::normal_distribution<double> normal{0, 1};
      std::uniform_real_distribution<double> uniform{-1, 1};

      Acts::ActsMatrix<2, 2> lltxy = covxy.llt().matrixL();
      Acts::ActsMatrix<2, 2> lltpc = covpc.llt().matrixL();

      for (size_t i=0;i<1e4;i++) {
        std::cout << "ANNULUS COV: ";
        std::cout << surface.geometryId();

        Amg::Vector2D rnd{
          normal(gen),
          normal(gen)
        };

        // XY
        {
          Amg::Vector2D xy = lltxy * rnd + locxy;
          Amg::Vector3D xyz = surf.localToGlobal(xy);
          std::cout << "," << xy.x() << "," << xy.y(); 
          std::cout << "," << xyz.x() << "," << xyz.y() << "," << xyz.z(); 
        }
        // PC
        {
          // Amg::Vector2D xy = lltpc * rnd + loc.head<2>();
          Amg::Vector2D rt = lltpc * rnd + locpc;
          Amg::Vector3D xyz = surface.localToGlobal(gctx, rt, Acts::Vector3{});
          // Amg::Vector3D xyz = surface.transform(gctx).rotation() * Acts::Vector3{rt.x(), rt.y(), 0};

          std::cout << "," << rt.x() << "," << rt.y(); 
          std::cout << "," << xyz.x() << "," << xyz.y() << "," << xyz.z(); 
        }

        std::cout << std::endl;
      }
    }
  }

  return ATLASSourceLink(surface, *measurement, dim, loc, cov);
}

const std::vector<ATLASSourceLink>
ActsATLASConverterTool::ATLASTrackToSourceLink(const Acts::GeometryContext& gctx, const Trk::Track &track) const {
  
  auto hits = track.measurementsOnTrack();   
  auto outliers = track.outliersOnTrack();  

  std::vector<ATLASSourceLink> sourceLinks;
  sourceLinks.reserve(hits->size()+outliers->size());

  for (auto it = hits->begin(); it != hits->end(); ++it){
    sourceLinks.push_back(ATLASMeasurementToSourceLink(gctx, *it));
  }
  for (auto it = outliers->begin(); it != outliers->end(); ++it){
    sourceLinks.push_back(ATLASMeasurementToSourceLink(gctx, *it));
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

std::unique_ptr<const Trk::TrackParameters>
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
      return std::make_unique<const Trk::AtaCone>(actsParameter.get<Acts::eBoundLoc0>(),
                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                   coneSurface,
                                                   cov);
      
      break;
    }
    case Acts::Surface::SurfaceType::Cylinder : {
      const Trk::CylinderSurface &cylSurface = dynamic_cast<const Trk::CylinderSurface &>(ActsSurfaceToATLAS(actsSurface));
      return std::make_unique<const Trk::AtaCylinder>(actsParameter.get<Acts::eBoundLoc0>(),
                                                          actsParameter.get<Acts::eBoundLoc1>(),
                                                          actsParameter.get<Acts::eBoundPhi>(),
                                                          actsParameter.get<Acts::eBoundTheta>(),
                                                          actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                          cylSurface,
                                                          cov);
      break;
    } 
    case Acts::Surface::SurfaceType::Disc : {
      if(const auto* discSurface = dynamic_cast<const Trk::DiscSurface *>(&ActsSurfaceToATLAS(actsSurface)); discSurface != nullptr) {
        return std::make_unique<const Trk::AtaDisc>(actsParameter.get<Acts::eBoundLoc0>(),
                                                     actsParameter.get<Acts::eBoundLoc1>(),
                                                     actsParameter.get<Acts::eBoundPhi>(),
                                                     actsParameter.get<Acts::eBoundTheta>(),
                                                     actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                     *discSurface,
                                                     cov);
      }
      else if(const auto* planeSurface = dynamic_cast<const Trk::PlaneSurface*>(&ActsSurfaceToATLAS(actsSurface)); planeSurface != nullptr) {
        // need to convert to plane position on plane surface (annulus bounds)
        auto helperSurface = Acts::Surface::makeShared<Acts::PlaneSurface>(planeSurface->transform());

        auto boundToFree = actsSurface.boundToFreeJacobian(gctx, actsParameter.parameters());
        
        auto covpc = actsParameter.covariance().value();
        Acts::FreeVector freePars = Acts::detail::transformBoundToFreeParameters(actsSurface, gctx, actsParameter.parameters());
        Acts::FreeMatrix freeCov = boundToFree * covpc * boundToFree.transpose();


        Acts::BoundVector targetPars = Acts::detail::transformFreeToBoundParameters(freePars, *helperSurface, gctx).value();
        Acts::CovarianceCache covCache{freePars, freeCov};
        auto [varNewCov, varNewJac] = Acts::transportCovarianceToBound(gctx, *helperSurface, freePars, covCache);
        auto targetCov = std::get<Acts::BoundSymMatrix>(varNewCov);

        auto pars = std::make_unique<const Trk::AtaPlane>(targetPars[Acts::eBoundLoc0],
                                                     targetPars[Acts::eBoundLoc1],
                                                     targetPars[Acts::eBoundPhi],
                                                     targetPars[Acts::eBoundTheta],
                                                     targetPars[Acts::eBoundQOverP]*1_MeV,
                                                     *planeSurface,
                                                     targetCov.topLeftCorner<5,5>());

        if(m_visualDebugOutput) {
          std::cout << "ANNULUS PAR COV: ";
          std::cout << actsParameter.referenceSurface().geometryId();
          for(unsigned int i = 0;i<5;i++) {
            for(unsigned int j = 0;j<5;j++) {
              std::cout << "," << covpc(i, j);
            }
          }
          for(unsigned int i = 0;i<5;i++) {
            for(unsigned int j = 0;j<5;j++) {
              std::cout << "," << targetCov(i, j);
            }
          }
          std::cout << std::endl;

          static std::mt19937 gen{42};
          std::normal_distribution<double> normal{0, 1};

          Acts::ActsMatrix<2, 2> lltxy = targetCov.topLeftCorner<2, 2>().llt().matrixL();
          Acts::ActsMatrix<2, 2> lltpc = covpc.topLeftCorner<2, 2>().llt().matrixL();

          for (size_t i=0;i<1e4;i++) {
            std::cout << "ANNULUS PAR: ";
            std::cout << actsParameter.referenceSurface().geometryId();

            Acts::ActsVector<2> rnd;
            rnd <<
              normal(gen),
              normal(gen);

            // XY
            {
              Acts::ActsVector<2> xy = lltxy.topLeftCorner<2,2>() * rnd + targetPars.head<2>();
              Amg::Vector3D xyz;
              planeSurface->localToGlobal(Amg::Vector2D{xy.head<2>()}, Amg::Vector3D{}, xyz);
              for(unsigned int i = 0;i<2;i++) {
                std::cout << "," << xy[i];
              }
              std::cout << "," << xyz.x() << "," << xyz.y() << "," << xyz.z(); 
            }
            // PC
            {
              Acts::ActsVector<2> rt = lltpc.topLeftCorner<2,2>() * rnd + actsParameter.parameters().head<2>();
              Amg::Vector3D xyz = actsParameter.referenceSurface().localToGlobal(gctx, Acts::Vector2{rt.head<2>()}, Acts::Vector3{});

              for(unsigned int i = 0;i<2;i++) {
                std::cout << "," << rt[i];
              }
              std::cout << "," << xyz.x() << "," << xyz.y() << "," << xyz.z(); 
            }

            std::cout << std::endl;
          }
        }


        return pars;

      }
      else {
        throw std::runtime_error{"Acts::DiscSurface is not associated with ATLAS disc or plane surface"};
      }
      break;
    } 
    case Acts::Surface::SurfaceType::Perigee : {
      const Trk::PerigeeSurface perSurface(actsSurface.center(gctx));
      return std::make_unique<const Trk::Perigee>(actsParameter.get<Acts::eBoundLoc0>(),
                                                  actsParameter.get<Acts::eBoundLoc1>(),
                                                  actsParameter.get<Acts::eBoundPhi>(),
                                                  actsParameter.get<Acts::eBoundTheta>(),
                                                  actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                  perSurface,
                                                  cov);

      break;
    } 
    case Acts::Surface::SurfaceType::Plane : {
      const Trk::PlaneSurface &plaSurface = dynamic_cast<const Trk::PlaneSurface &>(ActsSurfaceToATLAS(actsSurface));
      return std::make_unique<const Trk::AtaPlane>(actsParameter.get<Acts::eBoundLoc0>(),
                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                   plaSurface,
                                                   cov);
      break;
    } 
    case Acts::Surface::SurfaceType::Straw : {
      const Trk::StraightLineSurface &lineSurface = dynamic_cast<const Trk::StraightLineSurface &>(ActsSurfaceToATLAS(actsSurface));
      return std::make_unique<const Trk::AtaStraightLine>(actsParameter.get<Acts::eBoundLoc0>(),
                                                                   actsParameter.get<Acts::eBoundLoc1>(),
                                                                   actsParameter.get<Acts::eBoundPhi>(),
                                                                   actsParameter.get<Acts::eBoundTheta>(),
                                                                   actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                                   lineSurface,
                                                                   cov);
      break;
    }
    case Acts::Surface::SurfaceType::Curvilinear : {
      return std::make_unique< Trk::CurvilinearParameters>(actsParameter.position(gctx),
                                                                               actsParameter.get<Acts::eBoundPhi>(),
                                                                               actsParameter.get<Acts::eBoundTheta>(),
                                                                               actsParameter.get<Acts::eBoundQOverP>()*1_MeV,
                                                                               cov);
      break;
    }  
    case Acts::Surface::SurfaceType::Other : {
      break;
    }
  }

  throw std::domain_error("Surface type not found");
} 

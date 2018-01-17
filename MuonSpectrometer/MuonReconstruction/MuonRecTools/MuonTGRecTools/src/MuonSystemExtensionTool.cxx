/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSystemExtensionTool.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/DiscSurface.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndexHelpers.h"
#include "MuonDetDescrUtils/MuonChamberLayerDescription.h"

#include "MuonLayerEvent/MuonSystemExtension.h"
#include "MuonLayerEvent/MuonSystemExtensionCollection.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"


namespace Muon {

  MuonSystemExtensionTool::MuonSystemExtensionTool(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
  {
    declareInterface<IMuonSystemExtensionTool>(this);

    declareProperty("Extrapolator",m_extrapolator );
    declareProperty("ParticleCaloExtensionTool",m_caloExtensionTool );    
    declareProperty("MuonIdHelperTool",m_idHelper );    

  }

  MuonSystemExtensionTool::~MuonSystemExtensionTool() { }

  StatusCode MuonSystemExtensionTool::finalize() {
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSystemExtensionTool::initialize() {

    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_caloExtensionTool.retrieve());
    ATH_CHECK(m_extrapolator.retrieve());

    if( !initializeGeometry() ) return StatusCode::FAILURE;

    return StatusCode::SUCCESS;
  }

  bool MuonSystemExtensionTool::initializeGeometry() {

    // initialize reference surfaces hash vectors per region and sector
    m_referenceSurfaces.resize(MuonStationIndex::DetectorRegionIndexMax);
    for( auto& vec : m_referenceSurfaces ) vec.resize(MuonStationIndex::numberOfSectors());

    // first build the barrel, we need different reference planes for all sectors
    ATH_MSG_DEBUG("Initializing barrel ");
    for( unsigned int sector = 1; sector <= 16; ++sector ){

      // get rotation into sector frame
      double sectorPhi = m_sectorMapping.sectorPhi(sector);
      Amg::AngleAxis3D sectorRotation(sectorPhi,Amg::Vector3D(0.,0.,1.));
      if( !initializeGeometryBarrel( sector, sectorRotation ) ) return false;

      if( !initializeGeometryEndcap( sector, MuonStationIndex::EndcapA, sectorRotation ) ) return false;
      if( !initializeGeometryEndcap( sector, MuonStationIndex::EndcapC, sectorRotation ) ) return false;

    }

//    if( !initializeGeometryEndcap( MuonStationIndex::EndcapA ) ) return false;
//    if( !initializeGeometryEndcap( MuonStationIndex::EndcapC ) ) return false;
    
    return true;
  }

  bool MuonSystemExtensionTool::initializeGeometryEndcap( int sector, MuonStationIndex::DetectorRegionIndex regionIndex, const Amg::AngleAxis3D& sectorRotation ) {

    ATH_MSG_DEBUG("Initializing endcap: sector " << sector << " " << MuonStationIndex::regionName(regionIndex) );

    SurfaceVec& surfaces = m_referenceSurfaces[regionIndex][sector-1];
    MuonChamberLayerDescription chamberLayerDescription;

    std::vector< MuonStationIndex::StIndex > layers = { MuonStationIndex::EI, MuonStationIndex::EE,MuonStationIndex::EM,MuonStationIndex::EO };

    for( std::vector< MuonStationIndex::StIndex >::const_iterator stLayer = layers.begin(); stLayer != layers.end() ; ++stLayer ){

      // calculate reference position from radial position of the layer
      MuonStationIndex::LayerIndex layer = MuonStationIndex::toLayerIndex(*stLayer);
      MuonChamberLayerDescriptor layerDescriptor = chamberLayerDescription.getDescriptor( sector, regionIndex, layer );
      Amg::Vector3D globalPosition(0.,0.,layerDescriptor.referencePosition);

      // reference transform + surface
      Amg::Transform3D* trans   = new Amg::Transform3D(sectorRotation); //*Amg::AngleAxis3D(xToZRotation,Amg::Vector3D(0.,1.,0.)
      trans->pretranslate(globalPosition);

      Trk::PlaneSurface* surface = new Trk::PlaneSurface(trans);
      MuonLayerSurface data(MuonLayerSurface::SurfacePtr(surface),sector,regionIndex,layer);
      surfaces.push_back( data );

      // sanity checks
      if( msgLvl(MSG::VERBOSE) ){
        for( unsigned int test=0;test<2;++test ){
          Amg::Vector3D lpos3d = surface->transform().inverse()*globalPosition;
          Amg::Vector2D lpos;
          surface->globalToLocal(globalPosition,globalPosition,lpos);

          ATH_MSG_VERBOSE(" sector " << sector
                          << " layer " << MuonStationIndex::layerName(layer)
                          << " hit x " << globalPosition.x()  << " hit z " << globalPosition.z()
                          << " lpos3d " << lpos3d.x() << " " << lpos3d.y() << " " << lpos3d.z()
                          << " lpos " << lpos[Trk::loc1] << " " << lpos[Trk::loc2]
                          << " center " << surface->center().z()
                          << " normal: theta " << surface->normal().phi() << " phi " << surface->normal().theta() );
          globalPosition[0] += 100;
        }
      }
    }
    ATH_MSG_VERBOSE(" Total number of surfaces " << surfaces.size() );
    return true;
  }

  bool MuonSystemExtensionTool::initializeGeometryBarrel( int sector, const Amg::AngleAxis3D& sectorRotation ) {

    MuonChamberLayerDescription chamberLayerDescription;

    SurfaceVec& surfaces = m_referenceSurfaces[MuonStationIndex::Barrel][sector-1];
    double xToZRotation = -0.5*M_PI;

    for( unsigned int stationLayer = MuonStationIndex::BI; stationLayer <= MuonStationIndex::BE; ++stationLayer ){

      // skip BEE if in small sectors, not installed
      if( stationLayer == MuonStationIndex::BE && MuonStationIndexHelpers::isSmall(sector) ) continue;

      // calculate reference position from radial position of the laeyr
      MuonStationIndex::LayerIndex layer = MuonStationIndex::toLayerIndex((MuonStationIndex::StIndex)(stationLayer));
      MuonChamberLayerDescriptor layerDescriptor = chamberLayerDescription.getDescriptor( sector, MuonStationIndex::Barrel, layer );
      Amg::Vector3D positionInSector(layerDescriptor.referencePosition,0.,0.);
      Amg::Vector3D globalPosition = sectorRotation*positionInSector;

      //Amg::Vector3D positionInSector2(layerDescriptor.referencePosition+100,0.,0.);
      //Amg::Vector3D globalPosition2 = sectorRotation*positionInSector2;

      // reference transform + surface
      Amg::Transform3D* trans = new Amg::Transform3D(sectorRotation*Amg::AngleAxis3D(xToZRotation,Amg::Vector3D(0.,1.,0.)));
      trans->pretranslate(globalPosition);
      Trk::PlaneSurface* surface = new Trk::PlaneSurface(trans);

      MuonLayerSurface data( MuonLayerSurface::SurfacePtr(surface),sector,MuonStationIndex::Barrel,layer);
      surfaces.push_back( data );

      // sanity checks
      if( msgLvl(MSG::VERBOSE) ){
        Amg::Vector3D lpos3d = surface->transform().inverse()*globalPosition;
        //Amg::Vector3D lpos3d2 = surface->transform().inverse()*globalPosition2;
        Amg::Vector2D lpos;
        surface->globalToLocal(globalPosition,globalPosition,lpos);
        double sectorPhi = m_sectorMapping.sectorPhi(sector);

        ATH_MSG_VERBOSE(" sector " << sector << " layer " << MuonStationIndex::layerName(layer) << " phi " << sectorPhi
                        << " ref theta " << globalPosition.theta() << " phi " << globalPosition.phi() << " r " << globalPosition.perp()
                        << " pos " << globalPosition.x() << " " << globalPosition.y() << " " << globalPosition.z()
                        << " lpos3d " << lpos3d.x() << " " << lpos3d.y() << " " << lpos3d.z() );
        // << " lpos " << lpos[Trk::loc1] << " " << lpos[Trk::loc2]
        // << " ref theta2 " << globalPosition2.theta() << " phi " << globalPosition2.phi() << " r " << globalPosition2.perp()
        // << " lpos3d2 " << lpos3d2.x() << " " << lpos3d2.y() << " " << lpos3d2.z()
        // << " normal: theta " << surface->normal().phi() << " phi " << surface->normal().theta() );
      }
    }
    return true;
  }


  bool MuonSystemExtensionTool::muonSystemExtension( const xAOD::TrackParticle& indetTrackParticle, const MuonSystemExtension*& muonSystemExtention ) const {
    // get calo extension
    const Trk::CaloExtension* caloExtension = 0;
    m_caloExtensionTool->caloExtension( indetTrackParticle, caloExtension );    
    if( !caloExtension || !caloExtension->muonEntryLayerIntersection() ) {
      ATH_MSG_VERBOSE("Failed to get CaloExtension ");
      return false;
    }
    
    // get entry parameters, use it as current parameter for the extrapolation
    const Trk::TrackParameters* currentPars = caloExtension->muonEntryLayerIntersection();
    
    // get reference surfaces
    ATH_MSG_VERBOSE(" getSurfacesForIntersection " << currentPars);
    SurfaceVec surfaces = getSurfacesForIntersection(*currentPars);
    
    // store intersections
    std::vector< MuonSystemExtension::Intersection > intersections;
    
    // garbage collection
    std::vector< std::shared_ptr<const Trk::TrackParameters> > trackParametersVec;

    // loop over reference surfaces
    SurfaceVec::const_iterator it = surfaces.begin();
    SurfaceVec::const_iterator it_end = surfaces.end();
    for( ;it!=it_end;++it ){
      
      // extrapolate to next layer
      const Trk::Surface& surface = *it->surfacePtr;
      if( msgLvl(MSG::VERBOSE) ){
        msg(MSG::VERBOSE) << " startPars: phi "  << currentPars->position().phi() 
                          << " r " << currentPars->position().perp() << " z " << currentPars->position().z() 
                          << " local " << currentPars->parameters()[Trk::locX] << " " << currentPars->parameters()[Trk::locY];
        if( currentPars->covariance() ) msg(MSG::VERBOSE) << " err " << Amg::error(*currentPars->covariance(),Trk::locX) << " " << Amg::error(*currentPars->covariance(),Trk::locY);
        msg(MSG::VERBOSE) << " destination: sector " << it->sector << "  " << MuonStationIndex::regionName(it->regionIndex)
                          << "  " << MuonStationIndex::layerName(it->layerIndex) 
                          << " phi  " << surface.center().phi() << " r " << surface.center().perp() << " z " << surface.center().z() << endmsg;
      }
      const Trk::TrackParameters* exPars = m_extrapolator->extrapolate(*currentPars,surface,Trk::alongMomentum,false,Trk::muon);
      if( !exPars ){
        ATH_MSG_VERBOSE("extrapolation failed, trying next layer ");
        continue;
      }
      // create shared pointer and add to garbage collection
      std::shared_ptr<const Trk::TrackParameters> sharedPtr(exPars);
      trackParametersVec.push_back(sharedPtr);

      if( it->regionIndex != MuonStationIndex::Barrel ){
        // in the endcaps we need to update the sector and check that we are in the correct sector
        double phi = exPars->position().phi();
        std::vector<int> sectors;
        m_sectorMapping.getSectors( phi, sectors );
        
        // loop over sectors, if the surface sector and the sector in the loop are either both large or small, pick the sector
        int sector = -1;
        for( auto sec : sectors ){
          if( it->sector%2 == sec%2 ){
            sector = sec;
            break;
          }
        }
        ATH_MSG_DEBUG(" sector " << it->sector << " selected " << sector << " nsectors " << sectors);
        // only select if we found a matching sector in the set
        if( sector != -1 ){
          MuonSystemExtension::Intersection intersection(sharedPtr,*it);
          intersection.layerSurface.sector = sector;
          intersections.push_back( intersection );
        }
      }else{
        MuonSystemExtension::Intersection intersection(sharedPtr,*it);
        intersections.push_back( intersection );
      }
      currentPars = exPars;
    }
    
    ATH_MSG_DEBUG(" completed extrapolation: destinations " << surfaces.size() << " intersections " << intersections.size() );

    if( intersections.empty() ) return false;

     MuonSystemExtension* theExtension = new MuonSystemExtension( std::unique_ptr<const Trk::TrackParameters>(caloExtension->muonEntryLayerIntersection()->clone()), 
                                                   std::move(intersections) );
    
    muonSystemExtention = theExtension;
    return true;
  }
  
  MuonSystemExtensionTool::SurfaceVec MuonSystemExtensionTool::getSurfacesForIntersection( const Trk::TrackParameters& muonEntryPars ) const {
    
    // if in endcaps pick endcap surfaces
    double eta = muonEntryPars.position().eta();
    MuonStationIndex::DetectorRegionIndex regionIndex = MuonStationIndex::Barrel;
    if( eta < -1.05 ) regionIndex = MuonStationIndex::EndcapC;
    if( eta > 1.05 )  regionIndex = MuonStationIndex::EndcapA;

    // in barrel pick primary sector
    double phi = muonEntryPars.position().phi();
    std::vector<int> sectors;
    m_sectorMapping.getSectors( phi, sectors );
    SurfaceVec surfaces;
    for( auto sector : sectors ){
    	surfaces.insert(surfaces.end(),m_referenceSurfaces[regionIndex][sector-1].begin(),m_referenceSurfaces[regionIndex][sector-1].end());
    }
    auto sortFunction = (regionIndex == MuonStationIndex::Barrel) ?
        [](const MuonLayerSurface& s1, const MuonLayerSurface& s2){return s1.surfacePtr->center().perp() < s2.surfacePtr->center().perp();} :
        [](const MuonLayerSurface& s1, const MuonLayerSurface& s2){return std::abs(s1.surfacePtr->center().z()) < std::abs(s2.surfacePtr->center().z());};

    std::stable_sort(surfaces.begin(),surfaces.end(),sortFunction);
    return surfaces;
  }

}
 

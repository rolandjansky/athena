/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TileVolumeBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Tile
#include "TileTrackingGeometry/TileVolumeBuilder.h"
// Tile
#include "TileDetDescr/TileDetDescrManager.h"
// Calo
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
// GeoModel
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/Units.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
//#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/AlignableTrackingVolume.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/make_unique.h"

using Gaudi::Units::mm;
using CxxUtils::make_unique;

// constructor
Tile::TileVolumeBuilder::TileVolumeBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_tileMgr(0),
  m_tileMgrLocation("Tile"),
  m_calo_dd(0),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_trackingVolumeCreator("Trk::CylinderVolumeCreator/TrackingVolumeCreator"),
  m_tileBarrelEnvelope(25.*mm),
  m_useCaloSurfBuilder(true),
  m_tileBarrelLayersPerSampling(1),
  m_surfBuilder("CaloSurfaceBuilder"),
  m_forceSymmetry(true)

{
  declareInterface<Trk::ITrackingVolumeBuilder>(this);
  // declare the properties via Python
  declareProperty("TileDetManagerLocation",                 m_tileMgrLocation);
  // layers and general setup
  declareProperty("BarrelEnvelopeCover",                    m_tileBarrelEnvelope);
  declareProperty("ForceVolumeSymmetry",                    m_forceSymmetry);
  // helper tools 
  declareProperty("TrackingVolumeHelper",                   m_trackingVolumeHelper);    
  declareProperty("TrackingVolumeCreator",                  m_trackingVolumeCreator);  
  declareProperty("UseCaloSurfBuilder",                     m_useCaloSurfBuilder);
  declareProperty("BarrelLayersPerSampling",                m_tileBarrelLayersPerSampling);
  declareProperty("CaloSurfaceBuilder",                     m_surfBuilder);
}

// destructor
Tile::TileVolumeBuilder::~ TileVolumeBuilder()
{}


// Athena standard methods
// initialize
StatusCode Tile::TileVolumeBuilder::initialize()
{
  // Get the Calo geometry
  StatusCode status = detStore()->retrieve(m_calo_dd);
  if(status.isFailure()) return status;
  
  // get Tile Detector Description Manager
  if (detStore()->retrieve(m_tileMgr, m_tileMgrLocation).isFailure()){  
    ATH_MSG_FATAL( "Could not get TileDetDescrManager! Tile TrackingVolumes will not be built" );
    return StatusCode::FAILURE;
  }
  
  // Retrieve the tracking volume helper   -------------------------------------------------    
  if (m_trackingVolumeHelper.retrieve().isFailure())
    {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << m_trackingVolumeHelper );
      return StatusCode::FAILURE;
    } else
    ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeHelper );

    // Retrieve the second volume creator
    if (m_trackingVolumeCreator.retrieve().isFailure()){
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeCreator );
        return StatusCode::FAILURE;
    } else
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeCreator );
    
  if(m_surfBuilder.retrieve().isFailure())
    {
      ATH_MSG_FATAL(  "Failed to retrieve tool " << m_surfBuilder );
      return StatusCode::FAILURE;
    } else
    ATH_MSG_INFO( "Retrieved tool " << m_surfBuilder );

  ATH_MSG_INFO( " initialize() successful" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode Tile::TileVolumeBuilder::finalize()
{
  ATH_MSG_INFO( "finalize() successful" );
  return StatusCode::SUCCESS;
}

const std::vector<const Trk::TrackingVolume*>* Tile::TileVolumeBuilder::trackingVolumes() const
{
  // the return vector
  std::vector<const Trk::TrackingVolume*>* tileTrackingVolumes = new std::vector<const Trk::TrackingVolume*>;
  // the converter helpers
  Trk::GeoShapeConverter    geoShapeToVolumeBounds;
  Trk::GeoMaterialConverter geoMaterialToMaterialProperties;
  // dummy material 
  Trk::Material tileMaterial;

  // dimensions
  double tileZ = 0.;
  double gapZ  = 0.;
  // ITC hardcoded : simple shortcut (needs resizing anyway )
  double plug1Z  = 3405.;
  double plug2Z  = 3512.075;
  double plug1R  = 3440.;
  double plug2R  = 2959.;
  double plug1hZ  = 154.5;
  double plug2hZ  = 47.425;

  // The Volumes To be Created ( by parsing geoModel ) ===========
  const Trk::TrackingVolume* tileBarrel                              = 0;
  const Trk::TrackingVolume* tileGirder                              = 0;
  
  const Trk::TrackingVolume* tilePositiveExtendedBarrel              = 0;
  const Trk::TrackingVolume* tileNegativeExtendedBarrel              = 0;

  // The Volumes To be Created ( Gap Volumes ) ====================
  const Trk::TrackingVolume* tileBarrelPositiveFingerGap             = 0;
  const Trk::TrackingVolume* tileBarrelNegativeFingerGap             = 0;
  const Trk::TrackingVolume* tilePositiveFingerGap                   = 0;
  const Trk::TrackingVolume* tileNegativeFingerGap                   = 0;

  // The Bounds To be Assigned (for resizing) ==================== 
  Trk::CylinderVolumeBounds* tileBarrelBounds                        = 0;
  std::unique_ptr<Trk::CylinderVolumeBounds> tileBarrelGirderBounds;

  Trk::CylinderVolumeBounds  tilePositiveExtendedBarrelBounds;
  Trk::CylinderVolumeBounds* itcPlug1Bounds                          = 0;
  Trk::CylinderVolumeBounds* itcPlug2Bounds                          = 0;
  Trk::CylinderVolumeBounds* gapBounds                               = 0;

  // dummy objects
  const Trk::LayerArray* dummyLayers = 0;
  const Trk::TrackingVolumeArray* dummyVolumes = 0;

  // load layer surfaces 
  std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > entrySurf = m_surfBuilder->entrySurfaces();
  std::vector<std::pair<const Trk::Surface*,const Trk::Surface*> > exitSurf  = m_surfBuilder->exitSurfaces();
  
  // averaged material properties 
  const Trk::Material* barrelProperties = new Trk::Material(22.7, 212., 45.8, 21.4, 0.0062);
  const Trk::Material* extendedBarrelProperties = new Trk::Material(22.7, 210., 45.8, 21.4, 0.0062);
  // material properties with layer encoding - to be defined later 
  const Trk::BinnedMaterial* barrelMaterialBinned = 0;
  const Trk::BinnedMaterial* extendedMaterialBinned = 0;

  Trk::Material girderProperties = Trk::Material(28.6, 272.5, 40.4, 19., 0.0049);
  Trk::Material extendedGirderProperties = Trk::Material(27.9, 266.4, 41., 19.2, 0.005);
  Trk::Material fingerProperties = Trk::Material(46., 426.6, 34.2, 16.2, 0.0032);
             
  unsigned int numTreeTops =  m_tileMgr->getNumTreeTops();
  ATH_MSG_DEBUG( "Retrieved " << numTreeTops << " tree tops from the TileDetDescrManager. " );

  // layer material can be adjusted here
  std::vector<const Trk::IdentifiedMaterial*> matTB; 
  int baseID = Trk::GeometrySignature(Trk::Calo)*1000 + 12;
  matTB.push_back(new std::pair<const Trk::Material*,int>(barrelProperties,0));
  matTB.push_back(new std::pair<const Trk::Material*,int>(barrelProperties,baseID));
  matTB.push_back(new std::pair<const Trk::Material*,int>(barrelProperties,baseID+1));
  matTB.push_back(new std::pair<const Trk::Material*,int>(barrelProperties,baseID+2));
  
  // material index 
  std::vector<size_t> ltb{0,1,2,3};
  
  // layer material can be adjusted here
  std::vector<const Trk::IdentifiedMaterial*> matETB; 
  baseID = Trk::GeometrySignature(Trk::Calo)*1000 + 18;
  matETB.push_back(new std::pair<const Trk::Material*,int>(extendedBarrelProperties,0));
  matETB.push_back(new std::pair<const Trk::Material*,int>(extendedBarrelProperties,baseID));
  matETB.push_back(new std::pair<const Trk::Material*,int>(extendedBarrelProperties,baseID+1));
  matETB.push_back(new std::pair<const Trk::Material*,int>(extendedBarrelProperties,baseID+2));

  // layer material can be adjusted here
  //Trk::MaterialProperties barrelFingerGapProperties = Trk::MaterialProperties(1., 130./0.35, 0.003*pow(0.35,3),30.);
  Trk::Material barrelFingerGapProperties(353., 2254., 20.7, 10., 0.00057);

  //Trk::MaterialProperties fingerGapProperties = Trk::MaterialProperties(1., 88./0.1, 0.003*pow(0.1,3),30.);
  Trk::Material fingerGapProperties(552., 3925., 16.4, 8.1, 0.00034);

  for (unsigned int itreetop = 0; itreetop<numTreeTops; ++itreetop){
    PVConstLink currentVPhysVolLink   = m_tileMgr->getTreeTop(itreetop);
    
    // const GeoVPhysVol* top = &(*(m_tileMgr->getTreeTop(itreetop)));
    //GeoVolumeCursor vol (top);
    //while (!vol.atEnd()) {
    //  const GeoVPhysVol* cv = &(*(vol.getVolume()));
    //  printInfo(cv);
    //} 
    
    const GeoLogVol* currentLogVol = currentVPhysVolLink->getLogVol();
    
    unsigned int currentChilds = currentVPhysVolLink->getNChildVols();
    
    ATH_MSG_DEBUG( "Processing " << currentLogVol->getName() << "... has "
		   << currentChilds << " childs." );
    const GeoShape* currentShape = currentLogVol->getShape();
    ATH_MSG_VERBOSE( "  -> the shape is " << currentShape->type() );
    std::vector<double> zboundaries;
    
    ATH_MSG_DEBUG( "Start looping over childs. " );
    // the loop over the childs
    for (unsigned int ichild = 0; ichild < currentChilds; ++ichild){
      
      PVConstLink currentChildLink = currentVPhysVolLink->getChildVol(ichild);
      //printInfo(currentChildLink);
      const GeoLogVol* childLogVol = currentChildLink->getLogVol();
      const GeoShape* childShape = childLogVol->getShape();
      ATH_MSG_VERBOSE( "  Child: " << childLogVol->getName() << " has shape  " << childShape->type() );
      
      const GeoTubs* currentTubs  = dynamic_cast<const GeoTubs*>(childShape);
      Trk::CylinderVolumeBounds* childCylVolBounds = currentTubs ? geoShapeToVolumeBounds.convert(currentTubs) : 0;
      // get the transform
      GeoTrf::Transform3D childTransform = currentVPhysVolLink->getXToChildVol(ichild);
      double childZposition = childTransform.translation().z();
      
      if (childCylVolBounds){
	// screen output    
	ATH_MSG_VERBOSE( "  ---> CylinderVolumeBounds created as: " );
	ATH_MSG_VERBOSE( "  ---> Position in z: " << childTransform.translation().z() );
	ATH_MSG_VERBOSE( *childCylVolBounds );
	
	  // retrieve split radius from the TileBar2 exit surface
	double depth = exitSurf[CaloCell_ID::TileBar2].first->bounds().r();
	
	switch (itreetop){
	  
	case 0 : { // Tile Barrel case ==================================================================
	  
	  // the centered one is the TileBarrel
	  if ( fabs(childZposition)< 100.*mm ){
	    //depth is where we split childCylVolBounds
	    tileBarrelBounds = new Trk::CylinderVolumeBounds(childCylVolBounds->innerRadius(),depth,childCylVolBounds->halflengthZ());
	    
	    // assign Bounds
	    tileBarrelGirderBounds = make_unique<Trk::CylinderVolumeBounds>(depth,childCylVolBounds->outerRadius(),childCylVolBounds->halflengthZ());
	    
	    Amg::Transform3D* align=0;     //  no alignment given yet       
	    
	    // construct bin utilities 
	    std::vector<float> steps;
	    steps.push_back(childCylVolBounds->innerRadius());
	    steps.push_back(entrySurf[CaloCell_ID::TileBar0].first->bounds().r());
	    steps.push_back(entrySurf[CaloCell_ID::TileBar1].first->bounds().r());
	    steps.push_back(entrySurf[CaloCell_ID::TileBar2].first->bounds().r());
	    steps.push_back(depth);
	    Trk::BinUtility* rBU = new Trk::BinUtility(steps, Trk::open, Trk::binR);
	    
	    barrelMaterialBinned = new Trk::BinnedMaterial(barrelProperties,rBU,ltb,matTB);
            
	    tileBarrel = new Trk::AlignableTrackingVolume(0,align,                          
							  tileBarrelBounds,
							  barrelMaterialBinned,
							  12,
							  "Calo::Detectors::Tile::Barrel");	      
	  }
	} break;
	  
	default : { // Tile Extended Barrel ==================================================================
	  
	  std::string  volumeName;
	  std::string  girderName;
	  std::vector<double> girderLayerRadius;
	  std::vector<double> layerRadius;
	  std::vector<double> layerEnvelope; 
	  Trk::CylinderVolumeBounds* tileExtendedBounds = 0;
          bool tileExtendedBoundsUsed = false;
	  std::unique_ptr<Trk::CylinderVolumeBounds> gapVolBounds;
	  
	  // prepare for the Extended Barrel
	  if (childCylVolBounds->halflengthZ() > 1000.){
              volumeName = childZposition > 0. ? 
                "Calo::Detectors::Tile::PositiveExtendedBarrel" : "Calo::Detectors::Tile::NegativeExtendedBarrel";
	      
              //depth is where we split childCylVolBounds
              tileExtendedBounds = new Trk::CylinderVolumeBounds(childCylVolBounds->innerRadius(),depth,childCylVolBounds->halflengthZ());
	      
	  } else if (childCylVolBounds->halflengthZ() > 100.){ 
	    // prepare for the EBarrel Finger : (ST) : merge with combined girder
	    //tileFingerBounds = new Trk::CylinderVolumeBounds(depth,childCylVolBounds->outerRadius(),childCylVolBounds->halflengthZ());
	    
	  } else if ( childLogVol->getName()=="Gap" && !gapBounds ) {
	    
	    gapVolBounds = make_unique<Trk::CylinderVolumeBounds>(childCylVolBounds->innerRadius(),childCylVolBounds->outerRadius(),
						      childCylVolBounds->halflengthZ());
	    
	    gapZ = fabs(childZposition); 
	    
	  } else { 
	    ATH_MSG_VERBOSE(  "  ---> This Volume is not gonna built !" );
	    break;
	  }

	  // only account for misalignment along z
	  Amg::Vector3D childPosition(0.,0.,childZposition);
	  Trk::TrackingVolume* tileExtendedTrackingVolume = 0;
	  Amg::Transform3D* align=0;     //  no alignment given yet       

	  if(m_useCaloSurfBuilder  && childCylVolBounds->halflengthZ() > 1000.){
	    
	    // get some output 
	    ATH_MSG_VERBOSE ( "[C1] Creating TrackingVolume '" << volumeName  );
	    
	    // construct bin utilities 
	    std::vector<float> steps;
	    steps.push_back(tileExtendedBounds->innerRadius());
	    steps.push_back(entrySurf[CaloCell_ID::TileExt0].first->bounds().r());
	    steps.push_back(entrySurf[CaloCell_ID::TileExt1].first->bounds().r());
	    steps.push_back(entrySurf[CaloCell_ID::TileExt2].first->bounds().r());
	    steps.push_back(tileExtendedBounds->outerRadius());
	    Trk::BinUtility* eBU = new Trk::BinUtility(steps, Trk::open, Trk::binR);
	    
	    extendedMaterialBinned = new Trk::BinnedMaterial(extendedBarrelProperties,eBU,ltb,matETB);
	    
	    tileExtendedTrackingVolume = new Trk::AlignableTrackingVolume(new Amg::Transform3D(Amg::Translation3D(childPosition)),
									  align,
									  tileExtendedBounds,
									  extendedMaterialBinned,
									  18,
									  volumeName);
            tileExtendedBoundsUsed = true;
	    
	  } else {          
	    if ( gapVolBounds ) {
	      gapBounds = gapVolBounds.release(); 

	    } else if (tileExtendedBounds) {
	      
	      // construct bin utilities 
	      std::vector<float> steps;
	      steps.push_back(tileExtendedBounds->innerRadius());
	      steps.push_back(entrySurf[CaloCell_ID::TileExt0].first->bounds().r());
	      steps.push_back(entrySurf[CaloCell_ID::TileExt1].first->bounds().r());
	      steps.push_back(entrySurf[CaloCell_ID::TileExt2].first->bounds().r());
	      steps.push_back(tileExtendedBounds->outerRadius());
	      Trk::BinUtility* eBU = new Trk::BinUtility(steps, Trk::open, Trk::binR);
	      
	      extendedMaterialBinned = new Trk::BinnedMaterial(extendedBarrelProperties,eBU,ltb,matETB);
	      
	      tileExtendedTrackingVolume = new Trk::AlignableTrackingVolume(new Amg::Transform3D(Amg::Translation3D(childPosition)),
									    align,
									    tileExtendedBounds,
									    extendedMaterialBinned,
									    18,
									    volumeName);
              tileExtendedBoundsUsed = true;
	    }
	  }
	  // and assign it to the right one      
	  if (childCylVolBounds->halflengthZ() > 1000.){
	    if (childZposition > 0.) {
	      tilePositiveExtendedBarrel             = tileExtendedTrackingVolume;
	      tilePositiveExtendedBarrelBounds       = *tileExtendedBounds;
              // tileExtendedBoundsUsed = true; - this line is not needed, we copy everything from *tileExtendedBounds to tilePositiveExtendedBarrelBounds
	    } else { 
	      tileNegativeExtendedBarrel             = tileExtendedTrackingVolume;
	    }
	  } else if (childCylVolBounds->halflengthZ() > 100.) {
	    tileZ = fabs(childZposition)+childCylVolBounds->halflengthZ();
	  }
          if (!tileExtendedBoundsUsed)
            delete tileExtendedBounds;
	} break;
	} // end of switch
      } // end of ? cylVolBounds
      delete childCylVolBounds;
    } //end of ichild loop
  } // end of treetop loop

  if (!gapBounds) std::abort();
  if (!tileBarrelGirderBounds) std::abort();
  if (!tilePositiveExtendedBarrel) std::abort();
  if (!tileNegativeExtendedBarrel) std::abort();
  
  ATH_MSG_DEBUG( "TileDetDescrManager parsed successfully! " );

  // combined girder volume
  {
    auto tileGirderBounds = CxxUtils::make_unique<Trk::CylinderVolumeBounds>
      (tileBarrelGirderBounds->innerRadius(),
       tileBarrelGirderBounds->outerRadius(),
       tileZ);

    tileGirder =new Trk::TrackingVolume(0,                     
                                        tileGirderBounds.release(),
                                        girderProperties,
                                        dummyLayers, dummyVolumes,
                                        "Calo::Girder::TileCombined");
  }

  // build the gap volumes ( crack done by CaloTG )
  double tileExtZ = tilePositiveExtendedBarrel->center().z()-tilePositiveExtendedBarrelBounds.halflengthZ();

  // binned material for ITC : 
  std::vector<const Trk::IdentifiedMaterial*> matITC;
  // layer material can be adjusted here
  baseID = Trk::GeometrySignature(Trk::Calo)*1000;
  matITC.push_back(new Trk::IdentifiedMaterial(barrelProperties,baseID+15));
  matITC.push_back(new Trk::IdentifiedMaterial(barrelProperties,baseID+16));
  matITC.push_back(new Trk::IdentifiedMaterial(barrelProperties,baseID+17));

  // ITCPlug1
  double p1Z = 0.5*(plug1Z-plug1hZ+tileExtZ);
  double hp1Z = 0.5*(tileExtZ-plug1Z+plug1hZ);
  itcPlug1Bounds = new Trk::CylinderVolumeBounds(plug1R,
						 tileBarrelBounds->outerRadius(),
						 hp1Z);

  Amg::Vector3D itcP1Pos(0.,0.,p1Z);
  Amg::Vector3D itcP1Neg(0.,0.,-p1Z);
  Amg::Transform3D* itcP1PosTransform = new Amg::Transform3D(Amg::Translation3D(itcP1Pos));
  Amg::Transform3D* itcP1NegTransform = new Amg::Transform3D(Amg::Translation3D(itcP1Neg));


  std::vector<size_t> dummylay(1,1);
  std::vector<float> bpsteps{float(plug1R), float(tileBarrelBounds->outerRadius())};
  Trk::BinUtility* rBU = new Trk::BinUtility(bpsteps, Trk::open, Trk::binR);
  Trk::BinUtility* rBUc = rBU->clone();
  const Trk::BinnedMaterial* plug1MatPos = new Trk::BinnedMaterial(barrelProperties,rBU,dummylay,matITC);
  const Trk::BinnedMaterial* plug1MatNeg = new Trk::BinnedMaterial(barrelProperties,rBUc,dummylay,matITC);

  Amg::Transform3D* align=0;      

  Trk::AlignableTrackingVolume* itcPlug1Pos = new Trk::AlignableTrackingVolume(itcP1PosTransform, align,
									       itcPlug1Bounds,
									       plug1MatPos,
									       16,
									       "Calo::Detectors::Tile::ITCPlug1Pos");
      
  Trk::AlignableTrackingVolume* itcPlug1Neg = new Trk::AlignableTrackingVolume(itcP1NegTransform, align,
									       itcPlug1Bounds->clone(),
									       plug1MatNeg,
									       16,
									       "Calo::Detectors::Tile::ITCPlug1Neg");

  //std::cout <<"ITC Plug1 ok:"<< std::endl;

  // ITCPlug2
  double p2Z = 0.5*(plug2Z-plug2hZ+tileExtZ);
  double hp2Z = 0.5*(tileExtZ-plug2Z+plug2hZ);
  itcPlug2Bounds = new Trk::CylinderVolumeBounds(plug2R,
						 plug1R,
						 hp2Z);

  Amg::Vector3D itcP2Pos(0.,0.,p2Z);
  Amg::Vector3D itcP2Neg(0.,0.,-p2Z);
  Amg::Transform3D* itcP2PosTransform = new Amg::Transform3D(Amg::Translation3D(itcP2Pos));
  Amg::Transform3D* itcP2NegTransform = new Amg::Transform3D(Amg::Translation3D(itcP2Neg));

  std::vector<size_t> p2lay(1,0);
  std::vector<float> p2steps{float(plug2R), float(plug1R)};
  Trk::BinUtility* p2BU = new Trk::BinUtility(p2steps, Trk::open, Trk::binR);
  Trk::BinUtility* p2BUc = p2BU->clone();
  const Trk::BinnedMaterial* plug2MatPos = new Trk::BinnedMaterial(barrelProperties,p2BU,p2lay,matITC);
  const Trk::BinnedMaterial* plug2MatNeg = new Trk::BinnedMaterial(barrelProperties,p2BUc,p2lay,matITC);
      
  Trk::AlignableTrackingVolume* itcPlug2Pos = new Trk::AlignableTrackingVolume(itcP2PosTransform, align,
									       itcPlug2Bounds,
									       plug2MatPos,
									       15,
									       "Calo::Detectors::Tile::ITCPlug2Pos");
      
  Trk::AlignableTrackingVolume* itcPlug2Neg = new Trk::AlignableTrackingVolume(itcP2NegTransform, align,
									       itcPlug2Bounds->clone(),
									       plug2MatNeg,
									       15,
									       "Calo::Detectors::Tile::ITCPlug2Neg");

  //std::cout <<"ITC Plug2 ok:"<< std::endl;

  // Gap
  float gapi = 0.5*(gapZ-gapBounds->halflengthZ()+tileExtZ);
  double hgZ = 0.5*(tileExtZ-gapZ+gapBounds->halflengthZ());
  delete gapBounds;
  gapBounds = new Trk::CylinderVolumeBounds(tileBarrelBounds->innerRadius(),
					    plug2R,
					    hgZ);

  Amg::Vector3D gPos(0.,0.,gapi);
  Amg::Vector3D gNeg(0.,0.,-gapi);
  Amg::Transform3D* gapPosTransform = new Amg::Transform3D(Amg::Translation3D(gPos));
  Amg::Transform3D* gapNegTransform = new Amg::Transform3D(Amg::Translation3D(gNeg));

  std::vector<size_t> glay(1,2);
  std::vector<float> gsteps{float(gapi-gapBounds->halflengthZ()), float(gapi+gapBounds->halflengthZ())};
  Trk::BinUtility* gp = new Trk::BinUtility(gsteps, Trk::open, Trk::binZ);
  const Trk::BinnedMaterial* gpMat = new Trk::BinnedMaterial(barrelProperties,gp,glay,matITC);
      
  Trk::AlignableTrackingVolume* gapPos = new Trk::AlignableTrackingVolume(gapPosTransform, align,
									  gapBounds,
									  gpMat,
									  17,
									  "Calo::Detectors::Tile::GapPos");

  std::vector<float> nsteps{float(-gapi-gapBounds->halflengthZ()), float(-gapi+gapBounds->halflengthZ())};
  Trk::BinUtility* gn = new Trk::BinUtility(nsteps, Trk::open, Trk::binZ);
  const Trk::BinnedMaterial* gnMat = new Trk::BinnedMaterial(barrelProperties,gn,glay,matITC);
      
  Trk::AlignableTrackingVolume* gapNeg = new Trk::AlignableTrackingVolume(gapNegTransform, align,
									  gapBounds->clone(),
									  gnMat,
									  15,
									  "Calo::Detectors::Tile::GapNeg");

  // add buffer volumes and glue together gap sections
  float p1i = itcPlug1Pos->center().z()-itcPlug1Bounds->halflengthZ();
  float p2i = itcPlug2Pos->center().z()-itcPlug2Bounds->halflengthZ();
  float gi  = gapPos->center().z()-gapBounds->halflengthZ();
  float zgBuff = 0.5*(p1i+gi);
  float hgBuff = 0.5*(gi-p1i);
  Amg::Vector3D gBuffPos(0.,0.,zgBuff);
  Amg::Vector3D gBuffNeg(0.,0.,-zgBuff);
  Amg::Transform3D* gBuffPosTransform = new Amg::Transform3D(Amg::Translation3D(gBuffPos));
  Amg::Transform3D* gBuffNegTransform = new Amg::Transform3D(Amg::Translation3D(gBuffNeg));

  Trk::CylinderVolumeBounds* gapBuffBounds = new Trk::CylinderVolumeBounds(gapBounds->innerRadius(),
									   gapBounds->outerRadius(),
                                                                           hgBuff);               

      
  const Trk::TrackingVolume*  gBufferPos = new Trk::TrackingVolume(gBuffPosTransform,
								   gapBuffBounds,
								   fingerProperties,
								   dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::Tile::GapBufferPos");

      
  const Trk::TrackingVolume*  gBufferNeg = new Trk::TrackingVolume(gBuffNegTransform,
								   gapBuffBounds->clone(),
								   fingerProperties,
								   dummyLayers, dummyVolumes,
								   "Calo::GapVolumes::Tile::GapBufferNeg");

  const Trk::TrackingVolume* positiveGapSector = 0;
  if (gBufferPos) {
    std::vector<const Trk::TrackingVolume*> volsPosGap;
     volsPosGap.push_back(gBufferPos);
     volsPosGap.push_back(gapPos);
     positiveGapSector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsPosGap,
        tileMaterial,
        "Calo::Container::PositiveGap");
  } 
  const Trk::TrackingVolume* negativeGapSector = 0;
  if (gBufferNeg) {
    std::vector<const Trk::TrackingVolume*> volsNegGap;
     volsNegGap.push_back(gapNeg);
     volsNegGap.push_back(gBufferNeg);
     negativeGapSector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsNegGap,
        tileMaterial,
        "Calo::Container::NegativeGap");
  } 

  // plug2 sector
  float z2Buff = 0.5*(p1i+p2i);
  float h2Buff = 0.5*(p2i-p1i);
  Amg::Vector3D p2BuffPos(0.,0.,z2Buff);
  Amg::Vector3D p2BuffNeg(0.,0.,-z2Buff);
  Amg::Transform3D* p2BuffPosTransform = new Amg::Transform3D(Amg::Translation3D(p2BuffPos));
  Amg::Transform3D* p2BuffNegTransform = new Amg::Transform3D(Amg::Translation3D(p2BuffNeg));

  Trk::CylinderVolumeBounds* p2BuffBounds = new Trk::CylinderVolumeBounds(itcPlug2Bounds->innerRadius(),
									  itcPlug2Bounds->outerRadius(),
                                                                          h2Buff);               

      
  const Trk::TrackingVolume*  p2BufferPos = new Trk::TrackingVolume(p2BuffPosTransform,
								    p2BuffBounds,
								    fingerProperties,
								    dummyLayers, dummyVolumes,
								    "Calo::GapVolumes::Tile::Plug2BufferPos");

      
  const Trk::TrackingVolume*  p2BufferNeg = new Trk::TrackingVolume(p2BuffNegTransform,
								    p2BuffBounds->clone(),
								    fingerProperties,
								    dummyLayers, dummyVolumes,
								    "Calo::GapVolumes::Tile::Plug2BufferNeg");

  const Trk::TrackingVolume* positiveP2Sector = 0;
  if (p2BufferPos) {
    std::vector<const Trk::TrackingVolume*> volsPosP2;
     volsPosP2.push_back(p2BufferPos);
     volsPosP2.push_back(itcPlug2Pos);
     positiveP2Sector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsPosP2,
        tileMaterial,
        "Calo::Container::PositiveP2");
  } 
  const Trk::TrackingVolume* negativeP2Sector = 0;
  if (itcPlug2Neg) {
    std::vector<const Trk::TrackingVolume*> volsNegP2;
     volsNegP2.push_back(itcPlug2Neg);
     volsNegP2.push_back(p2BufferNeg);
     negativeP2Sector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsNegP2,
        tileMaterial,
        "Calo::Container::NegativeP2");
  } 

  // glue ITC sector radially
  const Trk::TrackingVolume* positiveITCSector = 0;
  if (positiveGapSector && positiveP2Sector) {
    std::vector<const Trk::TrackingVolume*> volsITCPos;
     volsITCPos.push_back(positiveGapSector);
     volsITCPos.push_back(positiveP2Sector);
     volsITCPos.push_back(itcPlug1Pos);
     positiveITCSector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsITCPos,
        tileMaterial,
        "Calo::Container::ITCPos");
  } 
  const Trk::TrackingVolume* negativeITCSector = 0;
  if (negativeGapSector && negativeP2Sector) {
    std::vector<const Trk::TrackingVolume*> volsITCNeg;
     volsITCNeg.push_back(negativeGapSector);
     volsITCNeg.push_back(negativeP2Sector);
     volsITCNeg.push_back(itcPlug1Neg);
     negativeITCSector = m_trackingVolumeCreator->createContainerTrackingVolume
       (volsITCNeg,
        tileMaterial,
        "Calo::Container::ITCNeg");
  } 

  ATH_MSG_DEBUG( "Gap volumes (ITC sector) built " );
           
  // ------------------------------ BARREL SECTION COMPLETION --------------------------------------------------
  
  // the Finger Gap Volumes to be constructed
  double rMin = tileBarrelBounds->innerRadius();
  double rMax = tileBarrelBounds->outerRadius();
   
  double zFG = 0.5*(tileBarrelBounds->halflengthZ()+p1i);
  double hZ = 0.5*(p1i-tileBarrelBounds->halflengthZ());
  Trk::CylinderVolumeBounds* tileBarrelFingerGapBounds = new Trk::CylinderVolumeBounds(rMin,rMax,hZ);

  Amg::Vector3D pBFPos(0.,0.,zFG);
  Amg::Vector3D pBFNeg(0.,0.,-zFG);
  Amg::Transform3D* bfPosTransform = new Amg::Transform3D(Amg::Translation3D(pBFPos));
  Amg::Transform3D* bfNegTransform = new Amg::Transform3D(Amg::Translation3D(pBFNeg));
    
  tileBarrelPositiveFingerGap = new Trk::TrackingVolume(bfPosTransform,
							tileBarrelFingerGapBounds,
							barrelFingerGapProperties,
							dummyLayers, dummyVolumes,
							"Calo::GapVolumes::Tile::BarrelPositiveFingerGap");
  
  tileBarrelNegativeFingerGap =  new Trk::TrackingVolume(bfNegTransform,
							 tileBarrelFingerGapBounds->clone(),
							 barrelFingerGapProperties,
							 dummyLayers, dummyVolumes,
							 "Calo::GapVolumes::Tile::BarrelNegativeFingerGap");
  
 
  // ------------------------------ ENDCAP SECTION COMPLETION --------------------------------------------------
  
  double zBE = tilePositiveExtendedBarrel->center().z()+tilePositiveExtendedBarrelBounds.halflengthZ();
  zFG = 0.5*(tileZ + zBE);
  hZ  = 0.5*(tileZ - zBE);

  Trk::CylinderVolumeBounds* tilePositiveFingerGapBounds = new Trk::CylinderVolumeBounds(  
                                                                         tilePositiveExtendedBarrelBounds.innerRadius(),
                                                                         tilePositiveExtendedBarrelBounds.outerRadius(),
                                                                         hZ);

  Amg::Vector3D pEFPos(0.,0.,zFG);
  Amg::Vector3D pEFNeg(0.,0.,-zFG);
  Amg::Transform3D* efPosTransform = new Amg::Transform3D(Amg::Translation3D(pEFPos));
  Amg::Transform3D* efNegTransform = new Amg::Transform3D(Amg::Translation3D(pEFNeg));
 
  tilePositiveFingerGap = new Trk::TrackingVolume(efPosTransform,
						  tilePositiveFingerGapBounds,
						  fingerGapProperties,
						  dummyLayers, dummyVolumes,
						  "Calo::GapVolumes::Tile::PositiveFingerGap");
     
  tileNegativeFingerGap = new Trk::TrackingVolume(efNegTransform,
						  tilePositiveFingerGapBounds->clone(),
						  fingerGapProperties,
						  dummyLayers, dummyVolumes,
						  "Calo::GapVolumes::Tile::NegativeFingerGap");


  // set the color code for displaying
  tileBarrel->registerColorCode( 4 );
  tilePositiveExtendedBarrel->registerColorCode( 4 );
  tileNegativeExtendedBarrel->registerColorCode( 4 );

  gapPos->registerColorCode( 4 );
  gapNeg->registerColorCode( 4 );
  itcPlug1Pos->registerColorCode( 4 );
  itcPlug1Neg->registerColorCode( 4 );
  itcPlug2Pos->registerColorCode( 4 );
  itcPlug2Neg->registerColorCode( 4 );

  tileGirder->registerColorCode( 12 );

  // finalize Tile : glue locally and return 1 volume 
  ATH_MSG_DEBUG( "Gluing Tile volumes" );

  // glue tile volumes in z 
  std::vector<const Trk::TrackingVolume*> tileVols;
  tileVols.push_back(tileNegativeFingerGap);
  tileVols.push_back(tileNegativeExtendedBarrel);
  tileVols.push_back(negativeITCSector);
  tileVols.push_back(tileBarrelNegativeFingerGap);
  tileVols.push_back(tileBarrel);
  tileVols.push_back(tileBarrelPositiveFingerGap);
  tileVols.push_back(positiveITCSector);
  tileVols.push_back(tilePositiveExtendedBarrel);
  tileVols.push_back(tilePositiveFingerGap);
  const Trk::TrackingVolume* tileCombinedSector = m_trackingVolumeCreator->createContainerTrackingVolume
    (tileVols,
     tileMaterial,
     "Calo::Container::Tile::InnerSector");

  // glue with combined girder
  std::vector<const Trk::TrackingVolume*> tileVolumes;
  tileVolumes.push_back(tileCombinedSector);
  tileVolumes.push_back(tileGirder);
  const Trk::TrackingVolume* tile = m_trackingVolumeCreator->createContainerTrackingVolume
    (tileVolumes,
     tileMaterial,
     "Calo::Container::Tile::Combined");

  ATH_MSG_DEBUG( "Combined Tile built " );

  tileTrackingVolumes->push_back(tile);                  // [0]
  tileTrackingVolumes->push_back(tilePositiveExtendedBarrel);                  // [1]

  if (msgLvl(MSG::INFO)) {
    ATH_MSG_DEBUG( "Checking the existence of all Tracking Volumes:" );
    ATH_MSG_DEBUG( "   -> Tile::Barrel                                       ");
    printCheckResult(msg(MSG::DEBUG), tileBarrel);
    ATH_MSG_DEBUG( "   -> Tile::PositiveExtendedBarrel                       ");
    printCheckResult(msg(MSG::DEBUG), tilePositiveExtendedBarrel);
    ATH_MSG_DEBUG( "   -> Tile::NegativeExtendedBarrel                       ");
    printCheckResult(msg(MSG::DEBUG), tileNegativeExtendedBarrel);
    ATH_MSG_DEBUG( "   -> Tile::BarrelPositiveFingerGap                      ");
    printCheckResult(msg(MSG::DEBUG), tileBarrelPositiveFingerGap);
    ATH_MSG_DEBUG( "   -> Tile::BarrelNegativeFingerGap                      ");
    printCheckResult(msg(MSG::DEBUG), tileBarrelNegativeFingerGap);
    ATH_MSG_DEBUG( "   -> Tile::PositiveFingerGap                            ");
    printCheckResult(msg(MSG::DEBUG), tilePositiveFingerGap);
    ATH_MSG_DEBUG( "   -> Tile::NegativeFingerGap                            ");
    printCheckResult(msg(MSG::DEBUG), tileNegativeFingerGap);
    ATH_MSG_DEBUG( "   -> Tile::Girder                                 ");
    printCheckResult(msg(MSG::DEBUG), tileGirder);
  } // end of detailed output

  return tileTrackingVolumes;
}

void Tile::TileVolumeBuilder::printCheckResult(MsgStream& log, const Trk::TrackingVolume* vol) const
{
  if (vol) log << "... ok" << endmsg;
  else     log << "... missing" << endmsg;
}



void Tile::TileVolumeBuilder::printInfo(const PVConstLink pv) const
{
  const GeoLogVol* lv = pv->getLogVol();
  std::cout << "New Tile Object:"<<lv->getName()<<", made of"<<lv->getMaterial()->getName()<<","<<lv->getShape()->type()<<std::endl;
  //m_geoShapeConverter->decodeShape(lv->getShape());
  int igen=0;
  Amg::Transform3D transf =  pv->getX();
  printChildren(pv,igen,transf);
}

void Tile::TileVolumeBuilder::printChildren(const PVConstLink pv,int igen, Amg::Transform3D trIn) const
{
  // subcomponents
  unsigned int nc = pv->getNChildVols();
  igen++; 
  std::string cname; 
  for (unsigned int ic=0; ic<nc; ic++) {
    Amg::Transform3D transf = trIn*pv->getXToChildVol(ic);
 
    //
    //std::cout << " dumping transform to subcomponent" << std::endl;
    //std::cout << transf[0][0]<<"," <<transf[0][1]<<"," <<transf[0][2]<<","<<transf[0][3] << std::endl;
    //std::cout << transf[1][0]<<"," <<transf[1][1]<<"," <<transf[1][2]<<","<<transf[1][3] << std::endl;
    //std::cout << transf[2][0]<<"," <<transf[2][1]<<"," <<transf[2][2]<<","<<transf[2][3] << std::endl;
    //
    const PVConstLink cv = pv->getChildVol(ic);
    const GeoLogVol* clv = cv->getLogVol();
    std::cout << "  ";
    std::cout << "subcomponent:"<<igen<<":"<<ic<<":"<<clv->getName()<<", made of"<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()
	      <<std::endl;
    std::cout << "position:"<< "R:"<<transf.translation().perp()<<",phi:"<< transf.translation().phi()<<",x:"<<transf.translation().x()<<",y:"<<transf.translation().y()<<",z:"<<transf.translation().z()<<std::endl;
    const GeoTrd* trd=dynamic_cast<const GeoTrd*> (clv->getShape());
    if (trd) std::cout<<"trddim:"<< trd->getXHalfLength1()<<","<<trd->getXHalfLength2()<<","<<trd->getYHalfLength1()<<","<<trd->getYHalfLength2()<<","<<trd->getZHalfLength()<< std::endl;
    const GeoTubs* tub=dynamic_cast<const GeoTubs*> (clv->getShape());
    if (tub) std::cout<<"tubdim:"<< tub->getRMin()<<","<<tub->getRMax()<<","<<tub->getZHalfLength()<< std::endl;
    const GeoPcon* con=dynamic_cast<const GeoPcon*> (clv->getShape());
    if (con) {
      const unsigned int nPlanes=con->getNPlanes();
      for (unsigned int i=0; i<nPlanes; i++) {
	std::cout<<"polycone:"<<i<<":"<< con->getRMinPlane(i)<<","<<con->getRMaxPlane(i)<<","<<con->getZPlane(i)<< std::endl;
      }
    }

    if (ic==0 || cname != clv->getName() ) {  
      //m_geoShapeConverter->decodeShape(clv->getShape()); 	 
      printChildren(cv,igen,transf);
      cname = clv->getName();
    }
  }  
   
}

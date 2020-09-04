/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BeamPipeBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometry/BeamPipeBuilder.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk inlcude
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkSurfaces/CylinderBounds.h"
// GeoModel include
#include "BeamPipeGeoModel/BeamPipeDetectorManager.h"
#include "GeoModelKernel/GeoTube.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

// constructor
InDet::BeamPipeBuilder::BeamPipeBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_beamPipeFromDb(true),
  m_beamPipeMgr(0),
  m_beamPipeMgrName("BeamPipe"),
  m_beamPipeEnvelope(1.*Gaudi::Units::mm),
  m_beamPipeOffsetX(0.*Gaudi::Units::mm),
  m_beamPipeOffsetY(0.*Gaudi::Units::mm),
  m_beamPipeRadius(33.1*Gaudi::Units::mm),
  m_beamPipeHalflength(2.7*Gaudi::Units::m),
  m_beamPipeThickness(1.*Gaudi::Units::mm),
  m_beamPipeX0(352.8*Gaudi::Units::mm),
  m_beamPipeL0(407.*Gaudi::Units::mm),
  //m_beamPipedEdX(0.2945*Gaudi::Units::MeV/Gaudi::Units::mm),
  m_beamPipeA(9.012),
  m_beamPipeZ(4.),
  m_beamPipeRho(1.848e-3),
  m_beamPipeBinsZ(1),
  m_identification("BeamPipe")
{
  declareInterface<Trk::ILayerBuilder>(this);
  declareProperty("BeamPipeFromGeoModel" , m_beamPipeFromDb);
  declareProperty("BeamPipeManager"      , m_beamPipeMgrName);
  declareProperty("BeamPipeEnvelope"     , m_beamPipeEnvelope);
  // --------------------- by hand properties of the beam pipe itself -- //
  declareProperty("BeamPipeOffsetX"      , m_beamPipeOffsetX);
  declareProperty("BeamPipeOffsetY"      , m_beamPipeOffsetY);
  declareProperty("BeamPipeRadius"       , m_beamPipeRadius);
  declareProperty("BeamPipeHalflength"   , m_beamPipeHalflength);
  declareProperty("BeamPipeThickness"    , m_beamPipeThickness);
  // --------------------- by hand material properties ---------------- //
  declareProperty("BeamPipeX0"           , m_beamPipeX0);
  declareProperty("BeamPipeAverageA"     , m_beamPipeA);
  declareProperty("BeamPipeAverageZ"     , m_beamPipeZ);
  declareProperty("BeamPipeAverageRho"   , m_beamPipeRho);
  // Material bins 
  declareProperty("BeamPipeMaterialBinsZ", m_beamPipeBinsZ);
  // identification 
  declareProperty("Identification"       , m_identification);
}

// destructor
InDet::BeamPipeBuilder::~BeamPipeBuilder()
{}

// Athena standard methods
// initialize
StatusCode InDet::BeamPipeBuilder::initialize()
{
  if (m_beamPipeFromDb && detStore()->retrieve(m_beamPipeMgr,m_beamPipeMgrName).isFailure()){
      ATH_MSG_ERROR("Could not retrieve BeamPipe detector manager '" << m_beamPipeMgrName << "'." );
      return StatusCode::FAILURE;
  } else if (m_beamPipeFromDb)
      ATH_MSG_DEBUG("Successfully retrieved BeamPipe detector manager '" << m_beamPipeMgrName << "'." );
  
  ATH_MSG_DEBUG( "initialize()" );
  return StatusCode::SUCCESS;
}

// finalize
StatusCode InDet::BeamPipeBuilder::finalize()
{
    ATH_MSG_DEBUG( "finalize() successful" );
    return StatusCode::SUCCESS;
}


/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector< const Trk::CylinderLayer* >* InDet::BeamPipeBuilder::cylindricalLayers() const
{
  
  std::vector<const Trk::CylinderLayer*>* beamPipe = new std::vector<const Trk::CylinderLayer*>;
  
  // the geometry
  Amg::Transform3D* beamPipeTransform =  new Amg::Transform3D;
  beamPipeTransform->setIdentity();

  double beamPipeRadius = m_beamPipeRadius;
  
  if (m_beamPipeMgr){
        // get the central top volume
        PVConstLink beamPipeTopVolume =  m_beamPipeMgr->getTreeTop(0);
        (*beamPipeTransform) = Amg::Translation3D(beamPipeTopVolume->getX().translation().x(),
                                                  beamPipeTopVolume->getX().translation().y(),
                                                  beamPipeTopVolume->getX().translation().z());
        const GeoLogVol* beamPipeLogVolume = beamPipeTopVolume->getLogVol();
        const GeoTube* beamPipeTube = 0;
        if (beamPipeLogVolume){
            // get the geoShape and translate
            Trk::GeoShapeConverter geoShaper;
            beamPipeTube = dynamic_cast<const GeoTube*>(beamPipeLogVolume->getShape());
            if (beamPipeTube){

	      for(unsigned int i=0;i<beamPipeTopVolume->getNChildVols();i++){

		if(beamPipeTopVolume->getNameOfChildVol(i)=="SectionC03"){
		  PVConstLink childTopVolume =  beamPipeTopVolume->getChildVol(i);
		  const GeoLogVol* childLogVolume = childTopVolume->getLogVol();
		  const GeoTube* childTube = 0;

		  if (childLogVolume){
		    childTube = dynamic_cast<const GeoTube*>(childLogVolume->getShape());
		    if (childTube){
		      beamPipeRadius = 0.5 * (childTube->getRMax()+childTube->getRMin());
		    }
		  }

		  break; //Exit loop after SectionC03 is found
		}

	      } // Loop over child volumes

	    }

        }
        ATH_MSG_VERBOSE("BeamPipe constructed from Database: translation (yes) - radius "<< ( beamPipeTube ? "(yes)" : "(no)") << " - r = " << beamPipeRadius );        
  } else 
      (*beamPipeTransform) = Amg::Translation3D(m_beamPipeOffsetX, m_beamPipeOffsetY, 0.);

  ATH_MSG_VERBOSE("BeamPipe shift estimated as    : " 
      <<  beamPipeTransform->translation().x() << ", "
      <<  beamPipeTransform->translation().y() << ","
      <<  beamPipeTransform->translation().y());
  
  Trk::CylinderBounds* beamPipeBounds    = new Trk::CylinderBounds(beamPipeRadius, m_beamPipeHalflength);
  ATH_MSG_VERBOSE("BeamPipe bounds constructed as : " << (*beamPipeBounds) );

  // the material
  Trk::MaterialProperties beamPipeMaterial(m_beamPipeThickness,
					   m_beamPipeX0,
                                           m_beamPipeL0,
					   m_beamPipeA,
					   m_beamPipeZ,
					   m_beamPipeRho);
  
  // binned layer material for the beam pipe possible
  Trk::LayerMaterialProperties* beamPipeLayerMaterial=0;
  if (  m_beamPipeBinsZ == 1) 
     beamPipeLayerMaterial = new Trk::HomogeneousLayerMaterial(beamPipeMaterial, 1.0);
  else { 
      Trk::BinUtility layerBinUtility(m_beamPipeBinsZ, -m_beamPipeHalflength, m_beamPipeHalflength, Trk::open, Trk::binZ );
      beamPipeLayerMaterial = new Trk::BinnedLayerMaterial(layerBinUtility);  
  }
  
  ATH_MSG_DEBUG( "BeamPipe constructed with material-properties: " << beamPipeMaterial ); 
                                                                                    
  beamPipe->push_back(new Trk::CylinderLayer(beamPipeTransform,
                                             beamPipeBounds,
                                             *beamPipeLayerMaterial,
                                             m_beamPipeThickness));
  //delete beamPipeLayerMaterial; 
  return beamPipe;
  
} 
      




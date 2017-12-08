/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputLayerMaterialProvider.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// STL
#include <sstream>
// Trk include
#include "ISF_FatrasDetDescrTools/InputLayerMaterialProvider.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"

// constructor
iFatras::InputLayerMaterialProvider::InputLayerMaterialProvider(const std::string& t, const std::string& n, const IInterface* p)
  : AthAlgTool(t,n,p),
    m_layerLengthX(16.4),
    m_layerLengthY(60.8),
    m_NbinsX(5),
    m_NbinsY(5),
    m_NbinsSupport(3),
    m_sensorThickness(0.250),
    m_supportThickness(0.296),
    m_sensorPathX0(0.011),
    m_supportPathX0(0.024),
    m_materialL0(100),
    m_materialA(14),
    m_materialZ(28.0855),
    m_materialRho(0.00233)
{
  declareInterface<iFatras::IInputLayerMaterialProvider>(this);
    
  // Name specification from outside
  declareProperty("LayerLengthX", m_layerLengthX);	
  declareProperty("LayerLengthY", m_layerLengthY);	
  declareProperty("NbinsX",       m_NbinsX);	
  declareProperty("NbinsY",       m_NbinsY);	
  declareProperty("NbinsSupport", m_NbinsSupport);
      
  declareProperty("SensorMaterialThickness" , m_sensorThickness);	
  declareProperty("SupportMaterialThickness", m_supportThickness);
  
  declareProperty("SensorMaterialPathX0" , m_sensorPathX0);	
  declareProperty("SupportMaterialPathX0", m_supportPathX0);	

  declareProperty("MaterialL0" , m_materialL0);	
  declareProperty("MaterialA"  , m_materialA);	
  declareProperty("MaterialZ"  , m_materialZ);	
  declareProperty("MaterialRho", m_materialRho);	
      
}

// destructor
iFatras::InputLayerMaterialProvider::~InputLayerMaterialProvider()
{}

// Action to work on Layers 
Trk::LayerMaterialProperties * iFatras::InputLayerMaterialProvider::handleMaterial() {

  Trk::BinUtility layerBinUtilityX(m_NbinsX, -m_layerLengthX/2. , m_layerLengthX/2., Trk::open, Trk::binX);
  Trk::BinUtility layerBinUtilityY(m_NbinsY, -m_layerLengthY/2. , m_layerLengthY/2., Trk::open, Trk::binY);
  
  layerBinUtilityX += layerBinUtilityY;
  
  Trk::MaterialPropertiesMatrix  * materialMatrix = new Trk::MaterialPropertiesMatrix(m_NbinsX);

  for ( unsigned int x = 0; x < m_NbinsX; x++) {
    for ( unsigned int y = 0; y < m_NbinsY; y++) {
     if (y >= (m_NbinsY-m_NbinsSupport)/2 && y < (m_NbinsY+m_NbinsSupport)/2)
       (materialMatrix->at(x)).push_back( new Trk::MaterialProperties( m_sensorThickness+m_supportPathX0,
								       m_sensorThickness/m_sensorPathX0 + m_supportPathX0/m_supportPathX0,
								       m_materialL0,
								       m_materialA,
								       m_materialZ,
								       m_materialRho ));
     else
       (materialMatrix->at(x)).push_back( new Trk::MaterialProperties( m_sensorThickness,
								       m_sensorThickness/m_sensorPathX0,
								       m_materialL0,
								       m_materialA,
								       m_materialZ,
								       m_materialRho ));
    }
  }

  return new Trk::BinnedLayerMaterial(layerBinUtilityX, *materialMatrix); 
  
}


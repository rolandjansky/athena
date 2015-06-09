/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SCT Sensitive Detector Tool.
//
//#define SCTSD_CTB_DEBUG

//class header
#include "SctSensor_CTBTool.h"

//package includes
#include "SctSensor_CTB.h"

//helpers
#include "SimHelpers/DetectorGeometryHelper.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensor_CTBTool::SctSensor_CTBTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* SctSensor_CTBTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );

  DetectorGeometryHelper DGHelp;
  if(  DGHelp.GeometryType("SCT") == GeoModel ){
    ATH_MSG_DEBUG( "SCT Geometry is from GeoModel" );
  } else {
    ATH_MSG_ERROR( "SCT Geometry is from pure G4" );
    throw std::runtime_error("SCTGeometryFromPureG4");
  }

  return new SctSensor_CTB(name(), m_outputCollectionNames[0]);
}


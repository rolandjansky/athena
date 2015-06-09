/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SCT Sensitive Detector Tool.
//

// class header
#include "SctSensorSDTool.h"

// package includes
#include "SctSensorSD.h"

// athena includes
#include "SimHelpers/DetectorGeometryHelper.h"

// STL includes
#include <exception>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SctSensorSDTool::SctSensorSDTool(const std::string& type, const std::string& name, const IInterface* parent)
  : SensitiveDetectorBase( type , name , parent )
{
  declareInterface<ISensitiveDetector>(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VSensitiveDetector* SctSensorSDTool::makeSD()
{
  ATH_MSG_DEBUG( "Initializing SD" );

  DetectorGeometryHelper DGHelp;
  if (DGHelp.GeometryType("SCT") == GeoModel){
    ATH_MSG_DEBUG( "SCT Geometry is from GeoModel" );
  } else {
    ATH_MSG_ERROR( "SCT Geometry is from pure G4. NOT SUPPORTED!" );
    throw std::runtime_error("DoNotSupportPureG4Geometry");
  }

  return new SctSensorSD(name(), m_outputCollectionNames[0]);
}


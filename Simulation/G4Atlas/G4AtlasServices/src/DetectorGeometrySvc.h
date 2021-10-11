/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_DETECTORGEOMETRYSVC_H
#define G4ATLASSERVICES_DETECTORGEOMETRYSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Athena headers
#include "G4AtlasInterfaces/IDetectorConstructionTool.h"

// Gaudi headers
#include "GaudiKernel/ToolHandle.h" // For tool handle

class G4VUserDetectorConstruction;

class DetectorGeometrySvc : public extends<AthService, IDetectorGeometrySvc> {
public:
  // Standard constructor and destructor
  DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~DetectorGeometrySvc() = default;

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;

  G4VUserDetectorConstruction* GetDetectorConstruction() override final;

  std::vector<std::string>& GetParallelWorldNames() override final; // Called by G4AtlasAlg and (G4Legacy)TransportTool.

protected:

private:
  PublicToolHandle<IDetectorConstructionTool> m_detConstruction{this, "DetectorConstruction", "", "Tool handle of the DetectorConstruction"};
};

#endif

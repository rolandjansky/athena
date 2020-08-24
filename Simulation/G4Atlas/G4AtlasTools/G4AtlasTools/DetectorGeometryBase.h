/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_DETECTORGEOMETRYBASE_H
#define G4ATLASTOOLS_DETECTORGEOMETRYBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IDetectorGeometryTool.h"

// Athena headers
#include "G4AtlasInterfaces/IG4GeometryNotifierSvc.h"
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// Gaudi headers
#include "GaudiKernel/ServiceHandle.h" // For service handle
#include "GaudiKernel/ToolHandle.h" // For tool handle array

// STL library
#include <string>
#include <vector>

/// @todo NEEDS DOCUMENTATION
class DetectorGeometryBase : public extends<AthAlgTool, IDetectorGeometryTool>
{
public:
  // Basic constructor and destructor
  DetectorGeometryBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~DetectorGeometryBase() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override;


  /** purely virtual methods being implemented here */
  virtual void Build() override;

  virtual void BuildGeometry() override;

  virtual void PositionInParent() override;

  virtual void SetRotationAndOffset(); //override; TODO add to IDetectorGeometryTool?

  virtual void BuildSubDetectors() override;

  virtual void SetEnvelope() override;

  virtual void ResetEnvelope() override;

  virtual void SetAsWorld() override;

  std::string GetDetectorName() const override;

  void SetDetectorName(const std::string) override;

  void SetParent(IDetectorGeometryTool*) override;

  Envelope& GetEnvelope() override final;

  G4VPhysicalVolume* GetWorldVolume() override final;

protected:
  ToolHandleArray<IDetectorGeometryTool> m_subDetTools{this, "SubDetectors", {}, "Tool handle array of all subdetector tools"};

  ServiceHandle<IG4GeometryNotifierSvc> m_notifierSvc{this, "GeometryNotifierSvc", "G4GeometryNotifierSvc", "Detector name (same as the Tool name if not set"};

  IDetectorGeometryTool *m_theParent{};

  Envelope m_envelope;
  Gaudi::Property<std::string> m_detectorName{this, "DetectorName", "", "Detector name (same as the Tool name if not set"};
  bool m_isWorld{false};
  Gaudi::Property<double> m_rotateX{this, "RotateX" , 0.0, "Rotation around the X-axis"};
  Gaudi::Property<double> m_rotateY{this, "RotateY" , 0.0, "Rotation around the Y-axis"};
  Gaudi::Property<double> m_rotateZ{this, "RotateZ" , 0.0, "Rotation around the Z-axis"};
  Gaudi::Property<double> m_offsetX{this, "OffsetX" , 0.0, "Offset in the X-direction"};
  Gaudi::Property<double> m_offsetY{this, "OffsetY" , 0.0, "Offset in the Y-direction"};
  Gaudi::Property<double> m_offsetZ{this, "OffsetZ" , 0.0, "Offset in the Z-direction"};

};

#endif

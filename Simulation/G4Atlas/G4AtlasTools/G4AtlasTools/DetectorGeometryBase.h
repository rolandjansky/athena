/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
class DetectorGeometryBase : virtual public IDetectorGeometryTool, public AthAlgTool
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

  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**) override final;

protected:
  ToolHandleArray<IDetectorGeometryTool> m_subDetTools;

  ServiceHandle<IG4GeometryNotifierSvc> m_notifierSvc;

  IDetectorGeometryTool *m_theParent;

  Envelope m_envelope;
  std::string m_detectorName;
  bool m_isWorld;
  double m_rotateX;
  double m_rotateY;
  double m_rotateZ;
  double m_offsetX;
  double m_offsetY;
  double m_offsetZ;

};

#endif

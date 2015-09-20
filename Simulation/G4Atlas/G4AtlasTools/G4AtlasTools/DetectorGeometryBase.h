/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES_DETECTORGEOMETRYBASE_H
#define G4ATLASINTERFACES_DETECTORGEOMETRYBASE_H

// Base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "G4AtlasInterfaces/IDetectorGeometryTool.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

// Members
#include "GaudiKernel/ToolHandle.h" // For tool handle array
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"

// STL library
#include <string>
#include <vector>

class DetectorGeometryBase : virtual public IDetectorGeometryTool, public AthAlgTool 
{
public:
  // Basic constructor and destructor
  DetectorGeometryBase(const std::string& type, const std::string& name, const IInterface *parent);
  virtual ~DetectorGeometryBase() {}

  /** Athena method. called at initialization time, being customized here */
  StatusCode initialize() override;
  
  
  /** purely virtual methods being implemented here */
  virtual void Build();
  
  virtual void BuildGeometry();
  
  virtual void PositionInParent();
  
  virtual void BuildSubDetectors();
  
  virtual void SetEnvelope();
  
  virtual void ResetEnvelope();
  
  virtual void SetAsWorld();
  
  std::string GetDetectorName() const;
  
  void SetDetectorName(const std::string);
  
  void SetParent(IDetectorGeometryTool*);
  
  Envelope& GetEnvelope() override final; 
  
  G4VPhysicalVolume* GetWorldVolume() override final;
  
  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**);

protected:
  ToolHandleArray<IDetectorGeometryTool> m_subDetTools;
  
  IDetectorGeometrySvc* m_detGeoSvc;
  
  IDetectorGeometryTool* theParent;
  
  Envelope envelope;
  
  std::string m_detectorName;
  bool m_isWorld;

};

#endif

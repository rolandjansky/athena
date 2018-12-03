/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASSERVICES_G4GEOMETRYNOTIFIERSVC_H
#define G4ATLASSERVICES_G4GEOMETRYNOTIFIERSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IG4GeometryNotifierSvc.h"

class G4VNotifier;

class G4GeometryNotifierSvc : public extends<AthService, IG4GeometryNotifierSvc> {
public:
  // Standard constructor and destructor
  G4GeometryNotifierSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~G4GeometryNotifierSvc();

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;

  //FIXME This is a bit nasty as it is not thread-safe, but we assume
  //that the geometry building will be done in a single thread.
  void SetCurrentDetectorName(std::string s) override final {m_currentDetectorName=s;}
  const std::string GetCurrentDetectorName() const override final {return m_currentDetectorName;}

private:
  std::string m_currentDetectorName;

  bool m_activateLVNotifier=false;
  bool m_activatePVNotifier=true;
  G4VNotifier* lvNotifier;
  G4VNotifier* pvNotifier;
};

#endif //G4ATLASSERVICES_G4GEOMETRYNOTIFIERSVC_H

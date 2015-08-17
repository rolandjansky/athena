/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_SD_ACTIVATEUSERACTION_H
#define ISF_GEANT4TOOLS_SD_ACTIVATEUSERACTION_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"
//#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleHelper.h"
#include "ISF_Geant4Interfaces/ISDActivateUserAction.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "FadsActions/UserAction.h"
#include "FadsActions/TrackingAction.h"

#include <string>

namespace ISF {
  class IParticleHelper;
  class IParticleBroker;
}

class G4StackManager;

namespace FADS {
  class FadsSensitiveDetector;
}

namespace iGeant4 {

  
  class SDActivateUserAction: virtual public ISDActivateUserAction,  public AthAlgTool {
    
  public:
    SDActivateUserAction(const std::string& type,
			 const std::string& name,
			 const IInterface* parent);
    virtual ~SDActivateUserAction();

    StatusCode initialize();
    StatusCode finalize();

    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);
    void PreUserTrackingAction(const G4Track*);
    void PostUserTrackingAction(const G4Track*);

  private:
    
    void activateSD(bool active);
    
    G4StackManager* m_stackManager;
    std::vector<FADS::FadsSensitiveDetector*> m_sdToDeactivate;

    bool m_sdActivated;
  };
   
}

#endif // ISF_GEANT4TOOLS_SD_ACTIVATEUSERACTION_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H
#define ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/MsgStreamMember.h"

//#include "ISF_Geant4UserActions/TrackProcessor.h"

#include "G4RunManager.hh"
#include "G4VUserPhysicsList.hh"
#include "G4UserSteppingAction.hh"

#include "G4AtlasInterfaces/ISensitiveDetectorSvc.h"
#include "G4AtlasInterfaces/IFastSimulationSvc.h"


namespace ISF {
  class IParticleBroker;
}

namespace iGeant4
{
  /** @class G4AtlasRunManager
      
      @author Robert Harrington
  */

  class TrackProcessor;

  class G4AtlasRunManager : public G4RunManager {
    
    friend class G4TransportTool;
    
  public:
    virtual ~G4AtlasRunManager() {}
    
    static G4AtlasRunManager* GetG4AtlasRunManager();

    //    void SetPhysicsList(G4VUserPhysicsList* p) { m_pl = p; }
    
    void SetUserSteppingAction(G4UserSteppingAction* p) { m_steppingActions.push_back(p); }

    bool ProcessEvent(G4Event* event);

    void RunTermination(); 
    void SetCurrentG4Event(int);

    //void setParticleBroker(ServiceHandle<ISF::IParticleBroker>* stackSvc) { m_trackProcessorUserAction->setParticleBroker(stackSvc); }
    
    //void setParticleHelper(ToolHandle<ISF::IParticleHelper>* particleHelper) { m_trackProcessorUserAction->setParticleHelper(particleHelper); }

    /// Log a message using the Athena controlled logging system
    MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
    /// Check whether the logging system is active at the provided verbosity level
    bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }

  protected:
    
    void InitializeGeometry();
    void InitializePhysics();
    
  private:
    G4AtlasRunManager();
    void EndEvent();

    void SetReleaseGeo(bool b) { m_releaseGeo = b; }
    
    //    G4VUserPhysicsList * m_pl;

    std::vector<G4UserSteppingAction*> m_steppingActions;

    //    iGeant4::TrackProcessor* m_trackProcessorUserAction;

    bool m_releaseGeo;

    /// Private message stream member
    mutable Athena::MsgStreamMember m_msg;

    ISensitiveDetectorSvc* m_senDetSvc;
    IFastSimulationSvc* m_fastSimSvc;
  };

}


#endif // ISF_GEANT4TOOLS_G4ATLASRUNMANAGER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_GEANT4TOOLS_TRANSPORTTOOL_H
#define ISF_GEANT4TOOLS_TRANSPORTTOOL_H

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "ISF_Geant4Interfaces/ITransportTool.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"

#include <string>

class AthenaStackingAction;
class G4Event;
class StoreGateSvc;

namespace ISF {
  class ISFParticle;
  //class IParticleBroker;
  //class IParticleHelper;
}

namespace HepMC {
  class GenEvent;
}

//namespace PyAthena {
//  class Tool;
//}

namespace iGeant4
{
  
    /** @class G4TransportTool 
      
      Geant4 AlgTool to create a ISFParticle at a volume entry/exit
      - universal transport tool
      
      @author Robert Harrington
  */  

  class G4AtlasRunManager;
  class IPhysicsValidationUserAction;
  class ITrackProcessorUserAction;
  class IMCTruthUserAction;
  class ISDActivateUserAction;

  class G4TransportTool : virtual public ITransportTool, public AthAlgTool
  {

  public:					
    /** Constructor */
    G4TransportTool(const std::string&,const std::string&,const IInterface*);

    /** Destructor */
    virtual ~G4TransportTool ();
    
    /** AlgTool initialize method */
    virtual StatusCode initialize();
    /** AlgTool finalize method */
    virtual StatusCode finalize();
    
    /** Creates a new ParticleState from a given ParticleState, universal transport tool */
    StatusCode process(const ISF::ISFParticle& isp) const;

    /** Creates a new ParticleState from a given ParticleState, universal transport tool */
    StatusCode processVector(const std::vector<const ISF::ISFParticle*>& particles) const;

    /** Converts ISF::ISFParticle to G4Event */
    G4Event* ISF_to_G4Event(const ISF::ISFParticle& isp) const;

    /** Converts vector of ISF::ISFParticles to G4Event */
    G4Event* ISF_to_G4Event(const std::vector<const ISF::ISFParticle*>& isp) const;

  private:

    void addPrimaryVertex(G4Event* g4evt, const ISF::ISFParticle& isp) const;

    HepMC::GenEvent* genEvent() const;

    G4AtlasRunManager    * p_runMgr;
    AthenaStackingAction * m_stackingAction;

    // Random number service
    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;

    ToolHandle<ISF::IG4RunManagerHelper>  m_g4RunManagerHelper;
    ToolHandle<iGeant4::IPhysicsValidationUserAction> m_physicsValidationUserAction;
    ToolHandle<iGeant4::ITrackProcessorUserAction> m_trackProcessorUserAction;
    ToolHandle<iGeant4::IMCTruthUserAction> m_mcTruthUserAction;
    ToolHandle<iGeant4::ISDActivateUserAction> m_sdActivateUserAction;

    //ServiceHandle<ISF::IParticleBroker> m_particleBroker;
    //ToolHandle<ISF::IParticleHelper> m_particleHelper;

    //ToolHandle<PyAthena::Tool> m_configTool;

    StoreGateSvc* m_storeGate; 

    std::string m_libList;					
    std::string m_physList;
    std::string m_fieldMap;
    std::string m_rndmGen;
    
    std::string m_mcEventCollectionName;

    bool   m_KillAllNeutrinos;
    double m_KillLowEPhotons;
    bool   m_releaseGeoModel;

  };
  

}


#endif // ISF_GEANT4TOOLS_TRANSPORTTOOL_H

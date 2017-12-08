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

#include "BarcodeEvent/Barcode.h"

#include "ISF_Geant4Interfaces/ITransportTool.h"
#include "ISF_Geant4Tools/IG4RunManagerHelper.h"
#include "G4AtlasInterfaces/IPhysicsListTool.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"

#include <string>

class G4Event;
class G4PrimaryParticle;

namespace Barcode {
  class IBarcodeSvc;
}

namespace ISF {
  class ISFParticle;
}

namespace HepMC {
  class GenEvent;
  class GenParticle;
}

namespace iGeant4
{

  /** @class G4TransportTool

      Geant4 AlgTool to create a ISFParticle at a volume entry/exit
      - universal transport tool

      @author Robert Harrington
  */

  class G4AtlasRunManager;

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
    StatusCode process(const ISF::ISFParticle& isp);

    /** Creates a new ParticleState from a given ParticleState, universal transport tool */
    StatusCode processVector(const std::vector<const ISF::ISFParticle*>& particles);

    /** Converts ISF::ISFParticle to G4Event */
    G4Event* ISF_to_G4Event(const ISF::ISFParticle& isp);

    /** Converts vector of ISF::ISFParticles to G4Event */
    G4Event* ISF_to_G4Event(const std::vector<const ISF::ISFParticle*>& isp);

  private:

    G4PrimaryParticle* getPrimaryParticle(const HepMC::GenParticle& gp) const;

    G4PrimaryParticle* getPrimaryParticle(const ISF::ISFParticle& isp) const;

    void addPrimaryVertex(G4Event* g4evt, const ISF::ISFParticle& isp) const;

    /** Tests whether the given ISFParticle is within the Geant4 world volume */
    bool isInsideG4WorldVolume(const ISF::ISFParticle& isp);

    HepMC::GenEvent* genEvent() const;

    G4AtlasRunManager    * m_pRunMgr;

    /// Activate multi-threading configuration
    bool m_useMT;
    /// user action service 
    ServiceHandle<G4UA::IUserActionSvc> m_userActionSvc;

    // Random number service
    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;
    ServiceHandle<Barcode::IBarcodeSvc>       m_barcodeSvc;                 //!< The ISF Barcode service
    Barcode::ParticleBarcode                  m_barcodeGenerationIncrement; //!< to be retrieved from ISF Barcode service

    ToolHandle<ISF::IG4RunManagerHelper>  m_g4RunManagerHelper;
    ToolHandle<IPhysicsListTool> m_physListTool;
    //  ToolHandle<IUserAction> m_physicsValidationUserAction;
    //  ToolHandle<IUserAction> m_trackProcessorUserAction;
    //  ToolHandle<IUserAction> m_mcTruthUserAction;

    std::string m_libList;
    std::string m_physList;
    std::string m_fieldMap;
    std::string m_rndmGen;

    std::string m_mcEventCollectionName;

    bool   m_releaseGeoModel;
    bool   m_recordFlux;
    bool   m_quasiStableParticlesIncluded; //<! will quasi-stable
                                           //particles be included in
                                           //simulation

    G4VSolid *m_worldSolid;                // the Geant4 world volume solid

    /// Commands to send to the G4 UI
    std::vector<std::string> m_g4commands;
  };


}


#endif // ISF_GEANT4TOOLS_TRANSPORTTOOL_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H
#define ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H

#include "AthenaBaseComps/AthAlgTool.h"

//#include "FadsActions/UserAction.h"
//#include "FadsActions/TrackingAction.h"

#include "G4AtlasInterfaces/IUserActionSvc.h"
#include "G4AtlasTools/UserActionBase.h"

#include "ISF_Interfaces/IParticleBroker.h"
#include "ISF_Interfaces/IParticleHelper.h"
#include "ISF_Interfaces/IGeoIDSvc.h"

//#include "ISF_Geant4Interfaces/IPhysicsValidationUserAction.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

// Atlas G4 Helpers
#include "SimHelpers/SecondaryTracksHelper.h"

#ifndef MAXCHILDREN
#define MAXCHILDREN 40
#endif

#include <string>

// ROOT forward declarations
class TTree;

namespace ISF {
  class IParticleHelper;
  class IParticleBroker;
}

namespace iGeant4 {

  class PhysicsValidationUserAction final:  public UserActionBase {

  public:
    PhysicsValidationUserAction(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);
    virtual ~PhysicsValidationUserAction() {}

    StatusCode initialize();
    StatusCode finalize();

    void BeginOfEvent(const G4Event*) override;
    void EndOfEvent(const G4Event*) override;
    void BeginOfRun(const G4Run*) override;

    void Step(const G4Step*) override;

    void PreTracking(const G4Track* aTrack) override;

    virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:
    ServiceHandle<IUserActionSvc>  m_UASvc;

    SecondaryTracksHelper m_sHelper;

    ServiceHandle<ISF::IParticleBroker>  m_particleBroker;
    ToolHandle<ISF::IParticleHelper>     m_particleHelper;

    /** access to the central ISF GeoID serice*/
    ServiceHandle<ISF::IGeoIDSvc>        m_geoIDSvc;      //!< athena service handle
    ISF::IGeoIDSvc                      *m_geoIDSvcQuick; //!< quickaccess avoiding gaudi ovehead


    /** Validation output with histogram service */
    bool                                                         m_validationOutput; //!< turn validation mode on/off
    ServiceHandle<ITHistSvc>                                     m_thistSvc;         //!< the histogram service
    std::string                                                  m_validationStream; //!< validation THist stream name

    TTree                                                        *m_particles;    //!< ROOT tree containing track info
    int                                                           m_pdg;
    int                                                           m_scIn;
    int                                                           m_scEnd;
    int                                                           m_gen;
    int                                                           m_geoID;
    float                                                         m_theta;
    float                                                         m_pth;
    float                                                         m_pph;
    float                                                         m_p;
    float                                                         m_eloss;
    float                                                         m_radloss;
    float                                                         m_ionloss;
    float                                                         m_wzOaTr;
    float                                                         m_thIn;
    float                                                         m_phIn;
    float                                                         m_dIn;
    float                                                         m_thEnd;
    float                                                         m_phEnd;
    float                                                         m_dEnd;
    float                                                         m_X0;
    float                                                         m_L0;
    float                                                         m_wZ;
    float                                                         m_dt;

    TTree                                                        *m_interactions;   //!< ROOT tree containing vertex info
    mutable int                                                   m_process;
    mutable int                                                   m_pdg_mother;
    mutable int                                                   m_gen_mother;
    mutable int                                                   m_nChild;
    mutable  float                                                m_vtx_dist;
    mutable float                                                 m_vtx_theta;
    mutable float                                                 m_vtx_phi;
    mutable float                                                 m_vtx_e_diff;
    mutable float                                                 m_vtx_p_diff;
    mutable float                                                 m_vtx_plong_diff;
    mutable float                                                 m_vtx_pperp_diff;
    mutable float                                                 m_p_mother;
    mutable float                                                 m_radLength;
    mutable int                                                   m_pdg_child[MAXCHILDREN];     // decay product pdg code
    mutable float                                                 m_fp_child[MAXCHILDREN];      // fraction of incoming momentum
    mutable float                                                 m_oa_child[MAXCHILDREN];      // opening angle wrt the mother


    /** GeoIDSvc needs these? */
    double m_idR, m_idZ;
    double m_caloRmean,  m_caloZmean;
    double m_muonRmean, m_muonZmean;
    double m_cavernRmean, m_cavernZmean;
    int m_volumeOffset;
    int m_minHistoryDepth;

    mutable int m_currentTrack;
    std::map<int, int> m_trackGenMap;

  };

}

#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IPreTrackingAction.h"

namespace G4UA{
  namespace iGeant4 {
    class PhysicsValidationUserAction: public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction,  public IPreTrackingAction
    {

    public:

      struct Config
      {
        unsigned int verboseLevel=0;
      };

      PhysicsValidationUserAction(const Config& config);
      virtual void beginOfEvent(const G4Event*) override final;
      virtual void endOfEvent(const G4Event*) override final;
      virtual void beginOfRun(const G4Run*) override final;
      virtual void processStep(const G4Step*) override final;
      virtual void preTracking(const G4Track*) override final;
    private:
      Config m_config;
    }; // class PhysicsValidationUserAction

  } // namespace iGeant4

} // namespace G4UA

#endif // ISF_GEANT4TOOLS_PHYSICSVALIDATIONUSERACTION_H

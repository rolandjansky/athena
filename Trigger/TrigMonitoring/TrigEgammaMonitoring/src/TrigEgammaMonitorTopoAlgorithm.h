/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaMonitorTopoAlgorithm_H
#define TrigEgammaMonitorTopoAlgorithm_H


#include "TrigEgammaMonitorBaseAlgorithm.h"
#include "xAODBase/IParticleContainer.h"




class TrigEgammaMonitorTopoAlgorithm: public TrigEgammaMonitorBaseAlgorithm 
{


  struct Legs{
      const xAOD::IParticle *leg0; // online electron leg 0
      const xAOD::IParticle *leg1; // online electron leg 1
  };

  public:

    TrigEgammaMonitorTopoAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~TrigEgammaMonitorTopoAlgorithm() override;

    virtual StatusCode initialize() override;
    
    virtual StatusCode fillHistograms( const EventContext& ctx) const override;

 
  
  private:
 
    /*! List of configurations 
     *
     * d = {'trigger_num' : 'HLT_e26_lhtight_e15_etcut_Zee', 'trigger_den' : 'HLT_e26_lhtight_e15_etcut', 'leg0_type':'electron', 'leg1_type':'electron'}
     */
    Gaudi::Property<std::vector<std::map<std::string, std::string>>> m_trigListConfig{this, "TriggerListConfig", {}};
    /*! Min Delta R between online and offline */
    Gaudi::Property<float> m_dR{this, "DeltaR", 0.07};
    /*! Event Wise offline ElectronContainer Access and end iterator */
    SG::ReadHandleKey<xAOD::ElectronContainer> m_offElectronKey{ this, "ElectronKey", "Electrons", ""};
    SG::ReadHandleKey<xAOD::PhotonContainer> m_offPhotonKey{ this, "PhotonKey", "Photons", ""};

    /*! Get all combinations */
    void make_legs( std::string trigger, std::string key_leg0, std::string key_leg1, std::vector<Legs> &) const;
    bool match( const xAOD::IParticleContainer &container,const xAOD::IParticle *part_on , const xAOD::IParticle *&part_off) const;

};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#ifndef HAVE_DECORATED_HLT_TAU
#define HAVE_DECORATED_HLT_TAU

#include <iostream>
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"

class DecoratedHltTau {
  // an HLT tau candidate with associated preselected tracks etc.
  // needed because such an interface doesn't exist in RootCore
  //
  // NOTE: we don't own the HLT tau itself

  public:

    DecoratedHltTau(xAOD::TauJet *hlt_tau) : m_hlt_tau(hlt_tau), m_caloOnly_tau(nullptr) {
      m_preselTracksIso  = new xAOD::TrackParticleContainer();
      m_preselTracksCore = new xAOD::TrackParticleContainer();
      m_preselTracksIsoAux = new xAOD::TrackParticleAuxContainer();
      m_preselTracksCoreAux = new xAOD::TrackParticleAuxContainer();

      m_preselTracksIso->setStore(m_preselTracksIsoAux);
      m_preselTracksCore->setStore(m_preselTracksCoreAux); 
    }

    ~DecoratedHltTau() {
    }

    void setCaloOnyTau(xAOD::TauJet *caloOnly_tau) {
      m_caloOnly_tau = caloOnly_tau;
    }

    const xAOD::TauJet *getCaloOnyTau() const {
      return m_caloOnly_tau;
    }
  
    const xAOD::TauJet *getHltTau() const{
      return m_hlt_tau;
    }

    const xAOD::TrackParticleContainer* getPreselTracksIso() const {
      return m_preselTracksIso;
    }
    
    const xAOD::TrackParticleContainer* getPreselTracksCore() const {
      return m_preselTracksCore;
    }

    void addPreselTrackIso(xAOD::TrackParticle *t){
      m_preselTracksIso->push_back(t);
    }
    
    void addPreselTrackCore(xAOD::TrackParticle *t){
      m_preselTracksCore->push_back(t);
    }

    void addPreselTracksIso(const xAOD::TrackParticleContainer *trackContainer){
      for(auto track: *trackContainer) {
        xAOD::TrackParticle *new_track = new xAOD::TrackParticle();
        new_track->makePrivateStore(track);
        m_preselTracksIso->push_back(new_track);
      }
    }
    
    void addPreselTracksCore(const xAOD::TrackParticleContainer *trackContainer){
      for(auto track: *trackContainer) {
        xAOD::TrackParticle *new_track = new xAOD::TrackParticle();
        new_track->makePrivateStore(track);
        m_preselTracksCore->push_back(new_track);
      }
    }

  private:

    friend std::ostream & operator<<(std::ostream &os, const DecoratedHltTau& t){
        os << "Decorated HLT tau with pT=" << t.m_hlt_tau->pt() << "  eta=" << t.m_hlt_tau->eta() << " phi=" << t.m_hlt_tau->phi() << std::endl;
        if(t.m_caloOnly_tau){
          os << "   Calo only tau with pT=" << t.m_caloOnly_tau->pt() << "  eta=" << t.m_caloOnly_tau->eta() << " phi=" << t.m_caloOnly_tau->phi() << std::endl;
        }
        
        for(auto track: *(t.m_preselTracksIso)){
          os << "   ISO track " << track->pt() << " eta=" << track->eta() << " phi=" << track->phi() << std::endl;
        }
        for(auto track: *(t.m_preselTracksCore)){
          os << "   Core track " << track->pt() << " eta=" << track->eta() << " phi=" << track->phi() << std::endl;
        }

        return os;
    }


    xAOD::TauJet *m_hlt_tau;
    xAOD::TauJet *m_caloOnly_tau;

    xAOD::TrackParticleContainer *m_preselTracksIso;
    xAOD::TrackParticleContainer *m_preselTracksCore;

    xAOD::TrackParticleAuxContainer *m_preselTracksIsoAux;
    xAOD::TrackParticleAuxContainer *m_preselTracksCoreAux;

};

#endif

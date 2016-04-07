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

    DecoratedHltTau(xAOD::TauJet *_hlt_tau) : hlt_tau(_hlt_tau), caloOnly_tau(nullptr) {
      preselTracksIso  = new xAOD::TrackParticleContainer();
      preselTracksCore = new xAOD::TrackParticleContainer();
      preselTracksIsoAux = new xAOD::TrackParticleAuxContainer();
      preselTracksCoreAux = new xAOD::TrackParticleAuxContainer();

      preselTracksIso->setStore(preselTracksIsoAux);
      preselTracksCore->setStore(preselTracksCoreAux); 
    }

    ~DecoratedHltTau() {
    }

    void setCaloOnyTau(xAOD::TauJet *_caloOnly_tau) {
      caloOnly_tau = _caloOnly_tau;
    }

    const xAOD::TauJet *getCaloOnyTau() const {
      return caloOnly_tau;
    }
  
    const xAOD::TauJet *getHltTau() const{
      return hlt_tau;
    }

    const xAOD::TrackParticleContainer* getPreselTracksIso() const {
      return preselTracksIso;
    }
    
    const xAOD::TrackParticleContainer* getPreselTracksCore() const {
      return preselTracksCore;
    }

    void addPreselTrackIso(xAOD::TrackParticle *t){
      preselTracksIso->push_back(t);
    }
    
    void addPreselTrackCore(xAOD::TrackParticle *t){
      preselTracksCore->push_back(t);
    }

    void addPreselTracksIso(const xAOD::TrackParticleContainer *trackContainer){
      for(auto track: *trackContainer) {
        xAOD::TrackParticle *new_track = new xAOD::TrackParticle();
        new_track->makePrivateStore(track);
        preselTracksIso->push_back(new_track);
      }
    }
    
    void addPreselTracksCore(const xAOD::TrackParticleContainer *trackContainer){
      for(auto track: *trackContainer) {
        xAOD::TrackParticle *new_track = new xAOD::TrackParticle();
        new_track->makePrivateStore(track);
        preselTracksCore->push_back(new_track);
      }
    }

  private:

    friend std::ostream & operator<<(std::ostream &os, const DecoratedHltTau& t){
        os << "Decorated HLT tau with pT=" << t.hlt_tau->pt() << "  eta=" << t.hlt_tau->eta() << " phi=" << t.hlt_tau->phi() << std::endl;
        if(t.caloOnly_tau){
          os << "   Calo only tau with pT=" << t.caloOnly_tau->pt() << "  eta=" << t.caloOnly_tau->eta() << " phi=" << t.caloOnly_tau->phi() << std::endl;
        }
        
        for(auto track: *(t.preselTracksIso)){
          os << "   ISO track " << track->pt() << " eta=" << track->eta() << " phi=" << track->phi() << std::endl;
        }
        for(auto track: *(t.preselTracksCore)){
          os << "   Core track " << track->pt() << " eta=" << track->eta() << " phi=" << track->phi() << std::endl;
        }

        return os;
    }


    xAOD::TauJet *hlt_tau;
    xAOD::TauJet *caloOnly_tau;

    xAOD::TrackParticleContainer *preselTracksIso;
    xAOD::TrackParticleContainer *preselTracksCore;

    xAOD::TrackParticleAuxContainer *preselTracksIsoAux;
    xAOD::TrackParticleAuxContainer *preselTracksCoreAux;

};

#endif

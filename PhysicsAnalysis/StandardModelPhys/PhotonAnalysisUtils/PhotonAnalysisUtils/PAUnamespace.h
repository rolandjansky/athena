/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PAUnamespace.h, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////
//
// This file defines the namespace PAU, which is intended for **simple** 
// data classes related to H->gg analyses. By "simple" we intend data classes
// with only set/get methods, no algorithms!
//



#ifndef PHOTONANALYSISUTILS_PAUNAMESPACE_H
#define PHOTONANALYSISUTILS_PAUNAMESPACE_H

#include "egammaEvent/EMShower.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "VxVertex/VxCandidate.h"
#include "CaloEvent/CaloCluster.h"
#include "JetEvent/JetCollection.h"
#include "muonEvent/MuonContainer.h"
#include "MissingETEvent/MissingET.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/LinkToTrack.h"

namespace PAU {

  static const int IUNKNOWN = -99999 ;
  static const float UNKNOWN = -99999 ;

  //-------------------------------------------------------------------------------------
  // class PAU::jet
  class jet {
  private:
    const Jet * m_jet;
  public:
    jet()               : m_jet(0)   {};
    jet(const Jet *the_jet) : m_jet(the_jet) {};
    virtual ~jet() {};
    inline float pt() const { return m_jet->pt(); }
    inline float px() const { return m_jet->px(); }
    inline float py() const { return m_jet->py(); }
    inline float pz() const { return m_jet->pz(); }
    inline float eta() const { return m_jet->eta(); }
    inline float phi() const { return m_jet->phi(); }
    inline float e() const { return m_jet->e(); }
    inline const Jet * getJet() const { return m_jet; }
  };
  // end of PAU::jet class
  //-------------------------------------------------------------------------------------
  // class PAU::el
  class el {
  private:
    const Analysis::Electron * m_electron;
    const EMShower*            m_shower;
  public:
    el()                                           : m_electron(0) {};
    el(const Analysis::Electron *the_electron)     : m_electron(the_electron) {};
    virtual ~el() {};
    inline float pt() const { return m_electron->pt(); }
    inline float eta() const { return m_electron->eta(); }
    inline float phi() const { return m_electron->phi(); }
    //inline float phi() const { return m_electron->cluster()->phi(); }
    inline float e() const { return m_electron->e(); }
    inline const Analysis::Electron * electron() const { return m_electron; }
    inline void setShower(const EMShower* the_shower) { m_shower = the_shower; }
    inline const EMShower* shower() const { return m_shower; }
  };
  // end of PAU::electron class
  //-------------------------------------------------------------------------------------
  // class PAU::mu
  class mu {
  private:
    const Analysis::Muon * m_muon;
  public:
    mu()                                     : m_muon(0) {};
    mu(const Analysis::Muon *the_muon)       : m_muon(the_muon) {};
    virtual ~mu() {};
    inline float pt() const { return m_muon->pt(); }
    inline float eta() const { return m_muon->eta(); }
    inline float phi() const { return m_muon->phi(); }
    inline float e() const { return m_muon->e(); }
    inline const Analysis::Muon * muon() const { return m_muon; }
  };
  // end of PAU::mu class
  //-------------------------------------------------------------------------------------

  // class PAU::PtMiss
/*   class PtMiss { */
/*   private: */
/*     MissingET * m_ptmiss; */
/*   public: */
/*     PtMiss()                          : m_ptmiss(0) {}; */
/*     PtMiss(MissingET *MyPtmiss)       : m_ptmiss(MyPtmiss) {}; */
/*     virtual ~PtMiss() {}; */
/*     inline float pt() const { return m_ptmiss->et(); } */
/*     inline float px() const { return m_ptmiss->etx(); } */
/*     inline float py() const { return m_ptmiss->ety(); } */
/*     inline const MissingET * ptmiss() const {return m_ptmiss;} */
/*   }; */
  // end of PAU::PtMiss class
  
}

#endif // PHOTONANALYSISUTILS_PAUNAMESPACE_H

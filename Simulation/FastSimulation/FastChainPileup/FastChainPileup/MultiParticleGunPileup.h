/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCHAINPILEUP_MULTIPARTICLEGUN8_PILEUP_H
#define FASTCHAINPILEUP_MULTIPARTICLEGUN8_PILEUP_H

// MultiParticleGunPileup.h - generates multiple
// pileup events in one go for fast simulaton chain

// Olivier Arnaez, started 17/12/15

#include <string>
#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>

#include "GeneratorModules/GenModule.h"
#include "FastChainPileup/Samplers.h"

namespace HepMC {
  class GenEvent;
}

class MultiParticleGunPileup : public GenModule {

  public:
    MultiParticleGunPileup( const std::string &name, ISvcLocator *pSvcLocator );
    ~MultiParticleGunPileup();

    virtual StatusCode genInitialize();
    virtual StatusCode genFinalize();

    virtual StatusCode callGenerator();
    virtual StatusCode fillEvt( HepMC::GenEvent *event );

  private:
    int nPileupEvents();                // decide how much pileup events will be generated
    // parameters
    int m_ncollevent;                  // fixed number of collisions per event
    std::vector< int > m_pileupProfile;  // pileup mu-profile
    std::vector< float > m_multbcid;     // mu multipliers per BCID
    std::string m_filename;            // filename for histogram output

    int m_ngen;                         // total number of events generated
    int m_nbad;                         // total number of failed events
    // int m_evnumber;                     // event sequence number

    std::vector< HepMC::GenEvent * > m_evts;   // vector of generated events

    CLHEP::HepRandomEngine *m_randomEngine;

    std::vector< double > m_pileupProfileIntegral;

    TFile *m_file;
    TH1D *m_htgPileupProfile, *m_htgPileupMu, *m_htgPileupEvents;

    ParticleSampler * m_partSampler;
};

#endif

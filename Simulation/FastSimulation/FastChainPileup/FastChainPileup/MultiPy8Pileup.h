/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCHAINPILEUP_MULTIPY8_PILEUP_H
#define FASTCHAINPILEUP_MULTIPY8_PILEUP_H

// MultiPy8Pileup.h - extension of Pythia8_i to generate multiple
// pileup events in one go for fast simulaton chain
// Inherits from Pythia8_i, following example of Pythia8B_i

// Richard Hawkings, started 20/2/15

#include <string>
#include <TROOT.h>
#include <TFile.h>
#include <TH1D.h>

#include "Pythia8_i/Pythia8_i.h"

namespace HepMC {
  class GenEvent;
}

class MultiPy8Pileup : public Pythia8_i {

  public:
    MultiPy8Pileup( const std::string &name, ISvcLocator *pSvcLocator );
    ~MultiPy8Pileup();

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

    std::vector< HepMC::GenEvent * > m_evts;   // vector of generated minbias events

    CLHEP::HepRandomEngine *m_randomEngine;

    std::vector< double > m_pileupProfileIntegral;

    TFile *m_file;
    TH1D *m_htgPileupProfile, *m_htgPileupMu, *m_htgPileupEvents;
};

#endif

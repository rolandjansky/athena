/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBACKGROUNDCONVERTER_H
#define MUONBACKGROUNDCONVERTER_H

// -------------------------------------------------------------
// File: MuonBackgroundConverter/MuonBackgroundConverter.h
// Description:
//
//    Reads cavern-background events from an ASCII file, converts 
//    them to HepMC.
//
//    The output will be stored in the transient event store so it can be
//    passed to the simulation.
// 
// Author: A. Nairz, December 22, 2004
//         Based on work by W. Seligman, M. Shapiro, I. Hinchliffe;
//         S. Bentvelsen, N. van Eldik 
//
// Modifications:
//         - A.N. (Feb. 27, 2006): allow more flexible treatment of multiple 
//                                 hits with same energy (keep all/keep first)
//
//   	   - Ketevi A. Assamagan ( Jan 21, 2008): increase statistics with phi symmetry of Muon Spectrometer 
//                                                time distribution consistent with 25ns or 75ns or whatever
//                                                explicitly remove duplicates
// 
// --------------------------------------------------

#include "GaudiKernel/ServiceHandle.h"

#include "GeneratorModules/GenModule.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "CLHEP/Random/RandomEngine.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "AtlasHepMC/Polarization.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "CavernBkgGenerator/MuonBackgroundEventParser.h"


class MuonBackgroundConverter : public GenModule 
{
  public:

    MuonBackgroundConverter( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~MuonBackgroundConverter();

    virtual StatusCode genInitialize();
    virtual StatusCode callGenerator();
    virtual StatusCode genFinalize();
    virtual StatusCode fillEvt( HepMC::GenEvent* evt );


  private:

    int    m_nEvents;      // event counter
    int    m_phiN;      // rot symmetry = 8
    int    m_safetyFactor; // number of original input events to be merged in one output event 
    //bool   m_smearPhi;     // flag to switch on/off smearing in azimuthal angle phi (pT, p, particles/evt, ... conserved) 
    bool   m_firstHitOnly; // flag to switch on/off removal of multiple hits with same energy (true: only first hit is kept) 
    double m_ctcut;        // lifetime cut
    double m_t0;           // bunch crossing time
    int    m_used;         // keep track of how often a particle is used

    // variables related with input file
    std::string   m_infile;
    std::ifstream m_ffile;

    // Event scalars, three-vectors, and four-vectors:
    std::vector< int >                 m_pdgCode;
    std::vector< CLHEP::HepLorentzVector >    m_fourPos;
    std::vector< CLHEP::HepLorentzVector >    m_fourMom;
    std::vector< HepMC::Polarization > m_polarization;
    std::vector<MuonBackgroundEventParser> m_evt;

    ServiceHandle<IAthRNGSvc> m_rndmSvc;      // Random number service
    ATHRNG::RNGWrapper *m_rndmEngine;    // Random number engine used - not init in SiDigitization
    std::string m_rndmEngineName;// name of random engine

};

#endif

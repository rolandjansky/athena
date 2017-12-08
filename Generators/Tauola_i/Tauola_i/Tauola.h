/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATLASTAUOLA_H
#define ATLASTAUOLA_H
#include "GeneratorModules/GenModule.h"


#include "Tauola_i/Fftau.h"
#include "Tauola_i/Ffinout.h"
#include "Tauola_i/Atlas_HEPEVT.h"
#include "Tauola_i/hybrid.h"

#include "Tauola_i/TauDetails.h"
#include "Tauola_i/UserGamprt.h"

/// @todo Not in header!
using std::string;

/// @todo Make private -- this is currently 100% global!
typedef std::vector<std::string> CommandVector;

/**
@class   Tauola.h
@brief   Interface to Tauola for Herwig 6.5 and Pythia 6.2+
     to modify the HEPEVT common.

@author Borut Paul Kersevan (BPK), June 2003

*/
class IAtRndmGenSvc;

class Tauola : public GenModule {
public:
  Tauola(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Tauola();
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);
  StatusCode writeHepevt();
  
  // Accessors to TAUOLA COMMONS
  Fftau& fftau();
  Ffinout& ffinout();
  UserGamprt& usergamprt();

  // Pointer On AtRndmGenSvc
  /// @todo Use accessor from GenModule
  static IAtRndmGenSvc* p_AtRndmGenSvc;
  static std::string tauola_stream;

protected:
  // Settable Properties

  /// Atlas tauola output file
  int m_fortout;
  /// used as a flag
  int m_fortin;

  // Event seeds
  std::vector<long int> m_seeds;

  // event counter
  int m_events ;

  int m_eventnum ;
  double m_eventscale ;
  double m_eventqed ;
  double m_eventqcd ;

  int m_GenIndex;
  
  int m_SuperIndex;

  bool m_sherpa_blob;

  // missing boson pdg_id
  int m_pdgid ;

  // evt weights
  double m_avwgt,m_evwgt;

  double m_pari7,m_pari9,m_pari10; 

  // additional weights for ZW and WW McAtNlo processes
  double m_userwgts[28];
  
  bool m_usrwgt;
  int m_isize;

  // tauola parameters
  Fftau m_fftau;

  // io (LUN-s) for tauola
  Ffinout m_ffinout;

  // tauola extra details
  TauDetails m_tauDetails;

  // user GAMPRT
  UserGamprt m_usergamprt;

  //pdfinfo  
  HepMC::PdfInfo m_pdfinfo;


  // Commands to setup tauola
  CommandVector m_tauolaCommandVector;
  
  // I/O to HEPEVT
  void store_Atlas_HEPEVT(void);
  
  StatusCode restore_Atlas_HEPEVT(void);
  
  static Atlas_HEPEVT* s_atlas_HEPEVT;
  
  StatusCode sherpa_BLOB(HepMC::GenEvent* evt);
  
  // Sherpa placeholder
  HepMC::GenEvent m_genEvt;
  
  // Switch for storing Tauola details in HepMC (true by default)
  bool m_saveDetails;
  
  // How many taus need to be decayed
  int m_numberOfTauToDecay;
};

#endif

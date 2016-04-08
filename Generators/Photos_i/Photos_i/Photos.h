/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATLASPHOTOS_H
#define ATLASPHOTOS_H

#include "GeneratorModules/GenModule.h"

#include "Photos_i/Ffphot.h"
#include "Photos_i/Ffpholun.h"

#include "Tauola_i/hybrid.h"

/// @todo Move inside class - this def is GLOBAL!
typedef std::vector<std::string> CommandVector;


/**
   @class Photos_HepMC.h
   
   @brief  Interface to Photos for Herwig 6.5 and Pythia 6.2+
   to modify the HEPEVT common.
   
   @author Borut Paul Kersevan (BPK), June 2003
*/
class Photos : public GenModule {
public:

  Photos(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Photos();

  /// @name GenModule event methods
  //@{
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);
  //@}

  StatusCode writeHepevt();
  
  // Accessors to PHOTOS COMMONS
  Ffphot& ffphot();
  Ffpholun& ffpholun();

  // Pointer to AtRndmGenSvc
  /// @todo Replace with GenModule methods
  static IAtRndmGenSvc* p_AtRndmGenSvc;
  static std::string photos_stream;


protected:

  /// @name Settable properties
  //@{

  /// Photos output file
  int m_fortout;

  /// Event seeds
  std::vector<long int> m_seeds;

  /// Event counter
  /// @todo Use Athena counter, or put this functionality in GenModule
  int m_events;

  /// ??
  int m_eventnum;
  /// ??
  double m_eventscale;
  /// ??
  double m_eventqed;
  /// ??
  double m_eventqcd;

  /// ??
  int m_GenIndex;
  
  /// ??
  int m_SuperIndex;

  /// ??
  int m_SubIndex;

  /// Event weights
  double m_avwgt, m_evwgt;

  /// Common block params
  double m_pari7,m_pari9,m_pari10; 

  /// Photos parameters
  Ffphot m_ffphot;

  /// I/O (LUN-s) for Photos
  Ffpholun m_ffpholun;
  
  /// Commands to set up Photos
  CommandVector m_photosCommandVector;
  
  // I/O for SHERPA
  StatusCode sherpa_BLOB(HepMC::GenEvent* evt);

  // Event placeholder
  HepMC::GenEvent m_genEvt;
};

#endif

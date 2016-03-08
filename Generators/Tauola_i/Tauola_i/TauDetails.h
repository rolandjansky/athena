/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDETAILS_H
#define TAUDETAILS_H

#include "Tauola_i/Fftaudet.h"
#include "HepMC/GenEvent.h"

// Framework Related Headers
#include "GaudiKernel/MsgStream.h"

/**
@class TauDetails.h

@brief Interface to Tauola decay details
@author Eric Torrence, September 2009
**/

class TauDetails {
 public:
  TauDetails();
  ~TauDetails();

  void clear() { fftaudet().ntausav() = 0; }
  size_t size() { return fftaudet().ntausav(); }

  // Clear details, but only if this is a new event
  // Return value indicates whether details were actually cleared
  bool clearNewEvent(int eventNumber);

  // Used to indicate event number
  int  setEvent(int eventNumber);

  // Conversion of Pythia HEPEVT -> HepMC doesn't reorder
  // particles, use barcode to match taus
  void savePythia(MsgStream& log, HepMC::GenEvent* evt);

  // Conversion of Herwig HEPEVT -> HepMC does reorder
  // particles, use order found in listing to match taus
  void saveHerwig(MsgStream& log, HepMC::GenEvent* evt);

 protected:

  // Accessors to TAUOLA COMMONS
  Fftaudet& fftaudet();

 private:
  bool m_verbose;

  // Tauola extra details common block
  Fftaudet m_fftaudet;

};

#endif

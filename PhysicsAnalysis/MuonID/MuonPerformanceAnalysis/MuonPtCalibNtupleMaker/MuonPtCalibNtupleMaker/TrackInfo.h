/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrackInfo_h__
#define __TrackInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

//::: Framework includes
#include "CLHEP/Units/SystemOfUnits.h"

//::: EDM includes
#include "xAODTracking/TrackParticlexAODHelpers.h"

class TrackInfo { 
  
private:
  std::string m_Type;
  Float_t m_Pt, m_Eta, m_Phi, m_Charge, m_QoverP, m_QoverPSigma, m_Chi2, m_NDoF;

public:
  TrackInfo( std::string type );
  ~TrackInfo() {}

  void Reset();
  void Register( TTree* t, std::string p );
  void Fill( const xAOD::TrackParticle* tp );

};

#endif

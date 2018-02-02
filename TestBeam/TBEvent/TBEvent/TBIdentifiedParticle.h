//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/CLASS_DEF.h"

#ifndef TBIDENTIFIEDPARTICLE
#define TBIDENTIFIEDPARTICLE 

class TBIdentifiedParticle 
{
 public:
   enum ParticleName {
     UNKNOWN,
     ELECTRON,
     PION,
     MUON
   };


  TBIdentifiedParticle(); 
  TBIdentifiedParticle (const ParticleName name) {m_name=name;}
  inline void setParticle (const ParticleName name) {m_name=name;}
  inline ParticleName getParticle() const {return m_name;}
 

 private:
  ParticleName m_name;

};

CLASS_DEF( TBIdentifiedParticle, 78364895,1 )
//           TBIdentifiedParticle
#endif

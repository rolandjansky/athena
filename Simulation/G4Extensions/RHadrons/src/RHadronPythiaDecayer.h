/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHadronPythiaDecayer_H
#define RHadronPythiaDecayer_H

#include "G4VExtDecayer.hh"
#include "G4Track.hh"
#include <string>


#include "AthenaBaseComps/AthMessaging.h"


class G4DecayProducts;

class RHadronPythiaDecayer: public G4VExtDecayer, public AthMessaging
{
  public:
   RHadronPythiaDecayer( const std::string s , bool usePythia8=false );
   virtual G4DecayProducts* ImportDecayProducts(const G4Track&);
  private:
   bool m_usePythia8;
};

#endif

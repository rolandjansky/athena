/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_softdropobservables_header
#define jetsubstructureutils_softdropobservables_header

#include "JetSubStructureUtils/SubstructureCalculator.h"
#include "fastjet/contrib/SoftDrop.hh"

namespace JetSubStructureUtils {

  class SoftDropObservables : public SubstructureCalculator<double>{
    public:
      SoftDropObservables(std::string observableName) {
        m_observableName = observableName;
      };

      using SubstructureCalculator::result;
      virtual double result(const fastjet::PseudoJet &jet) const {
        // Set zcut to zero so that this doesn't groom the jet, just gives it the structure
        fastjet::contrib::SoftDrop sd(0., 0.); //beta, zcut
        // Apply the fake grooming to the jet
        fastjet::PseudoJet sdJet = sd(jet);
 
        double zg=sdJet.structure_of<fastjet::contrib::SoftDrop>().symmetry();
        double rg=sdJet.structure_of<fastjet::contrib::SoftDrop>().delta_R();
        if( strcmp(m_observableName.c_str(), "zg") == 0){
          return zg;
        }
        return rg;
      };
    private:
      std::string m_observableName; // Currently, able to return zg and rg

  };
}

#endif

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_multiplicities_header
#define jetsubstructureutils_multiplicities_header

#include "JetSubStructureUtils/SubstructureCalculator.h"

#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/tools/Recluster.hh"

namespace JetSubStructureUtils {

  class Multiplicities : public SubstructureCalculator<double>{
    public:
    Multiplicities(double beta=0., double zcut=0., double ktcut=0.) {
      m_beta = beta;
      m_zcut = zcut;
      m_ktcut = ktcut;
    };

    using SubstructureCalculator::result;
    virtual double result(const fastjet::PseudoJet &jet) const override {
      // apply desired soft drop to the jet: default values do no grooming, which is fine
      fastjet::contrib::SoftDrop sd(m_beta, m_zcut);
      // Apply the grooming to the jet
      fastjet::PseudoJet sdJet = sd(jet);
      
      int multiplicity=0;
      
      fastjet::JetDefinition jd(fastjet::cambridge_algorithm, fastjet::JetDefinition::max_allowable_R );
      fastjet::Recluster rc(jd);
      fastjet::PseudoJet j = rc.result(sdJet);
      
      fastjet::PseudoJet jj, j1, j2;
      jj = j;
      while (jj.has_parents(j1,j2)){
	if(j2.pt()*j1.delta_R(j2)>m_ktcut)
	  multiplicity++;
	if(multiplicity>500) // infinite loop failsafe
	  return -1;

	// follow harder branch
	jj = j1;
      }
      
      return multiplicity;
    };
  private:
    // soft drop parameters
    double m_beta; 
    double m_zcut;
    
    // les houches multiplcity cut
    double m_ktcut;
  };
}

#endif

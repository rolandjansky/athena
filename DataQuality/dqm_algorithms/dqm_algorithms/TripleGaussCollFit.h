/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_TRIPLEGAUSSCOLLFIT_H
#define DQM_ALGORITHMS_TRIPLEGAUSSCOLLFIT_H

#include <dqm_core/Algorithm.h>
#include <string>
#include <TF1.h>

namespace dqm_algorithms {
  
  class TripleGaussCollFit : public dqm_core::Algorithm {
    
  public:
    TripleGaussCollFit (const std::string& _name);
    ~TripleGaussCollFit(); 

    dqm_core::Result* execute (const std::string& s, const TObject& object, const dqm_core::AlgorithmConfig& cfg);
    using dqm_core::Algorithm::printDescription;
    void printDescription(std::ostream& out);
    TripleGaussCollFit* clone();

  private:
    std::string m_name;
    TF1* m_gaus3_fn;
    void fitSingle(TH1* hist1D);
    
  };  // end class
} //end namespace       


#endif // DQM_ALGORITHMS_TRIPLEGAUSSCOLLFIT_H

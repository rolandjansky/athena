/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file RootFit.h file declares the dqm_algorithms::RootFit  class.
 * \author Haleh Hadavand
*/

#ifndef DQM_ALGORITHMS_ROOTFIT_H
#define DQM_ALGORITHMS_ROOTFIT_H

#include <dqm_core/Algorithm.h>
#include <TF1.h>
#include <TCanvas.h>

namespace dqm_algorithms
{
  struct RootFit : public dqm_core::Algorithm
  {
    RootFit( const std::string & name );
	  
    ~RootFit();
    RootFit * clone( );
    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    using dqm_core::Algorithm::printDescription;
    void  printDescription(std::ostream& out);             
    private:
    std::string m_name;
    std::auto_ptr<TF1> m_func;	
  };
}

#endif // DQM_ALGORITHMS_ROOTFIT_H

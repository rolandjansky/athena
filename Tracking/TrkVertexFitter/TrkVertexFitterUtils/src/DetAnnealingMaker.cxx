/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          DetAnnealingMaker.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/DetAnnealingMaker.h"
#include <cmath>

namespace {
  double gauss(const double value2, const double error2) {
    return exp(-1./2.*value2/error2);
  }
}


namespace Trk
{
  
  DetAnnealingMaker::DetAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_SetOfTemperatures(1,3),
    m_cutoff(9.)
  {   
    declareProperty("SetOfTemperatures",     m_SetOfTemperatures);		    
    declareProperty("Cutoff", m_cutoff);    
  }

  StatusCode DetAnnealingMaker::initialize() 
  { 
    ATH_MSG_DEBUG( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }

  StatusCode DetAnnealingMaker::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful"  );
    return StatusCode::SUCCESS;
  }

  void DetAnnealingMaker::reset(AnnealingState& state) const
  {
    state = 0;
    ATH_MSG_DEBUG( "Annealing reset"  );
  }

  void DetAnnealingMaker::anneal(AnnealingState& state) const {

//check if there are some temperatures
    if (m_SetOfTemperatures.empty()) 
    {
      ATH_MSG_ERROR( "The vector SetOfTemperatures for the annealing is empty" );
    }
    //go one step further
    ++state;
  }

  double DetAnnealingMaker::getWeight(const AnnealingState& state,
                                      double chisq,
                                      const std::vector<double>& allchisq) const 
  {
    double temp = actualTemp (state);
    double allweight(0.);
    for (double ch : allchisq) {
      allweight+=gauss(ch,temp);
    }
    double actualweight(gauss(chisq,temp));
    return actualweight/(gauss(m_cutoff,temp)+allweight);
  }

  double DetAnnealingMaker::getWeight(const AnnealingState& state,
                                      double chisq) const
  {
    double temp = actualTemp (state);
    return gauss(chisq,temp)/(gauss(m_cutoff,temp)+gauss(chisq,temp));
  }

}

	

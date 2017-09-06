/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: KLFitterSelector.cxx 666816 2015-05-13 10:59:36Z morrisj $
#include "TopEventSelectionTools/KLFitterSelector.h"
#include "TopEvent/Event.h"
#include "TopEvent/KLFitterResult.h"

namespace top{
  
  KLFitterSelector::KLFitterSelector(const std::string& params) :
    SignValueSelector("KLFitter", params)
  {   
  }
  
  bool KLFitterSelector::apply(const top::Event& event) const
  {
    // check we have a xAOD::KLFitterResultContainer
    if (event.m_KLFitterResults == nullptr) {
      return false;
    }
    
    for (auto result : *event.m_KLFitterResults) {
      // check for errors
      if (
        result->minuitDidNotConverge() == 0 && 
        result->fitAbortedDueToNaN() == 0   &&
        result->atLeastOneFitParameterAtItsLimit() == 0 &&
        result->invalidTransferFunctionAtConvergence() == 0 
         ) 
      {
        bool passPermutation = checkFloat( result->logLikelihood() , value() );
        if (passPermutation) {
          return true;
        }
      } // check for errors
          
    } // Loop over Permutations - if we get this far, all Permutations have failed :(
    return false;
  }
}

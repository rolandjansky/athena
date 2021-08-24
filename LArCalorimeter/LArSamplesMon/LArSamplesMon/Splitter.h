/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamples::Splitter
   @brief Tools so split an ntuple into smaller bits
 */

#ifndef LArSamples_Splitter_H
#define LArSamples_Splitter_H

#include "LArCafJobs/CaloId.h"
#include "TString.h"

namespace LArSamples {
  
  class Interface;
  
  class Splitter  {
  
    public:
   
      /** @brief Constructor  */
      Splitter(const Interface& interface) : m_interface(&interface) { }
    
      bool splitEvents(const TString& name, unsigned int max = 20) const;
      
    private:
      
      const Interface* m_interface;
  };
}
  
#endif

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ParameterSpace.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#ifndef __TopoCore__ParameterSpace__
#define __TopoCore__ParameterSpace__

#include <iostream>
#include <vector>

#include "ParameterArray.h"

namespace TCS {

   class ParameterSpace {
   public:
      typedef std::vector<TCS::ParameterArray>  data_t;
      typedef data_t::iterator                  iterator;
      typedef data_t::const_iterator            const_iterator;
      
      ParameterSpace() {}
      
      // adds a new Parameter, throws TCS::Exception if parameter exists
      // isDefault flag will be set to true
      ParameterArray & addParameterArray(const ParameterArray & p);
      ParameterArray & addParameterArray(const std::string & algName);
      
      bool contains(const std::string & algName) const;

      TCS::ParameterArray & parameters(const std::string & algName);
      const TCS::ParameterArray & parameters(const std::string & algName) const;
      
      data_t & operator()() { return m_parameterArrays; }
      const data_t & operator()() const { return m_parameterArrays; }

   private:
      data_t m_parameterArrays;
      
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__ParameterSpace__) */

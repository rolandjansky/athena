/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ParameterArray.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//
//  Container for the list of parameters that are used by a single topological algorithms

#ifndef __TopoCore__ParameterArray__
#define __TopoCore__ParameterArray__

#include <iostream>
#include <vector>
#include <string>
#include "L1TopoCoreSimulation/Parameter.h"

namespace TCS {
   
   class ParameterArray {
   public:
      
      typedef std::vector<TCS::Parameter>  data_t;
      typedef data_t::iterator             iterator;
      typedef data_t::const_iterator       const_iterator;
      
      // default constructor
      ParameterArray(const std::string & algName) :
         m_algName(algName)
      {};
      ~ParameterArray(){};
      
      const std::string & algName() const { return m_algName; }
      
      // adds a new Parameter, throws TCS::Exception if parameter exists
      // isDefault flag will be set to true
      ParameterArray & addParameter(const Parameter & p);
      ParameterArray & addParameter(const std::string & name, TCS::parType_t value);
      
      // sets a parameter value, throws TCS::Exception if parameter doesn't exist
      // sets isDefault flag to false
      ParameterArray & setParameter(const Parameter & p);
      ParameterArray & setParameter(const std::string & name, TCS::parType_t value);
      
      const Parameter & parameter(const std::string & parameterName) const;
      
      // checks and returns true if a parameter of name parameterName already exists
      bool contains(const std::string & parameterName) const;
      
      void clear() { m_parameters.clear(); }
      
      iterator begin() { return m_parameters.begin(); }
      const_iterator begin() const { return m_parameters.begin(); }
      
      iterator end() { return m_parameters.end(); }
      const_iterator end() const { return m_parameters.end(); }
      
      data_t & operator()() { return m_parameters; }
      const data_t & operator()() const { return m_parameters; }
      
   private:
      // list of parameters for algorithm
      data_t m_parameters;
      
      // name of the algorithm that owns these parameters
      std::string m_algName;
      
   };
   
} // end of namespace TCS

std::ostream & operator<<(std::ostream &, const TCS::ParameterArray &);

#endif /* defined(__TopoCore__ParameterArray__) */

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ParameterSpace.h
//  TopoCore
//  Created by Joerg Stelzer on 11/18/12.

#ifndef __TopoCore__ParameterSpace__
#define __TopoCore__ParameterSpace__

#include <iostream>
#include <vector>

#include "Parameter.h"

namespace TCS {

   class ParameterSpace {
   public:
      typedef std::vector<TCS::Parameter>  data_t;
      typedef data_t::iterator             iterator;
      typedef data_t::const_iterator       const_iterator;
      
      ParameterSpace(const std::string & algName) :
         m_index(0),
         m_algName(algName)
      {};

      ParameterSpace() :
         m_index(0),
         m_algName("NONE")
      {};

      bool isInitialized() const { return m_algName != "NONE"; }

      const std::string & algName() const { return m_algName; }

      // adds a new Parameter, throws TCS::Exception if parameter exists
      // isDefault flag will be set to true
      ParameterSpace & addParameter(const Parameter & p);
      ParameterSpace & addParameter(const std::string & name, TCS::parType_t value);
      ParameterSpace & addParameter(const std::string & name, TCS::parType_t value, unsigned int selection);
      
      // sets a parameter value, throws TCS::Exception if parameter doesn't exist
      // sets isDefault flag to false
      ParameterSpace & setParameter(const Parameter & p);
      ParameterSpace & setParameter(const std::string & name, TCS::parType_t value);
      ParameterSpace & setParameter(const std::string & name, TCS::parType_t value, unsigned int selection);

      // access to single parameter
      const Parameter & parameter(const std::string & parameterName) const;
      
      // access to a multi-selection parameter by index
      const Parameter & parameter(const std::string & parameterName, unsigned int selection) const;

      // access to all parameters
      const std::vector<TCS::Parameter> & parameters() const { return m_parameters; }

      // checks and returns true if a parameter of name parameterName already exists
      bool contains(const std::string & parameterName, unsigned int selection) const;
      
      void clear() { m_parameters.clear(); }

      iterator begin() { return m_parameters.begin(); }
      const_iterator begin() const { return m_parameters.begin(); }

      iterator end() { return m_parameters.end(); }
      const_iterator end() const { return m_parameters.end(); }

      data_t & operator()() { return m_parameters; }
      const data_t & operator()() const { return m_parameters; }

      unsigned index() const { return m_index; }
      void setIndex(unsigned int index) { m_index = index; }

   private:
      data_t m_parameters;
      unsigned int m_index;

      // name of the algorithm that owns these parameters
      std::string m_algName;

   };
   
std::ostream & operator<<(std::ostream &, const TCS::ParameterSpace &);

}

#endif

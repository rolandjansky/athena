/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ParameterArray.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#include "L1TopoCoreSimulation/ParameterArray.h"
#include "L1TopoCoreSimulation/Exception.h"

using namespace std;

// adds a new Parameter, throws TCS::Exception if parameter exists
// isDefault flag will be set to true
TCS::ParameterArray &
TCS::ParameterArray::addParameter(const TCS::Parameter& p) {
   if( contains(p.name()) ) {
      TCS_EXCEPTION("Parameter '" << p.name() << "' exists for in parameter array '" << algName() << "' and can't be added");
   }
   m_parameters.push_back(p);
   m_parameters.back().setIsDefault(true);
   return *this;
}

TCS::ParameterArray &
TCS::ParameterArray::addParameter(const std::string & name, TCS::parType_t value) {
   return addParameter(Parameter(name, value));
}

// sets a parameter value, throws TCS::Exception if parameter doesn't exist
// sets isDefault flag to false
TCS::ParameterArray &
TCS::ParameterArray::setParameter(const Parameter & p) {
   iterator pa = begin();
   for(; pa != end(); ++pa) {
      if( pa->name() == p.name()) {
         pa->setValue(p.value());
         pa->setIsDefault(false);
         break;
      }
   }
   if( pa==end() ) {
      TCS_EXCEPTION("Parameter '" << p.name() << "' not found for algorithm " << algName() << " and can't be set.");
   }
   return *this;
}

TCS::ParameterArray &
TCS::ParameterArray::setParameter(const std::string & name, TCS::parType_t value) {
   return setParameter(Parameter(name, value));
}

bool
TCS::ParameterArray::contains(const std::string & parameterName) const {
   for(const_iterator pa = begin(); pa != end(); ++pa) {
      if( pa->name() == parameterName) return true;
   }
   return false;
}

const TCS::Parameter &
TCS::ParameterArray::parameter(const std::string & parameterName) const {
   const_iterator pa = begin();
   for(; pa != end(); ++pa) {
      if( pa->name() == parameterName) break;
   }
   if( pa==end() ) {
      TCS_EXCEPTION("Parameter " << parameterName << " not found for algorithm " << algName());
   }
   return *pa;
}


std::ostream &
operator<<(std::ostream &o, const TCS::ParameterArray & pa) {
   o << "parameters for algorithm: " << pa.algName() << endl;
   for(TCS::ParameterArray::const_iterator parameter = pa.begin(); parameter != pa.end(); ++parameter) {
      if(parameter != pa.begin()) o << endl;
      o << *parameter;
   }
   return o;
}

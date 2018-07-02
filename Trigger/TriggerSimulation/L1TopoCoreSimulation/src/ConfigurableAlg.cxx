/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  BaseTOBoAlg.cpp
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include "L1TopoCoreSimulation/ConfigurableAlg.h"
#include "L1TopoCoreSimulation/Exception.h"
#include <iostream>

using namespace std;

TCS::ConfigurableAlg::ConfigurableAlg(const  std::string & name) :
   m_name(name),
   m_parameters(name)
{}

void
TCS::ConfigurableAlg::setParameter(const Parameter & parameter) {
   m_parameters.setParameter(parameter);
}

void
TCS::ConfigurableAlg::setParameter(const std::string & name, TCS::parType_t value) {
   setParameter(Parameter(name,value));
}

void
TCS::ConfigurableAlg::setParameters(const ParameterArray & parameters) {
   if(name() != parameters.algName()) {
      TCS_EXCEPTION("Name of parameter set (" << parameters.algName() << ") do not match configurable name (" << this->name() << ")" );
   }
   for(ParameterArray::const_iterator p = parameters.begin(); p!= parameters.end(); ++p) {
      setParameter(*p);
   }
}

// define parameters by developer
void
TCS::ConfigurableAlg::defineParameter(const std::string & name, TCS::parType_t value) {
   defineParameter(Parameter(name, value));
}

void
TCS::ConfigurableAlg::defineParameter(const Parameter & parameter) {
   if( parameters().contains( parameter.name() ) ) {
      TCS_EXCEPTION("Can't define parameter '" << parameter.name() << "' for algorithm '" << name() << "' since the parameter already exists");
   }
   m_parameters.addParameter(parameter);
}



const TCS::Parameter &
TCS::ConfigurableAlg::parameter(const std::string & parameterName) const {
   return parameters().parameter(parameterName);
}

std::ostream &
operator<<(std::ostream & o, const TCS::ConfigurableAlg & alg) {
   o << "algorithm '" << alg.name() << "'" << endl;
   o << alg.parameters();
   return o;
}


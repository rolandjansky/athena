//  BaseTOBoAlg.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"
#include <iostream>

using namespace std;
using namespace TCS;


ConfigurableAlg::ConfigurableAlg(const  std::string & name, AlgType algType) :
   TrigConfMessaging(name),
   m_name(name),
   m_parameters(name),
   m_algType(algType)
{}


ConfigurableAlg::~ConfigurableAlg()
{}

// define parameters to be called by developer
void
ConfigurableAlg::defineParameter(const std::string & name, TCS::parType_t value) {
   defineParameter( Parameter(name, value) );
}


void
ConfigurableAlg::defineParameter(const std::string & name, TCS::parType_t value, unsigned int selection) {
   defineParameter( Parameter(name, value, selection) );
}


void
ConfigurableAlg::defineParameter(const Parameter & parameter) {
   if( parameters().contains( parameter.name(), parameter.selection() ) ) {
      TCS_EXCEPTION("Duplicate parameter definition '" << parameter.name() << "' for algorithm '" << name() << "'");
   }
   m_parameters.addParameter(parameter);
}


void
ConfigurableAlg::setParameters(const ParameterSpace & parameters) {
   
   // check if parameterspace is for this algorithm
   if(name() != parameters.algName()) {
      TCS_EXCEPTION("Name of parameter set (" << parameters.algName() << ") do not match configurable name (" << name() << ")" );
   }

   // copy parameters
   for(const Parameter & p : parameters) {
      try {
         m_parameters.setParameter(p);
      }
      catch(TCS::Exception e) {
         TCS_EXCEPTION("Algorithm configuration failed for " << name() << "! Caught exception: " << e.what());
      }
   }
}



const TCS::Parameter &
ConfigurableAlg::parameter(const std::string & parameterName) const {
   return parameters().parameter(parameterName);
}


const TCS::Parameter &
ConfigurableAlg::parameter(const std::string & parameterName, unsigned int selection) const {
   return parameters().parameter(parameterName, selection);
}




std::ostream &
operator<<(std::ostream & o, const TCS::ConfigurableAlg & alg) {

   o << "algorithm '" << alg.fullname() << "'" << endl;
   o << alg.parameters();
   return o;

}


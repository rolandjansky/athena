/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  ParameterSpace.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/18/12.

#include "L1TopoInterfaces/ParameterSpace.h"
#include "L1TopoCommon/Exception.h"

using namespace std;
using namespace TCS;


ParameterSpace & 
ParameterSpace::addParameter(const std::string & name, TCS::parType_t value) {
   return addParameter( Parameter(name, value));
}


ParameterSpace & 
ParameterSpace::addParameter(const std::string & name, TCS::parType_t value, unsigned int selection) {
   return addParameter( Parameter(name, value, selection));
}


// adds a new Parameter, throws TCS::Exception if parameter exists isDefault flag will be set to true
ParameterSpace &
ParameterSpace::addParameter(const TCS::Parameter& p) {
   if( contains(p.name(), p.selection()) ) {
      TCS_EXCEPTION("ParameterSpace: parameter '" << p.name() << "|" << p.selection() <<
                    "' exists already in parameter space of '" << algName() << "' and can't be added");
   }
   m_parameters.push_back(p);
   m_parameters.back().setIsDefault(true);
   return *this;
}


ParameterSpace &
ParameterSpace::setParameter(const std::string & name, TCS::parType_t value, unsigned int selection) {
   return setParameter(Parameter(name, value, selection));
}


// sets a parameter value, throws TCS::Exception if parameter doesn't exist
// sets isDefault flag to false
ParameterSpace &
ParameterSpace::setParameter(const Parameter & p) {

   bool foundParameter = false;

   //cout << "Setting parameter " << p.name() << "[" << p.selection() << "]  for algorithm " << algName() << endl;
  
   for(TCS::Parameter& pa : m_parameters) {
      foundParameter =    
         ( !pa.isExtended() && pa.name()==p.name() ) || 
         ( pa.isExtended() && pa.name()==p.name() && pa.selection()==p.selection() );

      if(! foundParameter) continue;

      if( pa.isExtended() ) {
            pa.setValue(p.value());
            pa.setPosition(p.position());
            pa.setSelection(p.selection()); 
            pa.setIsExtended(true); // for parameters to print correctly
      } else { 
         pa.setValue(p.value());
         pa.setIsExtended(false);
      }
      pa.setIsDefault(false);
      break;
   }

   if( ! foundParameter ) {
      cout << "Parameter '" << p.name() << "' not found for algorithm " << algName() 
           << " and can't be set. Available parameters are" << endl;

      for(TCS::Parameter& pa : m_parameters) {
         cout << "   " << pa.name() << "   " << (pa.isExtended()?"array":"single") << endl;
      }
      TCS_EXCEPTION( "Parameter '" << p.name() << "' not found for algorithm " << algName() << " and can't be set." );
   }
   return *this;
}





bool
ParameterSpace::contains(const std::string & parameterName, unsigned int selection) const {
   for(const Parameter& pa: m_parameters)
      if( (pa.name() == parameterName) && (pa.selection() == selection) ) return true;
   return false;
}



const Parameter &
ParameterSpace::parameter(const std::string & parameterName) const {
   for(const Parameter & pa : m_parameters) {
      if( pa.isExtended() ) continue;
      if( pa.name() == parameterName ) 
         return pa;
   }

   TCS_EXCEPTION("Single parameter " << parameterName << " not found for algorithm " << algName());
   return m_parameters[0]; // doesn't make sense, but should never get here
}


const Parameter &
ParameterSpace::parameter(const std::string & parameterName, unsigned int selection) const {
   for(const Parameter & pa : m_parameters) {
      if( ! pa.isExtended() ) continue;
      if( (pa.name() == parameterName) && (pa.selection() == selection) ) 
         return pa;
   }

   TCS_EXCEPTION("Array parameter " << parameterName << "[" << selection << "] not found for algorithm " << algName());
   return m_parameters[0]; // doesn't make sense, but should never get here
}


namespace TCS {


std::ostream &
operator<<(std::ostream &o, const TCS::ParameterSpace & ps) {
   if( ps.isInitialized()) {
      o << "    parameters: " << ps().size();
      for(TCS::Parameter parameter : ps) {
         o << endl << "    " << parameter;
      }
   }
   return o;
}


}

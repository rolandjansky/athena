/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ParameterSpace.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/18/12.
//

#include "L1TopoCoreSimulation/ParameterSpace.h"
#include "L1TopoCoreSimulation/Exception.h"

TCS::ParameterArray &
TCS::ParameterSpace::addParameterArray(const ParameterArray & p) {
   if(contains(p.algName())) {
      TCS_EXCEPTION("Can't add ParameterArray " << p.algName() << ", since it already exists!");
   }
   m_parameterArrays.push_back(p);
   return m_parameterArrays.back();
}

TCS::ParameterArray &
TCS::ParameterSpace::addParameterArray(const std::string & algName) {
   if(contains(algName)) {
      TCS_EXCEPTION("Can't add ParameterArray " << algName << ", since it already exists!");
   }
   m_parameterArrays.push_back(ParameterArray(algName));
   return m_parameterArrays.back();
}

bool
TCS::ParameterSpace::contains(const std::string & algName) const {
   for(const_iterator parArr = m_parameterArrays.begin();
       parArr != m_parameterArrays.end(); ++parArr)
   {
      if(parArr->algName() == algName) {
         return true;
      }
   }
   return false;
}


TCS::ParameterArray &
TCS::ParameterSpace::parameters(const std::string & algName) {
   for(iterator parArr = m_parameterArrays.begin();
       parArr != m_parameterArrays.end(); ++parArr)
   {
      if(parArr->algName() == algName) {
         return *parArr;
      }
   }
   TCS_EXCEPTION("ParameterSpace does not contain parameters for algorithm name " << algName)
}

const TCS::ParameterArray &
TCS::ParameterSpace::parameters(const std::string & algName) const {
   for(const_iterator parArr = m_parameterArrays.begin();
       parArr != m_parameterArrays.end(); ++parArr)
   {
      if(parArr->algName() == algName) {
         return *parArr;
      }
   }
   TCS_EXCEPTION("ParameterSpace does not contain parameters for algorithm name " << algName)
}

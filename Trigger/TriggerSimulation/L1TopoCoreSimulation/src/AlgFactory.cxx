/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  AlgFactory.cxx
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#include "L1TopoCoreSimulation/AlgFactory.h"
#include "L1TopoCoreSimulation/Exception.h"

#include "L1TopoCoreSimulation/ClusterSortingAlg.h"

TCS::AlgFactory* TCS::AlgFactory::fg_instance = 0;

TCS::AlgFactory&
TCS::AlgFactory::instance()
{
   if (!fg_instance) fg_instance = new TCS::AlgFactory();
   return *fg_instance;
}



TCS::ConfigurableAlg *
TCS::AlgFactory::create(const std::string & algType, const std::string & algName) {

   if(instance().algorithm(algName)) {
      TCS_EXCEPTION("AlgFactory: algorithm " << algName << " already exists. Serious configuration error.")
   }


   // find creator function
   CallMap::const_iterator it = instance().m_callMap.find(algType);
   
   // handle unknown algorithm request
   if (it == instance().m_callMap.end()) {
      TCS_EXCEPTION("TCS::AlgFactory: trying to instantiate algorithm of type " << algType << " with name " << algName << ", but it has not been registered.");
   }

   TCS::ConfigurableAlg * createdAlg = (it->second)(algName);

   instance().m_algs[algName] = createdAlg;

   return createdAlg;
}


bool
TCS::AlgFactory::Register( const std::string &name, Creator creator ) {
   // registers a algorithm creator function under the algorithm class name

   if(m_callMap.find(name) != m_callMap.end()) {
      TCS_EXCEPTION ("TCS::AlgFactory: registration of algorithm " << name << " failed since it already exists");
      return false;
   }
   return m_callMap.insert(CallMap::value_type(name, creator)).second;
}


bool
TCS::AlgFactory::Unregister( const std::string &name )
{
   // unregisters an algorithm class name
   return m_callMap.erase(name) == 1;
}


TCS::ConfigurableAlg *
TCS::AlgFactory::algorithm(const std::string & algName) {
   AlgMap_t::iterator findRes = m_algs.find(algName);

   if(findRes == m_algs.end())
      return (TCS::ConfigurableAlg*)0;

   return findRes->second;
}


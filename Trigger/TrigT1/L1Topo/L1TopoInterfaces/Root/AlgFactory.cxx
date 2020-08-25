// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include "L1TopoInterfaces/AlgFactory.h"
#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"

using namespace std;

TCS::AlgFactory* TCS::AlgFactory::fg_instance = 0;

TCS::AlgFactory&
TCS::AlgFactory::instance()
{
   if (!fg_instance) fg_instance = new TCS::AlgFactory();
   return *fg_instance;
}

void
TCS::AlgFactory::destroy_instance()
{
   delete fg_instance;
   fg_instance = nullptr;
}



TCS::AlgFactory::~AlgFactory() {
   for(auto x: m_algs)
      delete x.second;
}


TCS::ConfigurableAlg *
TCS::AlgFactory::create(const std::string & algType, const std::string & algName) {

   if( instance().algorithm(algName) ) {
      TCS_EXCEPTION("AlgFactory: algorithm " << algName << " already exists. Serious configuration error.")
   }

   // find creator function
   CallMap::const_iterator it = instance().m_callMap.find(algType);
   
   // handle unknown algorithm request
   if (it == instance().m_callMap.end()) {
      cout << "Registered algorithms are:" << endl;
      for(const auto & x: instance().m_callMap)
         cout << "   " << x.first << endl;
      TCS_EXCEPTION("AlgFactory: trying to instantiate algorithm of type " << algType << " with name " << algName << ", but it has not been registered.");
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


vector<string>
TCS::AlgFactory::getAllClassNames() const {
   vector<string> classNames; 
   classNames.reserve(m_callMap.size());
   for(const auto& x : m_callMap)
      classNames.push_back(x.first);
   return classNames;
}

TCS::ConfigurableAlg *
TCS::AlgFactory::algorithm(const std::string & algName) {
   AlgMap_t::iterator findRes = m_algs.find(algName);

   if(findRes == m_algs.end())
      return (TCS::ConfigurableAlg*)0;

   return findRes->second;
}


void
TCS::AlgFactory::PrintAlgorithmNames() {
   for(AlgMap_t::value_type entry : instance().m_algs) {
      cout << entry.first << " --> " << entry.second->fullname() << endl;
   }
}

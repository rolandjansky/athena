// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#include "L1TopoInterfaces/AlgFactory.h"
#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"

const TCS::AlgFactory&
TCS::AlgFactory::instance()
{
   static const TCS::AlgFactory factory;
   return factory;
}

TCS::AlgFactory&
TCS::AlgFactory::mutable_instance ATLAS_NOT_THREAD_SAFE()
{
   auto nc_instance = const_cast<TCS::AlgFactory*>(&instance());
   return *nc_instance;
}

TCS::ConfigurableAlg *
TCS::AlgFactory::create(const std::string & algType, const std::string & algName) ATLAS_NOT_THREAD_SAFE {

   if( algorithm(algName) ) {
      TCS_EXCEPTION("AlgFactory: algorithm " << algName << " already exists. Serious configuration error.")
   }

   // find creator function
   const auto& it = m_callMap.find(algType);
   
   // handle unknown algorithm request
   if (it == m_callMap.end()) {
      std::cout << "Registered algorithms are:" << std::endl;
      for(const auto & x: m_callMap)
         std::cout << "   " << x.first << std::endl;
      TCS_EXCEPTION("AlgFactory: trying to instantiate algorithm of type " << algType << " with name " << algName << ", but it has not been registered.");
   }

   auto createdAlg = (it->second)(algName);
   auto [itr,inserted] = m_algs.emplace(algName, std::move(createdAlg));

   return itr->second.get();
}


bool
TCS::AlgFactory::Register( const std::string &algType, Creator creator ) {
   const auto& [itr, inserted] = m_callMap.emplace(algType, creator);
   // registers a algorithm creator function under the algorithm class name
   if (!inserted) {
      TCS_EXCEPTION ("TCS::AlgFactory: registration of algorithm " << algType << " failed since it already exists");
   }
   return inserted;
}

std::vector<std::string>
TCS::AlgFactory::getAllClassNames() const {
   std::vector<std::string> classNames;
   classNames.reserve(m_callMap.size());
   for (const auto& [key, _] : m_callMap) {
      classNames.push_back(key);
   }
   return classNames;
}

TCS::ConfigurableAlg *
TCS::AlgFactory::algorithm(const std::string & algName) {
   auto findRes = m_algs.find(algName);

   if(findRes == m_algs.end())
      return nullptr;

   return findRes->second.get();
}


void
TCS::AlgFactory::printAlgorithmNames() const {
   for(const auto& [name, alg] : m_algs) {
      std::cout << name << " --> " << alg->fullname() << std::endl;
   }
}

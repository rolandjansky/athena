/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  AlgFactory.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/20/12.
//

#ifndef __L1TopoCoreSimulation__AlgFactory__
#define __L1TopoCoreSimulation__AlgFactory__

#include <iostream>
#include <map>

namespace TCS {
   
   class ConfigurableAlg;
   
   class AlgFactory {
   public:
      // function type to create an algorithm
      typedef TCS::ConfigurableAlg* (*Creator)(const std::string & methodName);

      static TCS::AlgFactory & instance();
      
      static ConfigurableAlg * create(const std::string & methodType, const std::string & methodName);
      
      bool Register( const std::string &name, Creator creator );
      bool Unregister( const std::string &name );

      ConfigurableAlg * algorithm(const std::string & algName);

   private:
      typedef std::map<std::string, Creator> CallMap;

      typedef std::map<std::string, TCS::ConfigurableAlg*> AlgMap_t;

      CallMap m_callMap;

      AlgMap_t m_algs;
      
      static AlgFactory * fg_instance;
   };
   
}

#define REGISTER_ALG(CLASS) \
   namespace { \
      TCS::ConfigurableAlg* Create##CLASS(const std::string & name) { \
         return (TCS::ConfigurableAlg*) new #CLASS(name); \
      } \
      \
      bool success = TCS::AlgFactory::Register(#CLASS, Create##CLASS); \
   }

#define REGISTER_ALG_NS(NS,CLASS) \
   namespace { \
      TCS::ConfigurableAlg* Create##CLASS(const std::string & methodName) { \
         return (TCS::ConfigurableAlg*) new NS::CLASS(methodName); \
      } \
      \
      bool success = TCS::AlgFactory::instance().Register(#CLASS, Create##CLASS); \
   }



#endif /* defined(__L1TopoCoreSimulation__AlgFactory__) */

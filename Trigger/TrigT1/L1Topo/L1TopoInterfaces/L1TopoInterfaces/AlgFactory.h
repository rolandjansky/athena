// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#ifndef L1TopoInterfaces_AlgFactory
#define L1TopoInterfaces_AlgFactory

#include "CxxUtils/checker_macros.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace TCS {
   
   class ConfigurableAlg;
   
   class AlgFactory {
   public:
      /// function type to create an algorithm
      typedef std::unique_ptr<ConfigurableAlg> (*Creator)(const std::string & methodName);

      static const AlgFactory& instance();                          //!< read-only access
      static AlgFactory& mutable_instance ATLAS_NOT_THREAD_SAFE();  //!< non-const access

      /// Register creator function for algorithm type
      bool Register( const std::string &algType, Creator creator );

      /// Create algorithm of given type and name
      ConfigurableAlg* create(const std::string & algType, const std::string & algName);

      /// Retrieve algorithm by name
      ConfigurableAlg* algorithm(const std::string & algName);

      std::vector<std::string> getAllClassNames() const;
      void printAlgorithmNames() const;

   private:
      AlgFactory() = default;   //<! singleton created via instance()
      AlgFactory(const AlgFactory&) = delete;             //!< no copy
      AlgFactory& operator=(const AlgFactory&) = delete;  //!< no assign

      std::map<std::string, Creator>                               m_callMap;
      std::map<std::string, std::unique_ptr<TCS::ConfigurableAlg>> m_algs;
   };
   
}

// Macro to register algorithm in arbitrary namespace
#define REGISTER_ALG_NS(NS,CLASS)                                       \
   namespace {                                                          \
      std::unique_ptr<TCS::ConfigurableAlg> Create##NS__##CLASS(const std::string & methodName) { \
         auto alg = std::make_unique<NS::CLASS>(methodName); \
         alg->setClassName( std::string(#NS) + "__" + #CLASS );         \
         alg->msg().setName( alg->fullname() );                         \
         return alg;                                                    \
      }                                                                 \
                                                                        \
      const bool success = TCS::AlgFactory::mutable_instance().Register( std::string(#NS) + "__" + #CLASS, Create##NS__##CLASS); \
   }


// Macro to register algorithm in TCS namespace
#define REGISTER_ALG_TCS(CLASS)                                         \
   namespace {                                                          \
      std::unique_ptr<TCS::ConfigurableAlg> Create##CLASS(const std::string & methodName) { \
         auto alg = std::make_unique<TCS::CLASS>(methodName); \
         alg->setClassName( #CLASS );                                   \
         alg->msg().setName(  alg->fullname() );                        \
         return alg;                                                    \
      }                                                                 \
                                                                        \
      const bool success = TCS::AlgFactory::mutable_instance().Register( #CLASS, Create##CLASS); \
   }


#endif

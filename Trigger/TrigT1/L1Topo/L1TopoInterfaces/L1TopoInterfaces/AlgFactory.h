// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#ifndef L1TopoInterfaces_AlgFactory
#define L1TopoInterfaces_AlgFactory

#include <string>
#include <vector>
#include <map>

namespace TCS {
   
   class ConfigurableAlg;
   
   class AlgFactory {
   public:

      /**@brief: destructor
       * destroys all algorithms
       */
      ~AlgFactory();

      // function type to create an algorithm
      typedef ConfigurableAlg* (*Creator)(const std::string & methodName);

      static AlgFactory & instance();

      static void destroy_instance();

      static ConfigurableAlg * create(const std::string & algType, const std::string & algName);
      
      bool Register( const std::string &name, Creator creator );
      bool Unregister( const std::string &name );

      ConfigurableAlg * algorithm(const std::string & algName);

      std::vector<std::string> getAllClassNames() const;

      static void PrintAlgorithmNames();

   private:
      typedef std::map<std::string, Creator> CallMap;

      typedef std::map<std::string, TCS::ConfigurableAlg*> AlgMap_t;

      CallMap m_callMap;

      AlgMap_t m_algs;
      
      static thread_local AlgFactory * fg_instance;
   };
   
}

#define REGISTER_ALG(CLASS) \
   namespace {                                                          \
      TCS::ConfigurableAlg* Create##CLASS(const std::string & name) {   \
         TCS::ConfigurableAlg* alg = (TCS::ConfigurableAlg*) new #CLASS(name); \
         alg->setClassName(#CLASS);                                     \
         alg->msg().setName( alg->fullname() );                         \
         return alg;                                                    \
      }                                                                 \
                                                                        \
      bool success = TCS::AlgFactory::instance().Register(#CLASS, Create##CLASS); \
   }


#define REGISTER_ALG_NS(NS,CLASS)                                       \
   namespace {                                                          \
      TCS::ConfigurableAlg* Create##CLASS(const std::string & methodName) { \
         TCS::ConfigurableAlg* alg = (TCS::ConfigurableAlg*) new NS::CLASS(methodName); \
         alg->setClassName( std::string(#NS) + "__" + #CLASS );         \
         alg->msg().setName( alg->fullname() );                         \
         return alg;                                                    \
      }                                                                 \
                                                                        \
      bool success = TCS::AlgFactory::instance().Register( std::string(#NS) + "__" + #CLASS, Create##CLASS); \
   }


#define REGISTER_ALG_TCS(CLASS)                                         \
   namespace {                                                          \
      TCS::ConfigurableAlg* Create##CLASS(const std::string & methodName) { \
         TCS::ConfigurableAlg* alg = (TCS::ConfigurableAlg*) new TCS::CLASS(methodName); \
         alg->setClassName( #CLASS );                                   \
         alg->msg().setName(  alg->fullname() );                        \
         return alg;                                                    \
      }                                                                 \
                                                                        \
      bool success = TCS::AlgFactory::instance().Register( #CLASS, Create##CLASS); \
   }


#endif

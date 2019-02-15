/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/versions/JetAccessorMap_v1.h"

namespace xAOD {
 
  namespace JetAttributeAccessor {

    /// IMPORTANT : this has to be kept in sync with JetAttribute::AssoParticlesID
    // const char* JetAssoParticleAccessors_v1::s_map[] = {
    //   "GhostTracks",
    //   "GhostTruthParticles",
    // };

    // we define a dummy accessor class
    namespace {      class Dummy {};       }

    template<>
    class AccessorWrapper<Dummy> : public Named {
    public:
      AccessorWrapper(const std::string&n) : Named(n){}
    };


    std::string name_v1(xAOD::JetAttribute::AttributeID id ) {
      // no specialization for now. Use dummy to avoid any conflict
      const AccessorWrapper<Dummy>* acc = AccessorMap_v1<Dummy>::accessor(id);
      if(acc) return acc->name();
      return "UknownAttribute";
    }


    template<>
    class ObjectAccessorWrapper<Dummy> : public Named {
    public:
      ObjectAccessorWrapper(const std::string&n) : Named(n){}
    };

    std::string assoName_v1(xAOD::JetAttribute::AssoParticlesID id )  {
      // no specialization for now. Use dummy to avoid any conflict
      const ObjectAccessorWrapper<Dummy>* acc = ObjectAccessorMap_v1<Dummy>::accessor(id);
      if(acc) return acc->name();
      return "UknownAssoObject";
    }

  }



}

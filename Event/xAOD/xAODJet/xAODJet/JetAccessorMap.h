// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETACCESSORMAP_H
#define XAODJET_JETACCESSORMAP_H
////////////////////////////////////////////////
///////////////////////////////////////////////
/// This header defines functions to retrieve a static accessor object
/// for an attribute given an the enum correspondig to this attribute
/// 
#include "xAODJet/versions/JetAccessorMap_v1.h"

namespace xAOD {
  

  // can't forward a template in c++-03 : use a templated function.
  namespace JetAttributeAccessor {
    ////////////////////////////////////////////////
    /// Returns an attribute accessor corresponding to an AttributeID.
    /// returns null if invalid AttributeID.
    ////////////////////////////////////////////////
    template<class T> 
    const AccessorWrapper<T>* accessor(xAOD::JetAttribute::AttributeID id ) {
      return AccessorMap_v1<T>::accessor(id);
    }
    
    inline std::string name(xAOD::JetAttribute::AttributeID id ) {
      return name_v1(id);
    }

  }

}

#endif

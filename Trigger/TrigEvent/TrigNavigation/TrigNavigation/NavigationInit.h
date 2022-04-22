/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavigation_NavigationInit_h
#define TrigNavigation_NavigationInit_h

  /**
   * @brief Classes used to provide static Trigger EDM list
   *
   * This code is from Scott Snyder
   */

#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include "AthLinks/ElementLink.h"
#include "AthLinks/DataLink.h"
#include "CxxUtils/checker_macros.h"

#include "TrigNavigation/NavigationTraits.h"
#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"

namespace HLT{  
  template <class FEATURE, class CONTAINER >
  struct FeatureContainerInit {
    FeatureContainerInit() ATLAS_CTORDTOR_NOT_THREAD_SAFE;
    void null() const {;}
  };

  template <class FEATURE, class CONTAINER > 
  struct RegisterFeatureContainerTypes {    
    static void instan() {s.null();}
    static const FeatureContainerInit<FEATURE, CONTAINER> s;
  };  
  template <class FEATURE, class CONTAINER>
  const FeatureContainerInit<FEATURE, CONTAINER> RegisterFeatureContainerTypes<FEATURE, CONTAINER>::s;


  template <class TYPE >
  struct AuxInit {
    AuxInit() ATLAS_CTORDTOR_NOT_THREAD_SAFE;
    void null() const {;}
  };

  template <class TYPE > 
  struct RegisterAuxType {    
    static void instan() {s.null();}
    static const AuxInit<TYPE> s;
  };  
  template <class TYPE>
  const AuxInit<TYPE> RegisterAuxType<TYPE>::s;
}

#include "TrigNavigation/NavigationInit.icc"

#endif

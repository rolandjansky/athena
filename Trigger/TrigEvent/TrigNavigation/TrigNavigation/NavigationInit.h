/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigNavigation_NavigationInit_h
#define TrigNavigation_NavigationInit_h

  /**
   * @brief Classes used to provide static Trigger EDM list
   *
   * This code is from Scott Snyder
   */

#include "AthLinks/ElementLink.h"
#include "AthLinks/DataLink.h"

#include "TrigNavigation/NavigationTraits.h"
#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"

namespace HLT{  
  template <class FEATURE, class CONTAINER >
  struct FeatureContainerInit {
    FeatureContainerInit();
    void null(){;}
  };

  template <class FEATURE, class CONTAINER > 
  struct RegisterFeatureContainerTypes {    
    static void instan(){s.null();}
    static FeatureContainerInit<FEATURE, CONTAINER> s; 
  };  
  template <class FEATURE, class CONTAINER> FeatureContainerInit<FEATURE, CONTAINER> RegisterFeatureContainerTypes<FEATURE, CONTAINER>::s;


  template <class TYPE >
  struct AuxInit {
    AuxInit();
    void null(){;}
  };

  template <class TYPE > 
  struct RegisterAuxType {    
    static void instan(){s.null();}
    static AuxInit<TYPE> s; 
  };  
  template <class TYPE> AuxInit<TYPE> RegisterAuxType<TYPE>::s;
}

#include "TrigNavigation/NavigationInit.icc"

#endif

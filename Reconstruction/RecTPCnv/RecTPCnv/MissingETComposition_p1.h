/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITION_P1_H
#define RECTPCNV_MISSINGETCOMPOSITION_P1_H

// DataModelAthenaPool includes
#include "DataModelAthenaPool/Navigable_p2.h"

namespace  MissingETHandlers {
   class Weight;
}

class MissingETComposition_p1
{
 public:
  
  struct Weight_p1 {
    Weight_p1() : wx(0), wy(0), wet(0), statusWord(0) { }
    Weight_p1(const MissingETHandlers::Weight& w);
    float wx;
    float wy;
    float wet;
    unsigned short int statusWord;
  }; 
  
  template <class T>
    friend class MissingETCompositionConverterBase;
  
  MissingETComposition_p1() { };
  virtual ~MissingETComposition_p1() = default;
  
  //GeneratePersNavigableType_p2<MissingETComposition, Weight_p1>::type m_nav;
  Navigable_p2<unsigned int, MissingETComposition_p1::Weight_p1>         m_nav;
};

#endif


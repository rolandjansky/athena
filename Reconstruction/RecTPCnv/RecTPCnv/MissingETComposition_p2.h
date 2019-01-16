/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITION_P2_H
#define RECTPCNV_MISSINGETCOMPOSITION_P2_H

// DataModelAthenaPool includes
#include "DataModelAthenaPool/Navigable_p2.h"

  
namespace  MissingETHandlers {
   class Weight;
}

struct Weight_p1 {
    Weight_p1() {  w[0]=0; w[1]=0; w[2]=0; statusWord=0; }
    Weight_p1(const MissingETHandlers::Weight& we);
    float w[3];
    unsigned short int statusWord;
};

class MissingETComposition_p2
{
 public:
  
  template <class T>
    friend class MissingETCompositionConverterBase;
  
  MissingETComposition_p2() { };
  virtual ~MissingETComposition_p2() = default;
  
  //GeneratePersNavigableType_p2<MissingETComposition, Weight_p1>::type m_nav;
  Navigable_p2<unsigned int, Weight_p1>         m_nav;
};

#endif

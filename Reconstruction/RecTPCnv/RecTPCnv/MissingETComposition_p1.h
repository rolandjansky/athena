/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECTPCNV_MISSINGETCOMPOSITION_P1_H
#define RECTPCNV_MISSINGETCOMPOSITION_P1_H

#include "CLIDSvc/CLASS_DEF.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/Navigable_p2.h"

// needed for the navigation weight
#include "MissingETEvent/MissingETComposition.h"

class MissingETComposition_p1
{
 public:
  
  struct Weight_p1 {
  Weight_p1() : wx(0), wy(0), wet(0), statusWord(0) { };
    Weight_p1(const MissingETHandlers::Weight& w) : wx(w.wpx()), wy(w.wpy()), wet(w.wet()), statusWord(w.statusWord()) { };
    float wx;
    float wy;
    float wet;
    unsigned short int statusWord;
  }; 
  
  template <class T>
    friend class MissingETCompositionConverterBase;
  
  MissingETComposition_p1() { };
  virtual ~MissingETComposition_p1() { };
  
  GeneratePersNavigableType_p2<MissingETComposition, Weight_p1>::type m_nav;
};

#endif

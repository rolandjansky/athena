/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOMBINEDEVENTTPCNV_TRIGCOMBINEDEVENTTPCNVDICT1_H
#define TRIGCOMBINEDEVENTTPCNV_TRIGCOMBINEDEVENTTPCNVDICT1_H
     
#include "TrigCombinedEventTPCnv/TrigCompositeContainer_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeContainerCnv_p1.h"
#include "TrigCombinedEventTPCnv/TrigComposite_p1.h"
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"
#include "TrigNavigation/TrigFeatureLink.h"

//dummy instances of all objects using std::vector<TPObjRef> for the dictionary
struct dummy_TrigCombinedEventTPCnvDict1
{
  std::vector<TrigComposite_p1>               m_dummyComposite_p1;
  std::vector<TrigCompositeContainer_p1>               m_dummyCompositeContainer_p1;
};	

#endif

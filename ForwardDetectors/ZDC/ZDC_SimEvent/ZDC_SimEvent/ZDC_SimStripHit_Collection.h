/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDC_SIMSTRIPHIT_COLLECTION
#define ZDC_SIMSTRIPHIT_COLLECTION

#include "ZDC_SimStripHit.h"
#include "HitManagement/AtlasHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

#include <string>

typedef AtlasHitsVector<ZDC_SimStripHit> ZDC_SimStripHit_Collection1;

class ZDC_SimStripHit_Collection: public ZDC_SimStripHit_Collection1
{

 public :
  ZDC_SimStripHit_Collection(std::string name="ZDC_SimStripHit_Collection") : ZDC_SimStripHit_Collection1(name.c_str()) {}
};

typedef  ZDC_SimStripHit_Collection::iterator       ZDC_SimStripHit_Iterator;
typedef  ZDC_SimStripHit_Collection::const_iterator ZDC_SimStripHit_ConstIterator;

#ifndef __CINT__
  CLASS_DEF(ZDC_SimStripHit_Collection, 1098700737, 1)
#endif

#endif

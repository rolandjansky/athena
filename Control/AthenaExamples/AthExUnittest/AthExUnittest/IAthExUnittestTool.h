// -*- mode: c++ -*-
//
//  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXUNITTEST_IATHEXUNITTESTTOOL_H
#define ATHEXUNITTEST_IATHEXUNITTESTTOOL_H 1

#include "GaudiKernel/IAlgTool.h"

class IAthExUnittestTool : public virtual IAlgTool {
public:

  //declare enums for properties here too, so that they are accessible through just the interface header
  //example of an enum you might end up using for your properties .. please put enums in a class! (not just a namespace)
  enum MyEnumProperty { Val1=1 , Val2=2 };

  virtual double useTheProperty() = 0;
  
};

#endif //> !ATHEXUNITTEST_IATHEXUNITTESTTOOL_H

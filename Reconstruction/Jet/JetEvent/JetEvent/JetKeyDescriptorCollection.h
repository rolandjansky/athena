/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Jetevent_Jetkeydescriptorcollection_H
#define Jetevent_Jetkeydescriptorcollection_H
#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "SGTools/BaseInfo.h"

#include "JetEvent/JetKeyDescriptor.h"

class JetKeyDescriptorCollection : public DataVector<JetKeyDescriptor> {
 public:
  // constructor
  virtual ~JetKeyDescriptorCollection() { };
};
CLASS_DEF( JetKeyDescriptorCollection , 1307237819 , 1 )

SG_BASE(JetKeyDescriptorCollection, DataVector<JetKeyDescriptor>);

#endif

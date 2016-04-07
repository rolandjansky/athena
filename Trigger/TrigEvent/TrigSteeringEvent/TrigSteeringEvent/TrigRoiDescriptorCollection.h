/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
//#ifndef XAOD_ANALYSIS
// #ifndef TRIGSTEERINGEVENT_TRIGROIDESCRIPTORCOLLECTION_H
#define TRIGSTEERINGEVENT_TRIGROIDESCRIPTORCOLLECTION_H

// #include "DataModel/DataVector.h"
#include "AthContainers/DataVector.h"
// #include "SGTools/BaseInfo.h"

#include "xAODCore/CLASS_DEF.h"
#include "xAODCore/BaseInfo.h"

#ifndef TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#endif

class TrigRoiDescriptorCollection : public DataVector<TrigRoiDescriptor> {
 public:
  TrigRoiDescriptorCollection() {};

  TrigRoiDescriptorCollection(SG::OwnershipPolicy pl) 
    : DataVector<TrigRoiDescriptor>(pl){};
};

CLASS_DEF( TrigRoiDescriptorCollection , 1097199488 , 1 )

SG_BASE(TrigRoiDescriptorCollection, DataVector<TrigRoiDescriptor>);

// #endif /// #ifndef TRIGSTEERINGEVENT_TRIGROIDESCRIPTORCOLLECTION_H

//#endif //XAOD_ANALYSIS

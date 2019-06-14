/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
 
#ifndef NSWVMMTIMECALIBCONTAINER_H
#define NSWVMMTIMECALIBCONTAINER_H 

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "NSWCalibData/VMMTimePar.h"
namespace Muon{
class NSWVMMTimeCalibContainer : public DataVector<VMMTimePar>
{
 public:
  NSWVMMTimeCalibContainer();
  ~NSWVMMTimeCalibContainer();

};

inline NSWVMMTimeCalibContainer::NSWVMMTimeCalibContainer():
  DataVector<VMMTimePar>()
{ } 

inline NSWVMMTimeCalibContainer::~NSWVMMTimeCalibContainer()
{ } 

CLASS_DEF(NSWVMMTimeCalibContainer,1165680501, 1)
}
#endif  // NSWVMMTimeCalibContainer_H

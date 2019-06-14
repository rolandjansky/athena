/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
 
#ifndef NSWVMMCHARGECALIBCONTAINER_H
#define NSWVMMCHARGECALIBCONTAINER_H 

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "NSWCalibData/VMMChargePar.h"
namespace Muon{
class NSWVMMChargeCalibContainer : public DataVector<VMMChargePar>
{
 public:
  NSWVMMChargeCalibContainer();
  ~NSWVMMChargeCalibContainer();

};

inline NSWVMMChargeCalibContainer::NSWVMMChargeCalibContainer():
  DataVector<VMMChargePar>()
{ } 

inline NSWVMMChargeCalibContainer::~NSWVMMChargeCalibContainer()
{ } 

CLASS_DEF(NSWVMMChargeCalibContainer,1232052380, 1)
}
#endif  // NSWVMMChargeCalibContainer_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ToyConversion/ToyConversionSvc.h"
long int
ToyConversionSvc::storageType() {
  return 0xff;
}

long int ToyConversionSvc::repSvcType() const {
  return storageType();
}

ToyConversionSvc::~ToyConversionSvc() {
  //std::cout << "deleting toy " << this << std::endl;
}

ToyConversionSvc::ToyConversionSvc(const std::string &name, ISvcLocator* svc) :
  ConversionSvc(name,svc,storageType()) 
{ 
  //std::cout << "building toy " << this << std::endl;
}
  

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_IHIVEEXSVC_H
#define ATHEXHIVE_IHIVEEXSVC_H 1

#include "GaudiKernel/IService.h"
#include <string>


class GAUDI_API IHiveExSvc : virtual public IService {

 public:
  
  virtual ~IHiveExSvc() {};
  
  DeclareInterfaceID(IHiveExSvc,1,0);
  
 public:

  virtual int get() const = 0;
  virtual int getTL() const = 0;

  virtual void set(const int&) = 0;
  virtual void setTL(const int&) = 0;

  virtual void add(const std::string&, const int&) = 0;

};

#endif





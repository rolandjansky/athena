/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  IHiveExSvc.h
 * @brief Abstract Interface class for HiveExSvc, that accumualtes
 * Algorithm run times by name
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

  // accumulate timing info
  virtual void add(const std::string&, const unsigned int&) = 0;

};

#endif





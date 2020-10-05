/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERIALIZECONVERSIONSVC_H
#define TRIGSERIALIZECONVERSIONSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/ConversionSvc.h"
#include "TrigSerializeCnvSvc/TrigStreamAddress.h"

template <class TYPE> class SvcFactory;


class TrigSerializeCnvSvc : public ConversionSvc {
public:
  static long int storageType() { return TrigStreamAddress::storageType(); }
  virtual long int repSvcType() const override { return storageType(); }

  virtual StatusCode initialize() override;

  TrigSerializeCnvSvc(const std::string &name, ISvcLocator* svc) :
    ConversionSvc(name,svc,storageType()) {}
};


#endif // 

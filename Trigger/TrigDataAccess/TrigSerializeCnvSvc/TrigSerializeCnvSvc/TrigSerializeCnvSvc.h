/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERIALIZECONVERSIONSVC_H
#define TRIGSERIALIZECONVERSIONSVC_H

#include "GaudiKernel/IService.h"
#include "GaudiKernel/ConversionSvc.h"
template <class TYPE> class SvcFactory;


static const InterfaceID IID_TrigSerializeCnvSvc("TrigSerializeCnvSvc", 0, 1);


class TrigSerializeCnvSvc : public ConversionSvc {
public:
  static long int storageType();
  virtual long int repSvcType() const;

  static const  InterfaceID& interfaceID() { return IID_TrigSerializeCnvSvc; }
  virtual StatusCode initialize();

  TrigSerializeCnvSvc(const std::string &name, ISvcLocator* svc) :
    ConversionSvc(name,svc,storageType()) { }

  virtual ~TrigSerializeCnvSvc() {};
};


#endif // 

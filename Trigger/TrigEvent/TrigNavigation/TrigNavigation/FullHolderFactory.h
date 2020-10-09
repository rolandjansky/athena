// Emacs -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVIGATION_FULLHOLDERFACTORY
#define TRIGNAVIGATION_FULLHOLDERFACTORY

#include <string>
#include "TrigNavStructure/ITrigHolderFactory.h"
#include "TrigNavStructure/BaseHolder.h"
#include "AsgMessaging/AsgMessaging.h"
#include "GaudiKernel/ServiceHandle.h"

class IConversionSvc;
class StringSerializer;
class StoreGateSvc;


namespace HLT {
  class FullHolderFactory : public ITrigHolderFactory, public ::asg::AsgMessaging {
  public:
    FullHolderFactory(const std::string& prefix);
    ::HLT::BaseHolder* fromSerialized(int version, const std::vector<uint32_t>::const_iterator& start, const std::vector<uint32_t>::const_iterator& end) override;    
    ::HLT::BaseHolder* createHolder(class_id_type clid, const std::string& label, uint16_t index) override;    
    void prepare(StoreGateSvc* store, IConversionSvc* serializer, bool readonly = true){
      m_storeGate = store;
      m_serializerSvc = serializer;
      m_readonly = readonly;
    }
    IConversionSvc* m_serializerSvc;
    StoreGateSvc* m_storeGate;
    std::string m_prefix;
    bool m_readonly;
  };
}
#endif

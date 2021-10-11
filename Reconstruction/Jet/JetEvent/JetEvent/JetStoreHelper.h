// emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETSTOREHELPER_H
#define JETEVENT_JETSTOREHELPER_H

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include <string>
#include <typeinfo>

struct JetStoreHelper
{

  inline static bool getStoreGate(StoreGateSvc*& pSG)
  {
    return (Gaudi::svcLocator()->service("StoreGate",pSG)).isSuccess();
  }
 
  template<class STORE>
  static bool recordStore(STORE* store,const std::string& key)
  {
    StoreGateSvc* pSG = 0;
    return getStoreGate(pSG)
      ? (pSG->record(store,key)).isSuccess()
      : false;
  }

  template<class STORE>
  static bool retrieveStore(STORE*& store,const std::string& key,
			    const std::string& msg)
  {
    IMessageSvc* msgSvc = 0;
    if ( (Gaudi::svcLocator()->service("MessageSvc",msgSvc)).isFailure() ) 
      return false;
    MsgStream report(msgSvc,msg);
    return retrieveStore(store,key,report); 
  }

  template<class STORE>
  static bool retrieveStore(STORE*& store,const std::string& key,
			    MsgStream& report)
  {
    // allocate StoreGate service
    StoreGateSvc* pSG = 0;
    if ( (Gaudi::svcLocator()->service("StoreGate",pSG)).isFailure() ) 
      {
	report << MSG::WARNING
	       << "cannot retrieve StoreGate service"
	       << endmsg;
	return false;
      }
        // 
    if ( (pSG->retrieve(store,key)).isFailure() )
      {
	std::string tStr(typeid(store).name()); stringFilter(tStr);
	report << MSG::WARNING 
	       << "cannot retrieve store type \042"
	       << tStr << "\042 with key <"
	       << key << ">" << endmsg;
	return false;
      }
    //
    return true;
  }

  static void stringFilter(std::string& str)
  {
    std::string::iterator fStr(str.begin());
    while ( fStr != str.end() )
      {
	if ( *fStr >= '0' && *fStr <= '9' ) 
	  {
	    fStr = str.erase(fStr);
	  }
	else
	  {
	    ++fStr;
	  }
      }
  }

};
#endif

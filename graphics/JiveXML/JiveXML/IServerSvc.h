/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML__ISERVERSVC_H
#define JIVEXML__ISERVERSVC_H

#include "GaudiKernel/IService.h"
#include "JiveXML/IEventReceiver.h"

namespace JiveXML {

  /**
   * Athena interface for all ServerSvc implementations. Note that this
   * interface only inherits the IEventReceiver, as we only want to update
   * events from Athena to the server and do not require the service also to
   * serve events (which it doesn't e.g in case of the ExternalONCRPCServerSvc). 
   */
  class GAUDI_API IServerSvc : virtual public IService,
			       virtual public IEventReceiver {
  public:

     DeclareInterfaceID(IServerSvc,2,0);

     virtual ~IServerSvc(){};
    
  };

} //namespace

#endif

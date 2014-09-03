/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @ class AddressProviderSvc 
 * @brief helper class for ProxyProvider implementers
 *
 * @author ATLAS Collaboration
 * $Id: AddressProviderSvc.h,v 1.4 2003-04-16 01:12:23 calaf Exp $
 ***************************************************************************/

#ifndef ATHENAKERNEL_ADDRESSPROVIDERSVC_H
#define ATHENAKERNEL_ADDRESSPROVIDERSVC_H

//<<<<<< INCLUDES                                                       >>>>>>
#ifndef _CPP_STRING
 #include <string>
#endif
#ifndef ATHENAKERNEL_IADDRESSPROVIDER_H
 #include "AthenaKernel/IAddressProvider.h"
#endif
#ifndef GAUDIKERNEL_SERVICE_H
 #include "GaudiKernel/Service.h"
#endif
#ifndef GAUDIKERNEL_ISVCLOCATOR_H
 #include "GaudiKernel/ISvcLocator.h"
#endif


class AddressProviderSvc : public Service, virtual public IAddressProvider {
  /// Standard Constructor                       
  AddressProviderSvc( const std::string& name, ISvcLocator* svcloc) :
    Service(name, svcloc) {};

  void dummy();
};

#endif // ATHENAKERNEL_ADRESSPROVIDERSVC_H




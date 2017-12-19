///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IHIVESTORE_H
# define ATHENAKERNEL_IHIVESTORE_H


#include "GaudiKernel/INamedInterface.h"


class IProxyDict;

/** @class IHiveStore   
 * @brief the interface that provides access to the current hive store
 *
 * @author Paolo Calafiura - ATLAS
 *  $Id: IHiveStore.h 714254 2015-12-12 03:38:51Z ssnyder $
 */

class IHiveStore : virtual public INamedInterface {
public:
  DeclareInterfaceID (INamedInterface, 2, 0);
  virtual ~IHiveStore() {}

  virtual IProxyDict* hiveProxyDict() = 0;
};

#endif // ATHENAKERNEL_IHIVESTORE_H











///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IHIVESTORE_H
# define ATHENAKERNEL_IHIVESTORE_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GaudiKernel/INamedInterface.h"

//<<<<<< FORWARD DECLARATIONS                                           >>>>>

class IProxyDictWithPool;

/** @class IHiveStore   
 * @brief the interface that provides access to the current hive store
 *
 * @author Paolo Calafiura - ATLAS
 *  $Id: IHiveStore.h 547194 2013-05-08 23:04:59Z leggett $
 */

class IHiveStore : virtual public INamedInterface {
public:
  virtual ~IHiveStore() {}

  virtual IProxyDictWithPool* hiveProxyDict() = 0;


  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();
};
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
inline
const InterfaceID& 
IHiveStore::interfaceID() {
  static const InterfaceID _IID("IHiveStore", 1, 0);
  return _IID;
}
#endif // ATHENAKERNEL_IHIVESTORE_H











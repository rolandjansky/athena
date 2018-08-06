///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_IHIVESTOREMGR_H
# define ATHENAKERNEL_IHIVESTOREMGR_H


#include "GaudiKernel/INamedInterface.h"
#include "GaudiKernel/DataObjID.h"


namespace SG {
  class DataProxy;
}

/** @class IHiveStoreMgr   
 * @brief the interface through which HiveWB control StoreGate instances 
 *
 * @author Paolo Calafiura - ATLAS
 *  $Id: IHiveStoreMgr.h 547067 2013-05-07 22:14:35Z calaf $
 */

class IHiveStoreMgr : virtual public INamedInterface {
public:
  /// clear the store
  virtual StatusCode clearStore(bool forceRemove=false) =0;

  /** Reset handles added since the last call to commit.
   */
  virtual void commitNewDataObjects() = 0;
  
  virtual ~IHiveStoreMgr() {}

  /// Gaudi boilerplate
  static const InterfaceID& interfaceID();
};

inline
const InterfaceID& 
IHiveStoreMgr::interfaceID() {
  static const InterfaceID IID("IHiveStoreMgr", 1, 0);
  return IID;
}
#endif // ATHENAKERNEL_IHIVESTOREMGR_H











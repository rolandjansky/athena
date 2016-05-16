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

  /** Get data objects registred in store since last getNewDataObjects call (or since init for 1st call)
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode getNewDataObjects(DataObjIDColl& products)=0;

  /** Check if something has been added to the store since last getNewDataObjects call
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Boolean indicating the presence of new products
   */
  virtual bool newDataObjectsPresent()=0; 

  /** make newly recorded DataObjects know to the WhiteBoard, by copying
   *    from thread local storag
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











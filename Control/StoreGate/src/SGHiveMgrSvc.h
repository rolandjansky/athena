/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ATHENAHIVE
#ifndef STOREGATE_HIVEMGRSVC_H
#define STOREGATE_HIVEMGRSVC_H

#include <vector>
#include <string>

/* maybe #include "tbb/mutex.h" */

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGHiveEventSlot.h"

class StoreGateSvc;
class ISvcLocator;

template <class TYPE> class SvcFactory;

/** @class HiveMgrSvc
 *  @brief A service that manages a multi-event collection of StoreGateSvc
 *  It implements the IHiveWhiteBoard interface
 *
 *  $Id: SGHiveMgrSvc.h 694789 2015-09-14 19:36:04Z leggett $
 **/
namespace SG {
class HiveMgrSvc : virtual public IHiveWhiteBoard, public Service {
  friend class SvcFactory<HiveMgrSvc>;
  friend class TestSGHiveMgrSvc;
public:
  //@{ @name IHiveWhiteBoard implementation
  /** Activate an given 'slot' for all subsequent calls within the
   * same thread id.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode selectStore(size_t slotIndex);

  /** Clear a given 'slot'.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode clearStore(size_t slotIndex);
  
  /** Set the number of 'slots'.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode setNumberOfStores(size_t slots);

  /** Get the number of 'slots'.
   * 
   * @return Number of event stores allocated in the whiteboard
   */
  virtual size_t getNumberOfStores();
 
  /** Get the latest new data objects registred in store.
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode getNewDataObjects(DataObjIDColl& products);

  /** Check if something is new in the whiteboard without getting the products.
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Boolean indicating the presence of new products
   */
  virtual bool newDataObjectsPresent(); 
  
  /** Allocate a store slot for new event
   *
   * @param     evtnumber     [IN]     Event number
   * @param     slot     [OUT]    Returned slot or slot number
   * @return Slot number (npos to indicate an error).
   */
  virtual size_t allocateStore( int evtnumber );
  
  /** Free a store slot
   *
   * @param     slot     [IN]     Slot number
   * @return Status code indicating failure or success.
   */
  virtual StatusCode freeStore( size_t slotIndex );
  
  
  /** Get the slot number corresponding to a given event
   *
   * @param     evtnumber     [IN]     Event number
   * @return    slot number (npos to indicate an error).
   */
  virtual size_t getPartitionNumber(int eventnumber) const;
  //@{ @name Gaudi Service boilerplate
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  //@}


private:
  ServiceHandle<StoreGateSvc> m_hiveStore;
  size_t m_nSlots; //property settable also by setNumberOfStores
  std::vector<SG::HiveEventSlot> m_slots;
  //maybe  ServiceHandle<ActiveStoreSvc> m_active;

protected:
  /// Standard Service Constructor. sets active store to default event store
  HiveMgrSvc(const std::string& name, ISvcLocator* svc);

  virtual ~HiveMgrSvc() {}

};
} //namespace SG
#endif // STOREGATE_HIVEMGRSVC_H
#endif // ATHENAHIVE




/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STOREGATE_HIVEMGRSVC_H
#define STOREGATE_HIVEMGRSVC_H

#include <vector>
#include <string>

/* maybe #include "tbb/mutex.h" */

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IIncidentListener.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SGHiveEventSlot.h"

class StoreGateSvc;
class ISvcLocator;


/** @class HiveMgrSvc
 *  @brief A service that manages a multi-event collection of StoreGateSvc
 *  It implements the IHiveWhiteBoard interface
 *
 *  $Id: SGHiveMgrSvc.h 794852 2017-01-31 23:24:04Z leggett $
 **/
namespace SG {
  class HiveMgrSvc : virtual public IHiveWhiteBoard, public Service,  public IIncidentListener {
  friend class TestSGHiveMgrSvc;
public:
  //@{ @name IHiveWhiteBoard implementation
  /** Activate an given 'slot' for all subsequent calls within the
   * same thread id.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode selectStore(size_t slotIndex) override;

  /** Clear a given 'slot'.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode clearStore(size_t slotIndex) override;
  
  /** Set the number of 'slots'.
   *
   * @param  slot     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode setNumberOfStores(size_t slots) override;

  /** Get the number of 'slots'.
   * 
   * @return Number of event stores allocated in the whiteboard
   */
  virtual size_t getNumberOfStores() const override;
 
  /** Get the latest new data objects registered in store.
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Status code indicating failure or success.
   */
  virtual StatusCode getNewDataObjects(DataObjIDColl& products) override;

  /** Check if something is new in the whiteboard without getting the products.
   *
   * @param  products     [IN]     Slot number (event slot)   *
   * @return Boolean indicating the presence of new products
   */
  virtual bool newDataObjectsPresent() override;
  
  /** Allocate a store slot for new event
   *
   * @param     evtnumber     [IN]     Event number
   * @param     slot     [OUT]    Returned slot or slot number
   * @return Slot number (npos to indicate an error).
   */
  virtual size_t allocateStore( int evtnumber ) override;
  
  /** Free a store slot
   *
   * @param     slot     [IN]     Slot number
   * @return Status code indicating failure or success.
   */
  virtual StatusCode freeStore( size_t slotIndex ) override;
  
  
  /** Get the slot number corresponding to a given event
   *
   * @param     evtnumber     [IN]     Event number
   * @return    slot number (npos to indicate an error).
   */
  virtual size_t getPartitionNumber(int eventnumber) const override;
  
  //@{ @name Gaudi Service boilerplate
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
  //@}

  //handle incidents
  virtual void handle(const Incident&) override final;    

  /// Standard Service Constructor. sets active store to default event store
  HiveMgrSvc(const std::string& name, ISvcLocator* svc);

  virtual ~HiveMgrSvc() {}

private:
  ServiceHandle<StoreGateSvc> m_hiveStore;
  size_t m_nSlots; //property settable also by setNumberOfStores
  std::vector<SG::HiveEventSlot> m_slots;
  //maybe  ServiceHandle<ActiveStoreSvc> m_active;

};
} //namespace SG
#endif // STOREGATE_HIVEMGRSVC_H

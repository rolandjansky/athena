/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGSERVICES_HLTROBDATAPROVIDERSVC_H
#define TRIGSERVICES_HLTROBDATAPROVIDERSVC_H
/**
 * @file   HltROBDataProviderSvc.h
 * @brief  Service to serve ROB data in online environment
 * @author Werner Wiedenmann
 */

// Package includes
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/SourceIdentifier.h"

// Framework includes
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "AthenaMonitoring/Monitored.h"

// STL includes
#include <string>
#include <string_view>
#include <vector>
#include <map>

// TBB includes
#include "tbb/concurrent_unordered_map.h"

/**
 * @class HltROBDataProviderSvc
 * @brief ROBDataProvider service for retrieving and serving ROB data in HLT online
 **/
class HltROBDataProviderSvc : public extends<AthService, IROBDataProviderSvc> {
public:
  /// ROB Fragment class
  typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

  HltROBDataProviderSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HltROBDataProviderSvc() override;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /// --- Implementation of IROBDataProviderSvc interface ---
  /// --- Legacy interface (deprecated) ---

  /// Signal ROB fragments which should be considered for prefetching in online running
  virtual void addROBData(const std::vector<uint32_t>& /*robIds*/, 
			  const std::string_view callerName="UNKNOWN") override;

  /// Start a new event with a set of ROB fragments, e.g. from LVL1 result, in online and add the fragments to the ROB cache
  virtual void setNextEvent(const std::vector<ROBF>& /*result*/) override;

  /// Start a new event with a full event fragment and add all ROB fragments in to the ROB cache
  virtual void setNextEvent(const RawEvent* /*re*/) override;

  /// Retrieve ROB fragments for given ROB ids from the ROB cache
  virtual void getROBData(const std::vector<uint32_t>& /*robIds*/, std::vector<const ROBF*>& /*robFragments*/, 
			  const std::string_view callerName="UNKNOWN") override;

  /// Retrieve the full event fragment
  virtual const RawEvent* getEvent() override;

  /// Store the status for the event.
  virtual void setEventStatus(uint32_t /*status*/) override;

  /// Retrieve the status for the event.
  virtual uint32_t getEventStatus() override;

  /// --- Implementation of IROBDataProviderSvc interface ---
  /// --- Context aware interface for MT ---

  /// Signal ROB fragments which should be considered for prefetching in online running
  virtual void addROBData(const EventContext& /*context*/, const std::vector<uint32_t>& /*robIds*/, 
			  const std::string_view callerName="UNKNOWN") override;

  /// Start a new event with a set of ROB fragments, e.g. from LVL1 result, in online and add the fragments to the ROB cache
  virtual void setNextEvent(const EventContext& /*context*/, const std::vector<ROBF>& /*result*/) override;

  /// Start a new event with a full event fragment and add all ROB fragments in to the ROB cache
  virtual void setNextEvent(const EventContext& /*context*/, const RawEvent* /*re*/) override;

  /// Retrieve ROB fragments for given ROB ids from the ROB cache
  virtual void getROBData(const EventContext& /*context*/, 
			  const std::vector<uint32_t>& /*robIds*/, std::vector<const ROBF*>& /*robFragments*/, 
			  const std::string_view callerName="UNKNOWN") override;

  /// Retrieve the full event fragment
  virtual const RawEvent* getEvent(const EventContext& /*context*/) override;

  /// Store the status for the event.
  virtual void setEventStatus(const EventContext& /*context*/, uint32_t /*status*/) override;

  /// Retrieve the status for the event.
  virtual uint32_t getEventStatus(const EventContext& /*context*/) override;

  /// Apply a function to all ROBs in the cache
  virtual void processCachedROBs(const EventContext& /*context*/, const std::function< void(const ROBF* )>& /*fn*/) const override;

  /// Flag to check if all event data have been retrieved
  virtual bool isEventComplete(const EventContext& /*context*/) const override;

  /// retrieve in online running all ROBs for the event from the readout system. Only those ROBs are retrieved which are not already in the cache
  virtual int collectCompleteEventData(const EventContext& /*context*/, 
				       const std::string_view callerName="UNKNOWN") override;

private:
  /*--------------+
   *  Event cache |
   *--------------+ 
   */
  /// map for all the ROB fragments
  typedef tbb::concurrent_unordered_map<uint32_t, ROBF> ROBMAP;

  /// struct which provides the event cache for each slot
  struct EventCache {
    ~EventCache();
    const RawEvent* event      = 0;
    uint32_t currentLvl1ID     = 0; 
    uint64_t globalEventNumber = 0;
    uint32_t eventStatus       = 0;    
    bool     isEventComplete   = false;    
    ROBMAP   robmap;
  };

  /// An event cache for each slot
  SG::SlotSpecificObj<EventCache> m_eventsCache;

  /*------------------------------+
   * Attributes for configuration |
   *------------------------------+ 
   */
  /// vector of Source ids and status words to be ignored for the ROB map
  typedef std::vector< std::pair<int, int> > ArrayPairIntType;
  Gaudi::Property< ArrayPairIntType > m_filterRobWithStatus{
    this, "filterRobWithStatus", {} , "List of ROBs with status code to remove"};
  Gaudi::Property< ArrayPairIntType > m_filterSubDetWithStatus{
    this, "filterSubDetWithStatus", {} , "List of SubDets with status code to remove"};

  /// map of full ROB Source ids and status words to be ignored for the ROB map
  typedef std::map<uint32_t, std::vector<uint32_t> > FilterRobMap;
  FilterRobMap          m_filterRobMap;
  /// map of Sub Det Source ids and status words to be ignored for the ROB map
  typedef std::map<eformat::SubDetector, std::vector<uint32_t> > FilterSubDetMap;
  FilterSubDetMap       m_filterSubDetMap;

  /// Filter out empty ROB fragments which are send by the ROS
  Gaudi::Property<bool> m_filterEmptyROB{
    this, "filterEmptyROB", false , "Filter out empty ROB fragments"};

  /// For Run 1 the module ID for the Lvl2/EF result contained the machine ID and nedded to
  /// be filtered out to access these result records transparently
  bool m_maskL2EFModuleID = false;    

  // read enabled ROBs from OKS when possible
  Gaudi::Property<bool> m_readROBfromOKS{
    this, "readROBfromOKS", true , "Read enabled ROBs from OKS"};

  // list of all enabled ROBs which can be retrieved    
  Gaudi::Property< std::vector<uint32_t> > m_enabledROBs{
    this, "enabledROBs", {} , "Enabled ROBs for retrieval"};

  /*------------------------+
   * Methods acting on ROBs |
   *------------------------+ 
   */
  /// method to filter ROBs with given Status code
  bool robmap_filterRobWithStatus(const ROBF*);

  /*------------------------------+
   * Methods acting on EventCache |
   *------------------------------+ 
   */
  /// method to clear an event cache in a slot
  /// input:
  ///     pointer to cache
  void eventCache_clear(EventCache*);

  /// method to compare a list of ROB Ids to the ones in an event cache in a slot
  /// input:
  ///     pointer to cache
  ///     vector of ROB Ids to compare to existing ones in cache
  /// output:
  ///     vector of ROB fragments available already in cache
  ///     vector of ROB Ids missing in cache
  void eventCache_checkRobListToCache(EventCache*, const std::vector<uint32_t>&, 
				      std::vector<const ROBF*>&, std::vector<uint32_t>& );

  /// method to add ROB fragments to an event cache in a slot
  /// input:
  ///     pointer to cache
  ///     vector of ROB fragments to add to the cache
  void eventCache_addRobData(EventCache*, const std::vector<ROBF>&) ;

  /// Monitoring tool
  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
};

#endif // TRIGSERVICES_HLTROBDATAPROVIDERSVC_H

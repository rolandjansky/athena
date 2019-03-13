/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BYTESTREAMCNVSVCBASE_ROBDATAPROVIDERSVC_H
#define BYTESTREAMCNVSVCBASE_ROBDATAPROVIDERSVC_H

/** ===============================================================
 * @class    ROBDataProviderSvc.h
 * @brief  ROBDataProvider class for accessing ROBData
 *
 *    Requirements: define a ROBData class in the scope
 *                  provide a method
 *       void getROBData(const vector<uint>& ids, vector<ROBData*>& v)
 *    Implementation: Use an interal map to store all ROBs
 *                    We can not assume any ROB/ROS relationship, no easy
 *                    way to search.
 *                    This implementation is used in offline
 *
 *    Created:      Sept 19, 2002
 *         By:      Hong Ma
 *    Modified:     Aug. 18  2003 (common class for Online/Offline)
 *         By:      Werner Wiedenmann
 *    Modified:     Apr  21  2005 (remove dependency on data flow repository)
 *         By:      Werner Wiedenmann
 */

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "eformat/SourceIdentifier.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include <vector>
#include <map>

class ROBDataProviderSvc :  public extends<AthService, IROBDataProviderSvc> {

public:
   /// ROB Fragment class
   typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

   /// Constructor
   ROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);
   /// Destructor
   virtual ~ROBDataProviderSvc();

   /// initialize the service
   virtual StatusCode initialize() override;

   /// Gaudi queryInterface method.
   //   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector
   virtual void addROBData(const std::vector<uint32_t>& robIds, const std::string_view callerName="UNKNOWN") override;

   /// Add a given LVL1/LVL2 ROBFragment to cache
   virtual void setNextEvent(const std::vector<ROBF>& result) override;

   /// Add all ROBFragments of a RawEvent to cache
   virtual void setNextEvent(const RawEvent* re) override;

   /// Retrieve ROBFragments for given ROB ids from cache
   virtual void getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments, const std::string_view callerName="UNKNOWN") override;

   /// Retrieve the whole event.
   virtual const RawEvent* getEvent() override;

   /// Store the status for the event.
   virtual void setEventStatus(uint32_t status) override;

   /// Retrieve the status for the event.
   virtual uint32_t getEventStatus() override;


   /// MT variants 
   virtual void addROBData(const EventContext& context, const std::vector<uint32_t>& robIds, const std::string_view callerName="UNKNOWN") override;
   virtual void setNextEvent(const EventContext& context, const std::vector<OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment>& result) override;
   virtual void setNextEvent(const EventContext& context, const RawEvent* re) override;
   virtual void getROBData(const EventContext& context, const std::vector<uint32_t>& robIds, VROBFRAG& robFragments, 
			   const std::string_view callerName="UNKNOWN") override;
   virtual const RawEvent* getEvent(const EventContext& context) override;
   virtual void setEventStatus(const EventContext& context, uint32_t status) override;
   virtual uint32_t getEventStatus(const EventContext& context) override;

   virtual void processCachedROBs(const EventContext& context, 
				  const std::function< void(const ROBF* )>& fn ) const override;

   virtual bool isEventComplete(const EventContext& /*context*/) const override { return true; }
   virtual int collectCompleteEventData(const EventContext& /*context*/, const std::string_view /*callerName*/ ) override {  return 0; }

protected:
   /// vector of ROBFragment class
   typedef std::vector<ROBF*> VROBF;

   /// map for all the ROB fragments
   typedef std::map<uint32_t, const ROBF*, std::less<uint32_t> > ROBMAP;

  struct EventCache {
    ~EventCache();
    const RawEvent* event = 0;
    uint32_t eventStatus = 0;    
    uint32_t currentLvl1ID = 0;    
    ROBMAP robmap;
 
  };
  SG::SlotSpecificObj<EventCache> m_eventsCache;

   /// Remaining attributes are for configuration
   /// vector of Source ids and status words to be ignored for the ROB map
   typedef SimpleProperty< std::vector< std::pair<int, int> > > ArrayPairIntProperty;
   ArrayPairIntProperty  m_filterRobWithStatus; // filter with full ROB SourceID
   ArrayPairIntProperty  m_filterSubDetWithStatus;  // filter with Sub Det ID

   /// map of full ROB Source ids and status words to be ignored for the ROB map
   typedef std::map<uint32_t, std::vector<uint32_t> > FilterRobMap;
   FilterRobMap          m_filterRobMap;
   /// map of Sub Det Source ids and status words to be ignored for the ROB map
   typedef std::map<eformat::SubDetector, std::vector<uint32_t> > FilterSubDetMap;
   FilterSubDetMap       m_filterSubDetMap;
   /// method to filter ROBs with given Status code
   bool filterRobWithStatus(const ROBF* rob);

   /// Filter out empty ROB fragments which are send by the ROS
   BooleanProperty m_filterEmptyROB;
   bool m_maskL2EFModuleID = false;    

private: // data

private: //
  static void robmapClear(ROBMAP& toclear);
};

#endif

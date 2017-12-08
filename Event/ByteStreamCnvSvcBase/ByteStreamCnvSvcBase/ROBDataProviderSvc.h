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

#include <vector>
#include <map>

class ROBDataProviderSvc :  public ::AthService, virtual public IROBDataProviderSvc {

public:
   /// ROB Fragment class
   typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF;

   /// Constructor
   ROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);
   /// Destructor
   virtual ~ROBDataProviderSvc();

   /// initialize the service
   virtual StatusCode initialize();

   /// Gaudi queryInterface method.
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector
   virtual void addROBData(const std::vector<uint32_t>& robIds, const std::string callerName="UNKNOWN");

   /// Add a given LVL1/LVL2 ROBFragment to cache
   virtual void setNextEvent(const std::vector<ROBF>& result);

   /// Add all ROBFragments of a RawEvent to cache
   virtual void setNextEvent(const RawEvent* re);

   /// Retrieve ROBFragments for given ROB ids from cache
   virtual void getROBData(const std::vector<uint32_t>& robIds, std::vector<const ROBF*>& robFragments, const std::string callerName="UNKNOWN");

   /// Retrieve the whole event.
   virtual const RawEvent* getEvent();

   /// Store the status for the event.
   virtual void setEventStatus(uint32_t status);

   /// Retrieve the status for the event.
   virtual uint32_t getEventStatus();

protected:
   /// vector of ROBFragment class
   typedef std::vector<ROBF*> VROBF;

   /// map for all the ROB fragments
   typedef std::map<uint32_t, const ROBF*, std::less<uint32_t> > ROBMAP;
   ROBMAP m_robmap;

   /// lvl1 ID of cached data
   uint32_t m_currentLvl1ID;

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

   /// flag which tells if module IDs from the L2 and EF result should be masked off
   bool m_maskL2EFModuleID;

private: // data
   const RawEvent* m_event;

   uint32_t m_eventStatus;

private: //
   void robmapClear();
};

#endif

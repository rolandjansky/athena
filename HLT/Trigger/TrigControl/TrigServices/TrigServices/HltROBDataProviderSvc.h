/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTROBDATAPROVIDERSVC_H
#define TRIGSERVICES_HLTROBDATAPROVIDERSVC_H

/** ===============================================================
 *     HltROBDataProviderSvc.h
 *  ===============================================================
 *    Description:  ROBDataProvider class for accessing ROBData
 *                  To be used for Offline/Online
 *
 *    Requirements: define a ROBData class in the scope
 *                  provide a method
 *       void getROBData(const vector<uint>& ids, vector<ROBData*>& v)
 *    Implementation: Use an interal map to store all ROBs
 *                    We can not assume any ROB/ROS relationship, no easy 
 *                    way to search.
 * 
 *    Created:      Sept 19, 2002
 *         By:      Hong Ma 
 *    Modified:     Aug. 18  2003 (common class for Online/Offline)
 *         By:      Werner Wiedenmann 
 *    Modified:     Apr  21  2005 (implementation for online)
 *         By:      Werner Wiedenmann
 *    Modified:     Jun. 02 2006 (added monitoring horograms)
 *         By:      Tomasz Bold
 *    Modified:     May  21  2007 (updates for release 13)
 *         By:      Werner Wiedenmann
 *    Modified:     Nov. 10  2008 (updates for L2/EF result node ID handling)   
 *         By:      Werner Wiedenmann
 *    Modified:     Mar. 05, 2013 (adapted from L2 for merged HLT)
 *         By:      Ricardo Abreu
 *    Modified:     Nov. 04, 2013 (implement complete hltinterface definitions)
 *         By:      Werner Wiedenmann
 */ 
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/HistoProperty.h"
#include "GaudiKernel/Property.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvcPrefetch.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include "TrigSteeringEvent/RobRequestInfo.h"
#include "eformat/Status.h"
#include "hltinterface/DCM_ROBInfo.h"
#include <vector>
#include <map>

// Forward declarations
class StoreGateSvc;  
class IAlgContextSvc;
class TH1F;   /// for monitoring purposes
class TH2F;   /// for monitoring purposes

class HltROBDataProviderSvc : public ROBDataProviderSvc,
			       virtual public ITrigROBDataProviderSvc,
			       virtual public ITrigROBDataProviderSvcPrefetch,
			       virtual public IIncidentListener
{
public:

    typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF ; 

    HltROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);

    virtual ~HltROBDataProviderSvc(void);
  
    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /// --- Implementation of IROBDataProviderSvc interface ---    

    /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector 
    virtual void addROBData(const EventContext& /*ctx*/,
                            const std::vector<uint32_t>& robIds,
			    const std::string callerName="UNKNOWN")
    {
      addROBData(robIds, callerName);
    }
    virtual void addROBData(const std::vector<uint32_t>& robIds,
			    const std::string callerName="UNKNOWN");

    /// Add a given LVL1/HLT ROBFragment to cache
    virtual void setNextEvent(const EventContext& /*ctx*/,
                              const std::vector<ROBF>& result)
    { setNextEvent (result); }
    virtual void setNextEvent(const std::vector<ROBF>& result);

    /// Add all ROBFragments of a RawEvent to cache 
    virtual void setNextEvent(const EventContext& /*ctx*/, const RawEvent* re)
    { setNextEvent(re); }
    virtual void setNextEvent(const RawEvent* re);

    /// Retrieve ROBFragments for given ROB ids from cache 
    virtual void getROBData(const EventContext& /*ctx*/,
                            const std::vector<uint32_t>& robIds, 
			    std::vector<const ROBF*>& robFragments,
			    const std::string callerName="UNKNOWN")
    {
      getROBData (robIds, robFragments, callerName);
    }
    virtual void getROBData(const std::vector<uint32_t>& robIds, 
			    std::vector<const ROBF*>& robFragments,
			    const std::string callerName="UNKNOWN");
 
    /// Retrieve the whole event.
    virtual const RawEvent* getEvent(const EventContext& /*ctx*/)
    { return getEvent(); }
    virtual const RawEvent* getEvent();

    /// --- Implementation of ITrigROBDataProviderSvc interface ---

    /// Return vector with all ROBFragments stored in the cache 
    virtual void getAllROBData(std::vector<const ROBF*>& robFragments) ;

    // Dump ROB cache
    virtual std::string dumpROBcache() const ;

    /// Return size of ROBFragments cache 
    virtual int sizeROBCache() { return m_online_robmap.size(); }

    /// iterators over cache entries
    virtual std::map<uint32_t, ROBF>::iterator beginROBCache() { return m_online_robmap.begin(); }
    virtual std::map<uint32_t, ROBF>::iterator endROBCache()   { return m_online_robmap.end(); }

    /// Flag to check if complete event data are already in cache
    virtual bool isEventComplete(const EventContext& /*ctx*/) const { return m_isEventComplete; }
    virtual bool isEventComplete() { return m_isEventComplete; }

    /// Collect all data for an event from the ROS and put them into the cache
    /// Return value: number of ROBs which were retrieved to complete the event
    /// Optinonally the name of the caller of this method can be specified for cost monitoring
    virtual int collectCompleteEventData(const EventContext& /*ctx*/,
                                         const std::string callerName="UNKNOWN")
    {
      return collectCompleteEventData(callerName);
    }
    virtual int collectCompleteEventData(const std::string callerName="UNKNOWN");

    /// set the name of the program which uses the ROBDataProviderSvc
    virtual void setCallerName(const std::string);

    /// get the name of the program which is presently registered in the ROBDataProviderSvc
    virtual std::string getCallerName() { return m_callerName; };

    /// --- Implementation of ITrigROBDataProviderSvcPrefetch interface ---

    /// Set access to ROB prefetching information from steering
    virtual void setRobRequestInfo(HLT::RobRequestInfo* robInfo) { m_RobRequestInfo = robInfo; } ;

    /// Get access to ROB prefetching information from steering
    virtual HLT::RobRequestInfo* robRequestInfo() const { return m_RobRequestInfo; };

    /// --- Implementation of IIncidentListener interface ---

    // handler for BeginRun actions
    void handle(const Incident& incident);


private:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Reference to StoreGateSvc;
    StoreGateSvc_t         m_storeGateSvc;
    
    /// Pointer to AlgContextSvc 
    IAlgContextSvc*        m_algContextSvc;

    typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
    typedef SimpleProperty< std::map<int,int> >     IntegerMapProperty;

    // flag indicates if running in online/offline 
    bool m_onlineRunning ;

    // read enabled ROBs from OKS when possible
    BooleanProperty m_readROBfromOKS;

    // list of all enabled ROBs which can be retrieved    
    Uint32ArrayProperty m_enabledROBs;

    // list of ROBs which should be ignored for retrieval    
    Uint32ArrayProperty m_ignoreROB;

    // list of all LAr MET ROBs which can be retrieved    
    Uint32ArrayProperty m_enabledLArMetROBs ;
    UnsignedIntegerProperty m_genericLArMetModuleID ;

    // list of all Tile MET ROBs which can be retrieved    
    Uint32ArrayProperty m_enabledTileMetROBs;
    UnsignedIntegerProperty m_genericTileMetModuleID ;

    // Separate data collector calls to ROS for normal ROBs and MET ROBs
    BooleanProperty m_separateMETandDetROBRetrieval ;

    // Filter out empty ROB fragments which are send by the ROS
    bool m_removeEmptyROB;

    // map for all the ROB fragments (cache for ROB data so that they are 
    // not deleted when local containers go out of scope)
    typedef std::map<uint32_t, ROBF> ONLINE_ROBMAP;
    ONLINE_ROBMAP m_online_robmap; 

    // helper function to retrieve ROB fragments over the network and to add them to the cache 
    void addROBDataToCache(std::vector<uint32_t>& robIdsForRetrieval,         // vector of ROBs to retrieve 
			   robmonitor::ROBDataMonitorStruct* p_robMonStruct); // pointer to ROB monitoring structure for retrieval

    // helper function to put retrieved ROB fragments into the local cache and update the monitoring records 
    void updateROBDataCache(std::vector<hltinterface::DCM_ROBInfo>& vRobInfo,  // vector of ROB Info records with retrieved fragments 
			    robmonitor::ROBDataMonitorStruct* p_robMonStruct); // pointer to ROB monitoring structure

    // Flag to indicate if all event data are already in the cache
    bool m_isEventComplete;

    // name of the program which presently uses the ROBDataProviderSvc
    std::string m_callerName;

    // ROB prefetching info
    HLT::RobRequestInfo* m_RobRequestInfo;

    // monitoring
    std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
    std::vector<std::string>                      m_vec_SpecificStatus;

    BooleanProperty m_doMonitoring;
    BooleanProperty m_doDetailedROBMonitoring;
    StringProperty  m_ROBDataMonitorCollection_SG_Name;

    Histo1DProperty m_histProp_requestedROBsPerCall;
    Histo1DProperty m_histProp_receivedROBsPerCall;
    Histo1DProperty m_histProp_timeROBretrieval;

    TH1F* m_hist_requestedROBsPerCall;
    TH1F* m_hist_receivedROBsPerCall;
    TH1F* m_hist_timeROBretrieval;
    TH2F* m_hist_genericStatusForROB;
    TH2F* m_hist_specificStatusForROB;

    // Temporary hack to make this version compile in master
    uint32_t m_currentLvl1ID;
};

#endif

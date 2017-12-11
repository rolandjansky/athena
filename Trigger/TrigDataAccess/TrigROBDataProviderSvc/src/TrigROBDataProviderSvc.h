/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2SERVICES_TRIGROBDATAPROVIDERSVC_H
#define TRIGL2SERVICES_TRIGROBDATAPROVIDERSVC_H

/** ===============================================================
 *     TrigROBDataProviderSvc.h
 *  ===============================================================
 *    Description:  ROBDataProvider class for simulating the ROB data
 *                  access offline in the merged L2/EF system after LS1.
 *
 *                  In contrast to the normal offline data access the 
 *                  the event data are retrieved incrementally online
 *                  over the network and in general the complete event 
 *                  fragment is not available.
 *                  All ROB data which are required for processing have
 *                  to be pre-registered for retrieval with a call to
 *
 *                  addROBData(const std::vector<uint32_t>& robIds)  
 *                   
 *                  before they can be used with 
 *               
 *                  getROBData(const std::vector<uint32_t>& robIds, 
 *			    std::vector<const ROBF*>& robFragments).
 *                    
 * 
 *    Created:      Mar 1, 2013
 *         By:      Werner Wiedenmann 
 *   Modified:      Feb. 03, 2014 (implement complete hltinterface definitions)
 *         By:      Werner Wiedenmann
 */ 
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/HistoProperty.h"
#include "GaudiKernel/Property.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include "eformat/Status.h"
#include <vector>
#include <map>

#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"

// Forward declarations
class StoreGateSvc;  
class IAlgContextSvc;
class TH1F;   /// for monitoring purposes
class TH2F;   /// for monitoring purposes

class TrigROBDataProviderSvc : public ROBDataProviderSvc, 
			       virtual public ITrigROBDataProviderSvc,   
			       virtual public IIncidentListener
{
public:

    typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF ; 

    TrigROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);

    virtual ~TrigROBDataProviderSvc(void);
  
    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /// --- Implementation of IROBDataProviderSvc interface ---    

    /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector 
    using ROBDataProviderSvc::addROBData;
    virtual void addROBData(const std::vector<uint32_t>& robIds,
			    const std::string callerName="UNKNOWN") override;

    /// Add a given LVL1 ROBFragment to cache 
    using ROBDataProviderSvc::setNextEvent;
    virtual void setNextEvent(const std::vector<ROBF>& result) override;


    /// Add all ROBFragments of a RawEvent to cache     
    virtual void setNextEvent(const RawEvent* re) override;

    /// Retrieve ROBFragments for given ROB ids from cache 
    using ROBDataProviderSvc::getROBData;
    virtual void getROBData(const std::vector<uint32_t>& robIds, 
			    std::vector<const ROBF*>& robFragments,
			    const std::string callerName="UNKNOWN") override;
 
    /// Retrieve the whole event.
    using ROBDataProviderSvc::getEvent;
    virtual const RawEvent* getEvent() override;

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

    /// Check if complete event data are already in cache
    virtual bool isEventComplete() { return m_isEventComplete; }

    /// Collect all data for an event from the ROS and put them into the cache
    /// Return value: number of ROBs which were retrieved to complete the event
    virtual int collectCompleteEventData(const std::string callerName="UNKNOWN");

    /// set the name of the program which uses the ROBDataProviderSvc
    virtual void setCallerName(const std::string);

    /// get the name of the program which is presently registered in the ROBDataProviderSvc
    virtual std::string getCallerName() { return m_callerName; };

    /// --- Implementation of IIncidentListener interface ---

    // handler for BeginRun actions
    void handle(const Incident& incident);

protected:
    /**
     * @brief Accessor method for the MsgStream.
     * @return handle to the MsgStream.
     */
    inline MsgStream& logStream() const { return *m_msg; }

    /**
     * @brief Accessor method for the message level variable.
     * @return value of the message level for this algorithm.
     */
    inline MSG::Level logLevel() const { return  (m_msg != 0) ? m_msg->level() : MSG::NIL; }


   // name of the program which presently uses the ROBDataProviderSvc
    std::string m_callerName;

    BooleanProperty m_doDetailedROBMonitoring;

    BooleanProperty m_doMonitoring;

    /// Pointer to AlgContextSvc 
    IAlgContextSvc*        m_algContextSvc;

    /// vector of Source ids  to be ignored for the ROB map clear
    std::vector<uint32_t>    m_l1_ROB_ids;

    /// lvl1 id of CTP fragment
    uint32_t lvl1_id(); 
private:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Reference to StoreGateSvc;
    StoreGateSvc_t         m_storeGateSvc;
    
 
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

    // flag which tells if module IDs from the L2 and EF result should be masked off
    bool m_maskL2EFModuleID;



    // ROS-ROB mapping
    IntegerMapProperty m_rob_ros_map ;
    BooleanProperty    m_useROSmappingForROBRetrieval ;
    std::map< uint32_t, int > m_Det_Robs_for_retrieval ; // internal map of ROB->ROS for retrieval

    // map for all the ROB fragments (cache for ROB data so that they are 
    // not deleted when local containers go out of scope)
    typedef std::map<uint32_t, ROBF> ONLINE_ROBMAP;
    ONLINE_ROBMAP m_online_robmap; 

    // helper function to retrieve ROB fragments over the network and to add them to the cache 
    void addROBDataToCache(std::vector<uint32_t>& robIdsForRetrieval,         // vector of ROBs to retrieve 
			   std::vector<uint32_t>& retrievedRobIds,            // vector of received ROB ids 
			   robmonitor::ROBDataMonitorStruct* p_robMonStruct); // pointer to ROB monitoring structure for retrieval
 
  
    // Flag to indicate if all event data are already in the cache
    bool m_isEventComplete;

   
    /** @brief Pointer to MsgStream.*/
    MsgStream* m_msg;

    // monitoring
    std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
    std::vector<std::string>                      m_vec_SpecificStatus;


    StringProperty  m_ROBDataMonitorCollection_SG_Name;

    Histo1DProperty m_histProp_requestedROBsPerCall;
    Histo1DProperty m_histProp_receivedROBsPerCall;
    Histo1DProperty m_histProp_timeROBretrieval;
    Histo1DProperty m_histProp_retrievedROBsPerAlgo;
    Histo1DProperty m_histProp_ROSRequest;

    TH1F* m_hist_requestedROBsPerCall;
    TH1F* m_hist_receivedROBsPerCall;
    TH1F* m_hist_retrievedROBsPerAlgo;
    TH1F* m_hist_timeROBretrieval;
    TH2F* m_hist_genericStatusForROB;
    TH2F* m_hist_specificStatusForROB;
    TH2F* m_hist_ROSRequest;

    // online simulation
    BooleanProperty m_simulateOnlineDataAccess;
    BooleanProperty m_printStackTraceGetROBData;
    std::map<uint32_t, int> m_map_Failed_ROBs_not_preloaded;
    std::map<uint32_t, int> m_map_Failed_ROBs_preloaded_not_arrived;
};

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDATAACCESSMONITORING_MONROBDATAPROVIDERSVC_H
#define TRIGDATAACCESSMONITORING_MONROBDATAPROVIDERSVC_H

/** ===============================================================
 *     MonROBDataProviderSvc.h
 *  ===============================================================
 *    Description:  MonROBDataProviderSvc class for enabling the online
 *                  ROB monitoring infrastructure with the offline
 *                  ROBDataProviderSvc
 *
 *    Created:     Sep. 1  2009   
 *         By:      Werner Wiedenmann
 */ 
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/HistoProperty.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"
#include "eformat/Status.h"
#include <vector>
#include <map>

// Forward declarations
class StoreGateSvc;  
class IAlgContextSvc;
class TH1F;   /// for monitoring purposes
class TH2F;   /// for monitoring purposes

class MonROBDataProviderSvc : public ROBDataProviderSvc, 
			      virtual public IIncidentListener
{
public:

    typedef OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment ROBF ; 

    MonROBDataProviderSvc(const std::string& name, ISvcLocator* svcloc);

    virtual ~MonROBDataProviderSvc(void);
  
    virtual StatusCode initialize();

    virtual StatusCode finalize();

    virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

    /// --- Implementation of IROBDataProviderSvc interface ---    

    /// Add ROBFragments to cache for given ROB ids, ROB fragments may be retrieved with DataCollector 
    using ROBDataProviderSvc::addROBData;
    virtual void addROBData(const std::vector<uint32_t>& robIds,
			    const std::string callerName="UNKNOWN");

    /// Add a given LVL1/LVL2 ROBFragment to cache 
    using ROBDataProviderSvc::setNextEvent;
    virtual void setNextEvent(const std::vector<ROBF>& result);

    /// Add all ROBFragments of a RawEvent to cache 
    virtual void setNextEvent(const RawEvent* re) override;

    /// Retrieve ROBFragments for given ROB ids from cache 
    using ROBDataProviderSvc::getROBData;
    virtual void getROBData(const std::vector<uint32_t>& robIds, 
			    std::vector<const ROBF*>& robFragments,
                            const std::string callerName="UNKNOWN");
 
    /// Retrieve the whole event.
    using ROBDataProviderSvc::getEvent;
    virtual const RawEvent* getEvent() ;

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

private:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Reference to StoreGateSvc;
    StoreGateSvc_t         m_storeGateSvc;
    
    /// Pointer to AlgContextSvc 
    IAlgContextSvc*        m_algContextSvc;

    /** @brief Pointer to MsgStream.*/
    MsgStream* m_msg;

    // monitoring
    std::map<eformat::GenericStatus, std::string> m_map_GenericStatus;
    const std::string& genericStatusName( eformat::GenericStatus ) const;

    std::vector<std::string>                      m_vec_SpecificStatus;
    
    SG::SlotSpecificObj< std::vector<uint32_t> > m_robAlreadyAccessed;

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
};

#endif

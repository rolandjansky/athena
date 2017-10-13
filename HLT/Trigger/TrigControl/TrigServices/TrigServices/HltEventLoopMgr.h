/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSERVICES_HLTEVENTLOOPMGR_H
#define TRIGSERVICES_HLTEVENTLOOPMGR_H

#include "TrigKernel/ITrigEventLoopMgr.h"
#include "EventInfo/EventID.h"  /* number_type */
#include "EventInfo/TriggerInfo.h"
#include "xAODEventInfo/EventInfo.h"
#include "eformat/eformat.h"

// Framework include files
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/MinimalEventLoopMgr.h"
#include "GaudiKernel/HistoProperty.h"
#include "AthenaKernel/Timeout.h"
#include "TrigKernel/HltOstreams.h"
#include "TrigKernel/HltAcceptFlag.h"
#include "TrigKernel/HltResultStatusCode.h"
#include "TrigKernel/HltPscErrorCode.h"
#include "TrigKernel/IHltTHistSvc.h"
#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"

#include <tuple>
#include <utility>
#include <functional>
#include <memory>
#include <vector>
#include <set>
#include <stdint.h>

// Forward declarations
class IIncidentSvc;
class IAlgContextSvc;
class StoreGateSvc;
class IROBDataProviderSvc;
class ITHistSvc;
class TrigISHelper;
class TH1F;
class TH2I;
class TProfile;
class EventInfo;
class TrigCOOLUpdateHelper;
class CondAttrListCollection;
namespace coral {
  class AttributeList;
}
namespace TrigConf {
  class IHLTConfigSvc;
}
namespace HLT {
  class HLTResult;
}
namespace eformat {
  namespace write {
    class ROBFragment;
  }
}

class HltEventLoopMgr : public MinimalEventLoopMgr,
                        virtual public ITrigEventLoopMgr,
                        virtual public Athena::TimeoutMaster
{

public:
  /// Creator friend class
  friend class SvcFactory<HltEventLoopMgr>;

  /// Standard Constructor
  HltEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc);
  /// Standard Destructor
  virtual ~HltEventLoopMgr();

  /// implementation of IInterface: queryInterface
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  /// \name State transitions
  //@{
  virtual StatusCode sysInitialize();
  virtual StatusCode initialize();
  
  virtual StatusCode prepareForRun(const boost::property_tree::ptree & pt);

  virtual StatusCode hltUpdateAfterFork(const boost::property_tree::ptree & pt);

  virtual StatusCode stop();

  virtual StatusCode sysFinalize();
  virtual StatusCode finalize();

  virtual StatusCode sysReinitialize();
  virtual StatusCode reinitialize();  
  //@}
  
  /// Event processing
  virtual StatusCode executeEvent(void* par);
  
  virtual StatusCode
  processRoIs(const std::vector<eformat::ROBFragment<const uint32_t*> >& l1_result,
              hltinterface::HLTResult& hlt_result,
              const hltinterface::EventId& evId);

  /// Event timoeut reached
  virtual StatusCode timeOutReached(const boost::property_tree::ptree& pt);

  /// return the application Name
  std::string applicationName() const { return m_applicationName.value(); }

    /// \name Obsolete methods for online running
  //@{
  virtual StatusCode nextEvent(int maxevt);          ///< Obsolete
  virtual StatusCode executeRun(int runNumber);      ///< Obsolete
  virtual StatusCode stopRun();                      ///< Obsolete
  virtual StatusCode start();                        ///< Obsolete
  //@}
  
private:

   /**
   * @brief Call execute method of algorithms
   * @return FAILURE in case one algorithm failed
   */
  StatusCode executeAlgorithms();

  /**
   * @brief Check if an HLT prescale update is required
   * @param hltCounter  list of "HLT counters" from CTP fragment (only one used)
   */
  StatusCode checkHltPrescaleUpdate(std::vector<uint32_t>& hltCounter);

  /**
   * @brief Do HLT prescale update
   * @param lumiBlock  LB for which the update should be done
   */
  StatusCode hltPrescaleUpdate(uint32_t lumiBlock);
  
  /**
   * @brief Helper to fill histograms from HLT result
   * @param hlt_result          reference to dataflow HLTResult object
   */
  void fillHltResultHistograms(const hltinterface::HLTResult& hlt_result);
  
  /// Check if running in partition
  bool validPartition() const {
    return (m_partitionName.value()!="None" && m_partitionName.value()!="NONE");  
  }

  /**
   * @brief Helper to set PSC error word
   * @param hlt_result    reference to dataflow HLTResult object; filled on return
   * @param pscErrorCode  PSC Error code which should be set
   */
  void HltSetPscError(hltinterface::HLTResult& hlt_result,
                      hltonl::PSCErrorCode pscErrorCode) const;

  /**
   * @brief Helper to build an empty dataflow HLT Result object
   * @param hlt_result          reference to dataflow HLTResult object; filled on return
   * @param run_no              for ROB fragment 
   * @param lvl1_id             for ROB fragment 
   * @param bunch_crossing_id   for ROB fragment
   * @param l1_Trigger_Type     for ROB fragment
   * @param l1_detev_type       for ROB fragment
   * @param pscErrorCode        PSC Error code which should be set
   */
  void HltEmptyResultROB(hltinterface::HLTResult& hlt_result,
			 uint32_t run_no, uint32_t lvl1_id, uint32_t bunch_crossing_id, 
			 uint32_t l1_Trigger_Type, uint32_t l1_detev_type,
			 hltonl::PSCErrorCode pscErrorCode);

  /**
   * @brief Helper to build the dataflow HLT Result object.
   * @return value              error code hltonl::PSCErrorCode 
   * @param hlt_result          reference to dataflow HLTResult object; filled on return
   * @param run_no              for ROB fragment 
   * @param lvl1_id             for ROB fragment 
   * @param bunch_crossing_id   for ROB fragment
   * @param l1_Trigger_Type     for ROB fragment
   * @param l1_detev_type       for ROB fragment
   * @param trigger_info        trigger info words for the HLT
   * @param stream_tags         stream tags generated during HLT processing
   */
  hltonl::PSCErrorCode HltResultROBs(hltinterface::HLTResult& hlt_result,
				       uint32_t run_no, uint32_t lvl1_id, uint32_t bunch_crossing_id, 
				       uint32_t l1_Trigger_Type, uint32_t l1_detev_type,
				       const std::vector<TriggerInfo::number_type>& trigger_info, 
				       const std::vector<TriggerInfo::StreamTag>& stream_tags,
				       const std::vector<xAOD::EventInfo::StreamTag>& xsts);

  /// Helper to build an empty HLTResult object
  void HltEmptyResultROB(hltinterface::HLTResult& hlt_result,
                         hltonl::PSCErrorCode pscErrorCode);
  /// Helper to build HLTResult object from Steering result
  void HltResult(hltinterface::HLTResult& hlt_result,
                 const EventInfo* pEvent,
                 const xAOD::EventInfo* xev);
  /// Helpers to book histograms
  void bookHistograms();
  void HltBookHistograms();
  void bookAllHistograms();
  void updateDFProps();
  // Call a simple IAlgorithm (usually member) function on all algorithms
  StatusCode callOnAlgs(const std::function<StatusCode(IAlgorithm&)> & func,
                        const std::string & fname, bool failureIsError = false);
  StatusCode prepareAlgs(const EventInfo& evinfo);
  /// Helper to reset necessary data at prepareForRun
  StatusCode internalPrepareResets();
  /// Helper to do whatever is necessary with RunParams (prepare) ptree
  const CondAttrListCollection *
  processRunParams(const boost::property_tree::ptree & pt);
  // Helper to update internally kept data from new sor
  void updInternal(const coral::AttributeList & sor_attrlist);
  // Helper to get update the metadata store with a new metadata record
  void updMetadaStore(const coral::AttributeList & sor_attrlist);
  /// Helper to clear per-event stores
  StatusCode clearTemporaryStores();
  /// Helper to update the detector mask
  void updDetMask(const std::pair<uint64_t, uint64_t>& dm);
  /// Helper to update the SOR time stamp
  void updSorTime(unsigned long long st);
  /// Helper to extract the single attr list off the SOR CondAttrListCollection
  const coral::AttributeList &
  getSorAttrList(const CondAttrListCollection * sor) const;
  /// Update the HLTConfigSvc (only required to succeed with DB based config)
  StatusCode updHLTConfigSvc();
  /// Set the EventInfo in the event store and return it
  const EventInfo * prepEventInfo() const;
  /// Set the xAOD::EventInfo in the event store
  StatusCode prepXAODEventInfo() const;
  /// Update the magnet field from IS when necessary and possible
  StatusCode updMagField(const boost::property_tree::ptree& pt) const;
  /// Reset selected proxies / IOV folders
  StatusCode resetCoolValidity();
  /// Helper to log a message with some details when ready to run
  void readyMsg() const;
  /// Helper to fill in HLTResult with stream tags
  void setStreamTags(hltinterface::HLTResult& hltr,
                  const std::vector<TriggerInfo::StreamTag>& stream_tags) const;
  void mergeStreamTags(hltinterface::HLTResult& hltr,
                     const std::vector<xAOD::EventInfo::StreamTag>& xsts) const;
  /// get the right hlt decision, setting debug stream tag if necessary
  hltonl::AcceptFlag processHltDecision(hltinterface::HLTResult& hltr) const;
  /// Serializes Steering's result into hltinterface's result robs
  hltonl::PSCErrorCode serializeRobs(hltinterface::HLTResult& hltr, bool& serOk,
                                     HLT::HLTResult* dobj, uint32_t rn,
                                     uint32_t l1id, uint32_t bcid,
                                     uint32_t l1tt, uint32_t dett);
  // Check whether a rob fits the space left. If not, issue an error
  bool checkRobSize(uint32_t robsize, uint32_t spaceleft, uint32_t maxsize);
  // Check consistency of the info received in the EventId and the CTP fragment
  bool checkEventIdConsistency(const hltinterface::EventId& evId) const;
  // serialize rob from steering's HLTResult
  bool serializeRob(uint32_t*& tmpstor,
                    eformat::write::ROBFragment& rob,
                    HLT::HLTResult& dobj,
                    unsigned int robid,
                    int payload_max);
  // add rob to hltinterface::HLTResult
  void addRobToHLTResult(hltinterface::HLTResult& hltr,
                         eformat::write::ROBFragment& rob,
                         uint32_t*& next_fragment,
                         uint32_t& spaceleft); /* don't pass original
                         fragment_pointer or max_result_size (notice parameters
                         passed by reference) */
  // Get monitoring information for navigation sizes of HLT EDM
  void recordEDMSizeInfo(size_t nav_size, bool serializationOk) const;
  // check if a ROB is enabled for readout in OKS
  bool isRobEnabled(uint32_t robid) const;
  // check if a Sub Detector is enabled for readout in OKS
  bool isSubDetectorEnabled(uint32_t subdetid) const;
  // filter a set of robs according to whether or not they are enabled
  std::set<uint32_t> filterRobs(const std::set<uint32_t>& robs) const;
  // filter a set of dets according to whether or not they are enabled
  std::set<eformat::SubDetector>
  filterDets(const std::set<uint32_t>& dets) const;

  // act on failure to process event
  void failedEvent(hltinterface::HLTResult& hlt_result,
                   hltonl::PSCErrorCode ecode,
                   const std::string& emsg,
                   bool empty_result = true);

  // get a vector with the ids of the expected L1R robs that are missing
  std::vector<uint32_t>
  missingL1Robs(const std::vector<eformat::ROBFragment<const uint32_t*>>& l1r)
  const;

  // check whether a subdetector is in the run, according to the current detmask
  bool isSubDetectorIn(eformat::SubDetector sd) const;

  /** Handles to required services/tools **/
  typedef ServiceHandle<IIncidentSvc> IIncidentSvc_t;
  IIncidentSvc_t         m_incidentSvc;

  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
  StoreGateSvc_t         m_evtStore;
  StoreGateSvc_t         m_detectorStore;
  StoreGateSvc_t         m_inputMetaDataStore;

  typedef ServiceHandle<IROBDataProviderSvc> IIROBDataProviderSvc_t;
  IIROBDataProviderSvc_t m_robDataProviderSvc;

  typedef ServiceHandle<ITHistSvc> ITHistSvc_t;
  ITHistSvc_t            m_THistSvc;

  ToolHandle<TrigISHelper>            m_isHelper;
  ToolHandle<TrigCOOLUpdateHelper>    m_coolHelper;

  /** Pointers to optional services/tools **/
  TrigConf::IHLTConfigSvc* m_hltConfigSvc{0};
  IAlgContextSvc*          m_algContextSvc{0};

  IntegerProperty          m_lvl1CTPROBid ;       // Source ID for CTP ROB fragment
  BooleanProperty          m_doMonitoring;        // Monitoring

  /// Helper classes for error codes
  hltonl::MapPscErrorCode  m_mapPscError;
  hltonl::MapResultStatusCode m_mapResultStatus;
  hltonl::MapAcceptFlag       m_mapAccept;

  EventID::number_type      m_currentRun{0};   //!< current run number
  EventID::number_type      m_currentLB{0};    //!< current lumiblock
  const EventInfo*          m_currentEvent{0}; //!< current EventInfo object in StoreGate
  
  /// Start of Run Time: posix time in seconds since 1970/01/01
  /// Start of Run Time: time stamp ns - ns time offset for time_stamp, 32 bit unsigned
  std::vector<EventID::number_type> m_sorTime_stamp;

  /// detector mask0,1,2,3 - bit field indicating which TTC zones have been
  /// built into the event, one bit per zone, 128 bit total
  /// significance increases from first to last
  typedef EventID::number_type numt;
  std::tuple<numt, numt, numt, numt> m_detector_mask;

  uint32_t                  m_l1_hltPrescaleUpdateLB;    //!< LB of prescale update from CTP fragment
  
  // --------------------------- Monitoring Histograms --------------------------
  TH1F*                     m_hist_eventAcceptFlags{0};            //!< Accept flags for processed events
  TH1F*                     m_hist_frameworkErrorCodes{0};         //!< PSC error codes
  TH1F*                     m_hist_Hlt_result_size{0};             //!< size of HLT result
  TH1F*                     m_hist_Hlt_result_status{0};           //!< Status flags in HLT result
  TH1F*                     m_hist_numStreamTags{0};               //!< Number of StreamTags from HLT
  TH1F*                     m_hist_streamTagTypes{0};              //!< StreamTag types from HLT   
  TH1F*                     m_hist_streamTagNames{0};              //!< StreamTag names from HLT
  TH1F*                     m_hist_num_partial_eb_robs{0};         //!< Number of ROBs for partial event building from HLT   
  TH1F*                     m_hist_num_partial_eb_SubDetectors{0}; //!< Number of SubDetectors for partial event building from HLT 
  TH1F*                     m_hist_partial_eb_SubDetectors_ROBs{0};//!< SubDetectors for partial event building derived from ROB list
  TH1F*                     m_hist_partial_eb_SubDetectors_SDs{0}; //!< SubDetectors for partial event building derived from SubDetector list
  TH1F*                     m_hist_Hlt_result_size_physics{0};           //!< size of HLT result in physics Main
  TH1F*                     m_hist_Hlt_result_size_express{0};           //!< size of HLT result in express stream
  TH1F*                     m_hist_Hlt_result_size_DataScouting{0};      //!< size of HLT result in express stream
  TProfile*                 m_hist_HltResultSizes_Stream_physics{0};     //!< HLT Result sizes for physiscs streams
  TProfile*                 m_hist_HltResultSizes_Stream_DataScouting{0};//!< HLT Result sizes for DataScouting streams
  TProfile*                 m_hist_HltEdmSizes_No_Truncation{0};   //!< HLT EDM sizes for all events without a truncated HLT result 
  TProfile*                 m_hist_HltEdmSizes_With_Truncation{0}; //!< HLT EDM sizes for all events with a truncated HLT result 
  TProfile*                 m_hist_HltEdmSizes_TruncatedResult_Retained_Collections{0}; //!< HLT EDM sizes for all collections which were retained in a truncated HLT result
  TProfile*                 m_hist_HltEdmSizes_TruncatedResult_Truncated_Collections{0};//!< HLT EDM sizes for all collections which were truncated in a truncated HLT result
  // --------------------------- Properties -------------------------------------
  BooleanProperty           m_setMagFieldFromPtree; //!< Read magnet currents from ptree
  StringProperty            m_applicationName;      //!< Application Name (="None" or "athenaHLT" for simulated data, "HLTMPPU-xx"? in online environment) */
  StringProperty            m_partitionName;        //!< Partition Name (="None" for offline, real partion name in online environment)
  BooleanProperty           m_forceHltReject;      // force reject of all events 
  BooleanProperty           m_forceHltAccept;      // force accept of all events
  StringProperty            m_HltResultName;       // name of HLT result in StoreGate
  StringProperty            m_HltDebugStreamName;  // stream name for Debug events 
  StringProperty            m_HltForcedStreamName; // stream name for forced accept events
  IntegerProperty           m_predefinedLumiBlock;
  IntegerProperty           m_prepareForRunSleep;    //!< max random sleep during prepareForRun in sec

  typedef SimpleProperty< std::vector<uint32_t> > Uint32ArrayProperty;
  Uint32ArrayProperty       m_enabledROBs;         //!< list of all enabled ROBs which can be retrieved
  Uint32ArrayProperty       m_enabledSubDetectors; //!< list of all enabled Sub Detectors which can be retrieved
  Uint32ArrayProperty       m_mandatoryL1ROBs;     //!< list of mandatory ROB IDs coming from the RoIB (must come in L1R seed)

  StringArrayProperty       m_hltEdmCollectionNames; //!< names of all HLT EDM collections for histogram label 
  
  StringProperty            m_jobOptionsType;        //!< JobOptions type (="NONE" or "DB", same as in PSC)

  Histo1DProperty           m_histProp_Hlt_result_size;
  Histo1DProperty           m_histProp_Hlt_result_size_physics;
  Histo1DProperty           m_histProp_Hlt_result_size_express;
  Histo1DProperty           m_histProp_Hlt_result_size_DataScouting;
  Histo1DProperty           m_histProp_numStreamTags;              
  Histo1DProperty           m_histProp_streamTagNames;  
  Histo1DProperty           m_histProp_num_partial_eb_robs;
  Histo1DProperty           m_histProp_Hlt_Edm_Sizes;       //!< HLT EDM sizes profile plots 

  int                       m_total_evt{0};
  int                       m_failed_evt{0};
  int                       m_invalid_lvl1_result{0};
  int                       m_invalid_hlt_result{0};
  int                       m_truncated_hlt_result{0};
  int                       m_truncated_hlt_result_to_debug{0};
  int                       m_truncated_hlt_result_not_to_debug{0};

  // for CTP Lvl1 Rob
  std::vector<uint32_t>     m_ctpRobIdVec ;
  // event number - 32 bit unsigned
  uint32_t                  m_lvl1id{0}  ;
  // run number - 32 bit unsigned
  EventID::number_type      m_run_no{0} ;
  // bunch crossing ID,  32 bit unsigned
  EventID::number_type      m_bunch_crossing_id{0} ;
  // time stamp - posix time in seconds from 1970, 32 bit unsigned
  EventID::number_type      m_time_stamp{0} ;
  // time stamp ns - ns time offset for time_stamp, 32 bit unsigned
  EventID::number_type      m_time_stamp_ns_offset{0} ;
  // luminosity block identifier, 32 bit unsigned
  EventID::number_type      m_lumi_block{0} ;
  // status element
  TriggerInfo::number_type  m_l1_Status_Element{0} ;
  // LVL1 trigger type
  TriggerInfo::number_type  m_l1_Trigger_Type{0} ;
  // LVL1 trigger info
  std::vector<TriggerInfo::number_type> m_l1_Trigger_Info ;
  // LVL1 detev type
  uint32_t                  m_l1_detev_type{0} ;

  /// Reference to a THistSvc which implements also the Hlt additions
  SmartIF<IHltTHistSvc>     m_hltTHistSvc;

  /// Reference to a ROBDataProviderSvc which implements also the Hlt additions
  SmartIF<ITrigROBDataProviderSvc> m_hltROBDataProviderSvc;

  /// Check integrity of CTP ROB and put event to debug stream when it fails
  BooleanProperty           m_lvl1CTPROBcheck ;

  /// Monitoring
  TH1F*                     m_hist_l1_robs{0};

  /// Flag to write events with truncated HLT result to a special debug stream
  BooleanProperty           m_writeHltTruncationToDebug ;
  /// Debug stream name for events with a truncated HLT result 
  StringProperty            m_HltTruncationDebugStreamName; 
  /// Stream names for which events should be not send to the debug stream even when the HLT result is truncated
  StringArrayProperty       m_excludeFromHltTruncationDebugStream; 
  /// Monitoring histogram for truncated HLT results
  TH1F*                     m_hist_Hlt_truncated_result{0};

  /// we need this maintain the data
  uint32_t                  m_status_words[3] = {0};

};

//=========================================================================
inline StatusCode HltEventLoopMgr::start()
{
  // This method might be called by ServiceManager::start and does nothing.
  // The real start method is prepareForRun and invoked by the PSC.
  return StatusCode::SUCCESS;
}

//=========================================================================
inline void
HltEventLoopMgr::HltSetPscError(hltinterface::HLTResult& hlt_result,
                                hltonl::PSCErrorCode pscErrorCode) const
{
  hlt_result.psc_errors.push_back(pscErrorCode);
}

#endif // TRIGSERVICES_HLTEVENTLOOPMGR_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINTERFACES_Algo_H
#define TRIGINTERFACES_Algo_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigInterfaces/AlgoConfig.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "TrigStorageDefinitions/EDM_TypeInfo.h"

#include "TrigNavigation/Holder.h"
#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/NavigationCore.h"

#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"

class ISvcLocator;
class ITrigTimerSvc;
class TrigTimer;
class StoreGateSvc;
#include "AthenaMonitoring/IMonitorToolBase.h"

#include <map>
using std::map;

#include <string>
using std::string;

namespace HLT
{
  class TriggerElement;
  class AlgoConfig;
  class ComboIterator;

  typedef std::vector<TriggerElement*> TEVec;
  typedef std::vector<const TriggerElement*> TEConstVec;

  namespace localns {
    static const std::vector<unsigned int> emptyUintVector;
  }


  /**
   * @brief Base class for HLT algorithms.
   *
   * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
   * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
   *
   * This is the base class for all types of HLT algorithms.
   *
   * The public virtual interface of this class includes methods to be re-implemented by algorithm
   * developers. All the operations performed by an algorithm at each state transition must be encapsulated
   * in one of the following methods:
   *
   * - hltInitialize()
   * - hltStart()
   * - hltBeginRun()
   * - hltEndRun()
   * - hltStop()
   * - hltFinalize()
   *
   * The interface for these methods is common for all algorithm types and is thus defined in this base class.
   * The interface for the hltExecute() method, which is called at each algorithm execution, is different for each
   * type of algorithm and is consequently defined in the corresponding derived classes (FexAlgo, HypoAlgo,
   * ComboAlgo, AllTEAlgo).
   *
   * The methods listed above are tightly coupled to the Gaudi and, for online running, the
   * TDAQ state machine. Code that should be executed once at the beginning and the end of the job
   * should be added to hltInitialize() and hltFinalize(), respectively. The remaining methods
   * are called at different times for online and offline running:
   *
   * \par Call sequence for OFFLINE running
   * The call sequence for offline running is as follows:   
     \verbatim
        hltInitialize -> hltStart -> [hltBeginRun -> hltEndRun]+ -> hltStop -> hltFinalize   \endverbatim   
   * For each run-number change the hltBeginRun() and hltEndRun() methods are called. hltStart() and
   * hltStop() are called once at the beginning and end of the job.
   *
   * \par Call sequence for ONLINE running
   * The call sequence for online running is as follows:
     \verbatim
        hltInitialize -> [hltStart -> hltBeginRun -> hltEndRun -> hltStop]+ -> hltFinalize   \endverbatim
   * Compared to offline, the hltStart() and hltStop() methods are called at the beginning and
   * end of each run, i.e. at every run-number change. This is to allow for resetting of
   * monitoring histograms, etc. for each run.
   *
   * As a general rule, code that has to be executed once per run in the online but once per job in
   * the offline (e.g. histogram booking) should be put into hltStart() and hltStop(). All other
   * run-dependent code should be put into hltBeginRun() and hltEndRun().
   */

  class Algo : public AthAlgorithm, public IMonitoredAlgo {

  public:

    /**
     * @brief Constructor; gets the algorithm name and service locator as arguments.
     */
    Algo(const std::string& name, ISvcLocator* pSvcLocator);

    /**
     * @brief Destructor.
     */
    virtual ~Algo();

    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the Initialize state transition.
     * @return HLT::ErrorCode for the Initialize transition.
     *
     * This method is called from within initialize() in the base class.
     */
    virtual HLT::ErrorCode hltInitialize() = 0;

    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the Start state transition.
     * @return HLT::ErrorCode for the Start transition.
     *
     * Offline this method is called once per job just before the first event.
     * Online it is called at the start of every run (i.e. run-number change)
     * just before hltBeginRun().
     */
    virtual HLT::ErrorCode hltStart() { return HLT::OK; }
    
    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the BeginRun state transition.
     * @return HLT::ErrorCode for the BeginRun transition.
     *
     * This method should contain code that is exectuded on each run-number change
     * for both online and offline running.
     */
    virtual HLT::ErrorCode hltBeginRun() { return HLT::OK; }

    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the EndRun state transition.
     * @return HLT::ErrorCode for the EndRun transition.
     *
     * This method should contain code that is exectuded on each run-number change
     * for both online and offline running.
     */
    virtual HLT::ErrorCode hltEndRun() { return HLT::OK; }

    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the Stop state transition.
     * @return StatusCode for the Stop process.
     *
     * Offline this method is called once per job after the last event.
     * Online it is called at the end of each run (i.e. run-number change)
     * just after hltEndRun().
     */
    virtual HLT::ErrorCode hltStop() { return HLT::OK; }

    
    /**
     * @brief Method to be redefined by the user to implement the actions performed
     *        by the algorithm at the Finalize state transition.
     * @return HLT::ErrorCode for the Finalize transition.
     *
     * This method is called from within finalize() in the base class.
     */
    virtual HLT::ErrorCode hltFinalize() = 0;


    /**
     * @brief Method processing a single input/output combination of TEs.
     * @return HLT::ErrorCode for the TE processing.
     *
     * This method is called by the protected processTEs for each TE combo.
     */
    virtual HLT::ErrorCode plugin_processTE(TEConstVec& inputTEs, 
                                            TriggerElement* outputTE) = 0;    

    /**
     * @brief Accessor method for the algorithm configuration.
     * @return pointer to the configuration object for this algorithm.
     */
    inline AlgoConfig* config() const { return m_config; }

    /**
     * @brief Accessor method for the algorithm timing flag.
     * @return flag corresponding to the enable/disable of timing measurements for this algorithm.
     */
    inline bool doTiming() const { return m_doTiming; }


    /**
     * @brief Method called at the end of the event.
     */
    virtual HLT::ErrorCode hltEndEvent() { return HLT::OK; };

    /**
     * @brief Method performing algorithm reset.
     *
     * Does nothing.
     */
    virtual bool reset();

    /**
     * @brief Accessor method for the TrigTimerSvc service.
     * @return handle to the TrigTimeSvc service.
     */
    ITrigTimerSvc* timerSvc() const; 

    /**
     * @brief Method providing a new instance of TrigTimer timers.
     * @param label name of the requested timer.
     * @param propName optional name of the associated property.
     * @return handle to the newly created requested timer.
     *
     * This method has to be used by developers to request new TrigTimer timers for each of the
     * algorithmic parts they want to measure.
     * A unique name must be associated to each timer; an optional property, usually needed for timing
     * normalization, can be specified.
     */
    TrigTimer* addTimer(std::string label, std::string propName = "");  

    /**
     * @brief Accessor method for TrigTimer timers.
     * @return handle to the requested timer.
     *
     * This method has to be used by developers to request an handle to a TrigTimer timer which
     * has already been requested with the addTimer method; the returned pointer shouldn't be used
     * directly, as in timer("myTimer")->start(), because this method implies a std::map lookup.
     */
    TrigTimer* timer(std::string label) const; 
    TrigTimer* totalTimeTimer() const; 

    /**
     * @brief Accessor method for the message level variable.
     * @return value of the message level for this algorithm.
     */
    inline unsigned int msgLvl() const { return msg().level(); }
    using AthAlgorithm::msgLvl;   // import the msgLvl(MSG::Level) version

    friend class TrigSteer;
    friend class Sequence;

  protected:

    /**
     * @brief Method performing the Initialize transition.
     * @return StatusCode for the Initialize process.
     *
     * Retrieves the StoreGateSvc and TrigTimerSvc services and calls the public hltInitialize method; not to be re-implemented/used by developers.
     */
    StatusCode initialize();

    /**
     * @brief Method performing the Finalize transition.
     * @return StatusCode for the Finalize process.
     *
     * Calls the public hltFinalize method; not to be re-implemented/used by developers.
     */
    StatusCode finalize();

    /**
     * @brief Method performing algorithm execution.
     * @return always returns StatusCode::SUCCESS.
     *
     * Does nothing; not to be re-implemented/used by developers.
     */
    StatusCode execute() { return StatusCode::SUCCESS; }

    /**
     * @brief Method performing the Start transition
     * @return StatusCode for the Start process.
     *
     * Calls the public hltStart method; not to be re-implemented/used by developers.
     */
    StatusCode start();
    
    /**
     * @brief Method performing the BeginRun transition.
     * @return StatusCode for the BeginRun process.
     *
     * Calls the public hltBeginRun method; not to be re-implemented/used by developers.     
     */
    StatusCode beginRun();

    /**
     * @brief Method performing the EndRun transition.
     * @return StatusCode for the EndRun process.
     *
     * Calls the public hltEndRun method; not to be re-implemented/used by developers.
     */
    StatusCode endRun();

    /**
     * @brief Method performing the Stop transition
     * @return StatusCode for the Stop process.
     *
     * Calls the public hltStop method; not to be re-implemented/used by developers.
     */
    StatusCode stop();
    
    
    /**
     * @brief Interface method called by HLT::Sequence for each algorithm in the sequence.
     * @return HLT::ErrorCode for the TE processing.
     * @param outputTEs output TEs from the previous algorithm executed in the sequence.
     *
     * Retrieves and loops on output TEs from the previous algorithm in the sequence,
     * calling the processTE method for each of them.
     */
    virtual HLT::ErrorCode processTEs(TEVec& outputTEs);


    /**
     * @brief Interface method for creating and processing TEs.
     * @return HLT::ErrorCode for TE creation and processing.
     * @param inputs vector of input TE types.
     * @param output TE type.
     * @param outputTEs vector of newly created output TE pointers.
     *
     * This method is called by HLT::Sequence to create output TEs and run the algorithm;
     * on its turn it calls the plugin_createAndProcessTEs method. In case the algorithm
     * is not to be run, already existing output TEs are retrieved and returned.
     */
    virtual HLT::ErrorCode createAndProcessTEs(std::vector<unsigned int>& inputTypes, 
					       unsigned int outputType,
					       TEVec& outputTEs, 
					       const std::vector<unsigned int>& topologicalStartFrom=localns::emptyUintVector) = 0;


    /**
     * @brief Accessor method for the algorithm rerun flag.
     * @return value of the re-run flag for this algorithm.
     *
     * Method used by the Steering to determine whether this algorithm should re-run every time
     * the trigger is re-run (e.g. for an HypoAlgo) or not (e.g. for a FexAlgo). 
     *
     * The re-run flag is false by default.
     */
    virtual bool runAlgo() const { return !m_isReRunning; }

    /**
     * @brief Helper method transforming vectors of TEs into const vectors of TEs.
     * @return const vector of TEs.
     * @param vec vector of TEs to be transformed.
     *
     * Transforms a vector of TEs, passed by reference, into a static const vector of TEs.
     */
    static TEConstVec constify(const TEVec& vec);

    /**
     * @brief Accessor method for the algorithm configuration.
     * @parameter cfg new configuration object for this algorithm.
     */
    void setConfig(AlgoConfig* cfg) { m_config = cfg; }

    AlgoConfig* m_config;

    /**
     * @brief Method used to retrieve single features attached to a TE.
     * @param te pointer to the TE from which the feature has to be retrieved.
     * @param feature reference to the requested feature.
     * @param label optional label identifying the feature.
     * @return HLT::ErrorCode for the request.
     * @warning A return status equal to HLT::OK does not mean that an object/feature was found. 
     *
     * This method has to be used by developers to request a feature already attached to a TE. An optional
     * label can be specified to correctly identify the feature in case more of the same type are available.
     *
     * If a feature of the required type is not attached to given TE, the structure is scanned backwards;
     * in particular, all the TEs related by "seeded_by" relations are checked, until the required object is found. 
     * If no objects/feature are found, the return pointer is set to "0". If the label is provided, only objects
     * labelled with it are considered. A return status different from HLT::OK is returned only in case of internal
     * errors in the navigation structure.
     */
    template<class T> HLT::ErrorCode getFeature(const TriggerElement* te, const T*&  feature, 
						const std::string& label = ""); 


    /**
     * As above but fills up the ElementLink given to it 
     * Note that the container type must be given to this method.
     * i.e. usage can be getFeature<MuonFeature, MuonFeatureCollection>(te, EL)
     */
    template<class C, class T> HLT::ErrorCode getFeatureLink(const TriggerElement* te, ElementLink<C>&  feature, 
							     const std::string& label = "");

    /**
     * @brief Method used to retrieve vectors of features attached to a TE.
     * @param te pointer to the TE from which the features have to be retrieved.
     * @param feature reference to the requested vector of features.
     * @param label optional label identifying the features.
     * @return HLT::ErrorCode for the request.
     * @warning A return status equal to HLT::OK does not mean that an object/feature was found. 
     *
     * This method has to be used by developers to request vectors of features already attached to a TE. An
     * optional label can be specified to correctly identify the feature in case more of the same type are
     * available.
     *
     * If a feature of the required type is not attached to given TE, the structure is scanned backwards;
     * in particular, all the TEs related by "seeded_by" relations are checked, until the required object is found. 
     * If no objects/feature are found, the return pointer is set to "0". If the label is provided, only objects
     * labelled with it are considered. A return status different from HLT::OK is returned only in case of internal
     * errors in the navigation structure.
     *
     * In case the request for a feature is executed on a TE produced in a topological selection, the search
     * for the requested features is stopped when the first topological TE is reached.
     */
    template<class T> HLT::ErrorCode getFeatures(const TriggerElement* te, std::vector<const T*>&  feature, 
						 const std::string& label = "");


    /**
     * @brief As getFeatures but uses ELV to return obejects
     * Note that the type and collection in which it is storred must be provided.
     */
    template<class C, class T> HLT::ErrorCode getFeaturesLinks(const TriggerElement* te, ElementLinkVector<C>&  feature, 
							 const std::string& label = "");


    /**
     * @brief Method used to attach a new feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param label label used to identify the attached feature.
     *
     * This method enables to attach a new feature to a TE; this is the method recommended for
     * attaching features in case no StoreGate storage is required.
     */
    template<class T> HLT::ErrorCode attachFeature(TriggerElement* te, T* feature, 
						   const std::string& label = "");



    /**
     * @brief Method used to store a new feature and attach it to a TE.
     * @return HLT::ErrorCode for feature recording and attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     *
     * This method enables to store a new feature and attach it to a TE.
     */
    template<class T> HLT::ErrorCode recordAndAttachFeature(TriggerElement* te, T* feature,
							    std::string& key,
							    const std::string& label = "");

    /**
     * @brief Method used to attach an already stored feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     *
     * This method enables to store a new feature and attach it to a TE.
     */
    template<class T> HLT::ErrorCode reAttachFeature(TriggerElement* te, T* feature,
						     std::string& key,
						     const std::string& label = "");


    /**
     * @brief Method used to obtain a StoreGate key for a feature produced by this algorithm.
     * @param feature pointer to the feature for which the key is requested.
     * @param key reference to the obtained key.
     * @return HLT::ErrorCode for the request.
     *
     * This method has to be used by developers to request keys for storing a feature they produced.
     */
    template<class T> HLT::ErrorCode getStoreGateKey(const T* feature, std::string& key);


    /**
     * @brief Method used to obtain a unique StoreGate key for a feature produced by this algorithm.
     * @param feature pointer to the feature for which the key is requested.
     * @param key reference to the obtained key.
     * @param label optional label identifying the feature.
     * @return HLT::ErrorCode for the request.
     *
     * This method has to be used by developers to request unique keys for storing a feature they produced,
     * optionally identified by a label.
     */
    template<class T> HLT::ErrorCode getUniqueKey(const T* feature, std::string& key,
						  const std::string& label = "");
    /**
     * @brief Accessor method for StoreGateSvc service.
     * @return handle to the StoreGateSvc service.
     *
     * This method has to be used by developers to request an handle to the StoreGateSvc service.
     */
    StoreGateSvc* store() { return evtStore().operator->(); }

    /**
     * @brief Method performing monitoring operations associated to the Initialize transition.
     * @return StatusCode for the monitoring Initialize process.
     *
     * Retrieves the declared monitoring tools.
     */
    StatusCode initializeMonitors();

    /**
     * @brief Method performing monitoring operations associated to the Start transition.
     * @return StatusCode for the monitoring Start process.
     *
     * Books histograms for the declared monitoring tools.
     */
    StatusCode startMonitors();

    /**
     * @brief Method performing monitoring operations associated to the Stop transition.
     * @return StatusCode for the monitoring Stop process, always StatusCode::SUCCESS.
     *
     * Fills full statistics histograms.
     */
    StatusCode stopMonitors();
    
    /**
     * @brief Method performing monitoring operations associated to the BeginRun transition.
     * @return StatusCode for the monitoring BeginRun process.
     *
     * Currently does nothing.
     */
    StatusCode beginRunMonitors();

    /**
     * @brief Method performing monitoring operations associated to the EndRun transition.
     * @return StatusCode for the monitoring EndRun process, always StatusCode::SUCCESS.
     *
     * Currently does nothing.
     */
    StatusCode endRunMonitors();

     /**
     * @brief Method performing monitoring operations before the event.
     * @return StatusCode for the pre-event monitoring process.
     *
     * Calls the Gaudi auditor hooks.
     */
    StatusCode beforeEventMonitors();

    /**
     * @brief Method performing monitoring operations after the event.
     * @return StatusCode for the post-event monitoring process.
     *
     * Calls the Gaudi auditor hooks.
     */
    StatusCode afterEventMonitors();
    
    /**
     * @brief Method performing monitoring operations before algorithm execution.
     * @return StatusCode for the pre-execution process.
     *
     * Currently only starts the total timer for the algorithm.
     */
    StatusCode beforeExecMonitors();

    /**
     * @brief Method performing monitoring operations after algorithm execution.
     * @return StatusCode for the post-execution process.
     *
     * Stops the total timer for the algorithm and fills event-by-event histograms.
     */
    StatusCode afterExecMonitors();

    /**
     * @brief Method performing monitoring operations associated to the Finalize transition.
     * @return StatusCode for the monitoring Finalize process.
     *
     * Currently does nothing.
     */
    StatusCode finalizeMonitors();

    /**
     * @brief Method to add operational infor with timing information to the TE
     */
    void addOperationalInfo(TriggerElement* te);
    bool m_monitoringStarted;

    /**
     * @brief Method to collect operational info for steering
     */
    void addSteeringOperationalInfo(bool wasRun, unsigned int ntes, TriggerElement* te);

    /** @brief Re-running enable/disable flag.*/
    static bool m_isReRunning;

    /** @brief Selection enable/disable flag; only used by HypoAlgo algorithms.*/
    static bool m_isNoSelection;

    /** @brief variable used to store eror code for monitoring purposes (this is passed up to the sequences). */
    HLT::ErrorCode m_returnErrorCode; 

    /** @brief variable used to store unmapped eror code for monitoring purposes. */
    HLT::ErrorCode m_unmappedErrorCode; 

    /** @brief Production of OperationalInfo enable/disable. This is not implemented for the moment. */
    bool m_doOperationalInfo;


  protected:

    /** @brief ErrorCode map inputs **/
    std::map<std::string, std::string>   m_errorCodeMap;
     //     std::map<int, int> m_mapCodeInt;
 
    /** @brief Actual ErrorCode map **/
    std::map<ErrorCode, ErrorCode> m_ecMap;
    unsigned                  m_ecMapSize;
 
    /** @brief Find the mapped return ErrorCode */
    void mapAndRecordError(ErrorCode& errorCode, TriggerElement* te=0);


    

  private:

    
    /** @brief Timing measurement enable/disable flag.*/
    bool m_doTiming;


    /** @brief Gaudi auditor enable/disable flag.*/
    bool m_doAuditors;
    
    /** @brief Pointer to TrigTimerSvc service.*/
    ITrigTimerSvc* m_timerSvc;

    /** @brief Map of timers declared by the developer. */
    std::map<std::string, TrigTimer*> m_timers;

    /** @brief Automatic total execution timer.*/
    TrigTimer* m_totalTime;

    /** @brief List of declared monitoring tools.*/
    ToolHandleArray < IMonitorToolBase > m_monitors;

    /**
     * @brief Method used to attach a new feature to a TE.
     * @return HLT::ErrorCode for feature attaching.
     * @param te pointer to the TE to which the feature must be attached.
     * @param feature pointer to the feature to be attached.
     * @param key store key used to identify the attached feature.
     * @param label label used to identify the attached feature.
     * @param mm memory management for the HLT::Navigation.
     *
     * This method enables to attach a new feature to a TE.
     */
    template<class T> HLT::ErrorCode attachFeature(TriggerElement* te, T* feature,
						   std::string& key,
						   const std::string& label,
						   HLT::Navigation::MemoryManagement mm);

    
  };

  template<class T> HLT::ErrorCode Algo::getFeature(const TriggerElement* te, const T*&  feature, 
						    const std::string& label) 
  {
    feature = 0;
    if (!m_config || !m_config->getNavigation()) return HLT::BAD_JOB_SETUP;

    //    std::vector<const T*> features;
    if (!m_config->getNavigation()->getRecentFeature(te, feature, label)) 
      return HLT::NAV_ERROR;

    // Return the "last" feature.
    //    if (features.size() > 0) feature = features.back();
    return HLT::OK;
  }
  
  template<class T> HLT::ErrorCode Algo::getFeatures(const TriggerElement* te, 
						     std::vector<const T*>&  feature, 
						     const std::string& label) 
  {
    if (!m_config || !m_config->getNavigation()) return HLT::BAD_JOB_SETUP;
    if (!m_config->getNavigation()->getRecentFeatures(te, feature, label))
      return HLT::NAV_ERROR; 

    return HLT::OK;
  }

  template<class C, class T> 
  HLT::ErrorCode Algo::getFeaturesLinks(const TriggerElement* te, ElementLinkVector<C>&  feature, 
					const std::string& label) {
    if (!m_config || !m_config->getNavigation()) return HLT::BAD_JOB_SETUP;
    HLT::Navigation *nav = m_config->getNavigation();
    if (!nav->getRecentFeaturesLinks<C,T>(te, feature, label))
      return HLT::NAV_ERROR; 
    
    return HLT::OK;
  }

  template<class C, class T> 
  HLT::ErrorCode Algo::getFeatureLink(const TriggerElement* te, ElementLink<C>&  feature, 
				      const std::string& label ) {


    if (!m_config || !m_config->getNavigation()) return HLT::BAD_JOB_SETUP;
    HLT::Navigation *nav  = m_config->getNavigation();
    //    ElementLinkVector<C> elv;
    if (!nav->getRecentFeatureLink<C,T>(te, feature, label))
      return HLT::NAV_ERROR; 
    
    // if ( elv.size() != 0 ) feature = elv.back();
    return HLT::OK;
  }


  template<class T> HLT::ErrorCode Algo::getStoreGateKey(const T* feature, 
                                                         std::string& key) 
  {  
    key = "";
    if (!store() ) return HLT::BAD_JOB_SETUP;
    
    SG::DataProxy* proxy = store()->proxy(feature);
    if (!proxy ) return HLT::BAD_JOB_SETUP;
    
    key = proxy->name();

    return HLT::OK;
  }

  template<class T> HLT::ErrorCode Algo::getUniqueKey(const T*,
                                                      std::string& key,
                                                      const std::string& label) 
  {  
    if (!m_config || !m_config->getNavigation()) return HLT::BAD_JOB_SETUP;
    key = m_config->getNavigation()->template getUniqueKey<T>( label );

    if (key == "") return HLT::NAV_ERROR;    
    return HLT::OK;
  }


  template<class T> HLT::ErrorCode 
    Algo::attachFeature(TriggerElement* te, T* feature,	std::string& key,
			const std::string& label, 
			HLT::Navigation::MemoryManagement mm) {    
    if (!feature) {
      msg() << MSG::FATAL << "Cannot attach null feature!" << endmsg;
      return HLT::FATAL;
    }
    
    if (!m_config || !m_config->getNavigation() ||
	!m_config->getNavigation()->attachFeature(te, feature, mm, key, label)) {
      msg() << MSG::ERROR << "Failed to attach feature!" << endmsg;
      return HLT::NAV_ERROR;
    }
    
    return HLT::OK;
  }
  
  template<class T> HLT::ErrorCode 
    Algo::attachFeature(TriggerElement* te, T* feature,	const std::string& label) {

    // not used
    std::string key = "";
    return attachFeature(te, feature, key, label, HLT::Navigation::ObjectCreatedByNew);
  }

  template<class T> HLT::ErrorCode 
    Algo::recordAndAttachFeature(TriggerElement* te, T* feature, std::string& key,
				 const std::string& label) { 

    std::string aliasKey = key;
    HLT::ErrorCode status = attachFeature(te, feature, key, label, HLT::Navigation::ObjectToStoreGate);
    if (status != HLT::OK) return status;

    if (aliasKey == "") return HLT::OK;
    if (!store() || store()->setAlias(feature, aliasKey).isFailure()) 
      return HLT::NAV_ERROR;

    return HLT::OK;
  }

  template<class T> HLT::ErrorCode 
    Algo::reAttachFeature(TriggerElement* te, T* feature, std::string& key,
			  const std::string& label) {
    
    return attachFeature(te, feature, key, label, HLT::Navigation::ObjectInStoreGate);
  }


  
} // end namespace




#endif

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TagInfoMgr.cxx
 *
 * @brief implementation
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "TagInfoMgr.h"

// Event includes
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "EventInfoUtils/EventIDFromStore.h"

// IOVDbSvc
#include "AthenaKernel/IIOVDbSvc.h"

// TES include
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"
#include "AthenaKernel/ExtendedEventContext.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/GaudiException.h"


const std::string IOVCbkObjKey( "TagInfoMgrIOVCBKKey" );


// Constructor with parameters:
TagInfoMgr::TagInfoMgr(const std::string &name, 
                       ISvcLocator *pSvcLocator) :
    AthService(name, pSvcLocator)
{}

TagInfoMgr::~TagInfoMgr() 
{}


/// Identify interfaces to which this service is responsive
StatusCode
TagInfoMgr::queryInterface( const InterfaceID& riid, void** ppvInterface ) 
{
    ATH_MSG_DEBUG("in queryInterface()");

    if ( ITagInfoMgr::interfaceID().versionMatch(riid) ) {
        ATH_MSG_DEBUG("matched ITagInfoMgr");
        *ppvInterface = (ITagInfoMgr*)this;
    }
    else {
        // Interface is not directly available: try out a base class
        return(::AthService::queryInterface(riid, ppvInterface));
    }

    return StatusCode::SUCCESS;
}

// Initialize method:
StatusCode TagInfoMgr::initialize()
{
    ATH_CHECK( AthService::initialize() );

    // Get the messaging service, print where you are
    ATH_MSG_DEBUG( "initialize()");
    
    if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( "ExtraTagValuePairs " );
        for (auto& tv : m_extraTagValuePairs) {
            ATH_MSG_DEBUG( " Value/tag pair: " << tv.first << " " << tv.second );
        }
    }

    // To copy TagInfo to EventInfo, we set listener to the
    // IncidentSvc for BeginRun and BeginInputFile
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());

    // Add BeginRun to trigger tag extraction and writing to output
    // file meta data
    incSvc->addListener( this, "BeginRun", 100);

    // Add BeginInputFile to trigger refilling meta data store after a new input file - priority has
    // to be < 50 to be run after IOVDbMetaDataTool (triggered by MetaDataSvc), which has mergeing
    // into the output file medat data the input meta data of the new file.
    incSvc->addListener(this, "BeginInputFile", 50); // 

    return StatusCode::SUCCESS;
}

// Start method:
StatusCode TagInfoMgr::start()
{
    ATH_MSG_DEBUG( "start()");

    // Register callback to CondAttrListCollection object containing
    // the TagInfo information - only if it exists, i.e. in the file
    // meta data
    if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
        const DataHandle<CondAttrListCollection> tagInfoH;
        if (m_detStore->regFcn(&TagInfoMgr::checkTagInfo, this,
                               tagInfoH, "/TagInfo").isSuccess()) {
          ATH_MSG_DEBUG( "Registered checkTagInfo callback for  /TagInfo ");
        }
        else {
          ATH_MSG_DEBUG("Cannot register checkTagInfo function for /TagInfo ");
        }
    }
    
    return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TagInfoMgr::finalize() 
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( "finalize()");
  return StatusCode::SUCCESS;
}

StatusCode 
TagInfoMgr::fillTagInfo(const CondAttrListCollection* tagInfoCond)
{

    // Fill tag info
    //
    //   We first fill normal tag and input tags with tags coming from
    //   either the EventInfo object or the tag info object coming
    //   from COOL
    //
    //   Then we fill in tags with list of extra tags which are the
    //   current tags for this job. These normally will "over ride"
    //   the tags coming from either COOL/EventInfo. And, of course,
    //   they are not saved as input tags.
    //

    // Get the messaging service, print where you are
    ATH_MSG_DEBUG( "fillTagInfo: ");
    // clear existing tags
    m_tagInfo = TagInfo();

    // Add tags to TagInfo

    if (tagInfoCond) {
        // Coming from COOL
        ATH_MSG_DEBUG( "fillTagInfo: - tags coming from COOL file meta data");

        // tagInfoCond->dump();

        CondAttrListCollection::const_iterator itr     = tagInfoCond->begin();
        if (itr != tagInfoCond->end()) {
            const coral::AttributeList& attrList = (*itr).second;
            coral::AttributeList::const_iterator itAttrList = attrList.begin();
            coral::AttributeList::const_iterator itLast     = attrList.end();
            for (; itAttrList != itLast; ++itAttrList) {
                // Add current tags:
                // -----------------
                // Assumption is that tags from file meta data should
                // be the "first ones in" and there should not be any
                // clash with existing tags. Return WARNING if this
                // happens
                const std::string& name  = (*itAttrList).specification().name();
                const std::string& value = (*itAttrList).data<std::string>();
                // First make sure that this tag name is not on the
                // list of those to be removed
                if (m_tagsToBeRemoved.find(name) == m_tagsToBeRemoved.end()) {
                    if (m_tagInfo.addTag(EventType::NameTagPair(name, value)).isFailure()) {
                        ATH_MSG_WARNING( "fillTagInfo: Unable to add value/tag to TagInfo " 
                              << name << " " << value);
                    }
                    ATH_MSG_DEBUG( "fillTagInfo: Added name/tag to TagInfo " 
                      << name << " " << value);
                }
                else
                   ATH_MSG_DEBUG( "fillTagInfo: Did NOT add tag - on remove list: name/tag: " 
                                  << name << " " << value);

                // Duplicate as input tags:
                // ------------------------
                // Assumption is that tags from file meta data should
                // be the "first ones in" and there should not be any
                // clash with existing tags. Return WARNING if this
                // happens
                if (m_tagInfo.addInputTag(EventType::NameTagPair(name, value)).isFailure()) {
                    ATH_MSG_WARNING( "fillTagInfo: Unable to add value/tag to TagInfo as input tag " 
                          << name << " " << value);
                }
                ATH_MSG_DEBUG( "fillTagInfo: Added value/tag to TagInfo as input tag " 
                          << name << " " << value);
            }
        }
        else {
            // error! 
            ATH_MSG_DEBUG( "fillTagInfo:  Could not get attribute list");      
            return (StatusCode::FAILURE);
        }
    }
    else {
        // **********************************************************
        // Get tags from EventInfo/EventType  
        // **********************************************************
        // ***** NOTE: THIS PART IS FOR BACKWARDS COMPATIBILITY *****
        // *****       TAGS ARE NOW WRITTEN TO FILE META DATA   *****
        // *****       THIS CAN BE REMOVED WHEN NO FILES ARE    *****
        // *****        READ WITH TAGS IN EVENT INFO            *****
        // *****        RDS 04/2009                             *****
        // **********************************************************

       //MN: FIX:  Is this case still in use? If not, remove it
       
        ATH_MSG_DEBUG( "fillTagInfo: Add in tags from EventInfo");
        const DataHandle<EventInfo> evtH;
        const DataHandle<EventInfo> evtHEnd;
        if (m_storeGate->retrieve( evtH, evtHEnd ).isFailure() ) {
            // For simulation, we may be in the initialzation phase
            // and we cannot get EventInfo. We simply skip this step,
            // assuming that most times the information is coming in
            // via conditions/file meta data.
            ATH_MSG_DEBUG( "fillTagInfo:  Could not get event info - skipping the filling of TagInfo from input EventInfo");      
        }
        else {
            if (evtH == evtHEnd) {
                ATH_MSG_ERROR( "fillTagInfo: No event info objects");
                return (StatusCode::FAILURE);
            }
            ATH_MSG_DEBUG( "fillTagInfo: Event ID: ["
                  << evtH->event_ID()->run_number()   << ","
                  << evtH->event_ID()->event_number() << ":"
                  << evtH->event_ID()->time_stamp() << "] " );
            EventType::NameTagPairVec pairs1;
            evtH->event_type()->get_detdescr_tags(pairs1);

            if(pairs1.size()) {
                ATH_MSG_DEBUG( "fillTagInfo: Pairs from EventType:");
            }
            else {
                ATH_MSG_DEBUG( "fillTagInfo: EventInfo/EventType has no tags");
            }
            for (unsigned int i = 0; i < pairs1.size(); ++i) {
                ATH_MSG_DEBUG( "fillTagInfo: " << pairs1[i].first << " : "
                      << pairs1[i].second
                     );
                // Add current tags:
                // -----------------
                // Assumption is that EventInfo tags should be the
                // "first ones in" and there should not be any clash
                // with existing tags. Return ERROR if this happens

                // First make sure that this tag name is not on the
                // list of those to be removed
                std::string& name = pairs1[i].first;
                if (m_tagsToBeRemoved.find(name) == m_tagsToBeRemoved.end()) {
                    if (m_tagInfo.addTag(pairs1[i]).isFailure()) {
                        ATH_MSG_ERROR( "fillTagInfo: Unable to add tag to TagInfo: name/tag " 
                              << pairs1[i].first << " : "
                              << pairs1[i].second);
                        return (StatusCode::FAILURE);
                    } 
                    else ATH_MSG_DEBUG( "fillTagInfo: Added name/tag to TagInfo " 
                                        << pairs1[i].first << " : "
                                        << pairs1[i].second );
                } else
                   ATH_MSG_DEBUG( "fillTagInfo: Did NOT add tag - on remove list: name/tag: " 
                                  << pairs1[i].first << " : "
                                  << pairs1[i].second );

                // Duplicate as input tags:
                // ------------------------
                // Assumption is that EventInfo tags should be the
                // "first ones in" and there should not be any clash
                // with existing tags. Return ERROR if this happens
                if (m_tagInfo.addInputTag(pairs1[i]).isFailure()) {
                    ATH_MSG_ERROR( "fillTagInfo: Unable to add input tag to TagInfo: name/tag " 
                        << pairs1[i].first << " : "
                        << pairs1[i].second);
                    return (StatusCode::FAILURE);
                } 
            }
            ATH_MSG_DEBUG( "fillTagInfo: Added EventInfo tags to TagInfo current/input");

        }
    }
    
    // Dump out contents of TagInfo
    ATH_MSG_DEBUG( "fillTagInfo: print out tags before adding extra tags");
    ATH_MSG_DEBUG(m_tagInfo.str());


    // Add in any extra tag value pairs if specified
    std::map<std::string,std::string> valueTagPairs(m_extraTagValuePairs);
    // Add in pairs from interface
    for (auto & tv : m_extraTagValuePairsViaInterface) {
      valueTagPairs[tv.first]=tv.second;
    }

    for (auto& tv : valueTagPairs) {
      ATH_MSG_DEBUG( "fillTagInfo: Adding extra value/tag pair: " << tv.first << " " << tv.second);
      if (m_tagInfo.addTag(EventType::NameTagPair(tv.first,tv.second), true).isFailure()) {
	ATH_MSG_WARNING( "fillTagInfo: Extra value/tag not added to TagInfo ");
      }
    }

    // Dump out contents of TagInfo
    ATH_MSG_DEBUG( "fillTagInfo: print out tags");
    ATH_MSG_DEBUG(m_tagInfo.str());
    
    return StatusCode::SUCCESS;

}


StatusCode
TagInfoMgr::fillMetaData(const CondAttrListCollection* tagInfoCond)
{
    // fillMetaData is called at the beginning of the job and for each
    // new file being read in.
    //
    // Fill IOV object in metadata store with information from tag
    // info. 
    //
    // For the IOV we use the IOV from the input conditions. For the first input
    // file, we use the incoming IOV if the current run falls within the IOV
    // range. If not we set the IOV to runNumber to runNumber + 1. For
    // subsequent input files, if the runNumber falls within the previous IOV
    // range, then we set the new IOV to [runNumber, lastStop), otherwise the
    // IOV is set to [runNumber, runNumber+1). This latter is used for the
    // zerobias overlaying.
    //

    ATH_MSG_DEBUG( "entering fillMetaData");

    // Get run number for IOV
    EventIDBase::number_type runNumber = EventIDBase::UNDEFNUM;

    if( m_currentRun != EventIDBase::UNDEFNUM ) {
        runNumber = m_currentRun;
    }
    else if( m_conditionsRun != EventIDBase::UNDEFNUM ) {
        // Not completely sure of the use-case for the setting of the conditions run number, but
        // this will be used if the current run number has not been set. RDS 2020/08
        runNumber = m_conditionsRun;
        ATH_MSG_INFO( "fillMetaData: Using conditions run number: " << m_conditionsRun << " rather then current run number: " << m_currentRun);
    } else {
       // For simulation, we may be in the initialization phase and
       // must get the run number from the event selector
       if (StatusCode::SUCCESS != getRunNumber (runNumber)) {
           ATH_MSG_ERROR( "fillMetaData:  Could not get event info neither via retrieve nor from the EventSelectror");      
           return (StatusCode::FAILURE);
       }
    }
    
    // Copy tags to AttributeList
    coral::AttributeList attrList;
    EventType::NameTagPairVec pairs;
    m_tagInfo.getTags(pairs);
    ATH_MSG_DEBUG( "fillMetaData: Adding value/tag pairs to file meta data: ");
    for (unsigned int i = 0; i < pairs.size(); ++i) {
        attrList.extend(pairs[i].first, "string");
        attrList[pairs[i].first].setValue(pairs[i].second);
        ATH_MSG_DEBUG( pairs[i].first << " " << pairs[i].second);
    }
    
    // Fill collection
    CondAttrListCollection* attrListColl = new CondAttrListCollection(true);
    attrListColl->add(0, attrList);
    // Set IOV:

    if (tagInfoCond) {
        // set to the IOV of the incoming conditions
        //  if run num is in previous IOV, set new IOV to be [runNum, lastStop)
        IOVTime testTime(runNumber, 0);
        IOVTime start = m_lastIOVRange.start();
        IOVTime stop  = m_lastIOVRange.stop();
        bool isFirstIOVCheck = false;
        if (!start.isValid() || !stop.isValid()) {
            // Start of job, set lastIOVRange to the incoming IOV
            IOVRange minRange = tagInfoCond->minRange();
            start = minRange.start();
            stop  = minRange.stop();
            ATH_MSG_DEBUG( "IOV minRange from IOVMetaData: " << tagInfoCond->minRange());
            m_lastIOVRange = IOVRange(start, stop);
            isFirstIOVCheck = true;
        }
        if (m_lastIOVRange.isInRange(testTime)) {
            // set start to runNumber after the 
            if (!isFirstIOVCheck) start = testTime; 
            ATH_MSG_DEBUG( "fillMetaData: run number is in previous IOVRange: " << runNumber << " " << m_lastIOVRange);
        }
        else {
            // Out of range
            start = testTime;
            stop  = IOVTime(runNumber + 1, 0);
            ATH_MSG_DEBUG( "fillMetaData: run number is outside of previous IOVRange: " << runNumber << " " << m_lastIOVRange << ". Reset range to run number.");
        }
        attrListColl->addNewStart(start);
        attrListColl->addNewStop (stop);
        m_lastIOVRange = IOVRange(start, stop);
        ATH_MSG_DEBUG( "fillMetaData: start, stop: " << start << " " << stop);
    }
    else {
        // set to the IOV of this run to run+1
        attrListColl->addNewStart(IOVTime(runNumber, 0));
        attrListColl->addNewStop(IOVTime(runNumber + 1, 0));
    }
    
    /// Register folder in the IOV Db MetaData
    if (StatusCode::SUCCESS != m_metaDataTool->registerFolder("/TagInfo")) {
        ATH_MSG_ERROR( "fillMetaData: Unable to register folder for TagInfo with meta data tool ");
        return StatusCode::FAILURE;
    }

    // Add payload
    if (StatusCode::SUCCESS != m_metaDataTool->addPayload("/TagInfo", attrListColl)) {
        ATH_MSG_ERROR( "fillMetaData: Unable to register folder for TagInfo with meta data tool ");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;

}


StatusCode          
TagInfoMgr::getRunNumber (unsigned int& runNumber)
{
    // check if EventSelector is being used to override run numbers if
    // so, we return the runNumber from the properties of the
    // EventSelector

    // Get run number parameter from the EventSelector 
    ATH_MSG_DEBUG( "getRunNumber: check if tag is set in jobOpts");
    // Get name of event selector from the application manager to
    // make sure we get the one for MC signal events
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("ApplicationMgr", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR( "getRunNumber: Cannot get ApplicationMgr "); 
        return StatusCode::FAILURE;
    }
    StringProperty property("EvtSel", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        ATH_MSG_ERROR( "getRunNumber: unable to get EvtSel: found " << property.value());
        return StatusCode::FAILURE;
    }
    // Get EventSelector for ApplicationMgr
    std::string eventSelector = property.value();
    sc = serviceLocator()->service(eventSelector, propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR( "getRunNumber: Cannot get EventSelector " << eventSelector); 
        return StatusCode::FAILURE;
    }
    BooleanProperty overrideRunNumber = BooleanProperty("OverrideRunNumber", false);
    sc = propertyServer->getProperty(&overrideRunNumber);
    if (!sc.isSuccess()) {
        // Not all EventSelectors have this property, but we should
        // not get here if the ES is not one for simulation => return failure
        ATH_MSG_WARNING( "getRunNumber: unable to get OverrideRunNumber property from EventSelector ");
        return StatusCode::FAILURE;
    }
    if (overrideRunNumber.value()) {
        IntegerProperty runNumberProp = IntegerProperty("RunNumber", 0);
        sc = propertyServer->getProperty(&runNumberProp);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR( "getRunNumber: unable to get RunNumber from EventSelector: found " 
                << runNumberProp.value());
            return StatusCode::FAILURE;
        }
        else {
            runNumber = runNumberProp.value();
            ATH_MSG_DEBUG( "getRunNumber: Run number:  "
                  << runNumber << " obtained from "
                  << eventSelector);
        }
    }
    else {
        ATH_MSG_ERROR( "getRunNumber: OverrideRunNumber from EventSelector is false ");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}



void 
TagInfoMgr::handle(const Incident& inc) {

    /**
    ** This method is called at the both the BeginRun and BeginInputFile incidents:
    **
    **   1) For the first begin run, we retrieve the TagInfo and set
    **      up IOVDbSvc so that is can use TagInfo to define its
    **      hierarchical tags. 
    **      The member m_currentRun is saved to be used for the IOV 
    **      in fillMetaData.
    **
    **   2) Then whenever a new file is opened (BeginInputFile), we
    **      use the TagInfo object from the detector store to "re-fill"
    **      and overwrite the newly merged TagInfo meta data in the output
    **      meta data store. We receive BeginInputFile AFTER the 
    **      IOVDbMetaDataTool has done this automatic merging, so that we 
    **      can safely overwrite with the desired TagInfo information for
    **      this job/run.
    **      As well, if there is a second BeginRun during the processing, 
    **      here we update m_currentRun and overwrite again the TagInfo in the  
    **      meta store with the new run which will just extend the IOV of the
    **      TagInfo meta data.
    **
    **/

    // Get the messaging service, print where you are
    ATH_MSG_DEBUG( "handle: entering handle(), incidence type " << inc.type()
          << " from " << inc.source());

    const EventIDBase eventID =  inc.context().eventID();

    if (inc.type() == IncidentType::BeginRun) {
       // For begin run, reset the current run number

       // get conditionsRun from the Context - can be used if no EventID in the SG (for HLT)
       m_conditionsRun = Atlas::getExtendedEventContext(inc.context()).conditionsRun();

       // Set current run number to be used for fillMetaData
       m_currentRun = eventID.run_number();

       ATH_MSG_DEBUG( "handle: BeginRun incident - Event ID: ["
                      << eventID.run_number()   << ","
                      << eventID.lumi_block() << ":"
                      << eventID.time_stamp() << "] ");
       if( m_conditionsRun != EventIDBase::UNDEFNUM ) {
          ATH_MSG_DEBUG( "handle: conditionsRun = " << m_conditionsRun);
       }
       if( m_currentRun != EventIDBase::UNDEFNUM ) {
          ATH_MSG_DEBUG( "handle: currentRun = " << m_currentRun);
       }
    }

    // At first BeginRun we retrieve TagInfo and trigger IOVDbSvc to use it
    if (inc.type() == IncidentType::BeginRun && m_isFirstBeginRun) {

        // No longer first BeginRun 
        m_isFirstBeginRun = false; 

        // For the moment, we must set IOVDbSvc into the BeginRun
        // state to be able to access TagInfo from the file meta data

        // create IOV time from current event coming in with BeginRun incident
        EventIDBase::number_type  run = eventID.run_number();
        EventIDBase::number_type  lb  = eventID.lumi_block();
        IOVTime curTime;
        curTime.setRunEvent(run, lb);

        // save both seconds and ns offset for timestamp
        uint64_t nsTime = eventID.time_stamp()*1000000000LL;
        nsTime         += eventID.time_stamp_ns_offset();
        curTime.setTimestamp(nsTime);

        if (StatusCode::SUCCESS != m_iovDbSvc->signalBeginRun(curTime, inc.context()))
        {
            ATH_MSG_ERROR( "Unable to signal begin run to IOVDbSvc");
            throw GaudiException( "Unable to signal begin run to IOVDbSvc", "TagInfoMgr::handle", StatusCode::FAILURE );
        }
        ATH_MSG_DEBUG( "Signaled begin run to IOVDbSvc " << curTime);
        
        // For BeginRun, update TagInfo
        if( updateTagInfo().isFailure() ) {
           throw GaudiException( "updateTagInfo ERROR:", "TagInfoMgr::handle", StatusCode::FAILURE );
        }

            // May have pre-existing TagInfo with incorrect IOV
            // range. Try dropping and retrieving again.
            //    bool sc = m_iovDbSvc->dropObject("/TagInfo");  //MN: No idea what that was supposed to do

	// Process TagInfo by IOVDbSvc
        if (m_iovDbSvc->processTagInfo().isFailure() ) {
           throw GaudiException( "iovDbSvc::processTagInfo ERROR", "TagInfoMgr::handle", StatusCode::FAILURE );
        }
        ATH_MSG_DEBUG( "handle: TagInfo successfully processed by IOVDbSvc to register callback");
    }
    else if ((inc.type() == IncidentType::BeginInputFile || inc.type() == IncidentType::BeginRun)
             && !m_isFirstBeginRun)
    {
       // For a new file incident or a subsequent begin run incident, we must "refill" the meta
       // data store with the current value of the TagInfo folder in the detector store.
       if( updateTagInfo().isFailure() ) {
          throw GaudiException( "updateTagInfo ERROR:", "TagInfoMgr::handle", StatusCode::FAILURE );
       }
    }
}

//______________________________________________________________________________
StatusCode
TagInfoMgr::checkTagInfo(IOVSVC_CALLBACK_ARGS)
{
    // The conditions object in file meta data has change so we need
    // to reset the TagInfo object in the detector store.
    //
    ATH_MSG_DEBUG( "checkTagInfo IOV callback");
    StatusCode sc = updateTagInfo();
    return sc;
}

//______________________________________________________________________________
void
TagInfoMgr::notifyListeners() const
{
   ATH_MSG_DEBUG( "notifyListeners  (" << m_listeners.size() <<" registered)");

   m_mutex.lock_shared();
   const auto listeners = m_listeners;
   m_mutex.unlock_shared();

   for( auto listener : listeners ) {
      listener->tagInfoUpdated();
   }
}

//______________________________________________________________________________
StatusCode 
TagInfoMgr::updateTagInfo() {

    // Fill the TagInfo object
    //   The tag info may come from either the input file meta data or
    //   the currently available EventInfo object

    ATH_MSG_DEBUG( "updateTagInfo: getting /TagInfo");
    std::unique_lock guard(m_mutex);

    // Check whether TagInfo is coming from file meta data or the
    // input event
    const CondAttrListCollection* attrListColl = 0;
    if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
        if (m_detStore->retrieve( attrListColl, "/TagInfo" ).isFailure() ) {
            ATH_MSG_ERROR( "updateTagInfo: No TagInfo meta data in DetectorStore");
            return StatusCode::FAILURE ;
        }
        else {
            ATH_MSG_DEBUG( "updateTagInfo: Retrieved TagInfo meta data from detStore. size " << attrListColl->size());
        }
    }
    else {
        ATH_MSG_DEBUG( "updateTagInfo: det store does NOT contain AttrListColl for TagInfo");
    }

    // Fill TagInfo from (file meta data) conditions if they exist. If
    // they are empty, we restore from the saved TagInfo
    // information. Otherwise we fill from from event info (OLD and
    // most likely not used anymore. RDS 08/2012).
    if (attrListColl && attrListColl->size() == 0) {
        ATH_MSG_DEBUG( "updateTagInfo: /TagInfo empty - do nothing");
        ATH_MSG_DEBUG(m_tagInfo.str());
        return StatusCode::SUCCESS;
    }

    ATH_CHECK( fillTagInfo(attrListColl) );
    if (attrListColl) ATH_MSG_DEBUG( "updateTagInfo: Filled TagInfo from file meta data ");
    else ATH_MSG_DEBUG( "updateTagInfo: Filled TagInfo from input event ");
    
    // Copy TagInfo to meta data store for writing to file meta data
    ATH_CHECK( fillMetaData(attrListColl) );
    ATH_MSG_DEBUG( "updateTagInfo: Wrote TagInfo to MetaDataStore ");

    return StatusCode::SUCCESS;
}


//______________________________________________________________________________
/// Method to allow clients add in tags as: tag name, tag value
StatusCode
TagInfoMgr::addTag(const std::string& tagName, const std::string& tagValue)
{
    // Add name/value to input vector
    ATH_MSG_DEBUG("addTag - adding name/value pairs: " << tagName << " " << tagValue);
    std::unique_lock  guard( m_mutex );
    m_extraTagValuePairsViaInterface[tagName]=tagValue;
    return StatusCode::SUCCESS;
}

//______________________________________________________________________________
/// Method to allow clients to remove a tag which may have come in on
/// the input
StatusCode
TagInfoMgr::removeTagFromInput(const std::string& tagName)
{
    // Add name/value to input vector
    ATH_MSG_DEBUG( "removeTagFromInput - adding tag name to be removed: " << tagName);
    std::unique_lock  guard( m_mutex );
    m_tagsToBeRemoved.insert(tagName);
    return StatusCode::SUCCESS;
}

//______________________________________________________________________________
std::string TagInfoMgr::findTag(const std::string & name) const
{
   std::shared_lock guard(m_mutex);
   const auto iter = m_extraTagValuePairsViaInterface.find(name);
   if( iter != m_extraTagValuePairsViaInterface.end() ) {
      return iter->second;
   }
   return m_tagInfo.findTag(name);
}

//______________________________________________________________________________
std::string TagInfoMgr::findInputTag(const std::string & name) const
{
   std::shared_lock guard(m_mutex);
   return m_tagInfo.findInputTag(name);
}

//______________________________________________________________________________
ITagInfoMgr::NameTagPairVec
TagInfoMgr::getInputTags() const
{
   std::shared_lock guard(m_mutex);
   return m_tagInfo.getInputTags();
}

//______________________________________________________________________________
std::string 
TagInfoMgr::dumpTagInfoToStr() const
{
   std::shared_lock guard(m_mutex);
   return m_tagInfo.str();
}

//______________________________________________________________________________
void
TagInfoMgr::printTags(MsgStream& log) const
{
   std::shared_lock guard(m_mutex);
   m_tagInfo.printTags(log);
}


StatusCode
TagInfoMgr::iovCallback(IOVSVC_CALLBACK_ARGS)
{
   ATH_MSG_DEBUG( "IOV callback");
   notifyListeners();
   return StatusCode::SUCCESS;
}

//______________________________________________________________________________
void TagInfoMgr::addListener(Listener* listener)
{
   std::unique_lock guard(m_mutex);
   if( not m_detStore->contains<std::string>(IOVCbkObjKey)  ) {
      ATH_MSG_DEBUG("adding IOV callback object to DetStore");
      auto alignObj = std::make_unique<std::string>("Geometry aligment callbck trigger object");
      if( m_detStore->record(std::move(alignObj), IOVCbkObjKey)  != StatusCode::SUCCESS) {
         ATH_MSG_WARNING("Cannot register IOV callback object with key "  << IOVCbkObjKey);
      }
      else {
         SG::DataProxy *dp =  m_detStore->proxy(ClassID_traits<std::string>::ID(), IOVCbkObjKey);
         if( dp ) {
            dp->setProvider(this, StoreID::DETECTOR_STORE);

            const DataHandle<std::string> alignObjH;
            if(m_detStore->regFcn(&TagInfoMgr::iovCallback, this,
                                  alignObjH, IOVCbkObjKey) != StatusCode::SUCCESS) {
               ATH_MSG_WARNING("Cannot register IOV callback for key - TagInfo update notification will not work"  << IOVCbkObjKey);
            }
            else {
               ATH_MSG_DEBUG("Registered IOV callback for TagInfo changes");
            }
         } else {
            ATH_MSG_WARNING("Failed to retrieve "  << IOVCbkObjKey);
         }
      }
   }

   if( m_listeners.find(listener) == m_listeners.end() )
      m_listeners.insert( listener );
}

//______________________________________________________________________________
void TagInfoMgr::removeListener(Listener* listener)
{
   std::unique_lock guard(m_mutex);
   m_listeners.erase( listener );
}




StatusCode 
TagInfoMgr::preLoadAddresses( StoreID::type storeID, tadList& /*tlist*/ )
{
   if (storeID == StoreID::DETECTOR_STORE) {
      ATH_MSG_DEBUG( "preLoadAddresses - not expected to be called");
   }
   return StatusCode::SUCCESS;
}



StatusCode       
TagInfoMgr::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* /*tad*/,
                          const EventContext& /*ctx*/)
{
   ATH_MSG_DEBUG( "updateAddress");
   return StatusCode::SUCCESS;
}


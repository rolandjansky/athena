/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TagInfoMgr.cxx
 *
 * @brief implementation
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: TagInfoMgr.cxx,v 1.29 2009-04-29 07:47:40 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "TagInfoMgr.h"

// Event includes
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "EventInfo/TriggerInfo.h"

// IOVDbSvc
#include "AthenaKernel/IIOVDbSvc.h"

// TES include
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "SGTools/TransientAddress.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"
#include "CxxUtils/make_unique.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GaudiException.h"

// External definitions
long TagInfoMgr_StorageType = 0x50;

// Constructor with parameters:
TagInfoMgr::TagInfoMgr(const std::string &name, 
                       ISvcLocator *pSvcLocator) :
        ConversionSvc(name, pSvcLocator, TagInfoMgr_StorageType),    
        m_overrideEventInfoTags(true),
        m_storeGate("StoreGateSvc", name),
        m_detStore("DetectorStore", name),
        m_iovDbSvc("IOVDbSvc", name),
        m_metaDataTool("IOVDbMetaDataTool"),
        m_isFirstBeginRun(true),
        m_newFileIncidentSeen(false),
        m_lastIOVRange(IOVRange(IOVTime(),IOVTime())),
        m_log(msgSvc(), name)
{
    declareProperty("OverrideEventInfoTags", m_overrideEventInfoTags);
    declareProperty("ExtraTagValuePairs",    m_extraTagValuePairs);
    declareProperty("TagInfoKey",            m_tagInfoKey = "ProcessingTags");
}

TagInfoMgr::~TagInfoMgr() 
{}


/// Identify interfaces to which this service is responsive
StatusCode
TagInfoMgr::queryInterface( const InterfaceID& riid, void** ppvInterface ) 
{
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "in queryInterface()" << endmsg;

    if ( ITagInfoMgr::interfaceID().versionMatch(riid) ) {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "matched ITagInfoMgr" << endmsg;
        *ppvInterface = (ITagInfoMgr*)this;
    }
    else {
        // Interface is not directly available: try out a base class
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "Default to ConversionSvc interface" << endmsg;
        // Interface is not directly available: try out a base class
        return(::ConversionSvc::queryInterface(riid, ppvInterface));
    }

    return StatusCode::SUCCESS;
}

/// Method to allow clients add in tags as: tag name, tag value
StatusCode
TagInfoMgr::addTag(const std::string& tagName, 
                   const std::string& tagValue) 
{
    // Add name/value to input vector
    if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "addTag - adding name/value pairs: " 
              << tagName << " " << tagValue
              << endmsg;
    }
    m_extraTagValuePairsViaInterface.push_back(tagName);
    m_extraTagValuePairsViaInterface.push_back(tagValue);
    return StatusCode::SUCCESS;
}

/// Method to allow clients to remove a tag which may have come in on
/// the input
StatusCode  
TagInfoMgr::removeTagFromInput(const std::string& tagName) 
{
    // Add name/value to input vector
    if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "removeTagFromInput - adding tag name to be removed: " 
              << tagName << endmsg;
    }
    m_tagsToBeRemoved.insert(tagName);
    return StatusCode::SUCCESS;
}

    

/// Method to allow clients to access the TagInfo object key.
std::string&
TagInfoMgr::tagInfoKey()
{
    return (m_tagInfoKeyValue);
}

// Initialize method:
StatusCode TagInfoMgr::initialize()
{

    // Must first initialize base class
    if(!(ConversionSvc::initialize()).isSuccess()) return StatusCode::FAILURE;

    m_log.setLevel( outputLevel() );

    // Get the key value
    m_tagInfoKeyValue = m_tagInfoKey.value();

    // Get the messaging service, print where you are
    if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "initialize()" << endmsg;
        m_log << MSG::DEBUG << "OverrideEventInfoTags   " << m_overrideEventInfoTags 
              << endmsg; 
        m_log << MSG::DEBUG << "TagInfoKey              " << m_tagInfoKey 
              << endmsg;
    }
    
    // Check that ExtraTagValuePairs is either 0 or an even number
    std::vector<std::string> valueTagPairs = m_extraTagValuePairs.value();
    // Add in pairs from interface
    std::copy(m_extraTagValuePairsViaInterface.begin(), 
              m_extraTagValuePairsViaInterface.end(),
              std::back_inserter(valueTagPairs));
    if ((valueTagPairs.size()%2) != 0) {
        m_log << MSG::ERROR << "initialize: Non-even number of extra value tag pairs !" << endmsg;
        return StatusCode::FAILURE;
    } 
    else {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "ExtraTagValuePairs      " << endmsg;
        std::vector<std::string>::const_iterator it;
        std::vector<std::string>::const_iterator it1;
        std::vector<std::string>::const_iterator it2;
        std::vector<std::string>::const_iterator itend = valueTagPairs.end();
        for (it = valueTagPairs.begin(); it != itend; ++it) {
            it1 = it;
            ++it;
            it2 = it;
            if (m_log.level() <= MSG::DEBUG) {
                m_log << MSG::DEBUG << " Value/tag pair: " 
                      << (*it1) << " "
                      << (*it2) << endmsg;
            }
        }
    }
    

    // To copy TagInfo to EventInfo, we set listener to the
    // IncidentSvc for BeginEvent and BeginRun
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
    long int pri=1000;

    // Add BeginRun to trigger tag extraction and writing to output
    // file meta data
    incSvc->addListener( this, "BeginRun", pri);

    // Add BeginEvent to trigger refilling meta data store after a new
    // input file
    incSvc->addListener( this, "BeginEvent", pri);

    // Set to be listener for BeginInputFile
    incSvc->addListener(this, "BeginInputFile", 50); // pri has to be < 50 to be after IOVDbMetaDataTool.

    return StatusCode::SUCCESS;
}

// Start method:
StatusCode TagInfoMgr::start()
{
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "start()" << endmsg;

    // Register callback to CondAttrListCollection object containing
    // the TagInfo information - only if it exists, i.e. in the file
    // meta data
    if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
        const DataHandle<CondAttrListCollection> tagInfoH;
        if (m_detStore->regFcn(&ITagInfoMgr::checkTagInfo, 
                               dynamic_cast<ITagInfoMgr*>(this), 
                               tagInfoH, "/TagInfo").isSuccess()) {
            if (m_log.level() <= MSG::DEBUG)
                m_log << MSG::DEBUG << "Registered checkTagInfo callback for  /TagInfo " << endmsg;
        }
        else {
            if (m_log.level() <= MSG::DEBUG)
                m_log << MSG::DEBUG << "Cannot register checkTagInfo function for /TagInfo " << endmsg;
        }
    }
    
    return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode TagInfoMgr::finalize() 
{
  // Get the messaging service, print where you are
  if (m_log.level() <= MSG::DEBUG) {
    m_log << MSG::DEBUG << "finalize()" << endmsg;
  }

  return ConversionSvc::finalize();
}

StatusCode 
TagInfoMgr::fillTagInfo(const CondAttrListCollection* tagInfoCond, TagInfo* tagInfo) const
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
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillTagInfo: " << endmsg;

    // Add tags to TagInfo

    if (tagInfoCond) {
        // Coming from COOL
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillTagInfo: - tags coming from COOL file meta data" << endmsg;

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
                    if (tagInfo->addTag(EventType::NameTagPair(name, value)).isFailure()) {
                        m_log << MSG::WARNING << "fillTagInfo: Unable to add value/tag to TagInfo " 
                              << name << " " << value << endmsg;
                    }
                    if (m_log.level() <= MSG::DEBUG) 
                        m_log << MSG::DEBUG << "fillTagInfo: Added name/tag to TagInfo " 
                              << name << " " << value << endmsg;
                }
                else if (m_log.level() <= MSG::DEBUG) 
                    m_log << MSG::DEBUG << "fillTagInfo: Did NOT add tag - on remove list: name/tag: " 
                          << name << " " << value << endmsg;

                // Duplicate as input tags:
                // ------------------------
                // Assumption is that tags from file meta data should
                // be the "first ones in" and there should not be any
                // clash with existing tags. Return WARNING if this
                // happens
                if (tagInfo->addInputTag(EventType::NameTagPair(name, value)).isFailure()) {
                    m_log << MSG::WARNING << "fillTagInfo: Unable to add value/tag to TagInfo as input tag " 
                          << name << " " << value << endmsg;
                }
                if (m_log.level() <= MSG::DEBUG) 
                    m_log << MSG::DEBUG << "fillTagInfo: Added value/tag to TagInfo as input tag " 
                          << name << " " << value << endmsg;
            }
        }
        else {
            // error! 
            m_log << MSG::DEBUG << "fillTagInfo:  Could not get attribute list" << endmsg;      
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
            
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillTagInfo: Add in tags from EventInfo" << endmsg;
        const DataHandle<EventInfo> evtH;
        const DataHandle<EventInfo> evtHEnd;
        if (m_storeGate->retrieve( evtH, evtHEnd ).isFailure() ) {
            // For simulation, we may be in the initialzation phase
            // and we cannot get EventInfo. We simply skip this step,
            // assuming that most times the information is coming in
            // via conditions/file meta data.
            m_log << MSG::DEBUG << "fillTagInfo:  Could not get event info - skipping the filling of TagInfo from input EventInfo" << endmsg;      
        }
        else {
            if (evtH == evtHEnd) {
                m_log << MSG::ERROR << "fillTagInfo: No event info objects" << endmsg;
                return (StatusCode::FAILURE);
            }
            if (m_log.level() <= MSG::DEBUG) {
                m_log << MSG::DEBUG << "fillTagInfo: Event ID: ["
                      << evtH->event_ID()->run_number()   << ","
                      << evtH->event_ID()->event_number() << ":"
                      << evtH->event_ID()->time_stamp() << "] "
                      << endmsg;
            }
            EventType::NameTagPairVec pairs1;
            evtH->event_type()->get_detdescr_tags(pairs1);

            if (m_log.level() <= MSG::DEBUG) {
                if(pairs1.size()) {
                    m_log << MSG::DEBUG << "fillTagInfo: Pairs from EventType:" << endmsg;
                }
                else {
                    m_log << MSG::DEBUG << "fillTagInfo: EventInfo/EventType has no tags" << endmsg;
                }
            }
            for (unsigned int i = 0; i < pairs1.size(); ++i) {
                if (m_log.level() <= MSG::DEBUG) {
                    m_log << MSG::DEBUG << "fillTagInfo: " << pairs1[i].first << " : "
                          << pairs1[i].second
                          << endmsg;
                }
                // Add current tags:
                // -----------------
                // Assumption is that EventInfo tags should be the
                // "first ones in" and there should not be any clash
                // with existing tags. Return ERROR if this happens

                // First make sure that this tag name is not on the
                // list of those to be removed
                std::string& name = pairs1[i].first;
                if (m_tagsToBeRemoved.find(name) == m_tagsToBeRemoved.end()) {
                    if (tagInfo->addTag(pairs1[i]).isFailure()) {
                        m_log << MSG::ERROR << "fillTagInfo: Unable to add tag to TagInfo: name/tag " 
                              << pairs1[i].first << " : "
                              << pairs1[i].second
                              << endmsg;
                        return (StatusCode::FAILURE);
                    } 
                    else if (m_log.level() <= MSG::DEBUG) 
                        m_log << MSG::DEBUG << "fillTagInfo: Added name/tag to TagInfo " 
                              << pairs1[i].first << " : "
                              << pairs1[i].second
                              << endmsg;
                }
                else if (m_log.level() <= MSG::DEBUG) 
                    m_log << MSG::DEBUG << "fillTagInfo: Did NOT add tag - on remove list: name/tag: " 
                          << pairs1[i].first << " : "
                          << pairs1[i].second
                          << endmsg;

                // Duplicate as input tags:
                // ------------------------
                // Assumption is that EventInfo tags should be the
                // "first ones in" and there should not be any clash
                // with existing tags. Return ERROR if this happens
                if (tagInfo->addInputTag(pairs1[i]).isFailure()) {
                    m_log << MSG::ERROR << "fillTagInfo: Unable to add input tag to TagInfo: name/tag " 
                        << pairs1[i].first << " : "
                        << pairs1[i].second
                        << endmsg;
                    return (StatusCode::FAILURE);
                } 
            }
            if (m_log.level() <= MSG::DEBUG) 
                m_log << MSG::DEBUG << "fillTagInfo: Added EventInfo tags to TagInfo current/input" << endmsg;

        }
    }
    
    // Dump out contents of TagInfo
    MsgStream log1(msgSvc(), "TagInfo");
    tagInfo->printTags(log1);


    // Add in any extra tag value pairs if specified
    std::vector<std::string> valueTagPairs = m_extraTagValuePairs.value();
    // Add in pairs from interface
    std::copy(m_extraTagValuePairsViaInterface.begin(), 
              m_extraTagValuePairsViaInterface.end(),
              std::back_inserter(valueTagPairs));
    if (valueTagPairs.size()) {
        
        std::vector<std::string>::const_iterator it;
        std::vector<std::string>::const_iterator it1;
        std::vector<std::string>::const_iterator it2;
        std::vector<std::string>::const_iterator itend = valueTagPairs.end();
        for (it = valueTagPairs.begin(); it != itend; ++it) {
            it1 = it;
            ++it;
            it2 = it;
            if (m_log.level() <= MSG::DEBUG) {
                m_log << MSG::DEBUG << "fillTagInfo: Adding extra value/tag pair: " 
                      << (*it1) << " "
                      << (*it2) << endmsg;
            }
            if (tagInfo->addTag(EventType::NameTagPair((*it1), (*it2)),
                                m_overrideEventInfoTags).isFailure()) {
                m_log << MSG::WARNING << "fillTagInfo: Extra value/tag not added to TagInfo " << endmsg;
            }
        }

        // Dump out contents of TagInfo
        MsgStream log1(msgSvc(), "TagInfo");
        tagInfo->printTags(log1);
    
    }

    return StatusCode::SUCCESS;

}

StatusCode
TagInfoMgr::fillMetaData   (const TagInfo* tagInfo, const CondAttrListCollection* tagInfoCond) 
{
    // fillMetaData is called at the beginning of the job and for each
    // new file being read in.
    //

    // Fill IOV object in metadata store with information from tag
    // info. 
    // For the IOV, we have two choices:
    //
    //   1) If there are input tags from file meta data, then we use
    //      the IOV from the input conditions. For the first input
    //      file, we use the incoming IOV if the current run falls
    //      within the IOV range. If not we set the IOV to runNumber
    //      to runNumber + 1. For subsequent input files, if the
    //      runNumber falls within the previous IOV range, then we set
    //      the new IOV to [runNumber, lastStop), otherwise the IOV is
    //      set to [runNumber, runNumber+1). This latter is used for
    //      the zerobias overlaying.
    //
    //   2) If we are taking them from the incoming events, then we
    //      simply assume that the tags are valid for one run.  (This
    //      is the old way of doing things, no longer valid since
    //      storing TagInfo in the MetaData
    //
    //  Notes: 
    //
    //    The add of tags to the file meta data will merge different
    //    IOVs together if the payloads are identical, so doing this
    //    run by run is ok for 2)
    //

    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "entering fillMetaData" << endmsg;

    // Get run number for IOV
    const EventInfo* evt   = 0;
    unsigned int runNumber = 0;
    if (StatusCode::SUCCESS != m_storeGate->retrieve(evt)) {
        // For simulation, we may be in the initialization phase and
        // must get the run number from the event selector
        if (StatusCode::SUCCESS != getRunNumber (runNumber)) {
            m_log << MSG::ERROR << "fillMetaData:  Could not get event info neither via retrieve nor from the EventSelectror" << endmsg;      
            return (StatusCode::FAILURE);
        }
    }
    else {
        runNumber = evt->event_ID()->run_number();
    }

    // Copy tags to AttributeList
    coral::AttributeList attrList;
    EventType::NameTagPairVec pairs;
    tagInfo->getTags(pairs);
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillMetaData: Adding value/tag pairs to file meta data: " 
                                           << endmsg;
    for (unsigned int i = 0; i < pairs.size(); ++i) {
        attrList.extend(pairs[i].first, "string");
        attrList[pairs[i].first].setValue(pairs[i].second);
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << pairs[i].first << " " << pairs[i].second << endmsg;
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
            m_lastIOVRange = IOVRange(start, stop);
            isFirstIOVCheck = true; // 
        }
        if (m_lastIOVRange.isInRange(testTime)) {
            // set start to runNumber after the 
            if (!isFirstIOVCheck) start = testTime; 
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillMetaData: run number is in previous IOVRange: " << runNumber << " " << m_lastIOVRange << endmsg;
        }
        else {
            // Out of range
            start = testTime;
            stop  = IOVTime(runNumber + 1, 0);
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillMetaData: run number is outside of previous IOVRange: " << runNumber << " " << m_lastIOVRange << ". Reset range to run number." << endmsg;
        }
        attrListColl->addNewStart(start);
        attrListColl->addNewStop (stop);
        m_lastIOVRange = IOVRange(start, stop);
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "fillMetaData: start, stop: " << start << " " << stop << endmsg;
    }
    else {
        // set to the IOV of this run to run+1
        attrListColl->addNewStart(IOVTime(runNumber, 0));
        attrListColl->addNewStop(IOVTime(runNumber + 1, 0));
    }
    
    /// Register folder in the IOV Db MetaData
    if (StatusCode::SUCCESS != m_metaDataTool->registerFolder("/TagInfo")) {
        m_log << MSG::ERROR << "fillMetaData: Unable to register folder for TagInfo with meta data tool " << endmsg;
        return StatusCode::FAILURE;
    }

    // Add payload
    if (StatusCode::SUCCESS != m_metaDataTool->addPayload("/TagInfo", attrListColl)) {
        m_log << MSG::ERROR << "fillMetaData: Unable to register folder for TagInfo with meta data tool " << endmsg;
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
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "getRunNumber: check if tag is set in jobOpts" << endmsg;
    // Get name of event selector from the application manager to
    // make sure we get the one for MC signal events
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("ApplicationMgr", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        m_log << MSG::ERROR << "getRunNumber: Cannot get ApplicationMgr " << endmsg; 
        return StatusCode::FAILURE;
    }
    StringProperty property("EvtSel", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        m_log << MSG::ERROR << "getRunNumber: unable to get EvtSel: found " << property.value() << endmsg;
        return StatusCode::FAILURE;
    }
    // Get EventSelector for ApplicationMgr
    std::string eventSelector = property.value();
    sc = serviceLocator()->service(eventSelector, propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        m_log << MSG::ERROR << "getRunNumber: Cannot get EventSelector " << eventSelector << endmsg; 
        return StatusCode::FAILURE;
    }
    BooleanProperty overrideRunNumber = BooleanProperty("OverrideRunNumber", false);
    sc = propertyServer->getProperty(&overrideRunNumber);
    if (!sc.isSuccess()) {
        // Not all EventSelectors have this property, but we should
        // not get here if the ES is not one for simulation => return failure
        m_log << MSG::ERROR << "getRunNumber: unable to get OverrideRunNumber property from EventSelector ";
        return StatusCode::FAILURE;
    }
    if (overrideRunNumber.value()) {
        IntegerProperty runNumberProp = IntegerProperty("RunNumber", 0);
        sc = propertyServer->getProperty(&runNumberProp);
        if (!sc.isSuccess()) {
            m_log << MSG::ERROR << "getRunNumber: unable to get RunNumber from EventSelector: found " 
                << runNumberProp.value()
                << endmsg;
            return StatusCode::FAILURE;
        }
        else {
            runNumber = runNumberProp.value();
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "getRunNumber: Run number:  "
                                                   << runNumber << " obtained from "
                                                   << eventSelector << endmsg;
        }
    }
    else {
        m_log << MSG::ERROR << "getRunNumber: OverrideRunNumber from EventSelector is false " 
              << endmsg;
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


void 
TagInfoMgr::handle(const Incident& inc) {

    /**

    ** This method is called at the BeginRun incident:
    **
    **   1) For the first begin run, we retrieve the TagInfo and set
    **      up IOVDbSvc so that is can use TagInfo to define its
    **      hierarchical tags.
    **                
    **   2) For (old) input files where the TagInfo is stored in each
    **      event, subsequent begin runs will fill the file meta data
    **      for output. This is done because the IOV is incremented by
    **      one each run. 
    **
    **   Note that if the input has TagInfo in the file meta data, the
    **   output will be written via the checkTagInfo callback - called
    **   each time the input TagInfo object changes.
    **                
    **/

    // Get the messaging service, print where you are
    if (m_log.level() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "handle: entering handle(), incidence type " << inc.type()
              << " from " << inc.source() << endmsg;
    }

    // Return quickly for BeginEvent if not needed
    if (!m_newFileIncidentSeen && inc.type() == "BeginEvent") return;

    // At first BeginRun we retrieve TagInfo and trigger IOVDbSvc to
    // use it
    if (inc.type() == "BeginRun" && m_isFirstBeginRun) {

        // No longer first BeginRun 
        m_isFirstBeginRun = false; 

        // reset flags 
        m_newFileIncidentSeen = false;

        // Print out EventInfo
	const EventInfo* evt{nullptr};
	if (StatusCode::SUCCESS != m_storeGate->retrieve(evt)) {
	  m_log << MSG::ERROR << "handle:  Unable to get EventInfo from BeginRun incident" << endmsg;
	  throw GaudiException( "Unable to get EventInfo from BeginRun incident handler", "TagInfoMgr::handle", StatusCode::FAILURE );
        }
        if (m_log.level() <= MSG::DEBUG) {
            m_log << MSG::DEBUG << "handle: BeginRun incident - Event info: " << endmsg;
            m_log << MSG::DEBUG << "handle: Event ID: ["
                  << evt->event_ID()->run_number()   << ","
                  << evt->event_ID()->event_number() << ":"
                  << evt->event_ID()->time_stamp() << "] "
                  << endmsg;
            if (evt->event_type()) {
                m_log << MSG::DEBUG << evt->event_type()->typeToString() << endmsg;
                m_log << MSG::DEBUG << "handle: Event type: user type "
                      << evt->event_type()->user_type()
                      << endmsg;
            }
        }
        
        // For the moment, we must set IOVDbSvc into the BeginRun
        // state to be able to access TagInfo from the file meta data

        // create IOV time from current event coming in with BeginRun incident
        unsigned int run = evt->event_ID()->run_number();
        unsigned int lb  = evt->event_ID()->lumi_block();
        IOVTime curTime;
        curTime.setRunEvent(run, lb);

        // save both seconds and ns offset for timestamp
        uint64_t nsTime = evt->event_ID()->time_stamp()*1000000000LL;
        nsTime         += evt->event_ID()->time_stamp_ns_offset();
        curTime.setTimestamp(nsTime);

        if (StatusCode::SUCCESS != m_iovDbSvc->signalBeginRun(curTime,
                                                              inc.context()))
        {
            m_log << MSG::ERROR << "Unable to signal begin run to IOVDbSvc" << endmsg;
            throw GaudiException( "Unable to signal begin run to IOVDbSvc", "TagInfoMgr::handle", StatusCode::FAILURE );
        }
        else if (m_log.level() <= MSG::DEBUG) {
            m_log << MSG::DEBUG << "Signaled begin run to IOVDbSvc " << curTime << endmsg;
        }
        

        m_log << MSG::DEBUG << "Retrieve tag info " << endmsg;

        // For BeginRun, retrieve and fill TagInfo 
        const TagInfo* tagInfo = 0;
        if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
            m_log << MSG::DEBUG << "handle:   Could not retrieve TagInfo object from the detector store on first try." << endmsg;      
            // May have pre-existing TagInfo with incorrect IOV
            // range. Try dropping and retrieving again.
            bool sc = m_iovDbSvc->dropObject("/TagInfo");
            m_log << MSG::DEBUG << "handle:   Try dropping /TagInfo - " << sc << endmsg;      
            if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
                m_log << MSG::ERROR << "handle:   Could not retrieve TagInfo object from the detector store" << endmsg;      

                return;
            }
        }
        m_log << MSG::DEBUG << "Retrieved tag info " << endmsg;

        // NOTE: registerTagInfoCallback for IOVDbSvc actually causes
        // the IOVDbSvc to go and fetch the TagInfo, rather than just
        // setting a callback. This is needed for now because the
        // IOVDbSvc must be initialized first. RDS 2006/09

        // Request IOVDbSvc to register call back for TagInfo. This needs
        // to be done here because IOVDbSvc::initialize cannot - it is
        // called too early from the ProxyProviderSvc.
        // Get the IOVDbSvc
        if (m_iovDbSvc->registerTagInfoCallback().isFailure() ) {
            m_log << MSG::ERROR << "handle: Unable register IOVDbSvc callback" << endmsg;
        }
        else {
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle: Requested IOVDbSvc to register callback" << endmsg;
        }
    }
    else if (inc.type() == "BeginRun") {
        // For subsequent BeginRuns, check whether the /TagInfo folder
        // exists, and if so return. We only treat the case where
        // TagInfo is coming in with each event. In this case we fill
        // file meta data and increment IOV to cover each run

        // reset flags 
        m_newFileIncidentSeen = false;
        // Return if /TagInfo exists
        const CondAttrListCollection* attrListColl = 0;
        if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle: TagInfo input from meta data - no need to fill output file meta data" << endmsg;
            return;
        } 

        const TagInfo* tagInfo = 0;
        if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
            m_log << MSG::ERROR << "handle: Could not retrieve TagInfo object from the detector store" << endmsg;      
            throw GaudiException( "Could not retrieve TagInfo object from the detector store", "TagInfoMgr::handle", StatusCode::FAILURE );
        }

        // Copy TagInfo to meta data store for writing to file meta data
        if (StatusCode::SUCCESS != fillMetaData(tagInfo, attrListColl)) {
            m_log << MSG::ERROR << "handle: Unable to write TagInfo to MetaDataStore !" << endmsg;
            throw GaudiException( "Unable to write TagInfo to MetaDataStore !", "TagInfoMgr::handle", StatusCode::FAILURE );
        } 
        else {
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle: Wrote TagInfo to MetaDataStore " << endmsg;
        }
    }
    else if (inc.type() == "BeginInputFile"  && !m_isFirstBeginRun) {
        // We must keep track of BeginInputFile incidents because they
        // will override the TagInfo in the file meta data. This flag
        // will be used to restore the TagInfo folder in the meta data
        // store at the next BeginEvent if and only if BeginRun has
        // NOT already been called. (BeginRun will also reset the
        // TagInfo in the meta data store.)
        m_newFileIncidentSeen = true;
    }
    else if (m_newFileIncidentSeen && inc.type() == "BeginEvent") {
        // If a new file incident has been seen and there is NOT a
        // BeginRun before the next BeginEvent, then we "refill" the
        // meta data store with the current value of the TagInfo
        // folder in the detector store.

        // reset flags 
        m_newFileIncidentSeen = false;
        // Return if /TagInfo does NOT exists
        const CondAttrListCollection* attrListColl = 0;
        if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
            if (m_detStore->retrieve( attrListColl, "/TagInfo" ).isFailure() ) {
                m_log << MSG::ERROR << "handle - BeginInputFile: No TagInfo meta data in DetectorStore" << endmsg;
                throw GaudiException( "BeginInputFile - No TagInfo meta data in DetectorStore", "TagInfoMgr::handle", StatusCode::FAILURE );
            }
            else {
                if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle - BeginInputFile: Retrieved TagInfo meta data from detStore" << endmsg;
            } 
        }
        else {
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle - BeginInputFile: det store does NOT contain AttrListColl for TagInfo" << endmsg; 
            return;
        }

        const TagInfo* tagInfo = 0;
        if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
            m_log << MSG::ERROR << "handle - BeginInputFile: Could not retrieve TagInfo object from the detector store" << endmsg;      
            throw GaudiException( "BeginInputFile - Could not retrieve TagInfo object from the detector store", "TagInfoMgr::handle", StatusCode::FAILURE );
        }

        // Copy TagInfo to meta data store for writing to file meta data
        if (StatusCode::SUCCESS != fillMetaData(tagInfo, attrListColl)) {
            m_log << MSG::ERROR << "handle - BeginInputFile: Unable to write TagInfo to MetaDataStore !" << endmsg;
            throw GaudiException( "BeginInputFile - Unable to write TagInfo to MetaDataStore !", "TagInfoMgr::handle", StatusCode::FAILURE );
        } 
        else {
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "handle - BeginInputFile: Wrote TagInfo to MetaDataStore " << endmsg;
        }
    }
}


StatusCode
TagInfoMgr::checkTagInfo(IOVSVC_CALLBACK_ARGS)
{
    // The conditions object in file meta data has change so we need
    // to reset the TagInfo object in the detector store.
    //
    // To do so, we retrieve the TagInfo object proxy, reset it to
    // delete the current TagInfo object, and then retrieve the
    // TagInfo object to assure that it is recreated with the
    // information coming from both the file meta data and the
    // job-specific overrides.
    //
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "checkTagInfo: entering checkTagInfo" << endmsg;

    // Retrieve and save the contents of the TagInfo object in case
    // file meta data is empty, then the saved contents will be reused
    // and a new IOV assigned.
    const TagInfo* tagInfo = 0;
    if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
        m_log << MSG::ERROR << "checkTagInfo: Could not retrieve TagInfo object from the detector store to save the contents " << endmsg;      
        return StatusCode::FAILURE;
    }
    else {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "checkTagInfo: retrieved TagInfo to save its contents" << endmsg;
    }
    m_lastTagInfo = TagInfo(*tagInfo);
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "checkTagInfo: saved TagInfo contents" << endmsg;

    // Now remove the object in the detector store
    SG::DataProxy* tagInfoProxy = 
        m_detStore->proxy( ClassID_traits<TagInfo>::ID(), m_tagInfoKeyValue );

    if (0 == tagInfoProxy) {
        m_log << MSG::ERROR << "checkTagInfo: Unable to retrieve TagInfo object with clid/key: " 
              << ClassID_traits<TagInfo>::ID() << " " << m_tagInfoKeyValue << endmsg;
        return StatusCode::FAILURE;
    }
    // reset and retrieve
    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "checkTagInfo: reset TagInfo" << endmsg;
    tagInfoProxy->reset();
    tagInfo = 0;
    if (m_detStore->retrieve( tagInfo, m_tagInfoKeyValue ).isFailure() ) {
        m_log << MSG::ERROR << "checkTagInfo: Could not retrieve TagInfo object from the detector store" << endmsg;      
        return StatusCode::FAILURE;
    }
    else {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "checkTagInfo: retrieved TagInfo" << endmsg;
    }
    return StatusCode::SUCCESS;
}

StatusCode 
TagInfoMgr::preLoadAddresses( StoreID::type storeID,
                              tadList& tlist )
{
    if (storeID == StoreID::DETECTOR_STORE) {

        std::unique_ptr<SG::TransientAddress> tad = 
          CxxUtils::make_unique<SG::TransientAddress>( ClassID_traits<TagInfo>::ID(), m_tagInfoKeyValue );
        IAddressProvider* addp = this;
        tad->setProvider(addp, storeID);
        // Get IOpaqueAddress and add to tad
        IOpaqueAddress* ioa = 0;
        std::string refAddr(m_tagInfoKeyValue);
        StatusCode sc = createAddress(TagInfoMgr_StorageType, tad->clID(), refAddr, ioa);
        if ( sc.isFailure() ) {
            m_log << MSG::ERROR << "preLoadAddresses: Could not create IOpaqueAddress" << endmsg;      
            delete ioa;
            return StatusCode::FAILURE;
        }
        tad->setAddress(ioa);
        // set flag to avoid resetting the ioa when a proxy is reset
        tad->clearAddress(false);
        tlist.push_back( tad.release() );

        if (m_log.level() <= MSG::DEBUG) {
            m_log << MSG::DEBUG << "preLoadAddresses - add transient address for TagInfo to detector store" << endmsg;
            m_log << MSG::DEBUG << "preLoadAddresses - Found CLID: " << ioa->clID()  
                  << " key from ioa " << *(ioa->par()) << endmsg;
        }
    }
    return StatusCode::SUCCESS;
}


StatusCode       
TagInfoMgr::updateAddress(StoreID::type /*storeID*/, SG::TransientAddress* tad,
                          const EventContext& /*ctx*/)
{
    //
    // Here we do nothing, simply return success for TagInfo
    //
    if (m_log.level() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "updateAddress - fill TagInfo" << endmsg;
    CLID clid        = tad->clID();
    std::string key  = tad->name();
  
    if (m_log.level() <= MSG::VERBOSE) {
        m_log << MSG::VERBOSE << "updateAddress - Found CLID: " << clid << " -  key: " << key << endmsg;
        //m_log << MSG::VERBOSE << "updateAddress - provider: " << tad->provider() << " -  address: " << tad->address() << endmsg;
    }
    
    if (clid != ClassID_traits<TagInfo>::ID() || key != m_tagInfoKeyValue) {
        if (m_log.level() <= MSG::VERBOSE) {
            m_log << MSG::VERBOSE << "updateAddress - Found CLID: " << clid << " -  key: " << key 
                  << " NOT TagInfo - do nothing "
                  << endmsg;
        }
        return StatusCode::FAILURE ;
    }
    if (m_log.level() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "updateAddress - doing NOTHING, just return success!!" << endmsg;

    return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode 
TagInfoMgr::createObj(IOpaqueAddress* addr, DataObject*& dataObj) {

    // Create new TagInfo object 
    //   The tag info may come from either the input file meta data or
    //   the currently available EventInfo object

    if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "createObj: get TagInfo" << endmsg; 

    // Do checks: 
    // IOpaqueAddress set?
    if (addr == 0) {
        m_log << MSG::ERROR << "createObj: IOpaqueAddress is not set" << endmsg;
        return(StatusCode::FAILURE);
    }

    // Correct clid/key?
    CLID clid              = addr->clID();
    const std::string& key = *(addr->par());
    if (clid != ClassID_traits<TagInfo>::ID() || key != m_tagInfoKeyValue) {
        m_log << MSG::ERROR << "createObj:  Found CLID: " << clid << " -  key: " << key 
            << " NOT TagInfo - expect: " << ClassID_traits<TagInfo>::ID() << " " 
            << m_tagInfoKeyValue
            << endmsg;
        return StatusCode::FAILURE ;
    }

    // Create TagInfo from either incoming conditions for from the
    // saved TagInfo object
    std::unique_ptr<TagInfo> tagInfo;

    // Check whether TagInfo is coming from file meta data or the
    // input event
    const CondAttrListCollection* attrListColl = 0;
    if (m_detStore->contains<CondAttrListCollection>("/TagInfo")) { 
        if (m_detStore->retrieve( attrListColl, "/TagInfo" ).isFailure() ) {
            m_log << MSG::ERROR << "createObj: No TagInfo meta data in DetectorStore" << endmsg;
            return StatusCode::FAILURE ;
        }
        else {
            if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "createObj: Retrieved TagInfo meta data from detStore. size " << attrListColl->size() << endmsg;
        } 
    }
    else {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "createObj: det store does NOT contain AttrListColl for TagInfo" << endmsg; 
    }

    // Fill TagInfo from (file meta data) conditions if they exist. If
    // they are empty, we restore from the saved TagInfo
    // information. Otherwise we fill from from event info (OLD and
    // most likely not used anymore. RDS 08/2012).
    if (attrListColl && attrListColl->size() == 0) {
        tagInfo = CxxUtils::make_unique<TagInfo>(m_lastTagInfo);
        if (m_log.level() <= MSG::DEBUG) {
            m_log << MSG::DEBUG << "createObj: recreate tagInfo from saved info" << endmsg; 
            // Dump out contents of TagInfo
            MsgStream log1(msgSvc(), "TagInfo");
            tagInfo->printTags(log1);
        }
    }
    else {
        tagInfo = CxxUtils::make_unique<TagInfo>();
        if (StatusCode::SUCCESS != fillTagInfo(attrListColl, tagInfo.get())) {
            m_log << MSG::DEBUG << "createObj: Unable to fill TagInfo !" << endmsg;
            return StatusCode::FAILURE;
        } 
        if (m_log.level() <= MSG::DEBUG) {
            if (attrListColl) m_log << MSG::DEBUG << "createObj: Filled TagInfo from file meta data " << endmsg;
            else m_log << MSG::DEBUG << "createObj: Filled TagInfo from input event " << endmsg;
        }
    }
    
    // Copy TagInfo to meta data store for writing to file meta data
    if (StatusCode::SUCCESS != fillMetaData(tagInfo.get(), attrListColl)) {
        m_log << MSG::ERROR << "createObj: Unable to write TagInfo to MetaDataStore !" << endmsg;
        return StatusCode::FAILURE;
    } 
    else {
        if (m_log.level() <= MSG::DEBUG) m_log << MSG::DEBUG << "createObj: Wrote TagInfo to MetaDataStore " << endmsg;
    }

    // Do standard conversion to data object
    dataObj = SG::asStorable(std::move(tagInfo));

    if (outputLevel() <= MSG::DEBUG) {
        m_log << MSG::DEBUG << "createObj:  created new TagInfo object " << endmsg;
    }
    return StatusCode::SUCCESS;

}

//______________________________________________________________________________
StatusCode 
TagInfoMgr::createAddress(long svcType,
                const CLID& clid,
                const std::string* par,
                const unsigned long* ip,
                IOpaqueAddress*& refpAddress) {
   return(::ConversionSvc::createAddress(svcType, clid, par, ip, refpAddress));
}

//______________________________________________________________________________
StatusCode 
TagInfoMgr::convertAddress(const IOpaqueAddress* pAddress,
                std::string& refAddress) {
   assert(pAddress);
   refAddress = *pAddress->par();
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
StatusCode
TagInfoMgr::createAddress(long svcType,
                          const CLID& clid,
                          const std::string& refAddress,
                          IOpaqueAddress*& refpAddress) {
    if (svcType != TagInfoMgr_StorageType) {
        m_log << MSG::ERROR << "createAddress: svcType != TagInfoMgr_StorageType "
            << svcType
            << " "
            << TagInfoMgr_StorageType 
            << endmsg;
        return(StatusCode::FAILURE);
    }
    refpAddress = new GenericAddress(TagInfoMgr_StorageType, clid, refAddress);
    return(StatusCode::SUCCESS);
}

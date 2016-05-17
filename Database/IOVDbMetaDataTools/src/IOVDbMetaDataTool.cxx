/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   IOVDbMetaDataTool.cxx
 * 
 * @brief This is a tool used to manage the IOV Meta Data for a given
 * object into the Meta Data Store.
 * 
 * @author Antoine Perus <perus@lal.in2p3.fr>
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Header: /build/atlas/cvs/atlas/offline/Database/IOVDbMetaDataTools/src/IOVDbMetaDataTool.cxx,v 1.12 2009-04-29 07:44:13 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>
#include "IOVDbMetaDataTool.h"

// Gaudi includes
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/ToolFactory.h"

// Athena includes
#include "StoreGate/StoreGate.h"

// MetaData includes
#include "IOVDbDataModel/IOVMetaDataContainer.h"

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

//--------------------------------------------------------------------------

IOVDbMetaDataTool::IOVDbMetaDataTool(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface*  parent)
        : 
        AthAlgTool(type, name, parent),
        m_metaDataStore ("StoreGateSvc/MetaDataStore",      name), 
        m_inputStore    ("StoreGateSvc/InputMetaDataStore", name),
        m_processedFirstInputFileIncidence(false),
        m_overrideRunNumber(false),
        m_overrideMinMaxRunNumber(false),
        m_newRunNumber(0),
        m_oldRunNumber(0),
        m_minRunNumber(0),
        m_maxRunNumber(0),
        m_modifyFolders(false)
{
    // Declare additional interface
    declareInterface<IIOVDbMetaDataTool>(this);
    declareInterface<IMetaDataTool>(this);

    // Declare properties
    declareProperty("MinMaxRunNumbers",  m_minMaxRunNumbers);

    // Folders and attributes to be deleted
    declareProperty("FoldersToBeModified",  m_foldersToBeModified = 
                    std::vector<std::string>(1, "/Simulation/Parameters"));
    declareProperty("AttributesToBeRemoved",  m_attributesToBeRemoved =
                    std::vector<std::string>(1, "RandomSeedOffset"));
    
    

}

//--------------------------------------------------------------------------
IOVDbMetaDataTool::~IOVDbMetaDataTool()
{}


//--------------------------------------------------------------------------

StatusCode 
IOVDbMetaDataTool::initialize()
{
    ATH_MSG_DEBUG("in initialize()");

    // locate the meta data stores
    StatusCode sc = m_metaDataStore.retrieve();
    if (!sc.isSuccess() || 0 == m_metaDataStore) {
        ATH_MSG_ERROR("Could not find MetaDataStore");
        return(StatusCode::FAILURE);
    }
    sc = m_inputStore.retrieve();
    if (!sc.isSuccess() || 0 == m_inputStore) {
        ATH_MSG_ERROR("Could not find InputMetaDataStore");
        return(StatusCode::FAILURE);
    }

    // Set to be listener for FirstInputFile and BeginInputFile
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
    sc = incSvc.retrieve();
    if (!sc.isSuccess()) {
        ATH_MSG_ERROR("Unable to get the IncidentSvc");
        return(sc);
    }
    incSvc->addListener(this, "FirstInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
    incSvc->addListener(this, "BeginInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.
    //incSvc->addListener(this, "EndFile", 50); // pri has to be > 10 to be before MetaDataSvc.

    // Check whether folders need to be modified
    m_modifyFolders = false;
    if (m_foldersToBeModified.value().size() > 0) m_modifyFolders = true;
    if (m_modifyFolders) ATH_MSG_DEBUG("initialize(): need to modify folders ");

    return(StatusCode::SUCCESS);

}

//--------------------------------------------------------------------------

StatusCode 
IOVDbMetaDataTool::finalize()
{
    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void 
IOVDbMetaDataTool::handle(const Incident& inc) {
    const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
    if (fileInc == 0) {
        ATH_MSG_ERROR(" Unable to get FileName from BeginInputFile/EndInputFile incident");
        return;
    }
    const std::string fileName = fileInc->fileName();
    ATH_MSG_DEBUG("handle() " << inc.type() << " for " << fileName);

    if (inc.type() == "FirstInputFile" || inc.type() == "BeginInputFile") {

        if (inc.type() == "FirstInputFile") {
            m_processedFirstInputFileIncidence = true;
            // Check if we need to override run number - only needed
            // for simulation
            checkOverrideRunNumber();
        }
        else if (m_processedFirstInputFileIncidence) {
            // We skip the first BeginInputFile incidence following
            // the FirstInputFile - both are fired for the first file
            m_processedFirstInputFileIncidence = false;
            ATH_MSG_DEBUG("The first BeginInputFile incidence is fired along with the FirstInputFile incidence so we skip the processing of the Input File MetaData ");
            return;
        }
        // Process the input file meta data
        StatusCode sc = processInputFileMetaData(fileName);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR(" Could not process input file meta data");
            throw std::exception();
        }
    } else if (inc.type() == "EndFile") {
        // Do nothing
    }
}


//--------------------------------------------------------------------------

void
IOVDbMetaDataTool::checkOverrideRunNumber() 
{
    ATH_MSG_DEBUG("begin checkOverrideRunNumber");

    // Check if override run numbers have been set by properties or by
    // the EventSelector

    if (m_minMaxRunNumbers.value().size() > 0) {
        m_overrideMinMaxRunNumber = true;
        m_minRunNumber = m_minMaxRunNumbers.value()[0];
        if (m_minMaxRunNumbers.value().size() > 1) m_maxRunNumber = m_minMaxRunNumbers.value()[1];
        else m_maxRunNumber = m_minRunNumber;

        if (m_maxRunNumber > IOVTime::MAXRUN) m_maxRunNumber = IOVTime::MAXRUN;
        
        ATH_MSG_INFO("checkOverrideRunNumber: overriding IOV for range - min: " << m_minRunNumber 
                      << " max: " << m_maxRunNumber);
        return;
    }

    ATH_MSG_DEBUG("checkOverrideRunNumber: check if tag is set in jobOpts");

    // Get name of event selector from the application manager to
    // make sure we get the one for MC signal events
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("ApplicationMgr", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR("checkOverrideRunNumber: Cannot get ApplicationMgr "); 
        return;
    }
    StringProperty property("EvtSel", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        ATH_MSG_ERROR("checkOverrideRunNumber: unable to get EvtSel: found " << property.value());
        return;
    }
    // Get EventSelector for ApplicationMgr
    std::string eventSelector = property.value();
    sc = serviceLocator()->service(eventSelector, propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        ATH_MSG_ERROR("checkOverrideRunNumber: Cannot get EventSelector " << eventSelector); 
        return;
    }

    // Is flag set to override the run number? 
    BooleanProperty overrideRunNumber = IntegerProperty("OverrideRunNumberFromInput", false);
    sc = propertyServer->getProperty(&overrideRunNumber);
    if (!sc.isSuccess()) {
        // Not all EventSelectors have this property, so we must be tolerant
        ATH_MSG_DEBUG("resetRunNumber: unable to get OverrideRunNumberFromInput property from EventSelector ");
        return;
    }
    m_overrideRunNumber = overrideRunNumber.value();
    if (m_overrideRunNumber) {
        // New run number
        IntegerProperty runNumber = IntegerProperty("RunNumber", 0);
        sc = propertyServer->getProperty(&runNumber);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("checkOverrideRunNumber: unable to get RunNumber from EventSelector: found "
                          << runNumber.value());
            return;
        }
        m_newRunNumber = runNumber.value();
        // Old run number
        runNumber = IntegerProperty("OldRunNumber", 0);
        sc = propertyServer->getProperty(&runNumber);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("checkOverrideRunNumber: unable to get OldRunNumber from EventSelector: found "
                          << runNumber.value());
            return;
        }
        m_oldRunNumber = runNumber.value();

        ATH_MSG_DEBUG("checkOverrideRunNumber: Changing old to new run number:  " << m_oldRunNumber
                      << " " << m_newRunNumber << " obtained from " << eventSelector);
    }
    else ATH_MSG_DEBUG("checkOverrideRunNumber: OverrideRunNumberFromInput not set for " << eventSelector);
}

//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::registerFolder(const std::string& folderName) const
{
    // Set the default folder description for a CondAttrListCollection
    // which will be read back via IOVDbSvc
    std::string folderDescr = "<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"256\" clid=\"1238547719\" /> </addrHeader><typeName>CondAttrListCollection</typeName>" ;

    return registerFolder(folderName, folderDescr);
}

//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::registerFolder(const std::string& folderName, 
                                  const std::string& folderDescription) const
{
    ATH_MSG_DEBUG("begin registerFolder ");

    IOVMetaDataContainer* cont = getMetaDataContainer(folderName, folderDescription);
    if (!cont) {
        ATH_MSG_ERROR("Unable to register folder " << folderName);
        return(StatusCode::FAILURE);
    } 
    else {
        ATH_MSG_DEBUG("IOVMetaDataContainer  for folder " << folderName << " has been registered ");
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode      
IOVDbMetaDataTool::addPayload    (const std::string& folderName, 
                                  CondAttrListCollection* payload) const
{
    ATH_MSG_DEBUG("begin addPayload ");

    // Check if the  folder has already been found
    IOVMetaDataContainer* cont = 0;
    // see if it is in the meta data store
    if (!m_metaDataStore->contains<IOVMetaDataContainer>(folderName)) {
        ATH_MSG_ERROR("addPayload: Could not find IOVMetaDataContainer in MetaDataStore for folder " 
                      << folderName << " One must have previously called registerFolder. ");
        return(StatusCode::FAILURE);
    } 
    else {
        StatusCode sc = m_metaDataStore->retrieve(cont, folderName);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Could not retrieve IOVMetaDataContainer for folder " 
                          << folderName);
            return(StatusCode::FAILURE);
        }
        ATH_MSG_DEBUG("Retrieved IOVMetaDataContainer from MetaDataStore for folder " 
                      << folderName);
    }

    // Override run number if requested
    if (m_overrideRunNumber || m_overrideMinMaxRunNumber) overrideIOV(payload);

    // Add payload to container
    bool success = cont->merge(payload);
    if (success) {
        ATH_MSG_DEBUG("Added new payload for folder " << folderName);
    }
    else {
        ATH_MSG_DEBUG("Could not add new payload for folder " 
            << folderName 
            << " (may be duplicate payload). Dump of payload: " );
            //printOut(payload);
        delete payload;
        payload = 0;

    }

    if (payload && msgLvl(MSG::DEBUG)) {
        printOut(payload);
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::modifyPayload (const std::string& folderName, 
                                  CondAttrListCollection*& coll) const
{

    /// Modify a Payload for a particular folder - replaces one of the
    /// internal attributes
    ATH_MSG_DEBUG("begin modifyPayload for folder " << folderName);

    // check if this folder needs to be modified
    bool modifyAttr = false;
    std::string attributeName;
    const std::vector<std::string>& folders = m_foldersToBeModified.value();
    const std::vector<std::string>& attrs   = m_attributesToBeRemoved.value();
    for (unsigned int i = 0; i < folders.size(); ++i) {
        if (folderName == folders[i]) {
            if (attrs.size() > i) {
                attributeName = attrs[i];
                modifyAttr    = true;
                ATH_MSG_DEBUG("modifyPayload: remove attribute " << attributeName);
                break;
            }
        }
    }

    if (!modifyAttr) {
        ATH_MSG_DEBUG("modifyPayload: folder " << folderName << " OK ");
        return StatusCode::SUCCESS;
    }

    bool iovSizeIsZero = coll->iov_size() == 0;
    IOVRange testIOV = coll->minRange();
    IOVTime  start   = testIOV.start();
    IOVTime  stop    = testIOV.stop();
    // Set the IOV
    CondAttrListCollection* coll1 = new CondAttrListCollection(true);
    if (iovSizeIsZero) { 
        // Only add in overall range if channels do not have
        // IOVs - otherwise this is automatically calculated
        coll1->addNewStart(start);
        coll1->addNewStop (stop);
    }
    // Add in channels
    unsigned int nchans = coll->size();
    bool hasChanNames = (coll->name_size() == nchans);
    for (unsigned int ichan = 0; ichan < nchans; ++ichan) {
        CondAttrListCollection::ChanNum chan = coll->chanNum(ichan);
        // Now filter out the unwanted attribute
        CondAttrListCollection::AttributeList  newAttrList;
        CondAttrListCollection::AttributeList  oldAttrList = coll->attributeList(chan);
        for (unsigned int iatt = 0; iatt < oldAttrList.size(); ++iatt) {
            // skip the unwanted attribute
            if (attributeName == oldAttrList[iatt].specification().name()) {
                ATH_MSG_DEBUG("modifyPayload: skipping attribute name " << oldAttrList[iatt].specification().name());
                continue;
            }
                
            // copy the rest
            newAttrList.extend(oldAttrList[iatt].specification().name(),
                               oldAttrList[iatt].specification().type());
            const coral::Attribute&  oldAttr = oldAttrList[iatt];
            coral::Attribute&        newAttr = newAttrList[oldAttrList[iatt].specification().name()];
            newAttr = oldAttr;
            // newAttr.setValue(oldAttr.data());
            ATH_MSG_DEBUG("modifyPayload: copying attr name " 
                          << oldAttrList[iatt].specification().name() << " " 
                          /*<< newAttr*/);
        }
        coll1->add(chan, newAttrList);
        if (!iovSizeIsZero) coll1->add(chan, coll->iovRange(chan));
        if(hasChanNames)coll1->add(chan, coll->chanName(chan));
        ATH_MSG_DEBUG("modifyPayload: copyied attribute list for channel " << chan);
    }
    delete coll;
    coll = coll1;
    if (msgLvl(MSG::DEBUG)) printOut(coll);

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

IOVMetaDataContainer* 
IOVDbMetaDataTool::getMetaDataContainer(const std::string& folderName, 
                                        const std::string& folderDescription) const
{
    
    ATH_MSG_DEBUG("begin getMetaDataContainer ");

    // See if it is in the meta data store
    if (!m_metaDataStore->contains<IOVMetaDataContainer>(folderName)) {
        // Container not found, add in new one
        IOVMetaDataContainer* cont = new IOVMetaDataContainer(folderName, folderDescription);
        StatusCode sc = m_metaDataStore->record(cont, folderName);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Could not record IOVMetaDataContainer in MetaDataStore for folder " 
                          << folderName);
            return(0);
        }
        return (cont);
    } 
    else {
        ATH_MSG_DEBUG("IOVMetaDataContainer already in MetaDataStore for folder " << folderName);
        IOVMetaDataContainer* cont;
        StatusCode sc = m_metaDataStore->retrieve(cont, folderName);
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Could not retrieve IOVMetaDataContainer in MetaDataStore for folder " 
                          << folderName);
            return(0);
        }
        return (cont);
    }
}

//--------------------------------------------------------------------------
void
IOVDbMetaDataTool::printOut(const CondAttrListCollection* coll) const
{
    ATH_MSG_DEBUG("in printOut(const CondAttrListCollection* coll)");

    if (!coll) {
        ATH_MSG_DEBUG("null pointer - no printout");
        return;
    }

    ATH_MSG_DEBUG(coll->minRange() << " iov size " <<     coll->iov_size());
    CondAttrListCollection::iov_const_iterator  itIOV    = coll->iov_begin();
    CondAttrListCollection::iov_const_iterator  itIOVEnd = coll->iov_end  ();
    for (; itIOV != itIOVEnd; ++itIOV) {
        ATH_MSG_DEBUG((*itIOV).first << " " << (*itIOV).second);
    }

    CondAttrListCollection::const_iterator  itAtt    = coll->begin();
    CondAttrListCollection::const_iterator  itAttEnd = coll->end  ();
    for (; itAtt != itAttEnd; ++itAtt) {
        std::ostringstream attrStr;
        attrStr << "{";
        for (coral::AttributeList::const_iterator itr= (*itAtt).second.begin();
             itr !=  (*itAtt).second.end();++itr) {
            if (itr !=  (*itAtt).second.begin()) attrStr << ",";
            itr->toOutputStream(attrStr);
        }
        attrStr << "}";
        ATH_MSG_DEBUG((*itAtt).first << " " << attrStr.str());
    }
}


//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::processInputFileMetaData(const std::string& fileName)
{
    ATH_MSG_DEBUG("processInputFileMetaData: file name " << fileName);

    // Retrieve all meta data containers from InputMetaDataStore

    // Before migrating to SG::ConstIterator, the 'key' method needs to be defined! RDS 08/2010
    // SG::ConstIterator<IOVMetaDataContainer> cont;
    // SG::ConstIterator<IOVMetaDataContainer> contEnd;
    const DataHandle<IOVMetaDataContainer> cont;
    const DataHandle<IOVMetaDataContainer> contEnd;


    StatusCode sc = m_inputStore->retrieve(cont, contEnd);
    if (!sc.isSuccess()) {
        ATH_MSG_DEBUG("Could not retrieve IOVMetaDataContainer objects from InputMetaDataStore - cannot process input file meta data");
        return(StatusCode::SUCCESS);
    }

    ATH_MSG_DEBUG("Retrieved from IOVMetaDataContainer(s) from InputMetaDataStore");

    // For each container, merge its contents into the MetaDataStore 
    unsigned int ncolls    = 0;
    unsigned int ndupColls = 0;
    for (; cont != contEnd; ++cont) {

        IOVMetaDataContainer* contMaster    = getMetaDataContainer(cont->folderName(), 
                                                                   cont->folderDescription());

        // We assume that the folder is the same for all versions, and
        // now we loop over versions for the payloads

        std::list<SG::ObjectWithVersion<IOVMetaDataContainer> > allVersions;
        sc = m_inputStore->retrieveAllVersions(allVersions, cont.key());
        if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Could not retrieve all versions for " << cont.key());
            return sc;
        }

        std::list<SG::ObjectWithVersion<IOVMetaDataContainer> >::const_iterator versIt  = allVersions.begin(); 
        std::list<SG::ObjectWithVersion<IOVMetaDataContainer> >::const_iterator versEnd = allVersions.end(); 
        for (; versIt != versEnd; ++versIt) {

            const IOVPayloadContainer*  payload = versIt->dataObject->payloadContainer();

            ATH_MSG_DEBUG("New container: payload size " << payload->size() << " version key " << versIt->versionedKey);

            // detailed printout before merge
            if (msgLvl(MSG::VERBOSE)) {
                const IOVPayloadContainer*  payloadMaster = contMaster->payloadContainer();
                ATH_MSG_VERBOSE("Before merge, payload minRange for folder " << cont->folderName());
                if (payloadMaster && payloadMaster->size()) {
                    // Loop over AttrColls and print out minRange
                    IOVPayloadContainer::const_iterator itColl    = payloadMaster->begin();
                    IOVPayloadContainer::const_iterator itCollEnd = payloadMaster->end();
                    unsigned int iPayload = 0;
                    for (; itColl != itCollEnd; ++itColl, ++iPayload) {
                        ATH_MSG_VERBOSE(iPayload << " " << (*itColl)->minRange() << " " 
                                        << (*itColl)->size());
                    }
                }
                else { ATH_MSG_VERBOSE("  no payloads yet!"); }
            }
        }
        
        // Detailed printout
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("Current payload before merge " << contMaster->folderName());
            IOVPayloadContainer::const_iterator itColl1    = contMaster->payloadContainer()->begin();
            IOVPayloadContainer::const_iterator itCollEnd1 = contMaster->payloadContainer()->end();
            for (; itColl1 != itCollEnd1; ++itColl1) printOut(*itColl1);
        }


        //
        // Loop over CondAttrListCollections and do merge
        //
        versIt  = allVersions.begin(); 
        versEnd = allVersions.end(); 
        for (; versIt != versEnd; ++versIt) {
            const IOVPayloadContainer*  payload = versIt->dataObject->payloadContainer();
            IOVPayloadContainer::const_iterator itColl    = payload->begin();
            IOVPayloadContainer::const_iterator itCollEnd = payload->end();
            for (; itColl != itCollEnd; ++itColl) {

                // Make a copy of the collection and merge it into
                // master container in meta data store 
                CondAttrListCollection* coll = new CondAttrListCollection(**itColl);
                // Override run number if requested
                if (m_overrideRunNumber || m_overrideMinMaxRunNumber) overrideIOV(coll);

                // first check if we need to modify the incoming payload
                if (!modifyPayload (contMaster->folderName(), coll).isSuccess()) {
                    ATH_MSG_ERROR("Could not modify the payload for folder " 
                                  << contMaster->folderName());
                    return(StatusCode::FAILURE);
                }


                ATH_MSG_VERBOSE(" merge minRange: " << coll->minRange());
                if (!contMaster->merge(coll)) {
                    // Did not merge it in - was a duplicate, so we need to delete it 
                    delete coll;
                    ++ndupColls;
                    ATH_MSG_VERBOSE(" => not merged ");
                }
                else {
                    ++ncolls;
                    ATH_MSG_VERBOSE(" => merged ");
                }
            }
            ATH_MSG_DEBUG("Merged together containers for folder " << cont->folderName() << " ncoll/ndup " 
                          << ncolls << " " << ndupColls);

            // Check for consistency after merge
            const IOVPayloadContainer*  payloadMaster = contMaster->payloadContainer();
            if (payloadMaster && payloadMaster->size()) {
                // Loop over AttrColls and print out minRange
                IOVPayloadContainer::const_iterator itColl    = payloadMaster->begin();
                IOVPayloadContainer::const_iterator itCollEnd = payloadMaster->end();
                IOVTime lastStop;
                if ((*itColl)->minRange().start().isTimestamp()) lastStop = IOVTime(0);
                else lastStop = IOVTime(0,0);
                bool hasError = false;
                for (; itColl != itCollEnd; ++itColl) {
                    if ((*itColl)->minRange().start() < lastStop) hasError = true;
                    lastStop = (*itColl)->minRange().stop();
                }
                if (hasError) {
                    ATH_MSG_ERROR("processInputFileMetaData: error after merge of file meta data. " );
                    ATH_MSG_ERROR("processInputFileMetaData: Filename " << fileName);
                    ATH_MSG_ERROR("processInputFileMetaData: folder " << contMaster->folderName());
                    ATH_MSG_ERROR("processInputFileMetaData: MinRange for meta data folders ");
                    unsigned int iPayload = 0;
                    itColl    = payloadMaster->begin();
                    for (; itColl != itCollEnd; ++itColl, ++iPayload) {
                        ATH_MSG_ERROR(iPayload << " " << (*itColl)->minRange() << " " << (*itColl)->size());
                    }
                }
            }

            // detailed printout after merge
            if (msgLvl(MSG::VERBOSE)) {
                const IOVPayloadContainer*  payloadMaster = contMaster->payloadContainer();
                ATH_MSG_VERBOSE("After merge, payload minRange ");
                if (payloadMaster) {
                    // Loop over AttrColls and print out minRange
                    IOVPayloadContainer::const_iterator itColl    = payloadMaster->begin();
                    IOVPayloadContainer::const_iterator itCollEnd = payloadMaster->end();
                    unsigned int iPayload = 0;
                    for (; itColl != itCollEnd; ++itColl, ++iPayload) {
                        ATH_MSG_VERBOSE(iPayload << " " << (*itColl)->minRange() << " " 
                                        << (*itColl)->size());
                    }
                }
                else { ATH_MSG_ERROR("  no payloads yet!"); }
            }

            // Detailed printout
            if (msgLvl(MSG::DEBUG)) {
                ATH_MSG_DEBUG("Input payload " << cont->folderName());
                itColl    = payload->begin();
                itCollEnd = payload->end();
                for (; itColl != itCollEnd; ++itColl) printOut(*itColl);
                ATH_MSG_DEBUG("Output payload " << contMaster->folderName());
                itColl    = contMaster->payloadContainer()->begin();
                itCollEnd = contMaster->payloadContainer()->end();
                for (; itColl != itCollEnd; ++itColl) printOut(*itColl);
            }
        }
    }

    ATH_MSG_DEBUG("Total number of attribute collections  merged together " << ncolls
                  << " Number of duplicate collections " << ndupColls);
    return(StatusCode::SUCCESS);
}

//--------------------------------------------------------------------------

void
IOVDbMetaDataTool::overrideIOV(CondAttrListCollection*& coll) const
{
    ATH_MSG_DEBUG("overrideIOV ");

    // Override the IOV for run/event IOVs

    //   (ONLY TRUE FOR OVERRIDE COMING IN VIA EVENTSELECTOR:)
    //   NOTE: we require that the old run number falls within the
    //   IOVRange of the incoming collection. We override ALL IOVs for
    //   ALL channels forcing the IOVRange to be (newRunNumber,1) to
    //   (newRunNumber+1,1)

    bool iovSizeIsZero = coll->iov_size() == 0;
    IOVRange testIOV = coll->minRange();
    IOVTime  start   = testIOV.start();
    IOVTime  stop    = testIOV.stop();
    IOVTime  oldRun(m_oldRunNumber, 0);
    if (start.isRunEvent() && stop.isRunEvent()) { // only for run/event
        IOVRange newRange;
        // Two ways of resetting 
        if (m_overrideMinMaxRunNumber) newRange = IOVRange(IOVTime(m_minRunNumber, 0), IOVTime(m_maxRunNumber, IOVTime::MAXEVENT));
        else if (m_overrideRunNumber)  newRange = IOVRange(IOVTime(m_newRunNumber, 0), IOVTime(m_newRunNumber + 1, 0));

        if (m_overrideRunNumber && !testIOV.isInRange(oldRun)) { 
            // old run must be in the range
            ATH_MSG_ERROR("overrideIOV: old run number does not match. Old run number " << m_oldRunNumber << " IOVRange: " << testIOV);
            return;
        }
        // Now over ride IOVs
        CondAttrListCollection* coll1 = new CondAttrListCollection(true);
        if (iovSizeIsZero) { 
            // Only add in overall range if channels do not have
            // IOVs - otherwise this is automatically calculated
            coll1->addNewStart(newRange.start());
            coll1->addNewStop (newRange.stop());
        }
        // Add in channels
        unsigned int nchans = coll->size();
        bool hasChanNames = (coll->name_size() == nchans);
        for (unsigned int ichan = 0; ichan < nchans; ++ichan) {
            CondAttrListCollection::ChanNum chan = coll->chanNum(ichan);
            coll1->add(chan, coll->attributeList(chan));
            if (!iovSizeIsZero) coll1->add(chan, newRange);
            if(hasChanNames)coll1->add(chan, coll->chanName(chan));
            ATH_MSG_DEBUG("overrideIOV: overriding the IOV of collection " << chan);
        }
        delete coll;
        coll = coll1;
        if (msgLvl(MSG::DEBUG)) printOut(coll);
    }
    else ATH_MSG_DEBUG("overrideIOV: IOV is not run/event ");
}



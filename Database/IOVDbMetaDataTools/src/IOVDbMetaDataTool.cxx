/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
 */

#include "IOVDbMetaDataTool.h"

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/FileIncident.h"

#include "StoreGate/StoreGate.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "AthenaKernel/MetaCont.h"
#include "CxxUtils/checker_macros.h"

IOVDbMetaDataTool::IOVDbMetaDataTool(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface*  parent)
  : AthAlgTool(type, name, parent)
  , m_metaDataStore ("StoreGateSvc/MetaDataStore",      name)
  , m_inputStore    ("StoreGateSvc/InputMetaDataStore", name)
  , m_processedFirstInputFileIncident(false)
  , m_overrideRunNumber(false)
  , m_overrideMinMaxRunNumber(false)
  , m_newRunNumber(0)
  , m_oldRunNumber(0)
  , m_minRunNumber(0)
  , m_maxRunNumber(0)
  , m_modifyFolders(false)
{
  // Declare additional interface
  declareInterface<IIOVDbMetaDataTool>(this);
  declareInterface<IMetaDataTool>(this);

  // Declare properties
  declareProperty("MinMaxRunNumbers",  m_minMaxRunNumbers);

  // Folders and attributes to be deleted
  declareProperty("FoldersToBeModified"
		  , m_foldersToBeModified = std::vector<std::string>(1, "/Simulation/Parameters"));
  declareProperty("AttributesToBeRemoved"
		  , m_attributesToBeRemoved = std::vector<std::string>(1, "RandomSeedOffset"));

}

//--------------------------------------------------------------------------
IOVDbMetaDataTool::~IOVDbMetaDataTool()
{}


//--------------------------------------------------------------------------

StatusCode IOVDbMetaDataTool::initialize()
{
  ATH_MSG_DEBUG("in initialize()");

  // locate the meta data stores
  ATH_CHECK(m_metaDataStore.retrieve());
  ATH_CHECK(m_inputStore.retrieve());

  // Set to be listener for FirstInputFile
  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
  ATH_CHECK(incSvc.retrieve());
  incSvc->addListener(this, "FirstInputFile", 60); // pri has to be < 100 to be after MetaDataSvc.

  // Check whether folders need to be modified
  m_modifyFolders = (m_foldersToBeModified.value().size()>0);
  ATH_MSG_DEBUG("initialize(): " << (m_modifyFolders ? "" : "No ") << "need to modify folders");

  return(StatusCode::SUCCESS);
}

//--------------------------------------------------------------------------

StatusCode 
IOVDbMetaDataTool::finalize()
{
    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void IOVDbMetaDataTool::handle(const Incident& inc)
{
  const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
  if(!fileInc) throw std::runtime_error("Unable to get FileName from FirstInputFile incident");

  const std::string fileName = fileInc->fileName();
  ATH_MSG_DEBUG("handle() " << inc.type() << " for " << fileName);

  m_processedFirstInputFileIncident = true;
  // Check if we need to override run number - only needed for simulation
  checkOverrideRunNumber();

  StatusCode sc = processInputFileMetaData(fileName);
  if(!sc.isSuccess()) throw std::runtime_error("Could not process input file meta data");
}

StatusCode IOVDbMetaDataTool::beginInputFile(const SG::SourceID& sid)
{
  if(m_processedFirstInputFileIncident) {
    // We skip the first BeginInputFile incident following the FirstInputFile - both are fired for the first file
    m_processedFirstInputFileIncident = false;
    ATH_MSG_DEBUG("The first BeginInputFile incident is fired along with the FirstInputFile incident so we skip the processing of the Input File MetaData ");
    return StatusCode::SUCCESS;
  }
  else {
    return processInputFileMetaData(sid);
  }
}

StatusCode IOVDbMetaDataTool::endInputFile(const SG::SourceID&)
{
  return StatusCode::SUCCESS;
}

StatusCode IOVDbMetaDataTool::metaDataStop()
{
  StatusCode sc = dumpMetaConts();
  return sc;
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
    // lock the tool before getMetaDataContainer() call
    std::scoped_lock  guard( m_mutex );

    ATH_MSG_DEBUG("begin registerFolder ");

    if( ! getMetaDataContainer(folderName, folderDescription) ) {
        ATH_MSG_ERROR("Unable to register folder " << folderName);
        return(StatusCode::FAILURE);
    } 
    else {
        ATH_MSG_DEBUG("IOVMetaDataContainer  for folder " << folderName << " has been registered ");
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode IOVDbMetaDataTool::addPayload (const std::string& folderName
					  , CondAttrListCollection* payload) const
{
  // lock the tool while it is modifying the folder
  std::scoped_lock  guard( m_mutex );

  ATH_MSG_DEBUG("begin addPayload ");
  
  // Check if the  folder has already been found
  IOVMetaDataContainer* cont = m_metaDataStore->tryRetrieve<IOVMetaDataContainer>(folderName);
  if(cont) {
    ATH_MSG_DEBUG("Retrieved IOVMetaDataContainer from MetaDataStore for folder " 
		  << folderName);
  }
  else {
    ATH_MSG_ERROR("addPayload: Could not find IOVMetaDataContainer in MetaDataStore for folder " 
		   << folderName 
		   << ". One must have previously called registerFolder. ");
     return StatusCode::FAILURE;
  }

  // Override run number if requested
  if (m_overrideRunNumber || m_overrideMinMaxRunNumber) {
    StatusCode sc = overrideIOV(payload);
    ATH_CHECK( sc );
  }

  // Add payload to container
  bool success = cont->merge(payload);
  if (success) {
    ATH_MSG_DEBUG("Added new payload for folder " << folderName);
  }
  else {
    ATH_MSG_DEBUG("Could not add new payload for folder " 
		  << folderName 
		  << " (may be duplicate payload).");

    // To Do: the function implicitly assumes ownership on the payload pointer
    delete payload;
    payload = nullptr;
  }

  // Debug printout
  if(payload && msgLvl(MSG::DEBUG)) {
    std::ostringstream stream;
    payload->dump(stream);
    ATH_MSG_DEBUG(stream.str());
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::modifyPayload ATLAS_NOT_THREAD_SAFE  (const std::string& folderName, 
                                                         CondAttrListCollection*& coll) const
{
    // protected by lock in processInputFileMetaData()

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
        const CondAttrListCollection::AttributeList& oldAttrList = coll->attributeList(chan);
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
    if (msgLvl(MSG::DEBUG)) {
      std::ostringstream stream;
      coll->dump(stream);
      ATH_MSG_DEBUG(stream.str());
    }

    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

IOVMetaDataContainer*
IOVDbMetaDataTool::findMetaDataContainer(const std::string& folderName) const
{
  // lock the tool before this call
  // Return the folder if it is in the meta data store
  return m_metaDataStore->tryRetrieve<IOVMetaDataContainer>(folderName);
}


IOVMetaDataContainer* 
IOVDbMetaDataTool::getMetaDataContainer(const std::string& folderName
                                        , const std::string& folderDescription) const
{
  // protected by locks in addPayload() and registerFolder()
  ATH_MSG_DEBUG("begin getMetaDataContainer ");

  IOVMetaDataContainer* cont{nullptr};
  // See if it is in the meta data store
  if (!m_metaDataStore->contains<IOVMetaDataContainer>(folderName)) {
    // Container not found, add in new one
    cont = new IOVMetaDataContainer(folderName, folderDescription);
    ATH_MSG_DEBUG("No IOVMetaDataContainer in MetaDataStore for folder " << folderName
		  << ". Created a new instance");
    StatusCode sc = m_metaDataStore->record(cont, folderName);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not record IOVMetaDataContainer in MetaDataStore for folder " << folderName);
      delete cont;
      cont = nullptr;
    }
  } 
  else {
    ATH_MSG_DEBUG("IOVMetaDataContainer already in MetaDataStore for folder " << folderName);
    StatusCode sc = m_metaDataStore->retrieve(cont, folderName);
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not retrieve IOVMetaDataContainer in MetaDataStore for folder " << folderName);
      cont = nullptr;
    }
  }
  return cont;
}

//--------------------------------------------------------------------------

StatusCode IOVDbMetaDataTool::processInputFileMetaData(const std::string& fileName)
{
  // lock the tool while it is processing input metadata 
  std::scoped_lock  guard( m_mutex );

  ATH_MSG_DEBUG("processInputFileMetaData: file name " << fileName);

  // Retrieve all meta data containers from InputMetaDataStore
  SG::ConstIterator<IOVMetaDataContainer> cont;
  SG::ConstIterator<IOVMetaDataContainer> contEnd;

  StatusCode sc = m_inputStore->retrieve(cont, contEnd);
  if (!sc.isSuccess()) {
    ATH_MSG_DEBUG("processInputFileMetaData: Could not retrieve IOVMetaDataContainer objects from InputMetaDataStore - cannot process input file meta data");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_DEBUG("processInputFileMetaData: Retrieved from IOVMetaDataContainer(s) from InputMetaDataStore");

  // For each container, merge its contents into the MetaDataStore 
  unsigned int ncolls    = 0;
  unsigned int ndupColls = 0;
  for (; cont != contEnd; ++cont) {
    IOVMetaDataContainer* contMaster = getMetaDataContainer(cont->folderName()
							    , cont->folderDescription());

    // At the same time we want to create a new instance of IOVMetaDataContainer for new SID
    // and store it into MetaCont<IOVMetaDataContainer>
    IOVMetaDataContainer* newCont4Sid = new IOVMetaDataContainer(cont->folderName()
								 , cont->folderDescription()); 

    // We assume that the folder is the same for all versions, and
    // now we loop over versions for the payloads
    std::list<SG::ObjectWithVersion<IOVMetaDataContainer> > allVersions;
    sc = m_inputStore->retrieveAllVersions(allVersions, cont.key());
    if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not retrieve all versions for " << cont.key());
      return sc;
    }

    for (SG::ObjectWithVersion<IOVMetaDataContainer>& obj : allVersions) {
      const IOVPayloadContainer*  payload = obj.dataObject->payloadContainer();

      ATH_MSG_DEBUG("processInputFileMetaData: New container: payload size " << payload->size() << " version key " << obj.versionedKey);

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
	else { 
	  ATH_MSG_VERBOSE("  no payloads yet!"); 
	}
      }
    }
        
    // Detailed printout
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("processInputFileMetaData: Current payload before merge " << contMaster->folderName());
      IOVPayloadContainer::const_iterator itColl1    = contMaster->payloadContainer()->begin();
      IOVPayloadContainer::const_iterator itCollEnd1 = contMaster->payloadContainer()->end();
      std::ostringstream stream;
      for (; itColl1 != itCollEnd1; ++itColl1) (*itColl1)->dump(stream);
      ATH_MSG_DEBUG(stream.str());
    }

    //
    // Loop over CondAttrListCollections and do merge
    //
    for (SG::ObjectWithVersion<IOVMetaDataContainer>& obj : allVersions) {
      const IOVPayloadContainer*  payload = obj.dataObject->payloadContainer();
      IOVPayloadContainer::const_iterator itColl    = payload->begin();
      IOVPayloadContainer::const_iterator itCollEnd = payload->end();
      for (; itColl != itCollEnd; ++itColl) {

	// Make a copy of the collection and merge it into
	// master container in meta data store 
	CondAttrListCollection* coll = new CondAttrListCollection(**itColl);
	// Override run number if requested
	if (m_overrideRunNumber || m_overrideMinMaxRunNumber) {
          StatusCode sc = overrideIOV(coll);
          ATH_CHECK( sc );
        }

	// first check if we need to modify the incoming payload
        {
          // Should be ok.
          StatusCode sc ATLAS_THREAD_SAFE =
            modifyPayload (contMaster->folderName(), coll);
          if (!sc.isSuccess()) {
            ATH_MSG_ERROR("processInputFileMetaData: Could not modify the payload for folder " << contMaster->folderName());
            return StatusCode::FAILURE;
          }
        }

	// Do the same with newCont4Sid
        {
          // Should be ok.
          StatusCode sc ATLAS_THREAD_SAFE =
            modifyPayload (newCont4Sid->folderName(), coll);
          if (!sc.isSuccess()) {
            ATH_MSG_ERROR("processInputFileMetaData: Could not modify the payload for folder " << newCont4Sid->folderName());
            return StatusCode::FAILURE;
          }
        }

	// Before starting merging, make a copy for newCont4Sid
	CondAttrListCollection* collCopy = new CondAttrListCollection(*coll);

	ATH_MSG_VERBOSE("processInputFileMetaData: merge minRange: " << coll->minRange());
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

	ATH_MSG_VERBOSE("processInputFileMetaData:  merge for MetaCont minRange: " << collCopy->minRange());
	if (!newCont4Sid->merge(collCopy)) {
	  // Did not merge it in - was a duplicate, so we need to delete it 
	  delete collCopy;
	  ATH_MSG_VERBOSE(" => not merged ");
	}
	else {
	  ATH_MSG_VERBOSE(" => merged ");
	}

      }
      ATH_MSG_DEBUG("processInputFileMetaData: Merged together containers for folder " << cont->folderName() << " ncoll/ndup " 
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

      // Insert the merged container into MetaCont
      if(fillMetaCont(fileName,newCont4Sid).isFailure()) {
	ATH_MSG_ERROR("processInputFileMetaData: Failed to insert the merged IOVMetaDataContainer into MetaCont");
	return StatusCode::FAILURE;
      }

      // detailed printout after merge
      if (msgLvl(MSG::VERBOSE)) {
	const IOVPayloadContainer*  payloadMaster = contMaster->payloadContainer();
	ATH_MSG_VERBOSE("processInputFileMetaData: After merge, payload minRange ");
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
	ATH_MSG_DEBUG("processInputFileMetaData: Input payload " << cont->folderName());
	std::ostringstream streamInp;
	itColl    = payload->begin();
	itCollEnd = payload->end();
	for (; itColl != itCollEnd; ++itColl) (*itColl)->dump(streamInp);
	ATH_MSG_DEBUG(streamInp.str());
	ATH_MSG_DEBUG("processInputFileMetaData: Output payload " << contMaster->folderName());
	std::ostringstream streamOut;
	itColl    = contMaster->payloadContainer()->begin();
	itCollEnd = contMaster->payloadContainer()->end();
	for (; itColl != itCollEnd; ++itColl) (*itColl)->dump(streamOut);
	ATH_MSG_DEBUG(streamOut.str());
      }
    }
  }

  ATH_MSG_DEBUG("processInputFileMetaData: Total number of attribute collections  merged together " << ncolls
		<< " Number of duplicate collections " << ndupColls);
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

StatusCode
IOVDbMetaDataTool::overrideIOV (CondAttrListCollection*& coll) const
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
            return StatusCode::SUCCESS;
        }

        ATH_MSG_DEBUG("overrideIOV: overrideMinMaxRunNumber: " << (int)m_overrideMinMaxRunNumber
                      << " overrideRunNumber " << (int)m_overrideRunNumber
                      << " iovSizeIsZero: " << (int)iovSizeIsZero
                      << " newRange " << newRange);
        
        // Now over ride IOVs - two cases: 1) single IOV for full collection, 2) IOVs for individual channels.
        // Must treat the reset of collection IOV differently
        if (iovSizeIsZero) {
            // Only add in overall range if channels do not have
            // IOVs - otherwise this is automatically calculated
            coll->resetMinRange(); // must first reset to 'full range' and then reduce the IOVRange accordingly
            coll->addNewStart(newRange.start());
            coll->addNewStop (newRange.stop());
        }
        else {
            // Add in channels
            unsigned int nchans = coll->size();
            ATH_MSG_DEBUG("overrideIOV: nchans " << nchans);
            for (unsigned int ichan = 0; ichan < nchans; ++ichan) {
                // FIXME: O(N^2)!
                CondAttrListCollection::ChanNum chan = coll->chanNum(ichan);
                coll->add(chan, newRange);
                ATH_MSG_DEBUG("overrideIOV: overriding the IOV of collection chan " << chan);
            }
            // must reset the collection range AFTER the channels, because the collection range will be
            // 'narrowed' to that of the channels
            coll->resetMinRange();
        }
        if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG("overrideIOV: after  overriding the IOV of collection");
            std::ostringstream stream;
            coll->dump(stream);
            ATH_MSG_DEBUG(stream.str());
        }
    }
    else ATH_MSG_DEBUG("overrideIOV: IOV is not run/event ");

    return StatusCode::SUCCESS;
}

StatusCode IOVDbMetaDataTool::fillMetaCont(const std::string& sid
					   , const IOVMetaDataContainer* iovCont)
{
  ATH_MSG_DEBUG("fillMetaCont for SID=" << sid 
		<< " and folder name=" << iovCont->folderName());
 
  // Drop the leading 'FID:' from sid, if present
  std::string keyInMetaCont = sid.compare(0,4,std::string("FID:")) ? sid : sid.substr(4);

  IOVMetaDataContainer* copyCont = new IOVMetaDataContainer(*iovCont);
  ATH_MSG_DEBUG("Created a copy container");
  MetaCont<IOVMetaDataContainer>* pMetaCont = m_metaDataStore->tryRetrieve<MetaCont<IOVMetaDataContainer>>(iovCont->folderName());
  if(pMetaCont) {
    pMetaCont->insert(keyInMetaCont,copyCont);
    ATH_MSG_DEBUG("New element added to an existing MetaCont in the store");
  }
  else {
    std::unique_ptr<MetaCont<IOVMetaDataContainer>> pMetaContPtr = std::make_unique<MetaCont<IOVMetaDataContainer>>();
    pMetaContPtr->insert(keyInMetaCont,copyCont);
    ATH_MSG_DEBUG("Created new MetaCont, and added new element to it");

    StatusCode sc = m_metaDataStore->record(std::move(pMetaContPtr),iovCont->folderName());
    if(sc.isFailure()) {
      ATH_MSG_FATAL("Failed to record MetaCont with key " << iovCont->folderName() << " into the store");
      delete copyCont;
      return sc;
    }
    ATH_MSG_DEBUG("New MetaCont recorded into the store");
  } // pMetaCont == nullptr

  return StatusCode::SUCCESS;
}

StatusCode IOVDbMetaDataTool::dumpMetaConts()
{
  if(msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG("Begin dumping MetaCont<IOVMetaDataContainer> objects from MetaDataStore");
    const std::vector<std::string>& keys = m_metaDataStore->keys<MetaCont<IOVMetaDataContainer>>();
    if(keys.empty()) ATH_MSG_DEBUG(" ... NONE ...");
    for(const auto& key : keys) {
      const MetaCont<IOVMetaDataContainer>* metacont{nullptr};
      if(m_metaDataStore->retrieve(metacont,key).isFailure()) {
	ATH_MSG_FATAL("Failed to retrieve MetaCont<IOVMetaDataContainer> for key=" << key
		      << " from the MetaDataStore");
	return StatusCode::FAILURE;
      }

      ATH_MSG_DEBUG("... Dumping " << key);
      std::ostringstream stream;

      stream << "\nMetaCont :::::" << std::endl;
      std::vector<SG::SourceID> sources = metacont->sources();
      for(const auto& source : sources) {
	IOVMetaDataContainer* ptr{nullptr};
	if(metacont->find(source,ptr)) {
	  stream << "SID : " << source << std::endl;
	  stream << "OBJ :: " << std::endl;
	  ptr->dump(stream);
	}
	else {
	  ATH_MSG_FATAL("Failed to get IOVMetaDataContainer from MetaCont for the source " << source);
	  return StatusCode::FAILURE;
	}
      }

      ATH_MSG_DEBUG(stream.str());
      ATH_MSG_DEBUG("... ... ...");
    }
    ATH_MSG_DEBUG("End dumping MetaCont<IOVMetaDataContainer> objects from MetaDataStore");
  }
  return StatusCode::SUCCESS;
}

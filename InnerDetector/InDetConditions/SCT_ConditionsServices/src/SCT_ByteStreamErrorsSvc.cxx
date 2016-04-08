/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsSvc.cxx
 * implementation file for service that keeps track of errors in the bytestream
 * @author nbarlow@cern.ch
**/
/// header file for this class.
#include "SCT_ByteStreamErrorsSvc.h"
///STL includes
#include <set>

///Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentSvc.h"
///Athena includes
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

/** Constructor */
SCT_ByteStreamErrorsSvc::SCT_ByteStreamErrorsSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_sct_id(0),
  m_storeGate("StoreGateSvc",name),
  m_detStore("DetectorStore",name),
  m_cabling("SCT_CablingSvc",name),
  m_config("SCT_ConfigurationConditionsSvc",name),
  m_filled(false) ,
  m_lookForSGErrContainer(true),
  //
  m_timeOutErrors(0),
  m_bcIdErrors(0),
  m_lvl1IdErrors(0),
  m_preambleErrors(0),
  m_formatterErrors(0),
  m_trailerErrors(0),
  m_trailerOverflowErrors(0),
  m_headerTrailerLimitErrors(0),
  m_ABCDErrors(0),
  m_rawErrors(0),
  m_byteStreamParseErrors(0),
  m_maskedLinks(0),
  m_rodClockErrors(0),
  m_truncatedRod(0),
  m_robFragErrors(0),
  m_missingLinkHeaderErrors(0),
  m_rxRedundancy(0),
  //
  m_numTimeOutErrors(0),
  m_numBCIDErrors(0),
  m_numLVL1IDErrors(0),
  m_numPreambleErrors(0),
  m_numFormatterErrors(0),
  m_numTrailerErrors(0),
  m_numABCDErrors(0),
  m_numRawErrors(0),
  m_numDecodingErrors(0),
  m_numMaskedLinks(0),
  m_numRodClockErrors(0),
  m_numTruncatedRod(0),
  m_numRobFragErrors(0),
  m_numMissingLinkHeaderErrors(0),
  m_isRODSimulatedData(false),
  m_numRODsHVon(0),
  m_numRODsTotal(0),
  m_condensedMode(true),
  m_rodFailureFraction(0.1)
{
  declareProperty("EventStore",m_storeGate);
  declareProperty("DetectorStore",m_detStore);
  declareProperty("ContainerName",m_bsErrContainerName="SCT_ByteStreamErrs");
  declareProperty("CablingService",m_cabling);
  declareProperty("ConfigService",m_config);
  declareProperty("useDCSfromBS",m_useDCSfromBS=false);
  declareProperty("UseRXRedundancyInfo",m_useRXredundancy=true);
  declareProperty("disableRODs",m_disableRODs=false);
  declareProperty("RODFailureFraction",m_rodFailureFraction=0.1);
}

/** Initialize */
StatusCode 
SCT_ByteStreamErrorsSvc::initialize(){
  StatusCode sc(StatusCode::SUCCESS);

  m_timeOutErrors = new std::set<IdentifierHash>;
  m_bcIdErrors = new std::set<IdentifierHash>;
  m_lvl1IdErrors = new std::set<IdentifierHash>;
  m_preambleErrors = new std::set<IdentifierHash>;
  m_formatterErrors = new std::set<IdentifierHash>;
  m_trailerErrors = new std::set<IdentifierHash>;
  m_trailerOverflowErrors = new std::set<IdentifierHash>;
  m_headerTrailerLimitErrors = new std::set<IdentifierHash>;
  m_ABCDErrors = new std::set<IdentifierHash>;
  m_rawErrors = new std::set<IdentifierHash>;
  m_byteStreamParseErrors = new std::set<IdentifierHash>;
  m_maskedLinks = new std::set<IdentifierHash>;
  m_rodClockErrors = new std::set<IdentifierHash>;
  m_truncatedRod = new std::set<IdentifierHash>;
  m_robFragErrors = new std::set<IdentifierHash>;
  m_missingLinkHeaderErrors = new std::set<IdentifierHash>;

  m_rxRedundancy = new std::set<IdentifierHash>;

  m_isRODSimulatedData=false;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ) {
    incsvc->addListener( this, "BeginRun", priority);
    incsvc->addListener( this, "BeginEvent", priority);
  }
 
  /** Get a StoreGateSvc */
  if (m_storeGate.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_storeGate << endreq;
    return StatusCode::SUCCESS;
  } 
  else
    msg(MSG::INFO) << "Retrieved service " << m_storeGate << endreq;
  /**  Get a detector store */
  if (m_detStore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_detStore << endreq;
    return StatusCode::SUCCESS;
  }
  else
    msg(MSG::INFO) << "Retrieved service " << m_detStore << endreq ;
 
  sc = m_detStore->retrieve(m_sct_id,"SCT_ID") ;
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve SCT ID helper!"  << endreq;
    return StatusCode::SUCCESS;
  } 

  if (m_useRXredundancy) {
    sc = m_config.retrieve() ;
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve ConfigurationConditionsSvc!"  << endreq;
      return StatusCode::SUCCESS;
    } 
  }
  
  if (m_disableRODs || m_useRXredundancy) {
    sc = m_cabling.retrieve() ;
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve cabling!"  << endreq;
      return StatusCode::SUCCESS;
    } 
  }

  resetCounts();
  return sc;
}

/** Finalize */
StatusCode
SCT_ByteStreamErrorsSvc::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  delete m_timeOutErrors;
  delete m_bcIdErrors;
  delete m_lvl1IdErrors;
  delete m_preambleErrors;
  delete m_formatterErrors;
  delete m_trailerErrors;
  delete m_trailerOverflowErrors;
  delete m_headerTrailerLimitErrors;
  delete m_ABCDErrors;
  delete m_rawErrors;
  delete m_byteStreamParseErrors;
  delete m_maskedLinks;
  delete m_rodClockErrors;
  delete m_truncatedRod;
  delete m_robFragErrors;
  delete m_missingLinkHeaderErrors;
  delete m_rxRedundancy;

  return sc;
}

//////////////////////////////////////////////////////////////////////////////////

/** Clear the sets of IDHashes at the start of every event.
 *  and set the flag to not filled, so that first time the svc
 * is asked for its data, it will fill itself with fillData()  
 */

void
SCT_ByteStreamErrorsSvc::handle(const Incident& inc) {
  if ((inc.type() == "BeginRun") && (m_useRXredundancy)) {
    m_rxRedundancy->clear();
    m_rodDecodeStatuses.clear();

    std::vector<boost::uint32_t> listOfRODs;
    m_cabling->getAllRods(listOfRODs);
    std::vector<boost::uint32_t>::iterator rodIter = listOfRODs.begin();
    std::vector<boost::uint32_t>::iterator rodEnd = listOfRODs.end();
    for (; rodIter != rodEnd; ++rodIter) {
      // Store ROD ID and set all RODs as not decoded
      m_rodDecodeStatuses.insert(std::pair<boost::uint32_t, bool>(*rodIter, false));

      std::vector<IdentifierHash> listOfHashes;
      m_cabling->getHashesForRod(listOfHashes,*rodIter);
      std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
      std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
      for (; hashIt != hashEnd; ++hashIt) {
	Identifier wafId = m_sct_id->wafer_id(*hashIt);
	Identifier modId = m_sct_id->module_id(wafId);
	int side = m_sct_id->side(wafId);

	std::pair<bool, bool> links = m_config->badLinks(modId);
	if (((! links.first) && ( links.second) && (side==1)) ||  
	  ((links.first) && ( ! links.second) && (side==0))) {
	  m_rxRedundancy->insert(*hashIt);
	}
      }
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Number of hashes in redundancy are "<<m_rxRedundancy->size()<<endreq;
    
  } else if (inc.type() == "BeginEvent") {
    this->resetSets();
    m_filled = false;
    m_numRODsHVon=0;
    m_numRODsTotal=0;
    if (m_disableRODs) disableRODs();
    // Set all RODs as not decoded
    for(auto& rodDecodeStatus: m_rodDecodeStatuses) {
      rodDecodeStatus.second = false;
    }
  }
  return;
}

void 
SCT_ByteStreamErrorsSvc::disableRODs() {

  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs);

 /* initialize random seed: */
  srand ( time(NULL) );

  /* generate secret number: */

  for ( int irod = 0; irod < int(m_rodFailureFraction*listOfRODs.size()); ++irod){
    uint32_t RODindex = rand() % (listOfRODs.size());
  
    if (RODindex < listOfRODs.size()) {
      std::vector<IdentifierHash> listOfHashes;
      m_cabling->getHashesForRod(listOfHashes,listOfRODs.at(RODindex));
      std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
      std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
      for (; hashIt != hashEnd; ++hashIt) {
        addError(*hashIt,SCT_ByteStreamErrors::MaskedLink);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////

/** allow the Service to be retrieved via a ServiceHandle. */

StatusCode 
SCT_ByteStreamErrorsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( ISCT_ByteStreamErrorsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    /** Interface is not directly available : try out a base class */
    *ppvInterface =  dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool
SCT_ByteStreamErrorsSvc::isCondensedReadout() {
  return m_condensedMode;
}

void 
SCT_ByteStreamErrorsSvc::setCondensedReadout(bool isCondensed) {
  m_condensedMode = isCondensed;
}

bool
SCT_ByteStreamErrorsSvc::HVisOn() {
  if (m_numRODsTotal > 0 && ( ((float)m_numRODsHVon / (float)m_numRODsTotal) > 0.5 ) )
    return true;
  else
    return false;
}


void 
SCT_ByteStreamErrorsSvc::addRODHVCounter(bool HVisOn) {
  m_numRODsTotal++;
  if (HVisOn) m_numRODsHVon++;
  return;
}


/////////////////////////////////////////////////////////////////////////////

/** Used by ConditionsSummaySvc to decide whether to call isGood() for a particular
 * detector element.
 * Iin principle we could report about modules and/or strips too, and
 * use the id helper to navigate up or down the hierarchy to the wafer,
 * but in practice we don't want to do the time-consuming isGood() for 
 * every strip, so lets only report about wafers..
 */

bool 
SCT_ByteStreamErrorsSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h==InDetConditions::SCT_SIDE); 
}

///////////////////////////////////////////////////////////////////////////

/** this is the principle method which can be accessed via 
 * the ConditionsSummarySvc to decide if a wafer is good - in this
 * case we want to return false if the wafer has an error that would 
 * result in bad hits or no hits for that event */
 
bool 
SCT_ByteStreamErrorsSvc::isGood(const IdentifierHash & elementIdHash) {
  
  if (!m_filled) {
    StatusCode sc = fillData();
    if (sc.isFailure()) {
      msg(MSG::ERROR)<<"Failed to read BS errors from SG container "
		     <<m_bsErrContainerName
		     <<endreq;
      return true;
    }
  }
  
  if (m_useDCSfromBS && (! HVisOn() ) ) return false;

  if (m_isRODSimulatedData) return false;
  
  bool result(true);
  
  result = (std::find(m_timeOutErrors->begin(),
		      m_timeOutErrors->end(),
		      elementIdHash) == m_timeOutErrors->end());
  if (!result) return result;
  result = (std::find(m_bcIdErrors->begin(),
		      m_bcIdErrors->end(),
		      elementIdHash) == m_bcIdErrors->end());
  if (!result) return result;
  result = (std::find(m_lvl1IdErrors->begin(),
		      m_lvl1IdErrors->end(),
		      elementIdHash) == m_lvl1IdErrors->end());
  if (!result) return result;
  result = (std::find(m_maskedLinks->begin(),
		      m_maskedLinks->end(),
		      elementIdHash) == m_maskedLinks->end());
  if (!result) return result;

  result = (std::find(m_robFragErrors->begin(),
		      m_robFragErrors->end(),
		      elementIdHash) == m_robFragErrors->end());
  if (!result) return result;

  result = (std::find(m_missingLinkHeaderErrors->begin(),
		      m_missingLinkHeaderErrors->end(),
		      elementIdHash) == m_missingLinkHeaderErrors->end());
  if (!result) return result;
  
  return result;
}

bool 
SCT_ByteStreamErrorsSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h){
  
  if (m_isRODSimulatedData) return false;
  if (h==InDetConditions::SCT_SIDE) {
    const IdentifierHash elementIdHash = m_sct_id->wafer_hash(elementId);
    return isGood(elementIdHash);
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////

/** reset everything at the start of every event. */

void 
SCT_ByteStreamErrorsSvc::resetSets() {
  m_timeOutErrors->clear();
  m_bcIdErrors->clear();
  m_lvl1IdErrors->clear();
  m_preambleErrors->clear();
  m_formatterErrors->clear();
  m_trailerErrors->clear();
  m_trailerOverflowErrors->clear();
  m_headerTrailerLimitErrors->clear();
  m_ABCDErrors->clear();
  m_rawErrors->clear();
  m_byteStreamParseErrors->clear();
  m_maskedLinks->clear();
  m_rodClockErrors->clear();
  m_truncatedRod->clear();
  m_robFragErrors->clear();
  m_missingLinkHeaderErrors->clear();
  return;
}

////////////////////////////////////////////////////////////////////////////////////////

/** The accessor method that can be used by clients to 
 * retrieve a set of IdHashes of wafers with a given type of error.
 * e.g. for monitoring plots.
 */

std::set<IdentifierHash>* 
SCT_ByteStreamErrorsSvc::getErrorSet(int errType) {

  if (!m_filled) {
    StatusCode sc = fillData();
    if (sc.isFailure()) msg(MSG::ERROR) << "Failed to fill from SG container " 
					<<m_bsErrContainerName
					<<endreq;
  }
  switch(errType) {
  case SCT_ByteStreamErrors::ByteStreamParseError:
    return m_byteStreamParseErrors;
  case SCT_ByteStreamErrors::TimeOutError:
    return m_timeOutErrors;
  case SCT_ByteStreamErrors::BCIDError:
    return m_bcIdErrors;   
  case SCT_ByteStreamErrors::LVL1IDError:
    return m_lvl1IdErrors;   
  case SCT_ByteStreamErrors::PreambleError:
    return m_preambleErrors;   
  case SCT_ByteStreamErrors::FormatterError:
    return m_formatterErrors;   
  case SCT_ByteStreamErrors::TrailerError:
    return m_trailerErrors;   
  case SCT_ByteStreamErrors::TrailerOverflowError:
    return m_trailerOverflowErrors;   
  case SCT_ByteStreamErrors::HeaderTrailerLimitError:
    return m_headerTrailerLimitErrors;   
  case SCT_ByteStreamErrors::ABCDError:
    return m_ABCDErrors;   
  case SCT_ByteStreamErrors::RawError:
    return m_rawErrors;   
  case SCT_ByteStreamErrors::MaskedLink:
    return m_maskedLinks;
  case SCT_ByteStreamErrors::RODClockError:
    return m_rodClockErrors;
  case SCT_ByteStreamErrors::TruncatedROD:
    return m_truncatedRod;
  case SCT_ByteStreamErrors::ROBFragmentError:
    return m_robFragErrors;
  case SCT_ByteStreamErrors::MissingLinkHeaderError:
    return m_missingLinkHeaderErrors;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////

/** this function is used to populate the data of this service from 
 * the InDetBSErrContainer in StoreGate, loops through container,
 * calls addError() for each entry.
 */

StatusCode
SCT_ByteStreamErrorsSvc::fillData() {

  if (m_lookForSGErrContainer) {
    const InDetBSErrContainer* errCont;

    /** When running over ESD files without BSErr container stored, don't 
     * want to flood the user with error messages.  Should just have a bunch
     * of empty sets, and keep quiet.
     */
    bool gotErrors = m_storeGate->contains<InDetBSErrContainer>(m_bsErrContainerName);
    if (gotErrors) {
      StatusCode sc = m_storeGate->retrieve(errCont,m_bsErrContainerName);
      if (sc.isFailure() ) 
	msg(MSG::ERROR) <<"Failed to get BSError container from SG "
			<<"even though SG::contains() says it's there.. strange."<<endreq;
    } else {
      msg(MSG::INFO) <<"Failed to retrieve BS error container "
		     << m_bsErrContainerName
		     <<" from StoreGate.  "
		     <<"This is expected if you are reading an ESD file "
		     <<"which doesn't have the ByteStreamErrors container stored, "
		     <<"such as an MC ESD file, or a real data ESD file produced "
		     <<"with a release older than 14.5.0.  "
		     <<"Otherwise, you might have a problem.  "
		     <<"This message won't be printed again."
		     <<endreq;
      
      m_lookForSGErrContainer = false;
      m_filled = true;
      return StatusCode::SUCCESS;
    }

    /** OK, so we found the StoreGate container, now lets iterate
     * over it to populate the sets of errors owned by this Svc.
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"size of error container is "<<errCont->size()<<endreq;
    std::vector<std::pair<IdentifierHash, int>* >::const_iterator it=errCont->begin();
    std::vector<std::pair<IdentifierHash, int>* >::const_iterator itEnd=errCont->end();
    for (; it != itEnd; ++it) {
      addError((*it)->first,(*it)->second);
      if (m_useRXredundancy) {
	bool result = std::find(m_rxRedundancy->begin(),
				m_rxRedundancy->end(),
				(*it)->first) != m_rxRedundancy->end();
	if (result) {
	  /// error in a module using RX redundancy - add an error for the other link as well!!
	  int side = m_sct_id->side(m_sct_id->wafer_id((*it)->first));
	  if (side==0) {
	    IdentifierHash otherSide = IdentifierHash((*it)->first  + 1);
	    addError(otherSide,(*it)->second);
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Adding error to side 1 for module with RX redundancy"<<otherSide<<endreq;
	  } else if (side==1) {
	    IdentifierHash otherSide = IdentifierHash((*it)->first  - 1);
	    addError(otherSide,(*it)->second);
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Adding error to side 0 for module with RX redundancy"<<otherSide<<endreq;
	  }
	}
      }
    }
    m_filled=true;
    return StatusCode::SUCCESS;
  } else {
    /** We presumably tried looking for the container before and failed,
     * so lets just return happily to avoid spamming the user with error
     * messages.
     */
    m_filled=true;
    return StatusCode::SUCCESS;
  }
}

bool
SCT_ByteStreamErrorsSvc::filled() const{
  return m_filled;
}

/** The following method is used to populate 
 *  the sets of IdHashes for wafers with errors. 
 *  It is called by the fillData() method, which reads the 
 *  InDetBSErrContainer from StoreGate.
*/

void 
SCT_ByteStreamErrorsSvc::addError(IdentifierHash& id, int errorType) {
  switch(errorType) {
  case SCT_ByteStreamErrors::ByteStreamParseError:
    m_byteStreamParseErrors->insert(id);
    break;
  case SCT_ByteStreamErrors::TimeOutError:
    m_timeOutErrors->insert(id);
    break;
  case SCT_ByteStreamErrors::BCIDError:
    m_bcIdErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::LVL1IDError:
    m_lvl1IdErrors->insert(id);
    break;
  case SCT_ByteStreamErrors::PreambleError:
    m_preambleErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::FormatterError:
    m_formatterErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::TrailerError:
    m_trailerErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::TrailerOverflowError:
    m_trailerOverflowErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::HeaderTrailerLimitError:
    m_headerTrailerLimitErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::ABCDError:
    m_ABCDErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::RawError:
    m_rawErrors->insert(id);   
    break;
  case SCT_ByteStreamErrors::MaskedLink:
    m_maskedLinks->insert(id);
    break;
  case SCT_ByteStreamErrors::RODClockError:
    m_rodClockErrors->insert(id);
    break;
  case SCT_ByteStreamErrors::TruncatedROD:
    m_truncatedRod->insert(id);
    break;
  case SCT_ByteStreamErrors::ROBFragmentError:
    m_robFragErrors->insert(id);
    break;
  case SCT_ByteStreamErrors::MissingLinkHeaderError:
    m_missingLinkHeaderErrors->insert(id);
    break;
  }    
}

/** The HLT also wants to know how many errors there 
 * were in a given RoI.. unfortunately this means we
 * can't use the same model as for offline where the 
 * RodDecoder writes to a StoreGate container which is 
 * read in once per event by this Svc.. instead, have 
 * the RodDecoder call the following method as it goes 
 * along.  The counts can be reset by any client using
 * the resetCounts() method.
 */

void 
SCT_ByteStreamErrorsSvc::addErrorCount(int errorType) {
  switch(errorType) {
  case SCT_ByteStreamErrors::ByteStreamParseError:
    m_numDecodingErrors++;
    break;
  case SCT_ByteStreamErrors::TimeOutError:
    m_numTimeOutErrors++;
    break;
  case SCT_ByteStreamErrors::BCIDError:
    m_numBCIDErrors++;
    break;
  case SCT_ByteStreamErrors::LVL1IDError:
    m_numLVL1IDErrors++;
    break;
  case SCT_ByteStreamErrors::PreambleError:
    m_numPreambleErrors++;
    break;
  case SCT_ByteStreamErrors::FormatterError:
    m_numFormatterErrors++;
    break;
  case SCT_ByteStreamErrors::TrailerError:
    m_numTrailerErrors++;
    break;
  case SCT_ByteStreamErrors::TrailerOverflowError:
    m_numTrailerErrors++;
    break;
  case SCT_ByteStreamErrors::HeaderTrailerLimitError:
    m_numTrailerErrors++;
    break;
  case SCT_ByteStreamErrors::ABCDError:
    m_numABCDErrors++;
    break;
  case SCT_ByteStreamErrors::RawError:
    m_numRawErrors++;
    break;
  case SCT_ByteStreamErrors::MaskedLink:
    m_numMaskedLinks++;
    break;
  case SCT_ByteStreamErrors::RODClockError:
    m_numRodClockErrors++;
    break;
  case SCT_ByteStreamErrors::TruncatedROD:
    m_numTruncatedRod++;
    break;
  case SCT_ByteStreamErrors::ROBFragmentError:
    m_numRobFragErrors++;
    break;
  case SCT_ByteStreamErrors::MissingLinkHeaderError:
    m_numMissingLinkHeaderErrors++;
    break;
  }    
}


///////////////////////////////////////////////////////////////////////////////

/** A bit is set in a particular word in the ByteStream if the data
 * is coming from the ROD simulator rather than real modules. */
void
SCT_ByteStreamErrorsSvc::setRODSimulatedData() {
  m_isRODSimulatedData=true;
}
bool
SCT_ByteStreamErrorsSvc::isRODSimulatedData() {
  return m_isRODSimulatedData;
}

///////////////////////////////////////////////////////////////////////////////

/** The following methods are for HLT, to get the numbers of errors 
 *  per RoI, so won't necessarily match up with the sizes of the sets. 
 */

void 
SCT_ByteStreamErrorsSvc::resetCounts() {
  m_numDecodingErrors=0;
  m_numTimeOutErrors=0;
  m_numBCIDErrors=0;
  m_numLVL1IDErrors=0;
  m_numPreambleErrors=0;
  m_numTrailerErrors=0;
  m_numABCDErrors=0;
  m_numRawErrors=0;
  m_numMaskedLinks=0;
  m_numRodClockErrors=0;
  m_numTruncatedRod=0;
  m_numRobFragErrors=0;
  m_numMissingLinkHeaderErrors=0;
}

int 
SCT_ByteStreamErrorsSvc::getNumberOfErrors(int errorType) {

  switch(errorType)
    {
    case SCT_ByteStreamErrors::ByteStreamParseError:
      return m_numDecodingErrors;
    case SCT_ByteStreamErrors::TimeOutError:
      return m_numTimeOutErrors;
    case SCT_ByteStreamErrors::BCIDError:
      return m_numBCIDErrors;
    case SCT_ByteStreamErrors::LVL1IDError:
      return m_numLVL1IDErrors;
    case SCT_ByteStreamErrors::PreambleError:
      return m_numPreambleErrors;
    case SCT_ByteStreamErrors::TrailerError:
      return m_numTrailerErrors;
    case SCT_ByteStreamErrors::ABCDError:
      return m_numABCDErrors;
    case SCT_ByteStreamErrors::RawError:
      return m_numRawErrors;
    case SCT_ByteStreamErrors::MaskedLink:
      return m_numMaskedLinks;
    case SCT_ByteStreamErrors::RODClockError:
      return m_numRodClockErrors;
    case SCT_ByteStreamErrors::TruncatedROD:
      return m_numTruncatedRod;
    case SCT_ByteStreamErrors::ROBFragmentError:
      return m_numRobFragErrors;
    case SCT_ByteStreamErrors::MissingLinkHeaderError:
      return m_numMissingLinkHeaderErrors;
    }
  return 0;
}

void
SCT_ByteStreamErrorsSvc::setDecodedROD(const boost::uint32_t rodId) {
  // If the rodId is found, set the ROD as decoded.
  auto rodDecodeStatus = m_rodDecodeStatuses.find(rodId);
  if(rodDecodeStatus!=m_rodDecodeStatuses.end()) {
    rodDecodeStatus->second = true;
  }
}

std::vector<boost::uint32_t>
SCT_ByteStreamErrorsSvc::getRODOuts() const {
  std::vector<boost::uint32_t> rodOuts;
  // Create a vector of undecoded RODs as ROD outs
  for(auto& rodDecodeStatus: m_rodDecodeStatuses) {
    if(not rodDecodeStatus.second) {
      rodOuts.push_back(rodDecodeStatus.first);
    }
  }
  return rodOuts;
}

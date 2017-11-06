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
#include <map>

///Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IIncidentSvc.h"
///Athena includes
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_ConditionsServices/ISCT_ConfigurationConditionsSvc.h"

///Read Handle
#include "StoreGate/ReadHandle.h"

/** Constructor */
SCT_ByteStreamErrorsSvc::SCT_ByteStreamErrorsSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator), 
  m_sct_id(0),
  m_storeGate("StoreGateSvc",name),
  m_detStore("DetectorStore",name),
  m_cabling("SCT_CablingSvc",name),
  m_config("InDetSCT_ConfigurationConditionsSvc",name),
  m_filled(false) ,
  m_lookForSGErrContainer(true),
  //
  m_rxRedundancy(0),
  m_tempMaskedChips(nullptr),
  m_isRODSimulatedData(false),
  m_numRODsHVon(0),
  m_numRODsTotal(0),
  m_condensedMode(true),
  m_rodFailureFraction(0.1)
{
  declareProperty("EventStore",m_storeGate);
  declareProperty("DetectorStore",m_detStore);
  declareProperty("ContainerName",m_bsErrContainerName=std::string("SCT_ByteStreamErrs"));
  declareProperty("CablingService",m_cabling);
  declareProperty("ConfigService",m_config);
  declareProperty("useDCSfromBS",m_useDCSfromBS=false);
  declareProperty("UseRXRedundancyInfo",m_useRXredundancy=true);
  declareProperty("disableRODs",m_disableRODs=false);
  declareProperty("RODFailureFraction",m_rodFailureFraction=0.1);
  declareProperty("RandomNumberSeed",m_randomSeed=1); // The seed of random numbers for ROD disabling

  for(int errorType=0; errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES; errorType++) {
    m_bsErrors[errorType] = 0;
    m_numBsErrors[errorType] = 0;
  }
}

/** Initialize */
StatusCode 
SCT_ByteStreamErrorsSvc::initialize(){
  StatusCode sc(StatusCode::SUCCESS);

  for(int errorType=0; errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES; errorType++) {
    m_bsErrors[errorType] = new std::set<IdentifierHash>;
  }

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
    msg(MSG::FATAL) << "Failed to retrieve service " << m_storeGate << endmsg;
    return StatusCode::SUCCESS;
  } 
  else
    msg(MSG::INFO) << "Retrieved service " << m_storeGate << endmsg;
  /**  Get a detector store */
  if (m_detStore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "Failed to retrieve service " << m_detStore << endmsg;
    return StatusCode::SUCCESS;
  }
  else
    msg(MSG::INFO) << "Retrieved service " << m_detStore << endmsg ;
 
  sc = m_detStore->retrieve(m_sct_id,"SCT_ID") ;
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve SCT ID helper!"  << endmsg;
    return StatusCode::SUCCESS;
  } 

  m_cntx_sct = m_sct_id->wafer_context();

  sc = m_config.retrieve() ;
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Cannot retrieve ConfigurationConditionsSvc!"  << endmsg;
    return StatusCode::SUCCESS;
  } 
  
  if (m_disableRODs || m_useRXredundancy) {
    sc = m_cabling.retrieve() ;
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Cannot retrieve cabling!"  << endmsg;
      return StatusCode::SUCCESS;
    } 
  }

  m_tempMaskedChips = new std::map<Identifier, unsigned int>;

  // Read Handle Key
  ATH_CHECK(m_bsErrContainerName.initialize());

  resetCounts();
  return sc;
}

/** Finalize */
StatusCode
SCT_ByteStreamErrorsSvc::finalize(){
  StatusCode sc(StatusCode::SUCCESS);
  for(int errType=0; errType<SCT_ByteStreamErrors::NUM_ERROR_TYPES; errType++) {
    delete m_bsErrors[errType];
    m_bsErrors[errType] = 0;
  }
  delete m_rxRedundancy;
  m_rxRedundancy = 0;

  delete m_tempMaskedChips;
  m_tempMaskedChips = nullptr;

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
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Number of hashes in redundancy are "<<m_rxRedundancy->size()<<endmsg;
    
  } else if (inc.type() == "BeginEvent") {
    this->resetSets();
    this->resetCounts();
    m_filled = false;
    m_numRODsHVon=0;
    m_numRODsTotal=0;
    if (m_disableRODs) disableRODs();
    // Set all RODs as not decoded
    for(auto& rodDecodeStatus: m_rodDecodeStatuses) {
      rodDecodeStatus.second = false;
    }
    m_firstTempMaskedChips.clear();
    m_tempMaskedChips->clear();
  }
  return;
}

void 
SCT_ByteStreamErrorsSvc::disableRODs() {

  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs);

 /* initialize random seed: */
  srand ( m_randomSeed );

  /* generate secret number: */

  for ( int irod = 0; irod < int(m_rodFailureFraction*listOfRODs.size()); ++irod){
    uint32_t RODindex = rand() % (listOfRODs.size());
  
    if (RODindex < listOfRODs.size()) {
      std::vector<IdentifierHash> listOfHashes;
      m_cabling->getHashesForRod(listOfHashes,listOfRODs.at(RODindex));
      std::vector<IdentifierHash>::iterator hashIt = listOfHashes.begin();
      std::vector<IdentifierHash>::iterator hashEnd = listOfHashes.end();
      for (; hashIt != hashEnd; ++hashIt) {
        addError(*hashIt,SCT_ByteStreamErrors::MaskedROD);
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
		     <<m_bsErrContainerName.key()
		     <<endmsg;
      return true;
    }
  }
  
  if (m_useDCSfromBS && (! HVisOn() ) ) return false;

  if (m_isRODSimulatedData) return false;
  
  bool result(true);
  
  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::TimeOutError]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::TimeOutError]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::TimeOutError]->end());
  if (!result) return result;
  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::BCIDError]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::BCIDError]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::BCIDError]->end());
  if (!result) return result;
  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::LVL1IDError]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::LVL1IDError]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::LVL1IDError]->end());
  if (!result) return result;
  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::MaskedLink]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::MaskedLink]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::MaskedLink]->end());
  if (!result) return result;

  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::ROBFragmentError]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::ROBFragmentError]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::ROBFragmentError]->end());
  if (!result) return result;

  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::MissingLinkHeaderError]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::MissingLinkHeaderError]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::MissingLinkHeaderError]->end());
  if (!result) return result;

  result = (std::find(m_bsErrors[SCT_ByteStreamErrors::MaskedROD]->begin(),
		      m_bsErrors[SCT_ByteStreamErrors::MaskedROD]->end(),
		      elementIdHash) == m_bsErrors[SCT_ByteStreamErrors::MaskedROD]->end());
  if (!result) return result;

  // If all 6 chips of a link issue ABCD errors or are bad chips or temporarily masked chips, the link is treated as bad one. 
  const Identifier wafer_id(m_sct_id->wafer_id(elementIdHash));
  const Identifier module_id(m_sct_id->module_id(wafer_id));
  unsigned int badChips(m_config->badChips(module_id));
  unsigned int tempMaskedChips2(tempMaskedChips(module_id));
  const int side(m_sct_id->side(wafer_id));
  short id(side==0 ? 0 : 6);
  bool allChipsBad(true);
  for(int errType=SCT_ByteStreamErrors::ABCDError_Chip0; (errType<=SCT_ByteStreamErrors::ABCDError_Chip5) and allChipsBad; errType++) {
    bool issueABCDError = (std::find(m_bsErrors[errType]->begin(), m_bsErrors[errType]->end(), elementIdHash)!=m_bsErrors[errType]->end());
    bool isBadChip = ((badChips >> (id)) & 0x1);
    bool isTempMaskedChip = ((tempMaskedChips2 >> (id)) & 0x1);
    id++;
    allChipsBad = (issueABCDError or isBadChip or isTempMaskedChip);
    if(not allChipsBad) break;
  }
  if(allChipsBad) return false;
  
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

  for(int errType=0; errType<SCT_ByteStreamErrors::NUM_ERROR_TYPES; errType++) {
    m_bsErrors[errType]->clear();
  }

  return;
}

////////////////////////////////////////////////////////////////////////////////////////

/** The accessor method that can be used by clients to 
 * retrieve a set of IdHashes of wafers with a given type of error.
 * e.g. for monitoring plots.
 */

std::set<IdentifierHash>* 
SCT_ByteStreamErrorsSvc::getErrorSet(int errorType) {

  if (!m_filled) {
    StatusCode sc = fillData();
    if (sc.isFailure()) msg(MSG::ERROR) << "Failed to fill from SG container " 
					<<m_bsErrContainerName.key()
					<<endmsg;
  }
  if(errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    return m_bsErrors[errorType];
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
    SG::ReadHandle<InDetBSErrContainer> errCont(m_bsErrContainerName);

    /** When running over ESD files without BSErr container stored, don't 
     * want to flood the user with error messages.  Should just have a bunch
     * of empty sets, and keep quiet.
     */
    bool gotErrors = m_storeGate->contains<InDetBSErrContainer>(m_bsErrContainerName.key());
    if ((not gotErrors) or (not errCont.isValid())) {
      msg(MSG::INFO) <<"Failed to retrieve BS error container "
		     << m_bsErrContainerName.key()
		     <<" from StoreGate.  "
		     <<"This is expected if you are reading an ESD file "
		     <<"which doesn't have the ByteStreamErrors container stored, "
		     <<"such as an MC ESD file, or a real data ESD file produced "
		     <<"with a release older than 14.5.0.  "
		     <<"Otherwise, you might have a problem.  "
		     <<"This message won't be printed again."
		     <<endmsg;
      
      m_lookForSGErrContainer = false;
      m_filled = true;
      return StatusCode::SUCCESS;
    }

    /** OK, so we found the StoreGate container, now lets iterate
     * over it to populate the sets of errors owned by this Svc.
     */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"size of error container is "<<errCont->size()<<endmsg;
    for (const auto* elt : *errCont) {
      addError(elt->first,elt->second);
      if (m_useRXredundancy) {
	bool result = std::find(m_rxRedundancy->begin(),
				m_rxRedundancy->end(),
				elt->first) != m_rxRedundancy->end();
	if (result) {
	  /// error in a module using RX redundancy - add an error for the other link as well!!
	  int side = m_sct_id->side(m_sct_id->wafer_id(elt->first));
	  if (side==0) {
	    IdentifierHash otherSide = IdentifierHash(elt->first  + 1);
	    addError(otherSide,elt->second);
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Adding error to side 1 for module with RX redundancy"<<otherSide<<endmsg;
	  } else if (side==1) {
	    IdentifierHash otherSide = IdentifierHash(elt->first  - 1);
	    addError(otherSide,elt->second);
	    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<<"Adding error to side 0 for module with RX redundancy"<<otherSide<<endmsg;
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
SCT_ByteStreamErrorsSvc::addError(IdentifierHash id, int errorType) {
  if(errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    m_bsErrors[errorType]->insert(id);
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
  if(errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    m_numBsErrors[errorType]++;
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
  for(int errorType=0; errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES; errorType++) {
    m_numBsErrors[errorType] = 0;
  }
}

int 
SCT_ByteStreamErrorsSvc::getNumberOfErrors(int errorType) {
  if(errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    return m_numBsErrors[errorType];
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

void SCT_ByteStreamErrorsSvc::setFirstTempMaskedChip(const IdentifierHash& hashId, const unsigned int firstTempMaskedChip) {
  if(not hashId.is_valid()) {
    ATH_MSG_INFO("setFirstTempMaskedChip hashId " << hashId << " is invalid.");
    return;
  }
  if(firstTempMaskedChip==0) {
    ATH_MSG_WARNING("setFirstTempMaskedChip: firstTempMaskedChip should be greater than 0. firstTempMaskedChip is " << firstTempMaskedChip);
    return;
  }

  //// 1. set m_firstTempMaskedChips for this wafer
  std::pair<std::map<IdentifierHash, unsigned int>::const_iterator, bool>
    ret(m_firstTempMaskedChips.insert(std::make_pair(hashId, firstTempMaskedChip)));
  if(not ret.second) {
    ATH_MSG_WARNING("setFirstTempMaskedChip: already set for hashId " << hashId << 
		    " firstTempMaskedChip is " << ret.first->second << 
		    " and you are trying to put " << firstTempMaskedChip);
    return;
  }
  
  //// 2. set m_tempMaskedChips for this module
  
  // wafer hash -> wafer id -> module id -> wafer hash on side-0, wafer hash on side-1
  const Identifier wafId(m_sct_id->wafer_id(hashId));
  const Identifier moduleId(m_sct_id->module_id(wafId));
  
  // Side 0
  IdentifierHash hash_side0;
  m_sct_id->get_hash(moduleId, hash_side0, &m_cntx_sct);
  int firstTempMaskedChip_side0(getFirstTempMaskedChip(hash_side0));
  
  // Side 1
  IdentifierHash hash_side1;
  m_sct_id->get_other_side(hash_side0, hash_side1);
  int firstTempMaskedChip_side1(getFirstTempMaskedChip(hash_side1));
  
  // There should be at least one masked chip
  if(firstTempMaskedChip_side0==0 and firstTempMaskedChip_side1==0) {
    ATH_MSG_WARNING("setFirstTempMaskedChip: There should be at least one masked chip. Something is wrong.");
    return;
  }
  
  int type(0);
  // Check if Rx redundancy is used or not in this module
  if(m_rxRedundancy->find(hash_side0)!=m_rxRedundancy->end() or 
     m_rxRedundancy->find(hash_side1)!=m_rxRedundancy->end()) {
    // Rx redundancy is used in this module.
    std::pair<bool, bool> links(m_config->badLinks(moduleId));
    if(links.first and not links.second) {
      // link-1 is broken
      type = 1;
    } else if(links.second and not links.first) {
      // link-0 is broken
      type = 2;
    } else if(links.first and links.second) {
      // both link-0 and link-1 are working
      ATH_MSG_WARNING("setFirstTempMaskedChip: Both link-0 and link-1 are working. But Rx redundancy is used... Why?");
      return;
    } else {
      // both link-0 and link-1 are broken
      ATH_MSG_WARNING("setFirstTempMaskedChip: Both link-0 and link-1 are broken. But data are coming... Why?");
      return;
    }
  }

  if(type==1 or type==2) {
    if(firstTempMaskedChip_side0>0 and
       firstTempMaskedChip_side1>0 and
       firstTempMaskedChip_side0!=firstTempMaskedChip_side1) {
      ATH_MSG_WARNING("setFirstTempMaskedChip: Rx redundancy is used. " <<
		      "But, side 0 and side 1 have inconsistent first masked chip information. " <<
		      " firstTempMaskedChip_side0 " << firstTempMaskedChip_side0 <<
		      " firstTempMaskedChip_side1 " << firstTempMaskedChip_side1 <<
		      " firstTempMaskedChip " << firstTempMaskedChip);
    }
  }

  // "Modified" module (using Rx redundancy) case
  unsigned long long fullSerialNumber(m_cabling->getSerialNumberFromHash(hashId).to_ulonglong());
  if(// Readout through link-0
     fullSerialNumber==20220170200183 or // hash=4662 bec=0 layer=2 eta= 6 phi=39
     fullSerialNumber==20220330200606 or // hash=5032 bec=0 layer=3 eta=-2 phi= 7
     fullSerialNumber==20220330200693    // hash=5554 bec=0 layer=3 eta=-5 phi=29
     ) {
    if(type!=1) ATH_MSG_WARNING("Link-0 is broken but modified module readingout link-0, inconsistent");
    type = 3;
  }
  if(// Readout through link-1
     fullSerialNumber==20220170200653 or // hash=2786 bec=0 layer=1 eta= 4 phi= 1
     fullSerialNumber==20220330200117 or // hash=5516 bec=0 layer=3 eta= 1 phi=27
     fullSerialNumber==20220330200209 or // hash=5062 bec=0 layer=3 eta= 2 phi= 8
     fullSerialNumber==20220330200505 or // hash=5260 bec=0 layer=3 eta= 5 phi=16
     fullSerialNumber==20220330200637 or // hash=4184 bec=0 layer=2 eta=-6 phi=20
     fullSerialNumber==20220330200701    // hash=4136 bec=0 layer=2 eta=-6 phi=18
     ) {
    if(type!=2) ATH_MSG_WARNING("Link-1 is broken but modified module readingout link-1, inconsistent");
    type = 4;
  }

  static const int chipOrder[5][12] = {
    // type=0 not prepared for both link-0 and link-1 are working
    {},
    // type=1 link-1 is broken: chip 0 1 2 3 4 5 6 7 8 9 10 11
    {0, 1, 2, 3,  4,  5, 6, 7, 8,  9, 10, 11},
    // type=2 link-0 is broken: chip 6 7 8 9 10 11 0 1 2 3 4 5
    {6, 7, 8, 9, 10, 11, 0, 1, 2,  3,  4,  5},
    // type=3 "modified" modules and link-1 is broken: chip 0 1 2 3 4 5 7 8 9 10 11 6
    {0, 1, 2, 3,  4,  5, 7, 8, 9, 10, 11,  6},
    // type=4 "modified" modules and link-0 is broken: chip 6 7 8 9 10 11 1 2 3 4 5 0
    {6, 7, 8, 9, 10, 11, 1, 2, 3,  4,  5,  0}
  };

  unsigned int tempMaskedChips2(0);
  if(type==0) {
    // both link-0 and link-1 are working

    // Chips 0-5 are on side 0 and chips 6-11 are on side 1.
    // Normally, modules send hits on side 0 via link-0 and side 1 via link-1. 
    // The first temporally masked chip value is the id of the chip that is 
    // first masked in the readout chain "plus one". 
    // If the value is in between 1 to  6, it indicates side 0.
    // If the value is in between 7 to 12, it indicates side 1.
    // However, some special modules send hits on side 0 via link-1 and hits on
    // side 1 via link-0. If the first masked chip value on side 1 (0) is 
    // between 1 to 6 (7 to 12), it indicates the module is a special one.  
    // In that case, information is swapped.
    if((6<firstTempMaskedChip_side0 and firstTempMaskedChip_side0<=12) or
       (0<firstTempMaskedChip_side1 and firstTempMaskedChip_side1<= 6)) {
      int swapFirstTempMaskedChip_side0(firstTempMaskedChip_side0);
      firstTempMaskedChip_side0 = firstTempMaskedChip_side1;
      firstTempMaskedChip_side1 = swapFirstTempMaskedChip_side0;
    }

    if(firstTempMaskedChip_side0>0) {
      for(int iChip(firstTempMaskedChip_side0-1); iChip<6; iChip++) {
	tempMaskedChips2 |= (1<<iChip);
	addError(hash_side0, SCT_ByteStreamErrors::TempMaskedChip0+iChip);
      }
    }
    if(firstTempMaskedChip_side1>6) {
      for(int iChip(firstTempMaskedChip_side1-1); iChip<12; iChip++) {
	tempMaskedChips2 |= (1<<iChip);
	addError(hash_side1, SCT_ByteStreamErrors::TempMaskedChip0+iChip-6);
      }
    }
  } else {
    // type=1, 2, 3, 4: cases using Rx redundancy
    if(firstTempMaskedChip>0) {
      bool toBeMasked(false);
      for(int iChip(0); iChip<12; iChip++) {
	int jChip(chipOrder[type][iChip]);
	if(jChip==static_cast<int>(firstTempMaskedChip-1)) toBeMasked = true;
	if(toBeMasked) {
	  tempMaskedChips2 |= (1<<jChip);
	  if(jChip<6) addError(hash_side0, SCT_ByteStreamErrors::TempMaskedChip0+jChip);
	  else        addError(hash_side1, SCT_ByteStreamErrors::TempMaskedChip0+jChip-6);
	}
      }
    }
  }
  
  ATH_MSG_VERBOSE("setFirstTempMaskedChip Hash " << hashId 
		  << " SerialNumber " << m_cabling->getSerialNumberFromHash(hashId).str() 
		  << " moduleId " << moduleId
		  << " barrel_ec " << m_sct_id->barrel_ec(wafId) 
		  << " layer_disk " << m_sct_id->layer_disk(wafId) 
		  << " eta_module " << m_sct_id->eta_module(wafId) 
		  << " phi_module " << m_sct_id->phi_module(wafId) 
		  << " side " << m_sct_id->side(wafId) 
		  << " firstTempMaskedChip " << firstTempMaskedChip
		  << " tempMaskedChips2 " << tempMaskedChips2);

  (*m_tempMaskedChips)[moduleId] = tempMaskedChips2;
}

unsigned int SCT_ByteStreamErrorsSvc::getFirstTempMaskedChip(const IdentifierHash& hashId) const {
  std::map<IdentifierHash, unsigned int>::const_iterator it(m_firstTempMaskedChips.find(hashId));
  if(it!=m_firstTempMaskedChips.end()) return it->second;
  return 0;
}

unsigned int SCT_ByteStreamErrorsSvc::tempMaskedChips(const Identifier & moduleId) const {
  std::map<Identifier, unsigned int>::const_iterator it(m_tempMaskedChips->find(moduleId));
  if(it!=m_tempMaskedChips->end()) return it->second;
  return 0;
}

unsigned int SCT_ByteStreamErrorsSvc::abcdErrorChips(const Identifier & moduleId) const {
  unsigned int abcdErrorChips2(0);
  int chip(0);

  // Side 0
  IdentifierHash hash_side0;
  m_sct_id->get_hash(moduleId, hash_side0, &m_cntx_sct);
  if(std::find(m_bsErrors[SCT_ByteStreamErrors::ABCDError]->begin(),
	       m_bsErrors[SCT_ByteStreamErrors::ABCDError]->end(),
	       hash_side0)
     !=m_bsErrors[SCT_ByteStreamErrors::ABCDError]->end()) {
    for(int errType=SCT_ByteStreamErrors::ABCDError_Chip0; errType<=SCT_ByteStreamErrors::ABCDError_Chip5; errType++) {
      if(std::find(m_bsErrors[errType]->begin(), m_bsErrors[errType]->end(), hash_side0)!=m_bsErrors[errType]->end()) {
	abcdErrorChips2 |= (1 << chip);
      }
      chip++;
    }
  } else {
    chip = 6;
  }

  // Side 1
  IdentifierHash hash_side1;
  m_sct_id->get_other_side(hash_side0, hash_side1);
  if(std::find(m_bsErrors[SCT_ByteStreamErrors::ABCDError]->begin(),
               m_bsErrors[SCT_ByteStreamErrors::ABCDError]->end(),
               hash_side1)
     !=m_bsErrors[SCT_ByteStreamErrors::ABCDError]->end()) {
    for(int errType=SCT_ByteStreamErrors::ABCDError_Chip0; errType<=SCT_ByteStreamErrors::ABCDError_Chip5; errType++) {
      if(std::find(m_bsErrors[errType]->begin(), m_bsErrors[errType]->end(), hash_side1)!=m_bsErrors[errType]->end()) {
	abcdErrorChips2 |= (1 << chip);
      }
      chip++;
    }
  }

  return abcdErrorChips2;
}

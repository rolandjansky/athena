/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConfigurationConditionsSvc.h"

// STL includes
#include <vector>
#include <algorithm>
// Gaudi includes
#include "GaudiKernel/StatusCode.h"

// Athena includes
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

//#include "AthenaKernel/IIOVDbSvc.h"

// Local includes
#include "SCT_ReadoutTool.h"
#include "SCT_Chip.h"
#include "SCT_Cabling/SCT_SerialNumber.h"

// Static folder names 
static const std::string coolChannelFolderName("/SCT/DAQ/Configuration/Chip");
static const std::string coolModuleFolderName("/SCT/DAQ/Configuration/Module");
static const std::string coolMurFolderName("/SCT/DAQ/Configuration/MUR");

//Run2: folders change name in CONDBR2 database
static const std::string coolChannelFolderName2("/SCT/DAQ/Config/Chip");
static const std::string coolModuleFolderName2("/SCT/DAQ/Config/Module");
static const std::string coolMurFolderName2("/SCT/DAQ/Config/MUR");


// Static value to represent all 128 channels being good (signed int)
//static const int noMask(-1);
// in case the chip number cannot be retrieved, this is the invalid value
static const int invalidChipNumber(-1);
// Constructor
SCT_ConfigurationConditionsSvc::SCT_ConfigurationConditionsSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_data{},
  m_filled(false),
  m_detStore("DetectorStore", name),
  m_IOVSvc("IOVSvc", name),
  m_IOVDbSvc("IOVDbSvc", name),
  m_pHelper{nullptr},
  m_cablingSvc("SCT_CablingSvc", name),
  m_pManager{nullptr},
  m_checkStripsInsideModules(true) 
{ 
  declareProperty("checkStripsInsideModule" , m_checkStripsInsideModules);
}

// Initialize
StatusCode SCT_ConfigurationConditionsSvc::initialize(){
  ATH_MSG_INFO( "Initializing configuration" );
  ATH_CHECK(m_cablingSvc.retrieve());
  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_pManager,"SCT"));
  ATH_CHECK(m_detStore->retrieve(m_pHelper, "SCT_ID"));
  ATH_CHECK(m_IOVSvc.retrieve());
  ATH_CHECK(m_IOVDbSvc.retrieve());

  // Register callbacks for folders 
  const std::string channelFolderName=determineFolder(coolChannelFolderName,coolChannelFolderName2);
  const std::string moduleFolderName=determineFolder(coolModuleFolderName,coolModuleFolderName2);
  const std::string murFolderName=determineFolder(coolMurFolderName,coolMurFolderName2);
  //
  if (m_detStore->regFcn(&SCT_ConfigurationConditionsSvc::fillData, this, m_dataChannel,channelFolderName).isFailure() or
      m_detStore->regFcn(&SCT_ConfigurationConditionsSvc::fillData, this, m_dataModule, moduleFolderName).isFailure() or
      m_detStore->regFcn(&SCT_ConfigurationConditionsSvc::fillData, this, m_dataMur, murFolderName).isFailure())
    return msg(MSG:: ERROR)<< "Failed to register callback" << endmsg, StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_ConfigurationConditionsSvc::finalize(){
  ATH_MSG_INFO( "Configuration finalize" );

  return StatusCode::SUCCESS;
}

// Query interfaces.
StatusCode SCT_ConfigurationConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( ISCT_ConfigurationConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if ( ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

// What level of element can this service report about
bool SCT_ConfigurationConditionsSvc::canReportAbout(InDetConditions::Hierarchy h){
  return (h == InDetConditions::SCT_STRIP or
          h == InDetConditions::SCT_CHIP or
          h == InDetConditions::SCT_SIDE or
          h == InDetConditions::SCT_MODULE or
          h == InDetConditions::DEFAULT);
}

// Is an element with this Identifier and hierachy good?
bool SCT_ConfigurationConditionsSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy  h){
  if (not canReportAbout(h)) return true;

  bool result(true);
  if (h == InDetConditions::SCT_STRIP) {
    result = (not m_data.isBadStripId(elementId));
    // If strip itself is not bad, check if it's in a bad module
    if (result and m_checkStripsInsideModules) result = !isStripInBadModule(elementId);
  } else if (h == InDetConditions::SCT_MODULE) {
    result = (not m_data.isBadModuleId(elementId));
  } else if (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT) {
    result = (not m_data.isBadWaferId(elementId));
  } else if (h == InDetConditions::SCT_CHIP) {
    result = isGoodChip(elementId);
  }
  return result;
}

// Is a wafer with this IdentifierHash good?
bool SCT_ConfigurationConditionsSvc::isGood(const IdentifierHash & hashId){
  Identifier elementId(m_pHelper->wafer_id(hashId));
  return isGood(elementId);
}

// Is a chip with this Identifier good?
bool SCT_ConfigurationConditionsSvc::isGoodChip(const Identifier& stripId) const {
  // This check assumes present SCT.
  // Get module number
  const Identifier moduleId{m_pHelper->module_id(stripId)};
  if (not moduleId.is_valid()) {
    ATH_MSG_WARNING("moduleId obtained from stripId " << stripId << " is invalid.");
    return false;
  }

  // badChips word for the module
  const unsigned int v_badChips{badChips(moduleId)};
  // badChips holds 12 bits.
  // bit 0 (LSB) is chip 0 for side 0.
  // bit 5 is chip 5 for side 0.
  // bit 6 is chip 6 for side 1.
  // bit 11 is chip 11 for side 1.

  // If there is no bad chip, this check is done.
  if (v_badChips==0) return true;

  const int side{m_pHelper->side(stripId)};
  // Check the six chips on the side
  // 0x3F  = 0000 0011 1111
  // 0xFC0 = 1111 1100 0000
  // If there is no bad chip on the side, this check is done.
  if ((side==0 and (v_badChips & 0x3F)==0) or (side==1 and (v_badChips & 0xFC0)==0)) return true;

  int chip{getChip(stripId)};
  if (chip<0 or chip>=12) {
    ATH_MSG_WARNING("chip number is invalid: " << chip);
    return false;
  }

  // Check if the chip is bad
  const bool badChip{static_cast<bool>(v_badChips & (1<<chip))};

  return (not badChip);
}

  // Callback funtion (with arguments required by IovDbSvc) to fill channel, module and link info
StatusCode SCT_ConfigurationConditionsSvc::fillData(int& /*i*/ , std::list<std::string>& l){  
  std::list<std::string>::iterator itr(l.begin());
  std::list<std::string>::iterator itrEnd(l.end());
  const std::string channelFolderName=determineFolder(coolChannelFolderName,coolChannelFolderName2);
  const std::string moduleFolderName=determineFolder(coolModuleFolderName,coolModuleFolderName2);
  const std::string murFolderName=determineFolder(coolMurFolderName,coolMurFolderName2);
  // Fill module data if Module folder has changed
  if (find(itr, itrEnd, moduleFolderName) != itrEnd) {
    if (fillModuleData().isFailure()) return StatusCode::FAILURE;
  }
  // Fill strip, chip and link info if Chip or MUR folders change
  if (find(itr, itrEnd, channelFolderName) != itrEnd or
      find(itr, itrEnd, murFolderName)     != itrEnd) {
    if(fillChannelData().isFailure()) return StatusCode::FAILURE;
  }
  // The bad channel list contains all the information
  m_filled = (not m_data.getBadStripIds()->empty());
  return StatusCode::SUCCESS;
}

// Fill bad strip, chip and link info
StatusCode SCT_ConfigurationConditionsSvc::fillChannelData(){
  unsigned int nDisabledChips(0);
  unsigned int nDisabledChipsExclusive(0);
  unsigned int nDisabledStripsExclusive(0);
  const std::string channelFolderName=determineFolder(coolChannelFolderName,coolChannelFolderName2);
  const bool run1=(channelFolderName==coolChannelFolderName);
  //
  // indices change according to whether CoraCool or CoolVectorPayload
  enum RUN1_MODULE_INDICES{PK, FOREIGN_KEY, CRATE_1, ROD_1, CHANNEL_1,OUTPUTCURRENT_1,
   OUTPUTDELAY_1, OUTPUT_TOF_DELTA_1, OUTPUTMARKSPACE_1, STREAM0THRESHOLD_1,
   STREAM0DELAY_1, STREAM0ERRMASK_1, STREAM1THRESHOLD_1, STREAM1DELAY_1, STREAM1ERRMASK_1};
  //
  enum RUN2_MODULE_INDICES{CRATE_2, ROD_2, CHANNEL_2,OUTPUTCURRENT_2,
   OUTPUTDELAY_2, OUTPUTMARKSPACE_2, STREAM0THRESHOLD_2,
   STREAM0DELAY_2, STREAM0ERRMASK_2, STREAM1THRESHOLD_2, STREAM1DELAY_2, STREAM1ERRMASK_2};
  //
  enum RUN1_CHIP_INDICES{CHIP_PK, CHIP_FOREIGN_KEY,CHIP_1, ACTIVE_1,ADDRESS_1, CONFIG_1,
   MASK0_1,MASK1_1,MASK2_1,MASK3_1, VTHR_1, VCAL_1, DELAY_1, PREAMP_1, SHAPER_1,
   RC_FUNCTION_1, RC_ARGS_1, C_FACTOR_1, TARGET_1, TRIM_1};
  //
  enum RUN2_CHIP_INDICES{CHIP_2, ACTIVE_2, ADDRESS_2, CONFIG_2, MASK0_2,MASK1_2,MASK2_2,
   MASK3_2, VTHR_2, VCAL_2, DELAY_2, PREAMP_2, SHAPER_2, RC_FUNCTION_2, RC_ARGS_2,
   C_FACTOR_2, TARGET_2, TRIM_2};
  typedef unsigned int uint_t;
  const uint_t chipIndex=run1?uint_t(CHIP_1):uint_t(CHIP_2);
  const uint_t configIndex=run1?uint_t(CONFIG_1):uint_t(CONFIG_2);
  const uint_t mask0Index=run1?uint_t(MASK0_1):uint_t(MASK0_2);
  const uint_t mask1Index=run1?uint_t(MASK1_1):uint_t(MASK1_2);
  const uint_t mask2Index=run1?uint_t(MASK2_1):uint_t(MASK2_2);
  const uint_t mask3Index=run1?uint_t(MASK3_1):uint_t(MASK3_2);
  //
  // Clear previous information at callback
  m_data.clearBadStripIds();
  m_data.clearBadChips();
  // Fill link status
  if (fillLinkStatus().isFailure()) return StatusCode::FAILURE;
  // Get Chip folder
  if (retrieveFolder(m_dataChannel, channelFolderName).isFailure()) {
    return msg(MSG:: ERROR)<< "Could not fill channel configuration data" << endmsg, StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "fillChannelData: IOV callback resulted in a Chip CondAttrListVec of size " << m_dataChannel->size());
  }
  // Loop over modules (i.e groups of 12 chips) in DB folder 
  constexpr unsigned int nChips(12);
  CondAttrListVec::const_iterator modItr(m_dataChannel->begin());
  CondAttrListVec::const_iterator modEnd(m_dataChannel->end());
  for (;modItr != modEnd; modItr += nChips) {
    // Get SN and identifiers (the channel number is serial number+1 for the CoraCool folders but =serial number 
    //  for Cool Vector Payload ; i.e. Run 1 and Run 2 resp.)
    const unsigned int truncatedSerialNumber(run1?(modItr->first - 1):(modItr->first));
    const IdentifierHash& hash = m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber);
    if (not hash.is_valid()) continue;
    const Identifier  waferId(m_pHelper->wafer_id(hash));
    const Identifier  moduleId(m_pHelper->module_id(waferId));

    IdentifierHash oppWaferHash;
    m_pHelper->get_other_side(hash, oppWaferHash);
    const Identifier oppWaferId(m_pHelper->wafer_id(oppWaferHash));

    bool isBadModule(m_data.isBadModuleId(moduleId));

    // Get link status 
    // Can maybe be smarter if both links are bad (but the module will probably be bad then)
    std::pair<bool, bool> linkResults{badLinks(moduleId)};
    bool link0ok{linkResults.first};
    bool link1ok{linkResults.second};
    // Loop over chips within module
    CondAttrListVec::const_iterator channelItr(modItr);
    const CondAttrListVec::const_iterator channelEnd(modItr + nChips);
    std::vector<SCT_Chip*> chipsInMod;
    chipsInMod.reserve(12);

    bool isBadSide0 = true;
    bool isBadSide1 = true;
    
    for(; channelItr != channelEnd; ++channelItr){
      // Get chip id, config and masks and store as SCT_Chip object
      // Can get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
      const short id      = run1?(channelItr->second[chipIndex].data<short>()):(channelItr->second[chipIndex].data<unsigned char>());
      const short config  = run1?(channelItr->second[configIndex].data<short>()):(channelItr->second[configIndex].data<unsigned short>());
      const int mask0     = run1?(channelItr->second[mask0Index].data<int>()):(channelItr->second[mask0Index].data<uint_t>());
      const int mask1     = run1?(channelItr->second[mask1Index].data<int>()):(channelItr->second[mask1Index].data<uint_t>());
      const int mask2     = run1?(channelItr->second[mask2Index].data<int>()):(channelItr->second[mask2Index].data<uint_t>());  // (=noMask, declared as static int at top of this file)
      const int mask3     = run1?(channelItr->second[mask3Index].data<int>()):(channelItr->second[mask3Index].data<uint_t>());
      chipsInMod.push_back(new SCT_Chip(id, config, mask0, mask1, mask2, mask3));

      if(id>=0 and id< 6 and (mask0!=0 or mask1!=0 or mask2!=0 or mask3!=0)) isBadSide0 = false;
      if(id>=6 and id<12 and (mask0!=0 or mask1!=0 or mask2!=0 or mask3!=0)) isBadSide1 = false;
    }

    if(isBadSide0) m_data.setBadWaferId(waferId);
    if(isBadSide1) m_data.setBadWaferId(oppWaferId);
    
    // Check the module readout to look for bypassed chips, disabled links etc
    if (m_readoutTool->determineReadout(moduleId, chipsInMod, link0ok, link1ok).isFailure()) return StatusCode::FAILURE; 
    // Loop over chips again now know whether they're in the readout
    std::vector<int> badStripsVec;
    unsigned int chipStatusWord(0);
    for(const auto & thisChip:chipsInMod){
      // Bad strips (only need to do this if at least one bad channel)
      if (thisChip->numberOfMaskedChannels() != 0){
        // Add bad stips to vector
        badStripsVec.clear();
        thisChip->appendBadStripsToVector(badStripsVec);
        // Loop over bad strips and insert strip ID into set
        for(const auto & thisBadStrip:badStripsVec){
          Identifier stripId(getStripId(truncatedSerialNumber, thisChip->id(), thisBadStrip));
          // If in rough order, may be better to call with itr of previous insertion as a suggestion    
          if (stripId.is_valid()) m_data.setBadStripId(stripId);
        }
      }
      // Bad chips (= all strips bad) bitpacked
      // Should only do this for modules with at least one chip bad?
      if (thisChip->numberOfMaskedChannels() == stripsPerChip) {
        chipStatusWord |= (1<<thisChip->id());
        nDisabledChips++; // A bad chip
        if(!isBadModule) nDisabledChipsExclusive++; // A bad chip in a good module
      } else { // Good chip
        if(!isBadModule) nDisabledStripsExclusive += thisChip->numberOfMaskedChannels(); // Bad strips in a good chip of a good module
      }

    }
    // Store chip status if not all good (==0)
    if (chipStatusWord != 0) {
      m_data.setBadChips(moduleId, chipStatusWord);
    }
    // Clear up memory associated with chips    
    for (const auto & thisChip:chipsInMod){
      delete thisChip;
    }
  }

  // No longer need the conditions folder as stored locally
  m_IOVDbSvc->dropObject(channelFolderName,true); 

  const unsigned int totalBad(m_data.getBadStripIds()->size());
  msg(MSG:: INFO)<< "Total number of bad chips is " << nDisabledChips << endmsg;
  msg(MSG:: INFO)<< "Total number of bad chips not in bad modules is " << nDisabledChipsExclusive << endmsg;
  msg(MSG:: INFO)<< "Total number of bad strip identifiers is " << totalBad << endmsg;
  msg(MSG:: INFO)<< "Total number of bad strip identifiers not in bad modules nor bad chips is " << nDisabledStripsExclusive << endmsg;
  return StatusCode::SUCCESS;
}

// Fill bad module info
StatusCode SCT_ConfigurationConditionsSvc::fillModuleData(){
  const std::string moduleFolderName=determineFolder(coolModuleFolderName,coolModuleFolderName2);
  const bool run1=(moduleFolderName==coolModuleFolderName);
  unsigned int totalNumberOfModules(0);
  unsigned int totalNumberOfValidModules(0);

  // Clear previous information at callback
  m_data.clearBadModuleIds();
  m_data.clearBadWaferIds();

  // Get Module folder
  if (retrieveFolder(m_dataModule, moduleFolderName).isFailure()) {
    return msg(MSG:: ERROR)<< "Could not fill module configuration data" << endmsg, StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "fillModuleData: IOV callback resulted in a CondAttrListVec of size " << m_dataModule->size() );
  }
  // Loop over modules in DB folder
  CondAttrListVec::const_iterator pModule(m_dataModule->begin());
  CondAttrListVec::const_iterator pLastModule(m_dataModule->end());
  enum RUN1_INDICES{PK, FOREIGN_KEY, ID_1, GROUP_1, ACTIVE_1, SELECT_1};
  enum RUN2_INDICES{ID_2, SLAVEGROUP_2, ACTIVE_2, CLOCKSELECT_2, CHIPSTATUS_2};
  typedef unsigned int uint_t;
  const uint_t groupIndex=run1?uint_t(GROUP_1):uint_t(SLAVEGROUP_2);
  for (;pModule != pLastModule; ++pModule){
    // Get SN and identifiers (the channel number is serial number+1 for the CoraCool folders but =serial number 
    //  for Cool Vector Payload ; i.e. Run 1 and Run 2 resp.)
    const unsigned int truncatedSerialNumber(run1?(pModule->first - 1):(pModule->first));
    const IdentifierHash &hash=m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber);
    ++totalNumberOfModules;
    if (not hash.is_valid()) continue;
    Identifier  waferId(m_pHelper->wafer_id(hash));
    ++totalNumberOfValidModules;
    IdentifierHash oppWaferHash;
    m_pHelper->get_other_side(m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber) , oppWaferHash);
    const Identifier     oppWaferId(m_pHelper->wafer_id(oppWaferHash));
    const Identifier     moduleId(m_pHelper->module_id(waferId));
    // Get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
    // and get module info from this.  Bad module has a -ve group.
    const short group=pModule->second[groupIndex].data<short>();
    if (group < 0) { 
      // Insert module/wafer ID into set of bad modules/wafers IDs
      m_data.setBadModuleId(moduleId);
      m_data.setBadWaferId(waferId);
      m_data.setBadWaferId(oppWaferId);
    }
  }  
  // No longer need the conditions folder as stored locally
  m_IOVDbSvc->dropObject(moduleFolderName,true); 
  const unsigned int totalBad(m_data.getBadModuleIds()->size());
  ATH_MSG_INFO( "Total number of module identifiers is " << totalNumberOfModules );
  ATH_MSG_INFO( "Total number of modules also found in the cabling is " << totalNumberOfValidModules );
  ATH_MSG_INFO( "Total number of bad module identifiers is " << totalBad );    
  return StatusCode::SUCCESS;  
}

// Is the information filled?
bool SCT_ConfigurationConditionsSvc::filled() const{
  return m_filled;
}

// Get a DB folder
StatusCode SCT_ConfigurationConditionsSvc::retrieveFolder(const DataHandle<CondAttrListVec> &pDataVec, const std::string & folderName){
  if (not m_detStore) return (msg(MSG:: FATAL) << "The detector store pointer is NULL" << endmsg), StatusCode::FAILURE;
  if (m_detStore->retrieve(pDataVec, folderName).isFailure()) 
    return (msg(MSG:: FATAL) << "Could not retrieve AttrListVec for " << folderName << endmsg), StatusCode::FAILURE;
  if (0==pDataVec->size()) return (msg(MSG:: FATAL) << "This folder's data set appears to be empty: " << folderName << endmsg), StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

// Construct the strip ID from the module SN, chip number and strip number
Identifier 
SCT_ConfigurationConditionsSvc::getStripId(const unsigned int truncatedSerialNumber, const unsigned int chipNumber, const unsigned int stripNumber) const{
  Identifier     waferId;
  const Identifier invalidIdentifier;//initialiser creates invalid Id
  unsigned int   strip(0);
  IdentifierHash waferHash;
  if (not m_cablingSvc) {
    msg(MSG:: FATAL)<< "The cabling tool pointer is zero." << endmsg;
    return invalidIdentifier;
  }
  // If the chip is 0-5 we are in side 0, otherwise in side 1. 'getHash' only 
  // returns the side 0 hash, so we use the helper for side 1

  if (chipNumber<6){
    waferHash = m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber);
    strip     = chipNumber * stripsPerChip + stripNumber;
    if (waferHash.is_valid()) waferId   = m_pHelper->wafer_id(waferHash);
  } else {
    m_pHelper->get_other_side( m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber) , waferHash);
    strip   = (chipNumber - 6) * stripsPerChip + stripNumber;
    if (waferHash.is_valid()) waferId = m_pHelper->wafer_id(waferHash);
  }

  if (not waferId.is_valid()) return invalidIdentifier;

  const InDetDD::SiDetectorElement* pElement = (m_pManager->getDetectorElement(waferHash));
  if (! pElement) {
    msg(MSG:: FATAL)<< "Element pointer is NULL in 'getStripId' method" << endmsg;
    return invalidIdentifier;
  }
  strip = (pElement->swapPhiReadoutDirection()) ? lastStrip - strip: strip;
  return m_pHelper->strip_id(waferId, strip);
}

// Fill link info
StatusCode SCT_ConfigurationConditionsSvc::fillLinkStatus() {
  const std::string murFolderName=determineFolder(coolMurFolderName,coolMurFolderName2);
  const bool run1(murFolderName==coolMurFolderName);
  // Clear previous information at call back
  m_data.clearBadLinks();

  // Get MUR folders for link info 
  if (retrieveFolder(m_dataMur, murFolderName).isFailure()) {
    return msg(MSG:: ERROR)<< "Could not fill MUR configuration data" << endmsg, StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "fillLinkStatus: IOV callback resulted in a MUR CondAttrListColl of size " << m_dataMur->size() );
  }
  enum RUN1_INDICES{PK, FOREIGN_KEY, MUR_1, MODULE_1, MODULEID_1, RMODULEID_1, RX0FIBRE_1, RX1FIBRE_1, TXFIBRE_1};
  enum RUN2_INDICES{MUR_2, MODULE_2, MODULEID_2, RMODULEID_2, RX0FIBRE_2, RX1FIBRE_2, TXFIBRE_2};
  //
  // loop over MUR folder
  CondAttrListVec::const_iterator pMur(m_dataMur->begin());
  CondAttrListVec::const_iterator pLastMur(m_dataMur->end());
  typedef unsigned int uint_t;
  const uint_t snIndex=run1?uint_t(MODULEID_1):uint_t(MODULEID_2);
  const uint_t link0Index=run1?uint_t(RX0FIBRE_1):uint_t(RX0FIBRE_2);
  const uint_t link1Index=run1?uint_t(RX1FIBRE_1):uint_t(RX1FIBRE_2);
  //
  for (; pMur != pLastMur; ++pMur) {
    // Check for null values
    if (pMur->second[snIndex].isNull()) continue;
    const long long ullSerialNumber     = pMur->second[snIndex].data<long long>();
    const SCT_SerialNumber serialNumber(ullSerialNumber);  
    if (not serialNumber.is_valid()) continue;  
    const IdentifierHash& hash = m_cablingSvc->getHashFromSerialNumber(serialNumber.to_uint());
    if (not hash.is_valid()) continue;

    Identifier  waferId(m_pHelper->wafer_id(hash));
    Identifier  moduleId(m_pHelper->module_id(waferId));
    int link0 = run1?(pMur->second[link0Index].data<int>()):(pMur->second[link0Index].data<unsigned char>());
    int link1 = run1?(pMur->second[link1Index].data<int>()):(pMur->second[link1Index].data<unsigned char>());

    // Store the modules with bad links, represented by badLink (enum in header) = 255 = 0xFF 
    if (link0 == badLink or link1 == badLink) {
      m_data.setBadLinks(moduleId, (link0!=badLink), (link1!=badLink));
    }
  }
  // No longer need the conditions folder as stored locally
  m_IOVDbSvc->dropObject(murFolderName,true); 
  return StatusCode::SUCCESS;
}


// Check if a strip is within a bad module
bool SCT_ConfigurationConditionsSvc::isStripInBadModule(const Identifier& stripId){
  const Identifier moduleId(m_pHelper->module_id(m_pHelper->wafer_id(stripId)));
  return m_data.isBadModuleId(moduleId);
}

// Check if a wafer is within a bad module
bool SCT_ConfigurationConditionsSvc::isWaferInBadModule(const Identifier& waferId){
  Identifier moduleId(m_pHelper->module_id(waferId));
  return m_data.isBadModuleId(moduleId);
}

// Find the chip number containing a particular strip Identifier
int SCT_ConfigurationConditionsSvc::getChip(const Identifier & stripId) const {

  // Find side and strip number
  const int side(m_pHelper->side(stripId));
  int strip(m_pHelper->strip(stripId));

  // Check for swapped readout direction
  const IdentifierHash waferHash = m_pHelper->wafer_hash(m_pHelper->wafer_id(stripId));
  const InDetDD::SiDetectorElement* pElement = m_pManager->getDetectorElement(waferHash);
  if (! pElement){
     msg(MSG:: FATAL)<< "Element pointer is NULL in 'badStrips' method" << endmsg;
     return invalidChipNumber;
  }
  strip = (pElement->swapPhiReadoutDirection()) ? lastStrip - strip: strip;

  // Find chip number
  return (side==0 ? strip/stripsPerChip : strip/stripsPerChip + 6);
}

void SCT_ConfigurationConditionsSvc::badStrips(const Identifier & moduleId,  std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) {
  // Bad strips for a given module
  if (ignoreBadModules) {
    // Ignore strips in bad modules
    if (m_data.isBadModuleId(moduleId)) return;
  }

  std::set<Identifier>::const_iterator chanItr(m_data.getBadStripIds()->begin());
  std::set<Identifier>::const_iterator chanEnd(m_data.getBadStripIds()->end());
  for (; chanItr != chanEnd; ++chanItr) {
    if (ignoreBadChips) {
      // Ignore strips in bad chips
      int chip = getChip(*chanItr);
      if (chip!=invalidChipNumber){
        unsigned int chipStatusWord{m_data.getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }   
    }
    if (m_pHelper->module_id(m_pHelper->wafer_id((*chanItr))) == moduleId) strips.insert(*chanItr);
  }
}
       
std::pair<bool, bool> SCT_ConfigurationConditionsSvc::badLinks(const Identifier & moduleId) {
  // Bad links for a given module
  // Bad convetion is used. true is for good link and false is for bad link...
  return m_data.areBadLinks(moduleId);
}

unsigned int SCT_ConfigurationConditionsSvc::badChips(const Identifier & moduleId) const {
  // Bad chips for a given module
  return m_data.getBadChips(moduleId);
}

void 
SCT_ConfigurationConditionsSvc::badStrips(std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) {
  if (ignoreBadModules == false && ignoreBadChips == false) {
    std::copy(m_data.getBadStripIds()->begin(), m_data.getBadStripIds()->end(), std::inserter(strips,strips.begin()));
    return;
  }
  std::set<Identifier>::const_iterator chanItr(m_data.getBadStripIds()->begin());
  std::set<Identifier>::const_iterator chanEnd(m_data.getBadStripIds()->end());
  for (; chanItr != chanEnd; ++chanItr) {
    Identifier moduleId(m_pHelper->module_id(m_pHelper->wafer_id(*chanItr)));
    // Ignore strips in bad modules
    if (ignoreBadModules) {
      if (m_data.isBadModuleId(moduleId)) continue;
    }
    // Ignore strips in bad chips
    if (ignoreBadChips) {    
      int chip = getChip(*chanItr);
      if (chip !=invalidChipNumber){
        unsigned int chipStatusWord{m_data.getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }  
    }
    strips.insert(*chanItr);
  }
}

//determine which folder to use; COMP200 style or CONDBR2 style
std::string 
SCT_ConfigurationConditionsSvc::determineFolder(const std::string &option1, const std::string &option2) const{
std::string result("");
  const bool option1Exists = m_detStore->contains<CondAttrListVec>(option1);
  const bool option2Exists = m_detStore->contains<CondAttrListVec>(option2);
  //its only sensible if either of these exists (but not both)
  const bool nonsense = (option1Exists == option2Exists);
  if (nonsense) {
    if (not option1Exists) ATH_MSG_ERROR("The folder names "<<option1<<" or "<<option2<<" could not be found");
    else ATH_MSG_ERROR("Both folders "<<option1<<" and "<<option2<<" have been loaded; cannot determine run1/run2");
  } else {
    result = option1Exists ? option1 : option2;
    ATH_MSG_INFO("SCT_ConfigurationConditionsSvc will use the folder "<<result);
  }
 return result;
}

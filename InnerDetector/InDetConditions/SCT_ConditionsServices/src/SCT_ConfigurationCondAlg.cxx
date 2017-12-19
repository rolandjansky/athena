/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConfigurationCondAlg.h"

// Athena include
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "SCT_Cabling/SCT_SerialNumber.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Local includes
#include "SCT_ReadoutTool.h"
#include "SCT_Chip.h"

// Static folder names 
const std::string SCT_ConfigurationCondAlg::s_coolChannelFolderName{"/SCT/DAQ/Configuration/Chip"};
const std::string SCT_ConfigurationCondAlg::s_coolModuleFolderName{"/SCT/DAQ/Configuration/Module"};
const std::string SCT_ConfigurationCondAlg::s_coolMurFolderName{"/SCT/DAQ/Configuration/MUR"};

//Run2: folders change name in CONDBR2 database
const std::string SCT_ConfigurationCondAlg::s_coolChannelFolderName2{"/SCT/DAQ/Config/Chip"};
const std::string SCT_ConfigurationCondAlg::s_coolModuleFolderName2{"/SCT/DAQ/Config/Module"};
const std::string SCT_ConfigurationCondAlg::s_coolMurFolderName2{"/SCT/DAQ/Config/MUR"};

SCT_ConfigurationCondAlg::SCT_ConfigurationCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_writeCdo{nullptr}
  , m_readKeyChannel{"/SCT/DAQ/Config/Chip"}
  , m_readKeyModule{"/SCT/DAQ/Config/Module"}
  , m_readKeyMur{"/SCT/DAQ/Config/MUR"}
  , m_writeKey{"SCT_ConfigurationCondData"}
  , m_condSvc{"CondSvc", name}
  , m_cablingSvc{"SCT_CablingSvc", name}
  , m_detStore{"DetectorStore", name}
  , m_pHelper{nullptr}
  , m_pManager{nullptr}
{
  declareProperty("ReadKeyChannel", m_readKeyChannel, "Key of input (raw) conditions folder of chips");
  declareProperty("ReadKeyModule", m_readKeyModule, "Key of input (raw) conditions folder of modules");
  declareProperty("ReadKeyMur", m_readKeyMur, "Key of input (raw) conditions folder of Murs");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) conditions folder");
}

SCT_ConfigurationCondAlg::~SCT_ConfigurationCondAlg() {
}

StatusCode SCT_ConfigurationCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // SCT cabling service
  ATH_CHECK(m_cablingSvc.retrieve());

  ATH_CHECK(m_detStore.retrieve());
  ATH_CHECK(m_detStore->retrieve(m_pHelper, "SCT_ID"));
  ATH_CHECK(m_detStore->retrieve(m_pManager, "SCT"));

  // Check conditions folder names
  if ((m_readKeyChannel.key()!=s_coolChannelFolderName) and (m_readKeyChannel.key()!=s_coolChannelFolderName2)) {
    ATH_MSG_FATAL(m_readKeyChannel.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }
  if ((m_readKeyModule.key()!=s_coolModuleFolderName) and (m_readKeyModule.key()!=s_coolModuleFolderName2)) {
    ATH_MSG_FATAL(m_readKeyModule.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }
  if((m_readKeyMur.key()!=s_coolMurFolderName) and (m_readKeyMur.key()!=s_coolMurFolderName2)) {
    ATH_MSG_FATAL(m_readKeyMur.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle
  ATH_CHECK(m_readKeyChannel.initialize());
  ATH_CHECK(m_readKeyModule.initialize());
  ATH_CHECK(m_readKeyMur.initialize());

  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  // Register write handle
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_ConfigurationCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_ConfigurationCondData> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS;
  }

  // Construct the output Cond Object and fill it in
  m_writeCdo = new SCT_ConfigurationCondData();
  // clear structures before filling
  m_writeCdo->clear();

  // Fill module data
  if (fillModuleData().isFailure()) {
    delete m_writeCdo;
    m_writeCdo = nullptr;
    return StatusCode::FAILURE;
  }

  // Fill strip, chip and link info if Chip or MUR folders change
  if (fillChannelData().isFailure()) {
    delete m_writeCdo;
    m_writeCdo = nullptr;
    return StatusCode::FAILURE;
  }

  // Define validity of the output cond obbject and record it
  EventIDRange rangeW{EventIDRange::intersect(m_rangeChannel, m_rangeModule, m_rangeMur)};
  if(rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection range: " << rangeW << " " << m_rangeChannel << " " << m_rangeModule << " " << m_rangeMur);
    delete m_writeCdo;
    m_writeCdo = nullptr;
    return StatusCode::FAILURE;
  }
  if (writeHandle.record(rangeW, m_writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_ConfigurationCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  // Set null pointer for the next execute.
  m_writeCdo = nullptr;

  return StatusCode::SUCCESS;
}

// Fill bad strip, chip and link info
StatusCode SCT_ConfigurationCondAlg::fillChannelData() {
  // Check if the pointer of derived conditions object is valid.
  if (m_writeCdo==nullptr) {
    ATH_MSG_FATAL("Pointer of derived conditions object is null");
    return StatusCode::FAILURE;
  }

  unsigned int nDisabledChips{0};
  unsigned int nDisabledChipsExclusive{0};
  unsigned int nDisabledStripsExclusive{0};
  const std::string channelFolderName{m_readKeyChannel.key()};
  const bool run1{channelFolderName==s_coolChannelFolderName};

  // indices change according to whether CoraCool or CoolVectorPayload
  enum RUN1_MODULE_INDICES{PK, FOREIGN_KEY, CRATE_1, ROD_1, CHANNEL_1,OUTPUTCURRENT_1,
                           OUTPUTDELAY_1, OUTPUT_TOF_DELTA_1, OUTPUTMARKSPACE_1, STREAM0THRESHOLD_1,
                           STREAM0DELAY_1, STREAM0ERRMASK_1, STREAM1THRESHOLD_1, STREAM1DELAY_1, STREAM1ERRMASK_1};
  enum RUN2_MODULE_INDICES{CRATE_2, ROD_2, CHANNEL_2,OUTPUTCURRENT_2,
                           OUTPUTDELAY_2, OUTPUTMARKSPACE_2, STREAM0THRESHOLD_2,
                           STREAM0DELAY_2, STREAM0ERRMASK_2, STREAM1THRESHOLD_2, STREAM1DELAY_2, STREAM1ERRMASK_2};
  enum RUN1_CHIP_INDICES{CHIP_PK, CHIP_FOREIGN_KEY,CHIP_1, ACTIVE_1,ADDRESS_1, CONFIG_1,
                         MASK0_1,MASK1_1,MASK2_1,MASK3_1, VTHR_1, VCAL_1, DELAY_1, PREAMP_1, SHAPER_1,
                         RC_FUNCTION_1, RC_ARGS_1, C_FACTOR_1, TARGET_1, TRIM_1};
  enum RUN2_CHIP_INDICES{CHIP_2, ACTIVE_2, ADDRESS_2, CONFIG_2, MASK0_2,MASK1_2,MASK2_2,
                         MASK3_2, VTHR_2, VCAL_2, DELAY_2, PREAMP_2, SHAPER_2, RC_FUNCTION_2, RC_ARGS_2,
                         C_FACTOR_2, TARGET_2, TRIM_2};
  const unsigned int chipIndex{  run1 ? static_cast<unsigned int>(CHIP_1)   : static_cast<unsigned int>(CHIP_2)};
  const unsigned int configIndex{run1 ? static_cast<unsigned int>(CONFIG_1) : static_cast<unsigned int>(CONFIG_2)};
  const unsigned int mask0Index{ run1 ? static_cast<unsigned int>(MASK0_1)  : static_cast<unsigned int>(MASK0_2)};
  const unsigned int mask1Index{ run1 ? static_cast<unsigned int>(MASK1_1)  : static_cast<unsigned int>(MASK1_2)};
  const unsigned int mask2Index{ run1 ? static_cast<unsigned int>(MASK2_1)  : static_cast<unsigned int>(MASK2_2)};
  const unsigned int mask3Index{ run1 ? static_cast<unsigned int>(MASK3_1)  : static_cast<unsigned int>(MASK3_2)};

  // Clear previous information at callback
  m_writeCdo->clearBadStripIds();
  m_writeCdo->clearBadChips();
  // Fill link status
  if (fillLinkStatus().isFailure()) return StatusCode::FAILURE;

  // Get channel folder for link info 
  SG::ReadCondHandle<CondAttrListVec> readHandle{m_readKeyChannel};
  const CondAttrListVec* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Could not find MUR configuration data: " << m_readKeyChannel.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of " << m_readKeyChannel.key() << " folder is " << readCdo->size());

  // Get EventIDRange
  if (not readHandle.range(m_rangeChannel)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // Loop over modules (i.e groups of 12 chips) in DB folder 
  CondAttrListVec::const_iterator itr{readCdo->begin()};
  CondAttrListVec::const_iterator end{readCdo->end()};
  constexpr unsigned int nChips{12};
  for (; itr!=end; itr+=nChips) {
    // Get SN and identifiers (the channel number is serial number+1 for the CoraCool folders but =serial number 
    // for Cool Vector Payload ; i.e. Run 1 and Run 2 resp.)
    const unsigned int truncatedSerialNumber{run1 ? (itr->first-1) : (itr->first)};
    const IdentifierHash& hash{m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber)};
    if (not hash.is_valid()) continue;
    const Identifier waferId{m_pHelper->wafer_id(hash)};
    const Identifier moduleId{m_pHelper->module_id(waferId)};

    IdentifierHash oppWaferHash;
    m_pHelper->get_other_side(hash, oppWaferHash);
    const Identifier oppWaferId{m_pHelper->wafer_id(oppWaferHash)};
    bool isBadModule{m_writeCdo->isBadModuleId(moduleId)};

    // Get link status 
    // Can maybe be smarter if both links are bad (but the module will probably be bad then)
    std::pair<bool, bool> linkResults{m_writeCdo->areBadLinks(moduleId)};
    bool link0ok{linkResults.first};
    bool link1ok{linkResults.second};
    // Loop over chips within module
    std::vector<SCT_Chip*> chipsInMod;
    chipsInMod.reserve(nChips);
    bool isBadSide0{true};
    bool isBadSide1{true};
    CondAttrListVec::const_iterator channelItr{itr};
    CondAttrListVec::const_iterator channelEnd{itr+nChips};
    for (; channelItr!=channelEnd; ++channelItr) {
      // Get chip id, config and masks and store as SCT_Chip object
      // Can get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
      const short id{    run1 ? (channelItr->second[chipIndex].data<short>())   : static_cast<short>(channelItr->second[chipIndex].data<unsigned char>())};
      const short config{run1 ? (channelItr->second[configIndex].data<short>()) : static_cast<short>(channelItr->second[configIndex].data<unsigned short>())};
      const int mask0{   run1 ? (channelItr->second[mask0Index].data<int>())    : static_cast<int>(channelItr->second[mask0Index].data<unsigned int>())};
      const int mask1{   run1 ? (channelItr->second[mask1Index].data<int>())    : static_cast<int>(channelItr->second[mask1Index].data<unsigned int>())};
      const int mask2{   run1 ? (channelItr->second[mask2Index].data<int>())    : static_cast<int>(channelItr->second[mask2Index].data<unsigned int>())};
      const int mask3{   run1 ? (channelItr->second[mask3Index].data<int>())    : static_cast<int>(channelItr->second[mask3Index].data<unsigned int>())};
      chipsInMod.push_back(new SCT_Chip(id, config, mask0, mask1, mask2, mask3));
      if (id>=0 and id< 6 and (mask0!=0 or mask1!=0 or mask2!=0 or mask3!=0)) isBadSide0 = false;
      if (id>=6 and id<12 and (mask0!=0 or mask1!=0 or mask2!=0 or mask3!=0)) isBadSide1 = false;
    }
    if (isBadSide0) m_writeCdo->setBadWaferId(waferId);
    if (isBadSide1) m_writeCdo->setBadWaferId(oppWaferId);
    
    // Check the module readout to look for bypassed chips, disabled links etc
    if (m_readoutTool->determineReadout(moduleId, chipsInMod, link0ok, link1ok).isFailure()) return StatusCode::FAILURE;

    // Loop over chips again now know whether they're in the readout
    std::vector<int> badStripsVec;
    unsigned int chipStatusWord{0};
    for (const auto& thisChip:chipsInMod) {
      // Bad strips (only need to do this if at least one bad channel)
      if (thisChip->numberOfMaskedChannels()!=0) {
        // Add bad stips to vector
        badStripsVec.clear();
        thisChip->appendBadStripsToVector(badStripsVec);
        // Loop over bad strips and insert strip ID into set
        for (const auto& thisBadStrip:badStripsVec) {
          Identifier stripId{getStripId(truncatedSerialNumber, thisChip->id(), thisBadStrip)};
          // If in rough order, may be better to call with itr of previous insertion as a suggestion    
          if (stripId.is_valid()) m_writeCdo->setBadStripId(stripId);
        }
      }
      // Bad chips (= all strips bad) bitpacked
      // Should only do this for modules with at least one chip bad?
      if (thisChip->numberOfMaskedChannels()==stripsPerChip) {
        chipStatusWord |= (1<<thisChip->id());
        nDisabledChips++; // A bad chip
        if (not isBadModule) nDisabledChipsExclusive++; // A bad chip in a good module
      } else { // Good chip
        if (not isBadModule) nDisabledStripsExclusive += thisChip->numberOfMaskedChannels(); // Bad strips in a good chip of a good module
      }

    }
    // Store chip status if not all good (==0)
    if (chipStatusWord!=0) {
      m_writeCdo->setBadChips(moduleId, chipStatusWord);
    }
    // Clear up memory associated with chips    
    for (const auto& thisChip: chipsInMod) {
      delete thisChip;
    }
  }

  const long unsigned int totalBad{m_writeCdo->getBadStripIds()->size()};
  ATH_MSG_INFO("Total number of bad chips is " << nDisabledChips);
  ATH_MSG_INFO("Total number of bad chips not in bad modules is " << nDisabledChipsExclusive);
  ATH_MSG_INFO("Total number of bad strip identifiers is " << totalBad);
  ATH_MSG_INFO("Total number of bad strip identifiers not in bad modules nor bad chips is " << nDisabledStripsExclusive);
  
  return StatusCode::SUCCESS;
}

// Fill bad module info
StatusCode SCT_ConfigurationCondAlg::fillModuleData() {
  // Check if the pointer of derived conditions object is valid.
  if (m_writeCdo==nullptr) {
    ATH_MSG_FATAL("Pointer of derived conditions object is null");
    return StatusCode::FAILURE;
  }

  const std::string moduleFolderName{m_readKeyModule.key()};
  const bool run1{moduleFolderName==s_coolModuleFolderName};
  unsigned int totalNumberOfModules{0};
  unsigned int totalNumberOfValidModules{0};

  // Clear previous information at callback
  m_writeCdo->clearBadModuleIds();
  m_writeCdo->clearBadWaferIds();

  // Get Module folder
  SG::ReadCondHandle<CondAttrListVec> readHandle{m_readKeyModule};
  const CondAttrListVec* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Could not find MUR configuration data: " << m_readKeyModule.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of " << m_readKeyModule.key() << " is " << readCdo->size());

  // Get EventIDRange
  if (not readHandle.range(m_rangeModule)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // Set index
  enum RUN1_INDICES{PK, FOREIGN_KEY, ID_1, GROUP_1, ACTIVE_1, SELECT_1};
  enum RUN2_INDICES{ID_2, SLAVEGROUP_2, ACTIVE_2, CLOCKSELECT_2, CHIPSTATUS_2};
  const unsigned int groupIndex{run1 ? static_cast<unsigned int>(GROUP_1) : static_cast<unsigned int>(SLAVEGROUP_2)};

  // Loop over modules in DB folder
  CondAttrListVec::const_iterator itr{readCdo->begin()};
  CondAttrListVec::const_iterator end{readCdo->end()};
  for (; itr!=end; ++itr) {
    // Get SN and identifiers (the channel number is serial number+1 for the CoraCool folders but =serial number 
    //  for Cool Vector Payload ; i.e. Run 1 and Run 2 resp.)
    const unsigned int truncatedSerialNumber{run1 ? (itr->first-1) : (itr->first)};
    const IdentifierHash& hash{m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber)};
    ++totalNumberOfModules;
    if (not hash.is_valid()) continue;

    Identifier waferId{m_pHelper->wafer_id(hash)};
    ++totalNumberOfValidModules;
    IdentifierHash oppWaferHash;
    m_pHelper->get_other_side(m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber), oppWaferHash);
    const Identifier oppWaferId{m_pHelper->wafer_id(oppWaferHash)};
    const Identifier moduleId{m_pHelper->module_id(waferId)};
    // Get AttributeList from second (see http://lcgapp.cern.ch/doxygen/CORAL/CORAL_1_9_3/doxygen/html/classcoral_1_1_attribute_list.html)
    // and get module info from this.  Bad module has a -ve group.
    const short group{itr->second[groupIndex].data<short>()};
    if (group<0) { 
      // Insert module/wafer ID into set of bad modules/wafers IDs
      m_writeCdo->setBadModuleId(moduleId);
      m_writeCdo->setBadWaferId(waferId);
      m_writeCdo->setBadWaferId(oppWaferId);
    }
  }  

  const long unsigned int totalBad{m_writeCdo->getBadModuleIds()->size()};
  ATH_MSG_INFO("Total number of module identifiers is " << totalNumberOfModules);
  ATH_MSG_INFO("Total number of modules also found in the cabling is " << totalNumberOfValidModules);
  ATH_MSG_INFO("Total number of bad module identifiers is " << totalBad);

  return StatusCode::SUCCESS;
}

// Fill link info
StatusCode SCT_ConfigurationCondAlg::fillLinkStatus() {
  // Check if the pointer of derived conditions object is valid.
  if (m_writeCdo==nullptr) {
    ATH_MSG_FATAL("Pointer of derived conditions object is null");
    return StatusCode::FAILURE;
  }

  const std::string murFolderName{m_readKeyMur.key()};
  const bool run1{murFolderName==s_coolMurFolderName};

  // Clear previous information at call back
  m_writeCdo->clearBadLinks();

  // Get MUR folder for link info 
  SG::ReadCondHandle<CondAttrListVec> readHandle{m_readKeyMur};
  const CondAttrListVec* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Could not find MUR configuration data: " << m_readKeyMur.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of " << m_readKeyMur.key() << " is " << readCdo->size());

  // Get EventIDRange
  if (not readHandle.range(m_rangeMur)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // Set indices
  enum RUN1_INDICES{PK, FOREIGN_KEY, MUR_1, MODULE_1, MODULEID_1, RMODULEID_1, RX0FIBRE_1, RX1FIBRE_1, TXFIBRE_1};
  enum RUN2_INDICES{MUR_2, MODULE_2, MODULEID_2, RMODULEID_2, RX0FIBRE_2, RX1FIBRE_2, TXFIBRE_2};
  const unsigned int snIndex{   run1 ? static_cast<unsigned int>(MODULEID_1) : static_cast<unsigned int>(MODULEID_2)};
  const unsigned int link0Index{run1 ? static_cast<unsigned int>(RX0FIBRE_1) : static_cast<unsigned int>(RX0FIBRE_2)};
  const unsigned int link1Index{run1 ? static_cast<unsigned int>(RX1FIBRE_1) : static_cast<unsigned int>(RX1FIBRE_2)};

  // loop over MUR folder
  CondAttrListVec::const_iterator itr{readCdo->begin()};
  CondAttrListVec::const_iterator end{readCdo->end()};
  for (; itr!=end; ++itr) {
    // Check for null values
    if (itr->second[snIndex].isNull()) continue;
    // Check serial number
    const long long ullSerialNumber{itr->second[snIndex].data<long long>()};
    const SCT_SerialNumber serialNumber{ullSerialNumber};
    if (not serialNumber.is_valid()) continue;
    // Check module hash
    const IdentifierHash& hash{m_cablingSvc->getHashFromSerialNumber(serialNumber.to_uint())};
    if (not hash.is_valid()) continue;

    Identifier waferId{m_pHelper->wafer_id(hash)};
    Identifier moduleId{m_pHelper->module_id(waferId)};
    int link0{run1 ? (itr->second[link0Index].data<int>()) : static_cast<int>(itr->second[link0Index].data<unsigned char>())};
    int link1{run1 ? (itr->second[link1Index].data<int>()) : static_cast<int>(itr->second[link1Index].data<unsigned char>())};

    // Store the modules with bad links, represented by badLink (enum in header) = 255 = 0xFF 
    if (link0==badLink or link1==badLink) {
      m_writeCdo->setBadLinks(moduleId, (link0!=badLink), (link1!=badLink));
    }
  }

  return StatusCode::SUCCESS;
}

// Construct the strip ID from the module SN, chip number and strip number
Identifier 
SCT_ConfigurationCondAlg::getStripId(const unsigned int truncatedSerialNumber, const unsigned int chipNumber, const unsigned int stripNumber) const {
  Identifier waferId;
  const Identifier invalidIdentifier; //initialiser creates invalid Id
  unsigned int strip{0};
  IdentifierHash waferHash;
  if (not m_cablingSvc) {
    ATH_MSG_FATAL("The cabling tool pointer is zero.");
    return invalidIdentifier;
  }
  // If the chip is 0-5 we are in side 0, otherwise in side 1. 'getHash' only 
  // returns the side 0 hash, so we use the helper for side 1

  if (chipNumber<6) {
    waferHash = m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber);
    strip = chipNumber * stripsPerChip + stripNumber;
    if (waferHash.is_valid()) waferId = m_pHelper->wafer_id(waferHash);
  } else {
    m_pHelper->get_other_side(m_cablingSvc->getHashFromSerialNumber(truncatedSerialNumber), waferHash);
    strip = (chipNumber - 6) * stripsPerChip + stripNumber;
    if (waferHash.is_valid()) waferId = m_pHelper->wafer_id(waferHash);
  }

  if (not waferId.is_valid()) return invalidIdentifier;

  const InDetDD::SiDetectorElement* pElement{m_pManager->getDetectorElement(waferHash)};
  if (!pElement) {
    ATH_MSG_FATAL("Element pointer is NULL in 'getStripId' method");
    return invalidIdentifier;
  }
  strip = (pElement->swapPhiReadoutDirection()) ? (lastStrip-strip) : strip;
  return m_pHelper->strip_id(waferId, strip);
}

StatusCode SCT_ConfigurationCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

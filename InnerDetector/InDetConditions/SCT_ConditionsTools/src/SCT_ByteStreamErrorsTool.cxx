/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTool.cxx
 * implementation file for tool that keeps track of errors in the bytestream
 * @author nbarlow@cern.ch
 **/

/// header file for this class.
#include "SCT_ByteStreamErrorsTool.h"
#include "SCT_DetectorElementStatus.h"

///Athena includes
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "StoreGate/ReadHandle.h"
#include "SCT_ReadoutGeometry/SCT_ChipUtils.h"
#include "InDetConditionsSummaryService/IDetectorElementStatusTool.h"

/** Constructor */
SCT_ByteStreamErrorsTool::SCT_ByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

/** Initialize */
StatusCode
SCT_ByteStreamErrorsTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_sct_id, "SCT_ID"));
  m_cntx_sct = m_sct_id->wafer_context();

  ATH_CHECK(m_config.retrieve());

  // Read (Cond)Handle Keys
  ATH_CHECK(m_bsIDCErrContainerName.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  m_badErrorMask=0;
  for (auto badError : SCT_ByteStreamErrors::BadErrors) {
     if (badError>=63) {
        ATH_MSG_FATAL("Logic error: Error code too large and cannot represented as a bit.");
     }
     m_badErrorMask |= (1<<badError);
  }

  return StatusCode::SUCCESS;
}

/** Finalize */
StatusCode
SCT_ByteStreamErrorsTool::finalize() {
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////

/** Used by ConditionsSummayTool to decide whether to call isGood() for a particular
 * detector element.
 * In principle we could report about modules and/or strips too, and
 * use the id helper to navigate up or down the hierarchy to the wafer,
 * but in practice we don't want to do the time-consuming isGood() for
 * every strip, so lets only report about wafers..
 */

bool
SCT_ByteStreamErrorsTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_CHIP);
}

const IDCInDetBSErrContainer* SCT_ByteStreamErrorsTool::getContainer(const EventContext& ctx) const {
  SG::ReadHandle<IDCInDetBSErrContainer> idcErrCont(m_bsIDCErrContainerName, ctx);
  /** When running over ESD files without BSErr container stored, don't
   * want to flood the user with error messages. Should just have a bunch
   * of empty sets, and keep quiet.
   */
  if (not idcErrCont.isValid()) {
    m_nRetrievalFailure++;
    if (m_nRetrievalFailure<=3) {
      ATH_MSG_INFO("SCT_ByteStreamErrorsTool Failed to retrieve BS error container "
                   << m_bsIDCErrContainerName.key()
                   << " from StoreGate.");
      if (m_nRetrievalFailure==3) {
        ATH_MSG_INFO("SCT_ByteStreamErrorsTool This message on retrieval failure of " << m_bsIDCErrContainerName.key() << " is suppressed.");
      }
    }
    return nullptr;
  }
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool IDC Container fetched " << m_bsIDCErrContainerName.key());
  return idcErrCont.cptr();
}

///////////////////////////////////////////////////////////////////////////
SCT_ByteStreamErrorsTool::IDCCacheEntry* SCT_ByteStreamErrorsTool::getCacheEntry(const EventContext& ctx) const {
  IDCCacheEntry* cacheEntry{m_eventCache.get(ctx)};
  if (cacheEntry->needsUpdate(ctx)) {
    const auto *idcErrContPtr{getContainer(ctx)};
    if (idcErrContPtr == nullptr) { // missing or not, the cache needs to be reset
      cacheEntry->reset(ctx.evt(), nullptr);
    } else {
      cacheEntry->reset(ctx.evt(), idcErrContPtr->cache());
    }
    ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool Cache for the event reset " << cacheEntry->eventId  << " with IDC container" << idcErrContPtr);
  }
  return cacheEntry;
}

/** this is the principle method which can be accessed via
 * the ConditionsSummaryTool to decide if a wafer is good - in this
 * case we want to return false if the wafer has an error that would
 * result in bad hits or no hits for that event */
bool
SCT_ByteStreamErrorsTool::isGood(const IdentifierHash& elementIdHash, const EventContext& ctx) const {
  {

    std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};
    ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool isGood called for " << elementIdHash);
    const auto *idcCachePtr{getCacheEntry(ctx)->IDCCache};
    if (idcCachePtr == nullptr) {
      ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool No cache! ");
      return true;
    }

    auto errorCode{idcCachePtr->retrieve(elementIdHash)};

    for (auto badError : SCT_ByteStreamErrors::BadErrors) {
      if (errorCode == badError) {
        ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool Bad Error " << errorCode  << " for ID " << elementIdHash);
        return false;
      }
    }
  } // end of cache operations protection via m_cacheMutex, following code has own protection
  
  // If all 6 chips of a link issue ABCD errors or are bad chips or temporarily masked chips, the link is treated as bad one. 
  const Identifier wafer_id{m_sct_id->wafer_id(elementIdHash)};
  const Identifier module_id{m_sct_id->module_id(wafer_id)};
  unsigned int badChips{m_config->badChips(module_id, ctx)};
  unsigned int v_abcdErrorChips{abcdErrorChips(module_id, ctx)};
  unsigned int v_tempMaskedChips{tempMaskedChips(module_id, ctx)};
  const int side{m_sct_id->side(wafer_id)};
  bool allChipsBad{true};
  const int chipMax{static_cast<short>(side==0 ? N_CHIPS_PER_SIDE : N_CHIPS_PER_SIDE*N_SIDES)};
  for (int chip{chipMax-N_CHIPS_PER_SIDE}; chip<chipMax; chip++) {
    bool issueABCDError{((v_abcdErrorChips >> chip) & 0x1) != 0};
    bool isBadChip{((badChips >> chip) & 0x1) != 0};
    bool isTempMaskedChip{((v_tempMaskedChips >> chip) & 0x1) != 0};
    allChipsBad = (issueABCDError or isBadChip or isTempMaskedChip);
    if (not allChipsBad) break;
  }
  return !allChipsBad;
}

namespace {
   unsigned int getValueOrZero(const std::unordered_map<size_t, unsigned int>  &map, size_t key) {
      std::unordered_map<size_t, unsigned int>::const_iterator iter = map.find(key);
      return iter != map.end() ? iter->second : 0;
   }
}


void
SCT_ByteStreamErrorsTool::getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status,
                                                   SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const {
   std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};

   const auto *idcCachePtr{getCacheEntry(ctx)};
   if (idcCachePtr == nullptr || !idcCachePtr->IDCCache) {
      ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool No cache! ");
      return;
   }
   if (whandle) {
     ATH_MSG_ERROR("SCT_ByteStreamErrorsTool is not for conditions objects");
     whandle->addDependency (IDetectorElementStatusTool::getInvalidRange());
   }

   if (fillData(ctx).isFailure()) {
      return; // @TODO what is the correct way to handle this ?  set status to false for all ?
   }

   std::vector<bool> &status = element_status.getElementStatus();
   std::vector<InDet::ChipFlags_t> &chip_status = element_status.getElementChipStatus();
   if (status.empty()) {
      status.resize(idcCachePtr->IDCCache->rawReadAccess().size(),true);
   }
   SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> si_element_list{m_SCTDetEleCollKey, ctx};
   if (not si_element_list.isValid()) {
      std::stringstream msg;
      msg << "Failed to get SCT detector element collection with key " << m_SCTDetEleCollKey.key();
      throw std::runtime_error(msg.str());
   }
   if (whandle) {
     whandle->addDependency (si_element_list);
   }

   constexpr InDet::ChipFlags_t all_flags_set = static_cast<InDet::ChipFlags_t>((1ul<<(N_CHIPS_PER_SIDE)) - 1ul);
   static_assert( (1ul<<(N_CHIPS_PER_SIDE*N_SIDES)) - 1ul <= std::numeric_limits<InDet::ChipFlags_t>::max());
   if (chip_status.empty()) {
      chip_status.resize(status.size(), all_flags_set);
   }
   unsigned int element_i=0;
   for ( const auto &val : idcCachePtr->IDCCache->rawReadAccess()) {
      uint64_t error_code = val;
      bool is_bad=(error_code<63) && ((1ull<<error_code) &  m_badErrorMask);
      status.at(element_i) = status.at(element_i) & not is_bad;
      if ( is_bad ) {
         ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool Bad Error " << error_code  << " for ID " << element_i);
      }
      else {
         IdentifierHash hash(element_i);
         const InDetDD::SiDetectorElement *siElement = si_element_list->getDetectorElement(hash);
         const Identifier wafer_id{m_sct_id->wafer_id(hash)};
         const Identifier module_id{m_sct_id->module_id(wafer_id)};
         size_t modhash =  static_cast<size_t>(module_id.get_compact());

         unsigned int badChips{m_config->badChips(module_id, ctx)}; // @todo only call once for all
         unsigned int v_abcdErrorChips{ getValueOrZero( idcCachePtr->abcdErrorChips, modhash) };
         unsigned int v_tempMaskedChips{ getValueOrZero( idcCachePtr->tempMaskedChips, modhash) };
         const int side{m_sct_id->side(wafer_id)};
         bool allChipsBad{true};
         const int chipMax{static_cast<short>(side==0 ? N_CHIPS_PER_SIDE : N_CHIPS_PER_SIDE*N_SIDES)};
         InDet::ChipFlags_t bad_chip_flags = 0; 
         for (int chip{chipMax-N_CHIPS_PER_SIDE}; chip<chipMax; chip++) {
            bool issueABCDError{((v_abcdErrorChips >> chip) & 0x1) != 0};
            bool isBadChip{((badChips >> chip) & 0x1) != 0};
            bool isTempMaskedChip{((v_tempMaskedChips >> chip) & 0x1) != 0};
            bool isBad = (issueABCDError or isBadChip or isTempMaskedChip);
            bad_chip_flags |= static_cast<InDet::ChipFlags_t>(isBad) << SCT::getGeometricalFromPhysicalChipID(side, siElement->swapPhiReadoutDirection(), chip);
            allChipsBad &= isBad;
         }
         status.at(element_i) =  status.at(element_i) & not allChipsBad;
         chip_status.at(element_i) &= (~bad_chip_flags) & all_flags_set;
      }

      ++element_i;
   }
}


bool
SCT_ByteStreamErrorsTool::isGood(const IdentifierHash& elementIdHash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementIdHash, ctx);
}

bool
SCT_ByteStreamErrorsTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;

  if (h==InDetConditions::SCT_SIDE) {
    const IdentifierHash elementIdHash{m_sct_id->wafer_hash(elementId)};
    return isGood(elementIdHash, ctx);
  }
  if (h==InDetConditions::SCT_CHIP) {
    return isGoodChip(elementId, ctx);
  }

  return true;
}

bool
SCT_ByteStreamErrorsTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(elementId, ctx, h);
}

bool
SCT_ByteStreamErrorsTool::isGoodChip(const Identifier& stripId, const EventContext& ctx) const {
  // This check assumes present SCT.
  // Get module number
  const Identifier moduleId{m_sct_id->module_id(stripId)};
  if (not moduleId.is_valid()) {
    ATH_MSG_WARNING("moduleId obtained from stripId " << stripId << " is invalid.");
    return false;
  }

  // tempMaskedChips and abcdErrorChips hold 12 bits.
  // bit 0 (LSB) is chip 0 for side 0.
  // bit 5 is chip 5 for side 0.
  // bit 6 is chip 6 for side 1.
  // bit 11 is chip 11 for side 1.
  // Temporarily masked chip information
  const unsigned int v_tempMaskedChips{tempMaskedChips(moduleId, ctx)};
  // Information of chips with ABCD errors
  const unsigned int v_abcdErrorChips{abcdErrorChips(moduleId, ctx)};
  // Take 'OR' of tempMaskedChips and abcdErrorChips
  const unsigned int badChips{v_tempMaskedChips | v_abcdErrorChips};

  // If there is no bad chip, this check is done.
  if (badChips==0) return true;

  const int side{m_sct_id->side(stripId)};
  // Check the six chips on the side
  // 0x3F  = 0000 0011 1111
  // 0xFC0 = 1111 1100 0000
  // If there is no bad chip on the side, this check is done.
  if ((side==0 and (badChips & 0x3F)==0) or (side==1 and (badChips & 0xFC0)==0)) return true;

  int chip{getChip(stripId, ctx)};
  if (chip<0 or chip>=12) {
    ATH_MSG_WARNING("chip number is invalid: " << chip);
    return false;
  }

  // Check if the chip is bad
  const bool badChip{static_cast<bool>(badChips & (1<<chip))};

  return (not badChip);
}

int
SCT_ByteStreamErrorsTool::getChip(const Identifier& stripId, const EventContext& ctx) const {
  const Identifier waferId{m_sct_id->wafer_id(stripId)};
  const IdentifierHash waferHash{m_sct_id->wafer_hash(waferId)};
  const InDetDD::SiDetectorElement* siElement{getDetectorElement(waferHash, ctx)};
  if (siElement==nullptr) {
    ATH_MSG_DEBUG ("InDetDD::SiDetectorElement is not obtained from stripId " << stripId);
    return -1;
  }
  return SCT::getChip(*m_sct_id, *siElement, stripId);
}

////////////////////////////////////////////////////////////////////////////////////////

/** The accessor method that can be used by clients to
 * retrieve a set of IdHashes of wafers with a given type of error.
 * e.g. for monitoring plots.
 */

std::set<IdentifierHash>
SCT_ByteStreamErrorsTool::getErrorSet(int errorType, const EventContext& ctx) const {
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool getErrorSet " << errorType);
  std::set<IdentifierHash> result;
  if (errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    const auto *idcErrCont{getContainer(ctx)};
    if (idcErrCont != nullptr) {
      const std::set<size_t>& Mask = idcErrCont->getMask();
      const auto& raw = idcErrCont->wholeEventReadAccess();
      for (const size_t hashId : Mask) {
        auto errCode = raw[hashId].load(std::memory_order_relaxed);
        if (SCT_ByteStreamErrors::hasError(errCode, static_cast<SCT_ByteStreamErrors::ErrorType>(errorType))) {
          result.insert(hashId);
        }
      }
    }
  }
  return result;
}

std::set<IdentifierHash>
SCT_ByteStreamErrorsTool::getErrorSet(int errorType) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getErrorSet(errorType, ctx);
}

////////////////////////////////////////////////////////////////////////

/** this function is used to populate the data of this tool from
 * the InDetBSErrContainer in StoreGate
 */

StatusCode
SCT_ByteStreamErrorsTool::fillData(const EventContext& ctx) const {
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool fillData");

  const IDCInDetBSErrContainer* idcErrCont{getContainer(ctx)};
  if (idcErrCont == nullptr) {
    ATH_MSG_VERBOSE("idcErrCont == nullptr");
    return StatusCode::SUCCESS;
  }

  auto *cacheEntry{getCacheEntry(ctx)};
  
  unsigned int idcErrCont_set_number =  idcErrCont->numberSet();

  if (cacheEntry->m_set_number == idcErrCont_set_number){
    ATH_MSG_VERBOSE("Same set number found, skip the next steps.");
    return StatusCode::SUCCESS;
  }else{
    cacheEntry->m_set_number = idcErrCont_set_number; //update the set number in cacheEntry.
  }

  /** OK, so we found the StoreGate container, now lets iterate
   * over it to populate the sets of errors owned by this Tool.
   */
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool size of error container is " << idcErrCont->maxSize());
  const std::vector<std::pair<size_t, uint64_t>> errorcodesforView{idcErrCont->getAll()};

  for (const auto& [ hashId, errCode ] : errorcodesforView) {

    Identifier wafer_id{m_sct_id->wafer_id(hashId)};
    Identifier module_id{m_sct_id->module_id(wafer_id)};
    size_t hash = static_cast<size_t>(module_id.get_compact());
    if (errCode == uint64_t{0}) {
      // That means this hashId was decoded but had no error
      // In such case we want to fill the cache also with zero so we do not have to fill the cache again for a given view
      // (see logic in: getErrorCodeWithCacheUpdate)
      // Note: invocation of the [] operator on the map will create missing entry and set the value to default (here 0)
      cacheEntry->abcdErrorChips[ hash ];
      cacheEntry->tempMaskedChips[ hash ];
      continue;
    }


    ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool filling event cache for module " << module_id << " ec " << errCode);

    const int side{m_sct_id->side(m_sct_id->wafer_id(hashId))};
    // Each bit of errCode represents each SCT_ByteStreamErrors for one wafer
    // Multiple bits can be true.
    // Convert errCode to 12 bits of abcdErrorChips and 12 bits of tempMaskedChips for one module (=two wafers).
    IDCInDetBSErrContainer::ErrorCode v_abcdErrorChips{errCode & SCT_ByteStreamErrors::ABCDErrorMask()};
    if (v_abcdErrorChips) {
      v_abcdErrorChips >>= SCT_ByteStreamErrors::ABCDError_Chip0; // bit 0 (5) is for chip 0 (5) for both sides
      v_abcdErrorChips <<= (side*N_CHIPS_PER_SIDE); // bit 0 (6) is for chip 0 on side 0 (1)
      cacheEntry->abcdErrorChips[hash] |= v_abcdErrorChips;
    } else {
      cacheEntry->abcdErrorChips[hash] = 0;
    }
    IDCInDetBSErrContainer::ErrorCode v_tempMaskedChips{errCode & SCT_ByteStreamErrors::TempMaskedChipsMask()};
    if (v_tempMaskedChips) {
      v_tempMaskedChips >>= SCT_ByteStreamErrors::TempMaskedChip0; // bit 0 (5) is for chip 0 (5) for both sides0
      v_tempMaskedChips <<= (side*N_CHIPS_PER_SIDE); // bit 0 (6) is for chip 0 on side 0 (1)
      cacheEntry->tempMaskedChips[hash] |= v_tempMaskedChips;
    } else {
      cacheEntry->tempMaskedChips[hash] = 0;
    }

  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int SCT_ByteStreamErrorsTool::tempMaskedChips(const Identifier& moduleId, const EventContext& ctx) const {
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool tempMaskedChips");
  std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};
  auto *cacheEntry{getCacheEntry(ctx)};
  if (cacheEntry->IDCCache == nullptr) {
    ATH_MSG_VERBOSE("cacheEntry->IDCCache == nullptr");
    return 0;
  }

  auto [status, v_tempMaskedChips] = getErrorCodeWithCacheUpdate(moduleId, ctx, cacheEntry->tempMaskedChips);
  if (status.isFailure()) {
    ATH_MSG_ERROR("SCT_ByteStreamErrorsTool Failure getting temp masked chip errors");
  }
  return v_tempMaskedChips; // 12 bits are used.
  // Bit 0 is for chip 0 on side 0, bit 1 is for chip 1 on side 0, ..., and bit 11 is for chip 5 on side 1
}

unsigned int SCT_ByteStreamErrorsTool::tempMaskedChips(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return tempMaskedChips(moduleId, ctx);
}

unsigned int SCT_ByteStreamErrorsTool::abcdErrorChips(const Identifier& moduleId, const EventContext& ctx) const {
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool abcdErrorChips");
  std::scoped_lock<std::mutex> lock{*m_cacheMutex.get(ctx)};
  auto *cacheEntry{getCacheEntry(ctx)};
  if (cacheEntry->IDCCache == nullptr) {
    ATH_MSG_VERBOSE("cacheEntry->IDCCache == nullptr");
    return 0;
  }

  auto [status, v_abcdErrorChips] = getErrorCodeWithCacheUpdate(moduleId, ctx, cacheEntry->abcdErrorChips);
  if (status.isFailure()) {
    ATH_MSG_ERROR("SCT_ByteStreamErrorsTool Failure getting ABCD chip errors");
  }
  return v_abcdErrorChips; // 12 bits are used.
  // Bit 0 is for chip 0 on side 0, bit 1 is for chip 1 on side 0, ..., and bit 11 is for chip 5 on side 1
}

unsigned int SCT_ByteStreamErrorsTool::abcdErrorChips(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return abcdErrorChips(moduleId, ctx);
}

std::pair<StatusCode, unsigned int> SCT_ByteStreamErrorsTool::getErrorCodeWithCacheUpdate(const Identifier& moduleId, const EventContext& ctx,
                                                                                          std::unordered_map<size_t, unsigned int>& whereExected) const {
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool getErrorCodeWithCacheUpdate " << moduleId);
  size_t modhash =  static_cast<size_t>(moduleId.get_compact());
  auto it{whereExected.find(modhash)};
  if (it != whereExected.end()) return std::make_pair(StatusCode::SUCCESS, it->second);

  // even if there are no errors for this module at all filled
  // we want the entry of value 0 so we know we walked over it and do not need to invoke filling again
  // and and do not need to do it again

  auto *cacheEntry{getCacheEntry(ctx)};
  cacheEntry->abcdErrorChips[modhash] =  0;
  cacheEntry->tempMaskedChips[modhash] = 0;

  // the content is missing, look for actual errors
  StatusCode sc{fillData(ctx)};
  if (sc.isFailure()) {
    return std::make_pair(StatusCode::FAILURE, 0);
  }
  // handle situation when the cache does not contain desired datum after the update
  it = whereExected.find(modhash);
  if (it == whereExected.end()) {
    ATH_MSG_ERROR("After fillData in abcdErrorChips, cache does not have an infomation about the " << moduleId);
    ATH_MSG_ERROR("Likely cause is a request for for different region");
    return std::make_pair(StatusCode::FAILURE, 0);
  }
  return std::make_pair(StatusCode::SUCCESS, it->second);
}

const InDetDD::SiDetectorElement* SCT_ByteStreamErrorsTool::getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey, ctx};
  if (not condData.isValid()) return nullptr;
  return condData->getDetectorElement(waferHash);
}

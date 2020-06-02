/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ByteStreamErrorsTool.cxx
 * implementation file for tool that keeps track of errors in the bytestream
 * @author nbarlow@cern.ch
 **/

/// header file for this class.
#include "SCT_ByteStreamErrorsTool.h"

///Athena includes
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "StoreGate/ReadHandle.h"

/** Constructor */
SCT_ByteStreamErrorsTool::SCT_ByteStreamErrorsTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent)
{
}

/** Initialize */
StatusCode 
SCT_ByteStreamErrorsTool::initialize() {
  StatusCode sc = detStore()->retrieve(m_sct_id, "SCT_ID") ;
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve SCT ID helper!");
    return StatusCode::FAILURE;
  } 
  m_cntx_sct = m_sct_id->wafer_context();

  sc = m_config.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Cannot retrieve ConfigurationConditionsTool!");
    return StatusCode::FAILURE;
  } 
  
  // Read (Cond)Handle Keys
  ATH_CHECK(m_bsIDCErrContainerName.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  return sc;
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
  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool IDC Container fetched " << m_bsIDCErrContainerName.key() );
  return idcErrCont.cptr();
}

///////////////////////////////////////////////////////////////////////////
SCT_ByteStreamErrorsTool::IDCCacheEntry* SCT_ByteStreamErrorsTool::getCacheEntry(const EventContext& ctx) const {
  IDCCacheEntry* cacheEntry = m_eventCache.get( ctx );
  if( cacheEntry->needsUpdate( ctx ) ) {
    auto idcErrContPtr = getContainer( ctx );
    if ( idcErrContPtr == nullptr ) {     // missing or not, the cache needs to be reset
      cacheEntry->reset( ctx.evt(), nullptr );
    } else {
      cacheEntry->reset( ctx.evt(), idcErrContPtr->cache() );
    }
    ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool Cache for the event reset " << cacheEntry->eventId  << " with IDC container" << idcErrContPtr );
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
    std::lock_guard<std::mutex> lock{m_cacheMutex};
    ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool isGood called for " << elementIdHash );
    auto idcCachePtr = getCacheEntry(ctx)->IDCCache;
    if ( idcCachePtr == nullptr ) {
      ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool No cache! " );
      return true;
    }

    auto errorCode = idcCachePtr->retrieve(elementIdHash);
  
    for(auto badError : SCT_ByteStreamErrors::BadErrors) {
      if(errorCode == badError) {
	ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool Bad Error " << errorCode  << " for ID " << elementIdHash );
	return false;
      }
    }
  } // end of cache operations protection via m_cacheMutex, following code has own protection
  
  // If all 6 chips of a link issue ABCD errors or are bad chips or temporarily masked chips, the link is treated as bad one. 
  const Identifier wafer_id{m_sct_id->wafer_id(elementIdHash)};
  const Identifier module_id{m_sct_id->module_id(wafer_id)};
  unsigned int badChips{m_config->badChips(module_id, ctx)};
  unsigned int abcdErrorChips2{abcdErrorChips(module_id, ctx)};
  unsigned int tempMaskedChips2{tempMaskedChips(module_id, ctx)};
  const int side{m_sct_id->side(wafer_id)};
  bool allChipsBad{true};
  const int idMax{static_cast<short>(side==0 ? 6 : 12)};
  for (int id{idMax-6}; id<idMax; id++) {
    bool issueABCDError{((abcdErrorChips2 >> id) & 0x1) != 0};
    bool isBadChip{((badChips >> id) & 0x1) != 0};
    bool isTempMaskedChip{((tempMaskedChips2 >> id) & 0x1) != 0};
    allChipsBad = (issueABCDError or isBadChip or isTempMaskedChip);
    if (not allChipsBad) break;
  }
  if (allChipsBad) return false;
  
  return true;
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

  // Get strip number
  const int strip{m_sct_id->strip(stripId)};
  if (strip<0 or strip>=768) {
    // This check assumes present SCT.
    ATH_MSG_WARNING("strip number is invalid: " << strip);
    return -1;
  }

  // Conversion from strip to chip (specific for present SCT)
  int chip{strip/128}; // One ABCD chip reads 128 strips
  // Relation between chip and offline strip is determined by the swapPhiReadoutDirection method.
  // If swap is false
  //  offline strip:   0            767
  //  chip on side 0:  0  1  2  3  4  5
  //  chip on side 1: 11 10  9  8  7  6
  // If swap is true
  //  offline strip:   0            767
  //  chip on side 0:  5  4  3  2  1  0
  //  chip on side 1:  6  7  8  9 10 11
  const bool swap{siElement->swapPhiReadoutDirection()};
  const int side{m_sct_id->side(stripId)};
  if (side==0) {
    chip = swap ?  5 - chip :     chip;
  } else {
    chip = swap ? 11 - chip : 6 + chip;
  }

  return chip;
}

////////////////////////////////////////////////////////////////////////////////////////

/** The accessor method that can be used by clients to 
 * retrieve a set of IdHashes of wafers with a given type of error.
 * e.g. for monitoring plots.
 */

std::set<IdentifierHash>
SCT_ByteStreamErrorsTool::getErrorSet(int errorType, const EventContext& ctx) const {
  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool getErrorSet  " << errorType );
  std::set<IdentifierHash> result;
  if (errorType>=0 and errorType<SCT_ByteStreamErrors::NUM_ERROR_TYPES) {
    auto idcErrCont = getContainer( ctx );
    if ( idcErrCont != nullptr  ) {
      const std::vector<std::pair<size_t, uint64_t>> errorcodesforView = idcErrCont->getAll();
      for (const auto& [hashId, errCode] : errorcodesforView) {
	if (  SCT_ByteStreamErrors::hasError( errCode, static_cast<SCT_ByteStreamErrors::ErrorType>( errorType ) ) ) {
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

  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool fillData  ");

  const IDCInDetBSErrContainer* idcErrCont = getContainer( ctx );
  if ( idcErrCont == nullptr ) {
    return StatusCode::SUCCESS;
  }

  auto cacheEntry = getCacheEntry( ctx );

  /** OK, so we found the StoreGate container, now lets iterate
   * over it to populate the sets of errors owned by this Tool.
   */
  ATH_MSG_VERBOSE("SCT_ByteStreamErrorsTool size of error container is " << idcErrCont->maxSize());
  const std::vector<std::pair<size_t, uint64_t>> errorcodesforView = idcErrCont->getAll();

  for (const auto& [ hashId, errCode ] : errorcodesforView) {

    Identifier wafer_id{m_sct_id->wafer_id(hashId)};
    Identifier module_id{m_sct_id->module_id(wafer_id)};

    if ( errCode == idcErrCont->emptyValue() ) continue; // not filled == all was ok in deciding

    ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool filling event cache for module " << module_id  << " ec " << errCode );

    int side{m_sct_id->side(m_sct_id->wafer_id(hashId))};
    if ( errCode & SCT_ByteStreamErrors::ABCDErrorMask() ) {
      cacheEntry->abcdErrorChips[module_id] |= (1 << (errCode - SCT_ByteStreamErrors::ABCDError_Chip0 + side * 6));
    } else if (errCode & SCT_ByteStreamErrors::TempMaskedChipsMask()) {
      cacheEntry->tempMaskedChips[module_id] |= (1 << (errCode- SCT_ByteStreamErrors::TempMaskedChip0 + side * 6));
    }

  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////

unsigned int SCT_ByteStreamErrorsTool::tempMaskedChips(const Identifier& moduleId, const EventContext& ctx) const {
  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool tempMaskedChips  ");
  std::lock_guard<std::mutex> lock{m_cacheMutex};
  auto cacheEntry = getCacheEntry(ctx);
  if ( cacheEntry->IDCCache == nullptr ) return 0;

  auto [status, bsErrorCode] = getErrorCodeWithCacheUpdate( moduleId, ctx, cacheEntry->tempMaskedChips);
  if ( status.isFailure() ) {
    ATH_MSG_ERROR("SCT_ByteStreamErrorsTool Failure getting temp masked chip errors");
  }
  return bsErrorCode;

}

unsigned int SCT_ByteStreamErrorsTool::tempMaskedChips(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return tempMaskedChips(moduleId, ctx);
}

unsigned int SCT_ByteStreamErrorsTool::abcdErrorChips(const Identifier& moduleId, const EventContext& ctx) const {
  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool abcdErrorChips  ");
  std::lock_guard<std::mutex> lock{m_cacheMutex};
  auto cacheEntry = getCacheEntry(ctx);
  if ( cacheEntry->IDCCache == nullptr ) return 0;

  auto [status, bsErrorCode] = getErrorCodeWithCacheUpdate( moduleId, ctx, cacheEntry->abcdErrorChips);
  if ( status.isFailure() ) {
    ATH_MSG_ERROR("SCT_ByteStreamErrorsTool Failure getting ABC chip errors");
  }
  return bsErrorCode;
}

unsigned int SCT_ByteStreamErrorsTool::abcdErrorChips(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return abcdErrorChips(moduleId, ctx);
}

std::pair<StatusCode, unsigned int> SCT_ByteStreamErrorsTool::getErrorCodeWithCacheUpdate( const Identifier& moduleId, const EventContext& ctx,
											   std::map<Identifier, unsigned int>& whereExected ) const {
  ATH_MSG_VERBOSE( "SCT_ByteStreamErrorsTool getErrorCodeWithCacheUpdate  " << moduleId );
  auto it{ whereExected.find( moduleId ) };
  if ( it != whereExected.end() ) return std::make_pair(StatusCode::SUCCESS, it->second);

  // even if there are no errors for this module at all filled
  // we want the entry of value 0 so we know we walked over it and do not need to invoke filling again
  // and and do not need to do it again
  whereExected[moduleId] = 0;

  // the content is missing, look for actual errors
  StatusCode sc{fillData(ctx)};
  if (sc.isFailure()) {
    return std::make_pair(StatusCode::FAILURE, 0);
  }
  // handle situation when the cache does not contain desired datum after the update
  it = whereExected.find( moduleId );
  if ( it == whereExected.end() ) {
    ATH_MSG_ERROR("After fillData in abcdErrorChips, cache does not have an infomation about the " << moduleId);
    ATH_MSG_ERROR("Likely cause is a request for for different region");
    std::make_pair(StatusCode::FAILURE, 0);
  }
  return std::make_pair(StatusCode::SUCCESS, it->second);
}

const InDetDD::SiDetectorElement* SCT_ByteStreamErrorsTool::getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const {
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey, ctx};
  if (not condData.isValid()) return nullptr;
  return condData->getDetectorElement(waferHash);
}

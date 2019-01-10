/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ReadoutTool.h"

#include "InDetIdentifier/SCT_ID.h"

#include <algorithm>

#include "SCT_ConditionsData/SCT_ReadoutData.h"

// Constructor
SCT_ReadoutTool::SCT_ReadoutTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_sctId{nullptr},
  m_msgSvc{nullptr}
{
}

// Initialize
StatusCode SCT_ReadoutTool::initialize() {
  ATH_MSG_DEBUG("Initialize SCT_ReadoutTool");
  // Retrieve cabling
  ATH_CHECK(m_cablingTool.retrieve());
  // Retrieve SCT helper
  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));
  // Get MessageSvc for SCT_ReadoutData
  ISvcLocator* svcLocator{Gaudi::svcLocator()};
  ATH_CHECK(svcLocator->service("MessageSvc", m_msgSvc));
  return StatusCode::SUCCESS;
}

// Finalize
StatusCode SCT_ReadoutTool::finalize() {
  ATH_MSG_DEBUG("Finalize SCT_ReadoutTool");
  return StatusCode::SUCCESS;
}

StatusCode SCT_ReadoutTool::determineReadout(const int truncatedSerialNumber, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok) const {
  // Determine which chips are in the module readout from truncated serial number
  
  // Get moduleId
  const IdentifierHash& hash{m_cablingTool->getHashFromSerialNumber(truncatedSerialNumber)};
  if (not hash.is_valid()) return StatusCode::SUCCESS;
  Identifier moduleId{m_sctId->module_id(m_sctId->wafer_id(hash))};

  // Call identifier version
  return determineReadout(moduleId, chips, link0ok, link1ok);
}

StatusCode SCT_ReadoutTool::determineReadout(const Identifier& moduleId, std::vector<SCT_Chip*>& chips, bool link0ok, bool link1ok) const {
  // This determineReadout(const Identifier is the main method of this class.
  // Methods of SCT_ReadoutData are called in the following order in this method:
  // setChips
  // setLinkStatus
  // setModuleType
  // setChipMap
  // clearChipReadout
  // checkLink
  // \_ followReadoutUpstream
  //    \_ setChipIn
  // \_ setChipOut
  // \_ hasConnectedInput
  //    \_ inputChip
  // maskChipsNotInReadout
  // \_ isChipReadOut
  // printStatus
  // \_ isLinkStandard
  //
  // The following two methods are not used.
  // isEndBeingTalkedTo
  // \_ outputChip

  // Determine which chips are in the module readout from Identifier
  ATH_MSG_DEBUG("Determining Readout for module ID = " << moduleId);

  // Make sure there are 12 chips
  if (chips.size()!=12) {
    ATH_MSG_DEBUG("Readout must contain exactly 12 chips");
    return StatusCode::SUCCESS;
  }

  SCT_ReadoutData data(m_msgSvc);

  // Set the chips and sort in order of ID
  data.setChips(chips);

  // Set link status
  data.setLinkStatus(link0ok, link1ok);

  // Determine module type (as per ModuleType enum) and set mapping
  data.setModuleType(moduleId, m_sctId->barrel_ec(moduleId));
  data.setChipMap();

  // Set all chips out of readout and clear both links to start
  data.clearChipReadout();

  // Check the links
  data.checkLink(0);
  data.checkLink(1);

  // Mask chips not in readout
  data.maskChipsNotInReadout();

  data.printStatus(moduleId);

  return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_ConditionsData/SCT_ReadoutData.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "Identifier/Identifier.h"

#include <algorithm>

// Helper functions to indicate whether a barrel module is modified.
// The following modules have extra routing on one side or the other
// 20220170200653 -> 169922560 has link0 modified
// 20220170200113 -> 170801152 has link0 modified
// 20220170200941 -> 170983424 has link1 modified
// 20220330200701 -> 172556288 has link0 modified
// 20220330200637 -> 172621824 has link0 modified
// 20220170200183 -> 173268992 has link1 modified
// 20220330200606 -> 174301184 has link1 modified
// 20220330200209 -> 174342144 has link0 modified
// 20220330200505 -> 174610432 has link0 modified
// 20220330200117 -> 174962688 has link0 modified
// 20220330200693 -> 175015936 has link1 modified

static bool modified0(Identifier moduleId) {
  return ((moduleId==169922560) or (moduleId==170801152) or (moduleId==172556288) or (moduleId==172621824) or 
          (moduleId==174342144) or (moduleId==174610432) or (moduleId==174962688));
}

static bool modified1(Identifier moduleId) {
  return ((moduleId==170983424) or (moduleId==173268992) or (moduleId==174301184) or (moduleId==175015936));
}

using namespace SCT_Parameters;

// Constructor
SCT_ReadoutData::SCT_ReadoutData(IMessageSvc* msgSvc) {
  if (msgSvc) m_msg = std::make_unique<Athena::MsgStreamMember>(msgSvc, "SCT_ReadoutData");
}

void SCT_ReadoutData::setModuleType(const Identifier& moduleId, int bec) {
  // Set module type as per the ModuleType enum
  if (std::abs(bec) == 2) {
    m_type = SCT_Parameters::ENDCAP;
  } else if (modified0(moduleId)) {
    m_type = SCT_Parameters::MODIFIED_0;
  } else if (modified1(moduleId)) {
    m_type = SCT_Parameters::MODIFIED_1;
  } else {
    m_type = SCT_Parameters::BARREL;
  }
}

void SCT_ReadoutData::setChipMap() {
  // Set Chip mapping depending on module type
 
  // Clear between calls and reserve space for the 12 chips
  m_chipMap.clear();
  m_chipMap.reserve(12);

  if (m_type == BARREL) {
    m_chipMap.emplace_back(Chip1,  Chip2,  None ,  None);
    m_chipMap.emplace_back(Chip2,  Chip3,  Chip0,  Chip11);
    m_chipMap.emplace_back(Chip3,  Chip4,  Chip1,  Chip0);
    m_chipMap.emplace_back(Chip4,  Chip5,  Chip2,  Chip1);
    m_chipMap.emplace_back(Chip5,  None ,  Chip3,  Chip2);
    m_chipMap.emplace_back(None,   Chip7,  Chip4,  Chip3);
    m_chipMap.emplace_back(Chip7,  Chip8,  None,   None);
    m_chipMap.emplace_back(Chip8,  Chip9,  Chip6,  Chip5);
    m_chipMap.emplace_back(Chip9,  Chip10, Chip7,  Chip6);
    m_chipMap.emplace_back(Chip10, Chip11, Chip8,  Chip7);
    m_chipMap.emplace_back(Chip11, None,   Chip9,  Chip8);
    m_chipMap.emplace_back(None,   Chip1,  Chip10, Chip9);
  } else if (m_type == MODIFIED_0) {
    m_chipMap.emplace_back(Chip1,  Chip2,  Chip5,  None);
    m_chipMap.emplace_back(Chip2,  Chip3,  Chip0,  Chip11);
    m_chipMap.emplace_back(Chip3,  Chip4,  Chip1,  Chip0);
    m_chipMap.emplace_back(Chip4,  Chip5,  Chip2,  Chip1);
    m_chipMap.emplace_back(Chip5,  None,   Chip3,  Chip2);
    m_chipMap.emplace_back(Chip0,  Chip7,  Chip4,  Chip3);
    m_chipMap.emplace_back(Chip7,  Chip8,  None,   None);
    m_chipMap.emplace_back(Chip8,  Chip9,  Chip6,  Chip5);
    m_chipMap.emplace_back(Chip9,  Chip10, Chip7,  Chip6);
    m_chipMap.emplace_back(Chip10, Chip11, Chip8,  Chip7);
    m_chipMap.emplace_back(Chip11, None,   Chip9,  Chip8);
    m_chipMap.emplace_back(None,   Chip1,  Chip10, Chip9);
  } else if (m_type == MODIFIED_1) {
    m_chipMap.emplace_back(Chip1,  Chip2,  None,   None);
    m_chipMap.emplace_back(Chip2,  Chip3,  Chip0,  Chip11);
    m_chipMap.emplace_back(Chip3,  Chip4,  Chip1,  Chip0);
    m_chipMap.emplace_back(Chip4,  Chip5,  Chip2,  Chip1);
    m_chipMap.emplace_back(Chip5,  None ,  Chip3,  Chip2);
    m_chipMap.emplace_back(None,   Chip7,  Chip4,  Chip3);
    m_chipMap.emplace_back(Chip7,  Chip8,  Chip11, None);
    m_chipMap.emplace_back(Chip8,  Chip9,  Chip6,  Chip5);
    m_chipMap.emplace_back(Chip9,  Chip10, Chip7,  Chip6);
    m_chipMap.emplace_back(Chip10, Chip11, Chip8,  Chip7);
    m_chipMap.emplace_back(Chip11, None,   Chip9,  Chip8);
    m_chipMap.emplace_back(Chip6,  Chip1,  Chip10, Chip9);
  } else if (m_type == ENDCAP) {
    m_chipMap.emplace_back(Chip1,  Chip2,  Chip11, None);
    m_chipMap.emplace_back(Chip2,  Chip3,  Chip0,  Chip11);
    m_chipMap.emplace_back(Chip3,  Chip4,  Chip1,  Chip0);
    m_chipMap.emplace_back(Chip4,  Chip5,  Chip2,  Chip1);
    m_chipMap.emplace_back(Chip5,  None,   Chip3,  Chip2);
    m_chipMap.emplace_back(Chip6,  Chip7,  Chip4,  Chip3);
    m_chipMap.emplace_back(Chip7,  Chip8,  Chip5,  None);
    m_chipMap.emplace_back(Chip8,  Chip9,  Chip6,  Chip5);
    m_chipMap.emplace_back(Chip9,  Chip10, Chip7,  Chip6);
    m_chipMap.emplace_back(Chip10, Chip11, Chip8,  Chip7);
    m_chipMap.emplace_back(Chip11, None,   Chip9,  Chip8);
    m_chipMap.emplace_back(Chip0,  Chip1,  Chip10, Chip9);
  }
}

void SCT_ReadoutData::checkLink(int link) {
  // Follow the chips for each link to determine which chips are in the readout 
  // and if readout is sane. 

  // Follow chip from the start chip for each side
  SCT_Chip& startChip{m_chips->at(link*6)};
  bool linkSane{followReadoutUpstream(link, startChip)};

  if (not linkSane) {
    std::vector<int>& chipsOnThisLink{(link==0) ? m_chipsOnLink0 : m_chipsOnLink1};

    // Remove chips in that link from the readout
    for (const int linkItr: chipsOnThisLink) setChipOut(m_chips->at(linkItr));

    // We do not have ERROR/FAILURE if the readout is not sane as it possibly only affects one of the SCT modules
    if (m_msg) ATH_MSG_WARNING("Readout for link " << link << " not sane");
  }
}

bool SCT_ReadoutData::hasConnectedInput(const SCT_Chip& chip) const {
  // Does the chip have a correctly connected input

  // The chip must not be an end 
  if (chip.isEnd()) return false;
  
  int inChipId{inputChip(chip)};

  // The port the chip is listening on should be mapped to an input (if the chip is not an end)
  // Otherwise it'll never get to an end giving a timeout
  if (inChipId == None) {
    if (m_msg) ATH_MSG_WARNING("Chip " << chip.id() << " is not an end but port " << chip.inPort() << " is not mapped to anything");
    return false;
  }
  
  // The mapped chip should be talking on the same port as this chip is listening (if the chip is not an end)
  // Again, otherwise it'll never get to an end giving a timeout
  if (m_chips->at(inChipId).outPort()!=chip.inPort()) {
    if (m_msg) ATH_MSG_WARNING("Chip" << chip.id() << " is not an end and is listening on Port " << chip.inPort() << " but nothing is talking to it");
    return false;
  }
  return true;
}

bool SCT_ReadoutData::isEndBeingTalkedTo(const SCT_Chip& chip) const {
  // Is something talking to a chip which is configured as an end
  // (can possibly happen as chips must be talking to something)

  // Is chip actually an end
  if (not chip.isEnd()) return false;

  // Is anything trying to talk to the end.
  for (SCT_Chip& thisChip: *m_chips) {
    if (outputChip(thisChip) == chip.id()) {
      if (m_msg) ATH_MSG_WARNING("Chip " << chip.id() << " is configured as end but something is trying to talk to it");
      return true;
    }
  }
  return false;
}

void SCT_ReadoutData::maskChipsNotInReadout() {
  // Mask chip (is set mask to 0 0 0 0) if not in readout
  // If the readout of a particular link is not sane mask all chips on that link
  for (SCT_Chip& thisChip: *m_chips) {
    if (not isChipReadOut(thisChip)) {
      if (m_msg) ATH_MSG_DEBUG("Masking chip " <<  thisChip.id());
      uint32_t masked{0};
      thisChip.initializeMaskFromInts(masked, masked, masked, masked);
    }
  }
}

bool SCT_ReadoutData::followReadoutUpstream(int link, const SCT_Chip& chip, int remainingDepth) {
  // Follow the readout upstream (to input side).  Will return true if the readout is sane
  // The "error" cases are only warnings since they possibly only affect one module of the SCT
  // Have we gone though all 12 chips -> infinite loop
  if (remainingDepth < 0) {
    if (m_msg) ATH_MSG_WARNING("Infinite loop detected in readout");
    return false;
  }

  // Can the chip be a master
  if (chip.canBeMaster()) {

    if (chip.id()==link*6) {
      if (m_msg) ATH_MSG_DEBUG("Link " << link << " is " << (m_linkActive[link] ? "ENABLED" : "DISABLED"));
    }

    // Is the link enabled. If not the readout is still sane so return true
    if (not m_linkActive[link]) return true;

    // Is the chip actually configured as a master
    if (chip.isMaster()) {
      if (m_msg) ATH_MSG_DEBUG("MasterChip");
      // Chip will be set in readout below
    } else if (chip.id() == link*6) {
      // Link is active but the master position for THAT link does not contain a master 
      // This can happen if everything is readout via other link, therefore the readout is still sane.

      if (m_msg) ATH_MSG_DEBUG("Link " << link << " is enabled but chip " << chip.id() << " is not a master");

      return true;
    }
  }

  if (m_msg) ATH_MSG_DEBUG("Looking at chip " << chip.id());

  // Is a slave chip mistakenly configured as a master 
  if (chip.slaveConfiguredAsMaster()) {
    if (m_msg) ATH_MSG_WARNING("Found master chip in slave position " << chip.id());
    return false;
  }

  // The chip is in the readout (this doesn't mean the readout necessarily is sane).
  setChipIn(chip, link);

  // Is the chip configured as an end (can be master and end)
  if (chip.isEnd()) {
    if (m_msg) ATH_MSG_DEBUG("End Chip");

    // End chip is in readout and readout is sane
    return true;
  } 

  // Find the next chip if there is one connected
  if (not hasConnectedInput(chip)) return false;
  SCT_Chip& nextChip{m_chips->at(inputChip(chip))};
  return followReadoutUpstream(link, nextChip, remainingDepth-1);
}

bool SCT_ReadoutData::isLinkStandard(int link) const {
  // Is the readout for a particular link standard 
  // (i.e link active and 0-5 through link 0 or 6-11 through link 1)
  
  // First, the link must be active
  if (not m_linkActive[link]) return false;

  const std::vector<int>& chipsOnThisLink{(link==0) ? m_chipsOnLink0 : m_chipsOnLink1};

  // Then it must have six chips being readout ...
  if (chipsOnThisLink.size()!=6) return false;

  // ... in the correct order
  int ichip{link*6};
  for (int linkItr: chipsOnThisLink) {
    if (linkItr!=ichip) return false;
    ++ichip;
  }

  return true;
}

void SCT_ReadoutData::printStatus(const Identifier& moduleId) const {
  // Print status for module (a la online) and whether it is standard or not
  if (not m_msg) return;
  if (not msgLvl(MSG::DEBUG)) return;

  bool standard{isLinkStandard(0) and isLinkStandard(1)};

  msg(MSG::DEBUG) << "Readout status " <<  moduleId << ": "
                  << ((m_type == SCT_Parameters::ENDCAP) ? "ENDCAP" : "")
                  << ((m_type == SCT_Parameters::BARREL) ? "BARREL" : "")
                  << ((m_type == SCT_Parameters::MODIFIED_0) ? "MOD_0" : "")
                  << ((m_type == SCT_Parameters::MODIFIED_1) ? "MOD_1" : "");
  msg(MSG::DEBUG) << " Link0 = " << std::boolalpha << m_linkActive[0] << " (";
  
  if (m_chipsOnLink0.empty()) {
    msg(MSG::DEBUG) << "X";
  } else {
    for (unsigned int ilink0{0}; ilink0 < m_chipsOnLink0.size(); ++ilink0)  {
      msg(MSG::DEBUG) << m_chipsOnLink0.at(ilink0) << " ";
    }
  }
  
  msg(MSG::DEBUG) << ") Link1 = " << std::boolalpha << m_linkActive[1] << " (";
  
  if (m_chipsOnLink1.empty()) {
    msg(MSG::DEBUG) << "X";
  } else {
    for (unsigned int ilink1{0}; ilink1 < m_chipsOnLink1.size(); ++ilink1) {
      msg(MSG::DEBUG) << m_chipsOnLink1.at(ilink1) << " ";
    }
  }

  msg(MSG::DEBUG) << ") " << (standard ? "Standard" : "Non-standard") << endmsg;
}

void SCT_ReadoutData::setChips(std::vector<SCT_Chip>& chips) {
  // Set the chips and sort in order of ID
  m_chips = &chips;
  std::sort(m_chips->begin(), m_chips->end(), [](SCT_Chip& a, SCT_Chip& b) { return a.id() < b.id(); });
}

void SCT_ReadoutData::setLinkStatus(bool link0ok, bool link1ok) {
  // Set link status
  m_linkActive[0] = link0ok;
  m_linkActive[1] = link1ok;
}

void SCT_ReadoutData::clearChipReadout() {
  // Set all chips out of readout and clear both links to start
  m_chipInReadout.reset();
  m_chipsOnLink0.clear();
  m_chipsOnLink1.clear();
}

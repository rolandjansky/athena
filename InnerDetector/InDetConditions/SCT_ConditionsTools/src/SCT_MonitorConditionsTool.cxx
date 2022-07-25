/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorConditionsTool.cxx
 *
 * @brief Implementation file for the SCT_MonitorConditionsTool class 
 * in package SCT_ConditionsTools
 *
 * @author Kazu
 * @date 5 March 2008
 **/

#include "SCT_MonitorConditionsTool.h"
#include "SCT_DetectorElementStatus.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SCT_ReadoutGeometry/SCT_ChipUtils.h"

#include "SCT_ConditionsData/SCT_ConditionsParameters.h"

#include "InDetIdentifier/SCT_ID.h"

#include "StoreGate/ReadCondHandle.h"

#include <iterator>
#include <istream>

using std::string;

namespace {
  bool doesNotHaveNumbers(const std::string& numberString) {
    return (numberString.empty() or numberString.find_first_of("0123456789") == std::string::npos);
  }
}

/////////////////////////////////////////////////////////////////////////////

const string SCT_MonitorConditionsTool::s_separator{"-"};

SCT_MonitorConditionsTool::SCT_MonitorConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent)
{
}

///////////////////////////////////////////////////////////////////////////////////////////

StatusCode
SCT_MonitorConditionsTool::initialize() {
  if (detStore()->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    ATH_MSG_FATAL("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());

  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

StatusCode
SCT_MonitorConditionsTool::finalize() {
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

bool 
SCT_MonitorConditionsTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return ((h==InDetConditions::SCT_MODULE) or (h==InDetConditions::SCT_SIDE) or
          (h==InDetConditions::SCT_CHIP) or (h==InDetConditions::SCT_STRIP));
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  const Identifier waferId{m_pHelper->wafer_id(elementId)};
  const Identifier moduleId{m_pHelper->module_id(waferId)};
  const IdentifierHash waferHash{m_pHelper->wafer_hash(waferId)};
  const IdentifierHash moduleHash{m_pHelper->wafer_hash(moduleId)};
  const int strip{m_pHelper->strip(elementId)};

  const SCT_MonitorCondData* condData{getCondData(ctx)};
  if (condData) {
    switch (h) {
    case InDetConditions::SCT_MODULE:
      return not (condData->nBadStripsForModule(moduleHash)>=m_nhits_noisymodule);
    case InDetConditions::SCT_SIDE:
      return not (condData->nBadStripsForWafer(waferHash)>=m_nhits_noisywafer);
    case InDetConditions::SCT_CHIP:
      return not (condData->nBadStripsForChip(waferHash, strip)>=m_nhits_noisychip);
    case InDetConditions::SCT_STRIP: {
      return not condData->isBadStrip(waferHash, strip);
    }
    default:
      return true;
    }//end of switch statement
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

///////////////////////////////////////////////////////////////////////////////////

bool 
SCT_MonitorConditionsTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  return isGood(elementId, ctx, InDetConditions::SCT_SIDE);
}

//////////////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
}


void SCT_MonitorConditionsTool::getDetectorElementStatus(const EventContext& ctx, InDet::SiDetectorElementStatus &element_status, 
                                                         SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const  {
  SG::ReadCondHandle<SCT_MonitorCondData> condDataHandle{m_condKey, ctx};
  if (not condDataHandle.isValid()) {
     ATH_MSG_ERROR("Invalid cond data handle " << m_condKey.key() );
     return;
  }
  const SCT_MonitorCondData* condData{condDataHandle.cptr() };
  if (whandle) {
    whandle->addDependency (condDataHandle);
  }
  if (condData) {
     std::vector<bool> &status = element_status.getElementStatus();
     if (status.empty()) {
        status.resize(m_pHelper->wafer_hash_max(),true);
     }
     for (unsigned int hash=0; hash<status.size(); ++hash) {
        status.at(hash) = status.at(hash) && not (condData->nBadStripsForWafer(hash)>=m_nhits_noisywafer);
     }

     std::vector<InDet::ChipFlags_t> &chip_status = element_status.getElementChipStatus();
     if (chip_status.empty()) {
        constexpr InDet::ChipFlags_t all_chips_set = static_cast<InDet::ChipFlags_t>((1ul<<(SCT::N_CHIPS_PER_SIDE*SCT::N_SIDES)) - 1ul);
        static_assert( (1ul<<(SCT::N_CHIPS_PER_SIDE*SCT::N_SIDES)) - 1ul <= std::numeric_limits<InDet::ChipFlags_t>::max());
        chip_status.resize(m_pHelper->wafer_hash_max(),all_chips_set);
     }

     std::vector<std::vector<unsigned short> >  &bad_strips = element_status.getBadCells();
     if (bad_strips.empty()) {
        bad_strips.resize(status.size());
     }

     std::vector<std::pair<unsigned int, unsigned int> > tmp_bad_strips;
     for (unsigned int module_hash=0; module_hash<status.size(); ++module_hash) {
        IdentifierHash moduleHash(module_hash);

        std::vector<unsigned short> &bad_module_strips_out = bad_strips.at(module_hash);
        std::array<unsigned int, SCT::N_CHIPS_PER_SIDE> bad_strip_counts{};

        const std::array<std::bitset<SCT_ConditionsData::STRIPS_PER_CHIP>,
                         SCT_ConditionsData::CHIPS_PER_SIDE>
           &bad_module_strips_in = condData->badStripsForModule(moduleHash);

        unsigned int strip_i=0;
        tmp_bad_strips.clear();
        tmp_bad_strips.reserve(bad_module_strips_in.size()*SCT_ConditionsData::STRIPS_PER_CHIP);

        for (unsigned int chip_i=0; chip_i < bad_module_strips_in.size(); ++chip_i) {
           unsigned int geoemtrical_chip_id = SCT::getGeometricalChipID(strip_i);

           for (unsigned int strip_per_chip_i=0; strip_per_chip_i<bad_module_strips_in[chip_i].size(); ++strip_per_chip_i) {
              if (bad_module_strips_in[chip_i].test(strip_per_chip_i)) {
                 tmp_bad_strips.push_back(std::make_pair(geoemtrical_chip_id,strip_i));
                 ++bad_strip_counts.at(geoemtrical_chip_id);
              }
              ++strip_i;
           }
        }

        InDet::ChipFlags_t bad_chips=0;
        for (unsigned int the_chip=0; the_chip< bad_strip_counts.size(); ++the_chip) {
           bad_chips |= static_cast<InDet::ChipFlags_t >((bad_strip_counts[the_chip]>=m_nhits_noisychip) << the_chip);
        }
        chip_status[module_hash] &= ~bad_chips;

        for (const std::pair<unsigned int, unsigned int> &chip_and_strip : tmp_bad_strips) {
           unsigned int strip_i=chip_and_strip.second;
           std::vector<unsigned short>::const_iterator iter = std::lower_bound(bad_module_strips_out.begin(),bad_module_strips_out.end(),strip_i);
           if (iter == bad_module_strips_out.end() || *iter != strip_i) {
              bad_module_strips_out.insert( iter, strip_i);
           }
        }
     }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsTool::badStrips(std::set<Identifier>& strips, const EventContext& ctx) const {
  // Set of bad strip Identifers for all modules
  SCT_ID::const_id_iterator waferItr{m_pHelper->wafer_begin()}, waferEnd{m_pHelper->wafer_end()};
  // Loop over modules (side-0 of wafers)
  for (; waferItr != waferEnd; ++waferItr) {
    if (m_pHelper->side(*waferItr) != 0) continue;
    Identifier moduleId{m_pHelper->module_id(*waferItr)};
    badStrips(moduleId, strips, ctx);
  }
}

void
SCT_MonitorConditionsTool::badStrips(std::set<Identifier>& strips) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  badStrips(strips, ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsTool::badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx) const {
  // Set of bad strip Identifers for a given module
  // Get defect string and check it is sensible, i.e. non-empty and contains numbers
  std::string defectStr{getList(moduleId, ctx)};
  if (doesNotHaveNumbers(defectStr)) return;

  // Expand the string
  std::set<int> defectList;
  expandList(defectStr, defectList);

  // Convert strip number to Identifier and add to list
  std::set<int>::const_iterator defectItr{defectList.begin()}, defectEnd{defectList.end()};
  for (; defectItr!=defectEnd; ++defectItr) {
    // Strips in the DB are numbered 0-767 (side 0) and 768-1535 (side 1).  
    // This is different from the usual online/offline numbering schemes
    int side{(*defectItr) > SCT_ConditionsData::STRIPS_PER_WAFER-1 ? 1 : 0};
    int stripNum{side==1 ? (*defectItr) - SCT_ConditionsData::STRIPS_PER_WAFER : (*defectItr)};
   
    Identifier stripId{m_pHelper->strip_id(m_pHelper->barrel_ec(moduleId),  m_pHelper->layer_disk(moduleId),
                                           m_pHelper->phi_module(moduleId), m_pHelper->eta_module(moduleId),
                                           side, stripNum)};

    ATH_MSG_DEBUG("Bad Strip: Strip number in DB = " << *defectItr<< ", side/offline strip number = " << side << "/" << stripNum<< ", Identifier = " << stripId);

    strips.insert(stripId);
  }
}

void
SCT_MonitorConditionsTool::badStrips(const Identifier& moduleId, std::set<Identifier>& strips) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badStrips(moduleId, strips, ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////

std::string 
SCT_MonitorConditionsTool::badStripsAsString(const Identifier& moduleId, const EventContext& ctx) const {
  return getList(moduleId, ctx);
}

std::string
SCT_MonitorConditionsTool::badStripsAsString(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badStripsAsString(moduleId, ctx);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Local stuff
//////////////////////////////////////////////////////////////////////////////////////////

std::string
SCT_MonitorConditionsTool::getList(const Identifier& moduleId, const EventContext& ctx) const {
  string currentDefectList{""};
  const SCT_MonitorCondData* condData{getCondData(ctx)};
  if (condData) {
    const IdentifierHash moduleHash{m_pHelper->wafer_hash(moduleId)};
    condData->find(moduleHash, currentDefectList);
  } else {
    ATH_MSG_ERROR("In getList - no data");
  }
  return currentDefectList;
}

///////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsTool::expandRange(const std::string& rangeStr, std::set<int>& rangeList) {
  // Expand a given defect range
  // Find separator
  std::string::size_type sepPos{rangeStr.find(s_separator)};
  // Check if it's a range
  if (sepPos != std::string::npos) {
    // Extract min and max
    std::string::size_type len1{sepPos++}, len2{rangeStr.size()-sepPos};
    int min{std::stoi(rangeStr.substr(0,len1))};
    int max{std::stoi(rangeStr.substr(sepPos,len2))};
    // Add all strips in range to list
    while (min != (max+1)) rangeList.insert(min++);
  } else { 
    // Assume single number
    rangeList.insert(std::stoi(rangeStr));
  }  
}

///////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsTool::expandList(const std::string& defectStr, std::set<int>& defectList) {
  // Expand a given defect list

  // Empty list or one without numbers
  if (doesNotHaveNumbers(defectStr)) return;

  std::istringstream is{defectStr};
  std::istream_iterator<std::string> defectItr{is};
  std::istream_iterator<std::string> defectEnd;     //relies on default constructor to produce eof

  // Loop over (space-seperated) defects and add to list
  for (; defectItr != defectEnd; ++defectItr) expandRange(*defectItr, defectList);
}

///////////////////////////////////////////////////////////////////////////////////

const SCT_MonitorCondData*
SCT_MonitorConditionsTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<SCT_MonitorCondData> condData{m_condKey, ctx};
  return condData.retrieve();
}

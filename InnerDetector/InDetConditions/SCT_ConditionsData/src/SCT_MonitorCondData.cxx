/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//----------------------------------------------------------------------
// Implementation file for the data object class for SCT_MonitorConditionsSvc
//----------------------------------------------------------------------

#include "SCT_ConditionsData/SCT_MonitorCondData.h"

#include <iterator>
#include <utility>

namespace {
  bool doesNotHaveNumbers(const std::string& numberString) {
    return (numberString.empty() or numberString.find_first_of("0123456789") == std::string::npos);
  }
}

//----------------------------------------------------------------------
// Constructor
SCT_MonitorCondData::SCT_MonitorCondData()
{
  clear();
}

//----------------------------------------------------------------------
// Check if a module has a defect (a list of bad strips). If it does not have defect return false.
bool SCT_MonitorCondData::find(const IdentifierHash& hash, std::string& defectList) const
{
  std::size_t moduleIndex{static_cast<std::size_t>(hash/SCT_ConditionsData::SIDES_PER_MODULE)};
  std::string result{m_defectListArray[moduleIndex]};
  if (result.empty()) return false;
  defectList = result;
  return true;
}

//----------------------------------------------------------------------
// Insert a new defect (a list of bad strips) for a module
void SCT_MonitorCondData::insert(const IdentifierHash& hash, const std::string& defectList)
{
  std::size_t moduleIndex{static_cast<std::size_t>(hash/SCT_ConditionsData::SIDES_PER_MODULE)};
  m_defectListArray[moduleIndex] = defectList;

  // Set all strips, chips, wafers and module itself of this module good.
  clearModule(moduleIndex);

  if (doesNotHaveNumbers(defectList)) return;

  // Fill bad strips
  std::istringstream is{defectList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for (; readString != endOfString; ++readString) {
    const std::string& stringRange{*readString};
    std::string::size_type p{stringRange.find(s_separator)};
    if (p!=std::string::npos) { //its a range
      std::string::size_type len1{p++};
      std::string::size_type len2{stringRange.size()-p};
      int min{std::stoi(stringRange.substr(0, len1))};
      int max{std::stoi(stringRange.substr(p, len2))};
      for (int strip{min}; strip<=max; strip++) {
        fillBadStrip(moduleIndex, strip);
      }
    } else { //assume its a single number
      int strip{std::stoi(*readString)};
      fillBadStrip(moduleIndex, strip);
    }
  }

  // Fill the number of defects in chips, wafers, module
  for (std::size_t waferIndex{0}; waferIndex<SCT_ConditionsData::SIDES_PER_MODULE; waferIndex++) {
    for (std::size_t chipIndex{0}; chipIndex<SCT_ConditionsData::CHIPS_PER_SIDE; chipIndex++) {
      m_badChipArray[moduleIndex][waferIndex][chipIndex] = m_badStripArray[moduleIndex][waferIndex][chipIndex].count();
      m_badWaferArray[moduleIndex][waferIndex] += m_badChipArray[moduleIndex][waferIndex][chipIndex];
    }
    m_badModuleArray[moduleIndex] += m_badWaferArray[moduleIndex][waferIndex];
  }
}

//----------------------------------------------------------------------
// Clear m_defectListMap
void SCT_MonitorCondData::clear()
{
  m_defectListArray.fill("");

  for (std::size_t moduleIndex{0}; moduleIndex<SCT_ConditionsData::NUMBER_OF_MODULES; moduleIndex++) {
    clearModule(moduleIndex);
  }
}

std::size_t SCT_MonitorCondData::nBadStripsForModule(const IdentifierHash& moduleHash) const
{
  std::size_t moduleIndex{static_cast<std::size_t>(moduleHash/SCT_ConditionsData::SIDES_PER_MODULE)};
  return m_badModuleArray[moduleIndex];
}

std::size_t SCT_MonitorCondData::nBadStripsForWafer(const IdentifierHash& waferHash) const
{
  std::size_t moduleIndex{static_cast<std::size_t>(waferHash/SCT_ConditionsData::SIDES_PER_MODULE)};
  std::size_t waferIndex{static_cast<std::size_t>(waferHash%SCT_ConditionsData::SIDES_PER_MODULE)};
  return m_badWaferArray[moduleIndex][waferIndex];
}

std::size_t SCT_MonitorCondData::nBadStripsForChip(const IdentifierHash& waferHash, const int strip) const
{
  // 0 <= strip < 768
  if (strip<0) return false;
  if (strip>=SCT_ConditionsData::STRIPS_PER_WAFER) return false;

  // 0 to 4087
  std::size_t moduleIndex{static_cast<std::size_t>(waferHash/SCT_ConditionsData::SIDES_PER_MODULE)};
  // 0 to 1
  std::size_t waferIndex{static_cast<std::size_t>(waferHash%SCT_ConditionsData::SIDES_PER_MODULE)};
  // 0 to 5
  std::size_t chipIndex{static_cast<std::size_t>(strip/SCT_ConditionsData::STRIPS_PER_CHIP)};

  return m_badChipArray[moduleIndex][waferIndex][chipIndex];
}

bool SCT_MonitorCondData::isBadStrip(const IdentifierHash& waferHash, const int strip) const
{
  // 0 <= strip < 768
  if (strip<0) return false;
  if (strip>=SCT_ConditionsData::STRIPS_PER_WAFER) return false;
  
  // 0 to 4087
  std::size_t moduleIndex{static_cast<std::size_t>(waferHash/SCT_ConditionsData::SIDES_PER_MODULE)};
  // 0 to 1
  std::size_t waferIndex{static_cast<std::size_t>(waferHash%SCT_ConditionsData::SIDES_PER_MODULE)};
  // 0 to 5
  std::size_t chipIndex{static_cast<std::size_t>(strip/SCT_ConditionsData::STRIPS_PER_CHIP)};
  // 0 to 127
  std::size_t stripIndex{static_cast<std::size_t>(strip%SCT_ConditionsData::STRIPS_PER_CHIP)};

  return m_badStripArray[moduleIndex][waferIndex][chipIndex].test(stripIndex);
}

void SCT_MonitorCondData::clearModule(const std::size_t moduleIndex) {
  // std::array is not automatically initialized.
  for (std::size_t waferIndex{0}; waferIndex<SCT_ConditionsData::SIDES_PER_MODULE; waferIndex++) {
    for (std::size_t chipIndex{0}; chipIndex<SCT_ConditionsData::CHIPS_PER_SIDE; chipIndex++) {
      m_badStripArray[moduleIndex][waferIndex][chipIndex].reset();
      m_badChipArray[moduleIndex][waferIndex][chipIndex] = 0;
    }
    m_badWaferArray[moduleIndex][waferIndex] = 0;
  }
  m_badModuleArray[moduleIndex] = 0;
}

void SCT_MonitorCondData::fillBadStrip(const std::size_t moduleIndex, const int strip) {
  // 0 <= strip < 1536
  if (strip<0) return;
  if (strip>=SCT_ConditionsData::SIDES_PER_MODULE*SCT_ConditionsData::STRIPS_PER_WAFER) return;

  // 0 or 1
  std::size_t waferIndex{static_cast<std::size_t>(strip/SCT_ConditionsData::STRIPS_PER_WAFER)};
  // 0 to 5
  std::size_t chipIndex{static_cast<std::size_t>(strip/SCT_ConditionsData::STRIPS_PER_CHIP)%SCT_ConditionsData::CHIPS_PER_SIDE};
  // 0 to 127
  std::size_t stripIndex{static_cast<std::size_t>(strip%SCT_ConditionsData::STRIPS_PER_CHIP)};
  // Set the strip as bad
  m_badStripArray[moduleIndex][waferIndex][chipIndex].set(stripIndex);
}

const std::string SCT_MonitorCondData::s_separator{"-"};

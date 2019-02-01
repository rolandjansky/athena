/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

string SCT_MonitorConditionsTool::s_separator{string("-")};

SCT_MonitorConditionsTool::SCT_MonitorConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_nhits_noisychip{64},
  m_nhits_noisywafer{384},
  m_nhits_noisymodule{768},
  m_pHelper{nullptr},
  m_mutex{},
  m_cache{},
  m_condData{}
{
  declareProperty("Nnoisychip",    m_nhits_noisychip);
  declareProperty("Nnoisywafer",   m_nhits_noisywafer);
  declareProperty("Nnoisycmodule", m_nhits_noisymodule);
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
  Identifier waferid{m_pHelper->wafer_id(elementId)};
  Identifier iimodule{m_pHelper->module_id(waferid)};
  // defectlist is based on each module
  std::string defectlist{getList(iimodule, ctx)};

  if (not defectlist.empty()) {
    switch (h) {
    case InDetConditions::SCT_MODULE:
      return (!moduleIsNoisy(defectlist));
    case InDetConditions::SCT_SIDE:
      return (!waferIsNoisy(computeIstrip4moncond(elementId), defectlist));
    case InDetConditions::SCT_CHIP:
      return (!chipIsNoisy(computeIstrip4moncond(elementId), defectlist));
    case InDetConditions::SCT_STRIP:
      return (!stripIsNoisy(computeIstrip4moncond(elementId), defectlist));
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
  return isGood(elementId, ctx);
}

//////////////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(hashId, ctx);
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
    // Strips in the DB are numbered 0-767 (side 0) and 768-1536 (side 1).  
    // This is different from the usual online/offline numbering schemes
    int side{(*defectItr) > 767 ? 1 : 0};
    int stripNum{side==1 ? (*defectItr) - 768 : (*defectItr)};
   
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

bool
SCT_MonitorConditionsTool::stripIsNoisy(const int strip, const std::string& defectList) const {
  return inList(strip, defectList);
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::chipIsNoisy(int strip, const std::string& defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisychip;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::waferIsNoisy(const int strip, const std::string& defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisywafer;
}

///////////////////////////////////////////////////////////////////////////////////

bool SCT_MonitorConditionsTool::moduleIsNoisy(const std::string& defectList) const {
  return nDefect(defectList)>=m_nhits_noisymodule;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsTool::inRange(const int theNumber, const std::string& stringRange) const {
  std::string::size_type p{stringRange.find(s_separator)};
  if (p != std::string::npos) { //its a range
    std::string::size_type len1{p++}, len2{stringRange.size()-p};
    int min{std::stoi(stringRange.substr(0,len1))};
    int max{std::stoi(stringRange.substr(p,len2))};
    return inRange(theNumber, min, max);
  } else { //assume its a single number
    return std::stoi(stringRange) == theNumber;
  }
}

///////////////////////////////////////////////////////////////////////////////////
  
bool 
SCT_MonitorConditionsTool::inList(const int theNumber, const std::string& theList) const {
  if (doesNotHaveNumbers(theList)) return false;
  
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  bool answer{false};
  for (;readString != endOfString; ++readString) {
    answer = inRange(theNumber, *readString);
    if (answer) break;
  }

  return answer;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsTool::nBlock(const int theNumber, const std::string& stringRange) const {
  const int one{1};
  int ndefect{0};
  std::string::size_type p{stringRange.find(s_separator)};
  if (p!=std::string::npos) { //its a range
    std::string::size_type len1{p++}, len2{stringRange.size()-p};
    int min{std::stoi(stringRange.substr(0,len1))};
    int max{std::stoi(stringRange.substr(p,len2))};
    if (inRange(theNumber, min, max))
      ndefect = max-min+one;
  } else { //assume its a single number
    if (std::stoi(stringRange) == theNumber) ndefect = 0;
  }
  return ndefect;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsTool::nDefect(const int theNumber, const std::string& theList) const {
  int sum{0};
  if (doesNotHaveNumbers(theList)) return 0;
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for (;readString != endOfString; ++readString) {
    sum += nBlock(theNumber, *readString);
  }
  return sum;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsTool::nBlock(const std::string& stringRange) const {
  const int one{1};
  int ndefect{0};
  std::string::size_type p{stringRange.find(s_separator)};
  if (p!=std::string::npos) { //its a range
    std::string::size_type len1{p++}, len2{stringRange.size()-p};
    int min{std::stoi(stringRange.substr(0,len1))};
    int max{std::stoi(stringRange.substr(p,len2))};
    ndefect = max-min+one;
  } else { //assume its a single number
    ndefect = one;
  }
  return ndefect;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsTool::nDefect(const std::string& theList) const {
  int sum{0};
  if (doesNotHaveNumbers(theList)) return 0;
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for (;readString != endOfString; ++readString) {
    sum += nBlock(*readString);
  }
  return sum;
}

///////////////////////////////////////////////////////////////////////////////////

bool 
SCT_MonitorConditionsTool::inRange(const int x, const int min, const int max) const {
  return ((x >= min) and (x <= max));
}

///////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsTool::expandRange(const std::string& rangeStr, std::set<int>& rangeList) const {
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
SCT_MonitorConditionsTool::expandList(const std::string& defectStr, std::set<int>& defectList) const {
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

unsigned int
SCT_MonitorConditionsTool::computeIstrip4moncond(const Identifier& elementId) const {
  const unsigned int stripsPerSide{768};
  unsigned int iiside{static_cast<unsigned int>(m_pHelper->side(elementId))};
  unsigned int iistrip{static_cast<unsigned int>(m_pHelper->strip(elementId))};
  return stripsPerSide*iiside + iistrip;
}

///////////////////////////////////////////////////////////////////////////////////

const SCT_MonitorCondData*
SCT_MonitorConditionsTool::getCondData(const EventContext& ctx) const {
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};
  if (slot>=m_cache.size()) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_cache.resize(slot+1, invalidValue); // Store invalid values in order to go to the next IF statement.
  }
  if (m_cache[slot]!=evt) {
    std::lock_guard<std::mutex> lock{m_mutex};
    SG::ReadCondHandle<SCT_MonitorCondData> condData{m_condKey};
    if (not condData.isValid()) {
      ATH_MSG_ERROR("Failed to get " << m_condKey.key());
    }
    m_condData.set(*condData);
    m_cache[slot] = evt;
  }
  return m_condData.get();
}

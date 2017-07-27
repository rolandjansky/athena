/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MonitorConditionsSvc.cxx
 *
 * @brief Implementation file for the SCT_MonitorConditionsSvc class 
 * in package SCT_ConditionsServices
 *
 * @author Kazu
 * @date 5 March 2008
 **/

#include "SCT_MonitorConditionsSvc.h"

#include "SCT_SlhcIdConverter.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

#include <iterator>
#include <istream>

using std::string;
using SCT_ConditionsServices::castId;

namespace {
  bool doesNotHaveNumbers(const std::string & numberString) {
    return (numberString.empty() or numberString.find_first_of("0123456789") == std::string::npos);
  }
}

/////////////////////////////////////////////////////////////////////////////

string SCT_MonitorConditionsSvc::s_separator{string("-")};
string SCT_MonitorConditionsSvc::s_defectFolderName{string("/SCT/Derived/Monitoring")};

SCT_MonitorConditionsSvc::SCT_MonitorConditionsSvc(const std::string& name,ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_attrListCollection{nullptr},
  m_detStore{"DetectorStore", name},
  m_nhits_noisychip{64},
  m_nhits_noisywafer{384},
  m_nhits_noisymodule{768},
  m_pHelper{nullptr},
  m_currentDefectList{""}
{
  declareProperty("Nnoisychip",        m_nhits_noisychip);
  declareProperty("Nnoisywafer",       m_nhits_noisywafer);
  declareProperty("Nnoisycmodule",     m_nhits_noisymodule);
}

///////////////////////////////////////////////////////////////////////////////////////////

SCT_MonitorConditionsSvc::~SCT_MonitorConditionsSvc()
{}

///////////////////////////////////////////////////////////////////////////////////////////

StatusCode
SCT_MonitorConditionsSvc::initialize() {
  if(m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  }

  if(m_detStore->regFcn(&SCT_MonitorConditionsSvc::getAttrListCollection, this, m_DefectData, s_defectFolderName).isFailure()) {
    ATH_MSG_ERROR("Failed to register callback");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

StatusCode
SCT_MonitorConditionsSvc::finalize() {
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

StatusCode
SCT_MonitorConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if(ISCT_MonitorConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else if(ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

bool 
SCT_MonitorConditionsSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return ((h==InDetConditions::SCT_MODULE) or (h==InDetConditions::SCT_SIDE) or
	  (h==InDetConditions::SCT_CHIP) or (h==InDetConditions::SCT_STRIP));
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  Identifier waferid{m_pHelper->wafer_id(elementId)};
  Identifier iimodule{m_pHelper->module_id(waferid)};
  // defectlist is based on each module
  std::string defectlist{getList(iimodule)};

  if(not defectlist.empty()) {
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
SCT_MonitorConditionsSvc::isGood(const IdentifierHash& hashId) {
  //bool result(true);
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  return isGood(elementId);
}

///////////////////////////////////////////////////////////////////////////////////

StatusCode 
SCT_MonitorConditionsSvc::fillData() {
  // this is a dummy function
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::filled() const {
  //code
  return m_filled;
}

//////////////////////////////////////////////////////////////////////////////////////////

void 
SCT_MonitorConditionsSvc::badStrips(std::set<Identifier>& strips) {
  // Set of bad strip Identifers for all modules
  SCT_ID::const_id_iterator waferItr{m_pHelper->wafer_begin()}, waferEnd{m_pHelper->wafer_end()};
  // Loop over modules (side-0 of wafers)
  for(; waferItr != waferEnd; ++waferItr) {
    if(m_pHelper->side(*waferItr) != 0) continue;
    Identifier moduleId{m_pHelper->module_id(*waferItr)};
    badStrips(moduleId, strips);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////

void 
SCT_MonitorConditionsSvc::badStrips(const Identifier & moduleId, std::set<Identifier>& strips) {
  // Set of bad strip Identifers for a given module
  // Get defect string and check it is sensible, i.e. non-empty and contains numbers
  std::string defectStr{getList(moduleId)};
  if(doesNotHaveNumbers(defectStr)) return;

  // Expand the string
  std::set<int> defectList;
  expandList(defectStr, defectList);

  // Convert strip number to Identifier and add to list
  std::set<int>::const_iterator defectItr{defectList.begin()}, defectEnd{defectList.end()};
  for(; defectItr!=defectEnd; ++defectItr) {
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

//////////////////////////////////////////////////////////////////////////////////////////

std::string 
SCT_MonitorConditionsSvc::badStripsAsString(const Identifier & moduleId) {
   return getList(moduleId);
}

///////////////////////////////////////////////////////////////////////////////////////////
// Local stuff
//////////////////////////////////////////////////////////////////////////////////////////

std::string
SCT_MonitorConditionsSvc::getList(const Identifier& imodule) const {
  static const string errorstring{""};
  //to be completely robust, could use iter->index("DefectList") below (before the loop)
  if(not m_attrListCollection) {
    ATH_MSG_ERROR("In getList - no attrListCollection");
    return errorstring;
  }
  m_currentDefectList = "";
  int channelNumber{static_cast<int>(imodule.get_identifier32().get_compact())};
  std::map<const int, const std::string>::iterator it{m_defectListMap.find(channelNumber)};
  if(it!=m_defectListMap.end()) {
    m_currentDefectList = it->second;
  }
  return m_currentDefectList;

}

///////////////////////////////////////////////////////////////////////////////////

StatusCode 
SCT_MonitorConditionsSvc::getAttrListCollection(int& /*i*/ , std::list<std::string>& /*l*/) {
    // trying to find the pointer in the hashmap
    // if it exists, return it, otherwise put it in.

  m_attrListCollectionMap.erase(s_defectFolderName);
  m_defectListMap.clear();

  StatusCode sc{m_detStore->retrieve(m_attrListCollection, s_defectFolderName)};
  if(sc.isFailure()) {
    ATH_MSG_ERROR("Could not retrieve " << s_defectFolderName);
    // Using COOL, is failure
    return StatusCode::FAILURE;
  }
  m_attrListCollectionMap.insert(make_pair(s_defectFolderName, m_attrListCollection));
  static const unsigned int defectListIndex{7};
  CondAttrListCollection::const_iterator iter{m_attrListCollection->begin()};
  CondAttrListCollection::const_iterator last{m_attrListCollection->end()};
  for(; iter != last; ++iter) {
      const AthenaAttributeList& list{iter->second};
      if(list.size()) {
        m_defectListMap.insert(make_pair(iter->first,list[defectListIndex].data<std::string>()));
      }
  }
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::stripIsNoisy(const int strip, const std::string & defectList) const {
  return inList(strip, defectList);
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::chipIsNoisy(int strip, const std::string & defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisychip;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::waferIsNoisy(const int strip, const std::string & defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisywafer;
}

///////////////////////////////////////////////////////////////////////////////////

bool SCT_MonitorConditionsSvc::moduleIsNoisy(const std::string & defectList) const {
  return nDefect(defectList)>=m_nhits_noisymodule;
}

///////////////////////////////////////////////////////////////////////////////////

bool
SCT_MonitorConditionsSvc::inRange(const int theNumber, const std::string & stringRange) const {
  std::string::size_type p{stringRange.find(s_separator)};
  if(p != std::string::npos) { //its a range
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
SCT_MonitorConditionsSvc::inList(const int theNumber, const std::string& theList) const {
  if(doesNotHaveNumbers(theList)) return false;
  
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  bool answer{false};
  for(;readString != endOfString; ++readString) {
    answer = inRange(theNumber, *readString);
    if(answer) break;
  }

  return answer;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsSvc::nBlock(const int theNumber, const std::string& stringRange) const {
  const int one{1};
  int ndefect{0};
  std::string::size_type p{stringRange.find(s_separator)};
  if(p!=std::string::npos) { //its a range
    std::string::size_type len1{p++}, len2{stringRange.size()-p};
    int min{std::stoi(stringRange.substr(0,len1))};
    int max{std::stoi(stringRange.substr(p,len2))};
    if(inRange(theNumber, min, max))
      ndefect = max-min+one;
  } else { //assume its a single number
    if(std::stoi(stringRange) == theNumber) ndefect = 0;
  }
  return ndefect;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsSvc::nDefect(const int theNumber, const std::string& theList) const {
  int sum{0};
  if(doesNotHaveNumbers(theList)) return 0;
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for(;readString != endOfString; ++readString) {
    sum += nBlock(theNumber, *readString);
  }
  return sum;
}

///////////////////////////////////////////////////////////////////////////////////

int 
SCT_MonitorConditionsSvc::nBlock(const std::string& stringRange) const {
  const int one{1};
  int ndefect{0};
  std::string::size_type p{stringRange.find(s_separator)};
  if(p!=std::string::npos) { //its a range
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
SCT_MonitorConditionsSvc::nDefect(const std::string& theList) const {
  int sum{0};
  if(doesNotHaveNumbers(theList)) return 0;
  std::istringstream is{theList};
  std::istream_iterator<std::string> readString{is};
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for(;readString != endOfString; ++readString) {
    sum += nBlock(*readString);
  }
  return sum;
}

///////////////////////////////////////////////////////////////////////////////////

bool 
SCT_MonitorConditionsSvc::inRange(const int x, const int min, const int max) const {
  return ((x >= min) and (x <= max));
}

///////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsSvc::expandRange(const std::string& rangeStr, std::set<int>& rangeList) {
  // Expand a given defect range
  // Find separator
  std::string::size_type sepPos{rangeStr.find(s_separator)};
  // Check if it's a range
  if(sepPos != std::string::npos) {
    // Extract min and max
    std::string::size_type len1{sepPos++}, len2{rangeStr.size()-sepPos};
    int min{std::stoi(rangeStr.substr(0,len1))};
    int max{std::stoi(rangeStr.substr(sepPos,len2))};
    // Add all strips in range to list
    while(min != (max+1)) rangeList.insert(min++);
  } else { 
    // Assume single number
    rangeList.insert(std::stoi(rangeStr));
  }  
}

///////////////////////////////////////////////////////////////////////////////////

void
SCT_MonitorConditionsSvc::expandList(const std::string& defectStr, std::set<int>& defectList) {
  // Expand a given defect list

  // Empty list or one without numbers
  if(doesNotHaveNumbers(defectStr)) return;

  std::istringstream is{defectStr};
  std::istream_iterator<std::string> defectItr{is};
  std::istream_iterator<std::string> defectEnd;     //relies on default constructor to produce eof

  // Loop over (space-seperated) defects and add to list
  for(; defectItr != defectEnd; ++defectItr) expandRange(*defectItr, defectList);
}

///////////////////////////////////////////////////////////////////////////////////

unsigned int
SCT_MonitorConditionsSvc::computeIstrip4moncond(const Identifier& elementId) const {
  const unsigned int stripsPerSide{768};
  unsigned int iiside{static_cast<unsigned int>(m_pHelper->side(elementId))};
  unsigned int iistrip{static_cast<unsigned int>(m_pHelper->strip(elementId))};
  return stripsPerSide*iiside + iistrip;
}

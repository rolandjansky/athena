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
// IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "CoralBase/Attribute.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralBase/AttributeListSpecification.h"

#include "StoreGate/StoreGateSvc.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"

// Event Info 
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

//path resolver to find the file
#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iterator>
#include <sstream>
#include <iostream>
#include <istream>

using std::string;
using SCT_ConditionsServices::castId;
namespace{
 coral::AttributeListSpecification*
 basicAttrList(const bool camelCasedBec){
   const std::string becString=(camelCasedBec)?("BarrelEndcap"):("barrel_endcap");
   coral::AttributeListSpecification *basicSpec=new  coral::AttributeListSpecification();
   basicSpec->extend("SampleSize","int");
   basicSpec->extend(becString,"int");
   basicSpec->extend("Layer","int");
   basicSpec->extend("Eta","int");
   basicSpec->extend("Phi","int");
   return basicSpec;
 } 
  bool doesNotHaveNumbers(const std::string & numberString){
    return (numberString.empty() or numberString.find_first_of("0123456789") == std::string::npos);
  }
}
/////////////////////////////////////////////////////////////////////////////
string SCT_MonitorConditionsSvc::s_separator = string("-");
string SCT_MonitorConditionsSvc::s_defectFolderName = string("/SCT/Derived/Monitoring");
string SCT_MonitorConditionsSvc::s_deadStripFolderName = string("/SCT/Derived/DeadStrips");
string SCT_MonitorConditionsSvc::s_deadChipFolderName = string("/SCT/Derived/DeadChips");
string SCT_MonitorConditionsSvc::s_effFolderName = string("/SCT/Derived/Efficiency");
string SCT_MonitorConditionsSvc::s_noFolderName = string("/SCT/Derived/NoiseOccupancy");

SCT_MonitorConditionsSvc::SCT_MonitorConditionsSvc( const std::string& name,ISvcLocator* pSvcLocator ) :
  AthService( name, pSvcLocator ),
  m_attrListCollection(0),
  m_attrListColl(0),
  m_attrListColl_eff(0),
  m_attrListColl_no(0),
  m_detStore("DetectorStore", name),
  m_IOVDbSvc("IOVDbSvc", name),
  m_writeCondObjs(false),
  m_regIOV(false),
  m_readWriteCool(false),
  m_twoStepWriteReg(false),
  m_manualiov(false),
  m_regTime(0),
  m_version(0),
  m_nhits_noisychip(64),
  m_nhits_noisywafer(384),
  m_nhits_noisymodule(768),
  m_beginRun(IOVTime::MINRUN),
  m_endRun(IOVTime::MAXRUN),
  m_streamName("CondStreamTest"),
  m_evt(0),
  m_regSvc(0),
  m_streamer(0),
  m_filled(false),
  m_defectRecorded(false),
  m_effRecorded(false),
  m_noRecorded(false),
  m_pHelper(0),
  m_currentDefectList("")
{
  declareProperty("WriteCondObjs",     m_writeCondObjs);
  declareProperty("RegisterIOV",       m_regIOV);
  declareProperty("ReadWriteCool",     m_readWriteCool);
  declareProperty("TwoStepWriteReg",   m_twoStepWriteReg);
  declareProperty("ManualIOV",         m_manualiov);
  declareProperty("RegTime",           m_regTime);  // Register time in sec
  declareProperty("VersionN",          m_version);
  declareProperty("Nnoisychip",        m_nhits_noisychip);
  declareProperty("Nnoisywafer",       m_nhits_noisywafer);
  declareProperty("Nnoisycmodule",     m_nhits_noisymodule);
  declareProperty("BeginRun",          m_beginRun);
  declareProperty("EndRun",            m_endRun);
  declareProperty("StreamName",        m_streamName);
  declareProperty("TagID4List",        m_tagID4List);
  declareProperty("TagID4Eff",         m_tagID4Eff);
  declareProperty("TagID4NO",          m_tagID4NO);
}

///////////////////////////////////////////////////////////////////////////////////////////

SCT_MonitorConditionsSvc::~SCT_MonitorConditionsSvc()
{ }

///////////////////////////////////////////////////////////////////////////////////////////

StatusCode SCT_MonitorConditionsSvc::initialize(){
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    msg(MSG:: ERROR) << "SCT helper failed to retrieve" << endmsg;
    return StatusCode::FAILURE;
  }

  // Retrieve IOVDb service
  if (m_IOVDbSvc.retrieve().isFailure())
    return msg(MSG:: ERROR)<< "Failed to retrieve IOVDbSvc " << endmsg, StatusCode::FAILURE;

  // ------------------------------------------------------------
  // The following is requried for writing out something to COOL

  // CondAttrListCollection to store table temporarily
  m_attrListColl = new CondAttrListCollection(true);
  m_attrListColl_eff = new CondAttrListCollection(true);
  m_attrListColl_no = new CondAttrListCollection(true);

  // Get Output Stream tool for writing
  if (m_writeCondObjs) {
    IToolSvc* toolSvc = 0;// Pointer to Tool Service
    StatusCode sc = service("ToolSvc", toolSvc);
    if (sc.isFailure()) return msg(MSG:: ERROR)<< " Tool Service not found "<< endmsg, StatusCode::FAILURE;
    //different versions have different names for the tool?
    const std::string outputToolName=(m_version==0)?("AthenaOutputStreamTool"):("AthenaPoolOutputStreamTool");
    sc = toolSvc->retrieveTool(outputToolName,m_streamName, m_streamer);
    if (sc.isFailure()) return msg(MSG:: ERROR)<< "Unable to find "<<outputToolName << endmsg, StatusCode::FAILURE;
  }
    
  // Get the IOVRegistrationSvc when needed
  if (m_regIOV) {
    StatusCode sc = service("IOVRegistrationSvc", m_regSvc);
    if (sc.isFailure()){
       msg(MSG:: ERROR)<< "Unable to find IOVRegistrationSvc "<< endmsg;
       return StatusCode::FAILURE;
    }  
  }
  
  if (m_detStore->regFcn(&SCT_MonitorConditionsSvc::getAttrListCollection, this, m_DefectData, s_defectFolderName).isFailure())
    return msg(MSG:: ERROR)<< "Failed to register callback" << endmsg, StatusCode::FAILURE;
  // This should not be here and causes a SG WARNING (CBG)
  //  m_IOVDbSvc->dropObject(s_defectFolderName,false);
  
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////
StatusCode SCT_MonitorConditionsSvc::finalize(){
  if (!m_defectRecorded) delete m_attrListColl;
  if (!m_effRecorded) delete m_attrListColl_eff;
  if (!m_noRecorded) delete m_attrListColl_no;
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

StatusCode SCT_MonitorConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISCT_MonitorConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool 
SCT_MonitorConditionsSvc::canReportAbout(InDetConditions::Hierarchy h) {
  //  return h?(h==InDetConditions::SCT_SIDE):true; 
  return ( (h==InDetConditions::SCT_MODULE) || (h==InDetConditions::SCT_SIDE)|| (h==InDetConditions::SCT_CHIP) || (h==InDetConditions::SCT_STRIP) );
}

unsigned int
SCT_MonitorConditionsSvc::computeIstrip4moncond(const Identifier& elementId) const {
  const unsigned int stripsPerSide(768);
  unsigned int iiside = m_pHelper->side(elementId);
  unsigned int iistrip = m_pHelper->strip(elementId);
  return stripsPerSide*iiside + iistrip;
}

bool
SCT_MonitorConditionsSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  Identifier waferid = m_pHelper->wafer_id(elementId);
  Identifier iimodule = m_pHelper->module_id(waferid);
  // defectlist is based on each module
  std::string defectlist = getList(iimodule);

  if (!defectlist.empty()) { 
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
  } else 
    return true;
}


bool 
SCT_MonitorConditionsSvc::isGood(const IdentifierHash& hashId) {
  //bool result(true);
  Identifier elementId=m_pHelper->wafer_id(hashId);
  return isGood(elementId);
}


StatusCode 
SCT_MonitorConditionsSvc::fillData(){
  // this is a dummy function
  return StatusCode::SUCCESS;
}


bool
SCT_MonitorConditionsSvc::filled() const {
  //code
  return m_filled;
}

//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// Local stuff
//////////////////////////////////////////////////////////////////////////////////////////
string
SCT_MonitorConditionsSvc::addDefect(const string& defectlist,const int defectBeginChannel,const int defectEndChannel) const{
  // check size of defect list,
  // if it is empty then use createDefectString to make first entry. 
  if (defectlist.empty()){
    return createDefectString(defectBeginChannel,defectEndChannel);
  }
  // adding another Defect in DefectList
  std::string defect= defectlist + " " +std::to_string( defectBeginChannel);
  if (defectBeginChannel==defectEndChannel){
     defect+= " ";
  } else {
    defect+= "-" +std::to_string( defectEndChannel) + " ";
  }
  return defect;
}

///////////////////////////////////////////////////////////////////////////////////////
std::string
SCT_MonitorConditionsSvc::createDefectString(const int defectBeginChannel,const int defectEndChannel) const{
  std::string defect(" ");
  defect += std::to_string(defectBeginChannel);
  if (defectBeginChannel!=defectEndChannel) {
    defect += "-" + std::to_string(defectEndChannel);
  }
  defect += " ";
  return defect;
}
StatusCode 
SCT_MonitorConditionsSvc::setBasicListValues(coral::AttributeList & attrList0,
    const Identifier & module_id,
    const SCT_ID* sctId,
    const int samplesize,
    const bool camelCasedBec) const{
  const int eta = sctId->eta_module(module_id);
  const int phi = sctId->phi_module(module_id);
  const int barrel_ec = sctId->barrel_ec(module_id);
  const int layer = sctId->layer_disk(module_id);
  const std::string becString=(camelCasedBec)?("BarrelEndcap"):("barrel_endcap");
  //coral::AttributeList & attrList0(*pattrList);
  attrList0["SampleSize"].setValue(static_cast<int>(samplesize));
  attrList0[becString].setValue(static_cast<int>(barrel_ec));
  attrList0["Layer"].setValue(static_cast<int>(layer));
  attrList0["Phi"].setValue(static_cast<int>(phi));
  attrList0["Eta"].setValue(static_cast<int>(eta));
  //not allow any failure at this stage?
  return StatusCode::SUCCESS;
}


/////////////////////////////////////////////////////////////////////////////////////////////
StatusCode SCT_MonitorConditionsSvc::createCondObjects(const Identifier& module_id,
                   const SCT_ID* sctId,
                   const int samplesize,
                   const std::string & defectType,
                   const float threshold,
                   const std::string & defectList) const{
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}

  const bool camelCasedBec=true;
  coral::AttributeListSpecification* attrSpec =basicAttrList(camelCasedBec);
  attrSpec->extend("DefectType","string");
  attrSpec->extend("Threshold","float");
  attrSpec->extend("DefectList","string");
 
  if (!attrSpec->size()) return msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg, StatusCode::FAILURE;

  // Add attr list values
  coral::AttributeList  attrList0(*attrSpec);
  if (setBasicListValues(attrList0,module_id,sctId,samplesize,camelCasedBec).isFailure()) return StatusCode::FAILURE;
  attrList0["DefectType"].setValue(static_cast<std::string>(defectType));
  attrList0["Threshold"].setValue(static_cast<float>(threshold));
  attrList0["DefectList"].setValue(static_cast<std::string>(defectList));

  std::ostringstream attrStr2;
  attrList0.toOutputStream( attrStr2 );
  m_attrListColl->add(castId(module_id), attrList0);

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

StatusCode SCT_MonitorConditionsSvc::createListEff(const Identifier& module_id,const SCT_ID* sctId,const int samplesize, const float eff) const {
  if(!m_writeCondObjs) {return StatusCode::SUCCESS;}

  const bool camelCasedBec=false;
  coral::AttributeListSpecification* attrSpec =basicAttrList(camelCasedBec);
  attrSpec->extend("Efficiency", "float");
 
  if (!attrSpec->size()) return msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg,StatusCode::FAILURE;

  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  if (setBasicListValues(attrList0,module_id,sctId,samplesize,camelCasedBec).isFailure()) return StatusCode::FAILURE;
  attrList0["Efficiency"].setValue(static_cast<float>(eff));
  
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_eff->add(castId(module_id), attrList0);

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

StatusCode SCT_MonitorConditionsSvc::createListNO(const Identifier& module_id,const SCT_ID* sctId,const int samplesize,const float noise_occ) const {
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  const bool camelCasedBec=false;
  coral::AttributeListSpecification* attrSpec =basicAttrList(camelCasedBec);
  attrSpec->extend("NoiseOccupancy", "float");
 
  if (!attrSpec->size()) return msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg,StatusCode::FAILURE;

  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  if (setBasicListValues(attrList0,module_id,sctId,samplesize,camelCasedBec).isFailure()) return StatusCode::FAILURE;
  attrList0["NoiseOccupancy"].setValue(static_cast<float>(noise_occ));
  
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_no->add(castId(module_id), attrList0);
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

std::string
SCT_MonitorConditionsSvc::getList(const Identifier& imodule) const{
  static const string errorstring("");
  //static const string defectlistString("DefectList"); <- this should correspond to index 7 in the payload
  //static const unsigned int defectlistIndex(7); //slightly faster, but less transparent than using the string name
  //to be completely robust, could use iter->index("DefectList") below (before the loop)
  if (not m_attrListCollection) {
    msg(MSG::ERROR) << "In getList - no attrListCollection" << endmsg;
    return errorstring;
  }
  m_currentDefectList = "";
  int channelNumber(imodule.get_identifier32().get_compact());
  std::map<const int, const std::string>::iterator it(m_defectListMap.find(channelNumber));
  if (it !=  m_defectListMap.end()) {
    m_currentDefectList = it->second;
  }
  return m_currentDefectList;

}

// =============================================================================
const CondAttrListCollection*
SCT_MonitorConditionsSvc::getAttrListCollectionByFolder(const string& foldername) const {
    // trying to find the pointer in the hashmap
    // if it exists, return it, otherwise put it in.
    const CondAttrListCollection* attrListCollection = 0;
    if (m_attrListCollectionMap.count(foldername) == 0) {
        StatusCode sc = m_detStore->retrieve(attrListCollection, foldername);
        if (sc.isFailure()) {
            msg(MSG:: ERROR)<< "Could not retrieve " << foldername << endmsg;
            return 0;
        }
        m_attrListCollectionMap.insert(make_pair(foldername, attrListCollection));
    } else {
        attrListCollection = m_attrListCollectionMap[foldername];
    }
    return attrListCollection;
}

// =============================================================================
StatusCode 
SCT_MonitorConditionsSvc::getAttrListCollection(int& /*i*/ , std::list<std::string>& /*l*/) {
    // trying to find the pointer in the hashmap
    // if it exists, return it, otherwise put it in.

  m_attrListCollectionMap.erase(s_defectFolderName);
  m_defectListMap.clear();

  StatusCode sc = m_detStore->retrieve(m_attrListCollection, s_defectFolderName);
  if (sc.isFailure()) {
    msg(MSG:: ERROR)<< "Could not retrieve " << s_defectFolderName << endmsg;
    // Using COOL, is failure
    return StatusCode::FAILURE;
  }
  m_attrListCollectionMap.insert(make_pair(s_defectFolderName, m_attrListCollection));
  static const unsigned int defectListIndex(7);
  CondAttrListCollection::const_iterator iter = m_attrListCollection->begin();
  CondAttrListCollection::const_iterator last = m_attrListCollection->end();
  for (; iter != last; ++iter) {
      const AthenaAttributeList& list(iter->second);
      if (list.size()) {
        m_defectListMap.insert(make_pair(iter->first,list[defectListIndex].data<std::string>()));
      }
  }
  return StatusCode::SUCCESS;
}

//
std::string 
SCT_MonitorConditionsSvc::getDeadThingList(const Identifier & imodule, const bool thingIsStrip) const{
  static const string deadstring("");
  static const string defectlistString("DefectList");
  const string folderName=(thingIsStrip)?(s_deadStripFolderName):(s_deadChipFolderName);
  const CondAttrListCollection* attrListColl_local = getAttrListCollectionByFolder(folderName);
  if (not attrListColl_local) {
    return deadstring;
  }

    // Loop over collection
  CondAttrListCollection::const_iterator iter = attrListColl_local->begin();
  CondAttrListCollection::const_iterator last = attrListColl_local->end();
  for (; iter != last; ++iter) {
    Identifier offlineID((*iter).first);
    m_currentDefectList = "";
    if (offlineID==imodule) {
      const AthenaAttributeList& list(iter->second);
      if (list.size()) {
        try {m_currentDefectList = (*iter).second[defectlistString].data<std::string>();}
        catch (const std::exception& e) {
          // Do Nothing
        // if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getDeadStripList catch : " << e.what()<< endmsg;
        }
      }
      return m_currentDefectList;
    }
  }
  return deadstring;
}
  



// ===============================================================================
std::string
SCT_MonitorConditionsSvc::getDeadStripList(const Identifier& imodule) const{
  const bool thingIsStrip(true);
  return getDeadThingList(imodule, thingIsStrip);
}

// ===============================================================================
std::string
SCT_MonitorConditionsSvc::getDeadChipList(const Identifier& imodule) const{
  const bool thingIsStrip(false);
  return getDeadThingList(imodule,thingIsStrip);
}

// ===============================================================================
float
SCT_MonitorConditionsSvc::getEff(const Identifier& imodule,const std::string& foldername) const{
  static const string efficiencyString("Efficiency");
  const CondAttrListCollection* attrListColl_local = getAttrListCollectionByFolder(foldername);
  if (not attrListColl_local) {
    return -1.;
  }

  // Loop over collection
  CondAttrListCollection::const_iterator iter = attrListColl_local->begin();
  CondAttrListCollection::const_iterator last = attrListColl_local->end();
  for (; iter != last; ++iter) {
    Identifier offlineID(iter->first);
    if (offlineID==imodule){
      return (iter->second)[efficiencyString].data<float>();
    }
  }
  return -1.;
}

// =============================================================================
float 
SCT_MonitorConditionsSvc::getNO(const Identifier& imodule, const std::string & foldername) const {
  static const string noiseOccupancyString("NoiseOccupancy");
  const CondAttrListCollection* attrListColl_local = getAttrListCollectionByFolder(foldername);
  if (not attrListColl_local) {
    return -1.;
  }
  
  // Loop over collection
  CondAttrListCollection::const_iterator iter = attrListColl_local->begin();
  CondAttrListCollection::const_iterator last = attrListColl_local->end();
  for (; iter != last; ++iter) {
    Identifier offlineID(iter->first);
    if (offlineID==imodule){
      return (iter->second)[noiseOccupancyString].data<float>();
    }
  }
  return -1;
}


// =============================================================================

bool SCT_MonitorConditionsSvc::stripIsNoisy(const int strip, const std::string & defectList) const {
  return inList(strip, defectList);
}

// =============================================================================
bool SCT_MonitorConditionsSvc::chipIsNoisy(int strip, const std::string & defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisychip;
}

// =============================================================================
bool SCT_MonitorConditionsSvc::waferIsNoisy(const int strip, const std::string & defectList) const {
  return nDefect(strip,defectList)>=m_nhits_noisywafer;
}

// =============================================================================
bool SCT_MonitorConditionsSvc::moduleIsNoisy(const std::string & defectList) const {
  return nDefect(defectList)>=m_nhits_noisymodule;
}

// ===========================================================================

bool SCT_MonitorConditionsSvc::inRange(const int theNumber, const std::string & stringRange) const{
  std::string::size_type p = stringRange.find(s_separator);
  if (p != std::string::npos) { //its a range
    std::string::size_type len1(p++), len2(stringRange.size()-p);
    int min = std::stoi(stringRange.substr(0,len1));
    int max = std::stoi(stringRange.substr(p,len2));
    return inRange(theNumber, min, max);
  } else { //assume its a single number
    return std::stoi(stringRange) == theNumber;
  }
}

// ===========================================================================
  
bool 
SCT_MonitorConditionsSvc::inList(const int theNumber, const std::string& theList) const{
  if (doesNotHaveNumbers(theList)) return false;
  
  std::istringstream is(theList);
  std::istream_iterator<std::string> readString(is);
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  bool answer(false);
  for (;readString != endOfString; ++readString) {
    answer = inRange(theNumber, *readString);
    if ( answer ) break;
  }

  return answer;
}

// ===========================================================================

int 
SCT_MonitorConditionsSvc::nBlock(const int theNumber, const std::string& stringRange) const{
  const int one(1);
  int ndefect=0;
  std::string::size_type p=stringRange.find(s_separator);
  if (p!=std::string::npos){ //its a range
    std::string::size_type len1(p++), len2(stringRange.size()-p);
    int min=std::stoi(stringRange.substr(0,len1));
    int max=std::stoi(stringRange.substr(p,len2));
    if ( inRange(theNumber, min, max) )
      ndefect = max-min+one;
  } else { //assume its a single number
    if ( std::stoi(stringRange) == theNumber )
      ndefect = 0;
  }
  return ndefect;
}

// ===========================================================================

int 
SCT_MonitorConditionsSvc::nDefect(const int theNumber, const std::string& theList) const{
  int sum = 0;
  if (doesNotHaveNumbers(theList)) return 0;
  std::istringstream is(theList);
  std::istream_iterator<std::string> readString(is);
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for (;readString != endOfString; ++readString){
    sum += nBlock(theNumber, *readString);
  }
  return sum;
}

// ===========================================================================

int 
SCT_MonitorConditionsSvc::nBlock(const std::string& stringRange) const{
  const int one(1);
  int ndefect=0;
  std::string::size_type p = stringRange.find(s_separator);
  if (p != std::string::npos){ //its a range
    std::string::size_type len1(p++), len2(stringRange.size()-p);
    int min=std::stoi(stringRange.substr(0,len1));
    int max=std::stoi(stringRange.substr(p,len2));
    ndefect = max-min+one;
  } else { //assume its a single number
    ndefect = one;
  }
  return ndefect;
}

// ===========================================================================

int 
SCT_MonitorConditionsSvc::nDefect(const std::string& theList) const{
  int sum=0;
  if (doesNotHaveNumbers(theList)) return 0;
  std::istringstream is(theList);
  std::istream_iterator<std::string> readString(is);
  std::istream_iterator<std::string> endOfString; //relies on default constructor to produce eof
  for (;readString != endOfString; ++readString){
    sum += nBlock(*readString);
  }
  return sum;
}

// ===========================================================================

bool 
SCT_MonitorConditionsSvc::inRange(const int x, const int min, const int max) const{
  return ((x >= min) and (x <= max));
}
// ===========================================================================





StatusCode SCT_MonitorConditionsSvc::genericWrapUp(const CondAttrListCollection* theCollection, const std::string & theFolderName, const std::string & theTag){
  StatusCode sc;  
  // This need to be on finalize?  yes.
  if (m_writeCondObjs) {
    sc = m_detStore->record(theCollection,theFolderName);
    m_defectRecorded = true;
    if (sc.isFailure()) return msg(MSG:: ERROR) << "Could not record "<<theFolderName << endmsg, StatusCode::FAILURE;
 
    // Stream out and register objects here
    sc = streamOutCondObjects(theFolderName);
    if (sc.isFailure()) return msg(MSG:: ERROR) <<"Could not stream out "<<theFolderName <<endmsg, StatusCode::FAILURE;
  }

  if (m_regIOV) {
    sc = registerCondObjects(theFolderName, theTag);
    if (sc.isFailure()) return msg(MSG:: ERROR) <<"Could not register "<<theFolderName <<endmsg, StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
  
}

// ======================================================================
StatusCode 
SCT_MonitorConditionsSvc::wrapUpNoisyChannel(){
  return genericWrapUp(m_attrListColl,s_defectFolderName, m_tagID4List);
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode 
SCT_MonitorConditionsSvc::wrapUpEfficiency(){
  return genericWrapUp(m_attrListColl_eff,s_effFolderName, m_tagID4Eff);
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode 
SCT_MonitorConditionsSvc::wrapUpNoiseOccupancy(){
  return genericWrapUp(m_attrListColl_no,s_noFolderName, m_tagID4NO);
}

///////////////////////////////////////////////////////////////////////////////////////

StatusCode 
SCT_MonitorConditionsSvc::printCondObjects(const std::string& foldername) const{
  // This function seems to be reserved for possible debugging.
  // Currently nobody calls this function.
  const CondAttrListCollection* attrListColl_local = getAttrListCollectionByFolder(foldername);
  if (!m_twoStepWriteReg && m_readWriteCool) {
    if (not attrListColl_local) return StatusCode::FAILURE;
    // Loop over collection
    CondAttrListCollection::const_iterator iter = attrListColl_local->begin();
    CondAttrListCollection::const_iterator last  = attrListColl_local->end();
    for (; iter != last; ++iter) {
      std::ostringstream attrStr1;
      (*iter).second.toOutputStream(attrStr1);
    } // end of loop over collections
  }    
  return StatusCode::SUCCESS;
}

// =====================================================================
StatusCode 
SCT_MonitorConditionsSvc::streamOutCondObjects(const std::string& foldername) const{
  StatusCode sc = m_streamer->connectOutput();
  if (sc.isFailure()) {
    msg(MSG:: ERROR) <<"Could not connect stream to output" <<endmsg;
    return( StatusCode::FAILURE);
  }
    
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys(1);
  if (m_readWriteCool) {
    IAthenaOutputStreamTool::TypeKeyPair attrCollPair("CondAttrListCollection",foldername);
    typeKeys[0] = attrCollPair;
  }
    
  sc = m_streamer->streamObjects(typeKeys);
  if (sc.isFailure()) {
    msg(MSG:: ERROR) << "Could not stream out AttributeLists" << endmsg;
    return StatusCode::FAILURE;
  }
  sc = m_streamer->commitOutput();
  if (sc.isFailure()) {
    msg(MSG:: ERROR) << "Could not commit output stream" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


// ======================================================================
StatusCode 
SCT_MonitorConditionsSvc::registerCondObjects(const std::string& foldername,const std::string& tagname) {
  // Register the IOV DB with the conditions data written out
  if (m_readWriteCool) {
    // Can only write out AttrList's if this is NOT write and reg in two steps
    if (!m_twoStepWriteReg) { 
      // Using COOL, write out attrlist and collection of attrlists
      // attrlist collection 
      StatusCode sc;
      unsigned int beginRun;
      unsigned int endRun;
      if ( !m_manualiov ) {
        StoreGateSvc* pStoreGate;
        sc = service("StoreGateSvc",pStoreGate);
        if (sc.isFailure()) {
          msg(MSG:: FATAL) << "StoreGate service not found !" << endmsg;
          return StatusCode::FAILURE;
        }
        sc = pStoreGate->retrieve(m_evt);
        if (sc.isFailure()) {
          msg(MSG:: ERROR) << "Unable to get the EventSvc" << endmsg;
          return sc;
        }
        beginRun = m_evt->event_ID()->run_number();
        endRun = beginRun;
      } else {
        beginRun = m_beginRun;
        endRun = m_endRun;
      }
      unsigned int beginLB = IOVTime::MINEVENT;
      unsigned int endLB = IOVTime::MAXEVENT;
      if (not tagname.empty()) {
        sc = m_regSvc->registerIOV("CondAttrListCollection",foldername,tagname,beginRun, endRun,beginLB, endLB);
      } else {
        sc = m_regSvc->registerIOV("CondAttrListCollection",foldername,"",beginRun, endRun,beginLB, endLB);
      }
      if (sc.isFailure()) {
        msg(MSG:: ERROR) <<"Could not register in IOV DB for CondAttrListCollection" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
std::string 
SCT_MonitorConditionsSvc::deadStrip_module(const Identifier& id) const {
  Identifier waferid = m_pHelper->wafer_id(id);
  Identifier imodule = m_pHelper->module_id(waferid);
  std::string defectlist = getDeadStripList(imodule);
  return defectlist;
}

//////////////////////////////////////////////////////////////////////////////

std::string 
SCT_MonitorConditionsSvc::deadChip_module(const Identifier& id) const {
  Identifier waferid = m_pHelper->wafer_id(id);
  Identifier imodule = m_pHelper->module_id(waferid);
  std::string defectlist = getDeadChipList(imodule);
  return defectlist;
}

//////////////////////////////////////////////////////////////////////////////

float 
SCT_MonitorConditionsSvc::eff_module(const Identifier& id) const {
  Identifier waferid = m_pHelper->wafer_id(id);
  Identifier imodule = m_pHelper->module_id(waferid);
  float efficiency = getEff(imodule, s_effFolderName);
  return efficiency;
}

//////////////////////////////////////////////////////////////////////////////

float 
SCT_MonitorConditionsSvc::no_module(const Identifier& id) const{
  Identifier waferid = m_pHelper->wafer_id(id);
  Identifier imodule = m_pHelper->module_id(waferid);
  float noise_occ = getNO(imodule, s_noFolderName);
  return noise_occ;
}

//////////////////////////////////////////////////////////////////////////////

std::string 
SCT_MonitorConditionsSvc::badStripsAsString(const Identifier & moduleId) {
   return getList(moduleId);
}

void 
SCT_MonitorConditionsSvc::badStrips(std::set<Identifier>& strips) {
  // Set of bad strip Identifers for all modules
  SCT_ID::const_id_iterator waferItr(m_pHelper->wafer_begin()), waferEnd(m_pHelper->wafer_end());
  // Loop over modules (side-0 of wafers)
  for(; waferItr != waferEnd; ++waferItr){
    if (m_pHelper->side(*waferItr) != 0) continue;
    Identifier moduleId = m_pHelper->module_id(*waferItr);
    badStrips(moduleId, strips);
  }
}

void 
SCT_MonitorConditionsSvc::badStrips(const Identifier & moduleId, std::set<Identifier>& strips) {
  // Set of bad strip Identifers for a given module
  // Get defect string and check it is sensible, i.e. non-empty and contains numbers
  std::string defectStr = getList(moduleId);
  if (doesNotHaveNumbers(defectStr)) return;

  // Expand the string
  std::set<int> defectList;
  expandList(defectStr, defectList);

  // Convert strip number to Identifier and add to list
  std::set<int>::const_iterator defectItr(defectList.begin()), defectEnd(defectList.end());
  for (; defectItr != defectEnd; ++defectItr) {
    // Strips in the DB are numbered 0-767 (side 0) and 768-1536 (side 1).  
    // This is different from the usual online/offline numbering schemes
    int side     = ((*defectItr) > 767 ? 1 : 0); 
    int stripNum = (side==1 ? (*defectItr) - 768 : (*defectItr));
   
    Identifier stripId = m_pHelper->strip_id(m_pHelper->barrel_ec(moduleId),  m_pHelper->layer_disk(moduleId),
               m_pHelper->phi_module(moduleId), m_pHelper->eta_module(moduleId),
               side, stripNum);

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Bad Strip: Strip number in DB = " << *defectItr<< ", side/offline strip number = " << side << "/" << stripNum<< ", Identifier = " << stripId << endmsg;

    strips.insert(stripId);
  }
}

void SCT_MonitorConditionsSvc::expandList(const std::string& defectStr, std::set<int>& defectList) {
  // Expand a given defect list

  // Empty list or one without numbers
  if (doesNotHaveNumbers(defectStr)) return;

  std::istringstream is(defectStr);
  std::istream_iterator<std::string> defectItr(is);
  std::istream_iterator<std::string> defectEnd;     //relies on default constructor to produce eof

  // Loop over (space-seperated) defects and add to list
  for (; defectItr != defectEnd; ++defectItr) expandRange(*defectItr, defectList);
}

void SCT_MonitorConditionsSvc::expandRange(const std::string& rangeStr, std::set<int>& rangeList) {
  // Expand a given defect range
  // Find separator
  std::string::size_type sepPos(rangeStr.find(s_separator));
  // Check if it's a range
  if (sepPos != std::string::npos) { 
    // Extract min and max
    std::string::size_type len1(sepPos++), len2(rangeStr.size()-sepPos);
    int min = std::stoi(rangeStr.substr(0,len1));
    int max = std::stoi(rangeStr.substr(sepPos,len2));
    // Add all strips in range to list
    while (min != (max+1)) rangeList.insert(min++);
  } else { 
    // Assume single number
    rangeList.insert(std::stoi(rangeStr));
  }  
}

//////////////////////////////////////////////////////////////////////////////
std::string SCT_MonitorConditionsSvc::deadStripsAsString(const Identifier & moduleId) {
  return getDeadStripList(moduleId);
}

//////////////////////////////////////////////////////////////////////////////
std::string SCT_MonitorConditionsSvc::deadChipsAsString(const Identifier & moduleId) {
  return getDeadChipList(moduleId);
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCTCalibWriteSvc.cxx
 *
 * @brief Implementation file for uploading to DB on CAF
 *
 * @author Jose E. Garcia
 **/
#include "SCT_CalibAlgs/SCTCalibWriteSvc.h"

// IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"

// Gaudi includes
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/IToolSvc.h"

// Athena includes
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "CoralBase/Attribute.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
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
#include <boost/lexical_cast.hpp>

using std::string;
/////////////////////////////////////////////////////////////////////////////
string SCTCalibWriteSvc::s_separator = string("-");
string SCTCalibWriteSvc::s_defectFolderName = string("/SCT/Derived/Monitoring");
string SCTCalibWriteSvc::s_deadStripFolderName = string("/SCT/Derived/DeadStrips");
string SCTCalibWriteSvc::s_deadChipFolderName = string("/SCT/Derived/DeadChips");
string SCTCalibWriteSvc::s_effFolderName = string("/SCT/Derived/Efficiency");
string SCTCalibWriteSvc::s_noFolderName = string("/SCT/Derived/NoiseOccupancy");
string SCTCalibWriteSvc::s_RawOccuFolderName = string("/SCT/Derived/RawOccupancy");
string SCTCalibWriteSvc::s_BSErrFolderName = string("/SCT/Derived/BSErrorsRun2");
string SCTCalibWriteSvc::s_LAFolderName = string("/SCT/Derived/LorentzAngleRun2_v2");

static bool becCapsFormat(true);
static bool becUnderscoreFormat(false);

SCTCalibWriteSvc::SCTCalibWriteSvc(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_detStore(0),
  
  m_attrListColl(0),
  m_attrListColl_deadStrip(0),
  m_attrListColl_deadChip(0),
  m_attrListColl_eff(0),
  m_attrListColl_no(0),
  m_attrListColl_RawOccu(0),
  m_attrListColl_BSErr(0),
  m_attrListColl_LA(0),
  //boolean properties
  m_writeCondObjs(true),
  m_regIOV(true),
  m_readWriteCool(true),
  m_twoStepWriteReg(false),
  m_manualiov(true),
  //
  /*m_regTime(0), never used */
  m_version(0),
  m_beginRun(IOVTime::MINRUN),
  m_endRun(IOVTime::MAXRUN),
  m_streamName("CondStreamTest"),
  
  m_evt(0),
  m_regSvc(0),
  //m_streamer(0),
  m_streamer(((m_version == 0) ? "AthenaOutputStreamTool" : "AthenaPoolOutputStreamTool"), this),
  m_badIds(),
  
  m_defectRecorded(false),
  m_deadStripRecorded(false),
  m_deadChipRecorded(false),
  m_effRecorded(false),
  m_noRecorded(false),
  m_RawOccuRecorded(false),
  m_BSErrRecorded(false),
  m_LARecorded(false),
  m_pHelper(0),
  m_currentDefectList("") 
{
  declareProperty("WriteCondObjs",        m_writeCondObjs);
  declareProperty("RegisterIOV",          m_regIOV);
  declareProperty("ReadWriteCool",        m_readWriteCool);
  declareProperty("TwoStepWriteReg",      m_twoStepWriteReg);
  declareProperty("ManualIOV",            m_manualiov);
  //declareProperty("RegTime",              m_regTime);  never used Register time in sec
  
  declareProperty("BeginRun",             m_beginRun);
  declareProperty("EndRun",               m_endRun);
  //string properties:
  declareProperty("StreamName",           m_streamName);
  declareProperty("TagID4NoisyStrips",    m_tagID4NoisyStrips);
  declareProperty("TagID4DeadStrips",     m_tagID4DeadStrips);
  declareProperty("TagID4DeadChips",      m_tagID4DeadChips);
  declareProperty("TagID4Efficiency",     m_tagID4Efficiency);
  declareProperty("TagID4NoiseOccupancy", m_tagID4NoiseOccupancy);
  declareProperty("TagID4RawOccupancy",   m_tagID4RawOccupancy);
  declareProperty("TagID4BSErrors",       m_tagID4BSErrors);
  declareProperty("TagID4LorentzAngle",   m_tagID4LorentzAngle);

}

///////////////////////////////////////////////////////////////////////////////////////////

SCTCalibWriteSvc::~SCTCalibWriteSvc()
{ }

///////////////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::initialize(){ 
  StatusCode sc = service("DetectorStore", m_detStore);
  if ( !sc.isSuccess() || 0 == m_detStore) return msg(MSG:: ERROR) << "Could not find DetStore" << endmsg, StatusCode::FAILURE;
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) return msg(MSG:: ERROR) << "SCT mgr failed to retrieve" << endmsg, StatusCode::FAILURE;
  
  // ------------------------------------------------------------
  // The following is required for writing out something to COOL

  // CondAttrListCollection to store table temporarily
  m_attrListColl = new CondAttrListCollection(true);
  m_attrListColl_deadStrip = new CondAttrListCollection(true);
  m_attrListColl_deadChip = new CondAttrListCollection(true);
  m_attrListColl_eff = new CondAttrListCollection(true);
  m_attrListColl_no = new CondAttrListCollection(true);
  m_attrListColl_RawOccu = new CondAttrListCollection(true);
  m_attrListColl_BSErr = new CondAttrListCollection(true);
  m_attrListColl_LA = new CondAttrListCollection(true);

  // Get Output Stream tool for writing
  /* if (m_writeCondObjs) {
    IToolSvc* toolSvc(0);// Pointer to Tool Service
    if (service("ToolSvc", toolSvc).isFailure()) return msg(MSG:: ERROR)<< " Tool Service not found "<< endmsg,StatusCode::FAILURE;
    if (m_version == 0) {
      sc = toolSvc->retrieveTool("AthenaOutputStreamTool",m_streamName, m_streamer);
    } else {
      sc = toolSvc->retrieveTool("AthenaPoolOutputStreamTool",m_streamName, m_streamer);
    }
    if (sc.isFailure()) return msg(MSG:: ERROR)<< "Unable to find Athena(Pool)OutputStreamTool" << endmsg, StatusCode::FAILURE;
  } */
    
  // Get the IOVRegistrationSvc when needed
  if (m_regIOV) {
    if (service("IOVRegistrationSvc", m_regSvc).isFailure()) return msg(MSG:: ERROR)<< "Unable to find IOVRegistrationSvc "<< endmsg, StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::finalize(){
  if (!m_defectRecorded) delete m_attrListColl;
  if (!m_deadStripRecorded) delete m_attrListColl_deadStrip;
  if (!m_deadChipRecorded) delete m_attrListColl_deadChip;
  if (!m_effRecorded) delete m_attrListColl_eff;
  if (!m_noRecorded) delete m_attrListColl_no;
  if (!m_RawOccuRecorded) delete m_attrListColl_RawOccu;
  if (!m_BSErrRecorded) delete m_attrListColl_BSErr;
  if (!m_LARecorded) delete m_attrListColl_LA;
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if(SCTCalibWriteSvc::interfaceID().versionMatch(riid) ){
      *ppvIF = (SCTCalibWriteSvc*)this; 
    } else{ 
      return AthService::queryInterface(riid, ppvIF); 
    }
  return StatusCode::SUCCESS; 
}

//////////////////////////////////////////////////////////////////////////////////////////

unsigned int
SCTCalibWriteSvc::computeIstrip4moncond(const Identifier& elementId) const {
    unsigned int iiside = m_pHelper->side(elementId);
    unsigned int iistrip = m_pHelper->strip(elementId);
    return 768*iiside + iistrip;
}

StatusCode 
SCTCalibWriteSvc::fillData(){
  // this is a dummy function
  return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
// Local stuff
//////////////////////////////////////////////////////////////////////////////////////////

string
SCTCalibWriteSvc::addDefect(const string& defectlist,const int defectBeginChannel,const int defectEndChannel) const{
  // check size of defect list,
  // if it is empty then use createDefectString to make first entry. 
  if (defectlist.empty()) return createDefectString(defectBeginChannel,defectEndChannel);

  // adding another Defect in DefectList
  std::ostringstream defect;
  defect << defectlist << " " << defectBeginChannel;
  if (defectBeginChannel==defectEndChannel){
     defect << " ";
  } else {
    defect << "-" << defectEndChannel << " ";
  }
  return defect.str();
}

///////////////////////////////////////////////////////////////////////////////////////
std::string
SCTCalibWriteSvc::createDefectString(const int defectBeginChannel,const int defectEndChannel) const{
  std::ostringstream defect;
  defect << " " << defectBeginChannel;
  if (defectBeginChannel!=defectEndChannel) {
    defect << "-" << defectEndChannel;
  }
  defect << " ";
  return defect.str();
}


///////////////////////////////////////////////////////////////////////////////////////

std::string
SCTCalibWriteSvc::addNumber(const string numStr,const unsigned long long number) const{
  std::ostringstream num_string;
  // if it is empty then use createDefectString to make first entry. 
  if (numStr.empty()){
    num_string << number;
  }else{  // adding another number to numStr
    num_string << numStr << " " << number;
  }
  return num_string.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::createCondObjects(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,
  const std::string & defectType,const float threshold,const std::string & defectList) const{
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becCapsFormat);
  attrSpec->extend("DefectType","string");
  attrSpec->extend("Threshold","float");
  attrSpec->extend("DefectList","string");
 
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  } 

  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize,sctId,becCapsFormat);
  attrList0["DefectType"].setValue(static_cast<std::string>(defectType));
  attrList0["Threshold"].setValue(static_cast<float>(threshold));
  attrList0["DefectList"].setValue(static_cast<std::string>(defectList));
  std::ostringstream attrStr2;
  attrList0.toOutputStream( attrStr2 );
  m_attrListColl->add(wafer_id.get_identifier32().get_compact(), attrList0);
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::createListStrip(const Identifier& wafer_id,
                   const SCT_ID* sctId,
                   const int samplesize,
                   const std::string & defectType,
                   const float threshold,
                   const std::string & defectList) const
{
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becCapsFormat);
  attrSpec->extend("DefectType","string");
  attrSpec->extend("Threshold","float");
  attrSpec->extend("DefectList","string");
 
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  } 

  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize,sctId, becCapsFormat);
  attrList0["DefectType"].setValue(static_cast<std::string>(defectType));
  attrList0["Threshold"].setValue(static_cast<float>(threshold));
  attrList0["DefectList"].setValue(static_cast<std::string>(defectList));

  std::ostringstream attrStr2;
  attrList0.toOutputStream( attrStr2 );
  m_attrListColl_deadStrip->add(wafer_id.get_identifier32().get_compact(), attrList0);
  msg(MSG::INFO)<<"createListStrip: return StatusCode::SUCCESS"<<endmsg;
  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::createListChip(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,const std::string & defectType,const float threshold,const std::string & defectList) const{
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becCapsFormat);
  attrSpec->extend("DefectType","string");
  attrSpec->extend("Threshold","float");
  attrSpec->extend("DefectList","string");
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  } 

  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize,sctId, becCapsFormat);
  attrList0["DefectType"].setValue(static_cast<std::string>(defectType));
  attrList0["Threshold"].setValue(static_cast<float>(threshold));
  attrList0["DefectList"].setValue(static_cast<std::string>(defectList));

  std::ostringstream attrStr2;
  attrList0.toOutputStream( attrStr2 );
  m_attrListColl_deadChip->add(wafer_id.get_identifier32().get_compact(), attrList0);

  return StatusCode::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::createListEff(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,const float eff) const {
  if(!m_writeCondObjs) {return StatusCode::SUCCESS;}

  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becUnderscoreFormat);
  attrSpec->extend("Efficiency", "float");
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return(StatusCode::FAILURE);
  } 

  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize,sctId,becUnderscoreFormat);
  attrList0["Efficiency"].setValue(static_cast<float>(eff));
  
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_eff->add(wafer_id.get_identifier32().get_compact(), attrList0);

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::createListNO(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,const float noise_occ) const {
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becUnderscoreFormat);
  attrSpec->extend("NoiseOccupancy", "float");
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  } 
  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize,sctId,becUnderscoreFormat);
  attrList0["NoiseOccupancy"].setValue(static_cast<float>(noise_occ));
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_no->add(wafer_id.get_identifier32().get_compact(), attrList0);
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

StatusCode 
SCTCalibWriteSvc::createListRawOccu(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,const float raw_occu) const {
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}
  coral::AttributeListSpecification* attrSpec =createBasicDbSpec(becUnderscoreFormat);
  attrSpec->extend("RawOccupancy", "float");
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  } 
  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  setBasicValues(attrList0, wafer_id, samplesize, sctId,becUnderscoreFormat);
  attrList0["RawOccupancy"].setValue(static_cast<float>(raw_occu));
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_RawOccu->add(wafer_id.get_identifier32().get_compact(), attrList0);
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////

StatusCode 
SCTCalibWriteSvc::createListBSErr(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,const std::string & errorList,const std::string & probList) const {
  if (!m_writeCondObjs) {return StatusCode::SUCCESS;}


  int eta = sctId->eta_module(wafer_id);
  int phi = sctId->phi_module(wafer_id);
  int barrel_ec = sctId->barrel_ec(wafer_id);
  int layer = sctId->layer_disk(wafer_id);

  coral::AttributeListSpecification* attrSpec =
      new coral::AttributeListSpecification();
  attrSpec->extend("SampleSize", "int");
  attrSpec->extend("barrel_endcap", "int");
  attrSpec->extend("Layer", "int");
  attrSpec->extend("Eta", "int");
  attrSpec->extend("Phi", "int");
  attrSpec->extend("BSErrors", "string");
  attrSpec->extend("BadFraction", "string");

  if (!attrSpec->size()) {
      msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
      return StatusCode::FAILURE;
  }

  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  attrList0["SampleSize"].setValue(static_cast<int>(samplesize));
  attrList0["barrel_endcap"].setValue(static_cast<int>(barrel_ec));
  attrList0["Layer"].setValue(static_cast<int>(layer));
  attrList0["Phi"].setValue(static_cast<int>(phi));
  attrList0["Eta"].setValue(static_cast<int>(eta));
  attrList0["BSErrors"].setValue(static_cast<std::string>(errorList));
  attrList0["BadFraction"].setValue(static_cast<std::string>(probList));

  
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_BSErr->add(wafer_id.get_identifier32().get_compact(), attrList0);
  return StatusCode::SUCCESS;
}


StatusCode
SCTCalibWriteSvc::createListLA(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,int module, const float lorentz, const float err_lorentz, const float chisq, const float fitParam_a, const float err_a, const float fitParam_b, const float err_b, const float fitParam_sigma, const float err_sigma, const float MCW, const float err_MCW ) const {
  if (!m_writeCondObjs) return StatusCode::SUCCESS;
  int barrel_ec = sctId->barrel_ec(wafer_id);
  int layer = sctId->layer_disk(wafer_id);
  int side = sctId->side(wafer_id);
  
  coral::AttributeListSpecification* attrSpec =new coral::AttributeListSpecification();
  attrSpec->extend("SampleSize", "int");
  attrSpec->extend("barrel_endcap", "int");
  attrSpec->extend("Layer", "int");
  attrSpec->extend("Side", "int");
  attrSpec->extend("moduleType", "int");
  attrSpec->extend("lorentzAngle", "float");
  attrSpec->extend("err_lorentzAngle", "float");
  attrSpec->extend("chisq", "float");
  attrSpec->extend("fitParam_a", "float");
  attrSpec->extend("err_a", "float");
  attrSpec->extend("fitParam_b", "float");
  attrSpec->extend("err_b", "float");
  attrSpec->extend("fitParam_sigma", "float");
  attrSpec->extend("err_sigma", "float");
  attrSpec->extend("minClusterWidth", "float");
  attrSpec->extend("err_minClusterWidth", "float");
   
  if (!attrSpec->size()) {
    msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  // Add three attr lists
  coral::AttributeList attrList0(*attrSpec);
  attrList0["SampleSize"].setValue(static_cast<int>(samplesize));
  attrList0["barrel_endcap"].setValue(static_cast<int>(barrel_ec));
  attrList0["Layer"].setValue(static_cast<int>(layer));
  attrList0["Side"].setValue(static_cast<int>(side));
  attrList0["moduleType"].setValue(static_cast<int>(module));
  attrList0["lorentzAngle"].setValue(static_cast<float>(lorentz));
  attrList0["err_lorentzAngle"].setValue(static_cast<float>(err_lorentz));
  attrList0["chisq"].setValue(static_cast<float>(chisq));
  attrList0["fitParam_a"].setValue(static_cast<float>(fitParam_a));
  attrList0["err_a"].setValue(static_cast<float>(err_a));
  attrList0["fitParam_b"].setValue(static_cast<float>(fitParam_b));
  attrList0["err_b"].setValue(static_cast<float>(err_b));
  attrList0["fitParam_sigma"].setValue(static_cast<float>(fitParam_sigma));
  attrList0["err_sigma"].setValue(static_cast<float>(err_sigma));
  attrList0["minClusterWidth"].setValue(static_cast<float>(MCW));
  attrList0["err_minClusterWidth"].setValue(static_cast<float>(err_MCW));
   
  std::ostringstream attrStr2;
  attrList0.toOutputStream(attrStr2);
  m_attrListColl_LA->add(wafer_id.get_identifier32().get_compact(), attrList0);
  
  return StatusCode::SUCCESS;
}


// StatusCode 
// SCTCalibWriteSvc::createListLA(const Identifier& wafer_id,const SCT_ID* sctId,const int samplesize,int /*module*/, const float lorentz, const float MCW ) const {
//   if (!m_writeCondObjs) return StatusCode::SUCCESS;
//   int barrel_ec = sctId->barrel_ec(wafer_id);
//   int layer = sctId->layer_disk(wafer_id);
//   int side = sctId->side(wafer_id);

//   coral::AttributeListSpecification* attrSpec =new coral::AttributeListSpecification();
//   attrSpec->extend("SampleSize", "int");
//   attrSpec->extend("barrel_endcap", "int");
//   attrSpec->extend("Layer", "int");
//   attrSpec->extend("Side", "int");
//   attrSpec->extend("LorentzAngle", "float");
//   attrSpec->extend("MinClusterWidth", "float");
 
//   if (!attrSpec->size()) {
//     msg(MSG:: ERROR) << " Attribute list specification is empty" <<endmsg;
//     return StatusCode::FAILURE;
//   } 

//   // Add three attr lists
//   coral::AttributeList attrList0(*attrSpec);
//   attrList0["SampleSize"].setValue(static_cast<int>(samplesize));
//   attrList0["barrel_endcap"].setValue(static_cast<int>(barrel_ec));
//   attrList0["Layer"].setValue(static_cast<int>(layer));
//   attrList0["Side"].setValue(static_cast<int>(side));
//   attrList0["LorentzAngle"].setValue(static_cast<float>(lorentz));
//   attrList0["MinClusterWidth"].setValue(static_cast<float>(MCW));
  
//   std::ostringstream attrStr2;
//   attrList0.toOutputStream(attrStr2);
//   m_attrListColl_LA->add(wafer_id.get_identifier32().get_compact(), attrList0);

//   return StatusCode::SUCCESS;
// }

///////////////////////////////////////////////////////////////////////////////////

const CondAttrListCollection*
SCTCalibWriteSvc::getAttrListCollectionByFolder(const string& foldername) const {
    // trying to find the pointer in the hashmap
    // if it exists, return it, otherwise put it in.
  const CondAttrListCollection* nullPtr(0);
  const CondAttrListCollection* attrListCollection = nullPtr;
  if (m_attrListCollectionMap.count(foldername) == 0) {
      if ( m_detStore->retrieve(attrListCollection, foldername).isFailure()) return  msg(MSG:: ERROR)<< "Could not retrieve " << foldername << endmsg, nullPtr;
      m_attrListCollectionMap.insert(make_pair(foldername, attrListCollection));
  } else {
      attrListCollection = m_attrListCollectionMap[foldername];
  }
  return attrListCollection;
}

// ===========================================================================

int SCTCalibWriteSvc::stringToInt(const std::string& s) const {
    return atoi(s.c_str());  
}

// ======================================================================
StatusCode SCTCalibWriteSvc::wrapUpNoisyChannel(){
  if (recordAndStream(m_attrListColl,s_defectFolderName,m_defectRecorded).isFailure()) return StatusCode::FAILURE;
  if (registerCondObjectsWithErrMsg(s_defectFolderName, m_tagID4NoisyStrips).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpDeadStrips(){
  if ( recordAndStream(m_attrListColl_deadStrip,s_deadStripFolderName,m_deadStripRecorded).isFailure()) return  StatusCode::FAILURE;
  if (registerCondObjectsWithErrMsg(s_deadStripFolderName, m_tagID4DeadStrips).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpDeadChips(){
  if (recordAndStream(m_attrListColl_deadChip,s_deadChipFolderName,m_deadChipRecorded).isFailure())  return StatusCode::FAILURE;;
  if (registerCondObjectsWithErrMsg(s_deadChipFolderName, m_tagID4DeadChips).isFailure()) return StatusCode::FAILURE;  
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpEfficiency(){
  if (recordAndStream(m_attrListColl_eff, s_effFolderName,m_effRecorded).isFailure()) return StatusCode::FAILURE;
  if (registerCondObjectsWithErrMsg(s_effFolderName, m_tagID4Efficiency).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpNoiseOccupancy(){
  if (recordAndStream(m_attrListColl_no, s_noFolderName,m_noRecorded).isFailure()) return StatusCode::FAILURE;
  if (registerCondObjectsWithErrMsg(s_noFolderName, m_tagID4NoiseOccupancy).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpRawOccupancy(){
  if (recordAndStream(m_attrListColl_RawOccu, s_RawOccuFolderName,m_RawOccuRecorded).isFailure()) return StatusCode::FAILURE;
  if( registerCondObjectsWithErrMsg(s_RawOccuFolderName, m_tagID4RawOccupancy).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpBSErrors(){
  if (recordAndStream(m_attrListColl_BSErr, s_BSErrFolderName,m_BSErrRecorded).isFailure()) return StatusCode::FAILURE;;
  if (registerCondObjectsWithErrMsg(s_BSErrFolderName, m_tagID4BSErrors).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////
StatusCode SCTCalibWriteSvc::wrapUpLorentzAngle(){
  if (recordAndStream(m_attrListColl_LA, s_LAFolderName,m_LARecorded).isFailure()) return StatusCode::FAILURE;
  if( registerCondObjectsWithErrMsg(s_LAFolderName, m_tagID4LorentzAngle).isFailure()) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::streamOutCondObjects(const std::string& foldername) const{
  //if (m_streamer->connectOutput().isFailure()) {
  if (m_streamer->connectOutput(m_streamName).isFailure()) {
    msg(MSG:: ERROR) <<"Could not connect stream to output" <<endmsg;
    return( StatusCode::FAILURE);
  }
  IAthenaOutputStreamTool::TypeKeyPairs typeKeys(1);
  if (m_readWriteCool) {
    IAthenaOutputStreamTool::TypeKeyPair
    attrCollPair("CondAttrListCollection",foldername);
    typeKeys[0] = attrCollPair;
  }
    
  if (m_streamer->streamObjects(typeKeys).isFailure()) {
    msg(MSG:: ERROR) << "Could not stream out AttributeLists" << endmsg;
    return StatusCode::FAILURE;
  }
    
  if (m_streamer->commitOutput().isFailure()) {
    msg(MSG:: ERROR) << "Could not commit output stream" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////////////

StatusCode SCTCalibWriteSvc::streamOutCondObjectsWithErrMsg(const std::string& foldername) const{
  if (streamOutCondObjects(foldername).isFailure()){
    return msg(MSG:: ERROR) <<"Could create conditions object  "<< foldername<< endmsg, StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode 
SCTCalibWriteSvc::registerCondObjects(const std::string& foldername,const std::string& tagname) {
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
        if (service("StoreGateSvc",pStoreGate).isFailure()) {
          msg(MSG:: FATAL) << "StoreGate service not found !" << endmsg;
          return StatusCode::FAILURE;
        }
        if (pStoreGate->retrieve(m_evt).isFailure()) {
          msg(MSG:: ERROR) << "Unable to get the EventSvc" << endmsg;
          return StatusCode::FAILURE;
        }
        beginRun = m_evt->event_ID()->run_number();
        endRun = beginRun;
      } else {
        beginRun = m_beginRun;
        if ( m_endRun != -1 )    endRun = m_endRun;
        else                     endRun = IOVTime::MAXRUN;
      }
      
      unsigned int beginLB = IOVTime::MINEVENT;
      unsigned int endLB = IOVTime::MAXEVENT;
      
      //msg(MSG:: INFO) <<"beginRun = "<<beginRun<<"   endRun = "<<endRun<<"    tag = "<<tagname<<"   m_manualiov = "<<m_manualiov << endmsg;

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

StatusCode 
SCTCalibWriteSvc::registerCondObjectsWithErrMsg(const std::string& foldername,const std::string& tagname) {
  if (m_regIOV){
    if (registerCondObjects(foldername,tagname).isFailure()){
       msg(MSG:: ERROR) << "Could not register "<<foldername << endmsg;
       return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode 
SCTCalibWriteSvc::recordAndStream(const CondAttrListCollection* pCollection,const std::string & foldername, bool & flag){
  if (m_writeCondObjs) {
    if (m_detStore->record(pCollection, foldername).isFailure()) {
      msg(MSG:: ERROR) << "Could not record "<<foldername << endmsg;
      return StatusCode::FAILURE;
    }
    flag=true;
    if (streamOutCondObjectsWithErrMsg(s_defectFolderName).isFailure()) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

coral::AttributeListSpecification*
SCTCalibWriteSvc::createBasicDbSpec(const bool capsFormat) const{
  coral::AttributeListSpecification* attrSpec =new coral::AttributeListSpecification();
  const std::string becName=capsFormat?"BarrelEndcap":"barrel_endcap";
  attrSpec->extend("SampleSize", "int");
  attrSpec->extend(becName, "int");
  attrSpec->extend("Layer", "int");
  attrSpec->extend("Eta", "int");
  attrSpec->extend("Phi", "int");
  return attrSpec;
}

void
SCTCalibWriteSvc::setBasicValues(coral::AttributeList & attrList, const Identifier& wafer_id, const int samplesize,const SCT_ID* sctId, const bool capsFormat) const{
  int eta = sctId->eta_module(wafer_id);
  int phi = sctId->phi_module(wafer_id);
  int barrel_ec = sctId->barrel_ec(wafer_id);
  int layer = sctId->layer_disk(wafer_id);
  //
  const std::string becName=capsFormat?"BarrelEndcap":"barrel_endcap";
  attrList["SampleSize"].setValue(static_cast<int>(samplesize));
  attrList[becName].setValue(static_cast<int>(barrel_ec));
  attrList["Layer"].setValue(static_cast<int>(layer));
  attrList["Eta"].setValue(static_cast<int>(eta));
  attrList["Phi"].setValue(static_cast<int>(phi));

  return;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



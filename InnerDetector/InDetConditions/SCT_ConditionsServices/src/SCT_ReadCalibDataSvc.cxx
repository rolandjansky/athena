/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ReadCalibDataSvc.cxx Implementation file for SCT_ReadCalibDataSvc.
    @author Per Johansson, 17/05/08, based on SCT_ReadCalibDataTool.
*/

// Include SCT_ReadCalibDataSvc.h
#include "SCT_ReadCalibDataSvc.h"

// Include Athena stuff
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h" 
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "StoreGate/StoreGateSvc.h"

// Include Gaudi stuff
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/StatusCode.h"

// Include STL stuff
#include <limits>
#include <map>
#include <sstream>

// Include boost stuff
#include "boost/lexical_cast.hpp"
#include "boost/tokenizer.hpp"

// Utility functions
namespace {
  template <typename C> 
  bool fillFromString(const std::string& source, C& userContainer) {
    typedef typename C::value_type V_t;
    V_t errVal{0};
    boost::char_separator<char> sep{" "};
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    Tokenizer tok{source, sep};
    bool noNan{true};
    for (Tokenizer::iterator i{tok.begin()}; i!=tok.end(); ++i) {
      try {
	userContainer.push_back(boost::lexical_cast<V_t>(*i));
      } catch (boost::bad_lexical_cast) {
	userContainer.push_back(errVal);
	noNan=false;
      }
    }
    return noNan;
  }
  float coerceToFloatRange(const double value) {
    const float maxfloat{std::numeric_limits<float>::max()};
    if (value>maxfloat) return maxfloat;
    else return static_cast<float>(value);
  } 
}

//----------------------------------------------------------------------
SCT_ReadCalibDataSvc::SCT_ReadCalibDataSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_storeGateSvc{"StoreGateSvc", name},
  m_detStoreSvc{"DetectorStore", name},
  m_IOVDbSvc{"IOVDbSvc", name},
  m_cabling{"SCT_CablingSvc", name},
  m_SCTdetMgr{nullptr},
  m_attrListColl{nullptr},
  m_id_sct{nullptr},
  m_dataFilled{false},
  m_NPGDefects{},
  m_NODefects{},
  m_printCalibDefectMaps{false},
  m_recoOnly{true},
  m_ignoreDefects{},
  m_ignoreDefectParameters{} {
    declareProperty("PrintCalibDefectMaps", m_printCalibDefectMaps=false, "Print data read from the Calib Defect map?");
    declareProperty("AttrListCollFolders", m_atrcollist, "List of calibration data folder?"); 
    declareProperty("RecoOnly", m_recoOnly, "Use new improved isGood method, all other methods defunct"); 
    declareProperty("IgnoreDefects", m_ignoreDefects, "Defects to ignore");
    declareProperty("IgnoreDefectsParameters", m_ignoreDefectParameters, "Limit on defect to ignore parameters");

    std::vector<std::string> names{2};
    names[0] = std::string{"/SCT/DAQ/Calibration/NPtGainDefects"};
    names[1] = std::string{"/SCT/DAQ/Calibration/NoiseOccupancyDefects"};
    m_atrcollist.setValue(names);
    WaferIsGoodInfo_t oneWaferIsGoodInfo;
    oneWaferIsGoodInfo.assign(0);
    m_isGoodAllWafersInfo.assign(oneWaferIsGoodInfo);
    m_ignoreDefects.push_back("NOISE_SLOPE");
    m_ignoreDefects.push_back("OFFSET_SLOPE");
    m_ignoreDefects.push_back("GAIN_SLOPE");
    m_ignoreDefects.push_back("BAD_OPE");
    m_ignoreDefectParameters.push_back(-1000.);
    m_ignoreDefectParameters.push_back(-1000.);
    m_ignoreDefectParameters.push_back(-1000.);
    m_ignoreDefectParameters.push_back(-1000.);
  }

//----------------------------------------------------------------------
SCT_ReadCalibDataSvc::~SCT_ReadCalibDataSvc() {
}

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataSvc::initialize() {
  StatusCode sc;
  // Print where you are
  ATH_MSG_DEBUG("in initialize()");
  // Get SCT detector manager
  sc = m_detStoreSvc->retrieve(m_SCTdetMgr, "SCT");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT detector manager");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE("Found SCT detector manager");
  }

  // Get SCT helper
  sc = m_detStoreSvc->retrieve(m_id_sct, "SCT_ID");
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failed to get SCT helper");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE("Found SCT helper");
  }

  // Retrieve IOVDb service
  sc = m_IOVDbSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve IOVDbSvc ");
    return StatusCode::FAILURE;
  }

  // Retrieve SCT Cabling service
  sc = m_cabling.retrieve();
  if (sc.isFailure()) {  
    ATH_MSG_ERROR("Failed to retrieve SCT cabling service");
    return StatusCode::FAILURE;
  }

  //Register callbacks for CalibData folders using a vector of keys defined in jobOpt
  std::vector<std::string>::const_iterator itr{m_atrcollist.value().begin()};
  std::vector<std::string>::const_iterator end{m_atrcollist.value().end()}; 
  for (;itr!=end;++itr) {
    m_key = *itr;
    if (m_key == "/SCT/DAQ/Calibration/NPtGainDefects") {
      if (StatusCode::SUCCESS==m_detStoreSvc->regFcn(&SCT_ReadCalibDataSvc::fillData, this, m_CalibDefects_NP, m_key)) {
        ATH_MSG_INFO("Registered callback for key: " << m_key);
      } else {
        ATH_MSG_ERROR("Cannot register callbacks function for key " << m_key);
      }
    } else if (m_key == "/SCT/DAQ/Calibration/NoiseOccupancyDefects") {
      if (StatusCode::SUCCESS==m_detStoreSvc->regFcn(&SCT_ReadCalibDataSvc::fillData, this, m_CalibDefects_NO, m_key)) {
        ATH_MSG_INFO("Registered callback for key: " << m_key);
      } else {
        ATH_MSG_ERROR("Cannot register callbacks function for key " << m_key);
      }
    } else {
      ATH_MSG_ERROR("Cannot register callbacks function for key " << m_key);
    }
  }
  
  // Initialize arrays and all strips to True
  for (int w{0}; w!=NUMBER_OF_WAFERS; ++w) {
    for (int s{0}; s!=STRIPS_PER_WAFER; ++s) {
      m_isGoodAllWafersInfo[w][s]=true;
    }
  }
  
  //Check ignoreDefects vectors are the same size
  if (m_ignoreDefects.size() != m_ignoreDefectParameters.size()) {
    ATH_MSG_ERROR("IgnoreDefect != IgnoreDefectsParameters, check job options!");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataSvc::initialize()

//----------------------------------------------------------------------
StatusCode SCT_ReadCalibDataSvc::finalize() {
  return StatusCode::SUCCESS;
} // SCT_ReadCalibDataSvc::finalize()

//----------------------------------------------------------------------
// Query the interfaces.
StatusCode SCT_ReadCalibDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ISCT_ReadCalibDataSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  } else if (ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
//Can only report good/bad at strip level
bool SCT_ReadCalibDataSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h==InDetConditions::SCT_STRIP);
}

//----------------------------------------------------------------------
// Returns ok if fillData worked properly
bool SCT_ReadCalibDataSvc::filled() const {
  return m_dataFilled;
} //SCT_ReadCalibDataSvc::filled()

//----------------------------------------------------------------------
// Fill the data structures from a callback
StatusCode SCT_ReadCalibDataSvc::fillData(int& /*i*/ , std::list<std::string>& l) {
  // Print where you are
  std::list<std::string>::const_iterator itr;
  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "fillData has been triggered by: ";
    for (itr=l.begin(); itr!=l.end(); ++itr) {
      msg(MSG::DEBUG) << *itr << " ";
    }
    msg(MSG::DEBUG) << endmsg;
  }

  StatusCode sc0{SCT_ReadCalibDataSvc::fillCalibDefectData(l)};
  // No longer need the conditions folder as stored locally
  for (itr=l.begin(); itr!=l.end(); ++itr) {
    m_IOVDbSvc->dropObject(*itr,true); 
  }

  if (sc0==StatusCode::SUCCESS) {
    m_dataFilled = true;
    ATH_MSG_INFO("Calib Defect array/maps filled ok");
    // Print the defect maps
    if (m_printCalibDefectMaps and not m_recoOnly) {
      std::string npgCalibDefectMap{m_NPGDefects.str()};
      std::string noCalibDefectMap{m_NODefects.str()};
      ATH_MSG_DEBUG("\n" << npgCalibDefectMap);
      ATH_MSG_DEBUG(noCalibDefectMap);
    }
    return StatusCode::SUCCESS;
  } else {
    m_dataFilled = false;
    ATH_MSG_ERROR("fillData failed");
    return StatusCode::FAILURE;
  }
} //SCT_ReadCalibDataSvc::fillData()

//----------------------------------------------------------------------
// Callback for Calib defect data
StatusCode SCT_ReadCalibDataSvc::fillCalibDefectData(std::list<std::string>& keys) {
  // Defect type map, contains Fit, NPtGain and No defects for now
  static std::map<int, std::string> s_defectMapIntToString;
  // Fit Defects
  s_defectMapIntToString[0]  = "UNKNOWN";       //<! Defect type not in this map, add!  
  s_defectMapIntToString[1]  = "DEAD";          //<! Output always < 1%
  s_defectMapIntToString[2]  = "STUCKON";       //<! Output always > 98%
  s_defectMapIntToString[3]  = "UNDER";         //<! Occupancy never reaches max, always less than 95%
  s_defectMapIntToString[4]  = "OVER";          //<! Occcupancy greater than 100%
  s_defectMapIntToString[5]  = "BADFIT";        //<! The fit was not good for some reason - parameter is a chi2 cut
  // NPt Gain Defects
  s_defectMapIntToString[32] = "VLO_GAIN";      //<! Gain < 0.3 * chip average
  s_defectMapIntToString[9]  = "LO_GAIN";       //<! Gain < 0.75 * chip average
  s_defectMapIntToString[10] = "HI_GAIN";       //<! Gain > 1.25 * chip average
  s_defectMapIntToString[11] = "LO_OFFSET";     //<! Offset < -100
  s_defectMapIntToString[12] = "HI_OFFSET";     //<! Offset > 200
  s_defectMapIntToString[13] = "UNBONDED";      //<! Noise <= 750
  s_defectMapIntToString[14] = "PARTBONDED";    //<! Noise <= 1100
  s_defectMapIntToString[15] = "NOISY";         //<! Noise > 1.15* av chip noise
  s_defectMapIntToString[33] = "V_NOISY";       //<! Noise > 1.25* av chip noise
  s_defectMapIntToString[34] = "NOISE_SLOPE";   //<! Slope in noise across module, slope/chan > 1.
  s_defectMapIntToString[35] = "OFFSET_SLOPE";  //<! Slope in offset across module, slope/chan > 0.07
  s_defectMapIntToString[36] = "GAIN_SLOPE";    //<! Slope in gain across module, slope/chan > 0.04
  // Noise Occupancy Defects
  s_defectMapIntToString[19] = "NO_HI";         //<! High noise occupancy, 0.0005
  s_defectMapIntToString[37] = "BAD_OPE";       //<! Bad occupancy per event variance/binomial variance > 2.0)
  s_defectMapIntToString[38] = "DOUBTR_HI";     //<! High double trigger noise occupancy, > 5

  if (not m_recoOnly) {
    // Check if maps empty, and if not clear them
    if (not m_NPGDefects.empty()) m_NPGDefects.clear();
    if (not m_NODefects.empty()) m_NODefects.clear();
  }
  
  // Create pointer to CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects theseDefects;
  
  // Retrieve CondAttrListCollection
  const CondAttrListCollection* atrlistcol;
  std::list<std::string>::const_iterator itr_key;
  for (itr_key=keys.begin(); itr_key!=keys.end(); ++itr_key) {
    std::string folder{*itr_key};
    if (m_detStoreSvc->retrieve(atrlistcol, *itr_key)==StatusCode::SUCCESS) {
      // loop over collection
      CondAttrListCollection::const_iterator itLoop{atrlistcol->begin()};
      CondAttrListCollection::const_iterator itLoop_end{atrlistcol->end()};
      for (; itLoop!=itLoop_end; ++itLoop) {
        CondAttrListCollection::ChanNum chanNum{(*itLoop).first};
        coral::AttributeList anAttrList{(*itLoop).second};
  
        // Convert chanNum=offlineID into identifier
        Identifier moduleId{chanNum};
        IdentifierHash hashId0{m_id_sct->wafer_hash(moduleId)};
        IdentifierHash hashId1;
        m_id_sct->get_other_side(hashId0, hashId1);

        // Check for PhiSwap readout
        bool phiSwap0Present{false};
        bool phiSwap1Present{false};
        InDetDD::SiDetectorElement* p_element{m_SCTdetMgr->getDetectorElement(hashId0)};
        if (p_element->swapPhiReadoutDirection()) { phiSwap0Present = true; }
        p_element = (m_SCTdetMgr->getDetectorElement(hashId1));
        if (p_element->swapPhiReadoutDirection()) { phiSwap1Present = true; }
       
        // Clear theseDefects
        theseDefects.begDefects.clear();
        theseDefects.endDefects.clear();
        theseDefects.typeOfDefect.clear();
        theseDefects.parValue.clear();
  
        // Get all defect parameters from COOL attrib list
        std::string gaindefectb{((anAttrList)["defectBeginChannel"]).data<std::string>()};
        std::string gaindefecte{((anAttrList)["defectEndChannel"]).data<std::string>()};
        std::string defectType{((anAttrList)["defectType"]).data<std::string>()};
        std::string parValue{((anAttrList)["defectParameter"]).data<std::string>()};
  
        // Convert the defect strings to vectors
        std::vector<unsigned int> gaindefectbvec;
        fillFromString(gaindefectb, gaindefectbvec);
        std::vector<unsigned int> gaindefectevec;
        fillFromString(gaindefecte, gaindefectevec);
        std::vector<unsigned int> defectTypevec;
        fillFromString(defectType, defectTypevec);
        std::vector<double> parValuevec;
        fillFromString(parValue, parValuevec);

        // Fill the Calib defect objects
        long unsigned int gainvec_size{gaindefectbvec.size()};
        for (long unsigned int i{0}; i<gainvec_size; ++i) {
          theseDefects.begDefects.push_back(gaindefectbvec[i]);
          theseDefects.endDefects.push_back(gaindefectevec[i]);
          theseDefects.typeOfDefect.push_back(s_defectMapIntToString[defectTypevec[i]]);
          theseDefects.parValue.push_back(coerceToFloatRange(parValuevec[i]));
        }
        // Fill the isGoodWaferArray
        if (not theseDefects.begDefects.empty()) {
          for (unsigned int i{0}; i<theseDefects.begDefects.size(); ++i) { // loop over all defects
            // Check for defects and their limits not to take into account in isGood
            bool ignoreDefect{false};
            unsigned int ig{0};
            //std::string defectType = theseDefects.typeOfDefect[i];
            while (ig<m_ignoreDefects.size()) { //loop until found defect or end of ignoredefects
              if (m_ignoreDefects[ig] == theseDefects.typeOfDefect[i]) {
                if (                                                   m_ignoreDefectParameters[ig]<-999) ignoreDefect = true; //no check on parameter value, defect ignored
                else if (theseDefects.typeOfDefect[i]=="NO_HI"     and m_ignoreDefectParameters[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 0.0005 (in DB, so default values printed here)
                else if (theseDefects.typeOfDefect[i]=="NOISY"     and m_ignoreDefectParameters[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 1.15* av chip average
                else if (theseDefects.typeOfDefect[i]=="V_NOISY"   and m_ignoreDefectParameters[ig]>theseDefects.parValue[i]) ignoreDefect = true; //noise below threshold, > 1.25* av chip average
                else if (theseDefects.typeOfDefect[i]=="VLO_GAIN"  and m_ignoreDefectParameters[ig]<theseDefects.parValue[i]) ignoreDefect = true; // gain to low, < 0.3 * chip average
                else if (theseDefects.typeOfDefect[i]=="LO_GAIN"   and m_ignoreDefectParameters[ig]<theseDefects.parValue[i]) ignoreDefect = true; // gain to low < 0.75 * chip average
                else if (theseDefects.typeOfDefect[i]=="HI_GAIN"   and m_ignoreDefectParameters[ig]>theseDefects.parValue[i]) ignoreDefect = true; // gain to high > 1.25 * chip average
                else if (theseDefects.typeOfDefect[i]=="LO_OFFSET" and m_ignoreDefectParameters[ig]>theseDefects.parValue[i]) ignoreDefect = true; // offset to low < -100
                else if (theseDefects.typeOfDefect[i]=="HI_OFFSET" and m_ignoreDefectParameters[ig]<theseDefects.parValue[i]) ignoreDefect = true; // offset to high > 200
              }
              ig++;
            }
            if (!ignoreDefect) {
              //set the isGoodBool value for all strips for this defect
              for (unsigned int strip{theseDefects.begDefects[i]}; strip <= theseDefects.endDefects[i]; ++strip) { 
                // Check for phiSwap and which wafer side before filling isGood vector
                if (strip < STRIPS_PER_WAFER) { //side 0 0->767
                  const unsigned int waferId0{hashId0};
                  WaferIsGoodInfo_t& thisWaferIsGoodData0{m_isGoodAllWafersInfo[waferId0]};
                  const unsigned int side0StripNumber{phiSwap0Present ? (  STRIPS_PER_WAFER-1-strip) : strip};
                  thisWaferIsGoodData0[side0StripNumber] = false;
                } else { // side 1 768->1535 => 0->767
                  const unsigned int waferId1{hashId1};
                  WaferIsGoodInfo_t& thisWaferIsGoodData1{m_isGoodAllWafersInfo[waferId1]};
                  const unsigned int side1StripNumber{phiSwap1Present ? (2*STRIPS_PER_WAFER-1-strip) : (strip-STRIPS_PER_WAFER)};
                  thisWaferIsGoodData1[side1StripNumber] = false;
                }
              }
            }
          }
        }

        if (not m_recoOnly) {
	  // Check what folder we are in
          size_t folderfoundNP{folder.find("NPtGain")};
          size_t folderfoundNO{folder.find("NoiseOccupancy")};

          // Fill the CalibDefectData maps with the Calib defect objects
          if (folderfoundNP!=std::string::npos) {
            if (theseDefects.begDefects.empty()) {
              ATH_MSG_DEBUG("No NPtGain defects for module " << moduleId);
              continue;
            }
            if (!(m_NPGDefects.addModule(moduleId, theseDefects))) {
              ATH_MSG_ERROR("Unable to add module " << moduleId << " to NPtGain defect map");
              return StatusCode::FAILURE;
            } else {
              ATH_MSG_DEBUG("Defects for module " << moduleId << " added to NPG defect map");
            }
          } else if (folderfoundNO!=std::string::npos) {  
            if (theseDefects.begDefects.empty()) {
              ATH_MSG_DEBUG("No NoiseOccupancy defects for module " << moduleId);
              continue;
            }
            if (!(m_NODefects.addModule(moduleId, theseDefects))) {
              ATH_MSG_ERROR("Unable to add module " << moduleId << " to NoiseOccupancy defect map");
              return StatusCode::FAILURE;
            } else {
              ATH_MSG_DEBUG("Defects for module " << moduleId << " added to NoiseOccupancy defect map");
            } 
          }
        }
      }
    } else {
      ATH_MSG_ERROR("Could not retrieve CondAttrListCollection " << *itr_key);
    }  
  }

  if (not m_recoOnly) {
    ATH_MSG_DEBUG("There are " << m_NPGDefects.size() << " elements in the NPtGain module defect map");
    ATH_MSG_DEBUG("There are " << m_NODefects.size() << " elements in the NoiseOccupancy module defect map");
  }
 
  return StatusCode::SUCCESS;
} //SCT_ReadCalibDataSvc::fillCalibDefectData()

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool SCT_ReadCalibDataSvc::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) {
  // Status of the compId
  bool status{true};
  // Extract the moduleId from the comp identifier
  Identifier moduleId{m_id_sct->module_id(elementId)};
  switch (h) {
  case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Module good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Wafer good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Chip good/bad is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_STRIP:
    {
      // Get hashId
      IdentifierHash hashIdx{m_id_sct->wafer_hash(moduleId)};
      int side{m_id_sct->side(elementId)};
      if (side==1) m_id_sct->get_other_side(hashIdx, hashIdx);
      unsigned int waferIdx{hashIdx};
      //unsigned int waferIdx=hashIdx+side; //uhm
      // Get strip on wafer to check
      int strip{m_id_sct->strip(elementId)};
      // Retrieve isGood Wafer data
      WaferIsGoodInfo_t& wantedIsGoodWafer{m_isGoodAllWafersInfo[waferIdx]};
      // Set value
      status = wantedIsGoodWafer[strip];
      break;
    }
  default:
    {
      status = true ;
      ATH_MSG_INFO("Unknown component has been asked for, should be Module/Wafer/Chip or Strip; returning 'good' and continuing");
    }    
  } //end of switch structure
  
  // Print status  
  return status;
} //SCT_ReadCalibDataSvc::summary()


//----------------------------------------------------------------------
// Returns a defect summary of a defect strip, scan, type and value
SCT_ReadCalibDataSvc::CalibDefectType SCT_ReadCalibDataSvc::defectType(const Identifier& stripId, InDetConditions::Hierarchy h) {
  // Print where you are
  ATH_MSG_DEBUG("in defectType()");

  // Extract the moduleId from the comp identifier
  Identifier moduleId{m_id_sct->module_id(stripId)};
  ATH_MSG_DEBUG("Summary wanted for component: " << stripId << " on module: " << moduleId);

  // Create the CalibDataDefect objects
  SCT_CalibDefectData::CalibModuleDefects wantedNPGDefects;
  SCT_CalibDefectData::CalibModuleDefects wantedNODefects;

  // Create the calibDefectSummary
  CalibDefectType theseSummaryDefects;
  // Retrieve defect data from map
  wantedNPGDefects = m_NPGDefects.findModule(moduleId);
  wantedNODefects = m_NODefects.findModule(moduleId);

  switch (h) {
  case InDetConditions::SCT_MODULE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Module defect summary is not applicable for Calibration data");
      break;
    }

  case InDetConditions::SCT_SIDE:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Wafer defect summary is not applicable for Calibration data");
      break;
    }

  case InDetConditions::SCT_CHIP:
    {
      // Not applicable for Calibration data
      ATH_MSG_WARNING("summary(): Chip defect summary is not applicable for Calibration data");
      break;
    }
  case InDetConditions::SCT_STRIP:
    {
      // Get the strip/channel number to check
      int side{m_id_sct->side(stripId)};
      int strip{m_id_sct->strip(stripId)};
      unsigned int stripNum;
      const InDetDD::SiDetectorElement* p_element{m_SCTdetMgr->getDetectorElement(stripId)};
      if (p_element->swapPhiReadoutDirection()) {
        if (side == 0) {
          stripNum =   STRIPS_PER_WAFER-1 - strip;
        } else {
          stripNum = 2*STRIPS_PER_WAFER-1 - strip;
        }      
      } else {
        stripNum = side * STRIPS_PER_WAFER + strip;
      }
      
      // Find the bad strip and fill calibDefectSummary
      if (wantedNPGDefects.begDefects.empty()) {
        ATH_MSG_VERBOSE("No NPtGain defects in this module");
      } else {
        for (unsigned int i{0}; i<wantedNPGDefects.begDefects.size(); ++i) {
          if (stripNum>=wantedNPGDefects.begDefects[i] and stripNum<=wantedNPGDefects.endDefects[i]) {
            theseSummaryDefects.scan.push_back("NPtGain");
            theseSummaryDefects.defect.push_back(wantedNPGDefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNPGDefects.parValue[i]);
            ATH_MSG_VERBOSE("NPtGain defect summary for strip " << stripNum << " filled");
          }
        }
      }

      if (wantedNODefects.begDefects.empty()) {
        ATH_MSG_VERBOSE("No NoiseOccupancy defects in this module");
      } else {
        for (unsigned int i{0}; i != wantedNODefects.begDefects.size(); ++i) {
          if (stripNum>=wantedNODefects.begDefects[i] and stripNum <= wantedNODefects.endDefects[i]) {
            theseSummaryDefects.scan.push_back("NoiseOccupancy");
            theseSummaryDefects.defect.push_back(wantedNODefects.typeOfDefect[i]);
            theseSummaryDefects.value.push_back(wantedNODefects.parValue[i]);
            ATH_MSG_VERBOSE("NoiseOccupancy defect summary for strip " << stripNum << "  filled");
          }
        }
      } 
      if (theseSummaryDefects.scan.empty()) {
        ATH_MSG_VERBOSE("defectSummary(): can't retrieve the defects for this strip: " <<  stripNum << " since strip good");
      }     
      break;    
    }
  default:
    {
      ATH_MSG_INFO("Unknown component requested, should be one of Module/Side/Chip or Strip");
      return theseSummaryDefects;
    }

  }//end of switch structure

  return theseSummaryDefects;
} //SCT_ReadCalibDataSvc::defectType()

//----------------------------------------------------------------------
// Returns a summary of all defects on a module for a given scan
SCT_CalibDefectData::CalibModuleDefects SCT_ReadCalibDataSvc::defectsSummary(const Identifier& moduleId, const std::string& scan) {
  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;
  // Retrieve the correct defect map
  if (scan == "NPtGain") {
    wantedDefects = m_NPGDefects.findModule(moduleId);
  } else if (scan == "NoiseOccupancy") {
    wantedDefects = m_NODefects.findModule(moduleId);
  } else {
    ATH_MSG_ERROR("defectsSummary(): Module defects for scan" << scan << " does not exist (only NPtGain or NoiseOccupancy).");
  }

  return wantedDefects;
} //SCT_ReadCalibDataSvc::defectsSummary()

//---------------------------------------------------------------------- 
//----------------------------------------------------------------------
// Returns a list of all strips with a certain defects
std::list<Identifier> SCT_ReadCalibDataSvc::defectList(const std::string& defect) {
  std::list<Identifier> defectList;
  // Create pointer to the CalibDataDefect object 
  SCT_CalibDefectData::CalibModuleDefects wantedDefects;
  
  //Check which scan the defect belongs
  bool npDefect{false};
  bool noDefect{false};
  if (defect=="NO_HI" or defect=="BAD_OPE" or defect=="DOUBTR_HI") {
    noDefect = true;
  } else {
    npDefect = true;
  }
  
  //Loop over all wafers using hashIds from the cabling service
  std::vector<boost::uint32_t> listOfRODs;
  m_cabling->getAllRods(listOfRODs);
  std::vector<boost::uint32_t>::iterator rodIter{listOfRODs.begin()};
  std::vector<boost::uint32_t>::iterator rodEnd{listOfRODs.end()};
  for (; rodIter!=rodEnd; ++rodIter) {
    std::vector<IdentifierHash> listOfHashes;
    m_cabling->getHashesForRod(listOfHashes, *rodIter);
    std::vector<IdentifierHash>::iterator hashIt{listOfHashes.begin()};
    std::vector<IdentifierHash>::iterator hashEnd{listOfHashes.end()};
    for (; hashIt != hashEnd; ++hashIt) {
      Identifier waferId{m_id_sct->wafer_id(*hashIt)};
      int side{m_id_sct->side(waferId)};
      //Only use the hashid for side 0, since data saved per module basis
      if (side!=0) continue;
      Identifier moduleId{m_id_sct->module_id(waferId)};
      if (npDefect) {
        wantedDefects = m_NPGDefects.findModule(moduleId);
      }  else if (noDefect) {
        wantedDefects = m_NODefects.findModule(moduleId);
      }
      if (!wantedDefects.begDefects.empty()) {
        for (unsigned int i{0}; i < wantedDefects.begDefects.size(); ++i) {
          if (wantedDefects.typeOfDefect[i] == defect) {
            // Create identifier for all strips inside begin to end
            int strip_beg{static_cast<int>(wantedDefects.begDefects[i])};
            int strip_end{static_cast<int>(wantedDefects.endDefects[i])};
            // In DB: strip from 0-1535, need to convert to side and 0-767 and take into account phiSwaps
            for (int strip{strip_beg}; strip<strip_end+1; strip++) {
              int nside{(strip<STRIPS_PER_WAFER) ? 0 : 1};
              int strip_cor;
              const InDetDD::SiDetectorElement* p_element;
              if (nside==1) { // if side 1 need waferId of side 1 to get phiswap and correct stripId
                IdentifierHash hashSide1;
                m_id_sct->get_other_side(*hashIt, hashSide1);
                waferId = m_id_sct->wafer_id(hashSide1);
                p_element = (m_SCTdetMgr->getDetectorElement(hashSide1));  
              } else {
                p_element = (m_SCTdetMgr->getDetectorElement(*hashIt));  
              }
              if (p_element->swapPhiReadoutDirection()) {
                if (nside == 0) {
                  strip_cor =   STRIPS_PER_WAFER-1 - strip;
                } else {
                  strip_cor = 2*STRIPS_PER_WAFER-1 - strip;
                }      
              } else {
                strip_cor = strip - nside * STRIPS_PER_WAFER;
              }
              Identifier stripId{m_id_sct->strip_id(waferId,strip_cor)};
              defectList.push_back(stripId);
            }
          }
        }
      }
    }
  }
  return defectList;
} //SCT_ReadCalibDataSvc::defects()
//---------------------------------------------------------------------- 
